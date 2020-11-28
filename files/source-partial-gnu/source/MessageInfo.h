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

#ifndef MessageInfoH
#define MessageInfoH

#include "Globals.h"


// Data associated with an error/warning/info message, that links it to an I2adl object.  This
//		link is abstract, because specific I2adl instances can disappear.
class VMessageInfo
{
public:
    VDismember			*MemberOf;			// Hierarchy tracking information
	TColor				MessageColor;		// Color to display this type of message
	I2adl				*I2adlObject;
    I2adl				*SearchI2adl;
    AnsiString          SearchString;		// Search string that generated this message
    SearchCriteriaEnum	SearchCriteria;		// Type of search that generated this message
    int					Index;
    bool				IsDef;				// Indicates if I2adlObject is a master project object

	VMessageInfo();
	VMessageInfo(I2adl *_I2adlObject, I2adl *_SearchI2adl, SearchCriteriaEnum _SearchCriteria,
    	int _Index, AnsiString _SearchString = strNull);
    ~VMessageInfo();
};

#endif
