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

#ifndef MainH
#define MainH

#include <ImgList.hpp>
#include <Dialogs.hpp>
#include <Grids.hpp>
#include <ToolWin.hpp>
#include <StdCtrls.hpp>
#include <Menus.hpp>
#include <ExtCtrls.hpp>
#include <Controls.hpp>
#include <ComCtrls.hpp>
#include <Classes.hpp>
#include <Buttons.hpp>
#include <Forms.hpp>
#include "MyTScrollingWinControl.h"
#include "I2adlEditor.h"
#include <FileCtrl.hpp>

// The ObjectTree contains the following in the "data" portion of the tree nodes:
//     Data Set Exposers    NULL                   BaseSystem   *VivaSystem
//     DBitIn               *I2adl                 X86CPU       *VivaSystem
//     Primitive Objects    NULL                   Input        *I2adl
//     Assign               *I2adl                 X86UI        *VivaSystem
//     Composite Objects    NULL                   Image1       NULL (if form activated)
//     ADD                  *I2adl                 GetName      NULL (if form activated)
//     Tree Group           NULL                   UIXCPU       *VivaSystem
//     AND(2,1)             -1                     Trasport     *I2adl
//     AND                  *I2adl
//     AND(2,2)             -1
//     AND                  *I2adl
//
// Library tree groups are identified by an ImageIndex of LIBRARY_NODE_IMAGE_INDEX.
//		The root "Libraries" node has NULL in its "data" field, while "Library Name" nodes
//		have a AnsiString pointer to the name of the library.
//
// Subsystem tree groups are identified by an ImageIndex of SUBSYSTEM_NODE_IMAGE_INDEX.
//		Their nodes contain a VivaSystem pointer.
//
#define TREE_GROUP_NODE			NULL
#define FOOTPRINT_TREE_NODE		(void *) -1

#define ACCESS_KEY_STRING_LENGTH	4
#define DATA_SET_EDITOR_MARGIN		16

extern TStringGrid		*CostCalc;

enum					TreeStateEnum  {tsNeedsRebuild, tsAlreadyBuilt};

