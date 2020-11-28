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

#ifndef SettingsH
#define SettingsH

#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ComCtrls.hpp>
#include <Mask.hpp>
#include <ExtCtrls.hpp>
#include "CSPIN.h"
#include <CheckLst.hpp>

class TSettingsDialog : public TForm
{
__published:	// IDE-managed Components
    TTabSheet *tsRecentFiles;
    TButton *btnOK;
    TButton *btnCancel;
    TTabSheet *tsSheet;
    TCheckBox *cbObjectTreeAutoExpand;
    TBevel *Bevel2;
    TLabel *Label2;
    TBevel *Bevel3;
    TLabel *Label4;
    TRadioGroup *rgMainWindowSetting;
    TBevel *Bevel4;
    TButton *btnChangeWorkSpaceColor;
    TEdit *txtWorkSpaceColor;
    TLabel *Label3;
    TCSpinEdit *NodeSnapEdit;
    TLabel *NodeSnapLabel;
	TEdit *txtWorkSpaceColorView;
    TPageControl *PageControl;
    TRadioGroup *rgDeleteUnusedDataSets;
    TCheckBox *cbSaveSystemObjectsWithSheets;
    TBevel *Bevel1;
    TLabel *Label1;
    TCheckBox *cbDeleteWIPSheet;
	TCheckBox *cbCloseChangedSheets;
    TCheckBox *cbCloseUnnamedProjects;
    TCheckBox *cbCloseUnchangedSheets;
	TCheckBox *cbLoadCoreLib;
    TCheckBox *cbConfirmSaveDefalutName;
	TRadioGroup *rgHandleAmbig_Sheet;
	TCheckBox *cbMergeEquivObjects;
	TTabSheet *tsCompiler;
	TCheckBox *cbRunAfterCompile;
	TCheckBox *cbSaveBeforeCompile;
	TCheckBox *cbOptimizeGates;
	TCheckBox *cbEnforceVariantResolution;
	TCheckBox *cbMakeEdifIDs;
	TCheckBox *cbConfirmCloseProjects;
	TTabSheet *tsMessages;
	TCheckListBox *clbEnabledMessages;
	TCheckBox *cbShowAllWarnings;
	TRadioGroup *rgHandleAmbig_System;
	TBevel *Bevel5;
	TBevel *Bevel6;
	TLabel *Label5;
	TEdit *txtSelectionColorView;
	TEdit *txtSelectionColor;
	TButton *btnSelectionColor;
	TCheckBox *cbLoadLastProject;
	TCheckBox *cbConfirmCloseSystems;
	TRadioGroup *rgAutomaticBackup;
	TCheckBox *cbStaticSystemDesc;
	TCheckBox *cbCreateVexFile;
    void __fastcall btnOKClick(TObject *Sender);
    void __fastcall Edit1Change(TObject *Sender);
    void __fastcall Edit1MouseDown(TObject *Sender, TMouseButton Button,
          TShiftState Shift, int X, int Y);
    void __fastcall FormActivate(TObject *Sender);
    void __fastcall tsRecentFilesShow(TObject *Sender);
    void __fastcall txtWorkSpaceColorChange(TObject *Sender);
    void __fastcall txtWorkSpaceColorExit(TObject *Sender);
    void __fastcall btnChangeWorkSpaceColorClick(TObject *Sender);
	void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
	void __fastcall cbShowAllWarningsClick(TObject *Sender);
	void __fastcall btnSelectionColorClick(TObject *Sender);
	void __fastcall txtSelectionColorExit(TObject *Sender);
	void __fastcall txtSelectionColorChange(TObject *Sender);

public:
    __fastcall TSettingsDialog(TComponent *Owner);
    void LoadEnabledMessages();
};

extern PACKAGE TSettingsDialog *SettingsDialog;

#endif
