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

#include "COMFormDesigner.h"

#pragma package(smart_init)
#pragma resource "*.dfm"

#include "Globals.h"
#include "VivaCOMForm.h"
#include "ComManager.h"
#include "Main.h"
#include "ErrorTrap.h"
#include "Project.h"

TCOMDesigner    *COMDesigner;

__fastcall TCOMDesigner::TCOMDesigner(TComponent *Owner) : TForm(Owner)
{
    FormInEdit = NULL;
}


__fastcall TCOMDesigner::~TCOMDesigner()
{
}


bool TCOMDesigner::LoadFormIfOK(AnsiString FileName, bool DeleteOnFail)
{
    // We need to see if we should load this form.  If it is already loaded, then we
    //      need not do anything; if a different form with the same name is loaded,
    //      then issue an error to the user and don't load this one:
    AnsiString    FormName = GetShortFileName(FileName);

    for (int i = 0; i < ComForms->Count; i++)
    {
        TComForm    *ComForm = (TComForm *) ComForms->Items[i];

        if (FormName == ComForm->Name)
        {
            if (FileName == ComForm->FileName)
            {
                if (DeleteOnFail)
                    ErrorTrap(4035, FileName);
            }
            else
            {
                ErrorTrap(4034, FileName);
            }

            return false;
        }
    }

    TComForm    *NewComForm = new TComForm(NULL);

    if (!NewComForm->LoadFromFile(FileName) && DeleteOnFail)
    {
        delete NewComForm;
        return false;
    }

    ComForms->Add(NewComForm);

#if VIVA_IDE
    COMDesigner->BuildFormTree();
    COMDesigner->BuildClassTree();
#endif

    return true;
}


void TCOMDesigner::BuildFormTree(bool BuildMainFormTree)
{
    ClearTreeView(FormListTree);

    for (int i = 0; i < ComForms->Count; i++)
    {
        TComForm    *ComForm = (TComForm *) ComForms->Items[i];

        FormListTree->Items->AddObject(NULL, ComForm->Name, ComForm);
    }

    if (FormListTree->Items->Count > 0)
        FormListTree->Selected = FormListTree->Items->Item[FormListTree->Items->Count - 1];

    if (BuildMainFormTree && MainForm->ObjectTreePageControl->ActivePage == MainForm->ComTabSheet)
        BuildComObjectTree();
}


void TCOMDesigner::BuildClassTree(bool BuildMainFormTree)
{
    ClearTreeView(ObjectListTree);

    ObjectListTree->Visible = false;

    for (int i = 0; i < ComLibraries->Count; i++)
    {
        TLibInfo    *LibInfo = (TLibInfo *) ComLibraries->Objects[i];

        if (LibInfo == NULL)
            continue;

        ITypeLib    *TypeLib = LibInfo->TypeLib;

        if (TypeLib == NULL)
            continue;

        TTreeNode      *CurrRootNode = ObjectListTree->Items->AddObject(NULL, LibInfo->LibID,
            LibInfo);
        TStringList    *ObjectList = LibInfo->ObjectList;

        for (int j = 0; j < ObjectList->Count; j++)
        {
            TComObjectInfo    *ObjectInfo = (TComObjectInfo *) ObjectList->Objects[j];

            if (ObjectInfo->IsControl)
                ObjectListTree->Items->AddChildObject(CurrRootNode, ObjectList->Strings[j],
                    ObjectInfo);
        }
    }

    ObjectListTree->Visible = true;

    if (BuildMainFormTree && MainForm->ObjectTreePageControl->ActivePage == MainForm->ComTabSheet)
        BuildComClassTree();
}


void TCOMDesigner::ShowFormInEdit()
{
    if (FormInEdit != NULL)
        FormInEdit->Visible = true;
}


void __fastcall TCOMDesigner::EventHandlerForTVivaOleControl(tagMSG &Msg, bool &Handled)
{
    if (FormInEdit == NULL || FormInEdit->Visible == false || FormInEdit->IsDesigning == false)
        return; // This function should be performed only at design-time.

    if (FormInEdit->DeleteSelected) // We were earlier notifed to delete the active control.
    {
        FormInEdit->DeleteSelected = false;
        delete FormInEdit->SelectedControl;
        FormInEdit->SelectedControl = NULL;
		FormInEdit->Modified = true;

        if (MainForm->ObjectTreePageControl->ActivePage == MainForm->ComTabSheet)
            BuildComObjectTree();
    }

    TWinControl		*WinCon = FindControl(Msg.hwnd);
    VOleControl		*FoundCon = NULL;

    if (Mouse->Capture != NULL)
        return;

    if (WinCon != NULL)
    {
        if (WinCon->Parent != FormInEdit)
            return;
    }
    else
    {
        if (FormInEdit->SelectedControl &&
            (Msg.message == WM_KEYDOWN ||
             Msg.message == WM_KEYUP)   )
        {
            FoundCon = FormInEdit->SelectedControl;
        }
        else
        {
            int Message = Msg.message;
            if (Message == WM_LBUTTONDOWN       || Message == WM_LBUTTONUP
                || Message == WM_LBUTTONDBLCLK  || Message == WM_RBUTTONDOWN
                || Message == WM_RBUTTONUP      || Message == WM_RBUTTONDBLCLK
                || Message == WM_MBUTTONDOWN    || Message == WM_MBUTTONUP
                || Message == WM_MBUTTONDBLCLK  || Message == WM_MOUSEMOVE)
            {
                FoundCon = (VOleControl *) FindVCLWindow(Msg.pt);
            }
            else
            {
                for (int i = 0; i < FormInEdit->ControlCount; i++)
                {
                    VOleControl 	*Con = (VOleControl *) FormInEdit->Controls[i];

                    if (Msg.hwnd == Con->Handle || Con == WinCon)
                        FoundCon = Con;
                }
            }
        }
    }

    if (FoundCon != NULL)
    {
        TMessage Message;
        Message.Msg = Msg.message;
        Message.LParam = Msg.lParam;
        Message.WParam = Msg.wParam;

        FoundCon->WndProc(Message);
        Handled = true;
    }
}