class TMainForm : public TForm
{
__published:
    TOpenDialog               *OpenDialog;
    TSaveDialog               *SaveDialog;
    TPrintDialog              *PrintDialog;
    TPrinterSetupDialog       *PrintSetupDialog;
    TPopupMenu                *ObjectTreeMenu;
    TColorDialog              *ColorDialog;
    TImageList                *VivaCOImages;
    TImageList                *ObjectTypeList;
    TSplitter                 *TreeSplitter;
    TPanel                    *TreePanel;
    TTreeView                 *DataSetTree;
    TTreeView                 *SystemTree;
    TMenuItem                 *OpenAsSheet;
    TPopupMenu                *WipTreeMenu;
    TMenuItem                 *SortTreeByName1;
    TMenuItem                 *EditAttributes;
    TMenuItem                 *N9;
    TMenuItem                 *ChangeTreeGroup;
    TMenuItem                 *DeleteSheet;
    TPopupMenu                *FontEditTrigger;
    TMenuItem                 *MenuItem1;
    TFontDialog               *FontDialog1;
    TMenuItem                 *HideTreeGroups1;
    TMenuItem                 *DeleteObject;
    TMenuItem                 *ConvertSheet;
    TPopupMenu                *ToolBarMenu;
    TControlBar               *ControlBar1;
    TMainMenu                 *MainMenu;
    TMenuItem                 *ProjectMenu;
    TMenuItem                 *FileNewItem;
    TMenuItem                 *FileOpenItem;
    TMenuItem                 *SaveProject1;
	TMenuItem                 *cmdSaveProjectAs;
    TMenuItem                 *Separator01TB;
    TMenuItem                 *ProjectOptions;
    TMenuItem                 *Separator02;
    TMenuItem                 *FileExitItem;
    TMenuItem                 *SheetMenu;
    TMenuItem                 *OpenSheet;
	TMenuItem 				  *cmdSaveSheet;
	TMenuItem                 *cmdSaveSheetAs;
    TMenuItem                 *FileNewSheetItem;
    TMenuItem                 *DuplicateSheet;
    TMenuItem                 *Separator04TB;
    TMenuItem                 *CloseSheet;
    TMenuItem                 *ConvertSheettoObject;
    TMenuItem                 *cmdPlayStopSheet;
    TMenuItem                 *Separator05TB;
    TMenuItem                 *FilePrintItem;
    TMenuItem                 *Separator06TB;
    TMenuItem                 *EditMenu;
    TMenuItem                 *EditCutItem;
    TMenuItem                 *EditCopyItem;
    TMenuItem                 *EditPasteItem;
    TMenuItem                 *Delete1;
    TMenuItem                 *SelectAll1;
    TMenuItem                 *InvertSelection1;
    TMenuItem                 *ViewMenu;
    TMenuItem                 *SortTreeByName;
    TMenuItem                 *HideTreeGroups;
    TMenuItem                 *DescendIntoSheet;
    TMenuItem                 *Separator07TB;
    TMenuItem                 *SystemMenu;
    TMenuItem                 *Load1;
    TMenuItem                 *Separator08TB;
    TMenuItem                 *SystemMerge;
    TMenuItem                 *SystemSave;
    TMenuItem                 *ToolsMenu;
    TMenuItem                 *Preferences1;
    TMenuItem                 *HelpMenu;
	TMenuItem				  *OnlineHelp;
    TMenuItem                 *Separator09;
    TMenuItem                 *HelpAboutItem;
    TMenuItem                 *cmdBack;
    TMenuItem                 *cmdNext;
    TMenuItem                 *cmdUndo;
    TMenuItem                 *cmdRedo;
    TMenuItem                 *ExpandTreeView1;
    TMenuItem                 *CollapseTreeView1;
    TMenuItem                 *BringWidgetFormtoFront;
    TMenuItem                 *cmdCOMFormDesigner;
    TMenuItem                 *cmdObjectBrowser;
    TMenuItem                 *HideTreeView;
    TMenuItem                 *HideTreeView1;
    TMenuItem                 *ExpandTreeView;
    TMenuItem                 *CollapseTreeView;
    TMenuItem                 *Separator11TB;
    TMenuItem                 *cmdSynthesize;
    TMenuItem                 *ExecutableMenu;
    TMenuItem                 *cmdOpenExecutable;
    TMenuItem                 *cmdSaveExecutable;
    TMenuItem                 *N1;
    TMenuItem                 *cmdSaveExecutableAs;
	TStatusBar *mainStatusBar;
	TMenuItem *SynthesizetoEDIF1;
	TPanel *MainPanel;
	TPageControl *MainEditorsControl;
	TTabSheet *I2adlEdit;
	TSplitter *FrameSplitter;
	TPanel *ViewHolder;
	TTabSheet *DataSetEditor;
	TScrollBox *ScrollBox1;
	TLabel *Label3;
	TComboBox *DSName;
	TBitBtn *AssignDataSet;
	TTabSheet *CostEditor;
	TToolBar *ToolBar1;
	TBitBtn *ResourceDisplay;
	TBitBtn *ResourceSave;
	TBitBtn *ResourceRead;
	TStringGrid *CostCalc_;
	TTabSheet *SystemEditor;
	TScrollBox *ScrollBox2;
	TLabel *SystemName;
	TLabel *Label11;
	TLabel *Label12;
	TEdit *SysNameEdit;
	TComboBox *ResourcePrototypeSelect;
	TComboBox *SystemTypeSelect;
	TEdit *Location;
	TGroupBox *GroupBox2;
	TGroupBox *GroupBox1;
	TPanel *MessagePanel;
	TSplitter *MessageSplitter;
	TMemo *SysAttMemo;
	TMemo *SysDoc;
	TGroupBox *GroupBox3;
	TListBox *ElementsListBox;
	TGroupBox *GroupBox4;
	TComboBox *cmbTreeGroup;
	TLabel *Label5;
	TLabel *Label4;
	TComboBox *cmbAttribute;
	TEdit *txtColor;
	TEdit *txtNodeColor;
	TLabel *Label2;
	TButton *btnChangeNodeColor;
	TMenuItem *UserGuide;
	TMenuItem *cmdSaveWODependenciesAs;
	TMenuItem *cmdZoomIn;
	TMenuItem *cmdZoomOut;
	TMenuItem *cmdFitToSize;
	TMenuItem *cmdFitToWidth;
	TMenuItem *Separator10TB;
	TMenuItem *cmdZoomDefault;
	TMenuItem *cmdRename;
	TPanel *ObjectTreePanel;
	TSplitter *Splitter7;
	TPanel *WipComTreePanel;
	TTreeView *WipTree;
	TTreeView *ComClassTree;
	TPageControl *ObjectTreePageControl;
	TTabSheet *ProjectTabSheet;
	TTabSheet *SystemTabSheet;
	TTabSheet *ComTabSheet;
	TTreeView *ProjectObjectTree;
	TTreeView *SystemObjectTree;
	TTreeView *ComObjectTree;
	TMenuItem *cmdSearch;
	TPageControl *MessageTabControl;
	TTabSheet *MessageTabSheet;
	TStringGrid *MessageGrid;
	TTabSheet *SearchTabSheet;
	TStringGrid *SearchGrid;
	TMenuItem *ObjectDocumentation;
	TFileListBox *FileListBox;
	TFileListBox *DirectoryListBox;
	TMenuItem *N2;
	TMenuItem *cmdSearchByName;
	TMenuItem *cmdSearchOnName;
	TTabSheet *OverloadTabSheet;
	TStringGrid *OverloadGrid;
	TMenuItem *N3;
	TMenuItem *LibraryMenu;
	TMenuItem *cmdOpenLibrary;
	TMenuItem *N4;
	TMenuItem *N5;
	TMenuItem *cmdProjectAttributes;
	TBitBtn *NewSystemButton;
	TLabel *lblBaseSystemFileName;
	TEdit *edtBaseSystemFileName;
	TLabel *Label1;
	TMenuItem *ObjectReference;
	TMenuItem *SaveSystemWODependenciesAs;
	TPopupMenu *MessageMenu;
	TMenuItem *MessageExternalHelp;
	TMenuItem *MessageCopyOne;
	TMenuItem *MessageCopyAll;
	TMenuItem *MessageClearMessages;
	TMenuItem *ShowObjectAttributes;
	TMenuItem *MessageLocateObjectSep;
	TMenuItem *MessageLocateObjectAll;
	TMenuItem *MessageLocateObject01;

