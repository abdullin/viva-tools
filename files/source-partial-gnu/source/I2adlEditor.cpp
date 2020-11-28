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

#include "I2adlEditor.h"
#include "MyTScrollingWinControl.h"
#include "AttributeEditor.h"
#include "ErrorTrap.h"
#include "I2adl.h"
#include "MyTShape.h"
#include "Node.h"
#include "NodeList.h"
#include "MyTMemo.h"
#include "Main.h"
#include "Project.h"
#include "VivaIcon.h"
#include "DataSet.h"
#include "VivaSystem.h"
#include "VivaIO.h"
#include "ComManager.h"

#pragma package(smart_init)

namespace I2adlEditor
{
    int				ViewScale				= VIEW_SCALE_DEFAULT;
    int             CurrentUndoIndex;
    I2adl			*CurrentSheet;
    I2adlList		*OpenBehavior;
    TPoint          CopyPoint;
    TPoint          CurrentPoint;
    I2adlList       *ClipBoard				= NULL;
    int				ClipBoardTimeStamp		= 0;
    AnsiString		ClipBoardFileName;
    I2adl           *DropObject				= NULL;
    MyTMemo         *TextI2adlMemo;
    I2adl           *ActiveTextI2adl		= NULL;
    TColor          NewWorkSpaceColor;
    TColor          SelectionColor;
    TColor          NewSelectionColor;
    bool			CopyInProcess			= false;
    bool            LockedX					= false;
    bool            LockedY					= false;
	bool	 		DoObjectSnap			= true;
    int             MoveLimitX				= 0;
    int             MoveLimitY				= 0;
    bool            UpdateNavigationList	= true;
    SheetList       *NavigationSheetList;
    SheetStateList  *UndoList;
    POINT			PrevPoint;
    TColor			WorkSpaceColor;
    TColor			DrawColor;
    TTimer			*ShapeMoveTimer;
    TPopupMenu		*FontEditTrigger;
    TShapeList		*DragList;
    TShapeList		*MoveList;

    // 4 components are used to draw the selection rectangle so painting will be faster.
    TShape			*SelectRectTop;
    TShape			*SelectRectLeft;
    TShape			*SelectRectRight;
    TShape			*SelectRectBottom;
};


int NodeCompareUpdate(NodeList *NList, NodeList *UList)
{
    int    VCount = 0;

    for (int i = 0; i < NList->Count; i++)
    {
        Node    *UNode = UList->Items[i];

        if (UNode->DSet == VariantDataSet)
            VCount++;

        if (NList->Items[i]->DSet != UNode->DSet)
            NList->Items[i]->DSet = VariantDataSet;
    }

    return VCount;
}


// Create TShapes for all of the objects in the Sheet.
void I2adlEditor::CreateUI(I2adlList *_Sheet)
{
    I2adlView->Reset();

    if (_Sheet == NULL)
        return;

	// Calcualte the new view scale and make sure it is a valid zoom percent.
	int		NewViewScale = (_Sheet->LastViewScale > 0) ? _Sheet->LastViewScale
    										   		   : VIEW_SCALE_DEFAULT;

	if (NewViewScale > VIEW_SCALE_MAX)
		NewViewScale = VIEW_SCALE_MAX;

    // If the ViewScale is changing, then update scroll bar increments and the drop down menu.
    bool	ChangedZoomPercent = false;

	if (ViewScale != NewViewScale)
    {
    	ChangedZoomPercent = true;

        I2adlView->HorzScrollBar->Increment = short(NewViewScale * 2);
        I2adlView->VertScrollBar->Increment = short(NewViewScale * 2);
        MainForm->ViewScaleMenuItems[ViewScale - 1]->Checked = false;
        MainForm->ViewScaleMenuItems[NewViewScale - 1]->Checked = true;

	    ViewScale = NewViewScale;
    }

    int    ScrollBarX = (_Sheet->LastHorzScroll > 0) ? _Sheet->LastHorzScroll * ViewScale
													 : I2adlView->HorzScrollBar->Position;
    int    ScrollBarY = (_Sheet->LastVertScroll > 0) ? _Sheet->LastVertScroll * ViewScale
													 : I2adlView->VertScrollBar->Position;

	// Make sure the scroll bars are on the grid snap.
    ScrollBarX -= ScrollBarX % ViewScale;
    ScrollBarY -= ScrollBarX % ViewScale;

    I2adlView->HorzScrollBar->Position = 0;
    I2adlView->VertScrollBar->Position = 0;

    for (int i = 0; i < _Sheet->Count; i++)
    {
        I2adl    *CurrentI2adl = ((I2adl *) _Sheet->Items[i]);

        CurrentI2adl->CreateShape();
		CurrentI2adl->BringShapeToFront();
    }

    // Transport Cleanup:
    for (int h = 0; h < 3; h++) // no magic in the number 3; just works out well.
    {
        for (int i = 0; i < _Sheet->Count; i++)
        {
            I2adl    *CurrentI2adl = (*_Sheet)[i];

            if (CurrentI2adl->NameCode == NAME_TRANSPORT)
            {
                NodeList    *IONodes = CurrentI2adl->Inputs;

                for (int IO = 0; IO < 2; IO++) // For each NodeList
                {
                    Node    *ThisNode = IONodes->Items[0];
                    Node    *CNode = ThisNode->ConnectedTo;

                    if (CNode !=  NULL)
                    {
                        // Get logical coordinates of this node and that to which it is connected:
                        TPoint ThisPoint = TPoint(ThisNode->Left + ThisNode->Owner->X
                            , ThisNode->Top + ThisNode->Owner->Y);

                        // Make sure that nodes are not placed incorrectly:
                        if (((MyTShapeTransport *) CurrentI2adl->I2adlShape)->IsHorizontal)
                        {
                            ThisNode->Top = 0; // Always top-justified on a horizonal transport.
                            // If, for whatever reason, a node is between an end of the transport
                            //      and the other node, put it at the end that it is near:
                            if (ThisNode->Left != 0)
                            {
                                if (ThisNode->Left < CurrentI2adl->OtherEnd(ThisNode)->Left)
                                    ThisNode->Left = 0;
                                else
                                    ThisNode->Left = CurrentI2adl->Width - ThisNode->GetSize();
                            }
                        }
                        else // Same logic as above, except applied to vertical transport:
                        {
                            ThisNode->Left = 0;

                            if (ThisNode->Top != 0)
                            {
                                if (ThisNode->Top < CurrentI2adl->OtherEnd(ThisNode)->Top)
                                    ThisNode->Top = 0;
                                else
                                    ThisNode->Top = CurrentI2adl->Height - ThisNode->GetSize();
                            }
                        }

                        // If nodes are misaligned with that to which they are connected, then
                        //      detach and re-attach using UI calls:
                        TPoint    CPoint = TPoint(CNode->Left + CNode->Owner->X
                            , CNode->Top + CNode->Owner->Y);

#if 1
                        if (abs(ThisPoint.x - CPoint.x) > 1 || abs(ThisPoint.y - CPoint.y) > 1)
                        {
                            ThisNode->Disconnect();
                            InitTransportDrawMode(ThisNode); // set owner as ActiveTransport.
                            ConnectNodeToActiveTransport(CNode);
                            ResetMove(true);
                            break;
                        }
#endif                        
                    }

                    IONodes = CurrentI2adl->Outputs;
                }
            }
        }
    }

    // Restore the position of the scrollbars.
    //		If we have zoomed out, then don't increase the size of the I2adl Editor.
    if (!ChangedZoomPercent)
    {
        EnsureScrollRange(I2adlView->HorzScrollBar, I2adlView->Width + ScrollBarX -
            SCROLLRANGEBUFFER);
        EnsureScrollRange(I2adlView->VertScrollBar, I2adlView->Height + ScrollBarY -
            SCROLLRANGEBUFFER);
    }

    I2adlView->HorzScrollBar->Position = ScrollBarX;
    I2adlView->VertScrollBar->Position = ScrollBarY;
}


// Destroy the TShapes for the objects on the current Sheet.  Also reset scroll ranges.
void I2adlEditor::DestroyUI()
{
    if (CurrentSheet != NULL)
    {
    	// Record the current I2adl Editor information.
        CurrentSheet->Behavior->RecordGraphicInfo();

        for (int i = 0; i < CurrentSheet->Behavior->Count; i++)
        {
            I2adl *Object = (*CurrentSheet->Behavior)[i];
            delete Object->I2adlShape;
        }
    }

    I2adlView->HorzScrollBar->Range = 0;
    I2adlView->VertScrollBar->Range = 0;    
}


// Gets and entry and its index from the Navigation List based on sheet passed:
int I2adlEditor::GetNavigationListEntry(I2adl *Sheet, I2adl **UseSheet)
{
    // if CurrentSheet has is a copy of another sheet, it means that the sheet came from an
    //      Object in the Object Tree.
    if (Sheet != NULL && Sheet->Behavior->CopyOf != NULL)
    {
        *UseSheet = ProjectObjects->GetObject(Sheet->Behavior->CopyOf);
    }
    else
    {
        *UseSheet = Sheet;
    }

    return NavigationSheetList->IndexOf(*UseSheet);
}


// Code to update the Navigation Stack (for "Back" and "Next" commands), if applicable.
void I2adlEditor::UpdateSheetNavigationList(I2adl *Object, int OldIndex)
{
	I2adl	*UseSheet = NULL;
    int		Index = GetNavigationListEntry(Object, &UseSheet);

    if (UpdateNavigationList)
    {
        if (OldIndex > -1 && OldIndex < NavigationSheetList->Count - 1 && Index == -1)
        {
            // If the old CurrentSheet is in, and up from the tail of, the Navigation List, then
            //      remove it and everything behind (tailward of) it:
            for (int i = OldIndex + 1; i < NavigationSheetList->Count;) // Delete() decrements count
                NavigationSheetList->Delete(i);
        }
        // If the new CurrentSheet is already in the Navigation List, then remove it:
        else if (Index != -1)
        {
            NavigationSheetList->Delete(Index);
        }

        if (UseSheet != NULL)
        {
	        NavigationSheetList->Add(UseSheet);
	        SaveSheetState();
        }
    }
}


// Set the behavior up as the new current Sheet.
void I2adlEditor::SetCurrentSheet(I2adl *Object, bool AutoLeaveOpen)
{
    if (Object != NULL && !AutoLeaveOpen && !ProcessSheet(CurrentSheet, DoWhatToSheet(Object)))
        return;

    // Is the new behavior already the current sheet?
    bool	AlreadyCurrentSheet = (Object == CurrentSheet);
    I2adl	*UseSheet;
    int		OldIndex = GetNavigationListEntry(CurrentSheet, &UseSheet);

    ReleaseTransport();

    I2adlView->Visible = false; // No need to update visual properties of controls.

	if (CurrentSheet)
        DestroyUI();

    CurrentSheet = Object;

    // Make sure the Play button is accurate.
    MyProject->CompiledSheet = MyProject->CompiledSheet;

    if (CurrentSheet)
        CreateUI(CurrentSheet->Behavior);

    I2adlView->Visible = true;

    // The following has the "Edit;Paste" menu option work before a mouse move over the
    //		I2adl Editor (Bug #852).
    if (MainForm->MainEditorsControl->ActivePage == MainForm->I2adlEdit)
	    MainForm->ActiveControl = I2adlView;

    TTreeNode	*TNode = MyProject->GetTreeNode(MainForm->WipTree->Items, CurrentSheet);

    if (TNode != NULL)
    {
        MyProject->ActiveWipNode = TNode;
        TNode->Selected = true;
        TNode->Focused = true;
    }

	// If the sheet is already open, then don't wipe out the OpenBehavior flag.  Otherwise,
    //     openning an object twice in a row would turn off the automatic closing of the
    //     unchanged sheet.
    if (!AlreadyCurrentSheet)
		OpenBehavior = NULL;

    I2adlView->Invalidate();

    UpdateSheetNavigationList(Object, OldIndex);
}


// Whether or not user ptDiff is greater than the mouse move threshhold for I2adl shape drag. 
bool I2adlEditor::IsPastMoveTreshhold(TPoint ptDiff)
{
    TPoint    StartPoint = TPoint(PrevPoint.x + ViewScale / 2, PrevPoint.y + ViewScale / 2);

    return (max(abs(ptDiff.y - StartPoint.y), abs(ptDiff.x - StartPoint.x)) >= ViewScale);
}


// Performs a 1-time move on a Shape by amounts specified in ptDiff, even if such involves
//      breaking or stretching transports; sets up for move, including loading Move and Drag lists:
void I2adlEditor::DetermineMoveDragSingle(TShiftState Shift, TPoint &ptDiff, MyTShapeI2adl *Shape)
{
    DetermineMoveDrag(Shape, true, Shift.Contains(ssAlt) && Shift.Contains(ssCtrl), true);
    Shape->PaintNames(WorkSpaceColor);
    MoveLists(ptDiff, true);
}


// Loads MoveList and DragList with the objects that need to be moved/dragged.
void I2adlEditor::DetermineMoveDragAll(TShiftState Shift)
{
    // Every time we start a move, we need to reset the Locked variables.
    LockedX = false;
    LockedY = false;
    MoveLimitX = 999999;
    MoveLimitY = 999999;

    // Load MoveList and DragList from the objects that have been selected.
    //     CurrentSheetCount saves two indirections and skips the transports added mid loop.
    int     CurrentSheetCount = CurrentSheet->Behavior->Count;
    bool    Sever = Shift.Contains(ssCtrl) && Shift.Contains(ssAlt);
    bool    AllowToBreak = !Shift.Contains(ssShift);

    for (int i = 0; i < CurrentSheetCount; i++)
        DetermineMoveDrag((*CurrentSheet->Behavior)[i]->I2adlShape, false, Sever, AllowToBreak);

    // The difference between the first PrevPoint and the Left/Top most object position is
    //     the position that the mouse can move to (relative to the entire I2adlView) before
    //     objects will move off the sheet.
    MoveLimitX = max(PrevPoint.x - MoveLimitX, (long) 10);
    MoveLimitY = max(PrevPoint.y - MoveLimitY, (long) 10);
}


