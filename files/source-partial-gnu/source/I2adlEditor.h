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

#ifndef I2adlEditorH
#define I2adlEditorH

#define DO_NOTHING          0
#define CLOSE_SHEET         -1

#include "Globals.h"
#include "I2adlList.h"

#define SCROLLRANGEBUFFER   (ViewScale * 6)
#define UNDO_MAX            50
#define VIEW_SCALE_MAX 		10
#define VIEW_SCALE_DEFAULT	5

class TSheetState
{
public:
    I2adlList	*Sheet;
    I2adl		*AppliesTo;

    TSheetState(I2adlList *_Sheet, I2adl *_AppliesTo)
    {
        Sheet = _Sheet;
        AppliesTo = _AppliesTo;
    }

    ~TSheetState()
    {
        // Delete the objects in the sheet state.
        //     AppliesTo does not need to be deleted because it is a wip sheet.
        Sheet->DeleteObjects();

        delete Sheet;
    }
};


namespace I2adlEditor
{
	extern int				ViewScale;              // Zoom factor, in pixels per grid cell
    extern int              CurrentUndoIndex;       // Index into list of saved I2adl Editor
    												//		states
    extern I2adl			*CurrentSheet;			// Wip Sheet being edited
    extern I2adlList		*OpenBehavior;      	// Set when an object is opened and reset
                                                	//		when DuplicateSheet is pressed.
                                                	//      This can keep an unchanged sheet
                                                    //		from auto-closing.
    extern TPoint           CopyPoint;              // Mouse cursor point when objects were
    												//		copied.  Used on CNTL+Drag copies.
    extern TPoint           CurrentPoint;			// Mouse cursor point when Paste command
    												//		was invoked
    extern I2adlList        *ClipBoard;         	// I2adl Editor clipboard for cut/copy/paste
    extern int				ClipBoardTimeStamp;		// Identifies the clipboard file we wrote
	extern AnsiString		ClipBoardFileName;		// Contains I2adl Editor clipboard objects.
    extern I2adl            *DropObject;            // I2adl object being drug from Object Tree
    extern MyTMemo          *TextI2adlMemo;			// TMemo of edit mode on Text I2adl Shapes.
    extern I2adl            *ActiveTextI2adl;		// Text I2adl whose shape is in edit 
    extern TColor           SelectionColor;         // Color that selected shapes are tinted
    extern TColor			NewSelectionColor;      // User-selected tint for selected shapes
    extern bool             CopyInProcess;        	// Copying I2adl Shapes
    extern bool             UpdateNavigationList;   // Update back/next list for sheet
    												//		navigation actions.  Turned off for
                                                    //	    internally invoked navigation.
    extern bool             LockedX;				// I2adl drag is locked in X axis
													//		(shift + drag)
    extern bool             LockedY;				// I2adl drag is locked in Y axis
													//		(shift + drag)
	extern bool				DoObjectSnap;           // Snap objects to transport, so it doesn't
    												//		have to be jointed.  Turned off only
                                                    //		for to do internal stuff
    extern int              MoveLimitX;         	// How far to the left we can move without
    												//		going off the edge of the editor
    extern int              MoveLimitY;				// How far up we can move without
    												//		going off the edge of the editor
    extern SheetList        *NavigationSheetList; 	// Navigation Stack for Back/Next
    extern SheetStateList   *UndoList;				// Keeps TSheetState entries for Undo/Redo.
    extern POINT			PrevPoint;				// In a move operation, point from which
    												//		we started
    extern TColor			WorkSpaceColor;			// Current background color of I2adl Editor
    extern TColor           NewWorkSpaceColor;      // User-selected color for I2adl Editor
    extern TColor			DrawColor;				// Color for selection rectangle and node
    												//		names.  Either black or white,
                                                    //		to contrast to background color.
    extern TTimer			*ShapeMoveTimer;		// Controls the movement of shapes, to
													//		eliminate many redundant attempts.
    extern TPopupMenu		*FontEditTrigger;		// Present simply to override normal popup
    												//		on right-click, so that we can use
                                                    //		it to trigger font edit.
    extern TShapeList		*DragList;				// Transport shapes to be stretched
    extern TShapeList		*MoveList;				// I2adl shapes to be moved
    extern TShape			*SelectRectTop;         // Top line of shape selection rectangle
    extern TShape			*SelectRectLeft;        // Left line of shape selection rectangle
    extern TShape			*SelectRectRight;       // Right line of shape selection rectangle
    extern TShape			*SelectRectBottom;      // Bottom line of shape selection rectangle

