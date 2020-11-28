/* Viva,  heterogeneous architecture compiler tool

    Copyright (C) 2019  Kent Gilson



    This program is free software: you can redistribute it and/or modify

    it under the terms of the GNU General Public License as published by

    the Free Software Foundation, either version 3 of the License, or

    (at your option) any later version.



    This program is distributed in the hope that it will be useful,

    but WITHOUT ANY WARRANTY; without even the implied warranty of

    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the

    GNU General Public License for more details.



    You should have received a copy of the GNU General Public License

    along with this program.  If not, see <https://www.gnu.org/licenses/>.

*/

#include <vcl.h>
#pragma hdrstop

#include "CompileVivaForm.h"

#pragma package(smart_init)
#pragma resource "*.dfm"

TCompileForm *CompileForm;


// TCompileForm constructor.
//		Reads Globals.h to extract the list of Viva program names from the VERSION #defines.
__fastcall TCompileForm::TCompileForm(TComponent* Owner)
	: TForm(Owner)
{
	bool	VivaExeCount = 0;

    GlobalsH = new TStringList;
    ProgramNames = new TStringList;

    GlobalsH->LoadFromFile(GLOBAL_HEADER_FILE_NAME);

    for (int i = 0; i < GlobalsH->Count; i++)
    {
    	AnsiString	SourceCodeLine = GlobalsH->Strings[i];
        int			DefinePos = SourceCodeLine.Pos("#define");
        int			VersionPos = SourceCodeLine.Pos("VERSION_");

        if (DefinePos > 0 &&
        	DefinePos <= 3 &&
        	VersionPos > 8)
        {
        	// Extract the Viva program name from the VERSION #define statement.
            AnsiString	ProgramName = "";
            int			VivaPos = SourceCodeLine.Pos("VIVA_");
            int			StrPos = SourceCodeLine.Pos("str");

            if (VivaPos > 0)
	            ProgramName = SourceCodeLine.SubString(VivaPos + 5, SourceCodeLine.Length() - VivaPos - 4);
            else if (StrPos > 0)
            	ProgramName = SourceCodeLine.SubString(StrPos + 3, SourceCodeLine.Length() - StrPos - 2);
            else
            	VivaExeCount++;

            ProgramName = "Viva" + ProgramName + ".exe";

        	// Put the program name and the line number into ProgramNames.
        	ProgramNames->AddObject(ProgramName, (TObject *) i);
        }
    }

    // If there is not exactly one Viva.exe, then something is wrong.
    if (VivaExeCount != 1)
    {
        AnsiString	Message = "    Warning!  Parsing " GLOBAL_HEADER_FILE_NAME " found " +
        	IntToStr(VivaExeCount) + " Viva.exe programs.    ";

        Application->MessageBox(Message.c_str(), "Parse " GLOBAL_HEADER_FILE_NAME, MB_OK);
    }

    // Display the list of program names on the main form.
	Memo1->Text = ProgramNames->Text;
}


// TCompileForm destructor.
__fastcall TCompileForm::~TCompileForm()
{
	delete GlobalsH;
    delete ProgramNames;
}


// When the button is pushed, compile all of the Viva programs.
void __fastcall TCompileForm::Button1Click(TObject *Sender)
{
    // Make sure all VERSION #defines start out as comments.
    for (int i = 0; i < ProgramNames->Count; i++)
    {
        int			LineNumber = (int) ProgramNames->Objects[i];
        AnsiString  SourceCodeLine = GlobalsH->Strings[LineNumber];

        if (SourceCodeLine.Pos("#define") < 3)
            GlobalsH->Strings[LineNumber] = "//" + SourceCodeLine;
    }

    // Wait until the 2nd pass to compile Viva.exe so (1) Globals.h will be left in the default
    //		state and (2) Viva.exe will not be deleted by another compile.
	bool	CompileProblems = false;

    for (int i = 0; i < 2; i++)
    {
        for (int j = 0; j < ProgramNames->Count; j++)
        {
            int			LineNumber = (int) ProgramNames->Objects[j];
            AnsiString	ProgramName = ProgramNames->Strings[j];
            AnsiString  SourceCodeLine = GlobalsH->Strings[LineNumber];

            if (i <= 0)
            {
            	if (ProgramName == "Viva.exe")
                	continue;
            }
            else
            {
            	if (ProgramName != "Viva.exe")
                	continue;
            }

            // Uncomment out this VERSION #define so it will be compiled.
            GlobalsH->Strings[LineNumber] = SourceCodeLine.SubString(3, SourceCodeLine.Length() - 2);

            try
            {
                GlobalsH->SaveToFile(GLOBAL_HEADER_FILE_NAME);
            }
            catch(...)
            {
                // Can't update Globals.h.  Is it open or read only?
                AnsiString	Message = "    Can't update source code file "
                	GLOBAL_HEADER_FILE_NAME ".  Is it open or read only?    ";

                Application->MessageBox(Message.c_str(), "Compile problem", MB_OK);

                return;
            }

            // Delete the files being rebuilt.
            DeleteFile("Viva.exe");
            DeleteFile(ProgramName);

            if (FileExists("Viva.exe"))
            {
                AnsiString	Message = "    Can't delete program file Viva.exe.  Is it open or read only?    ";

                Application->MessageBox(Message.c_str(), "Compile problem", MB_OK);

                return;
            }

            if (FileExists(ProgramName))
            {
                CompileProblems = true;

                AnsiString	Message = "    Can't delete program file " + ProgramName + ".  Is it open or read only?    ";

                Application->MessageBox(Message.c_str(), "Compile problem", MB_OK);
            }
            else
            {
                // Run the "Make" utility to recompile this version of Viva.
                PROCESS_INFORMATION		procinfo;
                STARTUPINFO				lpStartUpInfo;

                memset(&lpStartUpInfo, 0, sizeof(STARTUPINFO));
                lpStartUpInfo.wShowWindow = SW_SHOWNORMAL;
                lpStartUpInfo.cb = sizeof(STARTUPINFO);

                if (CreateProcess(NULL, "Make -fViva.mak", 0, 0, 0, 0, 0, 0, &lpStartUpInfo, &procinfo))
                {
                    WaitForSingleObject(procinfo.hProcess, INFINITE);

                    CloseHandle(procinfo.hProcess);
                    CloseHandle(procinfo.hThread);

                    // Rename Viva.exe.
                    RenameFile("Viva.exe", ProgramName);
                }

                // Did the compile work?
                if (!FileExists(ProgramName))
                {
	                CompileProblems = true;

                    AnsiString	Message = "    Program " + ProgramName + " failed to compile!    ";

                    Application->MessageBox(Message.c_str(), "Compile problem", MB_OK);
                }
            }

            // Change the VERSION #define back into a comment.
            GlobalsH->Strings[LineNumber] = SourceCodeLine;
        }
    }

    // Done compiling.  Were there any problems.
	if (!CompileProblems)
	    Application->MessageBox("    All programs compiled successfully.    ", "Compile results", MB_OK);
}