// If the shape is selected, then load MoveList and DragList accordingly.
void I2adlEditor::DetermineMoveDrag(MyTShapeI2adl *Shape, bool Single, bool Sever,
    bool AllowToBreak)
{
    if (!Shape->Selected)
        return;

    Shape->PaintNames(WorkSpaceColor);

    // All selected objects move.
    Shape->MoveListAdd();

    // If the user has opted to sever the objects that he is dragging:
    if (Sever)
    {
        // If it is a transport, then we want to disconnect it from only non-transport objects:
        if (Shape->OwningI2adl->NameCode == NAME_TRANSPORT)
        {
        	// Pass #1 - Transport's input.
            Node    *CurrentNode = Shape->OwningI2adl->Inputs->Items[0];

            for (int pass = 0; pass < 2; pass++)
            {
                Node    *ConnectedNode = CurrentNode->ConnectedTo;

                if (ConnectedNode != NULL &&
                	ConnectedNode->Owner->NameCode != NAME_TRANSPORT &&
                    ConnectedNode->Owner->I2adlShape->Selected == false)
                {
                    CurrentNode->Disconnect(true);
                    ConnectedNode->Paint();
                }

                // Pass #2 - Transport's output.
                CurrentNode = Shape->OwningI2adl->Outputs->Items[0];
            }
        }
        else
        {
        	// Disconnect from all non-selected objects and return.
        	NodeList	*Inputs = Shape->OwningI2adl->Inputs;
			NodeList	*Outputs = Shape->OwningI2adl->Outputs;

            for (int i = 0; i < Inputs->Count; i++)
            {
            	Node	*InputNode = Inputs->Items[i];

                if (InputNode->ConnectedTo != NULL &&
                    !InputNode->ConnectedTo->Owner->I2adlShape->Selected)
	                    InputNode->Disconnect(true);
            }

            for (int i = 0; i < Outputs->Count; i++)
            {
            	Node	*OutputNode = Outputs->Items[i];

                if (OutputNode->ConnectedTo != NULL &&
                    !OutputNode->ConnectedTo->Owner->I2adlShape->Selected)
                    	OutputNode->Disconnect(true);
            }

            return;
        }
    }

    // Find all of the objects connected to this selected object.
    NodeList    *ObjectNodes = Shape->OwningI2adl->Inputs;  // Pass #1 - Inputs

    for (int IO = 0; IO < 2; IO++)
    {
        for (int j = 0; j < ObjectNodes->Count; j++)
        {
            Node    *ConnectNode = ObjectNodes->Items[j]->ConnectedTo;

            if (ConnectNode == NULL)
                continue;

            // If the connected object is also selected, then both objects will move.
            MyTShapeI2adl    *ConnectShape = ConnectNode->Owner->I2adlShape;

            if (ConnectShape->Selected & ~Single)
                continue;

            // If we are not connected to a transport, then we have a problem.
            if (ConnectNode->Owner->NameCode != NAME_TRANSPORT)
            {
                // If we can break transports, then insert 2 new transports so we can move.
                if (AllowToBreak)
                {
                    ((MyTShapeTransport *) Shape)->BreakSegment(ConnectNode);
                    ConnectNode = ObjectNodes->Items[j]->ConnectedTo;

                    // The following fixes an Access Violation when the object name exceeds
                    //     CMD_LEN (2000 characters).
                    if (ConnectNode == NULL)
                    {
                        LockedX = true;
                        LockedY = true;
                        break;
                    }

                    ConnectShape = ConnectNode->Owner->I2adlShape;
                }
                else
                {
                    // Otherwise, we are bound in both directions.
                    LockedX = true;
                    LockedY = true;

                    // No sense in checking anything else.
                    break;
                }
            }

            // If the transport is dangling, then it will move.
            Node    *Connect2Node = ConnectNode->Owner->OtherEnd(ConnectNode)->ConnectedTo;

            if (Connect2Node == NULL)
            {
                ConnectShape->MoveListAdd();
                continue;
            }

            // If the transport is connected to a selected object, then it will move.
            MyTShapeI2adl    *Connect2Shape = Connect2Node->Owner->I2adlShape;

            if (Connect2Shape->Selected & ~Single)
            {
                ConnectShape->MoveListAdd();
                continue;
            }

            // Add the transport into the DragList and set it up for dragging.
            ConnectShape->DragListAdd(ConnectNode, true);

            // If we are not connected to a second transport, then we have a problem.
            if (Connect2Node->Owner->NameCode != NAME_TRANSPORT)
            {
                // If we can break transports, then insert 2 new transports so we can move.
                if (AllowToBreak)
                {
                    ((MyTShapeTransport *) ConnectShape)->BreakSegment(Connect2Node);
                    Connect2Node = ConnectNode->Owner->OtherEnd(ConnectNode)->ConnectedTo;
                    Connect2Shape = Connect2Node->Owner->I2adlShape;
                }
                else
                {
                    // Otherwise, we are bound in one of the directions.
                    if (((MyTShapeTransport *) ConnectShape)->IsHorizontal)
                        LockedY = true;
                    else
                        LockedX = true;

                    // Don't try to drag this non-transport.
                    continue;
                }
            }

            // Start to drag the second transport.
            Connect2Shape->DragListAdd(Connect2Node, false);
        }

        ObjectNodes = Shape->OwningI2adl->Outputs;  // Pass #2 - Outputs
    }
}


void I2adlEditor::ControlDragCopyAndPaste()
{
    CopyInProcess = true;
    CopyIntoClipBoard();
    PasteFromClipBoard(CONTROL_DRAG);
}


// calls MoveLists() to do actual moving:
void I2adlEditor::ControlMoveAndDrag(TPoint &ptDiff)
{
    // We only want to move Shapes every MouseMove event if we are only moving
    //      a few of them on a small Sheet:
    int 	ListCount = DragList->Count + MoveList->Count;
    
    if (1 || ListCount < 10 || ListCount + CurrentSheet->Behavior->Count / 2 < 40)
        MoveLists(ptDiff);
    else if (ShapeMoveTimer->Enabled == false)
    // Calls MoveLists() when mouse has stopped moving; greatly hastens move operation:
        ShapeMoveTimer->Enabled = true;
}


// Resets move & drag lists.  Attempts to connect all applicable nodes and clean up transports.
void I2adlEditor::ResetMove(bool Cleanup)
{
    if (Cleanup)
        I2adlViewCleanup();

    for (int i = 0; i < CurrentSheet->Behavior->Count; i++)
    {
    	I2adl	*Object = (*CurrentSheet->Behavior)[i];

        // Repaint all of the objects because the background printing can mess them up.
      Object->I2adlShape->Paint();

        if (Object != NULL &&
        	Object->I2adlShape != NULL)
	        	Object->I2adlShape->Moving = false;
    }

    AreShapesMoving = false;
    LockedX = false;
    LockedY = false;
    MoveLimitX = 0;
    MoveLimitY = 0;
    DragList->Count = 0;
    MoveList->Count = 0;
}


void I2adlEditor::I2adlViewCleanup()
{
	DoObjectSnap = false;

    for (int i = 0; i < CurrentSheet->Behavior->Count; i++)
    {
        I2adl            *Object = (*CurrentSheet->Behavior)[i];

        if (Object->I2adlShape->Moving)
        {
            ConnectAllCompatibleNodes(Object);

            // This object may have disappeared; if so, back up one and skip:
            if (CurrentSheet->Behavior->IndexOf(Object) == -1)
            {
                i--;
                continue;
            }
        }

        // Handle 1x1 primitive transports as follows:
        // 1.  If either node is unconnected, then JustDelete it.
        // 2.  If both nodes are connected to non-transport objects, then do nothing.
        // 3.  If both nodes are connected to transports, then merge the 3 transports into 1.
        // 4.  Otherwise, delete it and JustConnect its transport to its non-transport.
        if (Object == I2adlView->ActiveTransport ||
        	Object->NameCode != NAME_TRANSPORT ||
        	Object->Height > ViewScale ||
        	Object->Width > ViewScale ||
            !Object->IsPrimitive)
        {
	    	continue;
        }

        Node    *UpstreamNode = Object->Inputs->Items[0]->ConnectedTo;
        Node    *DownstreamNode = Object->Outputs->Items[0]->ConnectedTo;
        bool	JustDelete = true;
        bool	JustConnect = false;

        // Are both nodes connected?
        if (UpstreamNode != NULL &&
        	DownstreamNode != NULL)
        {
        	JustDelete = false;  // Not case #1

            // How many transports are we connected to?
        	int		ConnectedTransportCount = 0;

            if (UpstreamNode->Owner->NameCode == NAME_TRANSPORT)
            	ConnectedTransportCount++;

            if (DownstreamNode->Owner->NameCode == NAME_TRANSPORT)
            	ConnectedTransportCount++;

            if (ConnectedTransportCount <= 0)
                continue;  // Case #2 - do nothing

            if (ConnectedTransportCount <= 1)
                JustConnect = true;  // Case #4
        }

        // Delete the transport.
        CurrentSheet->Behavior->Delete(i);
        i--;
        delete Object->I2adlShape;
        delete Object;

        if (JustDelete)
        {
        	// Case #1 - JustDelete it
            UpstreamNode->Paint();
           	DownstreamNode->Paint();
        }
        else if (JustConnect)
        {
        	// Case #4 - delete it and JustConnect its transport to its non-transport
            UpstreamNode->Connect(DownstreamNode);
            UpstreamNode->Paint();
            DownstreamNode->Paint();
        }
        else
        {
        	// Case #3 - merge the 3 transports into 1
			InitTransportDrawMode(UpstreamNode);

            // Localize to avoid access into deleted memory.
	        I2adl	*UpstreamObject = UpstreamNode->Owner;
	        I2adl	*DownstreamObject = DownstreamNode->Owner;
			I2adl	*Survivor = ConnectNodeToActiveTransport(DownstreamNode);

            if (Survivor == UpstreamObject)
				UpstreamNode->Paint();
            else if (Survivor == DownstreamObject)
				DownstreamNode->Paint();

			I2adlViewCleanup();

            // Another transport was deleted so back up again.
            if (i >= 0)
        		i--;
        }
    }

	DoObjectSnap = true;
}


// Deselect all of the objects on the current sheet.
void I2adlEditor::Deselect()
{
	// The following fixes an access violation when the empty Viva\Samples directory was
    //      deleted and TestActiveXWidget.idl was opened via a double click.
	if (CurrentSheet == NULL)
    	return;

    for (int i = 0; i < CurrentSheet->Behavior->Count; i++)
    {
        I2adl	*Object = (*CurrentSheet->Behavior)[i];

        if (Object != NULL)
        {
            MyTShapeI2adl	*ObjectShape = Object->I2adlShape;

            if (ObjectShape != NULL)
            {
                // Repaint all objects involved in the last move.
                if (ObjectShape->Selected ||
                	ObjectShape->Moving)
                {
                    ObjectShape->Moving = false;
                    ObjectShape->Selected = false;
                    ObjectShape->Invalidate();
                }
            }
        }
    }
}


// Redraw all of the objects on the current Sheet.
void I2adlEditor::RedrawSheet()
{
    for (int i = 0; i < CurrentSheet->Behavior->Count; i++)
        (*CurrentSheet->Behavior)[i]->I2adlShape->Invalidate();
}


void I2adlEditor::PaintAllNames()
{
#if VIVA_IDE
    if (MyProject == NULL || CurrentSheet == NULL)
        return;

    // Repaint the sheet name (incase an Input object's node name was on top of it).
    HDC    dc = GetDC(I2adlView->Handle);

    I2adlView->PaintWindow(dc);
    ReleaseDC(I2adlView->Handle, dc);


    for (int i = 0; i < CurrentSheet->Behavior->Count; i++)
    {
        MyTShapeI2adl    *XShape = (*CurrentSheet->Behavior)[i]->I2adlShape;

        if (XShape != NULL)
            XShape->PaintNames(DrawColor);
    }
#endif
}


//returns true if Sheet is nuked
bool I2adlEditor::ProcessSheet(I2adl *Sheet, int nChoice)
{
    switch (nChoice)
    {
        case mrYes:
            MainForm->WipTree->Selected = MainForm->ActiveWipTreeNode;
            MainForm->BtnConvertSheetClick(0);

            return (AttributeDialog->ModalResult == mrOk);

        case mrCancel:
            MainForm->WipTree->Selected = MainForm->ActiveWipTreeNode;
            return false;

        case mrNo:
            OpenBehavior = NULL;
            break;

        case DO_NOTHING:
            return true;

        default:
            break;
    }

    OpenBehavior = NULL;
    RemoveSheet(Sheet, false);
    return true;
}


// The answer returned corresponds to the question, "save changes now?".
int I2adlEditor::DoWhatToSheet(I2adl *ToBehavior)
{
    // Should try to close Sheet only if there is a library object open and not referencing self.
    if (OpenBehavior == NULL || ToBehavior == CurrentSheet)
        return DO_NOTHING;

    // Abort if we do not have both a valid behavior and a valid current Sheet:
    if (ToBehavior == NULL || CurrentSheet == NULL)
        return DO_NOTHING;

    // Prompt user only if they are not drilling into the same object and
    //      the system Object that is open is the same as that which this Sheet came from:
    if (OpenBehavior != ToBehavior->Behavior->CopyOf
    	&& OpenBehavior == CurrentSheet->Behavior->CopyOf)
    {
        if (!OpenBehavior->Equals(CurrentSheet->Behavior)) // Only if they changed it.
        {
            if (SettingFlags & SETTINGS_CLOSE_CHANGED_SHEETS)
                return (ErrorTrap(4003));
            else // If the user wants it, then just leave it open.
                return DO_NOTHING;
        }
    }

    if (CloseUnchangedSheets)
        return CLOSE_SHEET;

    return DO_NOTHING;
}


// Release the transport being drawn.
void I2adlEditor::ReleaseTransport()
{
    // If ActiveTransport has already been removed, then exit:
    if (I2adlView->ActiveTransport == NULL ||
    	I2adlView->ActiveTransport->I2adlShape == NULL)
	        return;

    // Repaint the transport and any attached nodes.
    I2adlView->ActiveTransport->I2adlShape->Invalidate();

    CurrentSheet->Behavior->Remove(I2adlView->ActiveTransport);

    // The transport has not been added to CurrentSheet so just delete it.
    //     Disconnect happens in the destructer.
    delete I2adlView->ActiveTransport->I2adlShape;
    delete I2adlView->ActiveTransport;

    I2adlView->ActiveTransport = NULL;
    ReleaseCapture();
}

