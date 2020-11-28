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

#include <process.h>
#include <Clipbrd.hpp>

#include "Globals.h"
#include "Settings.h"
#include "About.h"
#include "AttributeCalculator.h"
#include "AttributeEditor.h"
#include "DataSet.h"
#include "ErrorTrap.h"
#include "VivaIcon.h"
#include "Main.h"
#include "MyTMemo.h"
#include "NewSystemDialog.h"
#include "Project.h"
#include "ResourceManagers.h"
#include "SynthGraphic.h"
#include "SystemEditor.h"
#include "TreeGroupForm.h"
#include "VivaSystem.h"
#include "WidgetForm.h"
#include "I2adlEditor.h"
#include "VivaIO.h"
#include "ObjectBrowser.h"
#include "ComI2adl.h"
#include "ComFormDesigner.h"
#include "Resource.h"
#include "MyTShape.h"
#include "SearchEngine.h"
#include "Translators.h"
#include "MessageInfo.h"
#include "IniFile.h"
#include "ComManager.h"
#include "ProjectSettings.h"

#pragma resource "*.dfm"

bool    		ObjectTreeUpdating = false;
TStringGrid 	*CostCalc = NULL;
AnsiString		DimensionNames[4] = {"Left", "Top", "Right", "Bottom"};

TMainForm *MainForm;

__fastcall TMainForm::TMainForm(TComponent *Owner) : TForm(Owner)
{
#if VIVA_SD == false
    SysDoc->ReadOnly = true;
    SysAttMemo->ReadOnly = true;
	SystemSave->Visible = false;
	SaveSystemWODependenciesAs->Visible = false;
    SystemMerge->Visible = false;
    NewSystemButton->Visible = false;
    SystemTypeSelect->Enabled = false;
    ResourcePrototypeSelect->Enabled = false;

    // Change the Resource Editor to be "read only".
    ResourceSave->Visible = false;
    ResourceRead->Visible = false;
    CostCalc_->Options >> goEditing;
#endif

	FControlsEnabled = true;

	// Manualy change the Data Set Editor controls to be right justified.  This prevents the
    //     controls from being slid left every time C++ Builder opens Main.dfm.
    TAnchors		AnchorTopRight;

    AnchorTopRight << akTop << akRight;

    AssignDataSet->Anchors	= AnchorTopRight;
    DSName->Anchors			= AnchorTopRight;
    Label3->Anchors			= AnchorTopRight;
    GroupBox3->Anchors		= AnchorTopRight;
    GroupBox4->Anchors		= AnchorTopRight;

	// Reposition the Data Set Editor controls from the current right side.
    AssignDataSet->Left	= AssignDataSet->Parent->Width	- AssignDataSet->Width	- DATA_SET_EDITOR_MARGIN;
    DSName->Left		= AssignDataSet->Left			- DSName->Width			- DATA_SET_EDITOR_MARGIN;
    Label3->Left		= DSName->Left					- Label3->Width			- DATA_SET_EDITOR_MARGIN / 2;
    GroupBox3->Left		= GroupBox3->Parent->Width		- GroupBox3->Width		- DATA_SET_EDITOR_MARGIN;
    GroupBox4->Left		= GroupBox3->Left				- GroupBox4->Width		- DATA_SET_EDITOR_MARGIN;

	ErrorFileHandle = -1;

#ifdef LOG_EXPANDED_OBJECTS
	ExpandedObjectsLogFile = -1;
#endif

    HideTreeGroupIcon = HideTreeGroups->ImageIndex;
    HideTreeViewIcon = HideTreeView->ImageIndex;
    PlayStopSheetIcon = cmdPlayStopSheet->ImageIndex;
    AnsiString s = "ColorA=" + IntToHex((int) DEFAULT_WORKSPACE_COLOR, 6);
    ColorDialog->CustomColors->Insert(0, s);

    RecentFileMenuItemList = new VList(sizeof(TMenuItem));
    ZoomTextBoxMemos = new MyTMemoList(sizeof(MyTMemo));

    // Initialize ShapeMoveTimer (waits until mouse has stopped moving, then calls MoveLists()):
    ShapeMoveTimer = new TTimer(this);
    ShapeMoveTimer->Enabled = false;
    ShapeMoveTimer->Interval = 20;
    ShapeMoveTimer->OnTimer = ShapeMoveTimerOnTimer;

    ActiveTreeNode = NULL;

    I2adlView = new MyTScrollingWinControl(this);
    I2adlView->Parent = ViewHolder;
    I2adlView->Align = alClient;
    I2adlView->Reset();

    I2adlView->OnDragDrop = I2adlViewDragDrop;
    I2adlView->OnDragOver = I2adlViewDragOver;
    I2adlView->OnMouseDown = I2adlViewMouseDown;
    I2adlView->OnMouseMove = I2adlViewMouseMove;
    I2adlView->OnMouseUp = I2adlViewMouseUp;

    TextI2adlMemo = new MyTMemo(I2adlView);
    TextI2adlMemo->PopupMenu = FontEditTrigger;
    Designer = NULL;

    // Build the list of DataSet Attributes:

	AnsiString 		DSAttFileName = strVivaSystemDir + "DataSetAttributes.txt";

    if (LocateFile(DSAttFileName))
	    cmbAttribute->Items->LoadFromFile(DSAttFileName);
    else
    {
        UnInitProgram();
    	exit(EXIT_FAILURE);
    }

    cmbAttribute->ItemIndex = 0;

    MainCompiler = new SearchEngine();
    MainEditorsControl->Visible = true;
    CostCalc = CostCalc_;  // Load global cost calculator pointer.

    LoadSettings();

    // Initilize the resource editor column widths.
    CostCalc->ColWidths[0] = 90;
    CostCalc->ColWidths[1] = 100;
    CostCalc->ColWidths[2] = 55;
    CostCalc->ColWidths[3] = 55;
    CostCalc->ColWidths[4] = 55;
    CostCalc->ColWidths[5] = 80;
    CostCalc->ColWidths[6] = 238;

    // This call required for scroller bars to appear on main Wip Sheet
    Show();

    if (WindowState != wsMaximized && MainWindowSetting != mwCentered)
        BoundsRect = NewBoundsRect;

    // Now that the main window is sized, fix the height/width of the Wip Tree.
    WipComTreePanel->Height = IniFile->ReadInteger(strWindow, strWipTreeHeight, WipTree->Height);

	int		WipTreeWidth = IniFile->ReadInteger(strWindow, strWipTreeWidth, WipTree->Width);

    if (!AutoExpandObjectTree &&
    	WipTreeWidth != WipTree->Width)
    {
    	// Make sure the Wip tree is visible.
    	int		Adjustment = (WipTreeWidth < 125) ? 125 - WipTree->Width
        										  : WipTreeWidth - WipTree->Width;

        // Make sure the I2adl Editor is visible.
        if (TreeSplitter->Left - Adjustment < 400)
        	Adjustment -= 400 - TreeSplitter->Left + Adjustment;

   		TreePanel->Width += Adjustment;
    }

    WipTree->FullExpand();

    GenerateViewScaleMenuItems();

    WindowProc = MainFormWndProc;
    I2adlView->WindowProc = I2adlViewWndProc;

    DragList = new TShapeList(sizeof(MyTShapeI2adl));
    MoveList = new TShapeList(sizeof(MyTShapeI2adl));

    I2adlEditor::FontEditTrigger = FontEditTrigger;

	Application->OnActivate = OnAppActivate;

    BasicDataSetRootNode = NULL;
    ProjectObjectRootNode = NULL;
    LibraryRootNode = NULL;
}


// Automatically build the tool bar from the drop down menus.
void __fastcall TMainForm::CreateToolBar()
{
    // Initialize the new tool bar.
    MainToolBar = new TToolBar(ControlBar1);
    MainToolBar->Parent = ControlBar1;

    MainToolBar->EdgeInner = esNone;
    MainToolBar->EdgeOuter = esNone;
    MainToolBar->Height = 29;
    MainToolBar->ButtonHeight = 29;
    MainToolBar->ButtonWidth = 25;
    MainToolBar->Flat = true;
    MainToolBar->Tag = 1;
    MainToolBar->Images = VivaCOImages;
    MainToolBar->AutoSize = false;
    MainToolBar->Wrapable = false;
    MainToolBar->Align = alNone;

    // The ChevronButton is the button that appears when the buttons are off the screen
    ChevronButton = new TSpeedButton(MainToolBar);
    ChevronButton->Parent = MainToolBar;
    ChevronButton->Visible = false;

    ChevronButton->OnClick = ChevronButtonClick;
    ChevronButton->Width = 20;
    ChevronButton->Height = 27;
    ChevronButton->Name = strChevronButton;

	AnsiString 		CBFileName = strIconsDir + "Chevron.bmp";

    if (FileExists(CBFileName))
		ChevronButton->Glyph->LoadFromFile(CBFileName);

    int    ToolBarWidth = 0;

    // Create the tool bar buttons from the drop down menu items.
    //     Looping backwards prevents the buttons from being backwards.
    for (int i = MainMenu->Items->Count - 1; i >= 0; i--)
    {
        TMenuItem    *MenuItem = MainMenu->Items->Items[i];

        // Process the items in the drop down menu.
        for (int j = MenuItem->Count - 1; j >= 0; j--)
        {
            TMenuItem    *MenuSubItem = MenuItem->Items[j];

            // Skip the Help system if it is turned off.
            if (!MenuSubItem->Visible)
                continue;

            // Skip EditMenu items except for undo, redo, and separators.
            if (MenuItem->Name == "EditMenu" && j >= 3)
                continue;

            // Does this menu item have an icon or a separator with the "TB" (tool bar) flag?
            if (MenuSubItem->ImageIndex != -1 || MenuSubItem->Name.Pos("TB") == 12)
            {
                TToolButton    *NewButton = new TToolButton(MainToolBar);

                NewButton->Parent = MainToolBar;
                NewButton->ShowHint = true;
                NewButton->Hint = MenuSubItem->Hint;
                NewButton->ImageIndex = MenuSubItem->ImageIndex;
                NewButton->OnClick = MenuSubItem->OnClick;

                if (MenuSubItem->ImageIndex == -1)
                {
                    // Initialize the separator button.
                    NewButton->Style = tbsSeparator;
                    NewButton->Width = 8;
                }
                else
                {
                    // Initialize the regular icon button.
                    NewButton->Style = tbsButton;
                }
                
                // Save the pointers in the Tag field so the controls can find each other.
                NewButton->Tag = (int) MenuSubItem;
                MenuSubItem->Tag = (int) NewButton;
                MenuBtnSync(MenuSubItem);
                ToolBarWidth += NewButton->Width;
            }
        }
    }

    MainToolBar->Width = ToolBarWidth;
}


// Sync the menu item and it's relative toolbutton.
void __fastcall TMainForm::MenuBtnSync(TMenuItem *MenuItem, bool Toggle)
{
    if (Toggle)
        MenuItem->Checked = !MenuItem->Checked;

    if (MenuItem->Tag != NULL)
    {
        TToolButton *Button = (TToolButton *) MenuItem->Tag;

        // The following fixes Bugzilla entry #769.
        Button->ImageIndex = MenuItem->ImageIndex + 1;
        Button->ImageIndex = MenuItem->ImageIndex;

        if (MenuItem->Checked)
            Button->Style = tbsCheck;

        Button->Down = MenuItem->Checked;
        Button->OnClick = MenuItem->OnClick;

        if (MenuItem->Name == "HideTreeGroups" ||
        	MenuItem->Name == "HideTreeView")
        {
        	AnsiString	MenuCaption = MenuItem->Caption;
        	int			AndSignLoc = MenuCaption.Pos("&");

            if (AndSignLoc < 1)
	            Button->Hint = MenuCaption;
            else
            	Button->Hint = MenuCaption.SubString(1, AndSignLoc - 1) +
                	MenuCaption.SubString(AndSignLoc + 1, MenuCaption.Length() - AndSignLoc);
        }
    }
}


// Create a new empty project after asking the user if the changes should be saved.
void __fastcall TMainForm::FileNew(TObject *Sender)
{
    if (!MyProject->SaveChanges())
        return;

    delete MyProject;
    MyProject = NULL;

    new Project(1);
}


// Open a new project file after asking the user if the changes should be saved.
void __fastcall TMainForm::FileOpenClick(TObject *Sender)
{
	OpenFile(ftProject);
}


// Refines names for .idl and .vex files.
AnsiString TMainForm::MakeFileName(AnsiString RawFileName, bool Auto)
{
    // Add on the project path (even if strNull).
	RawFileName = MyProject->FilePath + RawFileName;

    // Increment the file name if the user has opted for it, this is Save (not Save As) and
    //		the project has a name.
    if (SettingFlags & SETTINGS_AUTO_VERSION &&
    	Auto &&
        MyProject->FilePath != strNull)
    {
        RawFileName = AutoVersion(RawFileName);
    }

    return RawFileName;
}


// Spews the image into a bitmap file named "I2adlView.bmp" and opens it in the default
//     bit map editor.
void __fastcall TMainForm::FilePrint(TObject *Sender)
{
    // The following prevents an infinite loop when the I2adl Editor is completly covered up.
    int 	width = I2adlView->ClientRect.Width();
    int     height = I2adlView->ClientRect.Height();

    if (width <= 0 ||
    	height <= 0)
	        return;

	// Make sure the I2adl Editor is visible.
    TTabSheet	*OrigActivePage = MainEditorsControl->ActivePage;

    if (MainEditorsControl->ActivePage != I2adlEdit)
    	MainEditorsControl->ActivePage = I2adlEdit;

    BringToFront();    

    // Create new bitmap and set its dimensions:
    TRect		dest;
    TRect		source;
    TBitMap 	*bmp = new TBitMap;
    TPoint		PrevPoint(I2adlView->HorzScrollBar->Position, I2adlView->VertScrollBar->
    	Position);

    bmp->Canvas->CopyMode = cmSrcCopy;
    bmp->Width = max(I2adlView->HorzScrollBar->Range, I2adlView->Width);
    bmp->Height = max(I2adlView->VertScrollBar->Range, I2adlView->Height);

    dest.bottom = 0;
	I2adlView->VertScrollBar->Position = 0;

    while (true)
    {
		I2adlView->HorzScrollBar->Position = 0;

        int		diff = 0;

        if (I2adlView->VertScrollBar->Position > 0 &&
        	I2adlView->VertScrollBar->Position + height >= I2adlView->VertScrollBar->Range)
            diff = height - (I2adlView->VertScrollBar->Position % height);

        source.Top = diff;
        source.bottom = height;
        dest.Top = dest.Bottom;
        dest.Bottom = dest.top + source.bottom - source.top;

	    source.left = 0;
	    dest.right = 0;

        while (true)
        {
            I2adlView->Invalidate();
            I2adlView->Update();

            for (int i = 0; i < CurrentSheet->Behavior->Count; i++)
                (*CurrentSheet->Behavior)[i]->I2adlShape->Paint();

            int		diff = 0;

            if (I2adlView->HorzScrollBar->Position > 0 &&
            	I2adlView->HorzScrollBar->Position + width >= I2adlView->HorzScrollBar->Range)
                diff = width - (I2adlView->HorzScrollBar->Position % width);

            source.right = width;
            source.left = diff;

            dest.left = dest.Right; //I2adlView->HorzScrollBar->Position;
            dest.Right = dest.left + source.right - source.left;

            // Need to use a valid Device Context for the copy operation:
            HDC		dc = GetDC(I2adlView->Handle);
            I2adlView->Canvas->Handle = dc;

            bmp->Canvas->CopyRect(dest, I2adlView->Canvas, source);

            ReleaseDC(I2adlView->Handle, dc);

            if (I2adlView->HorzScrollBar->Position + width >= I2adlView->HorzScrollBar->Range)
                break;

            I2adlView->HorzScrollBar->Position = I2adlView->HorzScrollBar->Position + width;
        };

        if (I2adlView->VertScrollBar->Position + height >= I2adlView->VertScrollBar->Range)
            break;

        I2adlView->VertScrollBar->Position = I2adlView->VertScrollBar->Position + height;
    };

    I2adlView->HorzScrollBar->Position = PrevPoint.x;
	I2adlView->VertScrollBar->Position = PrevPoint.y;

    // Save out the bitmap:
    AnsiString      BmpFileName(UserDir + "I2adlView.bmp");
    bool			FileSaved = true;

    try
    {
	    bmp->SaveToFile(BmpFileName);
    }
    catch (...)
    {
        FileSaved = false;
        ErrorTrap(4051, BmpFileName);
    }

    // Open the file in the ".bmp" file editor.
    if (FileSaved)
	    if (!ShellExecute(Handle, NULL, BmpFileName.c_str(), "", "", SW_SHOWNORMAL))
	        ErrorTrap(4007, BmpFileName);

    delete bmp;

    // Restore the original tab sheet.
    if (MainEditorsControl->ActivePage != OrigActivePage)
    	MainEditorsControl->ActivePage = OrigActivePage;
}

// Not currently used:
void __fastcall TMainForm::FilePrintSetup(TObject *Sender)
{
    PrintSetupDialog->Execute();
}


// Exit Viva.
void __fastcall TMainForm::FileExit(TObject *Sender)
{
    Close();
}


// Cut all of the selected objects by coping them into the ClipBoard and deleting them off
//		the sheet.
void __fastcall TMainForm::EditCut(TObject *Sender)
{
    if (ActiveControl == I2adlView)
    {
        CopyIntoClipBoard();
        DeleteSelectedObjects();
	}
}


// Copy all of the selected objects into the ClipBoard.
void __fastcall TMainForm::EditCopy(TObject *Sender)
{
    if (ActiveControl == I2adlView)
        CopyIntoClipBoard();
}


// Paste the objects in the ClipBoard onto the current Wip sheet.
void __fastcall TMainForm::EditPaste(TObject *Sender)
{
    if (ActiveControl == I2adlView)
        PasteFromClipBoard(Sender);
}


// Display the HTML Viva help screens.
void __fastcall TMainForm::OnlineHelpClick(TObject *Sender)
{
	AnsiString	VivaHelpFile = strHelpDir + "Viva_Help.htm";

    if (FileExists(VivaHelpFile))
        ShellExecute(Handle, NULL, VivaHelpFile.c_str(), "", "", SW_SHOWNORMAL);
    else
	    ErrorTrap(3044, VivaHelpFile);
}


// Display the Viva User Guide manual.
void __fastcall TMainForm::UserGuideClick(TObject *Sender)
{
	AnsiString	VivaHelpFile = strHelpDir + "VivaUserGuide.pdf";

    if (FileExists(VivaHelpFile))
        ShellExecute(Handle, NULL, VivaHelpFile.c_str(), "", "", SW_SHOWNORMAL);
    else
	    ErrorTrap(3044, VivaHelpFile);
}


void __fastcall TMainForm::HelpAbout(TObject *Sender)
{
    AboutDlg->ShowModal();
}


// Handles Windows Messages for DragOver from objects from the ObjectTree:
void __fastcall TMainForm::I2adlViewDragOver(TObject *Sender, TObject *Source
    , int X, int Y, TDragState State, bool &Accept)
{
    I2adlEditor::DragOver(Sender, Source, X, Y, State, Accept);
}


void __fastcall TMainForm::I2adlViewDragDrop(TObject *Sender, TObject *Source, int X, int Y)
{
    // Moved to end drag
    if (DropObject == NULL && Sender == ProjectObjectTree && Source == NULL)
    {
        bool    Dummy;

        if (ObjectTreePageControl->ActivePage == SystemTabSheet)
            I2adlViewDragOver(Sender, Sender, X, Y, TDragState(), Dummy);
    }

    if (DropObject != NULL)
    {
        MyProject->Modified();
        SnapI2adlToGrid(DropObject->I2adlShape);
        ConnectAllCompatibleNodes(DropObject);
        ResetMove(true);
        DropObject->BringShapeToFront();
        SaveSheetState();

        if (DropObject->NameCode == NAME_TEXT)
            TextI2adlMemo->ShowMemo(DropObject->I2adlShape);
        else
            DropObject->I2adlShape->Invalidate();

        PaintAllNames();

        DropObject = NULL;
        ReleaseCapture();
    }
}


// Main form resize event handler.
void __fastcall TMainForm::FormResize(TObject *Sender)
{
    // readjust windows for best viewing
    if (I2adlView == NULL || MyProject == NULL)
        return;

    if (I2adlView->Width > ClientRect.Right - FrameSplitter->MinSize)
    {
        I2adlView->ClientWidth = MainEditorsControl->ClientRect.Right - FrameSplitter->MinSize;

        if (I2adlView->ClientWidth < FrameSplitter->MinSize * 2)
            I2adlView->ClientWidth = FrameSplitter->MinSize * 2;
    }

    ResizeToolBar();
}


// Resize the tool bar, moving buttons that won't fit to the drop down menu.
void __fastcall TMainForm::ResizeToolBar()
{
    ChevronButton->Visible = false;

    // Move the drop down menu button to the far right edge of the tool bar.
    ChevronButton->Left = MainToolBar->Width - ChevronButton->Width;

    for (int i = 0; i < MainToolBar->ButtonCount; i++)
    {
        TToolButton    *ToolBarButton = MainToolBar->Buttons[i];

        if (ToolBarButton->Name == strChevronButton)
            continue;

        if ((ToolBarButton->Left + ToolBarButton->Width) > ChevronButton->Left)
        {
            ChevronButton->Visible = true;

            // Toggle Enabled and Marked to fix Bugzilla entry #766.
            ToolBarButton->Enabled = true;
            ToolBarButton->Marked = false;
            ToolBarButton->Enabled = false;
            ToolBarButton->Marked = true;
        }
        else if (ToolBarButton->Marked)
        {
            // Toggle Enabled and Marked to fix Bugzilla entry #766.
            ToolBarButton->Enabled = false;
            ToolBarButton->Marked = true;
            ToolBarButton->Enabled = true;
            ToolBarButton->Marked = false;
            MenuBtnSync((TMenuItem *) ToolBarButton->Tag);
        }
    }
}


// User clicked on the drop down menu button.  Display the non visible tool bar buttons.
void __fastcall TMainForm::ChevronButtonClick(TObject *Sender)
{
    ToolBarMenu->Items->Clear();

    int    X = BevelWidth + MainToolBar->Left + Left + ChevronButton->Left;
    int    Y = Top + ChevronButton->Height + 44; // 44 is height of title bar + mainmenu

    for (int i = 0; i < MainToolBar->ButtonCount; i++)
    {
        TToolButton    *ToolBarButton = MainToolBar->Buttons[i];

        // We use this to check for buttons that did not fit on toolbar.
        if (ToolBarButton->Name != strChevronButton &&
        	ToolBarButton->Left + ToolBarButton->Width > ChevronButton->Left)
        {
            // Create new menu item and set it's properties to that of the MainMenuItem
            TMenuItem    *NewItem = new TMenuItem(ToolBarMenu);
            TMenuItem    *MainMenuItem = (TMenuItem *) ToolBarButton->Tag;

            if (MainMenuItem->Caption == "-")
                NewItem->Caption = "-";
            else
                NewItem->Caption = ToolBarButton->Hint;

            NewItem->Checked = MainMenuItem->Checked;
            NewItem->OnClick = MainMenuItem->OnClick;
            NewItem->ImageIndex = MainMenuItem->ImageIndex;
            ToolBarMenu->Items->Add(NewItem);
        }
    }

    ToolBarMenu->Popup(X, Y);
}


void __fastcall TMainForm::I2adlViewMouseDown(TObject *Sender, TMouseButton Button, TShiftState
        Shift, int X, int Y)
{
    if (I2adlView->ActiveTransport != NULL)
    {
    	// Drawing a transport:   HandleTransportDraw() handles all cases.
        HandleTransportDraw(Button, X, Y);
    }
    else if (Shift.Contains(ssLeft) && !SelectRectTop->Visible) // Start a selection rectangle?
    {
        SelectRectTop->Width = 0;
        SelectRectLeft->Height = 0;
        SelectRectRight->Height = 0;
        SelectRectBottom->Width = 0;
        I2adlView->SelectStartPoint.x = X;
        I2adlView->SelectStartPoint.y = Y;
        SelectRectTop->Visible = true;
        SelectRectLeft->Visible = true;
        SelectRectRight->Visible = true;
        SelectRectBottom->Visible = true;
    }
}


void __fastcall TMainForm::I2adlViewMouseMove(TObject *Sender, TShiftState Shift, int X, int Y)
{
    if (ActiveControl != TextI2adlMemo)
    {
        ActiveControl = I2adlView;
        I2adlEditor::MouseMove(Shift, X, Y);
    }
}

void __fastcall TMainForm::I2adlViewMouseUp(TObject *Sender, TMouseButton Button
    , TShiftState Shift, int X, int Y)
{
    I2adlEditor::MouseUp(Button, Shift, X, Y);
}


