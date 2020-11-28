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

#ifndef COMFormDesignerH
#define COMFormDesignerH
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ComCtrls.hpp>
#include <ExtCtrls.hpp>
#include <Menus.hpp>

#include "VivaComForm.h"

class TCOMDesigner : public TForm
{
__published:
    
    TPanel       *Panel1;
    TTreeView    *ObjectListTree;
    TTreeView    *FormListTree;
    TSplitter    *Splitter1;
    TMainMenu    *MainMenu1;
    TMenuItem    *mnuFile;
    TMenuItem    *N1;
    TMenuItem    *cmdSaveAs;
    TMenuItem    *cmdSave;
    TMenuItem    *cmdOpen;
    TMenuItem    *cmdNew;

    void __fastcall ObjectListTreeStartDrag(TObject *Sender, TDragObject *&DragObject);
    void __fastcall FormShow(TObject *Sender);
    void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
    void __fastcall cmdOpenClick(TObject *Sender);
    void __fastcall cmdNewClick(TObject *Sender);
    void __fastcall cmdSaveClick(TObject *Sender);
    void __fastcall cmdSaveAsClick(TObject *Sender);
    void __fastcall FormActivate(TObject *Sender);
    void __fastcall FormListTreeKeyDown(TObject *Sender, WORD &Key, TShiftState Shift);
    void __fastcall FormListTreeClick(TObject *Sender);
    void __fastcall FormListTreeChange(TObject *Sender, TTreeNode *Node);
    void __fastcall FormListTreeEdited(TObject *Sender, TTreeNode *Node, AnsiString &S);
    void __fastcall ObjectListTreeKeyDown(TObject *Sender, WORD &Key, TShiftState Shift);
private:
public:
    
    TTreeNode    *DragNode;			// Node being drug from com form designer.
    TComForm     *FormInEdit;       // Design-time com form is having its controls edited.

    __fastcall TCOMDesigner(TComponent *Owner);
    __fastcall~TCOMDesigner();

    static bool LoadFormIfOK(AnsiString FileName, bool DeleteOnFail = true);
    void BuildFormTree(bool BuildMainFormTree = true);
    void BuildClassTree(bool BuildMainFormTree = true);
    void ShowFormInEdit();
    void __fastcall EventHandlerForTVivaOleControl(tagMSG &Msg, bool &Handled);
};

extern PACKAGE    TCOMDesigner *COMDesigner;
#endif