// Selects all shapes in I2adlView, unless Invert is true, in which case it inverts the selection:
void I2adlEditor::SelectAll(bool Invert)
{
    // Select all objects on the current Sheet:
    int		stop = CurrentSheet->Behavior->Count;

    for (int i = 0; i < stop; i++)
    {
        MyTShapeI2adl	*ObjectShape = (*CurrentSheet->Behavior)[i]->I2adlShape;
        ObjectShape->Moving = false;
        ObjectShape->Selected = Invert ? !ObjectShape->Selected
                                       : true;
        ObjectShape->Paint();
    }
}


void I2adlEditor::LoadAsSheet(I2adl *Object)
{
    if (Object == NULL)
        return;

    if (Object->Behavior == NULL)
        return;

    TTreeNode	*WipSheet = MainForm->WipTree->Items->GetFirstNode();

    // If the behavior is already open, then use that wip Sheet.
    while (WipSheet != NULL)
    {
        I2adl	*Sheet = (I2adl *) WipSheet->Data;

        if (Sheet != NULL && Sheet->Behavior->CopyOf == Object->Behavior)
        {
            SetCurrentSheet(Sheet);

            return;
        }

        WipSheet = WipSheet->GetNext();
    }

    // Make a copy of the behavior and edit it in a new wip Sheet.
    I2adl	*dummy = NULL;
    int		OldIndex = GetNavigationListEntry(CurrentSheet, &dummy);
    I2adl	*newobject = NewWipObject(new I2adlList(Object->Behavior), Object->NameInfo->Name);

    newobject->CopyOf = Object;

	if (!ProcessSheet(CurrentSheet, DoWhatToSheet(newobject)))
    {
		newobject->Behavior->DeleteObjects();
		delete newobject->Behavior;
    	delete newobject;
    	return;
    }

    UpdateSheetNavigationList(NULL, OldIndex);

    MyProject->Wip->Add(newobject);
    SetImageIndex(MainForm->WipTree->Items->AddChildObject(MainForm->WipTree->Items->
        GetFirstNode(), newobject->NameInfo->Name, newobject), 21);
    SetCurrentSheet(newobject, true);

    // If the displaying of the sheet has automatically changed the behavior, then automatically
    //     update the original I2adl Def.
    if (!newobject->Behavior->Equals(Object->Behavior))
    {
        TStringList     *InputsNew = newobject->Behavior->GetInputs(false);
        TStringList     *InputsOrig = Object->Behavior->GetInputs(false);
        TStringList     *OutputsNew = newobject->Behavior->GetOutputs(false);
        TStringList     *OutputsOrig = Object->Behavior->GetOutputs(false);

        if (InputsNew->Count == InputsOrig->Count &&
        	OutputsNew->Count == OutputsOrig->Count)
        {
        	// Revert back any Input data sets that were changed.
            for (int i = 0; i < InputsNew->Count; i++)
            {
	            I2adl    *InputNew = (I2adl *) InputsNew->Objects[i];
	            I2adl    *InputOrig = (I2adl *) InputsOrig->Objects[i];

                InputNew->Outputs->Items[0]->DSet = InputOrig->Outputs->Items[0]->DSet;
            }

            // Revert back any Output data sets that were changed.
            for (int i = 0; i < OutputsNew->Count; i++)
            {
	            I2adl    *OutputNew = (I2adl *) OutputsNew->Objects[i];
	            I2adl    *OutputOrig = (I2adl *) OutputsOrig->Objects[i];

                OutputNew->Inputs->Items[0]->DSet = OutputOrig->Inputs->Items[0]->DSet;
            }

            // Make a copy of the new behavior and put it back into the original I2adl Def
            //     without changing the behavior pointer.
			I2adlList	*FixedSheet = new I2adlList(newobject->Behavior);

			Object->Behavior->ModifiedOnOpen = true;

            Object->Behavior->DeleteObjects();
            Object->Behavior->Count = 0;

            for (int i = 0; i < FixedSheet->Count; i++)
            	Object->Behavior->Add(FixedSheet->Items[i]);

            delete FixedSheet;
        }
        else
        {
        	// Something big has changed.  Let the user manually update the changes.
        	ErrorTrap(137);
        }

        delete InputsNew;
        delete InputsOrig;
        delete OutputsNew;
        delete OutputsOrig;
    }

    OpenBehavior = newobject->Behavior->CopyOf;
    MainForm->ActiveWipTreeNode = MainForm->WipTree->Selected;
}


// Delete the Sheet from the wip.
void I2adlEditor::RemoveSheet(I2adl *Object, bool SetCurrent)
{
    // Don't try to delete a sheet that's not in the WipTree.
    TTreeNode    *DeleteNode = MyProject->GetTreeNode(MainForm->WipTree->Items, Object);

    if (DeleteNode == NULL)
        return;

	if (CurrentSheet == Object)
    {
        // If the current sheet came from a project object, then update the graphic info.
        //     This has unchanged wip sheets reopen to the object that the user drilled into.
        I2adl	*ProjectObject = ProjectObjects->GetObject(Object->Behavior->CopyOf);

        if (ProjectObject != NULL)
            ProjectObject->Behavior->RecordGraphicInfo();

    	if (SetCurrent)
        {
            // Find the node that will become the new active node.
            //     Look down the tree until we find a node with data or the end of the tree.
            TTreeNode    *SearchNode = DeleteNode->GetNext();

            while (SearchNode != NULL && SearchNode->Data == NULL)
                SearchNode = SearchNode->GetNext();

            // Did we find a good node with data?
            if (SearchNode == NULL)
            {
                // Look up the tree until we find a node with data or the top of the tree.
                SearchNode = DeleteNode->GetPrev();

                while (SearchNode != NULL && SearchNode->Data == NULL)
                    SearchNode = SearchNode->GetPrev();
            }

            // Remove the view and display the new current Sheet.
            if (SearchNode == NULL)
            {
       	        SetCurrentSheet(NULL);
                SetCurrentSheet(CreateNewSheet());
            }
            else
            {
                SetCurrentSheet((I2adl *) SearchNode->Data, true);
            }
        }
        else
        {
	        SetCurrentSheet(NULL);
        }
    }

    // Delete the Sheet from the Wip and the WipTree.
    if (Object->Behavior->Count > 0)
        MyProject->Modified();

    DeleteNode->Delete();
    MyProject->Wip->Remove(Object);

    if (Object->Behavior->CopyOf == OpenBehavior)
        OpenBehavior = NULL;

    // Delete the Sheet itself.
    Object->Behavior->DeleteObjects();
    delete Object->Behavior;
    delete Object;
}


// Create a new empty wip Sheet.
I2adl *I2adlEditor::CreateNewSheet()
{
    I2adlList	*NewSheet = new I2adlList;
    int			Index = MyProject->Wip->Count;

    // Assign the Sheet a unique name.
    for (int i = 0; i <= Index; i++)
    {
        AnsiString		SheetName = "Sheet_" + IntToStr(i + 1);
        TTreeNode		*TNode = MyProject->GetTreeNode(MainForm->WipTree->Items, SheetName);

        if (TNode == NULL)
        {
            I2adl		*newobj = NewWipObject(NewSheet, SheetName);

            SetImageIndex(MainForm->WipTree->Items->AddChildObject(MainForm->WipTree->Items->
                GetFirstNode(), newobj->NameInfo->Name, newobj), 21);

			MyProject->Wip->Add(newobj);            

            return newobj;
        }
    }

    ErrorTrap(17);

    return NULL;
}


// Create a new wip Sheet using the name and behavior passed.
I2adl *I2adlEditor::CreateNewSheet(AnsiString SheetName, I2adlList *Behavior)
{
	I2adl	*ret = NewWipObject(Behavior, SheetName);

    MyProject->Wip->Add(ret);
    SetImageIndex(MainForm->WipTree->Items->AddChildObject(MainForm->WipTree->Items->
        GetFirstNode(), ret->NameInfo->Name, ret), 21);

    return ret;
}


// Returns the closest node to (X, Y) that is (1) unconnected, (2) compatible (opposite) with
//		NodeType and (3) within SnapTolerance pixels.
Node *I2adlEditor::CheckNodeProximity(int X, int Y, int NodeType, int SnapTolerance, bool
	SkipActiveTran)
{
	if (CurrentSheet == NULL)
    	return NULL;

    Node    *ReturnNode = NULL;
    int     ReturnNodeDistance = MAXINT;

    for (int i = 0; i < CurrentSheet->Behavior->Count; i++)
    {
        I2adl       *Object = (*CurrentSheet->Behavior)[i];
        int         HalfNodeSize = Object->NodeSize() / 2;
        NodeList    *IONodes = (NodeType == TYPE_INPUT) ? Object->Outputs
                                                        : Object->Inputs;

        if (SkipActiveTran &&
        	Object == I2adlView->ActiveTransport)
            	continue;

        for (int j = 0; j < IONodes->Count; j++)
        {
            Node    *IONode = IONodes->Items[j];

            if (IONode->ConnectedTo != NULL)
                continue;

            // Are we within SnapTolerance pixels of the center of this node?
            int     DeltaX = abs(Object->X + IONode->Left + HalfNodeSize - X);
            int     DeltaY = abs(Object->Y + IONode->Top  + HalfNodeSize - Y);

            if (DeltaX <= SnapTolerance &&
            	DeltaY <= SnapTolerance)
            {
                // This node is within tolerance.  Is it the closest?
                if ((DeltaX + DeltaY) < ReturnNodeDistance)
                {
                    ReturnNode = IONode;
                    ReturnNodeDistance = DeltaX + DeltaY;
                }
            }
        }
    }

    return ReturnNode;
}


// Tries to connect the node passed (<ThisNode>) to any nodes within range,
//      starting with the nearest nodes; returns <true> if successful.
bool I2adlEditor::ConnectNodeIfPossible(Node *ThisNode, int paramX, int paramY, bool
	SkipActiveTran)
{
    if (ThisNode->ConnectedTo != NULL)
        return false;

    I2adl	*ThisObject = ThisNode->Owner;
    int     HalfNodeSize = ThisObject->NodeSize() / 2;
    int     X = (paramX) ? paramX
                         : ThisNode->Left + ThisObject->X + HalfNodeSize;
    int     Y = (paramY) ? paramY
                         : ThisNode->Top + ThisObject->Y + HalfNodeSize;
    int     SnapTolerance = NodeSnapTolerance * ViewScale / OLD_GRID_SNAP;
    Node    *NodeToConnect = CheckNodeProximity(X, Y, ThisNode->GetType(), SnapTolerance,
    	SkipActiveTran);

    // If there is no node within range, then exit:
    if (NodeToConnect == NULL)
        return false;

    // If the two nodes in question belong to the same I2adl, then exit:
    if (NodeToConnect->Owner == ThisNode->Owner)
        return false;

    // If the prospective connecting node is already connected, then exit:
    if (NodeToConnect->ConnectedTo != NULL)
        return false;

    // Should connect to the found node only if this is the closest node to it also:
    int 	ConnectNodeX = NodeToConnect->Left + NodeToConnect->Owner->X + HalfNodeSize;
    int 	ConnectNodeY = NodeToConnect->Top + NodeToConnect->Owner->Y + HalfNodeSize;
    Node 	*NearestNode = CheckNodeProximity(ConnectNodeX, ConnectNodeY,
    	NodeToConnect->GetType(), SnapTolerance, SkipActiveTran);

    if (NearestNode && NearestNode != ThisNode)
        return false;

    // If there was no appropriate compatability, then don't try to connect.
    if (!ThisNode->IsCompatible(NodeToConnect) || !NodeToConnect->IsCompatible(ThisNode))
        return true; // So that this transport is not finalized.

    // Have to 'draw' from ThisNode:
    if (I2adlView->ActiveTransport != ThisNode->Owner)
        TerminateDraw();

    Node	*UpstreamThisNode = GetUpOrDownStreamNode(ThisNode);
    Node	*UpstreamNodeToConnect = GetUpOrDownStreamNode(NodeToConnect);

	// Override Variant data sets on Input and Output objects.
    if (UpstreamNodeToConnect->IsOnVariantIO())
    {
        if (UpstreamNodeToConnect->Owner->NameCode == NAME_INPUT)
            UpstreamNodeToConnect->Owner->Outputs->Items[0]->DSet = UpstreamThisNode->DSet;
        else
            UpstreamNodeToConnect->Owner->Inputs->Items[0]->DSet = UpstreamThisNode->DSet;

        UpstreamNodeToConnect->Paint();
    }
    else if (UpstreamThisNode->IsOnVariantIO())
    {
        if (UpstreamThisNode->Owner->NameCode == NAME_INPUT)
            UpstreamThisNode->Owner->Outputs->Items[0]->DSet = UpstreamNodeToConnect->DSet;
        else
            UpstreamThisNode->Owner->Inputs->Items[0]->DSet = UpstreamNodeToConnect->DSet;

        UpstreamThisNode->Paint();
    }

    // ConnectNodeToActiveTransport() requires that there be an active transport:
    InitTransportDrawMode(ThisNode);

    TPoint 		P; // Used to store node's logical Sheet coordinates.

    // Move the tail of the transport to this node:
    ThisNode->SheetCoordinates(P);
    I2adlView->ActiveTransport->XShape->DrawTail(P.x, P.y, true);

    // Need to get the nodes at the end of the chain:
    UpstreamThisNode = GetUpOrDownStreamNode(ThisNode);
    UpstreamNodeToConnect = GetUpOrDownStreamNode(NodeToConnect);

    // Copy the name of the node to which you are connecting to the Input/Output's node.
    //     Don't copy the node name if transports were manually drawn.
    if (ThisNode->Owner->NameCode != NAME_TRANSPORT  &&
        NodeToConnect->Owner->NameCode != NAME_TRANSPORT)
    {
        Node    *Node1 = UpstreamThisNode;
        Node    *Node2 = UpstreamNodeToConnect;

        for (int i = 0; i < 2; i++)
        {
            if ((Node1->Owner->NameCode == NAME_INPUT  && Node2->Owner->NameCode != NAME_OUTPUT) ||
                (Node1->Owner->NameCode == NAME_OUTPUT && Node2->Owner->NameCode != NAME_INPUT))
            {
                // Only change default node names to user entered node names.
                if (!Node1->ShowNodeName() && Node2->ShowNodeName())
                {
                    Node1->Owner->I2adlShape->PaintNames(WorkSpaceColor);
                    Node1->SetName(*Node2->Name);
                    Node1->Owner->I2adlShape->PaintNames(DrawColor);
                }
            }

            Node1 = UpstreamNodeToConnect;
            Node2 = UpstreamThisNode;
        }
    }

    ConnectNodeToActiveTransport(NodeToConnect);
    ResetMove(true);

    return true; // If we got this far without aborting, it means that we did connect nodes.
}