void __fastcall TMainForm::FormKeyPress(TObject *Sender, char &Key)
{
    if (I2adlView->ActiveTransport != NULL)
    {
        // we are drawing a transport object handle esc
        if ((Key == 8) || (Key == 26))    //Backspace and ctrl-Z
            UndoTransport();

        if (Key == 27)    //Escape
            ReleaseTransport();
    }
}


void __fastcall TMainForm::FormKeyDown(TObject *Sender, WORD &Key, TShiftState Shift)
{
    if (ActiveTextI2adl != NULL)
        return;

    if (!Shift.Contains(ssShift) && !Shift.Contains(ssAlt))
    {
        if (Key == VK_F2)
        {
            if (Shift.Contains(ssCtrl))
                FileNew(this);
            else
                FileOpenClick(this);
        }
        else if (Key == VK_F3)
        {
            SaveProject(Shift.Contains(ssCtrl));
        }
        else if (Key == VK_F4)
        {
            if (Shift.Contains(ssCtrl))
                FileNewSheetItemClick(this);
            else
                OpenSheetClick(this);
        }
        else if (Key == VK_F5)
        {
            if (Shift.Contains(ssCtrl))
                SaveSheet(false);
            else
                SaveSheet(true);
        }
        else if (!Shift.Contains(ssCtrl))
        {
            if (Key == VK_F8)
            {
                BtnConvertSheetClick(this);
            }
            else if (Key == VK_F9)
            {
                cmdSynthesizeClick(this);
            }
            else if (Key == VK_F10)
            {
                FilePrint(this);
            }
            else if (Key == VK_F12)
            {
                cmdPlayStopSheet->Click();
            }
            else if (Key == VK_F1)
            {
            	// The user pressed the F1 (help) key.  Based on what control is active,
                //     display the requested help information.
                if (ActiveControl == I2adlView)
                {
	                // I2adl Editor:  Is an object is selected?
                    for (int i = 0; i < CurrentSheet->Behavior->Count; i++)
                    {
                        I2adl	*CurrentObject = (*CurrentSheet->Behavior)[i];

                        if (!CurrentObject->I2adlShape->Selected ||
                        	CurrentObject->NameCode == NAME_TRANSPORT ||
                            CurrentObject->NameCode == NAME_JUNCTION)
                        {
                            continue;
						}

                        // Is this a COM object or regular object?
                        if (CurrentObject->ExecuteType == EXECUTE_COM)
	                        ShowComHelp(CurrentObject->ComHelper->MemberDesc, Handle);
                        else
                        	CurrentObject->ShowHelpFile();

                        return;
                    }
                }
                else if (ActiveControl == ProjectObjectTree ||
                		 ActiveControl == SystemObjectTree)
                {
                	// Project/System object tree:
                	//		If the tree node is for an I2adl object, then display the
                    //		documentation?
                    GetI2adlObject(ActiveTreeNode)->ShowHelpFile();

                    return;
                }
                else if (ActiveControl == ComObjectTree)
                {
                	// F1 was pressed on the COM object tree.
                    TTreeNode	*Selected = ComObjectTree->Selected;

                    if (Selected == NULL || Selected->Data == NULL || Selected->Level == 0)
                        return;

                    if (Selected->Level == 1)
                        ShowComHelp(((TComObject *) Selected->Data)->ObjectInfo, Handle);
                    else if (Selected->Level == 2)
                        ShowComHelp((TMemberDesc *) Selected->Data, Handle);

                    return;
                }
                else if (ActiveControl == ComClassTree)
                {
                	// F1 was pressed on the COM class tree.
                    TTreeNode *Selected = ComClassTree->Selected;

                    if (Selected == NULL || Selected->Data == NULL)
                        return;

                    if (Selected->Level == 0)
                        ShowComHelp((TLibInfo *) Selected->Data, Handle);
                    else if (Selected->Level == 1)
                        ShowComHelp((TComObjectInfo *) Selected->Data, Handle);
                    else if (Selected->Level == 2)
                        ShowComHelp((TMemberDesc *) Selected->Data, Handle);

                    return;
                }
                else if (ActiveControl == MessageGrid)
                {
                	// F1 was pressed on a message.  Display the external documentation.
					DisplayExternalHelpFile();

                    return;
                }

                // All of the above failed, so display the main Viva help screen.
                OnlineHelpClick(this);
                
                return;
            }
        }
    }

    if ((Shift.Contains(ssCtrl) && Key == 'Z')
        || (Shift.Contains(ssAlt) && Key == VK_BACK))
    {
        if (Shift.Contains(ssShift))
            cmdRedoClick(Sender);
        else
            cmdUndoClick(Sender);
        Key = 0;
    }
    else if (Shift.Contains(ssCtrl))
    {
        if      (Key == 'A')
            SelectAll(Shift.Contains(ssShift));
        else if (Key == 'X')
            EditCut(this);
        else if (Key == 'C')
            EditCopy(this);
        else if (Key == 'V')
            EditPaste(this);
        else if (!Shift.Contains(ssAlt) && !Shift.Contains(ssShift) && Key == 45)
            EditCopy(Sender); // Ctrl + Insert
        else if (Key == 'S')
			cmdFitToSizeClick(NULL);
        else if (Key == 'W')
			cmdFitToWidthClick(NULL);
        else if (Key == 'I')
        	cmdSearchForInstancesClick(NULL);
        else if (Key == 'F')
        	cmdSearchByNameClick(NULL);
        else if (Key == VK_HOME)
			cmdZoomDefaultClick(NULL);
        else if (Key == VK_NEXT)
			cmdZoomInClick(NULL);
        else if (Key == VK_PRIOR)
			cmdZoomOutClick(NULL);
    }

    if (Shift.Contains(ssShift))
    {
        if  (!Shift.Contains(ssAlt) && !Shift.Contains(ssCtrl))
        {
            if (Key == VK_DELETE)
                EditCut(Sender);
            else if (Key == 45) // Insert
                EditPaste(Sender);
        }
    }
    else if (Key == VK_DELETE) //Delete; no modifiers
    {
        if (ActiveControl == I2adlView)
            DeleteSelectedObjects();
    }
    else if (Shift.Contains(ssAlt))
    {
        if (Key == VK_LEFT)
        {
            cmdBackClick(Sender); // Invoke "Back" command for sheet navigation
            Key = 0;
        }
        else if (Key == VK_RIGHT)
        {
            cmdNextClick(Sender); // Invoke "Next" command for sheet navigation
            Key = 0;
        }
    }
}


void __fastcall TMainForm::BtnConvertSheetClick(TObject *Sender)
{
    I2adlEditor::ConvertSheet();
}


// Do NOT call this function manually!!!!!!
void __fastcall TMainForm::Delete1Click(TObject *Sender)
{
    TWinControl    *TargetControl = (ActiveControl == I2adlView) ? this : ActiveControl;

    if (TargetControl != NULL)
        TargetControl->Perform(WM_KEYDOWN, VK_DELETE, 0);
}


// Load the Data Set Editor with the data set passed.
void __fastcall TMainForm::LoadDSEditor(AnsiString DataSetName)
{
    EditDataSet = GetDataSet(DataSetName);

    if (EditDataSet == NULL)
        return;

    DSName->Text = DataSetName;
    ElementsListBox->Clear();

    // Load the child data sets backwards to match the order of the data sets as they appear
    //     on the exposer/collector objects.
    for (int i = EditDataSet->ChildDataSets->Count - 1; i >= 0; i--)
    {
        DataSet    *ChildDS = (DataSet *) EditDataSet->ChildDataSets->Items[i];

        ElementsListBox->Items->Add(ChildDS->Name);
    }

    unsigned long    i = EditDataSet->AttributeFlags;
    int              index = 0;

    cmbAttribute->ItemIndex = -1;

    // Slide right until we hit a "1" bit; its position will become cmbAttribute->ItemIndex.
    while (i && !(i & 1))
    {
        i >>= 1;
        index++;
    }

    cmbAttribute->ItemIndex = (index < cmbAttribute->Items->Count) ? index : 0;

    cmbTreeGroup->Text = EditDataSet->TreeGroup;
    txtNodeColor->Text = EditDataSet->ColorCode;
    txtNodeColorExit(0);
}


// Update the display with all of the Data Sets information.
void __fastcall TMainForm::DSNameDragDrop(TObject *Sender, TObject *Source, int X, int Y)
{
    if (DataSetTree->Selected != NULL && DataSetTree->Selected->Data != TREE_GROUP_NODE)
        LoadDSEditor(DataSetTree->Selected->Text);
}


void __fastcall TMainForm::DSNameDragOver(TObject *Sender, TObject *Source, int X, int Y, 
        TDragState State, bool &Accept)
{
    Accept = (Source == DataSetTree  &&  DataSetTree->Selected->Data != TREE_GROUP_NODE);
}


void __fastcall TMainForm::ElementsListBoxDragOver(TObject *Sender, TObject *Source, int X
    , int Y, TDragState State, bool &Accept)
{
    Accept = (Source == DataSetTree  &&  DataSetTree->Selected->Data != TREE_GROUP_NODE);
}


void __fastcall TMainForm::ElementsListBoxDragDrop(TObject *Sender, TObject *Source, int X
    , int Y)
{
    if (DataSetTree->Selected != NULL  &&  DataSetTree->Selected->Data != TREE_GROUP_NODE)
        ElementsListBox->Items->Add(DataSetTree->Selected->Text);
}


// Create a new data set and add it to the Project/TreeListView.
void __fastcall TMainForm::AssignDataSetClick(TObject *Sender)
{
	// Prevent the user from changing system generated data sets.
    DataSet    *NewDataSet = GetDataSet(DSName->Text);

    if (NewDataSet != NULL &&
		NewDataSet->SystemGenerated != sgNot)
    {
    	// Display the original data set information that can't be changed.
	    LoadDSEditor(DSName->Text);

        ErrorTrap(110);

        // Rebuild data set tree because GetDataSet() creates pattern data sets.
        MyProject->BuildTreeView();
        return;
    }

    // Make sure each child data set is valid and will not cause a circular reference.
    for (int i = ElementsListBox->Items->Count - 1; i >= 0; i--)
    {
        DataSet    *ChildDataSet = GetDataSet(ElementsListBox->Items->Strings[i]);

        if (ChildDataSet == NULL)
        {
            ErrorTrap(10, ElementsListBox->Items->Strings[i]);

            return;
        }

        if (ChildDataSet->IsParentOf(NewDataSet))
        {
        	// Inform the user that this child data set is being discarded because it would
            //		cause a circular reference.
            ErrorTrap(209, ChildDataSet->Name);

            return;
        }
    }

    // Data sets must have at least one child data set.
    if (ElementsListBox->Items->Count <= 0)
    {
        ErrorTrap(9);
        return;
    }

    // Warn the user if more than 2 child data sets.
    if (ElementsListBox->Items->Count > 2)
        if (ErrorTrap(7) != mrYes)
            return;

    if (NewDataSet == NULL)
    {
        // Create the new data set.
        NewDataSet = new DataSet(DSName->Text, cmbTreeGroup->Text, NewWorkSpaceColor
        	, 1 << cmbAttribute->ItemIndex, sgNot);
    }
    else
    {
        // Confirm that it is ok to update an existing data set.
        if (ErrorTrap(8, NewDataSet->Name) != mrYes)
            return;

        NewDataSet->AttributeFlags = 1 << cmbAttribute->ItemIndex;
        NewDataSet->TreeGroup = cmbTreeGroup->Text;
        NewDataSet->ColorCode = NewWorkSpaceColor;
        NewDataSet->Exposer->DeleteProjectObject();
        NewDataSet->Collector->DeleteProjectObject();
        NewDataSet->Exposer   = NULL;
        NewDataSet->Collector = NULL;

        NewDataSet->ChildDataSets->Count = 0;
    }

    // Add the child data sets backwards because they were displayed backwards in the Data Set
    //     Editor inorder to match the order on the exposer/colletor object.
    for (int i = ElementsListBox->Items->Count - 1; i >= 0; i--)
    {
        DataSet    *ChildDataSet = GetDataSet(ElementsListBox->Items->Strings[i]);

        NewDataSet->ChildDataSets->Add(ChildDataSet);
    }

    // Create the new dataset Exposers.
    NewDataSet->BuildExposerAndCollector();
    MyProject->BuildTreeView();
    LoadDSEditor(NewDataSet->Name);
    MyProject->Modified();
}


// If only one object is selected, then display it's behavior as a new wip Sheet.
void __fastcall TMainForm::DescendClick(TObject *Sender)
{
    // Is there only one object seleted?
    I2adl    *SelectedObject = NULL;

    for (int i = 0; i < CurrentSheet->Behavior->Count; i++)
    {
        I2adl    *Object = (*CurrentSheet->Behavior)[i];

        // Ignore selected objects that do not have a behavior.
        if (Object->Behavior == NULL)
            continue;

        if (!Object->I2adlShape->Selected)
            continue;

        // The object is selected.  If more than one is selected, then don't know which to use.
        if (SelectedObject != NULL)
            return;

        SelectedObject = Object;
    }

    // Were any objects selected?
    if (SelectedObject == NULL)
        return;

    // Make sure the behavior is still valid (on a project object).
    LoadAsSheet(SelectedObject->ResolveObject(false, false, NULL, true));
}


void __fastcall TMainForm::FileNewSheetItemClick(TObject *Sender)
{
    SetCurrentSheet(CreateNewSheet());
}


// Save the Resource Editor information into the ResourceData.txt file.
void __fastcall TMainForm::ResourceSaveClick(TObject *Sender)
{
    TStringList		*ResourceLines = new TStringList;

    // The first row of CostCalc is blank so the columns can be resized.
    for (int RowNumber = 1; RowNumber < CostCalc->RowCount; RowNumber++)
    {
        // Combine the 7 cells in the row into a single string with 6 Tab separator characters.
        AnsiString     ResourceLine = CostCalc->Cells[0][RowNumber];

        for (int ColNumber = 1; ColNumber < 7; ColNumber++)
            ResourceLine = ResourceLine + "\t" + CostCalc->Cells[ColNumber][RowNumber];

        ResourceLines->Add(ResourceLine);

        // The EndOfSystems string signals the end of the resource data.
        if (CostCalc->Cells[COL_SYSTEM_NAME][RowNumber] == strEndOfSystems)
            break;
    }

    // Get the name of the ResourceData.txt file out of the base system.
    AnsiString	ResourceDataFileName = BaseSystem->GetAttribute(strResourceData);

    if (ResourceDataFileName == strNull)
        ResourceDataFileName = DefaultResourceData;

    ResourceDataFileName = FullPathName(ResourceDataFileName);

    // Save the resource information to disk.
    if (CheckForWrite(ResourceDataFileName, true))
    {
        try
        {
            ResourceLines->SaveToFile(ResourceDataFileName);
        }
        catch (...)
        {
            ErrorTrap(4051, ResourceDataFileName);
        }
    }

    delete ResourceLines;
}


// Refresh the Resource Editor with the data in ResourceData.txt.
//     The first row of CostCalc is blank so the columns can be resized.
void __fastcall TMainForm::ResourceReadClick(TObject *Sender)
{
	UpdateResources();
}


void TMainForm::UpdateResources(bool DiagnoseError)
{
    // Indicate that the usage information is not being displayed.
    ResourceDisplay->Caption = strDisplayUsage;
    ResourceSave->Enabled = true;

    // Get the name of the ResourceData.txt file out of the base system.
    AnsiString		ResourceDataFileName = BaseSystem->GetAttribute(strResourceData);
    AnsiString		FullResourceDataFileName = FullPathName(ResourceDataFileName);
    AnsiString		FullDefaultResourceData = FullPathName(DefaultResourceData);
    TStringList		*ResourceLines = new TStringList;
    bool			CanUseFile = false;

    // Try to open the requested ResourceData file.  Skip the default file because it will
    //		be tried if this attempt fails.
    if (ResourceDataFileName != strNull &&
        FullResourceDataFileName != FullDefaultResourceData)
    {
        try
        {
            ResourceLines->LoadFromFile(FullResourceDataFileName);

            // Is the file authorized for this version of Viva?
            if (IsResourceDataAuthorized(ResourceLines))
                CanUseFile = true;
            else
				ErrorTrap(207, VStringList(FullResourceDataFileName,
                	FullDefaultResourceData));
        }
        catch(...)
        {
            // We can't open the requested ResourceData file.
            ErrorTrap(144, VStringList(FullResourceDataFileName, FullDefaultResourceData));
        }
    }

    if (!CanUseFile)
    {
    	// Try to use the default ResourceData file.
        try
        {
            ResourceLines->LoadFromFile(FullDefaultResourceData);

            // Is the default ResourceData file authorized for this version of Viva?
            if (!IsResourceDataAuthorized(ResourceLines))
            {
				ErrorTrap(206, FullDefaultResourceData);

                return;
            }
        }
        catch(...)
        {
            ErrorTrap(192, FullDefaultResourceData);

            return;
        }
    }

    ResourcePrototypeSelect->Items->Clear();

    // Create just enough rows for the resources, first blank row and 10 lines for growth.
    CostCalc->RowCount = ResourceLines->Count + 11;

    for (int RowNumber = 0; RowNumber < ResourceLines->Count; RowNumber++)
    {
        AnsiString     ResourceLine = ResourceLines->Strings[RowNumber];

        for (int ColNumber = 0; ColNumber < 7; ColNumber++)
        {
            int    TabLocation = ResourceLine.Pos("\t");

            if (TabLocation <= 0)
            {
                // No Tab separator.  Use the rest of the string.
                CostCalc->Cells[ColNumber][RowNumber + 1] = ResourceLine;

                // Blank out the rest of the cells in the row.
                ResourceLine = strNull;
            }
            else
            {
                // Split the string before and after the Tab separator character.
                CostCalc->Cells[ColNumber][RowNumber + 1] = ResourceLine.SubString(1,
                    TabLocation - 1);

                ResourceLine = ResourceLine.SubString(TabLocation + 1,
                    ResourceLine.Length() - TabLocation);
            }
        }

        // Save the list of Resource Prototype names.
        if (CostCalc->Cells[0][RowNumber + 1] > strNull)
            ResourcePrototypeSelect->Items->Add(CostCalc->Cells[0][RowNumber + 1]);
    }

    // Remove the "Prototype", "Name" and "End of Systems" strings.
    ResourcePrototypeSelect->Items->Delete(0);
    ResourcePrototypeSelect->Items->Delete(0);
    ResourcePrototypeSelect->Items->Delete(ResourcePrototypeSelect->Items->Count - 1);

	ResourcePrototypeSelect->Items->Insert(0, strDefault);      

    // Blank out the last 10 lines.
    for (int RowNumber = ResourceLines->Count + 1; RowNumber < CostCalc->RowCount; RowNumber++)
    {
        for (int ColNumber = 0; ColNumber < 7; ColNumber++)
            CostCalc->Cells[ColNumber][RowNumber] = strNull;
    }

    delete ResourceLines;
}


// Create a new system and edit it.
void __fastcall TMainForm::NewSystemButtonClick(TObject *Sender)
{
    TNewSystemDLG    *Box = new TNewSystemDLG(I2adlView);
    
    Box->SystemTypeSelect->ItemIndex = 0;
    Box->ResourcePrototypeSelect->ItemIndex = 0;
    Box->ShowModal();
    
    if (Box->ModalResult != mrOk)
        return;

    // Create the new system.
    VivaSystem    *NewSystem = new VivaSystem(2, NULL, Box->SystemTypeSelect->ItemIndex + 1, 
    	Box->SystemName->Text);

    NewSystem->SysResourceManager->ResourcePrototypeName = Box->ResourcePrototypeSelect->Text;

    if (SystemTree->Selected == NULL)
        BaseSystem->Add(NewSystem);
    else
        ((VivaSystem *) SystemTree->Selected->Data)->Add(NewSystem);
    
    // Set up to use the new system.
    MyProject->SysEdit->DisplayInTree(SystemTree, false);
    BaseSystem->SyncLists();
    MyProject->Modified(true);
}


void __fastcall TMainForm::SystemTreeDragOver(TObject *Sender, TObject *Source, int X, int Y,
	TDragState State, bool &Accept)
{
    Accept = false;
    
#if VIVA_SD
    if (Source == SystemTree)
        if (SystemTree->Selected != NULL)
            Accept = true;
#else
    if (!WarnedSystemTreeDragOver)
    {
    	WarnedSystemTreeDragOver = true;

		ErrorTrap(4096);
    }
#endif
}


void __fastcall TMainForm::SystemTreeDragDrop(TObject *Sender, TObject *Source, int X, int Y)
{
#if VIVA_SD
    THitTests 			HT = SystemTree->GetHitTestInfoAt(X, Y);
    TNodeAttachMode 	AttachMode;
	TTreeNode    		*HitNode = SystemTree->GetNodeAt(X, Y);
    
    if (HT.Contains(htOnItem) || HT.Contains(htOnIcon))
        AttachMode = naAddChild;
    else if (HT.Contains(htOnIndent))
        AttachMode = naInsert;
    else
        return;

    VivaSystem 			*SelectedSystem = (VivaSystem *) SystemTree->Selected->Data;

    if (SelectedSystem != (VivaSystem *) HitNode->Data)
    {
        if (GetKeyState(VK_CONTROL) < 0)
        {
            // Control key is pressed.  Copy the system.
            VivaSystem    *NewSystem = new VivaSystem(SelectedSystem, NULL);

            ((VivaSystem *) HitNode->Data)->Add(NewSystem);

            HitNode = SystemTree->Items->AddChildObject(HitNode, *NewSystem->Name, NewSystem);

            SetImageIndex(HitNode, SUBSYSTEM_NODE_IMAGE_INDEX);
            MyProject->SysEdit->DisplayChildren(HitNode, NewSystem->SubSystems, false);
        }
        else
        {
            // Control key is not pressed.  Just move the system.

            // Make sure that HitNode is not subordinate to the moving node.
            TTreeNode		*ancestor = HitNode;

            for (int i = HitNode->Level; i > SystemTree->Selected->Level; i--)
            {
				ancestor = ancestor->Parent;
            }

            if (ancestor != SystemTree->Selected)
            {
                if (MyProject->SysEdit->Move(SelectedSystem, (VivaSystem *) HitNode->Data))
                    SystemTree->Selected->MoveTo(HitNode, AttachMode);
            }
        }

        BaseSystem->SyncLists();
    }

    MyProject->Modified(true);
#endif
}


void __fastcall TMainForm::SystemTreeKeyDown(TObject *Sender, WORD &Key, TShiftState Shift)
{
    if (Shift.Contains(ssCtrl))
    {
#if VIVA_SD
        SystemTree->Cursor = crHandPoint;
#endif
    }

    if (Key == VK_DELETE)
    {
#if VIVA_SD
        if (SystemTree->IsEditing())
            return;

        // Delete the selected system.
        VivaSystem	*DeleteSystem = (VivaSystem *) SystemTree->Selected->Data;

        if (DeleteSystem != NULL &&
        	DeleteSystem != BaseSystem)
        {
        	if (DeleteSystem->AssignedBehavior != NULL &&
            	MyProject->CompiledSheet != NULL)
            {
				ErrorTrap(4069, *DeleteSystem->Name);
				MyProject->CompiledSheet = NULL;
            }

            BaseSystem->Delete(DeleteSystem);
            SystemTree->Selected->Delete();

            MyProject->Modified(true);
        }
#else
		ErrorTrap(4095);
#endif
	}
    else if (Key == VK_RETURN)
    {
	    SystemTreeClick(Sender);
    }
}


void __fastcall TMainForm::SystemTreeKeyUp(TObject *Sender, WORD &Key, TShiftState Shift)
{
#if VIVA_SD
    SystemTree->Cursor = crDefault;
#endif    
}


void __fastcall TMainForm::SystemTreeEdited(TObject *Sender, TTreeNode *Node, AnsiString &S)
{
#if VIVA_SD
    VivaSystem    *EditingSystem = (VivaSystem *) Node->Data;

    EditingSystem->Name = SystemStringTable->QString(S);
    SysNameEdit->Text = S;
    BaseSystem->SyncLists();
    MyProject->Modified(true);
    Location->Text = MyProject->EditSystem->Location;
#endif    
}


void __fastcall TMainForm::SystemTreeClick(TObject *Sender)
{
	edtBaseSystemFileName->Text = MyProject->DynamicSystemName;

    if (SystemTree->Selected == NULL)
        return;

    // Fill in all of the System Editor Sheet infomation.
    MyProject->EditSystem = (VivaSystem *) SystemTree->Selected->Data;

    AnsiString	HoldSystemName = *MyProject->EditSystem->Name;

    // The following prevents the system name from being selected when switching to the System
    //		Editor tab after a system has been read in.
   	bool	OrigProjectChanged = MyProject->ProjectChanged;
   	bool	OrigSystemChanged = BaseSystem->SystemChanged;

    if (MyProject->EditSystem == BaseSystem)
	   	SysNameEdit->Text = HoldSystemName + HoldSystemName;

    SysNameEdit->Text = HoldSystemName;
    MyProject->ProjectChanged = OrigProjectChanged;
    BaseSystem->SystemChanged = OrigSystemChanged;

    SystemTypeSelect->ItemIndex = MyProject->EditSystem->Type - 1;
    ResourcePrototypeSelect->Text = strNull;  // Prevents the text from being selected.
    ResourcePrototypeSelect->Text = MyProject->EditSystem->SysResourceManager->
        ResourcePrototypeName;

    SysAttMemo->Lines->SetText(MyProject->EditSystem->GetAttributeText().c_str());
    SysDoc->Lines->Text = MyProject->EditSystem->Documentation;
    Location->Text = MyProject->EditSystem->Location;
}