    void __fastcall FileNew(TObject *Sender);
    void __fastcall FileOpenClick(TObject *Sender);
    void __fastcall FilePrint(TObject *Sender);
    void __fastcall FilePrintSetup(TObject *Sender);
    void __fastcall FileExit(TObject *Sender);
    void __fastcall EditCut(TObject *Sender);
    void __fastcall EditCopy(TObject *Sender);
    void __fastcall EditPaste(TObject *Sender);
    void __fastcall OnlineHelpClick(TObject *Sender);
	void __fastcall UserGuideClick(TObject *Sender);
    void __fastcall HelpAbout(TObject *Sender);
    void __fastcall I2adlViewDragOver(TObject *Sender, TObject *Source, int X, int Y, TDragState
            State, bool &Accept);
    void __fastcall I2adlViewDragDrop(TObject *Sender, TObject *Source, int X, int Y);
    void __fastcall ObjectTreeChange(TObject *Sender, TTreeNode *Node);
    void __fastcall FormResize(TObject *Sender);
    void __fastcall I2adlViewMouseDown(TObject *Sender, TMouseButton Button, TShiftState Shift,
            int X, int Y);
    void __fastcall I2adlViewMouseMove(TObject *Sender, TShiftState Shift, int X, int Y);
    void __fastcall I2adlViewMouseUp(TObject *Sender, TMouseButton Button, TShiftState Shift,
            int X, int Y);
    void __fastcall FormKeyPress(TObject *Sender, char &Key);
    void __fastcall Delete1Click(TObject *Sender);
    void __fastcall BtnConvertSheetClick(TObject *Sender);
    void __fastcall DSNameDragDrop(TObject *Sender, TObject *Source, int X, int Y);
    void __fastcall DSNameDragOver(TObject *Sender, TObject *Source, int X, int Y, TDragState
            State, bool &Accept);
    void __fastcall ElementsListBoxDragOver(TObject *Sender, TObject *Source, int X, int Y,
            TDragState State, bool &Accept);
    void __fastcall ElementsListBoxDragDrop(TObject *Sender, TObject *Source, int X, int Y);
    void __fastcall AssignDataSetClick(TObject *Sender);
    void __fastcall DescendClick(TObject *Sender);
    void __fastcall FileNewSheetItemClick(TObject *Sender);
    void __fastcall ResourceSaveClick(TObject *Sender);
    void __fastcall ResourceReadClick(TObject *Sender);
    void __fastcall NewSystemButtonClick(TObject *Sender);
    void __fastcall SystemTreeDragOver(TObject *Sender, TObject *Source, int X, int Y,
            TDragState State, bool &Accept);
    void __fastcall SystemTreeDragDrop(TObject *Sender, TObject *Source, int X, int Y);
    void __fastcall SystemTreeKeyDown(TObject *Sender, WORD &Key, TShiftState Shift);
    void __fastcall SystemTreeKeyUp(TObject *Sender, WORD &Key, TShiftState Shift);
    void __fastcall OpenSystem1Click(TObject *Sender);
    void __fastcall SaveSystem1Click(TObject *Sender);
    void __fastcall SysNameEditChange(TObject *Sender);
    void __fastcall SystemTreeEdited(TObject *Sender, TTreeNode *Node, AnsiString &S);
    void __fastcall SystemTypeSelectChange(TObject *Sender);
    void __fastcall WipTreeKeyDown(TObject *Sender, WORD &Key, TShiftState Shift);
    void __fastcall WipTreeKeyUp(TObject *Sender, WORD &Key, TShiftState Shift);
    void __fastcall WipTreeDragDrop(TObject *Sender, TObject *Source, int X, int Y);
    void __fastcall WipTreeDragOver(TObject *Sender, TObject *Source, int X, int Y, TDragState
            State, bool &Accept);
    void __fastcall BtnCopySheetClick(TObject *Sender);
    void __fastcall MergeSystem1Click(TObject *Sender);
    void __fastcall SaveProject1Click(TObject *Sender);
    void __fastcall SystemTreeClick(TObject *Sender);
    void __fastcall ResourcePrototypeSelectChange(TObject *Sender);
    void __fastcall SysAttRichEditChange(TObject *Sender);
    void __fastcall OpenSheetClick(TObject *Sender);
    void __fastcall FormActivate(TObject *Sender);
    void __fastcall MainEditorsControlChange(TObject *Sender);
    void __fastcall DSNameDropDown(TObject *Sender);
    void __fastcall ObjectTreeDragDrop(TObject *Sender, TObject *Source, int X, int Y);
    void __fastcall PopUpMenuOption1(TObject *Sender);
    void __fastcall ObjectTreeMenuPopup(TObject *Sender);
    void __fastcall WipTreeMenuPopup(TObject *Sender);
    void __fastcall FormKeyDown(TObject *Sender, WORD &Key, TShiftState Shift);
    void __fastcall CloseSheetClick(TObject *Sender);
    void __fastcall PopUpMenuOption2(TObject *Sender);
    void __fastcall ObjectTreeMouseDown(TObject *Sender, TMouseButton Button, TShiftState Shift,
            int X, int Y);
    void __fastcall SystemTreeMouseDown(TObject *Sender, TMouseButton Button, TShiftState Shift,
            int X, int Y);
    void __fastcall WipTreeMouseDown(TObject *Sender, TMouseButton Button, TShiftState Shift
            , int X, int Y);
    void __fastcall ObjectTreeEdited(TObject *Sender, TTreeNode *Node, AnsiString &S);
    void __fastcall ChangeTreeGroupClick(TObject *Sender);
    void __fastcall WipTreeDblClick(TObject *Sender);
    void __fastcall ObjectTreeDblClick(TObject *Sender);
    void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
    void __fastcall ObjectTreeSheetChange(TObject *Sender);
    void __fastcall TreeButton1Click(TObject *Sender);
    void __fastcall WipTreeEdited(TObject *Sender, TTreeNode *Node, AnsiString &S);
    void __fastcall ObjectTreeKeyDown(TObject *Sender, WORD &Key, TShiftState Shift);
    void __fastcall FontEditTriggerPopup(TObject *Sender);
    void __fastcall txtNodeColorExit(TObject *Sender);
    void __fastcall btnChangeNodeColorClick(TObject *Sender);
    void __fastcall txtNodeColorChange(TObject *Sender);
    void __fastcall ObjectTreeStartDrag(TObject *Sender, TDragObject *&DragObject);
    void __fastcall Preferences1Click(TObject *Sender);
    void __fastcall ObjectTreeExpanded(TObject *Sender, TTreeNode *Node);
    void __fastcall ResourceDisplayClick(TObject *Sender);
    void __fastcall ObjectTreeCollapsed(TObject *Sender, TTreeNode *Node);
    void __fastcall TreeButton2Click(TObject *Sender);
    void __fastcall ShapeMoveTimerOnTimer(TObject *Sender);
    void __fastcall SelectAll1Click(TObject *Sender);
    void __fastcall InvertSelection1Click(TObject *Sender);
    void __fastcall MainMenuChange(TObject *Sender, TMenuItem *Source, bool Rebuild);
    void __fastcall SysDocChange(TObject *Sender);
    void __fastcall I2adlViewWndProc(Messages::TMessage &Message);
    void __fastcall MainFormWndProc(Messages::TMessage &Message);
	void __fastcall DisabledControlEventHandler(tagMSG &Msg, bool &Handled);    
    void __fastcall MainFormMenuClick(TObject *Sender);
    void __fastcall cmdBackClick(TObject *Sender);
    void __fastcall cmdNextClick(TObject *Sender);
    void __fastcall cmdRedoClick(TObject *Sender);
    void __fastcall cmdUndoClick(TObject *Sender);
    void __fastcall ChevronButtonClick(TObject *Sender);
    void __fastcall ControlBar1BandMove(TObject *Sender, TControl *Control, TRect &ARect);
    void __fastcall ExpandTreeViewClick(TObject *Sender);
    void __fastcall CollapseTreeViewClick(TObject *Sender);
    void __fastcall BringWidgetFormtoFrontClick(TObject *Sender);
    void __fastcall DSNameClick(TObject *Sender);
    void __fastcall cmdObjectBrowserClick(TObject *Sender);
    void __fastcall cmdCOMFormDesignerClick(TObject *Sender);
    void __fastcall HideTreeViewClick(TObject *Sender);
    void __fastcall TreeSplitterMoved(TObject *Sender);
    void __fastcall ElementsListBoxKeyDown(TObject *Sender, WORD &Key, TShiftState Shift);
    void __fastcall DataSetTreeKeyDown(TObject *Sender, WORD &Key, TShiftState Shift);
    void __fastcall cmbTreeGroupDropDown(TObject *Sender);
    void __fastcall DataSetTreeDblClick(TObject *Sender);
    void __fastcall cmdSynthesizeClick(TObject *Sender);
    void __fastcall cmdOpenExecutableClick(TObject *Sender);
    void __fastcall cmdSaveExecutableClick(TObject *Sender);
    void __fastcall cmdSaveExecutableAsClick(TObject *Sender);
	void __fastcall SynthesizetoEDIF1Click(TObject *Sender);
	void __fastcall cmdPlayStopSheetClick(TObject *Sender);
	void __fastcall MessageGridDrawCell(TObject *Sender, int ACol, int ARow,
          TRect &Rect, TGridDrawState State);
	void __fastcall WipTreeClick(TObject *Sender);
	void __fastcall WipTreeKeyPress(TObject *Sender, char &Key);
	void __fastcall WipTreeContextPopup(TObject *Sender, TPoint &MousePos,
          bool &Handled);
	void __fastcall cmdSaveProjectAsClick(TObject *Sender);
	void __fastcall cmdSaveWODependenciesAsClick(TObject *Sender);
	void __fastcall cmdSaveSheetAsClick(TObject *Sender);
	void __fastcall cmdSaveSheetClick(TObject *Sender);
	void __fastcall cmdZoomInClick(TObject *Sender);
	void __fastcall cmdZoomOutClick(TObject *Sender);
	void __fastcall cmdFitToSizeClick(TObject *Sender);
	void __fastcall cmdFitToWidthClick(TObject *Sender);
	void __fastcall cmdZoomDefaultClick(TObject *Sender);
	void __fastcall cmdRenameClick(TObject *Sender);
	void __fastcall ControlBar1MouseMove(TObject *Sender, TShiftState Shift,
          int X, int Y);
	void __fastcall ObjectTreePageControlChange(TObject *Sender);
	void __fastcall ObjectTreeDragOver(TObject *Sender,
          TObject *Source, int X, int Y, TDragState State, bool &Accept);
	void __fastcall MessageGridDblClick(TObject *Sender);
	void __fastcall MessageLocateObjectClick(TObject *Sender);
	void __fastcall MessageGridKeyDown(TObject *Sender, WORD &Key,
          TShiftState Shift);
	void __fastcall cmdSearchClick(TObject *Sender);
	void __fastcall ObjectDocumentationClick(TObject *Sender);
	void __fastcall cmdSearchForInstancesClick(TObject *Sender);
	void __fastcall cmdSearchByNameClick(TObject *Sender);
	void __fastcall cmdSearchOnNameClick(TObject *Sender);
	void __fastcall WipTreeChange(TObject *Sender, TTreeNode *Node);
	void __fastcall WipTreeCustomDrawItem(TCustomTreeView *Sender,
          TTreeNode *Node, TCustomDrawState State, bool &DefaultDraw);
	void __fastcall cmdOpenLibraryClick(TObject *Sender);
	void __fastcall cmdProjectAttributesClick(TObject *Sender);
	void __fastcall SystemTreeChange(TObject *Sender, TTreeNode *Node);
	void __fastcall ObjectReferenceClick(TObject *Sender);
	void __fastcall FormCanResize(TObject *Sender, int &NewWidth,
          int &NewHeight, bool &Resize);
	void __fastcall SaveSystemWODependenciesAsClick(TObject *Sender);
	void __fastcall MessageMenuPopup(TObject *Sender);
	void __fastcall MessageExternalHelpClick(TObject *Sender);
	void __fastcall MessageCopyOneClick(TObject *Sender);
	void __fastcall MessageCopyAllClick(TObject *Sender);
	void __fastcall MessageClearMessagesClick(TObject *Sender);
	void __fastcall MessageGridMouseDown(TObject *Sender, TMouseButton Button,
          TShiftState Shift, int X, int Y);
	void __fastcall ShowObjectAttributesClick(TObject *Sender);

private:
    void __fastcall ViewScaleMenuItemClick(TObject *Sender);
    void __fastcall RecentFileItemClick(TObject *Sender);
    TMenuItem 		*GetMenuToInsertInto(FileTypeEnum TypeIndex);