// Copy the objects selected on the current Wip sheet into the ClipBoard file.
void I2adlEditor::CopyIntoClipBoard()
{
    CopyPoint = TPoint(I2adlView->ScreenToClient(Mouse->CursorPos));
    CopyPoint = TPoint(CopyPoint.x + I2adlView->HorzScrollBar->Position, CopyPoint.y +
    	I2adlView->VertScrollBar->Position);

    // Put all of the selected objects into a new list.
    I2adlList	*SelectedObjects = new I2adlList;

    for (int i = 0; i < CurrentSheet->Behavior->Count; i++)
    {
        I2adl    *Object = (*CurrentSheet->Behavior)[i];

        if (Object->I2adlShape->Selected)
            SelectedObjects->Add(Object);
    }

    // If no objects are selected, then keep the current clip board.
    if (SelectedObjects->Count > 0)
    {
		// Call the I2adl List copy constructor to fix up the ConnectedTo pointers.
        ClipBoard->DeleteObjects();
        *ClipBoard = *SelectedObjects;

        ClipBoard->FixSelectDataSets();

        // Delete the existing ClipBoard file.
        if (FileExists(ClipBoardFileName) &&
            CheckForWrite(ClipBoardFileName, true))
                DeleteFile(ClipBoardFileName);

        // Copy the objects into the ClipBoard file.
        int		OutFileHandle = FileCreate(ClipBoardFileName);

        if (OutFileHandle >= 0)
        {
            VTextBase	ClipBoardSaver(ftSheet);

            ClipBoardSaver.OutFileHandle = OutFileHandle;
            ClipBoardSaver.OutFileName = strNull;  // Used by CloseOutputFile().

            // Create the ClipBoard wraper object and list.
            I2adlList	*ObjectsToSave = new I2adlList;
            I2adl		*ClipBoardObject = new I2adl(strI2adlEditorClipBoard);

            ObjectsToSave->Add(ClipBoardObject);
            ClipBoardObject->Attributes->Add(strClipBoard);
            ClipBoardObject->IsPrimitive = false;
            ClipBoardObject->Behavior = ClipBoard;

            ClipBoardSaver.Write(ObjectsToSave, ftSheet, "ClipBoard  ");

            delete ObjectsToSave;
            delete ClipBoardObject;

            ClipBoardSaver.CloseOutputFile();

            // Record the time stamp of our clipboard file so we can quickly tell if another
            //		Viva program has changed it.
            ClipBoardTimeStamp = FileAge(ClipBoardFileName);
        }
        else
        {
            // Inform the user that the ClipBoard file could not be created.
            ErrorTrap(13, ClipBoardFileName);

            ClipBoardTimeStamp = 0;
        }
    }

    delete SelectedObjects;
}


// Paste the objects in the ClipBoard file into the current Wip sheet.
void I2adlEditor::PasteFromClipBoard(TObject *Sender)
{
	// Safety
    if (ClipBoard == NULL ||
    	CurrentSheet == NULL)
    		return;

	// If the ClipBoard file is the one we wrote out (not changed by another Viva program),
    //		then we don't have to read it back in.
    if (FileExists(ClipBoardFileName) &&
    	ClipBoardTimeStamp != FileAge(ClipBoardFileName))
    {
        ClipBoardTimeStamp = 0;
        ClipBoard->DeleteObjects();

        MainForm->OpenDialog->FileName = ClipBoardFileName;
        MainForm->OpenFile(ftSheet, true, true);
    }

    if (ClipBoard->Count <= 0)
        return;

    if (Sender != CONTROL_DRAG)
        SaveSheetState();

    CurrentPoint = TPoint(I2adlView->ScreenToClient(Mouse->CursorPos));

    // Make sure that the user pastes inside legal bounds:
    if (CurrentPoint.x < ViewScale * 3)
        CurrentPoint.x = ViewScale * 3;

    if (CurrentPoint.y < ViewScale * 3)
        CurrentPoint.y = ViewScale * 3;

    // Unselect everything in the current Sheet:
    ResetMove();
    Deselect();

    // For paste using <Ctrl + V>, get the upper-left corner of the logical minimal
    //      rectangle that encompases all copied shapes:
    int		LowestX = MAXINT;
    int		LowestY = MAXINT;

    // Make a copy of the ClipBoard objects and paste them into the current sheet.
    I2adlList	*ClipBoardCopy = new I2adlList(ClipBoard);

    for (int i = 0; i < ClipBoardCopy->Count; i++)
    {
        I2adl    *Object = (*ClipBoardCopy)[i];

        // Fix the copied objects to be their own source (Bugzilla #835).
        Object->MasterSource = Object;

        // If WidgetTop data is duplicated, then this Object's widget will have the same
        //      location as that of the Object from which it was copied; remove this
        //      attribute now if appropriate:
        if (Object->NameCode == NAME_INPUT || Object->NameCode == NAME_OUTPUT)
        {
            // Only remove if this sheet already have objects with which to conflict:
            if (CurrentSheet->Behavior->Count > 0)
            {
                int		indexoftop = Object->Attributes->IndexOfName(strWidgetTop);

                if (indexoftop > -1)
                    Object->Attributes->Delete(indexoftop);
            }
        }

        if (LowestX > Object->X)
            LowestX = Object->X;
        if (LowestY > Object->Y)
            LowestY = Object->Y;
    }

    LowestX -= (LowestX % ViewScale);
    LowestY -= (LowestY % ViewScale);

    // Need to adjust X and Y properties of the I2adl's according to mouse and scroller
    //      positions.  This has to be done before creating any shapes so that the code
    //      within the shape constructor meant to fix things up won't mess things up.
    CurrentPoint.x -= (CurrentPoint.x % ViewScale);

    for (int i = 0; i < ClipBoardCopy->Count; i++)
    {
        I2adl    *Object = (*ClipBoardCopy)[i];

        if (Sender == CONTROL_DRAG)
        {
            CopyPoint.x -= (CopyPoint.x % ViewScale);
            Object->X += (CurrentPoint.x - CopyPoint.x);
            Object->Y += (CurrentPoint.y - CopyPoint.y);
        }
        else
        {
            Object->X += (CurrentPoint.x - LowestX - 15);
            Object->Y += (CurrentPoint.y - LowestY - 15);
        }
    }

    // Copy the objects from the ClipBoard into the current Wip sheet.
    bool		NeedToRebuildUI = false;

    for (int i = 0; i < ClipBoardCopy->Count; i++)
    {
        I2adl	*Object = (*ClipBoardCopy)[i];
        int		OrigWidth = Object->Width;

        Object->CreateShape();
        CurrentSheet->Behavior->Add(Object);
        Object->I2adlShape->Moving = false;
        Object->I2adlShape->Selected = true;
        Object->I2adlShape->Paint();

		// If the object width has changed, then we need to rebuild the attached transports.
		if (Object->Width != OrigWidth &&
        	Object->NameCode != NAME_TEXT)
        		NeedToRebuildUI = true;
    }

    if (NeedToRebuildUI)
        RebuildUI();

    // Set CopyDecided to true to prevent MouseMove event (over new I2adl object) from
    //     thinking it can copy the objects:
    CopyInProcess = true;

    // Make the pasted (selected) objects move:
    AreShapesMoving = true;
    SetCaptureControl((*ClipBoardCopy)[0]->I2adlShape);

    // If the user uses <Ctrl + V>, then we have to pretend that the most recent place that
    //     they moved this selection is its current location:
    if (Sender != CONTROL_DRAG)
        PrevPoint = TPoint(CurrentPoint.x - CurrentPoint.x % ViewScale, CurrentPoint.y -
        	CurrentPoint.y % ViewScale);

    delete ClipBoardCopy;

    MyProject->Modified();
}


void I2adlEditor::UndoTransport()
{
    // Delete Active Transport.
    I2adl    *_Object = I2adlView->ActiveTransport;
    I2adl    *AttachedI2adl = I2adlView->AttachedI2adl();

    CurrentSheet->Behavior->Remove(_Object);
    delete _Object->I2adlShape;
    delete _Object;
    I2adlView->ActiveTransport = NULL;

    // Remove dangling transport (will be replaced on the Sheet).
    if (AttachedI2adl != NULL)
    {
        I2adlView->ActiveTransport = AttachedI2adl;

        // Find attached I2adl.
        Node    *ConnectedNode = ((MyTShapeTransport *)AttachedI2adl->I2adlShape)->
            StationaryNode->ConnectedTo;

        if (ConnectedNode != NULL && ConnectedNode->Owner->NameCode == NAME_TRANSPORT)
            ((MyTShapeTransport *)ConnectedNode->Owner->I2adlShape)->
                SetMovingStatus(ConnectedNode);
    }
    else
    {
        ReleaseCapture();
    }

    MyProject->Modified();
}


void I2adlEditor::TerminateDraw()
{
    I2adlView->ActiveTransport = NULL;
}


// Does actual node connection, and transport routing and cleanup associated therewith:
I2adl *I2adlEditor::ConnectNodeToActiveTransport(Node *ConnectNode)
{
    TPoint ToPoint;

    ConnectNode->SheetCoordinates(ToPoint);

    Node	*ThisNode = I2adlView->ActiveTransport->XShape->MovingNode;
    I2adl	*ThisObject = ThisNode->Owner;
    I2adl	*ConnectObject = ConnectNode->Owner;

    //  	                    *** Object Snap ***
    //
    // We want to move the one of the objects so that the transports will be straight,
    //     if the nodes in question are close and one of the objects
    //     are not currently connected to anything (except for the 'drawing' transport).
    bool    DidSnap = false;

    if (DoObjectSnap && I2adlView->ActiveTransport->XShape->IsHorizontal)
    {
        TPoint  NodePoint;
        I2adlView->ActiveTransport->XShape->MovingNode->SheetCoordinates(NodePoint);

        int     nAdjust = NodePoint.y - (NodePoint.y
            + I2adlView->VertScrollBar->Position) % ViewScale - ToPoint.y;

        nAdjust = -nAdjust;

        if (abs(nAdjust) < ViewScale * 3.21 && nAdjust != 0)
        {
            I2adl *CurrentObject = ThisObject;
            Node  *CurrentNode   = ThisNode;
            Node  *OtherNode     = ConnectNode;

            for (int i = 0; i < 2; i++)
            {
                if (CurrentObject->NumberOfNodesConnected() == 0)
                {
                    // Erase the node names off the back canvas.
                    CurrentObject->I2adlShape->PaintNames(WorkSpaceColor);
                    CurrentObject->I2adlShape->Top = CurrentObject->I2adlShape->Top + nAdjust;

                    if (CurrentObject->NameCode == NAME_TRANSPORT)
                    {
                        MyTShapeTransport *CurrentShape = CurrentObject->XShape;

                        CurrentShape->AnchorPoint = TPoint(CurrentShape->AnchorPoint.x
                            , CurrentShape->AnchorPoint.y + nAdjust);
                    }

                    DidSnap = true;
                    break;
                }

                // Also check upstream on each of the objects involved to see if they
                //      are only connected to the intermittent transport; if so,
                //      move both that object and the transport the appropriate amount:
                if (CurrentObject->NameCode == NAME_TRANSPORT)
                {
                    Node *OtherEnd = CurrentObject->OtherEnd(CurrentNode)->ConnectedTo;
                    if (OtherEnd != NULL && OtherEnd->Owner->NumberOfNodesConnected() < 2
                        && OtherEnd->Owner != GetUpOrDownStreamNode(OtherNode)->Owner)
                    {
                        // Erase the node names off the back canvas.
                        OtherEnd->Owner->I2adlShape->PaintNames(WorkSpaceColor);
                        CurrentObject->I2adlShape->PaintNames(WorkSpaceColor);

                        OtherEnd->Owner->I2adlShape->Top = OtherEnd->Owner->I2adlShape->Top + nAdjust;
                        CurrentObject->I2adlShape->Top = CurrentObject->I2adlShape->Top + nAdjust;

                        if (CurrentObject->NameCode == NAME_TRANSPORT)
                        {
                            MyTShapeTransport *CurrentShape = CurrentObject->XShape;

                            CurrentShape->AnchorPoint = TPoint(CurrentShape->AnchorPoint.x
                                , CurrentShape->AnchorPoint.y + nAdjust);
                        }

                        if (ConnectObject->NameCode == NAME_TRANSPORT)
                        {
                            MyTShapeTransport *CurrentShape = ConnectObject->XShape;

                            CurrentShape->AnchorPoint = TPoint(CurrentShape->AnchorPoint.x
                                , CurrentShape->AnchorPoint.y + nAdjust);
                        }

                        DidSnap = true;
                        break;
                    }
                }

                CurrentObject = ConnectObject;
                CurrentNode   = ConnectNode;
                OtherNode     = ThisNode;
                nAdjust       = -nAdjust; // going the other way
            }
        }
    }

    ConnectNode->SheetCoordinates(ToPoint); // Object Snap may have changes position of target.

    I2adlView->ActiveTransport->XShape->DrawTail(ToPoint.x, ToPoint.y, true);
    MergeRedundantTransports(ToPoint.x, ToPoint.y);

    ThisNode = I2adlView->ActiveTransport->XShape->MovingNode;
    ThisObject = ThisNode->Owner;
    ConnectObject = ConnectNode->Owner;

    // Auto Route; should not be performed in conjunction with Object Snap:
    if (!DidSnap)
    {
        TPoint  NodePoint;
        I2adlView->ActiveTransport->XShape->MovingNode->SheetCoordinates(NodePoint);

        TPoint p1, p2;
        GetUpOrDownStreamNode(ConnectNode)->SheetCoordinates(p1);
        GetUpOrDownStreamNode(ThisNode)->SheetCoordinates(p2);

        if (    (I2adlView->ActiveTransport->XShape->IsHorizontal && NodePoint.y != ToPoint.y)
             || (!I2adlView->ActiveTransport->XShape->IsHorizontal && NodePoint.x != ToPoint.x))
        {
            MyTShapeTransport *XShape = I2adlView->ActiveTransport->XShape;

            TerminateDraw();
            XShape->Selected = true;
            StartToMove(NodePoint.x, NodePoint.y);
            XShape->Moving = false;
            DetermineMoveDragSingle(TShiftState(), ToPoint, XShape);
            XShape->Selected = false;
        }
    }

    ThisNode->Connect(ConnectNode);

    Node    *OutputNode = (ThisNode->GetType() == TYPE_OUTPUT) ? ThisNode : ConnectNode;
    Node    *UpstreamOutputNode = GetUpOrDownStreamNode(OutputNode);
    Node    *DownstreamInputNode = GetUpOrDownStreamNode(OutputNode->ConnectedTo);
    Node    *DSetNode = (UpstreamOutputNode->IsOnVariantIO() ||
                        (UpstreamOutputNode->Owner->NameCode == NAME_TRANSPORT &&
                         DownstreamInputNode->Owner->NameCode != NAME_TRANSPORT))
                      ? DownstreamInputNode
                      : UpstreamOutputNode;
    Node	*UseNode  = (UpstreamOutputNode->Owner->NameCode == NAME_TRANSPORT)
            		  ? UpstreamOutputNode
    				  : UpstreamOutputNode->ConnectedTo;

	// Since PropagateOperation() only goes downstream, we have to update a Variant input horn
    //		manually.
	if (UpstreamOutputNode->IsOnVariantIO() &&
    	UpstreamOutputNode->DSet != DSetNode->DSet)
    {
    	UpstreamOutputNode->DSet = DSetNode->DSet;
        UpstreamOutputNode->Paint();
    }

    PropagateOperation(UseNode, poDataSets, DSetNode->DSet);

    // Do we need to combine with the transport we just connected to?
    if (ThisObject->NameCode == NAME_TRANSPORT &&
        ConnectObject->NameCode == NAME_TRANSPORT &&
            ((MyTShapeTransport *)ThisObject->I2adlShape)->IsHorizontal
             == ((MyTShapeTransport *)ConnectObject->I2adlShape)->IsHorizontal)
    {
        if (ConnectNode != ThisNode->ConnectedTo)
            ConnectNode = ThisNode->ConnectedTo;

        Node	*StationaryNode = ConnectNode->Owner->OtherEnd(ConnectNode);

        ThisNode->Connect(StationaryNode->ConnectedTo);

        ((MyTShapeTransport *) ThisObject->I2adlShape)->SetUpNodeForDrag(ThisNode);
        ((MyTShapeTransport *) ThisObject->I2adlShape)->Drag(
             ConnectNode->Left - StationaryNode->Left,
             ConnectNode->Top - StationaryNode->Top);

        CurrentSheet->Behavior->Remove(ConnectNode->Owner);
        delete ((MyTShapeTransport *) ConnectNode->Owner->I2adlShape);
        delete ConnectNode->Owner;
    }

    TerminateDraw();

    return ThisNode->Owner;
}