void __fastcall TMainForm::SysNameEditChange(TObject *Sender)
{
    VivaSystem    *EditingSystem = (VivaSystem *) SystemTree->Selected->Data;

    if (*EditingSystem->Name != SysNameEdit->Text)
    {
        EditingSystem->Name = SystemStringTable->QString(SysNameEdit->Text);
        SystemTree->Selected->Text = SysNameEdit->Text;
        BaseSystem->SyncLists();
	    Location->Text = MyProject->EditSystem->Location;
        MyProject->Modified(true);
    }
}


// Performs dialog for saving files.  Checks the file for preexistence and readonly.
//		If Auto = true, then don't prompt the user.  Automatically use the SaveDialog->FileName.
//
//		Returns one of the following:
//		-1		User canceled the save
//		0		COM form file is ok to be replaced.
//		Handle  File handle to write to.  This is a temporary file on the local machine.  
//				The calling routine must closes it by calling CloseOutputFile().
int TMainForm::SaveFileDlg(FileTypeEnum FileType, bool Auto, bool SaveDependencies)
{
    if (SaveDependencies)
		SaveDialog->Title = "Save As";
    else
    	SaveDialog->Title = "Save Without Dependencies As";

    if (GetFilePath(SaveDialog->FileName) == strNull)
    {
        AnsiString RecentFileName = IniFile->ReadString(KeyPrefixes[FileType]
            , KeyPrefixes[FileType] + '1', StartUpDir);

        SaveDialog->FileName = GetFilePath(RecentFileName) + SaveDialog->FileName;
    }

    if (!Auto)
    {
        // Remove the extension, if applicable:
        if (GetFileType(SaveDialog->FileName) != ftUndetermined)
            SaveDialog->FileName = SaveDialog->FileName.SubString
                (1, SaveDialog->FileName.LastDelimiter(strPeriod) - 1);

        SaveDialog->Filter = FilterStrings[FileType][0] + "|*." + FilterStrings[FileType][1];

        if (FileType == ftVSystem)
            SaveDialog->InitialDir = strVivaSystemDir;
        else
            OpenDialog->InitialDir = strNull;

        if (!SaveDialog->Execute())
            return -1;    // User cancel
    }

    AnsiString		FileName(SaveDialog->FileName);
    FileTypeEnum	FTChosen = GetFileType(FileName);

    if (FTChosen != FileType)
    {
        // Remove the extension if not valid:
        if (FTChosen != ftUndetermined)
            SaveDialog->FileName = FileName.SubString(1, FileName.LastDelimiter(strPeriod) - 1);
        // Add correct extension:
        SaveDialog->FileName = SaveDialog->FileName + strPeriod + FilterStrings[FileType][1];
    }

    if (!CheckForWrite(SaveDialog->FileName, Auto &&
        !(SettingFlags & SETTINGS_AUTO_VERSION)))
        	return -1;

    if (FileType >= ftLast || FileType == ftComForm)
        return 0;

    AnsiString	TempFileName = TempDir + GetShortFileName(SaveDialog->FileName);

   	DeleteFile(TempFileName);

    int		FileHandle = FileCreate(TempFileName);

    if (FileHandle == -1)
    {
        // Could not create the temporary save file.
        ErrorTrap(13, TempFileName);
        return -1;
    }

    return FileHandle;
}


// This routine handles the opening of all Viva files.  Returns false if file does not exist.
//		SelFileType is the file type implied by user selection.
bool TMainForm::OpenFile(FileTypeEnum SelFileType, bool bKeepFileName, bool ReadingClipBoard)
{
// If a problem occurs while reading in the file, then attempt a recovery.
try
{
    // Save off the number of project objects and data sets so we can see how many are added.
    int		OldProjectObjectsCount = (MyProject != NULL) ? ProjectObjects->Count
    													 : 0;

    OldDataSetCount = (MyProject != NULL) ? ProjectDataSets->Count
       									  : 0;

    // If a behavior is running, then stop it.
    if (IsRunning)
        HaltExecution();

    FileTypeEnum	FileType = SelFileType; // Actual file type, as it sits on disk.

    if (SelFileType == ftMergeSystem)
    	FileType = ftVSystem;

    if (!bKeepFileName)
    {
        OpenDialog->Filter = strNull;

        // Initialize the file filters in OpenDialog and SaveDialog:
        for (int i = 0; i < ftLast; i++)
        {
            OpenDialog->Filter = OpenDialog->Filter + FilterStrings[i][0] + "|*."
            	+ FilterStrings[i][1];

            if (i < ftLast - 1)
                OpenDialog->Filter = OpenDialog->Filter + '|';
        }

        OpenDialog->FilterIndex = FileType + 1;

        // Load the name of the last file opened as the default file name:
        AnsiString		RecentFileName = IniFile->ReadString(KeyPrefixes[FileType]
            , KeyPrefixes[FileType] + '1', strNull);

		OpenDialog->FileName = RecentFileName;

        if (RecentFileName != strNull)
        {
            // Set default directory
            OpenDialog->FileName = RecentFileName;
        }
        else
        {
            if (FileType == ftVSystem)
                OpenDialog->InitialDir = strVivaSystemDir;
            else
                OpenDialog->InitialDir = StartUpDir;

            OpenDialog->FileName = strNull;
        }

        // If we are merging systems, then change "Open" to "Merge".
        AnsiString	OrigTitle = OpenDialog->Title;

        if (SelFileType == ftMergeSystem)
	    	OpenDialog->Title = "Merge";

        bool	FileOpened = OpenDialog->Execute();

        OpenDialog->Title = OrigTitle;

        if (!FileOpened)
            return false;

        // If the user changed the type of file being opened, then adjust for it.
        if (OpenDialog->FilterIndex != FileType + 1)
        	SelFileType = (FileTypeEnum) (OpenDialog->FilterIndex - 1);
    }

	AnsiString		SelectedFileName = OpenDialog->FileName;

    // Now let's determine actual file type according to what the user actually opened:
    FileType = VivaGlobals::GetFileType(OpenDialog->FileName);

	if (SelFileType == ftLibrary && FileType == ftSheet)
    	FileType = ftLibrary;

    // Load default extension if the user didn't enter it.
    if (FileType == ftUndetermined)
    {
        FileType = (SelFileType == ftMergeSystem) ? ftVSystem
                                                  : SelFileType;

        AnsiString		Ext = (FileType >= ftSheet && FileType < ftLast)
            ? FilterStrings[FileType][1]
            : FilterStrings[ftProject][1];

        SelectedFileName = SelectedFileName + strPeriod + Ext;
    }

    // Save the changes to the current project/system before opening a new file because it
    //		might be the same file.
    if (FileType == ftProject)
    {
		if (MyProject != NULL &&
	        !MyProject->SaveChanges())
		    	return false;
    }
    else if (FileType == ftVSystem)
    {
        if (MyProject != NULL &&
	        SelFileType != ftMergeSystem)
        {
        	// The following turns off the prompt to save the changes to the current project.
        	bool	OrigProjectChanged = MyProject->ProjectChanged;

        	MyProject->ProjectChanged = false;

            bool	WasSystemSaved = MyProject->SaveChanges();

        	MyProject->ProjectChanged = OrigProjectChanged;

            if (!WasSystemSaved)
            	return false;
        }
    }

    // If this is a .vex file, then $Spawn (don't open) the file.
    if (FileType == ftCompiledProject)
    {
        AnsiString	FileName("\"" + SelectedFileName + "\"");

        if (!FileExists(SelectedFileName))
        {
			ErrorTrap(23, SelectedFileName);
           	return false;
        }

        spawnlp(P_DETACH, VivaRunFileName.c_str(), VivaRunFileName.c_str(), FileName.c_str(),
        	0);

#if VIVA_IDE
        if (RunMode != rmEdit)
        {
            UnInitProgram();
            exit(EXIT_SUCCESS);
        }
#endif
    }
    else
    {
        VTextBase	loader(FileType);

        if (FileType != ftComForm)
        {
            // Establish the input file stream.
            if (!loader.OpenInputFile(SelectedFileName))
                return false;
        }

        if (FileType == ftProject) // .IDL
        {
            delete MyProject;
            MyProject = NULL;

            new Project(0, &loader, SelectedFileName, false, false);
        }
        else if (FileType == ftSheet || FileType == ftLibrary) // .IPG
        {
            // Create a project if one does not exist yet, in the case that sheet is being loaded
            //		from a command-line paramter to Viva.
            if (MyProject == NULL)
                new Project(1);

            if (FileType == ftLibrary)
            {
                // First, figure out if this library is already loaded.
                bool	alreadyloaded = false;
                int		i = 0;

                for (; i < MyProject->Libraries->Count; i++)
                {
                    if (SelectedFileName == *(AnsiString *) MyProject->Libraries->Items[i])
                    {
                        alreadyloaded = true;
                        break;
                    }
                }

                // If loaded, then set at end and reload all libraries.
                if (alreadyloaded)
                {
                    loader.CloseInputFile();

                    if (ErrorTrap(4090, SelectedFileName) != mrYes)
                        return false;

                    AnsiString		*lib = (AnsiString *) MyProject->Libraries->Items[i];

                    MyProject->Libraries->Delete(i);

                    MyProject->Libraries->Add(lib);
                    MyProject->ReloadLibraries();
                }
                else
                {
                    AnsiString		*lib = new AnsiString(SelectedFileName);

                    MyProject->Libraries->Add(lib);

                    loader.LoadLibrary(lib);

                    RebuildUI();
                }
            }
            else
            {
                loader.ReadI2adl(NULL, &SelectedFileName, true);
            }

            MyProject->Modified();
        }
        else if (FileType == ftVSystem) // .SD
        {
            // Open System
            if (SelFileType == ftMergeSystem)
            {
                // User opted to merge, not just open, a System.
                //		Remove the DynamicSystemName so VivaIO will not think it needs to
                //		discard the AssignedBehavior system info.
                AnsiString	OrigDynamicSystemName = MyProject->DynamicSystemName;

                MyProject->DynamicSystemName = strNull;

                // Read it in as a new subsystem of BaseSystem.
                loader.ReadI2adl(BaseSystem);

                MyProject->DynamicSystemName = OrigDynamicSystemName;

                // Make sure the subsystem name matches the ".sd" file name.
                VivaSystem	*NewSystem = (VivaSystem *) BaseSystem->SubSystems->Items[
                    BaseSystem->SubSystems->Count - 1];

                NewSystem->SyncSystemName(OpenDialog->FileName);

                // Set up to use the new system.
                MyProject->SysEdit->DisplayInTree(SystemTree, false);
                BaseSystem->SyncLists();
                MyProject->Modified(true);
            }
            else
            {
                MyProject->NewBaseSystem(loader, OpenDialog->FileName, true);

                // Warn the user if the ResoureData attribute is bad.
                MainForm->UpdateResources();

                MyProject->Modified();
            }
        }
        else if (FileType == ftComForm)
        {
            if (!TCOMDesigner::LoadFormIfOK(SelectedFileName, true))
                return false;
        }

        // Warn the user if the control node removal conversion failed to remove all duplicate and
        //     ambiguous objects.
        if (loader.FileVersion < 2.32 && OldProjectObjectsCount > 0)
        {
            for (int i = OldProjectObjectsCount; i < ProjectObjects->Count; i++)
            {
                I2adl	*ProjectObject = (*ProjectObjects)[i];

                if (ProjectObject != NULL)
                {
                    I2adl	*RObject = ProjectObject->ResolveObject(false);

                    if (ProjectObject != RObject)
                    {
                        ErrorTrap(138);
                        break;
                    }
                }
            }
        }

        BaseSystem->SyncLists();

        // Check for and delete unused datasets.
        //		Rebuild the object tree for the new objects and deleted exposers/collectors.
        if (!ReadingClipBoard &&
            FileType != ftComForm)
        {
			TTreeNode	*OrigLibraryRootNode = LibraryRootNode;


            CheckDataSetUsage();
            MyProject->BuildTreeView();

            // The first time an object library is added into a project, expand the Library
            //		tree node so the user can see the objects.
            if (OrigLibraryRootNode == NULL &&
            	LibraryRootNode != NULL)
            {
            	LibraryRootNode->Expand(false);
            }
        }
    }

    if (!ReadingClipBoard &&
    	FileType > ftUndetermined &&
    	FileType < ftLast)
    {
        AddRecentFile(SelectedFileName, FileType);
    }

    return true;
}
catch(...)
{
    // If the project file is corrupt, then remove the LastProjectName and abort Viva.
    if (SelFileType == ftProject)
    {
	    ErrorTrap(4099);

		LastProjectName = strNull;
	    IniFile->WriteString(strSettings, strLastProjectName, LastProjectName);

    	exit(EXIT_FAILURE);
    }
    else
    {
        // Inform the user that the project may be corrupt.
        AnsiString	FileType = "unknown type";

             if (SelFileType == ftProject)			FileType = "project";
        else if (SelFileType == ftSheet)			FileType = "sheet";
        else if (SelFileType == ftLibrary)			FileType = "library";
        else if (SelFileType == ftVSystem)			FileType = "system";
        else if (SelFileType == ftComForm)			FileType = "COM form";
        else if (SelFileType == ftCompiledProject)	FileType = "executable";
        else if (SelFileType == ftMergeSystem)		FileType = "system merge";

	    ErrorTrap(4083, FileType);
    }

    return false;
}
}


// Open a new system file.
void __fastcall TMainForm::OpenSystem1Click(TObject *Sender)
{
    OpenFile(ftVSystem);
}


void __fastcall TMainForm::SaveSystem1Click(TObject *Sender)
{
	SaveSystem();
}


// Save the system out to a text base file.
//		Returns "false" if the user canceled the save.
bool TMainForm::SaveSystem(bool SaveDependencies)
{
    // Save existing system as raw system type
    SaveDialog->DefaultExt = FilterStrings[ftVSystem][1];
    SaveDialog->FileName = (MyProject->SystemName == strNull)
    					 ? DefaultSystemDesc
                         : MyProject->SystemName;

    if (MyProject->DynamicSystemName != strNull)
	    SaveDialog->FileName = MyProject->DynamicSystemName;

    // Does the user want to save the whole system or just one subsystem?
    VivaSystem    *SystemToSave;

    if (SystemTree->Selected == NULL)
        SystemToSave = BaseSystem;
    else
        SystemToSave = (VivaSystem *) SystemTree->Selected->Data;

    if (SystemToSave != BaseSystem)
    {
        // Confirm that the user selected the subsystem on purpose.
        if (ErrorTrap(88, *SystemToSave->Name) != mrYes)
            SystemToSave = BaseSystem;
        else
            SaveDialog->FileName = *SystemToSave->Name;
    }

    int		OutFileHandle = SaveFileDlg(ftVSystem, false, SaveDependencies);

    if (OutFileHandle < 0)
        return false;

	WarnAboutDuplicateSystemNames();

    VTextBase	syssaver(ftVSystem);

	syssaver.OutFileHandle = OutFileHandle;
    syssaver.OutFileName = SaveDialog->FileName;
    syssaver.SaveSystem(SystemToSave, SaveDependencies);

    return true;
}


void __fastcall TMainForm::SystemTypeSelectChange(TObject *Sender)
{
    // get the index then change the system organization
    if (MyProject->EditSystem != NULL)
    {
        MyProject->EditSystem->SetType(SystemTypeSelect->ItemIndex + 1);
        SystemTreeClick(SystemTree);
    }

    MyProject->Modified(true);
}


void __fastcall TMainForm::WipTreeKeyDown(TObject *Sender, WORD &Key, TShiftState Shift)
{
    if (Shift.Contains(ssCtrl))
        WipTree->Cursor = crHandPoint;

    if (Key == VK_DELETE)
    {
        if (WipTree->IsEditing())
            return;

        CloseSheetClick(Sender);
        MyProject->Modified();
    }
}


void __fastcall TMainForm::WipTreeKeyUp(TObject *Sender, WORD &Key, TShiftState Shift)
{
    WipTree->Cursor = crDefault;
}


void __fastcall TMainForm::WipTreeDragDrop(TObject *Sender, TObject *Source, int X, int Y)
{
    if (Source == ProjectObjectTree || Source == SystemObjectTree)
    {
    	// User wants to open the object in the I2adl Editor.
        LoadAsSheet((I2adl *) ActiveTreeNode->Data);
        MyProject->Modified();
    }
    else if (Source == WipTree)
    {
    	// The user wants to move the current wip sheet to this location.
        TTreeNode	*HitNode = WipTree->GetNodeAt(X, Y);

        if (HitNode != NULL &&
        	HitNode != WipTree->Selected)
        {
			TTreeNode	*RootTreeNode = WipTree->Items->GetFirstNode();
        	int			OrigLocation = RootTreeNode->IndexOf(WipTree->Selected);

        	if (RootTreeNode == HitNode)
            {
            	// Hit the root tree node (project name).  Insert at the top.
	            WipTree->Selected->MoveTo(RootTreeNode, naAddChildFirst);
            }
            else
            {
                if (RootTreeNode->IndexOf(HitNode) < OrigLocation)
                {
                	// Move up the tree.  Insert infront of the Hit node.
		            WipTree->Selected->MoveTo(HitNode, naInsert);
                }
                else
                {
	                TTreeNode	*AfterHitNode = HitNode->getNextSibling();

                    if (AfterHitNode == NULL)
                    {
                        // Moving down to the tree, to the very end.
                        WipTree->Selected->MoveTo(RootTreeNode, naAddChild);
                    }
                    else
                    {
                        // Moving down the tree, to after the Hit node.
                        WipTree->Selected->MoveTo(AfterHitNode, naInsert);
                    }
                }
            }

			// Keep the Wip list in sync.
        	int		NewLocation = RootTreeNode->IndexOf(WipTree->Selected);

            if (OrigLocation > -1 &&
            	NewLocation > -1 &&
                OrigLocation != NewLocation)
            {
            	if (OrigLocation < NewLocation)
                {
                	for (int i = OrigLocation; i < NewLocation; i++)
                    	MyProject->Wip->Items[i] = MyProject->Wip->Items[i + 1];
                }
                else
                {
                	for (int i = OrigLocation - 1; i >= NewLocation; i--)
                    	MyProject->Wip->Items[i + 1] = MyProject->Wip->Items[i];
                }

                MyProject->Wip->Items[NewLocation] = WipTree->Selected->Data;
            }

            MyProject->Modified();
        }
    }
}


void __fastcall TMainForm::WipTreeDragOver(TObject *Sender, TObject *Source, int X, int Y,
        TDragState State, bool &Accept)
{
    Accept = false;

    if (Source == ProjectObjectTree ||
    	Source == SystemObjectTree)
    {
	    // Dragging a composite object onto the Wip tree opens it in the I2adl Editor.
        I2adl	*Object = GetI2adlObject(ActiveTreeNode, false);

        if (Object != NULL &&
            Object->Behavior != NULL)
                Accept = true;
    }
    else if (Source == WipTree)
    {
    	// Dragging a Wip tree sheet (not the project name) can change its position.
        if (WipTree->Selected != NULL &&
        	WipTree->Selected->Data != NULL)
				Accept = true;
    }
}


void __fastcall TMainForm::BtnCopySheetClick(TObject *Sender)
{
    if (WipTree->Selected == NULL ||
    	WipTree->Selected->Data == NULL)
        	return;

    if (OpenBehavior != NULL)
    {
        OpenBehavior = NULL;
        return;
    }

    I2adlList    *SheetContents = ((I2adl *) WipTree->Selected->Data)->Behavior;
    I2adlList    *NewSheet = new I2adlList(SheetContents);

    NewSheet->CopyOf = SheetContents->CopyOf;

    SetCurrentSheet(CreateNewSheet(WipTree->Selected->Text, NewSheet));
    MyProject->Modified();
}


// Merge in a new system by making it a new subsystem of the current base system.
void __fastcall TMainForm::MergeSystem1Click(TObject *Sender)
{
    OpenFile(ftMergeSystem);
}


bool TMainForm::SaveProject(bool Auto)
{
    SaveDialog->DefaultExt = FilterStrings[ftProject][1];

    if (MyProject->Name == strNewProject && (SettingFlags & SETTINGS_SAVE_DEFAULT_NAME))
    	Auto = false;

    // Present the Save As dialog to the user if either this is a Save As, or the project name
    // has not been set internally and we are supposed to confirm naming of new projects:
    FileTypeEnum	FileType = ftProject;

    // Project name if not set internally.
    SaveDialog->FileName = MakeFileName(MyProject->Name, Auto);

	int		OutFileHandle = SaveFileDlg(FileType, Auto);

    // Will only bring up the dialog if FileType != ftAuto:
    if (OutFileHandle < 0)
        return false;

    bool        SavingANewProject = (MyProject->FilePath == strNull);
    VTextBase	projsaver(ftProject);

	projsaver.OutFileHandle = OutFileHandle;
    projsaver.OutFileName = SaveDialog->FileName;

    MyProject->SetName(projsaver.OutFileName);
    projsaver.WriteProject(SavingANewProject);

    return true;
}


// Save the project.
void __fastcall TMainForm::SaveProject1Click(TObject *Sender)
{
	SaveProject(true);
}


// The user changed the Resource Prototype.
void __fastcall TMainForm::ResourcePrototypeSelectChange(TObject *Sender)
{
    if (MyProject->EditSystem != NULL)
    {
        MyProject->EditSystem->SysResourceManager->ResourcePrototypeName =
            ResourcePrototypeSelect->Text;
        SystemTreeClick(SystemTree);

        MyProject->Modified(true);
    }
}


void __fastcall TMainForm::SysAttRichEditChange(TObject *Sender)
{
    VivaSystem		*EditingSystem = (VivaSystem *) SystemTree->Selected->Data;

    if (EditingSystem == NULL)
        return;

    AnsiString		OriginalAttributes = EditingSystem->GetAttributeText();

    EditingSystem->SetAttributeText(SysAttMemo->Text);

    if (OriginalAttributes != EditingSystem->GetAttributeText())
    {
        MyProject->Modified(true);

        // Make sure the DefaultTargetSystem is always kept current (used by ResolveObject()).
		if (EditingSystem == BaseSystem)
	        BaseSystem->LoadDefaultTargetSystem();
    }
}


// Save the current WIP sheet out to disk.  Only save objects/behaviors that are really needed.
void TMainForm::SaveSheet(bool PromptForFileName, bool SaveDependencies)
{
    if (CurrentSheet == NULL || CurrentSheet->Behavior->Count == 0)
    {
        ErrorTrap(3007);    // To save a Sheet, it must be active and non-empty.
        return;
    }

    SaveDialog->DefaultExt = FilterStrings[ftSheet][1];

    bool        Auto = false;
    AnsiString  SheetName = CurrentSheet->NameInfo->Name;

	// If the sheet name is not valid (contains invalid Windows file name characters), then
    //     ask the user if it is ok to rename it?
    if (!IsValidFileName(SheetName))
        if (ErrorTrap(154, VStringList(CurrentSheet->NameInfo->Name, SheetName)) != mrYes)
	        return;

    // If the sheet was opened from a file, then use the current sheet name and the original
    //     directory.
    AnsiString	FileName(((I2adl *) MainForm->WipTree->Selected->Data)->Behavior->FileName);

    SaveDialog->FileName = GetFilePath(FileName) + SheetName + strPeriod + FilterStrings[ftSheet][1];

    // When Auto version is on, increment name until we have a unique name
    while (true)
    {
        if (PromptForFileName)
            break;

        if (!(SettingFlags & SETTINGS_SAVE_DEFAULT_NAME) || !SheetName.Pos("Sheet_"))
            Auto = true;

        // Check for AutoVersionControl
        if ((SettingFlags & SETTINGS_AUTO_VERSION) == 0)
            break;

        if (!FileExists(SaveDialog->FileName))
            break;

        SaveDialog->FileName = AutoVersion(SaveDialog->FileName);
    }

    int		OutFileHandle = SaveFileDlg(ftSheet, Auto, SaveDependencies);

    if (OutFileHandle < 0)
        return;

    VTextBase	sheetsaver(ftSheet);

    sheetsaver.OutFileHandle = OutFileHandle;
    sheetsaver.OutFileName = SaveDialog->FileName;

    // Create new index lists for just the behaviors/objects used on this Sheet.
    I2adl		*SaveSheet = CurrentSheet;
    I2adlList	*SaveObjects = new I2adlList;

	// Update the sheet name to match the file name.
	SheetName = GetShortFileName(sheetsaver.OutFileName);
	MyProject->ActiveWipNode->Text = SheetName;

    // The following is required to put the sheet name in the top left corner of the I2adl
    //		Editor.
    CurrentSheet->SetName(SheetName);

    I2adl		*tmpSheet = new I2adl(CurrentSheet->NameInfo->Name);

	tmpSheet->Attributes->Add(strWipSheet);
    tmpSheet->IsPrimitive = false;
    tmpSheet->Behavior = SaveSheet->Behavior;
    SaveObjects->Add(tmpSheet);
   	SaveObjects->ExpandReferences(SaveDependencies);

    sheetsaver.Write(SaveObjects, ftSheet, "Sheet  " + *tmpSheet->AttResource);

    ((I2adl *) MainForm->WipTree->Selected->Data)->Behavior->FileName = sheetsaver.OutFileName;

    delete tmpSheet;
    delete SaveObjects;

    sheetsaver.CloseOutputFile();

    // In case file was saved under a name which is different from the sheet name:
    I2adlView->Invalidate();
    I2adlView->Update();
    PaintAllNames();

    // Make sure the Play button is accurate.
    MyProject->CompiledSheet = MyProject->CompiledSheet;
}