	bool			FControlsEnabled;
    bool			WarnedSystemTreeDragOver;	// Required to only warn once per drag.

public:
#ifdef LOG_EXPANDED_OBJECTS
	int				ExpandedObjectsLogFile;
#endif

    TTreeNode       *ActiveTreeNode;            // The node that is currently selected on the
    											//		selected Object Tree; can be NULL
	int		 		ErrorFileHandle;            // ErrorLog.txt file, to where messages are
    											//		echoed
    TTreeNode       *ActiveWipTreeNode;         // Node selected in WIP tree.  Can be NULL
    VList           *RecentFileMenuItemList;    // All recent file menu items; for purposes
    											//		of resetting them
    MyTMemoList		*ZoomTextBoxMemos;  		// One for each text object font to keep Viva
    											//		from shimmering.
    TToolBar        *MainToolBar;               // Dynamically created tool bar just under main
    											//		menu
    TSpeedButton    *ChevronButton;             // Button on MainToolBar that brings up a list
    											//		of items that don't appear on the tool
                                                //		bar, due to space constraints
    TMenuItem		*ViewScaleMenuItems[VIEW_SCALE_MAX]; // 1 for each scale sizes 1-10
    RECT			NewBoundsRect;              // Dimensions for this form, read from file
	DataSet			*EditDataSet;               // Dataset being editted or fabricated in the
    											//		Dataset Editor
    TTreeNode		*BasicDataSetRootNode;      // Root node for basic datasets in the Dataset
    											//		Tree
    TTreeNode		*ProjectObjectRootNode;     // Root node for composite objects in the
    											//		Project Object Tree.
	TTreeNode		*LibraryRootNode;  			// Root node for Library objects in the
    											//		Project Object Tree.