// Connects all of ThisObject's nodes that can be connected.
//		Returns true if one or more nodes were connected.
//
// Warning!  If ThisObject is a transport, then it might be deleted!
bool I2adlEditor::ConnectAllCompatibleNodes(I2adl *ThisObject, int X, int Y, bool
	SkipActiveTran)
{
	// Was an object provided?
    if (ThisObject == NULL)
        return false;

    // See if it is a transport now because ConnectNodeIfPossible() might delete it.
    bool    IsTransport = (ThisObject->NameCode == NAME_TRANSPORT);
    bool    WasNodeConnected = false;

    // Pass #1 - Inputs
    NodeList	*IONodes = ThisObject->Inputs;

    for (int i = 0; i < 2; i++)
    {
	    // Connect all of the nodes to any compatible node within range.
        //		Localize the node count because ConnectNodeIfPossible() can delete transports.
        int		ConnectNodeCount = IONodes->Count;

        for (int j = 0; j < ConnectNodeCount; j++)
        {
            if (ConnectNodeIfPossible(IONodes->Items[j], X, Y, SkipActiveTran))
	        {
                WasNodeConnected = true;

                // Safeguard against a disappearing transport.
                if (IsTransport)
                    return true;
        	}
        }

        // Pass #2 - Outputs
        IONodes = ThisObject->Outputs;
    }

    // Make sure the transport nodes are on top.
    if (WasNodeConnected)
        ThisObject->BringShapeToFront();

    return WasNodeConnected;
}


void I2adlEditor::SnapI2adlToGrid(MyTShapeI2adl *I2adlToSnap)
{
    I2adlToSnap->Left = I2adlToSnap->Left
    	- (I2adlToSnap->Left + I2adlView->HorzScrollBar->Position) % ViewScale;
    I2adlToSnap->Top = I2adlToSnap->Top
    	- (I2adlToSnap->Top + I2adlView->VertScrollBar->Position) % ViewScale;
}


// Checks up/down stream thru tranpsorts and junctions to return the Node at the other end.
//     If StartNode is an Input, then we look downstream.  Otherwise, we look upstream.
Node *I2adlEditor::GetUpOrDownStreamNode(Node *StartNode, bool StopAtJunction)
{
    if (StartNode->Owner->NameCode != NAME_TRANSPORT)
        return StartNode;

    Node    *NextNode = StartNode;
    Node    *CurrentNode = StartNode;
    bool	LookingDownstream = (StartNode->GetType() == TYPE_INPUT);

    // Iterate up/down stream.
    while(NextNode != NULL)
    {
        CurrentNode = NextNode;

        I2adl	*CurrentObject = CurrentNode->Owner;

        // If this object is not a transport, then we may be done.
        if (CurrentObject->NameCode != NAME_TRANSPORT)
            if (StopAtJunction || CurrentObject->NameCode != NAME_JUNCTION)
	            return CurrentNode;

        if (LookingDownstream)
            NextNode = CurrentObject->Outputs->Items[0]->ConnectedTo;
        else
            NextNode = CurrentObject->Inputs->Items[0]->ConnectedTo;
    }

    return CurrentNode;
}


void I2adlEditor::PropagateOperation(Node *CurrentNode, int OperationCode, void *Value)
{
    // Iterate thru the transport chain, changing the datasets of the nodes on either end:
    while(CurrentNode != NULL)
    {
        if (CurrentNode->Owner->NameCode == NAME_JUNCTION)
        {
            NodeList    *_IONodes = CurrentNode->Owner->Inputs;

            for (int IO = 0; IO < 2; IO++) // for each NodeList
            {
                for (int i = 0; i < _IONodes->Count; i++)
                {
                    Node    *CurrNode = _IONodes->Items[i];

                    if (CurrNode != CurrentNode && CurrNode->ConnectedTo != NULL)
                        PropagateOperation(CurrNode->ConnectedTo, OperationCode, Value);
                }

                _IONodes = CurrentNode->Owner->Outputs;
            }
        }

        NameCodes    _NameCode = CurrentNode->Owner->NameCode;

        switch (OperationCode)
        {
            case poSelect:
            {
                if (_NameCode == NAME_TRANSPORT || _NameCode == NAME_JUNCTION)
                    CurrentNode->Owner->I2adlShape->Selected = (bool) Value;

                break;
            }

            case poInvalidate:
            {
                CurrentNode->Owner->I2adlShape->Invalidate();
                break;
            }

            case poDataSets:
            {
                int         OldDataSetCount = ProjectDataSets->Count;
                DataSet     *NewDS = ((DataSet *) Value);                

				if (_NameCode == NAME_SSELECT)
                {
                    // $Select - Output must be variant unless both inputs match.
                    //     Figure out the connections to the "0" and "1" nodes, and the
                    //     datasets of the nodes connected thereto:
                    Node      *ConnectedTo0 = CurrentNode->Owner->Inputs->Items[0]->ConnectedTo;
                    Node      *ConnectedTo1 = CurrentNode->Owner->Inputs->Items[1]->ConnectedTo;
                    DataSet   *DataSet0 = ConnectedTo0 != NULL ? ConnectedTo0->DSet
                                                               : VariantDataSet;
                    DataSet   *DataSet1 = ConnectedTo1 != NULL ? ConnectedTo1->DSet
                                                               : VariantDataSet;
                    DataSet   *UniformDSet = VariantDataSet;

                    // Change the DataSet output to match a single input if it is the only
                    //      connected, or the other is connected but their DataSet match;
                    //      otherwise, make it Variant.  Propagate the said DataSet downstream:
                    if (DataSet0 == DataSet1 || (ConnectedTo0 != NULL && ConnectedTo1 == NULL))
                        UniformDSet = DataSet0;
                    else if ((ConnectedTo1 != NULL && ConnectedTo0 == NULL))
                        UniformDSet = DataSet1;

                    Node *Output = CurrentNode->Owner->Outputs->Items[0];

                    Output->DSet = UniformDSet;

                    if (Output->ConnectedTo != NULL)
                        Output->ConnectedTo->PropagateDataSet(UniformDSet);

                    Output->Paint();
                }
                else if (_NameCode == NAME_SYSCAST)
                {
                    // The propagation led to a $Cast object.  Determine if the output node's
                    //     data set should change.  If yes, propagate it downstream.
                    CurrentNode->DSet = NewDS;

                    DataSet    *NewOutputDataSet = CurrentNode->Owner->BuildCastOutputDataSet();
                    Node       *OutputNode = CurrentNode->Owner->Outputs->Items[0];

                    if (OutputNode->DSet != NewOutputDataSet)
                    {
                        OutputNode->DSet = NewOutputDataSet;
                        OutputNode->Paint();
                        if (OutputNode->ConnectedTo != NULL)
                            OutputNode->ConnectedTo->PropagateDataSet(NewOutputDataSet);
                    }
                }
                else if (_NameCode == NAME_TRANSPORT || _NameCode == NAME_JUNCTION)
                {
                    CurrentNode->Owner->ChangeDataSets(NewDS);
                }
                else if (_NameCode == NAME_REF)
                {
                    if (CurrentNode != CurrentNode->Owner->Inputs->Items[1])
                        break;

                    Node    *OutputNode = CurrentNode->Owner->Outputs->Items[1];

                    CurrentNode->DSet = NewDS;

					if (NewDS == SGDataSets[sdsVariant])
	                    OutputNode->DSet = NewDS;
					else
	                    OutputNode->DSet = GetOrMakePtrDS(NewDS->Name, NewDS);

                    OutputNode->Paint();

                    if (OutputNode->ConnectedTo != NULL)
                        OutputNode->ConnectedTo->PropagateDataSet(OutputNode->DSet);
                }
                else if (_NameCode == NAME_DEREF)
                {
                    Node    *InputNode = CurrentNode->Owner->Inputs->Items[1];

                    if (CurrentNode != InputNode)
                        break;

                    Node    *OutputNode = CurrentNode->Owner->Outputs->Items[1];

                    CurrentNode->DSet = NewDS;

                    if (NewDS->PointsToDS != NULL)
                        OutputNode->DSet = NewDS->PointsToDS;
                    else
                        OutputNode->DSet = NewDS;

                    OutputNode->Paint();
                    
                    if (OutputNode->ConnectedTo != NULL)
                        OutputNode->ConnectedTo->PropagateDataSet(OutputNode->DSet);
                }
                else if (_NameCode == NAME_OUTPUT)
                {
                	CurrentNode->Owner->Inputs->Items[0]->DSet = NewDS;
                }

                if (OldDataSetCount != ProjectDataSets->Count)
                    MyProject->BuildTreeView();  // Required by BuildCastOutputDataSet().

                break;
            }

            default:
            {
                break;
            }
        }

        if (CurrentNode->Owner->I2adlShape != NULL)
        	CurrentNode->Owner->I2adlShape->Invalidate();

        // We are done if the current I2adl is not a transport; all other cases are self-sufficient:
        if (_NameCode != NAME_TRANSPORT)
            return;

        CurrentNode = CurrentNode->Owner->Outputs->Items[0]->ConnectedTo;
    }
}