void __fastcall TMainForm::FormActivate(TObject *Sender)
{
    I2adlView->Brush->Color = WorkSpaceColor;
    ViewHolder->Brush->Color = WorkSpaceColor;
}


void __fastcall TMainForm::MainEditorsControlChange(TObject *Sender)
{
    TTabSheet    *_Sheet = MainEditorsControl->ActivePage;

    if (_Sheet == I2adlEdit)
    {
    	// Rebuild the objects so any changes will appear.
        RebuildUI();

        ObjectTreePanel->Visible = true;
        SystemTree->Visible = false;
        DataSetTree->Visible = false;
    }
    else if (_Sheet == SystemEditor || _Sheet == CostEditor)
    {
        ObjectTreePanel->Visible = false;
        SystemTree->Visible = true;
        DataSetTree->Visible = false;

        // If no system is selected, then select BaseSystem.
        if (SystemTree->Selected == NULL)
            SystemTree->Selected = SystemTree->Items->GetFirstNode();

        // Always calling prevents the system anme and prototype text from being selected.    
        SystemTreeClick(NULL);
    }
    else if (_Sheet == DataSetEditor)
    {
        ObjectTreePanel->Visible = false;
        SystemTree->Visible = false;
        DataSetTree->Visible = true;
    }

    MyProject->BuildTreeView();
}


// Load the data set names into the drop down box.
void __fastcall TMainForm::DSNameDropDown(TObject *Sender)
{
    DSName->Items->Clear();
    
    for (int i = 0; i < ProjectDataSets->Count; i++)
    {
        DataSet    *ProjectDataSet = (DataSet *) ProjectDataSets->Items[i];

        DSName->Items->Add(ProjectDataSet->Name);
    }
}


// Determine if the new tree node can have its name changed.
//     Handles Object Trees and DataSetTree.
void __fastcall TMainForm::ObjectTreeChange(TObject *Sender, TTreeNode *TreeNode)
{
    if (TreeNode == NULL)
        return;

    // Record the currently selected ObjectTree (not DataSetTree) node every time it changes.
    if (Sender == ProjectObjectTree ||
    	Sender == SystemObjectTree ||
    	Sender == ComObjectTree)
    {
        ActiveTreeNode = TreeNode;
    }

    ((TTreeView *) Sender)->ReadOnly = !CanTreeNodeBeRenamed(TreeNode, rmByClickTwice);
}


// Handles right-clicks on the tree view.
//     ObjectTree:   Enhance right click to select the new node.
//     DataSetTree:  Loads the dataset under the mouse into the DS editor:
void __fastcall TMainForm::ObjectTreeMouseDown(TObject *Sender, TMouseButton Button, TShiftState
    Shift, int X, int Y)
{
    // We are only handling right-clicks.
    if (Button != mbRight)
        return;

    TTreeView    *TreeView = (TTreeView *) Sender;

    TreeView->Selected = TreeView->GetNodeAt(X, Y);
}


// Decides if the object being dragged over the tree can be dropped.
//     Handles ObjectTree and DataSetTree.// Handles the object being dropped on the tree.
//     Handles ObjectTree and DataSetTree.
void __fastcall TMainForm::ObjectTreeDragDrop(TObject *Sender, TObject *Source, int X, int Y)
{
    if (Source == ComClassTree)
    {
        TComObjectInfo		*ObjectInfo = (TComObjectInfo *) ComClassTree->Selected->Data;
        AnsiString			ObjectName = GetUniqueComObjectName(ComClassTree->Selected->Text,
            UsedGlobalObjectNames);

        GlobalComObjects->Add(new TComObject(ObjectInfo, ObjectName));
        BuildComObjectTree();
    }
    else if (Source == WipTree)
    {
        BtnConvertSheetClick(Sender);  // Convert the selected wip Sheet to a project object.
    }
    else if (Source == ProjectObjectTree ||
    		 Source == SystemObjectTree ||
             Source == DataSetTree)
    {
        // The user wants to move the node under a different tree group node.
        TTreeView		*TreeView = (TTreeView *) Source;
        TTreeNode		*HitNode = TreeView->GetNodeAt(X, Y);

        if (HitNode == NULL)
            return;

        TTreeNode	*SourceNode =
        			(TreeView == ProjectObjectTree || TreeView == SystemObjectTree)
        		  ? ActiveTreeNode
				  : DataSetTree->Selected;

		if (SourceNode->ImageIndex == LIBRARY_NODE_IMAGE_INDEX)
		{
        	if (HitNode->ImageIndex != LIBRARY_NODE_IMAGE_INDEX)
            	return;

            int		index = (HitNode->Data == NULL)
            			  ? 0  // After root "Libraries" tree node.
						  : MyProject->Libraries->IndexOf(HitNode->Data);

			MyProject->Libraries->Remove(SourceNode->Data);
			MyProject->Libraries->Insert(index, SourceNode->Data);

            MyProject->ReloadLibraries();
	        MyProject->Modified();

            return;
        }

        if (InsideRootTreeNode(strLibraries, HitNode) ||
        	InsideRootTreeNode(strLibraries, SourceNode))
        		return;

        AnsiString	NewTreeGroup = GetTreeGroupName(HitNode);

        // Make sure we are not dragging a tree group inside itself (infinite loop).
        if (SourceNode->Data == TREE_GROUP_NODE)
        {
            AnsiString 		OrigTreeGroup = GetTreeGroupName(SourceNode);
            int				OrigTreeGroupLen = OrigTreeGroup.Length();
            int				NewTreeGroupLen = NewTreeGroup.Length();

            if (NewTreeGroupLen == OrigTreeGroupLen)
            {
                if (NewTreeGroup == OrigTreeGroup)
                	return;
            }
            else if (NewTreeGroupLen > OrigTreeGroupLen)
            {
                if (NewTreeGroup.SubString(1, OrigTreeGroupLen + 1) == OrigTreeGroup +
                	strBackSlash)
                		return;
            }

            // Since a tree group was dragged, make it a sub tree group.
            NewTreeGroup = (NewTreeGroup == strNull)
                         ? SourceNode->Text
                         : NewTreeGroup + strBackSlash + SourceNode->Text;
        }

        // Is the hit node inside the Composite/Basic Objects root tree node?
        if (TreeView == ProjectObjectTree ||
        	TreeView == SystemObjectTree)
        {
	        if (!InsideRootTreeNode(strCompositeObjects, HitNode))
	            return;
        }
        else
        {
	        if (!InsideRootTreeNode(strBasicDataSets, HitNode))
	            return;
        }

        // Rename the tree group in the tree node and all of its descendents.
        RenameTreeGroup(SourceNode, NewTreeGroup);

        // Redisplay the tree view.
        MyProject->BuildTreeView();
        MyProject->Modified();
    }
}


// Rename the tree group in TreeNode and all of its descendents.
//     Handles ObjectTree and DataSetTree.
void TMainForm::RenameTreeGroup(TTreeNode *TreeNode, AnsiString &NewTreeGroup)
{
    void    *TreeNodeData =  TreeNode->Data;

    if (TreeNodeData == TREE_GROUP_NODE  ||
        TreeNodeData == FOOTPRINT_TREE_NODE)
    {
        // Recursive call to rename the descendents.
        TTreeNode    *ChildNode = TreeNode->getFirstChild();

        while (ChildNode != NULL)
        {
            AnsiString    ChildTreeGroup = (ChildNode->Data == TREE_GROUP_NODE)
                                         ?  NewTreeGroup + strBackSlash + ChildNode->Text
                                         :  NewTreeGroup;

            RenameTreeGroup(ChildNode, ChildTreeGroup);

            ChildNode = ChildNode->getNextSibling();
        }
    }
    else
    {
        // Terminal leaf node.  Update the tree group.
        if (TreeNode->TreeView == ProjectObjectTree ||
        	TreeNode->TreeView == SystemObjectTree)
        {
            // Object tree.
            I2adl    *Object = (I2adl *) TreeNodeData;

            if (Object->Behavior != NULL)
            {
                Object->Behavior->TreeGroup = NewTreeGroup;

                MyProject->Modified(Object->IsSystemObject);
            }
        }
        else
        {
            // Data set tree.
            DataSet    *DSet = (DataSet *) TreeNodeData;

            // Don't allow the user to change system generated tree group assignments.
            if (DSet->SystemGenerated == sgNot)
                DSet->TreeGroup = NewTreeGroup;
        }
    }
}


// Returns TreeNode's tree group name.
//     Warning!  This routine must handle project object, system object and data set trees.
AnsiString TMainForm::GetTreeGroupName(TTreeNode *TreeNode)
{
   	// Library name nodes and subsystem nodes don't have tree group names.
    if (TreeNode->ImageIndex == LIBRARY_NODE_IMAGE_INDEX ||
    	TreeNode->ImageIndex == SUBSYSTEM_NODE_IMAGE_INDEX)
	        return strNull;

    // Tree group nodes build their tree group name by traversing the tree.
    if (TreeNode->Data == TREE_GROUP_NODE)
    {
	    AnsiString	ReturnTreeGroup = strNull;

        while (TreeNode->Parent != NULL)
        {
            ReturnTreeGroup = (ReturnTreeGroup == strNull)
                            ? TreeNode->Text
                            : TreeNode->Text + strBackSlash + ReturnTreeGroup;

            TreeNode = TreeNode->Parent;
        }

	    return ReturnTreeGroup;
    }

    // Is this the data set tree?
    if (TreeNode->TreeView == DataSetTree)
    	return ((DataSet *) TreeNode->Data)->TreeGroup;

    // Get the I2adl object from the tree node.
    I2adl	*Object = GetI2adlObject(TreeNode);

    if (Object != NULL &&
    	Object->Behavior != NULL)
		    return Object->Behavior->TreeGroup;

    return strNull;
}


// The first pop up menu option.
//     Objects - Edit Object Behavior
//     Data Sets - Edit Data Set Definition
void __fastcall TMainForm::PopUpMenuOption1(TObject *Sender)
{
    if (DataSetTree->Visible)
        DataSetTreeDblClick(Sender);
    else
        LoadAsSheet((I2adl *) ActiveTreeNode->Data);
}


// The second pop up menu option.
//     Objects - Edit Object Attributes
//     Data Sets - Add to Child Data Set List
void __fastcall TMainForm::PopUpMenuOption2(TObject *Sender)
{
    if (DataSetTree->Visible)
    {
        if (DataSetTree->Selected != NULL &&
        	DataSetTree->Selected->Data != TREE_GROUP_NODE)
            	ElementsListBox->Items->Add(DataSetTree->Selected->Text);
    }
    else
    {
        I2adl    *Object = (I2adl *) ActiveTreeNode->Data;

        if (Object == NULL)
            return;

        AttributeDialog->EditObjectAttributes(Object, Object);
    }
}


// Enable the popup menu options that are valid for the active tree node.
void __fastcall TMainForm::ObjectTreeMenuPopup(TObject *Sender)
{
    // Only Project Objects, System Objects and Data Sets can be sorted by name/have $Tree
    //		Groups.  Display what the new option will be (if chosen).
    SortTreeByName1->Enabled = (ObjectTreePageControl->ActivePage == ProjectTabSheet ||
    							ObjectTreePageControl->ActivePage == SystemTabSheet ||
                                DataSetTree->Visible);
    HideTreeGroups1->Enabled = SortTreeByName1->Enabled;

    if (SortingTreeByName)
        SortTreeByName1->Caption = "Sort by Tree Group";
    else
        SortTreeByName1->Caption = "Sort by Name";

    HideTreeGroups1->Caption = HideTreeGroups->Caption;

    // Load default values for the other 4 pop up menu items.
    OpenAsSheet->Enabled = false;
    EditAttributes->Enabled = false;
    DeleteObject->Enabled = false;
    ChangeTreeGroup->Enabled = false;
	ObjectDocumentation->Enabled = false;
    cmdSearchOnName->Enabled = false;
	cmdSearch->Enabled = false;

    // -------------------- For Data Sets Nodes ---------------------
    if (DataSetTree->Visible)
    {
		DeleteObject->Enabled = CanTreeNodeBeDeleted(DataSetTree->Selected);
        ChangeTreeGroup->Enabled = CanTreeNodeBeRenamed(DataSetTree->Selected, rmFromPopUP);

        // Set the popup menu item descriptions.
        OpenAsSheet->Caption = "Edit Data Set Definition";
        EditAttributes->Caption = "Add to Child Data Set List";

        ObjectDocumentation->Visible = false;
        cmdSearchOnName->Visible = false;
        cmdSearch->Visible = false;

        if (DataSetTree->Selected->Data != TREE_GROUP_NODE)
        {
            OpenAsSheet->Enabled = true;
            EditAttributes->Enabled = true;
        }

        return;
    }

    // Reset the popup menu item descriptions.
    OpenAsSheet->Caption = OrigOpenAsSheetCaption;
    EditAttributes->Caption = OrigEditAttributesCaption;
    DeleteObject->Caption = OrigDeleteObjectCaption;

    ObjectDocumentation->Visible = true;
    cmdSearchOnName->Visible = true;
    cmdSearch->Visible = true;

    if (ActiveTreeNode == NULL)
        return;

	DeleteObject->Enabled = CanTreeNodeBeDeleted(ActiveTreeNode);
    ChangeTreeGroup->Enabled = CanTreeNodeBeRenamed(ActiveTreeNode, rmFromPopUP);

    // COM object tree:
    if (ObjectTreePageControl->ActivePage == ComTabSheet)
    {
	    if (ActiveTreeNode->Level > 0)
			cmdSearch->Enabled = true;

        return;
    }

    // Project/System object tree:
    I2adl	*Object = (I2adl *) ActiveTreeNode->Data;

    if (Object != TREE_GROUP_NODE &&
    	ActiveTreeNode->ImageIndex != LIBRARY_NODE_IMAGE_INDEX &&
    	ActiveTreeNode->ImageIndex != SUBSYSTEM_NODE_IMAGE_INDEX)
    {
        cmdSearch->Enabled = true;
        cmdSearchOnName->Enabled = true;
        ObjectDocumentation->Enabled = true;

        if (Object != FOOTPRINT_TREE_NODE &&
        	Object->NameCode != NAME_TEXT &&
        	Object->ExposerCode == EXPOSER_NOT)
        {
            EditAttributes->Enabled = true;

            if (Object->Behavior != NULL)
                OpenAsSheet->Enabled = true;
        }
    }
}


// The lower 3 menu items (Convert, Delete, and Play/Stop) should be enabled
//     only if a node representing a valid behavior is selected:
void __fastcall TMainForm::WipTreeMenuPopup(TObject *Sender)
{
    // ConvertSheet, DeleteSheet, and PlayStopBehavior are names of menu items:
    ConvertSheet->Enabled = false;
    DeleteSheet->Enabled = false;

    if (WipTree->Selected != NULL && WipTree->Selected->Data != NULL)
    {
        ConvertSheet->Enabled = true;
        DeleteSheet->Enabled = true;
    }
}


// Close the current wip sheet (unless the user changes their mind).
void __fastcall TMainForm::CloseSheetClick(TObject *Sender)
{
    // If the sheet is empty or the user is taking full responsibility for lost changes, then
    //     skip conformation.
    if (CurrentSheet->Behavior->Count > 0 &&
    	SettingFlags & SETTINGS_CONFIRM_DELETE_WIP_SHEET)
    {
        // If an unchanged project object, then skip confirmation.
	    I2adl	*ProjectObject = ProjectObjects->GetObject(CurrentSheet->Behavior->CopyOf);

        if (ProjectObject == NULL ||
       		!ProjectObject->Behavior->Equals(CurrentSheet->Behavior))
        {
        	// Ask the user if the changes should be saved.
            int		SaveChanges = ErrorTrap(45);

            if (SaveChanges == mrCancel)
                return;  // Keep changed sheet open.

            if (SaveChanges == mrYes)
            {
            	// Save the changes (in a project object).
			    I2adlEditor::ConvertSheet();

            	return;
            }
        }
    }

    // Close (delete) the current wip sheet.
    RemoveSheet(CurrentSheet);
}


// This is required because right clicking on a tree does not select a new node.
void __fastcall TMainForm::SystemTreeMouseDown(TObject *Sender, TMouseButton Button, TShiftState
        Shift, int X, int Y)
{
    if (Button == mbRight)
    {
        SystemTree->Selected = SystemTree->GetNodeAt(X, Y);

        // The following is required because RightClicking does not generate a Click event.
        SystemTreeClick(Sender);
    }
}


// This is required because right clicking on a tree does not select a new node.
void __fastcall TMainForm::WipTreeMouseDown(TObject *Sender, TMouseButton Button, TShiftState
        Shift, int X, int Y)
{
    if (Button == mbRight)
        WipTree->Selected = WipTree->GetNodeAt(X, Y);
}


// The user renamed a tree group node.  Change the name in all lower level objects.
void __fastcall TMainForm::ObjectTreeEdited(TObject *Sender, TTreeNode *Node, AnsiString &S)
{
    if (ObjectTreePageControl->ActivePage == ComTabSheet)
    {
        TComObject *ComObject = ((TComObject *) Node->Data);

        // Renaming of a component could be rejected; in such case, an exception will be thrown,
        //      which will jump clean out of this function, and ComObject will not be renamed.

        // If the corresponding object is not global, then it contains a control; rename it
        //      also; yes, we should be able to assume that Parent is not NULL:
        if (Node->Parent->Data != COM_GLOBALS_ROOT_NODE)
            ((TComObject *) Node->Data)->OleControl->Name = S;

        ComObject->Name = S;

        bool NeedsRebuild = false;

        // We must now go and rename all I2adl's that refer to this Com Object:
        for (int i = 0; i < ComI2adlList->Count; i++)
        {
            I2adl    		*ProjectObject = (I2adl *) ComI2adlList->Items[i];
            TComObject 		*OwningObject = (ProjectObject->ExecuteType == NAME_DISPATCH)
            							  ? ProjectObject->ComObject
                                          : ProjectObject->ComHelper->OwningObject;

            if (OwningObject == ComObject)
            {
	            AnsiString		NewName = (ProjectObject->ExecuteType == NAME_DISPATCH)
				? ComObject->Name + "'s Dispatch"
				: OwningObject->Name + strPeriod + ProjectObject->ComHelper->MemberDesc->Name;

                ProjectObject->SetName(NewName);
                ProjectObject->NameCode = ProjectObject->ExecuteType; // SetName messed it up.
                ProjectObject->Conformalize();

                if (CurrentSheet->Behavior->IndexOf(ProjectObject) > -1)
                    NeedsRebuild = true;
            }
        }

        if (NeedsRebuild) // need a total UI rebuild:
            RebuildUI();
    }
    else
    {
        Node->Text = S;

        AnsiString    TreeGroup = GetTreeGroupName(Node);

        RenameTreeGroup(Node, TreeGroup);

        // Sort the renamed node to its proper position.
        //     Cannot call BuildTreeView() because destroying "Node" (passed in) causes an abort.
        Node->Parent->CustomSort((PFNTVCOMPARE) CustomSortCompare, 0);
        MyProject->Modified();
    }
}


// Displays the TreeGroupDialog form that allows the user to rename tree groups.
void __fastcall TMainForm::ChangeTreeGroupClick(TObject *Sender)
{
    TTreeNode     *TreeNode = (DataSetTree->Visible) ? DataSetTree->Selected
                                                     : ActiveTreeNode;
    AnsiString    OrigTreeGroup = GetTreeGroupName(TreeNode);

    TreeGroupDialog->cmbTreeGroup->Text = OrigTreeGroup;
    TreeGroupDialog->cmbTreeGroup->Items = (DataSetTree->Visible) ? DatasetTreeGroups
                                                                  : ObjectTreeGroups;
    TreeGroupDialog->ActiveControl = TreeGroupDialog->cmbTreeGroup;
    TreeGroupDialog->ShowModal();
    
    if (TreeGroupDialog->ModalResult == mrOk &&
        TreeGroupDialog->cmbTreeGroup->Text != OrigTreeGroup)
    {
        // Rename the tree group in the active tree node and all of its descendents.
        RenameTreeGroup(TreeNode, TreeGroupDialog->cmbTreeGroup->Text);

        // Redisplay the project object tree view.
        MyProject->BuildTreeView();
        MyProject->Modified();
    }
}


// When double click on the WipTree, convert the sheet into an object.
void __fastcall TMainForm::WipTreeDblClick(TObject *Sender)
{
    if (WipTree->Selected->Data != NULL)
    {
        BtnConvertSheetClick(Sender);
    }
}


// When double click on the ObjectTree, edit the object's behavior.
//     Handles ObjectTree and DataSetTree.
void __fastcall TMainForm::ObjectTreeDblClick(TObject *Sender)
{
	TTreeView	*ObjectTree = (TTreeView *) Sender;

	TPoint			mouseloc = ObjectTree->ScreenToClient(Mouse->CursorPos);
	TTreeNode		*nodeundermouse = ObjectTree->GetNodeAt(mouseloc.x, mouseloc.y);

    if (nodeundermouse == ActiveTreeNode)
		LoadAsSheet(GetI2adlObject(ActiveTreeNode, false));
}


// Allow the user to save the changes to the project before exiting Viva.
void __fastcall TMainForm::FormClose(TObject *Sender, TCloseAction &Action)
{
    if (Sender != NULL &&
    	MyProject != NULL &&
    	!MyProject->SaveChanges())
    {
        Action = caNone;
        return;
    }

    delete RecentFileMenuItemList;
    RecentFileMenuItemList = NULL;  // Prevents form from being closed twice.

    delete ZoomTextBoxMemos;
    DestroyUI();
    SaveSettings();
    delete DragList;
    delete MoveList;

    if (Sender != NULL)
	    UnInitProgram();
}


// The user selected a new tree view tab.  Display the new tree.
void __fastcall TMainForm::ObjectTreeSheetChange(TObject *Sender)
{
    // Clear the exising tree views so the proper nodes will be expanded.
    ClearTreeView((TTreeView *) Sender);

    MyProject->BuildTreeView();
}


// Open a behavior sheet.  Make it the current wip Sheet.
void __fastcall TMainForm::OpenSheetClick(TObject *Sender)
{
    OpenFile(ftSheet);
}


