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

#include "ErrorTrap.h"
#include "IniFile.h"

#pragma package(smart_init)


// Main constructor
VIniFile::VIniFile(AnsiString FileName, bool _ErrorIfReadFails)
	: TIniFile(FileName)
{
    ErrorIfReadFails = _ErrorIfReadFails;
}


// Write a value into the Viva.ini file.
void __fastcall VIniFile::WriteString(AnsiString Section, AnsiString Ident, AnsiString Value)
{
    if (IsFileValid())
		TIniFile::WriteString(Section, Ident, Value);
}


void __fastcall VIniFile::WriteBool(AnsiString Section, AnsiString Ident, bool Value)
{
    if (IsFileValid())
		TIniFile::WriteBool(Section, Ident, Value);
}


void __fastcall VIniFile::WriteInteger(AnsiString Section, AnsiString Ident, int Value)
{
    if (IsFileValid())
		TIniFile::WriteInteger(Section, Ident, Value);
}


// Read a value from the Viva.ini file.
AnsiString __fastcall VIniFile::ReadString(AnsiString Section, AnsiString Ident, AnsiString
	Default)
{
    IsFileValid();

	return TIniFile::ReadString(Section, Ident, Default);
}


bool __fastcall VIniFile::ReadBool(AnsiString Section, AnsiString Ident, bool Default)
{
    IsFileValid();

	return TIniFile::ReadBool(Section, Ident, Default);
}


int __fastcall VIniFile::ReadInteger(AnsiString Section, AnsiString Ident, int Default)
{
    IsFileValid();

	return TIniFile::ReadInteger(Section, Ident, Default);
}


// Returns true if the Viva.ini file is avaliable.
bool VIniFile::IsFileValid()
{
	if (ErrorIfReadFails)
    {
        int		FileHandle = FileOpen(FileName, fmOpenRead | fmShareDenyNone);

        if (FileHandle == -1)
        {
        	// Inform the user that the file can't be opened.
            if (FileExists(FileName))
                ErrorTrap(4075, FileName);
            else
                ErrorTrap(4076, FileName);

            return false;
        }
        else
        {
            FileClose(FileHandle);

            return true;
        }
    }

    return true;
}
