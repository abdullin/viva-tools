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

#ifndef ObjectBrowserH
#define ObjectBrowserH

#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <CheckLst.hpp>

#include "Globals.h"
#include <ComCtrls.hpp>

class TObjectBrowserForm : public TForm
{
__published:     
    TCheckListBox	*clbLibraries;
    TButton			*btnOK;
    TButton			*btnCancel;
    TLabel			*Label1;
    TLabel			*Label2;
    TLabel			*Label3;
    TListBox		*lbObjects;
    TListBox		*lbMembers;
    TMemo			*mmoLibName;
    TMemo			*mmoLibID;
    TMemo			*mmoTypeName;
    TMemo			*mmoProgID;
    TMemo			*mmoMemberDeclaration;
    
    void __fastcall clbLibrariesClick(TObject *Sender);
    void __fastcall btnOKClick(TObject *Sender);
    void __fastcall lbMembersClick(TObject *Sender);
    void __fastcall lbObjectsClick(TObject *Sender);
    void __fastcall FormShow(TObject *Sender);
    void __fastcall clbLibrariesClickCheck(TObject *Sender);
    void __fastcall lbObjectsKeyDown(TObject *Sender, WORD &Key, TShiftState Shift);
    void __fastcall lbMembersKeyDown(TObject *Sender, WORD &Key, TShiftState Shift);
    void __fastcall clbLibrariesKeyDown(TObject *Sender, WORD &Key, TShiftState Shift);

public:
    bool			Initialized; // Type Libraries have been enumerated and loaded
    
    __fastcall TObjectBrowserForm(TComponent *Owner);
    __fastcall~TObjectBrowserForm();
    
    void ClearlbObjects();
    void ClearlbProperties();
    void ClearlbLibraries();
};

extern PACKAGE    TObjectBrowserForm *ObjectBrowserForm;
#endif