// Load the last state of some of the controls/variables.  Called in TMainForm constructor.
void TMainForm::LoadSettings()
{
    WorkSpaceColor = (TColor) IniFile->ReadInteger(strWindow, strWindowColor, DEFAULT_WORKSPACE_COLOR);
    SelectionColor = (TColor) IniFile->ReadInteger(strWindow, strSelectionColor, DEFAULT_SELECTION_COLOR);
    NodeSnapTolerance = IniFile->ReadInteger(strWindow, strNodeSnapTolerance, 11);
    SetDrawColor();

    SortTreeByName->Checked = IniFile->ReadInteger(strIniSet, "TreeButton1", 0);
    ShowObjectAttributes->Checked = IniFile->ReadInteger(strIniSet, "ShowObjectAttributes", 0);
    HideTreeGroups->ImageIndex = IniFile->ReadInteger(strIniSet, "TreeButton2", HideTreeGroupIcon);
    LastGraphicVisible = IniFile->ReadInteger(strIniSet, "ShowGraphic", false);
    GlyphSize = IniFile->ReadInteger(strIniSet, "GraphSize", 5);

    SortingTreeByName = SortTreeByName->Checked;
    ShowingObjectAttributes = ShowObjectAttributes->Checked;

    if (HideTreeGroups->ImageIndex == HideTreeGroupIcon)
    {
        HidingTreeGroups = false;
        HideTreeGroups->Caption = strHideTreeGroups;
    }
    else
    {
        HidingTreeGroups = true;
        HideTreeGroups->Caption = strShowTreeGroups;
        HideTreeGroups->ImageIndex = HideTreeGroupIcon + 1;
    }

    SettingFlags = IniFile->ReadInteger(strSettings, strSettingFlags,
        SETTINGS_CONFIRM_DELETE_WIP_SHEET		|
        SETTINGS_CONFIRM_CLOSE_UNNAMED_PROJECT	|
        SETTINGS_SAVE_DEFAULT_NAME				|
        SETTINGS_SAVE_BEFORE_COMPILE);

    ConfirmCloseProjects = IniFile->ReadBool(strSettings, strConfirmCloseProjects, true);
    ConfirmCloseSystems = IniFile->ReadBool(strSettings, strConfirmCloseSystems, true);
    CloseUnchangedSheets = IniFile->ReadBool(strSettings, strCloseUnchangedSheets, true);
    LoadCoreLib = IniFile->ReadBool(strSettings, strLoadCoreLib, true);
    LoadLastProject = IniFile->ReadBool(strSettings, strLoadLastProject, false);
    LastProjectName = IniFile->ReadString(strSettings, strLastProjectName, strNull);
    AutoExpandObjectTree = IniFile->ReadBool(strSettings, strAutoExpandObjectTree, true);

#if VIVA_SD
    StaticSystemDesc = IniFile->ReadBool(strSettings, strStaticSystemDesc, false);
#else
    StaticSystemDesc = false;
#endif

    // Window states:
    MainWindowSetting = IniFile->ReadInteger(strWindow, strMainWindowSetting, mwLast);

    if (MainWindowSetting != mwCentered)
    {
        if (MainWindowSetting == mwMaximized ||
            IniFile->ReadBool(strWindow, strStartMaximized, false) == true)
        {
            WindowState = wsMaximized;
        }
        else
        {
            // Read in the 4 entries which apply to NewBoundsRect:

            TRect    _Rect = BoundsRect;
            int      pRect[4] = {_Rect.Left, _Rect.Top, _Rect.Right, _Rect.Bottom};

            // For up-to-date INI files
	        if (IniFile->ValueExists(strWindow, strMainForm + DimensionNames[0]))
			{
            	for (int i = 0; i < 4; i++)
                	pRect[i] = IniFile->ReadInteger(strWindow, strMainForm + DimensionNames[i]
                    , pRect[i]);

	            NewBoundsRect = TRect(TPoint(pRect[0], pRect[1]), TPoint(pRect[2], pRect[3]));
            }
            // For older INI files:
            else if (IniFile->ValueExists(strWindow, strMainWindowRect + 49))
            {
            	for (int i = 0; i < 4; i++)
                {
                	pRect[i] = IniFile->ReadInteger(strWindow
                    	, strMainWindowRect + IntToStr(i + 49), pRect[i]);
					IniFile->DeleteKey(strWindow, strMainWindowRect + IntToStr(i + 49));
                }

	            NewBoundsRect = TRect(TPoint(pRect[0], pRect[1]), TPoint(pRect[2], pRect[3]));
            }
            // For INI files with no relevant entry:
            else
            {
				NewBoundsRect.left = 30;
				NewBoundsRect.top = 25;
                NewBoundsRect.right = 770;
				NewBoundsRect.bottom = 575;
            }
        }

		// Make sure that a decent portion of MainForm is on the screen:
        if (NewBoundsRect.left > Screen->Width * .9)
        	NewBoundsRect.left = Screen->Width * .9;

        if (NewBoundsRect.top > Screen->Height * .8)
        	NewBoundsRect.top = Screen->Height * .8;
    }

    ObjectTreePageControl->ActivePage = ProjectTabSheet;

    int		mpheight = IniFile->ReadInteger(strWindow, strMessageGridHeight
    	, MessageGrid->Height);

    MessagePanel->Height = min(mpheight, MainForm->Height - 300);

    DeleteUnusedDataSets = (DeleteDSetsEnum) IniFile->ReadInteger(strSettings,
        strDeleteUnusedDataSets, soNo);

	if (IniFile->ValueExists(strSettings, strHandleAmbig_Sheet))
	    HandleAmbig_Sheet = (HandleAmbigEnum) IniFile->ReadInteger(strSettings,
    	   strHandleAmbig_Sheet, haRetainExisting);
    else
	    HandleAmbig_Sheet = (HandleAmbigEnum) IniFile->ReadInteger(strSettings,
    	   "HandleAmbiguousObjects", haRetainExisting);

    HandleAmbig_System = (HandleAmbigEnum) IniFile->ReadInteger(strSettings,
       strHandleAmbig_System, haReplaceExisting);

	RunAfterCompileOpt = IniFile->ReadBool(strCompiler, strRunAfterCompile, true);
	OptimizeGatesOpt = IniFile->ReadBool(strCompiler, strOptimizeGates, true);
	EnforceVariantRes = IniFile->ReadBool(strCompiler, strEnforceVariantResolution, true);
  	ShouldMakeEdifIDs = IniFile->ReadBool(strCompiler, strMakeEdifIDs, true);
  	CreateVexFile = IniFile->ReadBool(strCompiler, strCreateVexFile, false);

    CreateToolBar();

	ShowAllWarnings = IniFile->ReadBool(strEnabledMessages, strShowAllWarnings, false);
}


// When the name of the current Wip sheet is changed, this routine puts the new name in
//		the Wip tree node and the top left corner of the I2adl Editor.
void __fastcall TMainForm::WipTreeEdited(TObject *Sender, TTreeNode *Node, AnsiString &S)
{
	// Is this a real change?
    if (Node->Text == S)
    	return;

	// Put the new name into the Wip tree.
    Node->Text = S;

    // Put the new name into Wip sheet container object.
    if (Node->Data != NULL)
	    ((I2adl *) Node->Data)->SetName(Node->Text);

    // Put the new name into the I2adl Editor.
    I2adlView->Invalidate();
    I2adlView->Update();
    PaintAllNames();

    // Make sure the Play button is accurate.
    MyProject->CompiledSheet = MyProject->CompiledSheet;

    // If the project (root) node was changed, then put the new name into the project.
    if (Node->Level == 0 &&
    	MyProject != NULL)
    {
       	MyProject->Name = S;

        // If another project already has this name, then warn the user.
		AnsiString	NewProjectName = MyProject->FilePath + MyProject->Name + ".idl";

        if (FileExists(NewProjectName))
        	ErrorTrap(216, NewProjectName);
    }

    MyProject->Modified();
}


// Load the data set node that was double clicked on into the Data Set Editor.
void __fastcall TMainForm::DataSetTreeDblClick(TObject *Sender)
{
    // Is this a valid data set tree node?
    if (DataSetTree->Selected != NULL  &&  DataSetTree->Selected->Data != TREE_GROUP_NODE)
        DSNameDragDrop(NULL, NULL, 0, 0);
}


// Handles special keys on the object tree.
//     <Enter> - Performs double click function
//     Delete  - Deletes the object
void __fastcall TMainForm::ObjectTreeKeyDown(TObject *Sender, WORD &Key, TShiftState Shift)
{
	TTreeView	*ObjectTree = (TTreeView *) Sender;

    if (Key == VK_RETURN)
    {
		LoadAsSheet(GetI2adlObject(ActiveTreeNode, false));
    }
    else if (Key == VK_DELETE)
    {
        // The user wants to delete an object out of the project/system object tree view.
        if (!ObjectTree->IsEditing())
        	CanTreeNodeBeDeleted(ObjectTree->Selected, true);
    }
}


// Called on MouseUp for the right button.
// This pulls up a menu which immediately disappears, then activates the
//      Font dialog.  I did this in order to bypass the intrinsic
//      right-click functionality of TMemo.
void __fastcall TMainForm::FontEditTriggerPopup(TObject *Sender)
{
    MenuItem1->Visible = false; // The menu is only a trigger.

    // Put the current font of the Memo into the dialog:
    FontDialog1->Font->Assign(TextI2adlMemo->Font);

    // Show the dialog; if user hits 'Cancel', then we are done:
    if (!FontDialog1->Execute())
        return;

    MyTMemo *Memo = TextI2adlMemo; // Localize a pointer to the Memo

    // Changing the font will most likely alter how the text appears; we
    //      want to preserve its current state, so create a backup of
    //      the current text in the memo, named 'OldLines':
    TStringList *OldLines = new TStringList;
    OldLines->Assign(Memo->Lines);

    // Change the Memo's font to that of the Font Dialog:
    ActiveTextI2adl->I2adlShape->GetCanvas()->Font->Assign(FontDialog1->Font);

    // We are going to assign our old text back to the memo; need to
    //      temporarily turn off WordWrap so that it doesn't alter the text:
    Memo->WordWrap = false;
    Memo->Font->Assign(ActiveTextI2adl->I2adlShape->GetCanvas()->Font);
    Memo->WordWrap = true;

    // If there is text in the Memo, then resize it to fit text exactly;
    //      We really did need to go this far, even if there is no text:
    if (Memo->Text > strNull)
    {
        //And yes, AutosizeVertical does have to be called twice, in this order:
        Memo->AutosizeVertical(RESIZE_SHRINK_GROW);
        Memo->AutosizeHorizontal(OldLines);
        Memo->AutosizeVertical(RESIZE_SHRINK_GROW);
    }

    delete OldLines;
}


// When the user exits, the Node Color edit box, make sure it displays in hex:
void __fastcall TMainForm::txtNodeColorExit(TObject *Sender)
{
    UpdateColorFormat(txtNodeColor, NewWorkSpaceColor);
}


// This brings up the Color Dialog and updates the current
//      node copy's color with what is entered therein:
void __fastcall TMainForm::btnChangeNodeColorClick(TObject *Sender)
{
    UpdateColor(txtNodeColor, NewWorkSpaceColor);
}


// Updates the NewColor and the graphical color indicator (txtNodeColor):
void __fastcall TMainForm::txtNodeColorChange(TObject *Sender)
{
    ColorChange(txtNodeColor, txtColor, NewWorkSpaceColor);
}


// Hand-made event handler for <Recent File> menu items:
void __fastcall TMainForm::RecentFileItemClick(TObject *Sender)
{
    TMenuItem		*MI = (TMenuItem *) Sender;
    AnsiString		FileName = MI->Caption.SubString(ACCESS_KEY_STRING_LENGTH + 1
        , MI->Caption.Length() - ACCESS_KEY_STRING_LENGTH);
    FileTypeEnum	TypeIndex = (((TMenuItem *) Sender)->Parent == LibraryMenu)
    						  ? ftLibrary
                              : GetFileType(FileName);

    OpenDialog->FileName = FileName;

    if (!OpenFile(TypeIndex, true) && !FileExists(FileName))
    {
        TStringList		*RFList = new TStringList;

        IniFile->ReadSectionValues(KeyPrefixes[TypeIndex], RFList);

        int		Index = 0;

        for (int i = 0; i < RFList->Count; i++)
        {
            if (RFList->Strings[i].Pos(FileName))
            {
                // Find the quantifier in the value name:
                int		Pos = RFList->Names[i].Pos(KeyPrefixes[TypeIndex]);
                
                if (Pos > -1)
                {
                    Pos = Pos + KeyPrefixes[TypeIndex].Length();
                    Index = VStrToInt(RFList->Names[i].SubString(Pos, RFList->Strings[i].
                    	Length() - Pos)) - 1;
                }

                break;
            }
        }

        if (Index > -1)
            DeleteRFEntry(Index, TypeIndex);
            
        PopulateRecentFileMenu();
    }
}


void __fastcall TMainForm::ViewScaleMenuItemClick(TObject *Sender)
{
	Zoom(((TMenuItem *) Sender)->Tag);
}


// Figures out what Menu to use for Recent File entries, based on the file type:
TMenuItem *TMainForm::GetMenuToInsertInto(FileTypeEnum TypeIndex)
{
    if (TypeIndex == ftProject)
        return ProjectMenu;
    else if (TypeIndex == ftSheet)
        return SheetMenu;
    else if (TypeIndex == ftLibrary)
        return LibraryMenu;
    else if (TypeIndex == ftComForm)
        return COMDesigner->mnuFile;
    else if (TypeIndex == ftCompiledProject)
        return ExecutableMenu;
    else
        return SystemMenu;
}

// Inserts RECENT_FILE_MAX blanks, invisible items to be used as Recent Files menu items:
void __fastcall TMainForm::InitializeRecentFileMenu()
{
    for (int TypeIndex = 0; TypeIndex < ftLast; TypeIndex++) // For each file type
    {
        TMenuItem	*MenuToInsertInto = GetMenuToInsertInto((FileTypeEnum) TypeIndex);
        int			InsertAt = GetMenuPos(MenuToInsertInto) + 1;

        // Make a new separator and throw it in before the items:
        TMenuItem	*Separator = new TMenuItem(MainForm);
        Separator->Caption = "-"; // This is how to make it be a separator.
        MenuToInsertInto->Insert(InsertAt - 1, Separator);
        RecentFileMenuItemList->Add(Separator);

        MaxRecentFiles[TypeIndex] = IniFile->ReadInteger(strSettings
            , strMaxRecentFiles + (char) ('0' + TypeIndex), RECENT_FILE_DEFAULT);

        for (int i = 0; i < MaxRecentFiles[TypeIndex]; i++)
		{
            // Create the new MenuItem and set its event handler:
            TMenuItem	*NewItem = new TMenuItem(MainForm);
            
            NewItem->OnClick = RecentFileItemClick;
            MenuToInsertInto->Insert(InsertAt, NewItem);
            RecentFileMenuItemList->Add(NewItem);
        }
    }
}

// Deletes a RecentFile entry from the main INI file
void TMainForm::DeleteRFEntry(int Index, int TypeIndex)
{
    AnsiString 		KeyPrefix = KeyPrefixes[TypeIndex];
    AnsiString 		Value;
    int 			i;

    for (i = Index + 1; i < MaxRecentFiles[TypeIndex]; i++)
    {
        Value = IniFile->ReadString(KeyPrefix, KeyPrefix + IntToStr(i + 1), strNull);
        IniFile->WriteString(KeyPrefix, KeyPrefix + IntToStr(i), Value);
    }

    IniFile->WriteString(KeyPrefix, KeyPrefix + IntToStr(i), strNull);
}


// Adds all <recent file> entries from the INI file to the "File" menu:
void __fastcall TMainForm::PopulateRecentFileMenu()
{
    // Reset caption from all items from all menus, and make them invisible:
    TMenuItem	*CurrentItem;
    
    for (int i = 0; i < RecentFileMenuItemList->Count; i++)
    {
        CurrentItem = (TMenuItem *) RecentFileMenuItemList->Items[i];
        CurrentItem->Visible = false;
        CurrentItem->Caption = "-";
    }

    TStringList    *strFiles = new TStringList;
    AnsiString     Value; // General-use string.

    // Iterate thru each type (Project, Sheet, System, etc):
    for (int TypeIndex = 0; TypeIndex < ftLast; TypeIndex++)
    {
        RecentFiles[TypeIndex] = 0;

        // Iterate thru the entries for this type:
        for (int i = 1; i <= min(RECENT_FILE_MAX, MaxRecentFiles[TypeIndex]); i++)
        {
            // Read the entry from the INI file and see if it is not zero-length:
            Value = IniFile->ReadString(KeyPrefixes[TypeIndex]
                , KeyPrefixes[TypeIndex] + IntToStr(i), strNull);

            if (Value != strNull)
            {
                // First, see if it is a dupe; if so, remove it from the INI file:
                if (strFiles->IndexOf(Value) > -1)
                {
                    DeleteRFEntry(strFiles->IndexOf(Value), TypeIndex);
                }
                else // Otherwise, make a Recent File menu entry for it:
                {
                    AddRecentFileMenuItem(Value, (FileTypeEnum) TypeIndex);
                    strFiles->Add(Value);
                    RecentFiles[TypeIndex]++;
                }
            }
        }

        // Clear the list of Recent File names for this type:
        strFiles->Clear();
    }

    delete strFiles;
}


// Inserts Recent File entry to the head of the appropriate list on the menu
//      by setting the caption and making it visible:
void __fastcall TMainForm::AddRecentFileMenuItem(AnsiString FileName, FileTypeEnum TypeIndex)
{
    TMenuItem	*MenuToInsertInto = GetMenuToInsertInto(TypeIndex);

    // Also give it an access key according to its ordinal position:
    // Figure out what position to insert this item at (first of this group):
    int			InsertAt = GetMenuPos(MenuToInsertInto) - MaxRecentFiles[TypeIndex]
    	+ RecentFiles[TypeIndex];
    TMenuItem	*NewItem = MenuToInsertInto->Items[InsertAt];

    // We can give access keys to the first 10 entries ("0"-"9"):
    if (RecentFiles[TypeIndex] < 10)
        NewItem->Caption = "&" + IntToStr(RecentFiles[TypeIndex]) + "  " + FileName;
    else
        NewItem->Caption = "    " + FileName;

    // Make it visible:
    NewItem->Visible = true;

    // If first entry, then make the separator visible:
    if (RecentFiles[TypeIndex] == 0)
    {
        int		index = MenuToInsertInto->IndexOf(NewItem) - 1;
        
        MenuToInsertInto->Items[index]->Visible = true;
    }
}


// Adds a file name to the logical 'Recent Files' list corresponding to
//     the extension of <FileName>:
void __fastcall TMainForm::AddRecentFile(AnsiString FileName, FileTypeEnum TypeIndex)
{
	if (RunMode != rmEdit)
    	return;

    if (!(FileExists(FileName))) // Don't put it in if it is invalid
        return;

    // Determine which list to add the file name to:
    AnsiString		KeyPrefix;

    if (TypeIndex == ftUndetermined)
	    TypeIndex = GetFileType(FileName);

    if (TypeIndex == ftUndetermined) // Not a file type supported by Recent Files
        return;

    KeyPrefix = KeyPrefixes[TypeIndex];

    // First, search for duplicates.  If found, then remove it and put
    // this entry in at the end of list (top of stack):
    int				nDuplicate = RecentFiles[TypeIndex] + 1;
    AnsiString		Value;

    for (int i = 1; i <= MaxRecentFiles[TypeIndex]; i++)
    {
        if (FileName.UpperCase()
            == IniFile->ReadString(KeyPrefix, KeyPrefix + IntToStr(i), strNull).UpperCase())
            nDuplicate = i;
    }

    for (int i = nDuplicate; i > 1; i--)
    {
        Value = IniFile->ReadString(KeyPrefix, KeyPrefix + IntToStr(i - 1), strNull);
        IniFile->WriteString(KeyPrefix, KeyPrefix + IntToStr(i), Value);
    }

    IniFile->WriteString(KeyPrefix, KeyPrefix + '1', FileName);

    // Update the Menus:
    PopulateRecentFileMenu();
}


// Figure out what position to insert an item at relative to the menu that it is on:
int __fastcall TMainForm::GetMenuPos(TMenuItem *MenuToInsertInto)
{
    if (MenuToInsertInto == ProjectMenu)
        return ProjectMenu->Find("E&xit")->MenuIndex - 1; // Put before "Exit"
    else
        return MenuToInsertInto->Count;                   // Put at end of menu.
}


void TMainForm::GenerateViewScaleMenuItems()
{
	for (int i = 0; i < VIEW_SCALE_MAX; i++)
    {
    	TMenuItem	*newitem = new TMenuItem(ViewMenu);

        newitem->Tag = i + 1;
        
		if (i < 9)
	        newitem->Caption = "&" + IntToStr(i + 1) + "  " + IntToStr((i + 1) * 20) + "%";
        else
		    newitem->Caption = "&0  " + IntToStr((i + 1) * 20) + "%";

        newitem->OnClick = ViewScaleMenuItemClick;
		ViewMenu->Insert(ViewMenu->Count, newitem);
        ViewScaleMenuItems[i] = newitem;
    }

    ViewScaleMenuItems[ViewScale - 1]->Checked = true;
}


void TMainForm::Zoom(int NewViewScale)
{
	if (NewViewScale < 1 || NewViewScale > VIEW_SCALE_MAX)
    	return;

	RebuildUI(NewViewScale);
}


// Grows/Shrinks the current tree view to fit the size of the displayed nodes.
//     Handles ObjectTree and DataSetTree.
void __fastcall TMainForm::AutoSizeTreeView(TTreeView *TreeView)
{
    if (!AutoExpandObjectTree)
        return;

    if (TreeView == NULL)
        TreeView = (DataSetTree->Visible)
        			? DataSetTree
					: (TTreeView *) ObjectTreePageControl->ActivePage->Controls[0];

    TTreeNode	*TreeNode = TreeView->Items->GetFirstNode();
    int			LongestWidth = MIN_OBJECT_TREE_WIDTH;

    while (TreeNode != NULL)
    {
        GetLongestWidth(TreeNode, LongestWidth);
        TreeNode = TreeNode->getNextSibling();
    };

	LongestWidth = min(LongestWidth, MainForm->Width / 2);

    TreePanel->Width = LongestWidth;
}


TTreeView *TMainForm::GetActiveObjectTree()
{
    if (DataSetTree->Visible)
        return DataSetTree;

    return (TTreeView *) ObjectTreePageControl->ActivePage->Controls[0];
}


// Sometimes, when dragging an object from the system tree, if you stop
//      moving the mouse after the system has begun generating the drag
//      object and before it finishes, then there will be nothing to drop.
//      This function pre-empts the MyTScrollingWin::DragOver routine to
//      ensure that this does not happen.
void __fastcall TMainForm::ObjectTreeStartDrag(TObject *Sender, TDragObject *&DragObject)
{
    DragObject = NULL;
}


void __fastcall TMainForm::Preferences1Click(TObject *Sender)
{
    SettingsDialog->ShowModal();
}


// Expands ObjectTree to fit the extent of the nodes that are showing:
void __fastcall TMainForm::ObjectTreeExpanded(TObject *Sender, TTreeNode *Node)
{
    if (!ObjectTreeUpdating)
        AutoSizeTreeView((TTreeView *) Sender);
}


// Returns the width of the longest node in TreeNode and below.
void TMainForm::GetLongestWidth(TTreeNode *TreeNode, int &LongestWidth)
{
	TTreeView	*ObjectTree = (TTreeView *) TreeNode->TreeView;  
    int			TWidth = ObjectTree->Canvas->TextWidth(TreeNode->Text);

    SetMax(LongestWidth, TreeNode->Level * ObjectTree->Indent + TWidth + 80);

    if (!TreeNode->Expanded)
        return;

    TTreeNode    *ChildNode = TreeNode->getFirstChild();

    while (ChildNode != NULL)
    {
        GetLongestWidth(ChildNode, LongestWidth);

        ChildNode = ChildNode->getNextSibling();
    }
}


// Load the resource usage information into the Resource Editor.
void __fastcall TMainForm::ResourceDisplayClick(TObject *Sender)
{
    // Move the scroll bar to the top of the Resource Editor.
    CostCalc->TopRow = 0;

    // If the usage infomation is already displayed, then restore the prototype information.
    if (ResourceDisplay->Caption == strDisplayPrototype)
    {
        UpdateResources();
        return;
    }

    // Indicate that the usage information is being displayed.
    ResourceDisplay->Caption = strDisplayPrototype;
    ResourceSave->Enabled = false;

    // Make the spreadsheet large enough for all of the resources.
    CostCalc->RowCount = 2000;

    // Adjust some of the column headings.
    int    RowNumber = 1;

    CostCalc->Cells[COL_SYSTEM_NAME][RowNumber] = strSystem;
    CostCalc->Cells[COL_INTRINSIC_COST][RowNumber] = "Used";
    CostCalc->Cells[COL_MAX_INFO_RATE][RowNumber] = "Used";
    CostCalc->Cells[COL_PIN_LOCATION][RowNumber] = "Total";

    RowNumber++;

    CostCalc->Cells[COL_INTRINSIC_COST][RowNumber] = "Quantity";
    CostCalc->Cells[COL_MAX_INFO_RATE][RowNumber] = "Percent";
    CostCalc->Cells[COL_PIN_LOCATION][RowNumber] = "Objects";

    RowNumber++;

    // Load the resource usage information for each system.
    LoadResourceUsage(BaseSystem, RowNumber);

    // Remove the unused blank rows from the end of the speadsheet.
    CostCalc->RowCount = RowNumber;
}


