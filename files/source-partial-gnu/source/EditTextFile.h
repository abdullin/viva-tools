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


#ifndef EditTextFileH
#define EditTextFileH

#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>


class TEditTextFileForm : public TForm
{
__published:
	TButton *SaveChangesButton;
	TButton *CancelButton;
	TMemo *FileContentMemo;
	TLabel *FileNameLabel;
	TEdit *FileNameEdit;

	void __fastcall CancelButtonClick(TObject *Sender);
	void __fastcall SaveChangesButtonClick(TObject *Sender);

public:
	__fastcall TEditTextFileForm(TComponent* Owner);

    void EditTextFile(AnsiString FileName, TStringList *FileContent, int LineToSelect);
};

extern PACKAGE TEditTextFileForm *EditTextFileForm;

#endif