// This routine handles all of the cases of clicking on a node, an object or the background
//		while drawing a transport.
void I2adlEditor::HandleTransportDraw(TMouseButton Button, int X, int Y)
{
    // We must be drawing a transport.
    if (I2adlView->ActiveTransport == NULL)
    	return;

    // A right click discards the transport being drawn.
    if (Button == mbRight)
    {
        ReleaseTransport();
        return;
    }

	// Because we may have clicked on the ActiveTransport, see if there is any other
    //		object/node at this location.
    I2adlView->ActiveTransport->I2adlShape->Enabled = false;

	I2adl		 		*ObjectClicked = NULL;
	Node 		 		*NodeClicked = NULL;
    MyTShapeTransport	*ShapeClicked = (MyTShapeTransport *) I2adlView->ControlAtPos(
    	TPoint(X, Y), false, true);

    I2adlView->ActiveTransport->I2adlShape->Enabled = true;

    if (ShapeClicked != NULL)
    {
		ObjectClicked = ShapeClicked->OwningI2adl;

        // Clicks right next to a node are consider as on the node.  This prevents a 5x5
        //     transport that can be deleted in its own event handler.
        NodeClicked = ShapeClicked->GetNodeAt(ShapeClicked->ScreenToClient(I2adlView->
        	ClientToScreen(TPoint(X, Y))), ViewScale);
    }

    // Save the original X & Y.
    int    OrigX = X;
    int    OrigY = Y;

    X -= X % ViewScale - 1;
    Y -= Y % ViewScale;

    // If the user clicked on a transport while drawing from a compatible input node, then
    //      try to insert a junction and connect up to it.  IsCompatible() ensures the
    //		transport does not come from or lead to a common source/sink.
    MyTShapeTransport	*XShape = I2adlView->ActiveTransport->XShape;

    if (ObjectClicked != NULL &&
    	ObjectClicked->NameCode == NAME_TRANSPORT &&
        XShape->MovingNode->GetType() == TYPE_INPUT &&
        ObjectClicked->Inputs->Items[0]->IsCompatible(XShape->MovingNode) &&
        ObjectClicked->Outputs->Items[0]->IsCompatible(XShape->MovingNode))
    {
        // Calculate the DeltaX/DeltaY that will cause the closest node to get connected.
        int		DeltaX = 0;
        int		DeltaY = 0;
        int		AnchorTop = XShape->Top + XShape->StationaryNode->Top;
        int		AnchorLeft = XShape->Left + XShape->StationaryNode->Left;
        Node	*XShapeConneced = XShape->StationaryNode->ConnectedTo;
	    TPoint	ShapePoint = ShapeClicked->ScreenToClient(I2adlView->ClientToScreen(TPoint(X,
        	Y)));

        if (XShapeConneced != NULL &&
            XShapeConneced->Owner->NameCode == NAME_TRANSPORT)
        {
            Node   *AnchorNode = XShapeConneced->Owner->OtherEnd(XShapeConneced);

            AnchorTop = XShapeConneced->Owner->I2adlShape->Top + AnchorNode->Top;
            AnchorLeft = XShapeConneced->Owner->I2adlShape->Left + AnchorNode->Left;
        }

        if (ShapeClicked->Left + ShapePoint.x > AnchorLeft)
            DeltaX = -ViewScale * 1.5;
        else if (ShapeClicked->Left + ShapePoint.x < AnchorLeft)
            DeltaX = ViewScale * 1.5;

        if (ShapeClicked->Top + ShapePoint.y > AnchorTop)
            DeltaY = -ViewScale * 1.5;
        else if (ShapeClicked->Top + ShapePoint.y < AnchorTop)
            DeltaY = ViewScale * 1.5;

        if (NodeClicked == NULL)
        {
        	// The user clicked on the body of the transport.
            //		"- ViewScale" is required because InsertJunction() rounds up.
            ShapeClicked->InsertJunction(true, ShapePoint.x - ViewScale, ShapePoint.y -
            	ViewScale, true);

            OrigX += DeltaX;
            OrigY += DeltaY;
        }
        else
        {
        	// The user clicked on the transport's node.  If this is an eblow, then insert a
            //		junction.
            Node	*ConnectedNode = NodeClicked->ConnectedTo;

            if (ConnectedNode != NULL &&
                ConnectedNode->Owner->NameCode == NAME_TRANSPORT)
            {
                ShapeClicked->InsertJunction(true, NodeClicked->Left, NodeClicked->Top, true,
                	ConnectedNode->Owner, NodeClicked, ConnectedNode);

	            OrigX += DeltaX;
	            OrigY += DeltaY;
            }
        }
    }

    // The user wants the transport to go to this location.
    //		If there is an unconnected compatable node close by, then connect up to it.
    //		Otherwise, finalize the ActiveTransport and start a new transport off the end of
    //		it.
    if (I2adlView->ActiveTransport != NULL &&
    	!ConnectAllCompatibleNodes(I2adlView->ActiveTransport,
    	OrigX + I2adlView->HorzScrollBar->Position,
        OrigY + I2adlView->VertScrollBar->Position))
    {
        I2adlView->ActiveTransport->XShape->DrawTail(OrigX, OrigY);
        MergeRedundantTransports(OrigX, OrigY);

        StartTransport(I2adlView->ActiveTransport->XShape->MovingNode);
    }
}


void I2adlEditor::MouseMove(TShiftState Shift, int X, int Y)
{
    if (TextI2adlMemo->Visible)
        return;

    // Because the mouse is not over an I2adl Object, remove the object information from the
    //     status bar.
    if (NewCaptionSource(NULL))
        SetStatusCaption(strNull, false);

    if (I2adlView->ActiveTransport != NULL)
    {
        ((MyTShapeTransport *)I2adlView->ActiveTransport->I2adlShape)->DrawTail(X, Y);
        ScrollBarCage(X, I2adlView->HorzScrollBar);
        ScrollBarCage(Y, I2adlView->VertScrollBar);
    }
    else if (Shift.Contains(ssLeft) && SelectRectTop->Visible)
    {
        I2adlView->DrawSelectRect(X, Y);
    }

    if (AreShapesMoving)
        ResetMove();
}


void I2adlEditor::MouseUp(TMouseButton Button, TShiftState Shift, int X, int Y)
{
	if (CurrentSheet == NULL)
    	return;

    if (I2adlView->ActiveTransport != NULL)
    {
        if (!ConnectAllCompatibleNodes(I2adlView->ActiveTransport,
        	X + I2adlView->HorzScrollBar->Position,
            Y + I2adlView->VertScrollBar->Position))
        {
	       ((MyTShapeTransport *) I2adlView->ActiveTransport->I2adlShape)->DrawTail(X, Y);
        }

        return;
    }

    if (!(Shift.Contains(ssShift) || Shift.Contains(ssCtrl)))
        Deselect();

    if (SelectRectTop->Visible)
    {
        bool    InvertSelection = Shift.Contains(ssCtrl);
        bool	SelectedAnObject = false;

        I2adlView->DrawSelectRect(X, Y);

        // now look for all the controls and see if they are in this rect
        for (int i = 0; i < CurrentSheet->Behavior->Count; i++)
        {
            MyTShapeI2adl    *ChildControl = (*CurrentSheet->Behavior)[i]->I2adlShape;

            int    ChildRight = ChildControl->Left + ChildControl->Width;
            int    ChildBottom = ChildControl->Top + ChildControl->Height;

            // Is the object within 5 pixels of being inside the selection rectangle?
            //     Use 6 pixels on the horizontal because objects are one off the gird snap.
            if (ChildControl->Left + ViewScale * 1.2 >= SelectRectLeft->Left  &&
                ChildControl->Top + ViewScale >= SelectRectTop->Top  &&
                ChildRight - ViewScale * 1.2 <= SelectRectRight->Left + SELECT_RECT_WIDTH  &&
                ChildBottom - ViewScale <= SelectRectBottom->Top + SELECT_RECT_WIDTH)
            {
                // We are in the rect; select us:
                SelectedAnObject = true;

                if (InvertSelection)
                    ChildControl->Selected = !ChildControl->Selected;
                else
                    ChildControl->Selected = true;

                ChildControl->Invalidate();
            }
        }

        SelectRectTop->Visible = false;
        SelectRectLeft->Visible = false;
        SelectRectRight->Visible = false;
        SelectRectBottom->Visible = false;
        I2adlView->Update();
        PaintAllNames();

        // If the Alt+SelectBox did not enclose an object, then don't change the zoom percent.
        if (SelectedAnObject && Shift.Contains(ssAlt))
        {
			int		OldViewScale = ViewScale;
			TRect 	ExtentRect = ComputeCurrentSheetExtent(true);

			if (ExtentRect.Right != 0 && ExtentRect.Bottom != 0)
            {
	            MainForm->FitToSize(ExtentRect);

    	        ExtentRect.Left = (ExtentRect.Left * ViewScale) / OldViewScale;
        	    ExtentRect.Top = (ExtentRect.Top * ViewScale) / OldViewScale;
            	ExtentRect.Right = (ExtentRect.Right * ViewScale) / OldViewScale;
	            ExtentRect.Bottom = (ExtentRect.Bottom * ViewScale) / OldViewScale;

				int  NewHPos = ExtentRect.Left - (I2adlView->Width - ExtentRect.Width()) / 2;
        	    int  NewVPos = ExtentRect.Top - (I2adlView->Height - ExtentRect.Height()) / 2;

                // Make sure the scrollbars are on the grid snap.
        	    NewHPos -= NewHPos % ViewScale;
            	NewVPos -= NewVPos % ViewScale;

	            EnsureScrollRange(I2adlView->HorzScrollBar, NewHPos + I2adlView->Width);
    	        EnsureScrollRange(I2adlView->VertScrollBar, NewVPos + I2adlView->Height);

        	    I2adlView->HorzScrollBar->Position = NewHPos;
            	I2adlView->VertScrollBar->Position = NewVPos;
            }
        }
    }
}


void I2adlEditor::DragOver(TObject *Sender, TObject *Source, int X, int Y, TDragState State
    , bool &Accept)
{
    TTreeNode	*ActiveTreeNode = MainForm->ActiveTreeNode;

    if (DropObject != NULL)
    {
        Accept = true;

        POINT	P = Point(X - X % ViewScale, Y - Y % ViewScale);

        if (DropObject->X == P.x && DropObject->Y == P.y)
        {    // Don't reprocess until we have moved enough to make it worth our while.
            P.x++;
            return;
        }

        I2adl	*UObject = DropObject;

        if (UObject->X == P.x)
        {
            P.x++;
            return;
        }

        P = DropObject->I2adlShape->ScreenToClient(I2adlView->ClientToScreen(P));
        DropObject->I2adlShape->MouseMove(TShiftState() << ssLeft, P.x, P.y);

        return;
    }

    Accept = false;

    int		W = I2adlView->Width;
    int		Margin = W - (W >> 4) - 8;    // We do not want to pan right!

    if (X >= Margin)
        return;

    I2adl	*NewFunction = NULL;

    if (Source == MainForm->ComClassTree)
    {
        TTreeNode *Selected = MainForm->ComClassTree->Selected;

        if (Selected == NULL)
            return;

        if (Selected->Level == 1)
        {
            TComObjectInfo		*ObjectInfo = (TComObjectInfo *) Selected->Data;

            if (ObjectInfo->TypeAttr->typekind == TKIND_ENUM || ObjectInfo->ProgID == NULL)
                return;

            Accept = true;
            NewFunction = new I2adl((TComObjectInfo *) Selected->Data);
        }
        else if (Selected->Level == 2)
        {
            TComObjectInfo *ObjectInfo = (TComObjectInfo *) Selected->Parent->Data;

            if (ObjectInfo->TypeAttr->typekind == TKIND_ENUM)
            {
                Accept = (((TMemberDesc *) Selected->Data)->VarDesc->lpvarValue != NULL);

                if (!Accept)
                    return;

                TMemberDesc *MemberDesc = (TMemberDesc *) Selected->Data;

                NewFunction = new I2adl(NAME_INPUT);
                NewFunction->SetAttribute(strConstant, (int) MemberDesc->VarDesc->lpvarValue->lVal);
                NewFunction->Outputs->Items[0]->SetName(Selected->Text);
            }
            else
            {
                Accept = true;
                NewFunction = new I2adl(new TMemberDesc((TMemberDesc *) Selected->Data), NULL);
            }
        }
        else
        {
            Accept = false;
            return;
        }
    }
    else if (Source == MainForm->ProjectObjectTree ||
    		 Source == MainForm->SystemObjectTree ||
             Source == MainForm->ComObjectTree)
    {
        if (ActiveTreeNode == NULL)
			return;

        // Create the new object and its Shape:
        if (MainForm->ObjectTreePageControl->ActivePage == MainForm->ComTabSheet)
        {
            if (ActiveTreeNode->Level == 1)
            {
                if (ActiveTreeNode->Data != NULL)
	                NewFunction = new I2adl((TComObject *) ActiveTreeNode->Data);
                else
                	return;
            }
            else if (ActiveTreeNode->Level == 2)
            {
                NewFunction = new I2adl(new TMemberDesc((TMemberDesc *) ActiveTreeNode->Data),
                (TComObject *) ActiveTreeNode->Parent->Data);
            }
            else
            {
                return;
            }

            Accept = true;
        }
        else
        {
            // Do not allow library name or subsystem nodes to be dropped onto the sheet.
			if (ActiveTreeNode->ImageIndex == LIBRARY_NODE_IMAGE_INDEX ||
        		ActiveTreeNode->ImageIndex == SUBSYSTEM_NODE_IMAGE_INDEX)
					return;

            // Get a copy of the I2adl object being dragged.
            if (ActiveTreeNode->Data == TREE_GROUP_NODE)
            {
                // This is a tree group.  Is it inside Composite Objects or Libraries?
                if (!InsideRootTreeNode(strCompositeObjects, ActiveTreeNode, true) &&
                	!InsideRootTreeNode(strLibraries, ActiveTreeNode, true))
	                    return;

                // Create a "<TreeGroup>" text object.
                NewFunction = new I2adl(NAME_TEXT);

                TextI2adlMemo->InitAttributes(NewFunction);
                NewFunction->AttSystem = SystemStringTable->QString(str_TreeGroup_ +
                    MainForm->GetTreeGroupName(ActiveTreeNode));
            }
            else if (ActiveTreeNode->Data != FOOTPRINT_TREE_NODE)
            {
            	// This is a simple object node.
                NewFunction = new I2adl((I2adl *) ActiveTreeNode->Data, cnYesDisconnect,
                	true);
            }
            else
            {
                // Create a "most Variant" footprint object.
                TTreeNode    *ANode = ActiveTreeNode->getFirstChild();

                if (ANode->Data == NULL)    // not a composite variant subtree
                    return;

                int          VCount = 0;
                I2adlList    *Candidates = new I2adlList;

                NewFunction = new I2adl((I2adl *) ANode->Data, cnYesDisconnect, true);

                I2adl    *_Candidate;

                do
                {
                    _Candidate = (I2adl *) ANode->Data;

                    int    UCount = NodeCompareUpdate(NewFunction->Inputs, _Candidate->Inputs) * 1024;

                    UCount += NodeCompareUpdate(NewFunction->Outputs, _Candidate->Outputs);

                    if (VCount > UCount)
                        continue;

                    if (VCount < UCount)
                    {
                        Candidates->Count = 0;
                        VCount = UCount;
                    }

                    Candidates->Add(_Candidate);
                }
                while ((ANode = ActiveTreeNode->GetNextChild(ANode)) != NULL);

                // If more than one MaxVariant, then select highest level one
                for (int i = 0; i < Candidates->Count && Candidates->Count > 1; i++)
                {
                    _Candidate = (*Candidates)[i];

                    I2adlList    *_Behavior = _Candidate->Behavior;

                    if (_Behavior == NULL)
                        continue;

                    for (int j = 0; j < _Behavior->Count; j++)
                    {
                        I2adl    *BehaviorObject = (I2adl *) _Behavior->Items[j];

                        if (BehaviorObject->NameInfo != NewFunction->NameInfo)
                            continue;

                        // We have a recursive call to Object with the same name.
                        // Give precedence to "higher behavior"
                        // Make sure not self-referential
                        if ((void *) BehaviorObject->Behavior == (void *) _Behavior)
                            continue;

                        // if on list, eliminate;
                        for (int jdx = 0; jdx < Candidates->Count; jdx++)
                        {
                            if (jdx == i)
                                continue;

                            I2adl    *XCandid = (*Candidates)[jdx];

                            // Compare pointers
                            if ((void *) XCandid->Behavior != (void *) BehaviorObject->Behavior)
                                continue;

                            // We found it on list.  Eliminate it.
                            Candidates->Delete(jdx);

                            if (i >= jdx)
                                i--;

                            break;
                        }
                    }
                }

                _Candidate = (*Candidates)[0];
                NewFunction->Behavior = _Candidate->Behavior;
                delete Candidates;
            }
        }
    }
    else
    {
        return;
    }

    // The following makes sure the user is reminded of any unresolved node name mismatches.
    NewFunction->Warned4061 = false;

    Deselect();

	POINT	P = I2adlView->ScreenToClient(Mouse->CursorPos);

    P.x -= P.x % ViewScale;
    P.y -= P.y % ViewScale;

    NewFunction->X = P.x;
    NewFunction->Y = P.y;

    if (NewFunction->NameCode == NAME_TEXT)
        NewFunction->Width = ViewScale * 50;

    // Set the size before dragging onto the sheet
    NewFunction->Conformalize();
    NewFunction->CreateShape();

    MyTShapeI2adl	*NewShape = NewFunction->I2adlShape;
    
    // We want the shape to be under the mouse.
    StartToMove(NewShape->Left + NewShape->Width / 2, NewShape->Top + NewShape->Height / 2);
    NewShape->Selected = true;
    NewShape->Moving = false;

    CurrentSheet->Behavior->Add(NewFunction);
    DropObject = NewFunction;

    AreShapesMoving = true;

    Accept = true;
}


