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

#ifndef CompileVivaFormH
#define CompileVivaFormH

#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>

#define		GLOBAL_HEADER_FILE_NAME		"Globals.h"


class TCompileForm : public TForm
{
__published:
	TLabel *Label1;
	TButton *Button1;
	TLabel *Label2;
	TMemo *Memo1;
	void __fastcall Button1Click(TObject *Sender);

public:
    TStringList		*GlobalsH;			// Entire Globals.h source code file.
    TStringList		*ProgramNames;		// Viva program names extracted from VERSION #defines.
    									//		The GlobalsH line number is placed in Objects.

	__fastcall TCompileForm(TComponent* Owner);
    __fastcall ~TCompileForm();
};

extern PACKAGE TCompileForm *CompileForm;

#endif