    void CreateUI(I2adlList *_Sheet);
    void DestroyUI();
    bool IsPastMoveTreshhold(TPoint ptDiff);
    void ResetMove(bool Cleanup = false);
    void I2adlViewCleanup();
    void Deselect();
    void RedrawSheet();
    void PaintAllNames();
    int  DoWhatToSheet(I2adl *_Behavior);
    bool ProcessSheet(I2adl *Sheet, int nChoice);
    void ReleaseTransport();
    void SelectAll(bool Invert = false);
    void LoadAsSheet(I2adl *Object);
    int  GetNavigationListEntry(I2adl *Sheet, I2adl **UseSheet);
	void UpdateSheetNavigationList(I2adl *Object, int OldIndex = -1);
    void SetCurrentSheet(I2adl *Object, bool AutoLeaveOpen = false);
    void RemoveSheet(I2adl *Object, bool SetCurrent = true);
    I2adl *CreateNewSheet();
    I2adl *CreateNewSheet(AnsiString SheetName, I2adlList *Behavior);
    Node *CheckNodeProximity(int X, int Y, int NodeType, int SnapTolerance, bool
    	SkipActiveTran = false);
    bool ConnectNodeIfPossible(Node *ThisNode, int paramX = 0, int paramY = 0, bool
    	SkipActiveTran = false);
    void CopyIntoClipBoard();
    void PasteFromClipBoard(TObject *Sender);
    void UndoTransport();
    void TerminateDraw();
    I2adl *ConnectNodeToActiveTransport(Node *ThisNode);
    bool ConnectAllCompatibleNodes(I2adl *ThisObject, int X = 0, int Y = 0, bool
    	SkipActiveTran = false);
    void SnapI2adlToGrid(MyTShapeI2adl *ADL);
    Node *GetUpOrDownStreamNode(Node *StartNode, bool StopAtJunction = true);
    void PropagateOperation(Node *CurrentNode, int OperationCode, void *Value);
    void HandleTransportDraw(TMouseButton Button, int X, int Y);
    void MouseMove(TShiftState Shift, int X, int Y);
    void MouseUp(TMouseButton Button, TShiftState Shift, int X, int Y);
    void DragOver(TObject *Sender, TObject *Source, int X, int Y, TDragState State
        , bool &Accept);
    void UpdateWorkspaceColor();
    void UpdateSelectionColor();
    int EnsureScrollRange(TControlScrollBar *Scroller, int Data);
    int ScrollBarCage(int Pos, TControlScrollBar *Scroller);
    void ControlMoveAndDrag(TPoint &ptDiff);
    void MoveLists(TPoint ptDiff, bool Single = false);
    void ControlDragCopyAndPaste();
    void DetermineMoveDragAll(TShiftState Shift);
    void DetermineMoveDragSingle(TShiftState Shift, TPoint &ptDiff, MyTShapeI2adl *Shape);
    void DetermineMoveDrag(MyTShapeI2adl *Shape, bool Single, bool Sever, bool AllowToBreak);
    void StartTransport(Node *FromNode);
    void InitTransportDrawMode(Node *FromNode);
    void NavigateSheet(bool Forward = false);
    void RevertSheetState(bool Undo);
    void SaveSheetState();
    void DeleteSelectedObjects();
    void RebuildUI(int NewViewScale = 0);
    void StartToMove(int X, int Y);
    void ConvertSheet();
    void MergeRedundantTransports(int X, int Y);
    TRect ComputeCurrentSheetExtent(bool SelectedOnly = false);
    void SetStandardFont(TFont *Font, int _ViewScale = -1);
    void SetDrawColor();
    I2adl *NewWipObject(I2adlList *Behavior, AnsiString &Name);
}

using namespace I2adlEditor;
#endif
