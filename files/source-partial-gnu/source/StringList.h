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

#ifndef VStringListH
#define VStringListH


class VStringList
{
protected:
    TStringList		*StringList;	// The list of strings that this class is wrapping up.

public:
    VStringList(AnsiString String1 = strNull, AnsiString String2 = strNull, AnsiString
    	String3 = strNull, AnsiString String4 = strNull, AnsiString String5 = strNull);
    VStringList(VStringList &CopyFrom);
	VStringList(char *CharString);
	~VStringList();

    operator TStringList*();
    TStringList *operator->();
    VStringList &operator=(VStringList &Params);
};

#endif