void I2adlEditor::UpdateWorkspaceColor()
{
    WorkSpaceColor = (TColor) NewWorkSpaceColor;
    SetDrawColor();

    I2adlView->Brush->Color = WorkSpaceColor;
    I2adlView->Invalidate();
    I2adlView->Update();
    PaintAllNames();

    for (int i = 0; i < CurrentSheet->Behavior->Count; i++)
    {
        I2adl    *Object = (*CurrentSheet->Behavior)[i];

        if (Object->I2adlShape)
        {
            Object->I2adlShape->GetCanvas()->Brush->Color = WorkSpaceColor;
            Object->I2adlShape->Invalidate();
        }
    }
}


void I2adlEditor::UpdateSelectionColor()
{
    SelectionColor = NewSelectionColor;

	Screen->Cursor = crHourGlass;

	for (int i = 0; i < I2adlStringTable->Count; i++)
    {
    	VivaIcon	*icon = (VivaIcon *) I2adlStringTable->Items[i];

        icon->CreateSelectedIcon();
    }

	Screen->Cursor = crDefault;

    RedrawSheet();
}


// Convert window coordinates coordinates to absolute coordinates.
// Expand work space to contain object and add a little "room" for growth.
int I2adlEditor::EnsureScrollRange(TControlScrollBar *Scroller, int Data)
{
	// The extra 2 pixels fixes the bug of scroll bars not appearing on new projects.
    int    Extension = Data + SCROLLRANGEBUFFER + 2;

    if (Scroller->Range < Extension)
        Scroller->Range = Extension;

    return Data;
}


//  Pan routine.  Adjust scroll bar position as needed.
//  Also cages the mouse and object position to lie within the viewable screen
//  Returns the pan increment.
int I2adlEditor::ScrollBarCage(int Pos, TControlScrollBar *Scroller)
{
    EnsureScrollRange(Scroller, Pos + Scroller->Position);

    int		Limit;

    if (Scroller == I2adlView->HorzScrollBar)
        Limit = I2adlView->Width;
    else
        Limit = I2adlView->Height;

    int    Pan, Margin = SCROLLRANGEBUFFER - ViewScale;//Limit >> 4;

    Limit -= Margin;

    if (Pos > Limit)
    {
        Pan = Pos - Limit;
        Pan -= (Pan % ViewScale) - ViewScale;
    }
    else if (Pos < Margin && Scroller->Position > 0)
    {
        Pan = Pos - Margin;
        Pan -= (Pan % ViewScale) + ViewScale;
        Pan = max(Pan, -Scroller->Position);
    }
    else // No need to pan
        return 0;

    // We are panning .. hold the mouse constant while moving the window.
    Scroller->Position = Scroller->Position + Pan;
    Scroller->Position = Scroller->Position - Scroller->Position % ViewScale;

    TPoint		MPos = Mouse->CursorPos;

    if (Scroller == I2adlView->HorzScrollBar)
        MPos.x -= Pan;
    else
        MPos.y -= Pan;

    Mouse->CursorPos = MPos;

    return Pan;
}


// Does the actual moving of a selection of Shapes:
void I2adlEditor::MoveLists(TPoint ptDiff, bool Single)
{
    // Adjust the MoveLimits from I2adlView coordinates to TShape Coordinates (relative to
    //     just the visible portion of I2adlView).
    int    MoveLimitX1 = MoveLimitX - I2adlView->HorzScrollBar->Position;
    int    MoveLimitY1 = MoveLimitY - I2adlView->VertScrollBar->Position;

    MoveLimitX1 -= MoveLimitX1 % ViewScale;
    MoveLimitY1 -= MoveLimitY1 % ViewScale;

    // Pan the screen based on the current mouse position only.
    int    ptDiffX1 = ptDiff.x - ptDiff.x % ViewScale;
    int    ptDiffY1 = ptDiff.y - ptDiff.y % ViewScale;
    int    PanX = (Single) ? 0
                           : ScrollBarCage(ptDiffX1, I2adlView->HorzScrollBar);
    int    PanY = (Single) ? 0
                           : ScrollBarCage(ptDiffY1, I2adlView->VertScrollBar);

    // Prevent the objects from moving off the screen.
    int    ptDiffX2 = (ptDiffX1 >= MoveLimitX1) ? ptDiffX1
                                                : MoveLimitX1;
    int    ptDiffY2 = (ptDiffY1 >= MoveLimitY1) ? ptDiffY1
                                                : MoveLimitY1;
    int    DeltaX = (LockedX) ? 0
                              : PrevPoint.x - ptDiffX2 - PanX;
    int    DeltaY = (LockedY) ? 0
                              : PrevPoint.y - ptDiffY2 - PanY;

    PrevPoint.x = ptDiffX2;
    PrevPoint.y = ptDiffY2;

    // Don't redraw the objects unless they have been moved.
    if (DeltaX == 0  &&  DeltaY == 0)
        return;

    // Localize for speed:
    int    DragListCount = DragList->Count;
    int    MoveListCount = MoveList->Count;

    // Drag everything in the Drag list.
    for (int i = 0; i < DragListCount; i++)
        ((MyTShapeTransport *) DragList->Items[i])->Drag(DeltaX, DeltaY);

    for (int i = 0; i < MoveListCount; i++)
        ((MyTShapeTransport *) MoveList->Items[i])->Move(DeltaX, DeltaY);
}


// When clicking on a node, this routine creates a transport, makes it the active transport
// 		 object of the I2adlView, and connects it to FromNode.
void I2adlEditor::StartTransport(Node *FromNode)
{
    // Create new transport object and assign its begining node.
    // Find out where to start the first line on the View.
    POINT    _Point;
    FromNode->SheetCoordinates(_Point);

    I2adl    *NewTransport = new I2adl(NAME_TRANSPORT, _Point.x, _Point.y);

    // function whose shapelist is going to get the windows that make up the transport display
    I2adlView->ActiveTransport = NewTransport;
    CurrentSheet->Behavior->Add(I2adlView->ActiveTransport);

    // make the transports nodes DataSet the same type as this nodes DataSet
    NewTransport->ChangeDataSets(FromNode);
    NewTransport->CreateShape();

    MyTShapeTransport	*XShape = NewTransport->XShape;

    XShape->AnchorPoint = _Point;
    XShape->IsHorizontal = true;

    if (FromNode->GetType() == TYPE_OUTPUT)
    {
        // User clicked on a source node so hook up the sink node.
        FromNode->Connect(NewTransport->Inputs->Items[0]);

        XShape->MovingNode = NewTransport->Outputs->Items[0];
        XShape->StationaryNode = NewTransport->Inputs->Items[0];
    }
    else
    {
        // User clicked on a sink node so hook up the source node.
        FromNode->Connect(NewTransport->Outputs->Items[0]);

        XShape->MovingNode = NewTransport->Inputs->Items[0];
        XShape->StationaryNode = NewTransport->Outputs->Items[0];
    }

    TPoint MousePoint = I2adlView->ScreenToClient(Mouse->CursorPos);
//	I2adlEditor::MouseMove(TShiftState(), MousePoint.x, MousePoint.y);
	((MyTShapeTransport *)I2adlView->ActiveTransport->I2adlShape)
    	->DrawTail(MousePoint.x, MousePoint.y);
	ScrollBarCage(MousePoint.x, I2adlView->HorzScrollBar);
	ScrollBarCage(MousePoint.y, I2adlView->VertScrollBar);
}


void I2adlEditor::InitTransportDrawMode(Node *FromNode)
{
    // If not in the middle of drawing a transport, then start creating a new transport.
    if (I2adlView->ActiveTransport == NULL)
    {
        I2adl *OwningObject = FromNode->Owner;

        // Clicked on a regular I2adl object node.
        if (OwningObject->NameCode != NAME_TRANSPORT)
        {
            StartTransport(FromNode);
        }
        else
        {
            // Clicked on a dangling transport's node.
            I2adlView->ActiveTransport = OwningObject;

            // Set one node as "moving" and the other node as "stationary".
            ((MyTShapeTransport *) OwningObject->I2adlShape)->SetMovingStatus(FromNode);

            // Is this dangling transport connected to another transport?
            Node    *ConnectedNode = OwningObject->OtherEnd(FromNode)->ConnectedTo;

            if (ConnectedNode != NULL && ConnectedNode->Owner->NameCode == NAME_TRANSPORT)
                ((MyTShapeTransport *)ConnectedNode->Owner->I2adlShape)
                    ->SetMovingStatus(ConnectedNode);
        }

        FromNode->Paint();
    }
}


// Opens the next or previous sheet in the sheet navigation stack; called when "Back" and "Next"
//      commands are invoked:
void I2adlEditor::NavigateSheet(bool Forward)
{
    // Find the position of CurrentSheet within the Navigation Stack:
    I2adl	*UseSheet;
    int 	Index = GetNavigationListEntry(CurrentSheet, &UseSheet);

    // If it is there, then increment, or decrement, depending on which way we are going:
    if (Index != -1)
        Index += (Forward ? 1 : -1);

    // If the position is both valid and in bounds, then try to open the corresponding sheet:
    if (Index > -1 && Index < NavigationSheetList->Count)
    {
        I2adl	*DestSheet = (I2adl *) NavigationSheetList->Items[Index];

        // Is it in the Object Tree?  If so, then load it as a sheet:
        I2adl	*Object = (ProjectObjects->IndexOf(DestSheet) > -1) ? DestSheet
        														    : NULL;

        if (Object != NULL)
        {
            UpdateNavigationList = false; // Navigation Stack gets messed up otherwise.
            LoadAsSheet(Object);
            UpdateNavigationList = true;
        }
        // If not, is it already open in the WIP tree?  Just set it to current sheet if so:
        else if (MyProject->GetTreeNode(MainForm->WipTree->Items, DestSheet))
        {
            UpdateNavigationList = false; // Navigation Stack gets messed up otherwise.
            SetCurrentSheet(DestSheet);
            UpdateNavigationList = true;
        }
        else // Otherwise, we can't open it, since it no longer exists (was deleted).
        {
            NavigationSheetList->Delete(Index); // Because the entry is invalid.
            // Try to open the next (or previous) entry in list; this function will recurse as
            //      necessary until either it finds a valid entry or hits the head or tail of
            //      the Navigation Stack.
            NavigateSheet(Forward);
        }
    }
}


// Does Undo, or Redo if paramter "Undo" is false.
void I2adlEditor::RevertSheetState(bool Undo)
{
    // Because not every change to a wip sheet generates a call to SaveSheetState(), we
    //     might need to save off the current state on the very first Undo request.
    //     Otherwise, Redo will not bring back the latest changes.
    if (Undo &&
    	UndoList->Count > 0 &&
    	UndoList->Count - 1 == CurrentUndoIndex)
    {
        TSheetState		*LastSheetState = (TSheetState *) UndoList->Items[CurrentUndoIndex];

        if (LastSheetState->AppliesTo == CurrentSheet)
        {
        	CurrentSheet->Behavior->RecordGraphicInfo();

			if (!LastSheetState->Sheet->Equals(CurrentSheet->Behavior, true))
        		SaveSheetState();
        }
    }

    int Index = CurrentUndoIndex + (Undo ? -1 : 1);

    if (Index > -1 && Index < UndoList->Count) // Simple bounds check
    {
        // If this sheet state was not recorded on the present sheet, then open
        //      the applicable sheet if possible; if not, then do nothing more:
        TSheetState		*SheetState = (TSheetState *) UndoList->Items[Index];

        if (SheetState->AppliesTo != CurrentSheet)
        {
            if (MyProject->GetTreeNode(MainForm->WipTree->Items, SheetState->AppliesTo))
            {
                UpdateNavigationList = false; // Navigation Stack gets messed up otherwise.
                SetCurrentSheet(SheetState->AppliesTo, true);
                OpenBehavior = CurrentSheet->Behavior->CopyOf;
                UpdateNavigationList = true;
            }
            else
                return;
        }

        // Destroy all UI components for this sheet, revert to saved state, and rebuild UI:
        I2adlView->Visible = false;

        DestroyUI();

        // Preserve the CopyOf field; I2adlList::operator= alters it:
        I2adlList	*OldCopyOf = CurrentSheet->Behavior->CopyOf;
        
        CurrentSheet->Behavior->DeleteObjects();
        *CurrentSheet->Behavior = *SheetState->Sheet; // copy the saved state into CurrentSheet
        CurrentSheet->Behavior->CopyOf = OldCopyOf;
        I2adlView->ActiveTransport = NULL;

        // Also need to compensate for position of scroll bars:
        CreateUI(CurrentSheet->Behavior);

        // If CreateUI() cleaned up the sheet, then resave the state to capture the changes.
        if (!SheetState->Sheet->Equals(CurrentSheet->Behavior))
        {
            delete SheetState;
            UndoList->Items[Index] = new TSheetState(new I2adlList(CurrentSheet->Behavior)
	            , CurrentSheet);
        }

        I2adlView->Visible = true;

        // The following has the "Edit;Paste" menu option work before a mouse move over the
        //		I2adl Editor (Bug #852).
	    if (MainForm->MainEditorsControl->ActivePage == MainForm->I2adlEdit)
	        MainForm->ActiveControl = I2adlView;

        CurrentUndoIndex = Index;
    }
}


