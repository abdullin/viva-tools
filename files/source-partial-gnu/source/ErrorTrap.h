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

#ifndef ErrorTrapH
#define ErrorTrapH

#include "globals.h"
#include "StringList.h"
#include "MessageInfo.h"
#define ERROR_LOG_FILE_LIMIT 1000

extern int			MessageEntryCount;	// Count of messages presently loaded.
extern bool			ShowAllWarnings;	// Show all warning messages, regardless of which are
										//		disabled.

bool InitErrorMessages();

// Constructor has parameters which are all the class needs to know for the message about 85% of
//		the time.  The actual message is displayed on the destuctor of the class, so an
//		instance of the class having a one-line scope, which looks just like a function call,
//		(ex.: ErrorTrap(5001, "you are a bad Viva programmer");) will display the message at the
//		end of the execution of the line of code.  A broader-scope instance
//		(ex.: ErrorTrap		myerror(5002, "why can't you get yer variants straight?");)
//		will display the message at the end of the block in which it was declared.  This is so
//		that you can load parameters into the instance before the message is displayed.
//		Invoke() is the function that actually displays the message; calling in manually is OK.
class ErrorTrap
{
public:
	int					ErrorNumber;
	VStringList			MessageFragments;
    VMessageInfo		*MessageInfo;
    bool				IsMessageInfoInMessageGrid;
    int					IntegerValue;
    bool				AutoModal;
	AnsiString			YesButtonCaption;
    AnsiString			NoButtonCaption;
    AnsiString			OKButtonCaption;
    AnsiString			CancelButtonCaption;
    bool 				Echo;
	int					Result;
    bool				Invoked;

 	ErrorTrap(int _ErrorNumber, VStringList Messages = VStringList(""));

    ~ErrorTrap();

    operator int()
    {
    	Invoke();
        return Result;
    }

    void Invoke();
};


int  ErrorTrace(int ErrorNumber, I2adl *Object, VStringList Messages = VStringList(""),
	bool AutoModal = false);
bool TrapCheck(I2adl *Object, int CDex = 0);
bool UsageTrap(int MessageNr, I2adl *Object, I2adl *ParentObject, AnsiString Addendum = "");

enum MessageOptionsEnum
{
    moYesNo			= 0x1,
    moCancel		= 0x2,
    moModal			= 0x4,
    moWarning		= 0x8,
    moUserTrap		= 0x10,
    moJustInfo		= 0x20,
    moSearchResult	= 0x40,
    moOverload		= 0x80,
    moStatus		= 0x100,

    moDisabled		= 0x8000
};

#endif
