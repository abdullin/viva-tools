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

#include "ObjectBrowser.h"
#include "Globals.h"
#include "ComFormDesigner.h"
#include "Main.h"
#include "ErrorTrap.h"

#include "Registry.hpp"
#include "ObjBase.h"

#pragma package(smart_init)
#pragma resource "*.dfm"
#include "ComManager.h"

TObjectBrowserForm    *ObjectBrowserForm;


__fastcall TObjectBrowserForm::TObjectBrowserForm(TComponent *Owner) :
    TForm(Owner)
{
    Initialized = false;
}


__fastcall TObjectBrowserForm::~TObjectBrowserForm()
{
    // This is necessary because clbLibraries owns data which must now be deleted:
    for (int i = 0; i < clbLibraries->Items->Count; i++)
        delete (TLibInfo *) clbLibraries->Items->Objects[i];
}


// Displays all of the types in the Type Library corresponding to the item clicked.  Also
//      Displays Library ID and FileName.
void __fastcall TObjectBrowserForm::clbLibrariesClick(TObject *Sender)
{
    if (clbLibraries->ItemIndex == -1) // Click event can occur without a valid selection.
        return;

    TLibInfo    *LibInfo = (TLibInfo *) clbLibraries->Items->Objects[clbLibraries->ItemIndex];

    mmoLibName->Text = LibInfo->FileName;
    mmoLibID->Text = "Library ID: " + LibInfo->LibID;

    if (LibInfo->ObjectList->Count <= 0) // Indicates that it has not yet been populated.
        LibInfo->GetObjectList();

    // Just some cleanup, to avoid confusion:
    lbMembers->Items->Clear();
    mmoMemberDeclaration->Text = strNull;
    mmoTypeName->Text = strNull;
    mmoProgID->Text = strNull;

    lbObjects->Items->Assign(LibInfo->ObjectList);
}


void __fastcall TObjectBrowserForm::clbLibrariesClickCheck(TObject *Sender)
{
    // Do not allow the user to un-include libraries which are depended upon.
    if (!clbLibraries->Checked[clbLibraries->ItemIndex])
    {
        TLibInfo   *LibInfo = (TLibInfo *) clbLibraries->Items->Objects[clbLibraries->ItemIndex];

        //  Both of these testing functions prompt user with first found dependent:
        if (LibIsDependency(LibInfo, true) || IsDependency(TComResource(LibInfo)))
        {
            clbLibraries->Checked[clbLibraries->ItemIndex] = true; // Sorry, can't un-check.
        }
        else
        {
            // Ok to remove--need to find any existent entry in ComLibraries and remove it;
            //      must search by name:
            for (int i = 0; i < ComLibraries->Count; i++)
            {
                TLibInfo    *CurrLibInfo = (TLibInfo *) ComLibraries->Objects[i];

                if (CurrLibInfo->FileName == LibInfo->FileName)
                {
                    delete CurrLibInfo;
                    ComLibraries->Delete(i);
                    break;
                }
            }
        }
    }
    else
    {
        // Also, we can't allow a checklistbox entry whose corresponding library could not be
        //      loaded, to be checked:
        if (((TLibInfo *) clbLibraries->Items->Objects[clbLibraries->ItemIndex])->TypeLib ==
            NULL)
                clbLibraries->Checked[clbLibraries->ItemIndex] = false;
    }
}


// Displays the members of Com type corresponding to this entry, in lbProperties, and sets the
//      caption of the appropriate memos on this form:
void __fastcall TObjectBrowserForm::lbObjectsClick(TObject *Sender)
{
    if (lbObjects->ItemIndex == -1)
        return;

    TComObjectInfo    *ObjectInfo = (TComObjectInfo *) lbObjects->Items->Objects[lbObjects->
        ItemIndex];

    if (ObjectInfo == NULL)
        return;

    mmoMemberDeclaration->Text = strNull;

    lbMembers->Items->Assign(ObjectInfo->MemberList);

    ITypeInfo    *TypeInfo = ObjectInfo->TypeInfo;

    if (TypeInfo == NULL)
        return;

    TYPEATTR    *TypeAttr = ObjectInfo->TypeAttr;
    AnsiString  TKindName = ObjectTypeNames[TypeAttr->typekind];
    
    if (TypeAttr->typekind == TKIND_ALIAS)
    {
        TKindName = "(" +TKindName + " for " + NameFromTypeDesc(&TypeAttr->tdescAlias, TypeInfo)
        + ")";
    }
    
    mmoTypeName->Text = TKindName + "  " + ObjectInfo->ClassName;
    
    if (ObjectInfo->ProgID != NULL)
        mmoProgID->Text = "Programmatic ID: " +AnsiString(ObjectInfo->ProgID);
    else
        mmoProgID->Text = "";
}