// Load the resource usage information for this system and each subsystem.
void __fastcall TMainForm::LoadResourceUsage(VivaSystem *System_, int &RowNumber)
{
    ResourceList    *AvailableResources = System_->SysResourceManager->AvailableResources;

    if (AvailableResources != NULL)
    {
        bool    FirstResourceUsed = true;

        // Load the resource usage information for this system.
        for (int i = 0; i < AvailableResources->Count; i++)
        {
            Resource    *AvailableResource = (Resource *) AvailableResources->Items[i];

            // Skip resources that were not used.
            if (!AvailableResource->HasBeenUsed())
                continue;

            // If this is the first resource used in the system, then include a blank line and
            //     display the system name just once.
            if (FirstResourceUsed)
            {
                FirstResourceUsed = false;

                for (int ColNumber = 0; ColNumber < 7; ColNumber++)
                    CostCalc->Cells[ColNumber][RowNumber] = strNull;

                RowNumber++;

                CostCalc->Cells[COL_SYSTEM_NAME][RowNumber] = *System_->Name;
			    CostCalc->Cells[COL_PIN_LOCATION][RowNumber] = IntToStr(System_->
                	SysResourceManager->TotalObjects);
            }
            else
            {
                CostCalc->Cells[COL_SYSTEM_NAME][RowNumber] = strNull;
			    CostCalc->Cells[COL_PIN_LOCATION][RowNumber] = strNull;
            }

            // Load the resource information.
            AvailableResource->DisplayUsage(RowNumber);

            RowNumber++;
        }
    }

    for (int i = 0; i < System_->SubSystems->Count; i++)
        LoadResourceUsage((VivaSystem *) System_->SubSystems->Items[i], RowNumber);
}


// When ObjectTree collapses, shrink to fit contents, if applicable:
void __fastcall TMainForm::ObjectTreeCollapsed(TObject *Sender, TTreeNode *Node)
{
    if (!ObjectTreeUpdating)
        AutoSizeTreeView((TTreeView *) Sender);
}


// Switch between sorting the project objects by Tree Group or by Name.
void __fastcall TMainForm::TreeButton1Click(TObject *Sender)
{
    SortingTreeByName = !SortTreeByName->Checked;

    MenuBtnSync(SortTreeByName, true);

    // The following fixes Bugzilla entry #766 & #769.
    ResizeToolBar();

    // Clear the exising tree views so the proper nodes will be expanded.
    ClearTreeView(ActiveObjectTree);

    MyProject->BuildTreeView();
}


// Switch between showing/hiding $Tree Group objects.
void __fastcall TMainForm::TreeButton2Click(TObject *Sender)
{
    if (HideTreeGroups->ImageIndex == HideTreeGroupIcon)
    {
        // Hide the $Tree Groups.
        HidingTreeGroups = true;
        HideTreeGroups->ImageIndex = HideTreeGroupIcon + 1;
        HideTreeGroups->Caption = strShowTreeGroups;
    }
    else
    {
        // Show the $Tree Groups.
        HidingTreeGroups = false;
        HideTreeGroups->ImageIndex = HideTreeGroupIcon;
        HideTreeGroups->Caption = strHideTreeGroups;
    }

    MenuBtnSync(HideTreeGroups);

    // The following fixes Bugzilla entry #766 & #769.
    ResizeToolBar();

    MyProject->BuildTreeView();
}


// Switch between showing/hiding the object tree.
void __fastcall TMainForm::HideTreeViewClick(TObject *Sender)
{
    if (TreePanel->Width > 1)
    {
        // Hide the object tree.  Zero prevented TreeSplitter from reopening the tree.
        TreePanel->Width = 1;  
    }
    else
    {
        // Display the object tree.
        bool    OrigExpandObjectTree = AutoExpandObjectTree;

        AutoExpandObjectTree = true;

        AutoSizeTreeView();
        AutoExpandObjectTree = OrigExpandObjectTree;
    }

    TreeSplitterMoved(Sender);
}


// Because the TreePanel width has been changed, fix the HideTreeView icon.
void __fastcall TMainForm::TreeSplitterMoved(TObject *Sender)
{
    if (TreePanel->Width > 1)
    {
        // The tree is visible.
        HideTreeView->ImageIndex = HideTreeViewIcon;
        HideTreeView->Caption = strHideTreeView;
    }
    else
    {
        // The tree is hidden.
        HideTreeView->ImageIndex = HideTreeViewIcon + 1;
        HideTreeView->Caption = strShowTreeView;
    }

    MenuBtnSync(HideTreeView);
}


// Waits until mouse has stopped moving, then calls MoveLists():
void __fastcall TMainForm::ShapeMoveTimerOnTimer(TObject *Sender)
{
    static TPoint	p1 = TPoint(-1, -1); // meaning not set
    static TPoint	p2;

    p2 = Mouse->CursorPos;

    if (p2.x != p1.x || p2.y != p1.y)
    {
        p1 = Mouse->CursorPos;
    }
    else
    {
        p1 = TPoint(-1, -1);
        ShapeMoveTimer->Enabled = false;
        MoveLists(I2adlView->ScreenToClient(Mouse->CursorPos));
    }
}


void __fastcall TMainForm::SelectAll1Click(TObject *Sender)
{
    SelectAll();
}


void __fastcall TMainForm::InvertSelection1Click(TObject *Sender)
{
    SelectAll(true);
}


void __fastcall TMainForm::MainMenuChange(TObject *Sender, TMenuItem *Source, bool Rebuild)
{
    if (TextI2adlMemo != NULL && TextI2adlMemo->Visible)
        TextI2adlMemo->HideMemo();
}


// The system documentation has been changed so update the system.
void __fastcall TMainForm::SysDocChange(TObject *Sender)
{
    if (SystemTree->Selected != NULL)
    {
        VivaSystem    *EditingSystem = (VivaSystem *) SystemTree->Selected->Data;

        if (EditingSystem != NULL  &&  EditingSystem->Documentation != SysDoc->Lines->Text)
        {
            EditingSystem->Documentation = SysDoc->Lines->Text;
            MyProject->Modified(true);
        }
    }
}


// Save the current state of some of the controls/variables.  Called on TMainForm destructor.
void TMainForm::SaveSettings()
{
	// The try/catch block is required so Viva can exit when after the compiler/router runs the
    //     system out of memory.
    try
    {
        IniFile->WriteInteger(strIniSet, "TreeButton1", SortingTreeByName);
        IniFile->WriteInteger(strIniSet, "ShowObjectAttributes", ShowingObjectAttributes);
        IniFile->WriteInteger(strIniSet, "TreeButton2", HideTreeGroups->ImageIndex);
        IniFile->WriteInteger(strIniSet, "ShowGraphic", LastGraphicVisible);
        IniFile->WriteInteger(strIniSet, "ObjectsView", ObjectTreePageControl->ActivePageIndex);
        IniFile->WriteInteger(strIniSet, "GraphSize", GlyphSize);

        // Window states:
        IniFile->WriteInteger(strWindow, strWipTreeHeight, WipComTreePanel->Height);
        IniFile->WriteInteger(strWindow, strWipTreeWidth, WipComTreePanel->Width);
        IniFile->WriteInteger(strWindow, strMainWindowSetting, MainWindowSetting);
        IniFile->WriteInteger(strWindow, strMessageGridHeight, MessagePanel->Height);

        // This is a bit tricky: using the bytes in a string to represent a TRect:
        TRect 	_Rect = BoundsRect;
        int 	pRect[4] = {_Rect.Left, _Rect.Top, _Rect.Right, _Rect.Bottom};

        for (int i = 0; i < 4; i++)
            IniFile->WriteInteger(strWindow, strMainForm + DimensionNames[i], pRect[i]);

        IniFile->WriteBool(strWindow, strStartMaximized, WindowState == wsMaximized);
    }
    catch(...)
    {
    	ErrorTrap(159);
    }
}


void __fastcall TMainForm::MainFormWndProc(Messages::TMessage &Message)
{
    // Just minimizes MainForm instead of the whole application:
    if (Message.Msg == WM_NCLBUTTONDOWN && Message.WParam == 8 && IsRunning)
    {
        if (WindowState != wsMinimized)
        {
            WindowState = wsMinimized;
            Message.Msg = 0;
        }
    }

    if (RunMode != rmEdit)
        Visible = false;

    // This procedure is an add-on; this message still needs to be processed:
    WndProc(Message);
}


// Used during project load to prevent user from blowing Viva up with everything is shifting
//		around.
void __fastcall TMainForm::DisabledControlEventHandler(tagMSG &Msg, bool &Handled)
{
    if (Msg.message == WM_LBUTTONDOWN || Msg.message == WM_RBUTTONDOWN ||
        Msg.message == WM_KEYDOWN || Msg.message == WM_KEYUP)
		Handled = true;

    if (Msg.message == WM_LBUTTONDBLCLK || Msg.message == WM_NCLBUTTONDBLCLK)
		Handled = true;

	if (Msg.message == WM_NCLBUTTONDOWN)
		Handled = true;

	if (Msg.message == WM_SYSKEYDOWN || Msg.message == WM_SYSKEYDOWN)
		Handled = true;
}



// Whenever either scrollbar is scrolled, this forces its position to gridsnap and repaints
//      node names.
void __fastcall TMainForm::I2adlViewWndProc(Messages::TMessage &Message)
{
    if (Message.Msg == WM_HSCROLL || Message.Msg == WM_VSCROLL)
    {
        TControlScrollBar	*SB = (Message.Msg == WM_VSCROLL)
            ? (TControlScrollBar *) I2adlView->VertScrollBar
            : (TControlScrollBar *) I2adlView->HorzScrollBar;

        // Only snap this scroll if it is not currently being drug:
        int		Pos = SB->Position;

        if (Mouse->Capture == NULL || Pos == 0)
        {
            int		Offset = Pos % ViewScale;
            
            if (Offset)
            {
                SB->Position = Pos - Offset; // Round down to conform to grid snap
                Message.Msg = NULL; // Don't want default processing of Scroll event from here.
            }
        }

        if (CurrentSheet != NULL && Mouse->Capture == NULL)
            PaintAllNames();
    }

    // This procedure is an add-on; this message still needs to be processed:
    I2adlView->DoWndProc(Message);
}


// Application messaging; make sure that all menus on MainForm's main menu use this event handler:
void __fastcall TMainForm::MainFormMenuClick(TObject *Sender)
{
    if (TextI2adlMemo->Showing)
        TextI2adlMemo->HideMemo();

	// The following causes the wip tree to complete any node editing that may be in progress.
    //     It fixes a bug where Project/Sheet Save/SaveAs were using the original node names.
    if (WipTree->IsEditing())
    {
        I2adlView->SetFocus();
        Application->ProcessMessages();
    }
}


// Expand the tree view.
//     Handles ObjectTree and DataSetTree.
void __fastcall TMainForm::ExpandTreeViewClick(TObject *Sender)
{
    TTreeView	*TreeView = ActiveObjectTree;

    // Tell the system not to update tree until we are done:
    ObjectTreeUpdating = true;
    TreeView->Items->BeginUpdate();
    TreeView->FullExpand();
    TreeView->Items->EndUpdate();
    TreeView->Selected = TreeView->Items->GetFirstNode(); // Display the top of the object tree.
    ObjectTreeUpdating = false;

    // We prevented our system from Auto-sizing object tree repeatedly, so do it once now:
    AutoSizeTreeView(TreeView);
}


// Collapse the tree view.
//     Handles ObjectTree and DataSetTree.
void __fastcall TMainForm::CollapseTreeViewClick(TObject *Sender)
{
    TTreeView	*TreeView = ActiveObjectTree;

    // Tell the system not to update tree until we are done:
    ObjectTreeUpdating = true;
    TreeView->Items->BeginUpdate();
    TreeView->FullCollapse();
    TreeView->Items->EndUpdate();
    ObjectTreeUpdating = false;

    // We prevented our system from Auto-sizing object tree repeatedly, so do it once now:
    AutoSizeTreeView(TreeView);
}


// "Back" button
void __fastcall TMainForm::cmdBackClick(TObject *Sender)
{
    NavigateSheet(false);
}


// "Next" button
void __fastcall TMainForm::cmdNextClick(TObject *Sender)
{
    NavigateSheet(true);
}


void __fastcall TMainForm::cmdRedoClick(TObject *Sender)
{
    RevertSheetState(false);
}


void __fastcall TMainForm::cmdUndoClick(TObject *Sender)
{
    RevertSheetState(true);
}


void __fastcall TMainForm::ControlBar1BandMove(TObject *Sender, TControl *Control, TRect &ARect)
{
    ARect.Right = ControlBar1->BoundsRect.Right;
}


void __fastcall TMainForm::BringWidgetFormtoFrontClick(TObject *Sender)
{
	BringUIFormsToFront();
}


void __fastcall TMainForm::DSNameClick(TObject *Sender)
{
    LoadDSEditor(DSName->Text);
}


void __fastcall TMainForm::cmdObjectBrowserClick(TObject *Sender)
{
    ObjectBrowserForm->ShowModal();
}


void __fastcall TMainForm::cmdCOMFormDesignerClick(TObject *Sender)
{
    COMDesigner->Show();
}


void __fastcall TMainForm::ElementsListBoxKeyDown(TObject *Sender, WORD &Key, TShiftState Shift)
{
    if (Key == VK_DELETE)
    {
        if (ElementsListBox->SelCount > 0)
        {
            int last = 0;
            for (int i = 0; i < ElementsListBox->Items->Count; i++)
            {
                if (ElementsListBox->Selected[i])
                {
                    ElementsListBox->Items->Delete(i);
                    last = i;
                    i = -1;
                }
            }

            // Make sure that something is selected:
            last = (last >= ElementsListBox->Items->Count)
                 ? ElementsListBox->Items->Count - 1
                 : last;
            ElementsListBox->Selected[last] = true;
        }
    }
}


// Handles special keys on the data set tree.
//     <Enter> - Performs double click function
//     Delete  - Deletes the data set
void __fastcall TMainForm::DataSetTreeKeyDown(TObject *Sender, WORD &Key, TShiftState Shift)
{
    if (Key == VK_RETURN)
    {
        DataSetTreeDblClick(Sender);
    }
    else if (Key == VK_SPACE)
    {
        if (DataSetTree->Selected != NULL)
            ElementsListBox->Items->Add(DataSetTree->Selected->Text);
    }
    else if (Key == VK_DELETE)
    {
        // The user wants to delete a data set.
        if (!DataSetTree->IsEditing())
        	CanTreeNodeBeDeleted(DataSetTree->Selected, true);
    }
}


// Load the basic data set tree group names into the drop down list.
void __fastcall TMainForm::cmbTreeGroupDropDown(TObject *Sender)
{
    cmbTreeGroup->Items = DatasetTreeGroups;
}


void __fastcall TMainForm::cmdSynthesizeClick(TObject *Sender)
{
	if (cmdSynthesize->ImageIndex == 46)
	{
		CancelCompile = true;
		return;
	}

   	ClearMessageGridsAndLogFile(true);

    // If a behavior is currently running, then we must first stop it.
	if (IsRunning)
		HaltExecution();

	// Inform the user that the compile has stated.
	SetStatusCaption("Step 1 of 5; Compiler Setup...", true);

	// If we are compiling for EDIF Export, then inform the user that the system implementors
    //     will not be included.
	IsEDIFExport = SynthesizetoEDIF1->Checked;

    if (IsEDIFExport)
    	ErrorTrap(189);

    // It saves time to calculate this just once.
    MergeEquivObjects = SettingFlags & SETTINGS_MERGE_EQUIV_OBJECTS;

    // If the current sheet is empty, then can't synthesize it.
    if (CurrentSheet->Behavior->Count == 0)
    {
        ErrorTrap(2);
        HaltExecution();
        return;
    }

    CancelCompile = false;

    // Warn the user if there are objects on top of each other.
    AnsiString		ObjectLocations = CurrentSheet->Behavior->GetDuplicateObjectLocations();

    if (ObjectLocations != strNull)
    	ErrorTrap(179, ObjectLocations);

    // Warn the user if there is are unconnected transports.
    AnsiString	TransportLocations = CurrentSheet->Behavior->GetUnconnectedTransportLocs();

    if (TransportLocations != strNull)
    	ErrorTrap(171, TransportLocations);

    // If any primitive system object is missing it's Resource attribute, then don't start
    //		the compile.
	for (int i = 0; i < ProjectObjects->Count; i++)
    {
    	I2adl	*ProjectObject = (*ProjectObjects)[i];

        if (ProjectObject->IsPrimitive &&
        	ProjectObject->AttSystem != SystemstrNull &&
            ProjectObject->AttResource == ResourcestrNull)
        {
        	// Report the problem object.
	    	CancelCompile = true;

        	ErrorTrap(5, VStringList(ProjectObject->EnhancedName(), *ProjectObject->AttSystem));
        }
    }

    if (CancelCompile)
    {
        HaltExecution();
        return;
    }

    // If a ProjectObject has been changed in the Wip, then inform the user that the changes
    //		are not being used.
    for (int i = 0; i < MyProject->Wip->Count; i++)
    {
        I2adl	*WipObject = (*MyProject->Wip)[i];
        I2adl	*PObject = ProjectObjects->GetObject(WipObject->Behavior->CopyOf);

        // Did this Wip Sheet start from a project object?
        if (PObject == NULL)
            continue;

        // Changes to the current Sheet are used in the compile.
        if (WipObject == CurrentSheet)
            continue;

        // Has the ProjectObject been changed?
        if (WipObject->Behavior->Equals(PObject->Behavior))
            continue;

        // Warn the user that changes have not been saved.
        ErrorTrap	e(77, PObject->NameInfo->Name);

        e.Invoke();
        e.MessageInfo->IsDef = true;
        e.MessageInfo->I2adlObject = PObject;
    }

    ComputeDataSetLengths();

    // If the preference is set, then save the project at the start of the compile.
    if (MyProject->ProjectChanged &&
    	SettingFlags & SETTINGS_SAVE_BEFORE_COMPILE)
	        MainForm->SaveProject(true);

    // If this is a new project, then warn the user that a ".vex" file will not be created.
    if (CreateVexFile &&
    	MyProject->FilePath == strNull)
    		ErrorTrap(16);

    // Deactivate the previous compile information.
    if (MyProject->CompiledSheet != NULL)
    {
	    MyProject->CompiledSheet = NULL;

        if (!CreateVexFile)
		    MyProject->Modified();
    }

    // The timer is used during the compile step to display the "Expanding Object" messages.
    TimedEventType = etCompile;
    ProcessTimer->Enabled = true;

    // Compile the current sheet.
    bool	CompileWorked = false;

    try
    {
        CompileWorked = MainCompiler->Compile();
    }
    catch(Exception &e)
    {
        // Inform the user how to recover from errors while compiling.
        ErrorTrap(133, e.Message);
    }
    catch(bad_alloc &b)
    {
    	// We are out of memory.  Free some up so ErrorTrap() will work.
        HaltExecution();

        ErrorTrap(4067);
    }
    catch(...)
    {
        ErrorTrap(4068);
    }

    if (CompileWorked)
    {
        // Indicate compile completed by loading the last compiled sheet.
        TimedEventType = etTranslate;
        ProcessTimer->Enabled = true;
    }
    else
    {
        if (MyProject->Compiling)
        {
            // Compile failed.  Stop everything.
            MyProject->CompiledSheet = NULL;
            HaltExecution();
        }
    }
}


void __fastcall TMainForm::cmdOpenExecutableClick(TObject *Sender)
{
    OpenFile(ftCompiledProject);
}


void __fastcall TMainForm::cmdSaveExecutableClick(TObject *Sender)
{
    cmdSaveExecutableAsClick(NULL);
}


// Sender == NULL is a Save, not a Save As.
void __fastcall TMainForm::cmdSaveExecutableAsClick(TObject *Sender)
{
    // Use compiled project name, if existent; Default to Project name:
    AnsiString		FileName = MyProject->VexName;
    bool			Auto = (Sender == NULL &&
    				!(FileName == strNull && (SettingFlags & SETTINGS_SAVE_DEFAULT_NAME)));

    if (FileName == strNull)
        FileName = MakeFileName(MyProject->Name, Auto);
    else if (SettingFlags & SETTINGS_AUTO_VERSION && Auto)
        FileName = AutoVersion(FileName);

    // Project name if not set internally.
    SaveDialog->FileName = FileName;

    // Will only bring up the dialog if FileType != ftAuto:
	int		OutFileHandle = SaveFileDlg(ftCompiledProject, Auto);

    if (OutFileHandle < 0)
        return;

	VTextBase	saver(ftCompiledProject);

	saver.OutFileHandle = OutFileHandle;
    saver.OutFileName = SaveDialog->FileName;
    MyProject->VexName = saver.OutFileName;

    saver.WriteProject(false);
}


// added on 7/29/03 for build to edif cell only
//	this is a full menu option that uses the checkbox ability
void __fastcall TMainForm::SynthesizetoEDIF1Click(TObject *Sender)
{
	SynthesizetoEDIF1->Checked = !SynthesizetoEDIF1->Checked;
}


void __fastcall TMainForm::cmdPlayStopSheetClick(TObject *Sender)
{
	StartExecution();
}


void __fastcall TMainForm::MessageGridDrawCell(TObject *Sender, int ACol,
      int ARow, TRect &Rect, TGridDrawState State)
{
	TStringGrid		*grid = (TStringGrid *) Sender;

	if (grid->DefaultColWidth < grid->Width - 4)
		grid->DefaultColWidth = grid->Width - 4;

	VMessageInfo	*MessageInfo = (VMessageInfo *) grid->Objects[ACol][ARow];
	TColor			color = (MessageInfo != NULL)
    					  ? MessageInfo->MessageColor
                          : clBlack;

	if (State.Contains(gdSelected))
		grid->Canvas->Font->Color = clWhite;
    else
   		grid->Canvas->Font->Color = (TColor) color;

	grid->Canvas->TextOut(Rect.left + 2, Rect.top + 2, grid->Cells[ACol][ARow]);
}


// Clear all of the message grids and the ErrorLog.txt file.
//     If ReopenLogFile is true, then open (erase) the message log file.
void TMainForm::ClearMessageGridsAndLogFile(bool ReopenLogFile)
{
    ClearMessageGrid(MainForm->MessageGrid);
    ClearMessageGrid(MainForm->SearchGrid);
    ClearMessageGrid(MainForm->OverloadGrid);

    NonModalErrorCount = 0;

	if (ErrorFileHandle != -1)
    {
    	FileClose(ErrorFileHandle);
		ErrorFileHandle = -1;
    }

#ifdef LOG_EXPANDED_OBJECTS
	if (ExpandedObjectsLogFile != -1)
    {
    	FileClose(ExpandedObjectsLogFile);
		ExpandedObjectsLogFile = -1;
    }
#endif

	if (ReopenLogFile)
    {
        AnsiString	projectdir = "$(DirName:Project)";

	    AttCalculator->CalculateAttribute(projectdir);

		try
	    {
            ErrorFileHandle = FileCreate(projectdir + "ErrorLog.txt");
            FileClose(ErrorFileHandle);
            ErrorFileHandle = FileOpen(projectdir + "ErrorLog.txt", fmOpenWrite | fmShareDenyWrite);

#ifdef LOG_EXPANDED_OBJECTS
            ExpandedObjectsLogFile = FileCreate(projectdir + "ExpandedObjectLog.txt");
            FileClose(ExpandedObjectsLogFile);
            ExpandedObjectsLogFile = FileOpen(projectdir + "ExpandedObjectLog.txt", fmOpenWrite |
            	fmShareDenyWrite);
#endif
        }
        catch(...)
        {
            ErrorFileHandle = -1;

#ifdef LOG_EXPANDED_OBJECTS
            ExpandedObjectsLogFile = -1;
#endif
        }
    }
}


// Clear the message Grid, after deleting all of its messages. 
void TMainForm::ClearMessageGrid(TStringGrid *Grid)
{
    for (int i = 0; i < Grid->RowCount; i++)
    {
        Grid->Cells[0][i] = strNull;
        delete (VMessageInfo *) Grid->Objects[0][i];
        Grid->Objects[0][i] = NULL;
    }

    Grid->RowCount = 1;
}


// Display the current Wip Tree node in the I2adl Editor.
void __fastcall TMainForm::WipTreeClick(TObject *Sender)
{
	TTreeNode 	*Node = WipTree->Selected;

    if (Node != NULL &&
    	Node->Data != NULL &&				// Root node (project name)
        MyProject != NULL &&
        MyProject->ActiveWipNode != Node)	// Already in I2adl Editor
    {
    	SetCurrentSheet((I2adl *) Node->Data);
    }
}


void __fastcall TMainForm::WipTreeKeyPress(TObject *Sender, char &Key)
{
    if (Key == VK_RETURN || Key == VK_SPACE)
		WipTreeClick(Sender);
}


// This event handler is required to display the wip sheet that was right clicked on.
//     Without it the popup menu items will apply to the current sheet.
void __fastcall TMainForm::WipTreeContextPopup(TObject *Sender, TPoint &MousePos, bool &Handled)
{
	WipTreeClick(Sender);
}


void __fastcall TMainForm::OnAppActivate(TObject *Sender)
{
	if (IsRunning)
		BringUIFormsToFront();
}


void __fastcall TMainForm::cmdSaveProjectAsClick(TObject *Sender)
{
	SaveProject(false);
}


void __fastcall TMainForm::cmdSaveSheetClick(TObject *Sender)
{
	SaveSheet(false);
}


void __fastcall TMainForm::cmdSaveSheetAsClick(TObject *Sender)
{
	SaveSheet(true);
}