void __fastcall TCOMDesigner::ObjectListTreeStartDrag(TObject *Sender, TDragObject *&DragObject)
{
    DragNode = ObjectListTree->Selected;
}


void __fastcall TCOMDesigner::FormShow(TObject *Sender)
{
    Application->OnMessage = EventHandlerForTVivaOleControl;
    DragNode = NULL;
    BuildClassTree(false);

    if (FormInEdit == NULL)
    {
        if (FormListTree->Items->Count > 0)
        {
            FormListTree->Selected = FormListTree->Items->GetFirstNode();
            FormListTreeChange(this, FormListTree->Selected);
        }
    }
    else
    {
        ShowFormInEdit();
    }
}


void __fastcall TCOMDesigner::cmdNewClick(TObject *Sender)
{
    TComForm    *NewComForm = new TComForm(NULL);

    ComForms->Add(NewComForm);
    BuildFormTree();
}


void __fastcall TCOMDesigner::cmdOpenClick(TObject *Sender)
{
    MainForm->OpenDialog->FileName = "*." + FilterStrings[ftComForm][1];
    MainForm->OpenFile(ftComForm);
}


void __fastcall TCOMDesigner::cmdSaveClick(TObject *Sender)
{
    if (FormInEdit != NULL)
        FormInEdit->SaveToFile();
}


void __fastcall TCOMDesigner::cmdSaveAsClick(TObject *Sender)
{
    if (FormInEdit != NULL)
        if (FormInEdit->SaveToFile(true))
        {
            BuildFormTree();
            MyProject->Modified();
        }
}

void __fastcall TCOMDesigner::FormActivate(TObject *Sender)
{
    if (ComLibraries == NULL)
        return;
    
    if (ComForms->Count != FormListTree->Items->Count)
        BuildFormTree(false);
}


void __fastcall TCOMDesigner::FormClose(TObject *Sender, TCloseAction &Action)
{
    Application->OnMessage = NULL;

    if (FormInEdit != NULL)
        FormInEdit->Visible = false;
    
    ClearTreeView(ObjectListTree);
    
    if (MainForm->ObjectTreePageControl->ActivePage == MainForm->ComTabSheet)
        BuildComObjectTree();
}


void __fastcall TCOMDesigner::FormListTreeKeyDown(TObject *Sender, WORD &Key, TShiftState Shift)
{
    if (Key == VK_DELETE)
    {
        if (FormListTree->Selected == NULL)
            return;

        TComForm    *ComForm = (TComForm *) FormListTree->Selected->Data;

        if (!IsDependency(TComResource(ComForm)) && ErrorTrap(4024, ComForm->Name) == mrYes)
        {
            if (ComForm == FormInEdit)
            {
                FormInEdit->Visible = false;
                FormInEdit = NULL;
            }

            ComForms->Remove(ComForm);
            BuildFormTree();
            delete ComForm;
        }
    }
}


void __fastcall TCOMDesigner::FormListTreeClick(TObject *Sender)
{
    if (FormListTree->Selected != NULL && FormListTree->Selected->Data== FormInEdit)
    {
        FormInEdit->BringToFront();
        SetFocus();
    }
}

void __fastcall TCOMDesigner::FormListTreeChange(TObject *Sender, TTreeNode *Node)
{
    if (Visible == false)
        return;

    if (FormInEdit != NULL)
        FormInEdit->Visible = false;

    if (FormListTree->Selected != NULL)
    {
        FormInEdit = (TComForm *) FormListTree->Selected->Data;
        ShowFormInEdit();
    }
}


// Renames the corresponding form and its caption.
void __fastcall TCOMDesigner::FormListTreeEdited(TObject *Sender, TTreeNode *Node, AnsiString &S)
{
    // This will throw an exception if S is invalid; the Catch block is outside this function:
    FormInEdit->Name = S;
    FormInEdit->Caption = S;

    if (MainForm->ObjectTreePageControl->ActivePage == MainForm->ComTabSheet)
        BuildComObjectTree();
}


void __fastcall TCOMDesigner::ObjectListTreeKeyDown(TObject *Sender, WORD &Key
, TShiftState Shift)
{
    if (Key == VK_F1)
    {
        TTreeNode *Selected = ObjectListTree->Selected;

        if (Selected == NULL)
            return;

        if (Selected->Level == 0)
            ShowComHelp((TLibInfo *) Selected->Data, Handle);
        else if (Selected->Level == 1)
            ShowComHelp((TComObjectInfo *) Selected->Data, Handle);
    }
}