    DYNAMIC void __fastcall MouseWheelHandler(Messages::TMessage &Message);
    void __fastcall ResizeToolBar();
    virtual __fastcall TMainForm(TComponent *Owner);
    void __fastcall LoadDSEditor(AnsiString _DataSetName);
    void __fastcall CreateToolBar();
    void __fastcall MenuBtnSync(TMenuItem *MenuItem, bool Toggle = false);
    int				SaveFileDlg(FileTypeEnum FileType, bool Auto, bool SaveDependencies = true);
    bool			OpenFile(FileTypeEnum SelFileType, bool bKeepFileName = false, bool
    	ReadingClipBoard = false);
    void			RenameTreeGroup(TTreeNode *TreeNode, AnsiString &NewTreeGroup);
    AnsiString		GetTreeGroupName(TTreeNode *TreeNode);
    void			GetLongestWidth(TTreeNode *Node, int &LongestWidth);
    void __fastcall InitializeRecentFileMenu();
    void            DeleteRFEntry(int Index, int TypeIndex);
    void __fastcall PopulateRecentFileMenu();
    void __fastcall AddRecentFileMenuItem(AnsiString FileName, FileTypeEnum TypeIndex);
    void __fastcall AddRecentFile(AnsiString FileName, FileTypeEnum TypeIndex = ftUndetermined);
    int  __fastcall GetMenuPos(TMenuItem *MenuToInsertInto);
    void __fastcall LoadResourceUsage(VivaSystem *System_, int &RowNumber);
    void __fastcall AutoSizeTreeView(TTreeView *TreeView = NULL);
	void 			GenerateViewScaleMenuItems();
    void			Zoom(int NewViewScale);
    void            SaveSettings();
    void            LoadSettings();
    AnsiString      MakeFileName(AnsiString RawFileName, bool auto);
	void 			ClearMessageGridsAndLogFile(bool ReopenLogFile);
	void 			ClearMessageGrid(TStringGrid *Grid);
    bool			SaveProject(bool Auto);
    bool			SaveSystem(bool SaveDependencies = true);
    void __fastcall	OnAppActivate(TObject *);
    void			SaveSheet(bool PromptForFileName, bool SaveDependencies = true);
    void			WarnAboutDuplicateSystemNames();
	void			UpdateResources(bool DiagnoseError = true);
    void			FitToSize(TRect ExtentRect, bool WidthOnly = false, bool Readjust = false);
    TTreeView		*GetActiveObjectTree();
	void			FindI2adlRefs();
	void			FindI2adlRefs(AnsiString SearchString, bool AllowWildCardSearch);
	AnsiString		AutoVersion(AnsiString FileName);
    void			StatusMessage(AnsiString Message, bool SetStatusBar);
	void			EnableDisableControls(bool Enable);
    bool			CanTreeNodeBeDeleted(TTreeNode *TreeNode, bool PerformDelete = false);
    bool			CanTreeNodeBeRenamed(TTreeNode *TreeNode, RenameMethodEnum
    	RenameMethod);
    I2adl			*GetI2adlObject(TTreeNode *TreeNode, bool AllowFootprint = true);
    bool			IsResourceDataAuthorized(TStringList *ResourceLines);
    void			DisplayExternalHelpFile();
    TStringGrid		*GetActiveMessageGrid();
	void			MessageLocateObject(bool LocateAllObjects, VDismember *Hierarchy = NULL);

    __property TTreeView *ActiveObjectTree = {read = GetActiveObjectTree};
    __property bool ControlsEnabled = {read = FControlsEnabled};

};

extern PACKAGE TMainForm            *MainForm;

#endif