// Sets the caption of mmoMemberDeclarion (declaration for a Function, Property, or Constant):
void __fastcall TObjectBrowserForm::lbMembersClick(TObject *Sender)
{
    if (lbMembers->ItemIndex == -1)
        return;

    TMemberDesc   *MemberDesc = (TMemberDesc *) lbMembers->Items->Objects[lbMembers->ItemIndex];
    unsigned int  NameCount;
    WideString Names[MaxNameCount];

    if (MemberDesc->Type & mtFunction)
    {
        FUNCDESC    *FuncDesc = MemberDesc->FuncDesc;

        MemberDesc->OwningTypeInfo->GetNames(FuncDesc->memid, (BSTR *) Names
        , MaxNameCount, &NameCount);

        AnsiString    FuncName;

        if (MemberDesc->Type == mtEvent)
            FuncName = "Event ";

        if (FuncDesc->invkind == INVOKE_PROPERTYGET)
        {
            FuncName += "(Get) ";
        }
        else if (FuncDesc->invkind == INVOKE_PROPERTYPUT)
        {
            FuncName += "(Set) ";
            Names[NameCount++] = "Val";
        }
        else if (FuncDesc->invkind == INVOKE_PROPERTYPUTREF)
        {
            FuncName += "(Set) ";
            Names[NameCount++] = "& Val";
        }

        if (FuncDesc->funckind != FUNC_NONVIRTUAL && FuncDesc->funckind != FUNC_DISPATCH)
            FuncName += FuncKindNames[FuncDesc->funckind] + strSpace;

        FuncName += NameFromTypeDesc(&FuncDesc->elemdescFunc.tdesc, MemberDesc->OwningObjectInfo
            ->TypeInfo);
        FuncName += AnsiString(Names[0]);

        FuncName += "(";

        for (int counter = 1; counter < (int) NameCount; counter++)
        {
            if (counter >= (int) NameCount - FuncDesc->cParamsOpt)
                FuncName += "Optional ";

            FuncName += NameFromTypeDesc(&FuncDesc->lprgelemdescParam[counter -1].tdesc,
                    MemberDesc->OwningTypeInfo);
            FuncName += AnsiString(Names[counter]);

            if (counter < (int) NameCount - 1)
                FuncName += ", ";
        }

        FuncName += ")";

        mmoMemberDeclaration->Text = FuncName;
    }
    else
    {
        VARDESC    *VarDesc = MemberDesc->VarDesc;

        MemberDesc->OwningTypeInfo->GetNames(VarDesc->memid, (BSTR *) Names
            , MaxNameCount, &NameCount);

        AnsiString    Decl = NameFromTypeDesc(&VarDesc->elemdescVar.tdesc, MemberDesc->
                OwningTypeInfo) + Names[0];

        if (VarDesc->varkind == VAR_STATIC || VarDesc->varkind == VAR_CONST)
        {
            Decl = VarKindNames[VarDesc->varkind] + strSpace + Decl;

            if (VarDesc->lpvarValue != NULL)
                Decl += " = " +AnsiString(Variant(*VarDesc->lpvarValue));
        }

        mmoMemberDeclaration->Text = Decl;
    }
}


// Makes sure that for each entry in clbLibraries that is checked, there is an entry in
//      ComLibraries, and rebuilds the appropriate trees in MainForm and COMDesigner:
void __fastcall TObjectBrowserForm::btnOKClick(TObject *Sender)
{
    for (int i = 0; i < clbLibraries->Items->Count; i++)
    {
        if (clbLibraries->Checked[i])
        {
            // ComLibraries is a TStringList having a Library ID as its string.
            if (ComLibraries->IndexOf(((TLibInfo *) clbLibraries->Items->Objects[i])->LibID) == -1)
            {
                TLibInfo  *LibInfo = new TLibInfo((TLibInfo *) clbLibraries->Items->Objects[i]);

                ComLibraries->AddObject(LibInfo->LibID, (TObject *) LibInfo);
            }
        }
    }

    if (COMDesigner->Visible)
        COMDesigner->BuildClassTree();
    else if (MainForm->ObjectTreePageControl->ActivePage == MainForm->ComTabSheet)
        BuildComClassTree();
}


