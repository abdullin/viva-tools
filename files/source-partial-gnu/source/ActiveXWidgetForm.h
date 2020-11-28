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

#ifndef ActiveXWidgetFormH
#define ActiveXWidgetFormH

#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>


class TActiveXWidgetSelector : public TForm
{
__published:
    TListBox	*lstProgID;
    TListBox	*lstProperty;
    TListBox	*lstEvent;
    TLabel		*lblProgID;
    TLabel		*lblProperty;
    TLabel		*lblEvent;
    TButton		*btnOK;
    TButton		*btnCancel;

    void __fastcall lstProgIDClick(TObject *Sender);
    void __fastcall FormShow(TObject *Sender);
    void __fastcall btnOKClick(TObject *Sender);

public:
    I2adl	*ObjectBeingEdited;	// The input/output object having its ActiveX attributes edited.

    __fastcall TActiveXWidgetSelector(TComponent* Owner);
};

extern PACKAGE TActiveXWidgetSelector *ActiveXWidgetSelector;

#endif
