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

#ifndef IniFileH
#define IniFileH

#include <IniFiles.hpp>


class VIniFile : public TIniFile
{
public:
	bool	ErrorIfReadFails;

	VIniFile(AnsiString FileName, bool _ErrorIfReadFails = false);

	virtual void __fastcall WriteString(AnsiString Section, AnsiString Ident, AnsiString Value);
    virtual void __fastcall WriteBool(AnsiString Section, AnsiString Ident, bool Value);
	virtual void __fastcall WriteInteger(AnsiString Section, AnsiString Ident, int Value);

	virtual AnsiString __fastcall ReadString(AnsiString Section, AnsiString Ident,
    	AnsiString Default);
    virtual bool __fastcall ReadBool(AnsiString Section, AnsiString Ident, bool Default);
	virtual int __fastcall ReadInteger(AnsiString Section, AnsiString Ident, int Default);

    bool IsFileValid();
};

#endif
