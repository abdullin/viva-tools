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

#ifndef VDismemberH
#define VDismemberH


class VDismember
{
public:
    VDismember		*MemberOf;		// Pointer to the parent object's information
    I2adl			*SourceI2adl;   // Pointer to associated I2adl in original behavior.
    I2adl			*TopLevelSheet;	// Value of CompilingSheet at the time the hierarchy was
    								//		was recorded.
    AnsiString		*MemberName;    // Pointer to the object's name in the I2adlStringTable
    SystemString	*Documentation; // Value portion of the object's Documentation attribute
    int 			XLoc;			// Object's X location on the sheet
    int				YLoc;			// Object's Y location on the sheet

    VDismember();
    bool Equals(VDismember *Dismember);
    void GetValidObjects(I2adl **ParentObject, I2adl **ChildObject);
    AnsiString QualifiedName();
};

#endif