void __fastcall TMainForm::cmdSaveWODependenciesAsClick(TObject *Sender)
{
	SaveSheet(true, false);
}


void __fastcall TMainForm::MouseWheelHandler(Messages::TMessage &Message)
{
	TRect	rect = I2adlView->BoundsRect;

    if (PtInRect(&rect, ScreenToClient(Mouse->CursorPos)))
    {
    	int 	delta = (short(Message.WParamHi) * ViewScale) / 40;
        int		newY = I2adlView->VertScrollBar->Position - delta;

	    AlignToGrid(newY);

		int 	range = (I2adlView->VertScrollBar->Range - I2adlView->ClientRect.Height());

        newY = min(newY, int(range / ViewScale)	* ViewScale);

	    I2adlView->VertScrollBar->Position = newY;
    }
}


void TMainForm::WarnAboutDuplicateSystemNames()
{
	for (int i = 0; i < SystemTree->Items->Count; i++)
    {
		TTreeNode	*node1 = SystemTree->Items->Item[i];
    	AnsiString	name1 = node1->Text;

        if (node1->Level <= 0 || name1 == strNull)
            continue;

		for (int j = i + 1; j < SystemTree->Items->Count; j++)
        {
			TTreeNode	*node2 = SystemTree->Items->Item[j];
        	AnsiString	name2 = node2->Text;

            if (node2->Level <= 0 || name2 == strNull)
            	continue;

			if (name1 == name2)
            {
                VivaSystem	*sys1 = (VivaSystem *) node1->Data;
                VivaSystem	*sys2 = (VivaSystem *) node2->Data;

                if (sys1 != NULL && sys2 != NULL)
				{
                    AnsiString		part1 = sys1->Location;
                    AnsiString		part2 = sys2->Location;

                    if (part1 == strNull)
                    	part1 = name1;

                    if (part2 == strNull)
                    	part2 = name2;

	            	ErrorTrap(4065, part1 + " and " + part2);
                }
            }
        }
    }
}


void __fastcall TMainForm::cmdZoomInClick(TObject *Sender)
{
	Zoom(ViewScale + 1);
}


void __fastcall TMainForm::cmdZoomOutClick(TObject *Sender)
{
	Zoom(ViewScale - 1);
}


void __fastcall TMainForm::cmdFitToSizeClick(TObject *Sender)
{
	if (CurrentSheet->Behavior->Count > 0)
    {
		TRect		ExtentRect = ComputeCurrentSheetExtent();

        ExtentRect.Left = 0;
        ExtentRect.Top = 0;

        FitToSize(ExtentRect, false, true);
    }
}


void __fastcall TMainForm::cmdFitToWidthClick(TObject *Sender)
{
	if (CurrentSheet->Behavior->Count > 0)
    {
		TRect		ExtentRect = ComputeCurrentSheetExtent();

        ExtentRect.Left = 0;
        ExtentRect.Top = 0;

        FitToSize(ExtentRect, true, true);
    }
}



void TMainForm::FitToSize(TRect ExtentRect, bool WidthOnly, bool Readjust)
{
	double		XFactor = double(I2adlView->Width - SCROLLRANGEBUFFER)
    	/ (ExtentRect.Width());
	double		YFactor = double(I2adlView->Height - SCROLLRANGEBUFFER)
    	/ (ExtentRect.Height());
    double		MinFactor = (WidthOnly) ? XFactor
    									: min(XFactor, YFactor);
	int			NewScaleSize = MinFactor * double(ViewScale) ; //  + .5

	NewScaleSize = min(NewScaleSize, VIEW_SCALE_MAX);
    NewScaleSize = max(NewScaleSize, 1);

    I2adlView->HorzScrollBar->Position = 0;
    I2adlView->VertScrollBar->Position = 0;

	Zoom(NewScaleSize);

    TRect	newboundsrect = ComputeCurrentSheetExtent();

	if (Readjust)
    {
        while ((newboundsrect.Right * (NewScaleSize + 1) < I2adlView->Width * NewScaleSize
                - SCROLLRANGEBUFFER
                && (newboundsrect.Bottom * (NewScaleSize + 1) < I2adlView->Height
                * NewScaleSize - SCROLLRANGEBUFFER || WidthOnly))
            && NewScaleSize < VIEW_SCALE_MAX)
        {
            Zoom(++NewScaleSize);

            newboundsrect = ComputeCurrentSheetExtent();
        }

        while ((newboundsrect.Right > I2adlView->Width - SCROLLRANGEBUFFER
                || (newboundsrect.Bottom > I2adlView->Height - SCROLLRANGEBUFFER && !WidthOnly))
            && NewScaleSize > 1)
        {
            Zoom(--NewScaleSize);

            newboundsrect = ComputeCurrentSheetExtent();
        }
    }
}


void __fastcall TMainForm::cmdZoomDefaultClick(TObject *Sender)
{
	Zoom(VIEW_SCALE_DEFAULT);
}


void __fastcall TMainForm::cmdRenameClick(TObject *Sender)
{
	if (WipTree->Selected != NULL)
		WipTree->Selected->EditText();
}


void __fastcall TMainForm::ControlBar1MouseMove(TObject *Sender, TShiftState Shift, int X,
	int Y)
{
	// The following causes the wip tree to complete any node editing that may be in progress.
    //     It fixes a bug where Project/Sheet Save/SaveAs were using the original node names.
    if (WipTree->IsEditing())
    {
        I2adlView->SetFocus();
        Application->ProcessMessages();
    }
}


void __fastcall TMainForm::ObjectTreePageControlChange(TObject *Sender)
{
   	MyProject->BuildTreeView(false);

	ActiveTreeNode = ((TTreeView *) ObjectTreePageControl->ActivePage->Controls[0])->Selected;
}


void __fastcall TMainForm::ObjectTreeDragOver(TObject *Sender,
      TObject *Source, int X, int Y, TDragState State, bool &Accept)
{
    Accept = false;

    if (Source == WipTree)
    {
        // All wip tree nodes can be dropped on the object tree (convert sheet to object).
        Accept = true;
    }
    else if (Source == ComClassTree)
    {
        if (ComClassTree->Selected != NULL &&
        	ComClassTree->Selected->Level == 1)
        {
			TComObjectInfo	*ObjectInfo = ((TComObjectInfo *) ComClassTree->Selected->Data);

            if (ObjectInfo->TypeAttr->typekind != TKIND_ENUM)
                Accept = true;
        }
    }
    else if (Source == ProjectObjectTree ||
    		 Source == SystemObjectTree ||
    		 Source == ComObjectTree)
    {
        // Drop an object tree node on its own tree to change its tree group name.
        Accept = CanTreeNodeBeRenamed(ActiveTreeNode, rmByDragging);
    }
    else if (Source == DataSetTree)
    {
        // Drop a data set tree node on its own tree to change the tree group name.
        Accept = CanTreeNodeBeRenamed(DataSetTree->Selected, rmByDragging);
    }
}


// Handle double clicking on a message by locating the associated object (if any).
//		If the Ctrl key is down, then load all objects into the Back/Next list.
void __fastcall TMainForm::MessageGridDblClick(TObject *Sender)
{
	bool	LocateAllObjects = (GetKeyState(VK_CONTROL) < 0);

	MessageLocateObject(LocateAllObjects);
}


// Handle clicking on a LocateObject popup menu option by locating the associated object.
void __fastcall TMainForm::MessageLocateObjectClick(TObject *Sender)
{
	// The "All" option loads the Back/Next list.
	bool		LocateAllObjects = (Sender == MessageLocateObjectAll);
    VDismember	*Hierarchy = (VDismember *) ((TMenuItem *) Sender)->Tag;

	MessageLocateObject(LocateAllObjects, Hierarchy);
}


// If there is an I2adl object associated with the message that was double-clicked on, then
//		open the object's project object/wip behavior and select/center the object.
//		If Hierarchy != NULL, then the user selected one of the objects from the hiearachy
//		list on the popup menu.
void TMainForm::MessageLocateObject(bool LocateAllObjects, VDismember *Hierarchy)
{
	TStringGrid		*Grid = GetActiveMessageGrid();
	VMessageInfo	*messageinfo = (VMessageInfo *) Grid->Objects[Grid->Col][Grid->Row];

	if (messageinfo == NULL)
    	return;

	I2adl			*object = messageinfo->I2adlObject;
	VDismember      *parent = messageinfo->MemberOf;
	I2adl			*parentI2adl = NULL;

	if (object == NULL)
		return;

    if (Hierarchy != NULL)
    {
    	// Display the object selected from the popup menu.
		Hierarchy->GetValidObjects(&parentI2adl, &object);
    }
    else if (messageinfo->IsDef)
    {
		// Alluding to an object def, not ref (from Overloads list from Ctrl + Dbl Click).
	    //		Just open the object as a sheet; there is no object to select.
		parentI2adl = object;
        object = NULL;
    }
    else if (parent != NULL)
    {
        parentI2adl = parent->SourceI2adl;
    }
    else if (object->ParentI2adl != NULL)
    {
        parentI2adl = object->ParentI2adl;
    }

   	if (MyProject->Wip->IndexOf(parentI2adl) > -1)
    {
        if (CurrentSheet != parentI2adl)
			SetCurrentSheet(parentI2adl);
    }
    else
    {
        if (ProjectObjects->IndexOf(parentI2adl) <= -1)
        {
			// The Wip sheet has closed.
        	parentI2adl = NULL;
        }
        else
        {
            if (parentI2adl != NULL)
                LoadAsSheet(parentI2adl);

            // If we are locating all objects, then load the object's heirarchy into the
            //		back/next list.
            if (parent != NULL &&
            	LocateAllObjects)
            {
            	// Insert the hierarchy sheets infront of the sheet we just added.
                int		InsertPos = NavigationSheetList->Count - 1;

                // Put each level of the hierarchy into the prev/next list.
                while (parent != NULL)
                {
                	I2adl	*RecordedObject = (parent->MemberOf == NULL)
                    				  		? parent->TopLevelSheet
                                      		: parent->MemberOf->SourceI2adl;

                    // Non-iterative control allows breaking.
                    while (true)
                    {
						if (RecordedObject == NULL)
                        	break;

                    	// Find the corresponding project object and its Wip sheet (if open).
                        I2adl	*ProjObject = NULL;
                        I2adl	*WipObject = NULL;

                        if (ProjectObjects->IndexOf(RecordedObject) > -1)
                        {
                        	ProjObject = RecordedObject;
	                        WipObject = MyProject->Wip->GetObject(ProjObject->Behavior, true);
                        }
                        else if (MyProject->Wip->IndexOf(RecordedObject) > -1)
                        {
                        	WipObject = RecordedObject;
                        	ProjObject = ProjectObjects->GetObject(WipObject->Behavior->
                            	CopyOf);

                            // If we don't have a project object, then promote the wip object.
                            if (ProjObject == NULL)
                            {
                            	ProjObject = WipObject;
                                WipObject = NULL;
                            }
                        }
                        else
                        {
                        	break;
                        }

                        // Have we already selected/centered this object?
						int		CurrentPos = NavigationSheetList->IndexOf(ProjObject);

                        if (CurrentPos >= InsertPos)
                        	break;

                        // Was the object in the old back/next list?
                        if (CurrentPos >= 0)
                        {
                            NavigationSheetList->Delete(CurrentPos);
                            InsertPos--;
                        }

                        NavigationSheetList->Insert(InsertPos, ProjObject);

                        // Select and center the object in both behaviors.
                        //		Pass #1 - Project object
                        I2adl	*ParentObject = ProjObject;

                        for (int i = 0; i < 2; i++)
                        {
                            if (ParentObject != NULL)
                            {
                                // Find the closest matching object in the parent behavior.
                                I2adlList	*ParentBehavior = ParentObject->Behavior;
                                I2adl		*ExpandedObject = NULL;
                                bool		NameMatched = false;

                                for (int i = 0; i < ParentBehavior->Count; i++)
                                {
                                    I2adl	*NextObject = (*ParentBehavior)[i];

                                    // Unselect all of the other objects.
                                    NextObject->Selected = false;

                                    if (NextObject->I2adlShape != NULL)
                                        NextObject->I2adlShape->Selected = false;

                                    if (!NameMatched &&
                                        NextObject->XLoc == parent->XLoc &&
                                        NextObject->YLoc == parent->YLoc)
                                    {
                                        ExpandedObject = NextObject;

                                        if (ExpandedObject->NameInfo->Name ==
                                            *parent->MemberName)
                                                NameMatched = true;
                                    }
                                }

                                if (ExpandedObject != NULL)
                                {
                                    // We found the object.  Select and center it.
                                    ExpandedObject->Selected = true;

                                    if (ExpandedObject->I2adlShape != NULL)
                                        ExpandedObject->I2adlShape->Selected = true;

                                    float	ViewScaleFactor =
                                    		(ParentBehavior->LastViewScale > 0)
                                          ? float(ViewScale) / ParentBehavior->LastViewScale
                                          : 1.0;

                                    int	  newx = ExpandedObject->X + ExpandedObject->Width  / 2
                                        - I2adlView->Width  * ViewScaleFactor / 2;
                                    int	  newy = ExpandedObject->Y + ExpandedObject->Height / 2
                                        - I2adlView->Height * ViewScaleFactor / 2;

                                    ParentBehavior->LastHorzScroll = max(0, newx / ViewScale);
                                    ParentBehavior->LastVertScroll = max(0, newy / ViewScale);
                                }
                            }

                            // Pass #2 - Wip sheet.
                            ParentObject = WipObject;
                        }

                        break;
                    }

                    parent = parent->MemberOf;
                }
            }
        }
    }

    if (object != NULL)
    {
    	// If the exact object is on the current sheet, then use it.
        I2adl	*RealObject = (CurrentSheet->Behavior->IndexOf(object) > -1) ? object
        																	 : NULL;

        if (RealObject == NULL &&
        	messageinfo->Index > -1)
        {
        	// Get a list of the instances of this object and use the "Index" one.
	        I2adlList	*Instances = CurrentSheet->Behavior->FindInstances(messageinfo->
	        	SearchI2adl, messageinfo->SearchCriteria, parentI2adl, messageinfo->
                	SearchString);

			if (messageinfo->Index < Instances->Count)
	            RealObject = (*Instances)[messageinfo->Index];

        	delete Instances;
        }

        if (RealObject == NULL)
        {
            // As a last hope, return the object at the same (X, Y) location.
            for (int i = 0; i < CurrentSheet->Behavior->Count; i++)
            {
                I2adl	*CurrentSheetObject = (*CurrentSheet->Behavior)[i];

                if (CurrentSheetObject->X == object->X &&
                	CurrentSheetObject->Y == object->Y)
                {
                    RealObject = CurrentSheetObject;
                    break;
                }
            }
        }

        Deselect();

        if (RealObject != NULL &&
			RealObject->I2adlShape != NULL)
        {
            // Select and center the object.
            RealObject->I2adlShape->Selected = true;

            int		rangex = RealObject->X + RealObject->Width  / 2 + I2adlView->Width  / 2;
            int		rangey = RealObject->Y + RealObject->Height / 2 + I2adlView->Height / 2;
            int		newx   = RealObject->X + RealObject->Width  / 2 - I2adlView->Width  / 2;
            int		newy   = RealObject->Y + RealObject->Height / 2 - I2adlView->Height / 2;

            newx -= (newx % ViewScale);
            newy -= (newy % ViewScale);

            EnsureScrollRange(I2adlView->HorzScrollBar, rangex);
            EnsureScrollRange(I2adlView->VertScrollBar, rangey);

            I2adlView->HorzScrollBar->Position = max(0, newx);
            I2adlView->VertScrollBar->Position = max(0, newy);

            // The following is a easy/fast way to keep the object selected after pressing the
            //		Back button and then the Next button.  Not perfect, but close enough.
            I2adl	*ProjectObject = ProjectObjects->GetObject(CurrentSheet->Behavior->CopyOf);

            if (ProjectObject != NULL)
            {
            	int		ObjectNumber = CurrentSheet->Behavior->IndexOf(RealObject);

                if (ObjectNumber > -1 &&
                	ObjectNumber < ProjectObject->Behavior->Count)
                {
            		I2adl	*Object = (*ProjectObject->Behavior)[ObjectNumber];

                    if (Object->Equals(RealObject))
                    {
                        for (int i = 0; i < ProjectObject->Behavior->Count; i++)
                            (*ProjectObject->Behavior)[ObjectNumber]->Selected = false;

                    	Object->Selected = true;
                    }
                }
            }
        }

        RedrawSheet();
    }
}


// Performs I2adl JumpTo and text copy operations.
void __fastcall TMainForm::MessageGridKeyDown(TObject *Sender, WORD &Key, TShiftState Shift)
{
	if (Key == VK_RETURN)
    {
		MessageLocateObject(false);
    }
    else if (Shift.Contains(ssCtrl))
    {
    	// Copy the entire error message into the Windows clipboard.
    	if (Key == 'C' || Key == 'X' || Key == VK_INSERT)
			MessageCopyOneClick(NULL);
    }
}


// Finds all existing I2adl Refs for the Def selected in the Object Tree.
void TMainForm::FindI2adlRefs()
{
	if (ActiveTreeNode == NULL)
    	return;

	I2adl			  	*lookforobject = NULL;
    SearchCriteriaEnum 	SearchCriteria = scOverload;
    bool			  	found = false;
    I2adlList		  	*list = NULL;
    TComObject		  	*comobject = NULL;
    TMemberDesc       	*memberdesc = NULL;

    if (ObjectTreePageControl->ActivePage == ComTabSheet)
    {
    	// Setting 'comobject' or 'memberdesc', flags us for a different search.
        if (ActiveTreeNode->Level == 1)
        {
            comobject = (TComObject *) ActiveTreeNode->Data;
        }
        else if (ActiveTreeNode->Level == 2)
        {
            memberdesc = (TMemberDesc *) ActiveTreeNode->Data;
            comobject = (TComObject *) ActiveTreeNode->Parent->Data;
        }
        else
        {
        	return;
        }
    }
    else
    {
		lookforobject = (I2adl *) ActiveTreeNode->Data;;

        if (lookforobject == FOOTPRINT_TREE_NODE)
        {
            SearchCriteria = scFootprint;
            lookforobject = (I2adl *) ActiveTreeNode->getFirstChild()->Data;
        }
    }

    ClearMessageGrid(SearchGrid);

	// Actual search.  Two passes; one for Project Objects, one for WIP sheets.
    VList	*searchlist = ProjectObjects;

	for (int h = 0; h < 2; h++)
    {
        for (int i = 0; i < searchlist->Count; i++)
        {
            I2adl		*lookinobj = (I2adl *) searchlist->Items[i];
            I2adlList	*behavior = behavior = lookinobj->Behavior;

            if (behavior == NULL)
                continue;

            // In the case of COM functions and instances, we find Refs alluding to this
            //		function or instance, rather than search by name.  Why?  Name won't match.
            if (comobject != NULL || memberdesc != NULL)
            {
                list = new I2adlList(16);

                for (int i = 0; i < behavior->Count; i++)
                {
                    I2adl	*comp = (I2adl *) behavior->Items[i];
                    bool 	matches = false;

                    if (memberdesc != NULL)
                    {
                        if (comp->ExecuteType == EXECUTE_COM)
                            if (comp->ComHelper->MemberDesc->Name == memberdesc->Name
                                && comp->ComHelper->OwningObject == comobject)
                                matches = true;
                    }
                    else if (comobject != NULL)
                    {
                        if (comp->ExecuteType == NAME_DISPATCH && comp->ComObject == comobject)
                            matches = true;
                    }

                    if (matches)
                    {
                        if (lookinobj != NULL)
	                        comp->MemberOf = MainCompiler->ObjectTrace(lookinobj, false);

                        list->Add(comp);
                    }
                }
            }
            else
            {
                list = behavior->FindInstances(lookforobject, SearchCriteria, lookinobj);
            }

            // Now that we have found any existing instances, we get to iterate thru the
            //		list of results, generating an "error trap" for each of them, associating
            //		the I2adl object itself with the message.
            for (int i = 0; i < list->Count; i++)
            {
                I2adl			*comp = (I2adl *) list->Items[i];
                I2adl			*searchobject = (comobject != NULL)
                                              ? comp
                                              : lookforobject;
                I2adl			*inwipsheet = (h == 1) ? lookinobj
                									   : NULL;
                AnsiString		message(comp->QualifiedName() + lookinobj->ExternalName());

                if (inwipsheet != NULL)
                	message += " in WIP sheet \"" + inwipsheet->NameInfo->Name + "\"";

                ErrorTrap		e(6000, message);
                VMessageInfo    *minfo = new VMessageInfo(comp, searchobject, SearchCriteria, i);

                e.MessageInfo = minfo;
                found = true;
                comp->MemberOf = NULL;
            }

            delete list;
        }

	    searchlist = MyProject->Wip;
    }

    if (!found)
    {
		if (memberdesc != NULL)
		    ErrorTrap(6000, "No instances found of " + comobject->Name + strPeriod
            + memberdesc->Name);
		else if (comobject != NULL)
	        ErrorTrap(6000, "No instances found of " + comobject->Name);
        else
	        ErrorTrap(6000, "No instances found of " + lookforobject->QualifiedName());
    }
}


// Similar enough to the parameter-less function, except that is searches on the string passed,
//		rather than on the node selected in the Object Tree.
void TMainForm::FindI2adlRefs(AnsiString SearchString, bool AllowWildCardSearch)
{
    if (SearchString == strNull)
		return;

    ClearMessageGrid(SearchGrid);

	bool	found = false;

    VList	*searchlist = ProjectObjects;

	for (int h = 0; h < 2; h++)
    {
        for (int i = 0; i < searchlist->Count; i++)
        {
            I2adl		*lookinobj = (I2adl *) searchlist->Items[i];
            I2adlList	*behavior = lookinobj->Behavior;

            if (behavior == NULL)
                continue;

		    TStringList		*NamePieces = ParseString(SearchString, "*", false, false);
            bool			ValidPattern = false;

            if (AllowWildCardSearch)
            {
                // Do not allow every single object to be selected.
                for (int j = 0; j < NamePieces->Count; j++)
                {
                    AnsiString	NamePiece = NamePieces->Strings[j];

                    if (NamePiece == strNull)
                        continue;

                    ValidPattern = true;

                    // Fixup trailing "*" character.
                    if (SearchString[SearchString.Length()] == '*')
                        NamePieces->Add(strNull);

                    break;
                }

                // If no wildcard character ("*"), then just do a normal search.
                if (NamePieces->Count <= 1)
                    ValidPattern = false;
            }

            I2adlList	*list = (ValidPattern)
            	? behavior->FindInstances(NULL, scName, lookinobj, SearchString, NamePieces)
	            : behavior->FindInstances(NULL, scName, lookinobj, SearchString, NULL);

            delete NamePieces;

            // Display the matching objects.
            for (int i = 0; i < list->Count; i++)
            {
                I2adl			*comp = (I2adl *) list->Items[i];
                I2adl			*searchobject = NULL;
                I2adl			*inwipsheet = (h == 1) ? lookinobj
                									   : NULL;
                AnsiString		message(comp->QualifiedName() + lookinobj->ExternalName());

                if (inwipsheet != NULL)
                	message += " in WIP sheet \"" + inwipsheet->NameInfo->Name + "\"";

                ErrorTrap		e(6000, message);
                VMessageInfo    *minfo = new VMessageInfo(comp, searchobject, scName, i,
                	SearchString);

                e.MessageInfo = minfo;
                found = true;
                comp->MemberOf = NULL;
            }

            delete list;
        }

		searchlist = MyProject->Wip;        
    }

    if (!found)
        ErrorTrap(6000, "No instances found of \"" + SearchString + "\"");
}


