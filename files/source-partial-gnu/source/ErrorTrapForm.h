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

#ifndef ErrorTrapFormH
#define ErrorTrapFormH

#include <SysUtils.hpp>
#include <Windows.hpp>
#include <Messages.hpp>
#include <Classes.hpp>
#include <Graphics.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <ExtCtrls.hpp>
#include <Forms.hpp>
#include <Buttons.hpp>

class TErrorTrapForm : public TForm
{
__published:
    TCheckBox	*CheckDisableNext;
    TLabel		*LabelErrorNumber;
	TGroupBox	*GroupBox1;
	TMemo		*MemoMessage;
	TButton		*BtnCancel;
	TButton		*BtnNo;
	TButton		*BtnOK;
	TButton		*BtnYes;
    TStaticText	*EditErrorNumber;

    void __fastcall BtnOKClick(TObject *Sender);
	void __fastcall MemoMessageKeyDown(TObject *Sender, WORD &Key, TShiftState Shift);

public:
    virtual __fastcall TErrorTrapForm(TComponent *Owner);
};

extern TErrorTrapForm    *ErrorTrapForm;

#endif
