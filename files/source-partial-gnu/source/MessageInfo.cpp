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

#include "I2adl.h"
#include "MessageInfo.h"


// Default constructor
VMessageInfo::VMessageInfo()
{
    MemberOf = NULL;
	MessageColor = clBlack;
	I2adlObject = NULL;
    SearchI2adl = NULL;
    SearchString = strNull;
	SearchCriteria = scOverload;
    Index = -1;
    IsDef = false;
}


// Main constructor
VMessageInfo::VMessageInfo(I2adl *_I2adlObject, I2adl *_SearchI2adl, SearchCriteriaEnum
	_SearchCriteria, int _Index, AnsiString _SearchString)
{
    I2adlObject = new I2adl(_I2adlObject, cnYesDisconnect);
    I2adlObject->SearchListIndex = -1;

    if (_SearchI2adl == NULL)
    {
    	SearchI2adl = NULL;
    }
    else
    {
		SearchI2adl = new I2adl(_SearchI2adl, cnYesDisconnect);
	    SearchI2adl->SearchListIndex = -1;
    }

    MemberOf = I2adlObject->MemberOf;
	MessageColor = clBlack;
    SearchString = _SearchString;
    SearchCriteria = _SearchCriteria;
    Index = _Index;
    IsDef = false;
}


VMessageInfo::~VMessageInfo()
{
	if (!IsDef)
	    delete I2adlObject;

    delete SearchI2adl;
}


#pragma package(smart_init)