// AutoVersion control.
//		If FileName contains both alphabetic and numeric characters, then increment it.
//		Otherwise, append a version number.
AnsiString TMainForm::AutoVersion(AnsiString FileName)
{
    char	*fname  = FileName.c_str();
    int		StartScan = 0;
    int		EndScan;
    int		DotScan = 0;

    for (int i = 0; ; i++)
    {
        char	chr = fname[i];

        if (chr == '.')
        {
            DotScan = i;
        }
        else if (chr == ':' || chr == '\\')
        {
            StartScan = i + 1;
            DotScan = 0;
        }
        else if (chr == 0)
        {
            EndScan = i;

            if (DotScan == 0)
                DotScan = i;

            break;
        }
    }

    bool	HasLetters = false;
    bool	HasNumbers = false;
    char	echr;

    for (int i = StartScan; i < DotScan; i++)
    {
        echr = fname[i];

        if (echr >= 'A')
        {   // See if letter.. & 0x5f converts lower to upper case
            if ( (echr & 0x5f) <= 'Z' )
            	HasLetters = true;
        }
        else if (echr >= '0' && echr <= '9')
        {
        	HasNumbers = true;
        }
    }

    int		Numbering = -1;

    if (HasLetters & HasNumbers) // Non-iterative
    {    // break if we cannot increment file name
        // Try to increment the file name.

        AnsiString		NewName = FileName;

        for (int i = DotScan-1; i >= StartScan; i--)
        {
            char	chr = fname[i];

            if (chr == '9')
            {
                Numbering = i;
                NewName[i + 1] = '0';
                continue;
            }

            if (chr < '9')
            {    // accept
                if (chr < '0')
                    break;
            }
            else if (Numbering >= 0)
            {
                 break;
            }
            else if (chr >= 'A')
            {
                 char	uchr = (char)(chr & 0x5f);

                 if (uchr >= 'Z')
                 {
                     if (uchr > 'Z')
                         break;
                     NewName[i + 1] = (char) (chr - 25);
                     continue;
                 }
            }
            else
            {
                break;
            }

            // accept
            NewName[i + 1] = (char) (chr + 1);
            return NewName;
        }

        if (Numbering >= 0)
            FileName = NewName;
    }

    if (Numbering >= 0)
    {
        echr = '1';
        DotScan = Numbering;

    }
    else if (echr <= '9')
        echr = 'a';
    else
        echr = '1';

    // Extend the file name
    if (DotScan >= EndScan)
        FileName += echr;
    else
        FileName = FileName.SubString(1,DotScan) + echr
            + FileName.SubString(DotScan + 1, EndScan - DotScan);

    return FileName;
}


void __fastcall TMainForm::cmdSearchClick(TObject *Sender)
{
	FindI2adlRefs();
}


// Try to display the object documentation for the active tree node.
void __fastcall TMainForm::ObjectDocumentationClick(TObject *Sender)
{
    GetI2adlObject(ActiveTreeNode)->ShowHelpFile();
}


// Invoked by shortcut Ctrl + I.  Simply does the same thing as cmdSearchClick, after it has
//		validated the selected Object Tree node.       
void __fastcall TMainForm::cmdSearchForInstancesClick(TObject *Sender)
{
    ObjectTreeMenuPopup(NULL);

    if (cmdSearch->Enabled)
        cmdSearchClick(NULL);
    else
    	ErrorTrap(4086);
}


// Command to search for I2adl Refs by a string that the user enters on the spot.  Nuff said.
void __fastcall TMainForm::cmdSearchByNameClick(TObject *Sender)
{
	// Default to the last search string.
    static	AnsiString	SearchString = strNull;

	SearchString = InputBox("Search by Name",
    	"Search is case-sensitive.  Wild card character is \"*\".", SearchString);

	FindI2adlRefs(SearchString, true);
}


// Invokes a search on the name of the object selected in the Object Tree.
//		As of 03/12/05, is still case-sensitive.
void __fastcall TMainForm::cmdSearchOnNameClick(TObject *Sender)
{
	// If a footprint tree node is selected, then search on the name of the first child.
	I2adl	*Object = GetI2adlObject(ActiveTreeNode);

    if (Object != NULL)
	   	FindI2adlRefs(Object->NameInfo->Name, false);
}


// Update WIP sheets to have their name set to the caption of the corresponding WIP tree node.
void __fastcall TMainForm::WipTreeChange(TObject *Sender, TTreeNode *Node)
{
    if (Node != NULL && Node->Data != NULL)
    	((I2adl *) Node->Data)->SetName(Node->Text);

	WipTreeClick(Sender);
}


// Update WIP sheets to have their name set to the caption of the corresponding WIP tree node.
//		Used this event handler, cuz it's the only one which always effects.
//		Also in WipTreeEdited to get the name into CurrentSheet faster.
void __fastcall TMainForm::WipTreeCustomDrawItem(TCustomTreeView *Sender,
      TTreeNode *Node, TCustomDrawState State, bool &DefaultDraw)
{
    if (Node != NULL && Node->Data != NULL)
	    ((I2adl *) Node->Data)->SetName(Node->Text);
}


void __fastcall TMainForm::cmdOpenLibraryClick(TObject *Sender)
{
	OpenFile(ftLibrary);
}


void __fastcall TMainForm::cmdProjectAttributesClick(TObject *Sender)
{
	ProjectSettingsDialog->ShowModal();
}


void TMainForm::StatusMessage(AnsiString Message, bool SetStatusBar)
{
	ErrorTrap(8000, Message);

	if (SetStatusBar)
		SetStatusCaption(Message, true);
    else
		SetStatusCaption(strNull, true);
}


void TMainForm::EnableDisableControls(bool Enable)
{
#if VIVA_IDE
    FControlsEnabled = Enable;

    if (!FControlsEnabled)
	    Application->OnMessage = DisabledControlEventHandler;
    else
		Application->OnMessage = NULL;
#endif
}


// Returns true if TreeNode can be deleted.  If PerformDelete is true, then the tree node
//		and the associated objects are deleted.  Also, the popup menu option "DeleteObject"
//		will have its caption loaded for non object tree nodes.
bool TMainForm::CanTreeNodeBeDeleted(TTreeNode *TreeNode, bool PerformDelete)
{
    if (TreeNode == NULL)
    	return false;

    TTreeView	*ObjectTree = (TTreeView *) TreeNode->TreeView;

    // Data Set tree:
    if (ObjectTree == DataSetTree)
    {
    	DeleteObject->Caption = "&Delete Data Set";

    	if (TreeNode->Data == TREE_GROUP_NODE)
            return false;

        DataSet    *DataSetToDelete = (DataSet *) TreeNode->Data;

        // System generated data sets cannot be deleted.
        if (DataSetToDelete->SystemGenerated != sgNot)
        {
        	if (PerformDelete)
            	ErrorTrap(86);

            return false;
        }

        for (int i = 0; i < ProjectDataSets->Count; i++)
        {
            DataSet    *ProjectDataSet = (DataSet *) ProjectDataSets->Items[i];

            // Has the data set been used as a child data set?
            for (int j = 0; j < ProjectDataSet->ChildDataSets->Count; j++)
            {
                if (DataSetToDelete == (DataSet *) ProjectDataSet->ChildDataSets->Items[j])
                {
		        	if (PerformDelete)
                    	ErrorTrap(6, ProjectDataSet->Name);

                    return false;
                }
            }
        }

        // Has the data set been used?
        if (DataSetToDelete->IsUsed(PerformDelete))
            return false;

        if (PerformDelete)
		{
            // Delete the dataset and its exposer/collector.
            DataSetToDelete->Exposer->DeleteProjectObject();
            DataSetToDelete->Collector->DeleteProjectObject();

            ProjectDataSets->Remove(DataSetToDelete);
            delete DataSetToDelete;

            // The following prevents an access violation if the deleted data set is in the
            //		I2adl Editor clipboard.
			ClipBoardTimeStamp = 0;

            MyProject->BuildTreeView();
        }

        return true;
    }

    // Project/System object trees:
    if (ObjectTree == ProjectObjectTree ||
    	ObjectTree == SystemObjectTree)
	{
#if VIVA_SD == false
        // Only VivaSD.exe can delete system objects.
        if (ObjectTree == SystemObjectTree)
        {
        	if (PerformDelete)
	            ErrorTrap(4094);

            return false;
        }
#endif

        I2adl	*Object = (I2adl *) TreeNode->Data;

        if (Object == TREE_GROUP_NODE)
        {
            DeleteObject->Caption = "&Delete Tree Group";

            // Can only delete "Composite Object" tree group nodes.
            if (!InsideRootTreeNode(strCompositeObjects, TreeNode, true))
            {
	        	if (PerformDelete)
                	ErrorTrap(94);

                return false;
            }
        }
        else if (TreeNode->ImageIndex == LIBRARY_NODE_IMAGE_INDEX)
        {
            DeleteObject->Caption = "&Delete Library";

        	if (PerformDelete)
            {
                // Delete the AnsiString library name.
                ObjectTree->Items->Delete(TreeNode);
                MyProject->Libraries->Remove((AnsiString *) Object);
                MyProject->ReloadLibraries();

                delete (AnsiString *) Object;

		        MyProject->Modified();
            }

            return true;
        }
        else if (TreeNode->ImageIndex == SUBSYSTEM_NODE_IMAGE_INDEX)
        {
            // Must use the System Editor to delete subsystems.
        	if (PerformDelete)
	            ErrorTrap(95);

            return false;
        }
        else if (InsideRootTreeNode(strLibraries, TreeNode))
        {
        	// Object libraries cannot be changed.
        	if (PerformDelete)
	            ErrorTrap(4093);

            return false;
        }
        else if (Object == FOOTPRINT_TREE_NODE)
        {
            // Footprint Tree Nodes can be deleted (if the user elects to delete all of the
            //		child objects).
            DeleteObject->Caption = "&Delete Footprint";
        }
        else
        {
            // Cannot delete the system generated exposers and collectors.
            if (Object->ExposerCode != EXPOSER_NOT)
            {
	        	if (PerformDelete)
    	            ErrorTrap(96);

                return false;
            }

            // Cannot delete the library primitive objects.
            if (Object->OwningLib != NULL)
            {
                if (PerformDelete)
                    ErrorTrap(4093);

                return false;
            }

            if (PerformDelete)
            {
            	if (!Object->MayObjectBeDeleted())
                	return false;

                // Delete this object from the project/system object lists.
                BaseSystem->RemoveObject(Object);
		        MyProject->Modified(Object->IsSystemObject);

                I2adlList	*Behavior = Object->Behavior;

                Object->DeleteProjectObject();
                RemoveBehavior(Behavior);
                Object = NULL;
            }
        }

        if (!PerformDelete)
           	return true;

        // If we are deleting more than one object, then confirm the mass delete.
        if (TreeNode->HasChildren)
        {
            int		OkToDelete = ErrorTrap(93);

            if (OkToDelete != mrYes)
            {
                // Do not allow the warning message to be turned off on the Tree Group level.
                if (OkToDelete != mrOk || Object == TREE_GROUP_NODE)
                    return true;
            }

            // Start at the first child node and delete the child I2adl objects.
            TTreeNode	*ChildTreeNode = TreeNode->GetNext();

            Screen->Cursor = crHourGlass;

            while (ChildTreeNode != NULL && ChildTreeNode->Level > TreeNode->Level)
            {
                if (!ChildTreeNode->HasChildren)
                {
                    // Delete the I2adl object in the terminal leaf node.
                    I2adl    *Object = (I2adl *) ChildTreeNode->Data;

                    BaseSystem->RemoveObject(Object);
					MyProject->Modified(Object->IsSystemObject);

                    I2adlList	*Behavior = Object->Behavior;

                    Object->DeleteProjectObject();
                    RemoveBehavior(Behavior);
                }

                ChildTreeNode = ChildTreeNode->GetNext();
            }

            Screen->Cursor = crDefault;
        }

        // Delete the node (and any child nodes) from the tree view.
        // This deletion may obsolete a parent node, in which case it becomes a memory access
        //      hazard.  If this node's parent is a footprint tree node and would no longer
        //      have children after this deletion, then remove it also:
        TTreeNode	*Selected = TreeNode;

        while (Selected->Parent != NULL &&
        	   Selected->Parent->Count == 1 &&
               Selected->Parent->Level > 0)
        {
            Selected = Selected->Parent;
        }

        ObjectTree->Items->Delete(Selected);

        RebuildUI();
        MyProject->Modified();

        return true;
    }

    // COM object tree:
    if (ObjectTree == ComObjectTree)
    {
        if (TreeNode->Level > 0 &&
        	TreeNode->Parent->Data == COM_GLOBALS_ROOT_NODE &&
           !IsDependency(TComResource((TComObject *) TreeNode->Data)))
        {
            if (PerformDelete)
            {
            	int			PrefixLen = strCannotLoad.Length();
                TComObject	*ComObject = (TComObject *) TreeNode->Data;
                AnsiString	ObjectName = (ComObject == NULL)
                					   ? TreeNode->Text.SubString(PrefixLen + 1, TreeNode->
                                       		Text.Length() - PrefixLen)
                					   : ComObject->Name;

                if (ErrorTrap(4025, ObjectName) == mrYes)
                {
                	if (ComObject == NULL)
                    {
                		GlobalComObjectStubs->Delete(GlobalComObjectStubs->IndexOf(ObjectName));
                    }
                    else
                    {
                    	GlobalComObjects->Remove(ComObject);
                    	delete ComObject;
                    }

                    ObjectTree->Items->Delete(TreeNode);

                    MyProject->Modified();
                }
            }

            return true;
        }
    }

    return false;
}


// Returns true if the TreeNode can be renamed by the indicated method.
bool TMainForm::CanTreeNodeBeRenamed(TTreeNode *TreeNode, RenameMethodEnum RenameMethod)
{
    if (TreeNode == NULL)
    	return false;

    TTreeView	*ObjectTree = (TTreeView *) TreeNode->TreeView;

    // Data Set tree:
    if (ObjectTree == DataSetTree)
    {
        DataSet		*DSet = (DataSet *) TreeNode->Data;

        if (DSet == TREE_GROUP_NODE)
        {
            if (InsideRootTreeNode(strBasicDataSets, TreeNode, true))
                return true;
        }
        else
        {
        	if (RenameMethod == rmByClickTwice)
            	return false;

            return (DSet->SystemGenerated == sgNot);
        }
    }

    // Project/System object trees:
    if (ObjectTree == ProjectObjectTree ||
    	ObjectTree == SystemObjectTree)
	{
        I2adl	*Object = (I2adl *) TreeNode->Data;

#if VIVA_SD == false
        // Only VivaSD can change/delete system objects.
        if (ObjectTree == SystemObjectTree)
            return false;
#endif

        // Can only rename "Composite Object" tree group nodes.
        if (Object == TREE_GROUP_NODE)
            return InsideRootTreeNode(strCompositeObjects, TreeNode, true);

        // The following allows Libraries to be reordered.
        if (RenameMethod == rmByDragging &&
			TreeNode->ImageIndex == LIBRARY_NODE_IMAGE_INDEX)
	            return true;

        // The following prevents I2adl objects from being renamed.
        if (RenameMethod == rmByClickTwice)
            return false;

        return InsideRootTreeNode(strCompositeObjects, TreeNode);
    }

    // COM object tree:
    if (ObjectTree == ComObjectTree)
    {
    	// Cannot drag a node or use the pop up menu on the COM object tree.
    	if (RenameMethod == rmByClickTwice)
            return (TreeNode->Level == 1);
    }

    return false;
}


// Returns the I2adl object associated with the TreeNode as follows:
//		If TreeNode is for an object, then the object is returned.
//		If TreeNode is a footprint, then the first child node's object is returned.
//		Otherwise, NULL is returned.
I2adl *TMainForm::GetI2adlObject(TTreeNode *TreeNode, bool AllowFootprint)
{
    if (TreeNode == NULL)
    	return NULL;

    TTreeView	*ObjectTree = (TTreeView *) TreeNode->TreeView;

    // Project/System object trees:
    if (ObjectTree == ProjectObjectTree ||
    	ObjectTree == SystemObjectTree)
	{
        // Does this tree node pertain to an I2adl object?
        I2adl	*Object = (I2adl *) TreeNode->Data;

        if (Object != TREE_GROUP_NODE &&
            TreeNode->ImageIndex != LIBRARY_NODE_IMAGE_INDEX &&
            TreeNode->ImageIndex != SUBSYSTEM_NODE_IMAGE_INDEX)
        {
            // If this is a footprint tree node, then get a child node.
            if (Object == FOOTPRINT_TREE_NODE)
            {
            	// Unless footprints are not allowed.
            	if (!AllowFootprint)
                	return NULL;

                TTreeNode	*ObjectTreeNode = TreeNode->getFirstChild();

                if (ObjectTreeNode == NULL)
                    Object = NULL;
                else
                    Object = (I2adl *) ObjectTreeNode->Data;
            }

            return Object;
        }
    }

    return NULL;
}


// Returns "true" if the resource data is authorized for this version of Viva.
bool TMainForm::IsResourceDataAuthorized(TStringList *ResourceLines)
{
    // Are all files authorized?
    if (LICENSE_SD == strNull)
        return true;

    // These magic numbers create a non-standard (secure) checksum calculation.
    //		Note:  Intel byte ordering has the low 6 bytes in the front.
    int			FileSize = ResourceLines->Text.Length();
    __int64		CheckSum = 0;
    __int64		IntBuffer = 0;

    for (int i = 21; i < FileSize; i += 6)
    {
        memcpy(&IntBuffer, &ResourceLines->Text[i - 5], 6);

        CheckSum += IntBuffer;
    }

    // Everyone gets ResourceData.txt.
    if (FileSize == 27256 &&
        CheckSum == 277762164417078005)
            return true;

    // Everyone gets ResourceData_X86sd.txt.
    if (FileSize == 429 &&
        CheckSum == 4563500642006962)
            return true;

    // Can we use ResourceDataHC.txt?
    if (FileSize == 8367 &&
        CheckSum == 85614590949913798)
    {
	    if (LICENSE_SD.Pos(strBenOne) > 0)
        	return true;
    }

    // Can we use XSK's ResourceData.txt?
    if (FileSize == 2048 &&
        CheckSum == 20491418030792228)
    {
	    if (LICENSE_SD.Pos(strXSK) > 0)
        	return true;
    }

	return false;
}


// A new System Tree node has been selected.
//		Do not allow the system to be edited if Viva.exe or the base system.
void __fastcall TMainForm::SystemTreeChange(TObject *Sender, TTreeNode *Node)
{
    Location->ReadOnly = true;
	SystemTree->ReadOnly = true;
    SysNameEdit->ReadOnly = true;
	WarnedSystemTreeDragOver = false;

#if VIVA_SD
	if (SystemTree->Selected != NULL &&
    	SystemTree->Selected->Data != BaseSystem)
    {
        SystemTree->ReadOnly = false;
        SysNameEdit->ReadOnly = false;
    }
#endif
}


// Display the Object Reference Guide.
void __fastcall TMainForm::ObjectReferenceClick(TObject *Sender)
{
	AnsiString	ObjectReferenceGuide = strHelpDir + "Viva_Object_Reference_Guide.htm";

    if (FileExists(ObjectReferenceGuide))
        ShellExecute(Handle, NULL, ObjectReferenceGuide.c_str(), "", "", SW_SHOWNORMAL);
    else
	    ErrorTrap(3044, ObjectReferenceGuide);
}


void __fastcall TMainForm::FormCanResize(TObject *Sender, int &NewWidth, int &NewHeight,
	bool &Resize)
{
	if (NewWidth < Width &&
		MainPanel->Width < 200)
        	Resize = false;

	if (NewHeight < Height &&
		MainEditorsControl->Height < 200)
        	Resize = false;
}


void __fastcall TMainForm::SaveSystemWODependenciesAsClick(TObject *Sender)
{
	SaveSystem(false);
}


// Prepare the message grid popup menu by hiding the invalid menu options and loading the
//		object hierarchy (if any).
void __fastcall TMainForm::MessageMenuPopup(TObject *Sender)
{
	TStringGrid		*Grid = GetActiveMessageGrid();
	VMessageInfo	*MessageInfo = (VMessageInfo *) Grid->Objects[Grid->Col][Grid->Row];

    // Reset the menu options.
	MessageExternalHelp->Visible = (Grid == MessageGrid);
    MessageLocateObjectSep->Visible = false;
    MessageLocateObjectAll->Visible = false;
    MessageLocateObject01->Visible = false;

    for (int i = 0; i < LocateObjects->Count; i++)
    {
    	TMenuItem	*LocateObject = (TMenuItem *) LocateObjects->Items[i];

        if (LocateObject->Visible)
			LocateObject->Visible = false;
        else
        	break;
    }

    // Is there an object associated with the message?
    if (MessageInfo == NULL ||
		MessageInfo->I2adlObject == NULL)
        	return;

    // If this is the Search/Overloads tab, then things are really simple.
    MessageLocateObjectSep->Visible = true;
    MessageLocateObject01->Visible = true;
    MessageLocateObject01->Caption = OrigLocateObjectCaption;

    if (Grid != MessageGrid)
    	return;

    // Load the first hierarchy menu option.
    MessageLocateObjectAll->Visible = true;
    MessageLocateObject01->Caption = "Locate " + MessageInfo->I2adlObject->QualifiedName(true);

    VDismember	*Parent = MessageInfo->MemberOf;
    int			LevelCount = 0;

    if (Parent != NULL)
        MessageLocateObject01->Caption += " of " + *Parent->MemberName;

    // Load a menu option for each level in the object hierarchy.
    while (Parent != NULL &&
    	   LevelCount < 100)
    {
    	// Skip duplicate entries.
		if (!Parent->Equals(Parent->MemberOf))
        {
            I2adl	*ChildObject = NULL;
            I2adl	*ParentObject = NULL;

            Parent->GetValidObjects(&ParentObject, &ChildObject);

            if (ParentObject != NULL)
            {
            	// If all of the existing menu items have been used, then create another one.
                TMenuItem	*LocateObject = NULL;

                if (LevelCount < LocateObjects->Count)
                {
                    LocateObject = (TMenuItem *) LocateObjects->Items[LevelCount];
                }
                else
                {
                    LocateObject = new TMenuItem(MessageMenu);
                    LocateObject->OnClick = MessageLocateObjectClick;

                    LocateObjects->Add(LocateObject);
                    MessageMenu->Items->Add(LocateObject);
                }

                LevelCount++;
                LocateObject->Visible = true;
                LocateObject->Tag = (int) Parent;
                LocateObject->Caption = "Locate " + Parent->QualifiedName() + " of " +
                    ParentObject->NameInfo->Name;
            }
        }

        Parent = Parent->MemberOf;
    }
}


// Display the external help file for this message.
void __fastcall TMainForm::MessageExternalHelpClick(TObject *Sender)
{
	DisplayExternalHelpFile();
}


// Display the external documentation file for the active message (if any).
void TMainForm::DisplayExternalHelpFile()
{
    AnsiString	Message = MessageGrid->Cells[MessageGrid->Col][MessageGrid->Row];

    // Guard against the last cell (blank).
    if (Message.Length() >= 12)
    {
	    int			MessageNumber = 0;
        
        for (int i = 9; i <= 12; i++)
        {
            char	MessageChar = Message[i];

            if (MessageChar < '0' ||
                MessageChar > '9')
                    break;

            MessageNumber = MessageNumber * 10 + MessageChar - '0';
        }

        if (MessageNumber > 0)
        {
            AnsiString	HelpFileName = "Message" + IntToStr(MessageNumber + 10000).
                SubString(2, 4);

            ShowHelpFile(HelpFileName, false);
        }
    }
}


// Copy the active message into the clipboard.
void __fastcall TMainForm::MessageCopyOneClick(TObject *Sender)
{
	TStringGrid		*Grid = GetActiveMessageGrid();

    // Protect against an empty grid.
    if (Grid->Col >= 0 &&
    	Grid->Row >= 0)
    {
        AnsiString		Message = Grid->Cells[Grid->Col][Grid->Row];

        // If no message is selected, then don't erase the clipboard.
        if (Message != strNull)
            Clipboard()->AsText = Message;
    }
}


// Copy all of the messages into the clipboard.
void __fastcall TMainForm::MessageCopyAllClick(TObject *Sender)
{
	TStringGrid		*Grid = GetActiveMessageGrid();
    TStringList		*Messages = new TStringList;

    for (int i = 0; i < Grid->RowCount; i++)
    	Messages->Add(Grid->Cells[0][i]);

    // If there aren't any messages, then don't erase the clipboard.
    if (Messages->Text != strNull &&
    	Messages->Text != "\r\n")
	    	Clipboard()->AsText = Messages->Text;

	delete Messages;
}


// Clear all of the messages.
void __fastcall TMainForm::MessageClearMessagesClick(TObject *Sender)
{
	ClearMessageGridsAndLogFile(true);
}


// This is required because right clicking on a message does not select it.
void __fastcall TMainForm::MessageGridMouseDown(TObject *Sender, TMouseButton Button,
	TShiftState Shift, int X, int Y)
{
    if (Button == mbRight)
    {
		TStringGrid		*Grid = GetActiveMessageGrid();
        int				Col;
        int				Row;

        Grid->MouseToCell(X, Y, Col, Row);

        // Guard against the mouse not being over a cell.
        if (Col >= 0)
	        Grid->Col = Col;

        if (Row >= 0)
	        Grid->Row = Row;
    }
}


TStringGrid *TMainForm::GetActiveMessageGrid()
{
	if (MessageTabControl->ActivePage == SearchTabSheet)
		return SearchGrid;
    else if (MessageTabControl->ActivePage == OverloadTabSheet)
    	return OverloadGrid;
    else
    	return MessageGrid;
}


// Show/hide the object attributes.
void __fastcall TMainForm::ShowObjectAttributesClick(TObject *Sender)
{
    ShowingObjectAttributes = !ShowObjectAttributes->Checked;

    MenuBtnSync(ShowObjectAttributes, true);

    // The following fixes Bugzilla entry #766 & #769.
    ResizeToolBar();

    I2adlView->Invalidate();
    I2adlView->Update();
}