// Loads all Com Library header data from the Windows Registry, if it has not already been done:
void __fastcall TObjectBrowserForm::FormShow(TObject *Sender)
{
    if (!Initialized)
    {
        Screen->Cursor = crHourGlass; // May take 5-15 seconds; let user know that we're alive.

        // List that we will assign to clbLibraries->Items after we populate it:
        TStringList         *LibraryList = new TStringList;

        TStringList         *LibKeyNames = new TStringList;
        TStringList         *VersionKeyNames = new TStringList;
        TStringList         *ValueNames = new TStringList;
        TStringList         *VersionEntryNames = new TStringList;
        TStringList         *VersionSubEntry = new TStringList;
        AnsiString          ObjectName;
        const AnsiString    TypeLibKey = "\\Software\\CLASSES\\TypeLib\\";
        TRegistry           *Reg = new TRegistry(); // Wrapper for Windows Registry.

        LibraryList->Sorted = true;
        LibraryList->Duplicates = dupAccept;

        // If this confuses you, browse through the Registry via RegEdit.exe for a while.
        Reg->RootKey = HKEY_LOCAL_MACHINE;
        Reg->OpenKeyReadOnly(TypeLibKey);

        // Get the names of all the "Keys" (equivalent to directories) containing
        //  information on Com Libraries:
        Reg->GetKeyNames(LibKeyNames);

        for (int i = 0; i < LibKeyNames->Count; i++)
        {
            // Acquire all the sub-entries of this key; each of them should possess an
            //      entry for a Type Library:
            Reg->OpenKeyReadOnly(TypeLibKey + LibKeyNames->Strings[i]);
            Reg->GetKeyNames(VersionKeyNames);

            for (int j = 0; j < VersionKeyNames->Count; j++)
            {
                Reg->OpenKeyReadOnly(TypeLibKey + LibKeyNames->Strings[i]+ strBackSlash +
                        VersionKeyNames->Strings[j]);
                Reg->GetValueNames(ValueNames); // Gets the entries (not keys) under this key.

                if (ValueNames->Count < 1) // Nothing to read here.
                    continue;

                // There may be multiple versions of this Type Library; grab them all:
                ObjectName = Reg->ReadString(ValueNames->Strings[0]);
                Reg->GetKeyNames(VersionEntryNames);

                AnsiString Path = strBackSlash + Reg->CurrentPath + strBackSlash;

                for (int index = 0; index < VersionEntryNames->Count; index++)
                {
                    Reg->OpenKeyReadOnly(Path + VersionEntryNames->Strings[index]);
                    Reg->GetKeyNames(VersionSubEntry); 

                    if (VersionSubEntry->Count > 0)
                    {
                        // The key containing the library filename happens to be the 0th entry:
                        Reg->OpenKeyReadOnly(VersionSubEntry->Strings[0]);
                        Reg->GetValueNames(ValueNames);

                        if (ValueNames->Count > 0)
                        {
                            // The library filename is the value of the 0th entry:
                            AnsiString    LibName = Reg->ReadString(ValueNames->Strings[0]);

                            if (LibName != strNull
                                && GetFileExtension(LibName).LowerCase() != "oca")
                                LibraryList->AddObject(ObjectName, (TObject *) new TLibInfo(
                                        LibName));
                        }
                    }
                }
            }
        }

        clbLibraries->Items->Assign(LibraryList);
        
        delete LibraryList;
        delete LibKeyNames;
        delete VersionKeyNames;
        delete ValueNames;
        delete VersionEntryNames;
        delete VersionSubEntry;
        delete Reg;

        Screen->Cursor = crDefault; // Was hourglass, to indicate a potentially long wait.

        Initialized = true; // Let's not do this all over again next time.
    }

    TStringList     *TempLibs = new TStringList;
    TempLibs->Assign(ComLibraries);

    // Now make sure that every library in the ComLibraries list is "checked on":
    for (int i = 0; i < clbLibraries->Items->Count; i++)
    {
        clbLibraries->Checked[i] = false;

        TLibInfo    *LibInfo = (TLibInfo *) clbLibraries->Items->Objects[i];
        int         index = TempLibs->IndexOf(LibInfo->LibID);

        if (index > - 1)
            if (LibInfo->FileName == ((TLibInfo *) TempLibs->Objects[index])->FileName)
        {
            clbLibraries->Checked[i] = true;
            TempLibs->Delete(index);
        }
    }

    for (int i = 0; i < TempLibs->Count; i++)
    {
        TLibInfo	*LibInfo = (TLibInfo *) TempLibs->Objects[i];
        AnsiString	caption = (LibInfo->LibID != strNull) ? LibInfo->LibID
        												  : LibInfo->FileName;

        clbLibraries->Items->AddObject(caption, (TObject *) new TLibInfo(LibInfo));
        clbLibraries->Checked[clbLibraries->Items->Count - 1] = true;        
    }

    delete TempLibs;
}


void __fastcall TObjectBrowserForm::clbLibrariesKeyDown(TObject *Sender,
      WORD &Key, TShiftState Shift)
{
    if (Key == VK_F1)
    {
        int index = clbLibraries->ItemIndex;

        if (index < 0)
            return;

        TLibInfo *LibInfo = (TLibInfo *) clbLibraries->Items->Objects[index];

        if (LibInfo == NULL)
            return;

        if (LibInfo->HelpFileName == strNull)
        {
            ErrorTrap(4041);
            return;
        }

        ShowComHelp(LibInfo, Handle);
    }
}


void __fastcall TObjectBrowserForm::lbObjectsKeyDown(TObject *Sender,
      WORD &Key, TShiftState Shift)
{
    if (Key == VK_F1)
    {
        int index = lbObjects->ItemIndex;

        if (index < 0)
            return;

        TComObjectInfo *ObjectInfo = (TComObjectInfo *) lbObjects->Items->Objects[index];

        if (ObjectInfo != NULL)
            ShowComHelp(ObjectInfo, Handle);
    }
}


void __fastcall TObjectBrowserForm::lbMembersKeyDown(TObject *Sender,
      WORD &Key, TShiftState Shift)
{
    if (Key == VK_F1)
    {
        int index = lbMembers->ItemIndex;

        if (index >= 0)
            ShowComHelp((TMemberDesc *) lbMembers->Items->Objects[index], Handle);
    }
}

