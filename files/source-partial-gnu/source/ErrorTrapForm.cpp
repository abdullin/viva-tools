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

#include "ErrorTrapForm.h"
#include "Globals.h"

#pragma resource "*.dfm"

TErrorTrapForm    *ErrorTrapForm;


__fastcall TErrorTrapForm::TErrorTrapForm(TComponent *Owner) :
    TForm(Owner)
{
}


void __fastcall TErrorTrapForm::BtnOKClick(TObject *Sender)
{
    Hide();
}


void __fastcall TErrorTrapForm::MemoMessageKeyDown(TObject *Sender,
      WORD &Key, TShiftState Shift)
{
	if (Shift.Contains(ssCtrl) && Key == 'A')
    	MemoMessage->SelectAll();
}

