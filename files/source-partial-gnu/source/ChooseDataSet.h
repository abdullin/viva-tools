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

#ifndef TChooseDataSetH
#define TChooseDataSetH

#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>


class TChooseDataSetForm : public TForm
{
__published:
    TLabel		*Label1;
	TLabel		*Label2;
	TEdit		*DSName;
	TEdit		*InputName;
	TEdit		*DSEditBox;
	TListBox	*DSListBox;
	TCheckBox	*ApplyDSToRest;
	TButton		*OkButton;
	TButton		*CancelButton;

	void __fastcall DSEditBoxChange(TObject *Sender);
	void __fastcall DSEditBoxKeyDown(TObject *Sender, WORD &Key, TShiftState Shift);
	void __fastcall DSEditBoxKeyUp(TObject *Sender, WORD &Key, TShiftState Shift);
	void __fastcall DSEditBoxMouseUp(TObject *Sender, TMouseButton Button, TShiftState Shift,
    	int X, int Y);
	void __fastcall DSListBoxClick(TObject *Sender);
    void __fastcall DSListBoxDblClick(TObject *Sender);
	void __fastcall FormClose(TObject *Sender, TCloseAction &Action);

public:
	I2adl		*DynamicInput;			// The input object that is having its dynamic data
    									//		set replaced.
	bool		AutoCompleteDSName;		// Records that the user is deleting text so the data
    									//		set name will not be automatically completed.
    AnsiString	LastEditBoxText1;		// Records the last two edit box texts so CNTL+Z can
    AnsiString	LastEditBoxText2;		//		undo the last change.

    __fastcall TChooseDataSetForm(TComponent *Owner);

    int			LinesInListBox();
    AnsiString	GetListBoxDataSet();
    bool		IsCSDelimiter(bool &IsConstant, AnsiString Text, int Location, int Direction);
    AnsiString	TextUnderCursor(bool &IsConstant, AnsiString NewText = strNull, bool
    	SelectNewSuffix = false);
    void		LoadEditBox(int ListBoxIndex);
    AnsiString	LoadListBox(AnsiString DataSetPrefix);
    AnsiString	ChooseDataSet(I2adl *InputObject, int RemainingInputs);
    bool		ParseCompositeDataSet(AnsiString CompositeDataSet, I2adl *InputObject, bool
    	CreateObjects);
	bool		ParseDataSetDef(DataSet *SubjectDS, AnsiString &DataSetDef, I2adl *InputObject,
    	bool CreateObjects);
};

extern PACKAGE TChooseDataSetForm *ChooseDataSetForm;

#endif
