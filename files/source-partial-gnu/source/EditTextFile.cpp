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

#include "EditTextFile.h"
#include "ErrorTrap.h"

#pragma package(smart_init)
#pragma resource "*.dfm"


TEditTextFileForm *EditTextFileForm;


// Constructor.
__fastcall TEditTextFileForm::TEditTextFileForm(TComponent* Owner)
	: TForm(Owner)
{
}


// Display the text file and allow it to be edited.
//		Calling routine must pass the full file name in the FileName parameter and read the
//		entire file into the FileContent parameter.  The file will be displayed in the memo
//		memo object with the LineToSelect (zero based) selected and located at the top of
//		the memo.
void TEditTextFileForm::EditTextFile(AnsiString FullFileName, TStringList *FileContent,
	int LineToSelect)
{
	FileNameEdit->Text = FullFileName;
	FileContentMemo->Text = FileContent->Text;

    if (LineToSelect >= 0 &&
    	LineToSelect < FileContent->Count)
    {
    	// Count how many characters are before the LineToSelect.
        int		PreviousChars = LineToSelect * 2;  // Count CR/LF

        for (int i = 0; i < LineToSelect - 1; i++)
        	PreviousChars += FileContent->Strings[i].Length();

        // Selecting the very bottom of the file first will position the LineToSelect at the
        //		top of the memo (instead of at the bottom).
	    Visible = true;

        FileContentMemo->SelStart = FileContent->Text.Length() - 1;
        FileContentMemo->SelLength = 1;

        FileContentMemo->SelStart = PreviousChars;
        FileContentMemo->SelLength = FileContent->Strings[LineToSelect].Length();

	    Visible = false;
    }

    // Display the file and allow the user to edit/save it.
	ShowModal();
}


// Discard the changes.
void __fastcall TEditTextFileForm::CancelButtonClick(TObject *Sender)
{
	Close();
}


// Save the changes back into the original file.
void __fastcall TEditTextFileForm::SaveChangesButtonClick(TObject *Sender)
{
	// Save the changes.
    try
    {
	    FileContentMemo->Lines->SaveToFile(FileNameEdit->Text);
    }
    catch (...)
    {
        ErrorTrap(4051, FileNameEdit->Text).AutoModal = true;
    }

	Close();
}