// Adds the current state of CurrentSheet to the Undo Stack.
void I2adlEditor::SaveSheetState()
{
    // If the current sheet state is up from the tail of the Undo Stack, then remove
    //      everything behind it:
    for (int i = CurrentUndoIndex + 1; i < UndoList->Count; )
    {
        delete (TSheetState *) UndoList->Items[i];
        UndoList->Delete(i);
    }

    // Can't have more than UNDO_MAX entries in the Undo Stack; pop off its head to enforce this
    if (CurrentUndoIndex == UNDO_MAX)
    {
        delete (TSheetState *) UndoList->Items[0];
        UndoList->Delete(0);
    }

    // Spare the stack from redundant entries:
    if (CurrentSheet != NULL &&
    	(
	    	UndoList->Count == 0 ||
    	    !((TSheetState *) UndoList->Items[UndoList->Count - 1])->Sheet->Equals(
            CurrentSheet->Behavior, true))
        )
    {
       	UndoList->Add(new TSheetState(new I2adlList(CurrentSheet->Behavior), CurrentSheet));
    }

    // Any time that the Undo Stack is ammended, we need to be at the tail of it:
    CurrentUndoIndex = UndoList->Count - 1;
}


void I2adlEditor::DeleteSelectedObjects()
{
    if (DropObject)
        return;

    if (I2adlView->ActiveTransport != NULL)
    {
        UndoTransport();
    }
    else
    {
        // Delete all of the selected objects.
        for (int i = 0; i < CurrentSheet->Behavior->Count; i++)
        {
            I2adl    *Object = (*CurrentSheet->Behavior)[i];

            if (!Object->I2adlShape->Selected)
                continue;

            if (Object->NameCode == NAME_JUNCTION && GetKeyState(VK_CONTROL) >= 0)
            {
                Node	*InputConnection = Object->Inputs->Items[0]->ConnectedTo;

                if (InputConnection != NULL)
                {
                    Node 	*ConnectedOutput = NULL;

                    for (int j = 0; j < Object->Outputs->Count; j++)
                    {
                     	Node	*output = Object->Outputs->Items[j];

                        if (output->ConnectedTo != NULL)
                    	{
                        	if (ConnectedOutput == NULL)
                            {
                            	ConnectedOutput = output;
                            }
                            else
                            {
                                ConnectedOutput = NULL;
                            	break;
                            }
                        }
                    }

                    if (ConnectedOutput != NULL
                    	&& !ConnectedOutput->ConnectedTo->Owner->I2adlShape->Selected
                        && !InputConnection->Owner->I2adlShape->Selected)
                    {
                        CurrentSheet->Behavior->Remove(Object);

                        Node	*con1 = InputConnection, *con2 = ConnectedOutput->ConnectedTo;

                        InputConnection->Disconnect();
                        ConnectedOutput->ConnectedTo->Disconnect();

                        Node	*node1 = con1;
                        Node	*node2 = con2;
                        I2adl	*obj1 = node1->Owner;
                        I2adl	*obj2 = node2->Owner;

                        MyTShapeTransport 	*XShape1 = obj1->XShape;
                        MyTShapeTransport 	*XShape2 = obj2->XShape;

                        // if deleting a corner junction, then drag the tail of the horizontal
                        //		transport out so that neither transport "breaks" when connected.
                        if (XShape1->IsHorizontal != XShape2->IsHorizontal)
                        {
                            for (int j = 0; j < 2; j++)
                            {
	                        	XShape1 = node1->Owner->XShape;
    	                   		XShape2 = node2->Owner->XShape;

                                if (XShape1->IsHorizontal)
                                {
                                    int 	newx = XShape1->Left + node1->Left;

                                    newx += (node1->Left > 0) ? ViewScale : -ViewScale;
                                    InitTransportDrawMode(node1);
                                    XShape1->DrawTail(newx, XShape1->Top);
                                    TerminateDraw();
                                }
                                else
                                {
                                    int 	newy = XShape1->Top + node1->Top;

                                    newy += (node1->Top > 0) ? ViewScale : -ViewScale;
	                                InitTransportDrawMode(node1);
                                    XShape1->DrawTail(XShape1->Left, newy);
                                    TerminateDraw();
                                }

                                node1 = con2;
                                node2 = con1;
                            }
                        }

                        InitTransportDrawMode(con1);

                        ConnectNodeToActiveTransport(con2);

						if (CurrentSheet->Behavior->IndexOf(obj1) > -1)
							XShape1->Moving = false;

						if (CurrentSheet->Behavior->IndexOf(obj2) > -1)
	                        XShape2->Moving = false;

                        delete Object->I2adlShape;
                        delete Object;

						CurrentSheet->Behavior->Pack();

                        DeleteSelectedObjects();
                        I2adlViewCleanup();

                        return;
                    }
                }
            }

            // Redraw any disconnected nodes incase they will now be different.
            for (int j = 0; j < Object->Inputs->Count; j++)
            {
                Node    *NodeToDisconnect = Object->Inputs->Items[j]->ConnectedTo;

                if (NodeToDisconnect == NULL)
                    continue;

                NodeToDisconnect->Disconnect();
                NodeToDisconnect->Paint();
            }

            for (int j = 0; j < Object->Outputs->Count; j++)
            {
                Node    *NodeToDisconnect = Object->Outputs->Items[j]->ConnectedTo;

                if (NodeToDisconnect != NULL)
                {
                    NodeToDisconnect->Disconnect(true);
                    NodeToDisconnect->Paint();
                }
            }

            delete Object->I2adlShape;
            delete Object;
            CurrentSheet->Behavior->Items[i] = NULL;
        }

        CurrentSheet->Behavior->Pack();
        I2adlViewCleanup();
    }

    AreShapesMoving = false;
    SaveSheetState();

    PaintAllNames();
    MyProject->Modified();
}


// Re-creates all shapes on the current sheet.  Also sets the scroll range to fit the sheet.
void I2adlEditor::RebuildUI(int NewViewScale)
{
    if (CurrentSheet == NULL)
		return;

    I2adlView->Visible = false;

    DestroyUI();

    // Reopen the sheet in the new view scale.
	if (NewViewScale > 0)
        CurrentSheet->Behavior->LastViewScale = NewViewScale;

    CreateUI(CurrentSheet->Behavior);

    I2adlView->Visible = true;

    // The following has the "Edit;Paste" menu option work before a mouse move over the
    //		I2adl Editor (Bug #852).
    if (MainForm->MainEditorsControl->ActivePage == MainForm->I2adlEdit)
	    MainForm->ActiveControl = I2adlView;
}


// Sets PrevPoint, to gridsnap, and sets list counts to 0:
void I2adlEditor::StartToMove(int X, int Y)
{
    AreShapesMoving = true;
    PrevPoint = Point(X - X % ViewScale, Y - Y % ViewScale);
    LockedX = false;
    LockedY = false;
    MoveLimitX = 0;
    MoveLimitY = 0;
    DragList->Count = 0;
    MoveList->Count = 0;
}


// Turn the behavior Sheet into an object by either creating a new object or updating the
//     original object.
void I2adlEditor::ConvertSheet()
{
	// Record the current I2adl Editor information.
    CurrentSheet->Behavior->RecordGraphicInfo();

    // Cannot create an object if the behavior Sheet is empty.
    I2adl	*NewSheet = CurrentSheet;

    if (NewSheet->Behavior->Count <= 0)
    {
        ErrorTrap(44, "I2adlEditor::ConvertSheet");
        return;
    }

    // If there are objects on top of each other, then allow the user to move them.
    AnsiString	ObjectLocations = CurrentSheet->Behavior->GetDuplicateObjectLocations();

    if (ObjectLocations != strNull &&
		ErrorTrap(178, ObjectLocations) != mrNo)
       		return;

    // If there are unconnected transports, then allow the user to fix them.
    AnsiString	TransportLocations = CurrentSheet->Behavior->GetUnconnectedTransportLocs();

    if (TransportLocations != strNull &&
		ErrorTrap(170, TransportLocations) != mrNo)
       		return;

    // If only one object on the sheet, then give the user the option of turning it into a
    //     new I2adl "def", unless both objects would share the same behavior.
    if (NewSheet->Behavior->Count == 1)
    {
        I2adl    *OnlyObject = (*NewSheet->Behavior)[0];

        if (OnlyObject->Behavior == NULL &&
        	ErrorTrap(50) == mrYes)
        {
        	// The following prevents an access into deleted memory.
        	bool	IsSystemObject = (OnlyObject->AttSystem != SystemstrNull);

            if (IsSystemObject)
            {
#if VIVA_SD
                // System primitive objects must have a Resource attribute.
                if (OnlyObject->AttResource == ResourcestrNull)
                {
                	ErrorTrap(14);

                	return;
                }
#else
            	// Can't allow creation of new system objects.
                ErrorTrap(4092, *OnlyObject->AttSystem);

				OnlyObject->AttSystem = SystemstrNull;
#endif
            }

            // Create a new I2adl "def" object.
            ProjectObjects->Add(new I2adl(OnlyObject, cnYesDisconnect));

            BaseSystem->SyncLists();
            RemoveSheet(NewSheet);
            MyProject->BuildTreeView();
            MyProject->Modified(IsSystemObject);

            return;
        }
    }

    // Create a NewObject containing the changes the user made to the behavior.
    I2adl	*OrigObject = ProjectObjects->GetObject(NewSheet->Behavior->CopyOf);
    I2adl	*NewObject;

    if (OrigObject == NULL)
    {
        // Create a new composite (not primitive) object.
        NewObject = new I2adl(CurrentSheet->NameInfo->Name);
        NewObject->IsPrimitive = false;
    }
    else
    {
        // Copy the object (but not the nodes).
        NewObject = new I2adl(OrigObject, cnNo);

        // The following handles changing the behavior's name by clicking on the wip tree twice.
        NewObject->SetName(CurrentSheet->NameInfo->Name);
    }

    NewObject->Behavior = NewSheet->Behavior;

    // Allow the user to edit the information on the object level.
    //     NewObject will either be deleted or turned into a project object.
    AttributeDialog->EditObjectAttributes(OrigObject, NewObject);
}


// This method combines the ActiveTransport with its connected transport(s) if they are 
//		redundant (have the same horizontal/vertical direction).
void I2adlEditor::MergeRedundantTransports(int X, int Y)
{
    while (true)
    {
        // Is the ActiveTransport attached to another transport?
        I2adl	*SurvivingTransport = I2adlView->AttachedI2adl();

        if (SurvivingTransport == NULL)
            break;

        MyTShapeTransport	*SurvivingShape = (MyTShapeTransport*) SurvivingTransport->
        	I2adlShape;
        MyTShapeTransport	*ActiveShape = (MyTShapeTransport *) I2adlView->ActiveTransport->
        	I2adlShape;

        // If the transports go in different directions, then they are not redundant.
        if (SurvivingShape->IsHorizontal != ActiveShape->IsHorizontal)
            break;

        // Merge the ActiveTransport into the SurvivingTransport.
        Node	*NewMovingNode = (ActiveShape->MovingNode->GetType() == TYPE_OUTPUT)
        					   ? SurvivingShape->OwningI2adl->Outputs->Items[0]
        					   : SurvivingShape->OwningI2adl->Inputs->Items[0];

        CurrentSheet->Behavior->Remove(I2adlView->ActiveTransport);

        delete ActiveShape;
        delete I2adlView->ActiveTransport;

        I2adlView->ActiveTransport = NULL;

        InitTransportDrawMode(NewMovingNode);

        SurvivingShape->PaintNames(WorkSpaceColor);
        SurvivingShape->DrawTail(X, Y, false);
    }
}


TRect I2adlEditor::ComputeCurrentSheetExtent(bool SelectedOnly)
{
	TRect	ExtentRect;

	ExtentRect.Top = MAXINT;
	ExtentRect.Left = MAXINT;
    ExtentRect.Bottom = 0;
    ExtentRect.Right = 0;

    for (int i = 0; i < CurrentSheet->Behavior->Count; i++)
    {
    	I2adl	*object = (I2adl *) CurrentSheet->Behavior->Items[i];

        if (SelectedOnly && !object->I2adlShape->Selected)
        	continue;

        if (object->Y + object->Height > ExtentRect.Bottom)
			ExtentRect.Bottom = object->Y + object->Height;

        if (object->Y < ExtentRect.Top)
			ExtentRect.Top = object->Y;

        if (object->X + object->Width > ExtentRect.Right)
			ExtentRect.Right = object->X + object->Width;

        if (object->X < ExtentRect.Left)
	        ExtentRect.Left = object->X;
    }

    return ExtentRect;
}


void I2adlEditor::SetStandardFont(TFont *Font, int _ViewScale)
{
    if (_ViewScale == -1)
        _ViewScale = ViewScale;

    Font->Name = (_ViewScale > 5) ? "Lucida Console" : "Small Fonts"; // "Terminal" "FixedSys" "Courier"
    Font->Size = _ViewScale * 1.41 + (_ViewScale > 5) * 2;
};


void I2adlEditor::SetDrawColor()
{
    if ((WorkSpaceColor & 0x000000FF) + ((WorkSpaceColor & 0x0000FF00) >> 8)
        + ((WorkSpaceColor & 0x00FF0000) >> 16) < 350)
        DrawColor = clWhite;
    else
        DrawColor = clBlack;
}


I2adl *I2adlEditor::NewWipObject(I2adlList *Behavior, AnsiString &Name)
{
	I2adl	*obj = new I2adl(NAME_OTHER, 0, 0);

    obj->IsPrimitive = false;
    obj->Behavior = Behavior;
    obj->SetName(Name);

    return obj;
}
