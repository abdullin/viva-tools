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

#include "Globals.h"
#include "AttributeEditor.h"
#include "DataSet.h"
#include "ErrorTrap.h"
#include "I2adl.h"
#include "I2adlEditor.h"
#include "I2adlList.h"
#include "Main.h"
#include "MyTMemo.h"
#include "MyTScrollingWinControl.h"
#include "MyTShape.h"
#include "Node.h"
#include "Project.h"
#include "VivaIcon.h"
#include "VivaSystem.h"

#pragma package(smart_init)

bool               DidDoubleClick = false;
bool               HasRedrawn = false;

// Convert window coordinates coordinates to absolute coordinates.
// Expand work space to contain object and add a little "room" for growth.


// Constructor - Main
__fastcall MyTShape::MyTShape(TComponent *Owner) :
    TGraphicControl(Owner)
{
    Canvas->Brush->Color = WorkSpaceColor;
    Canvas->Brush->Style = bsClear;
    Visible = true;
    Cursor = crHandPoint;
    Left = 0;
    Top = 0;
    Width = 0;
    Height = 0;
}


// Need to send this event to I2adlView at appropriate coordinates:
void __fastcall MyTShape::DragOver(System::TObject *Source, int X, int Y
    , TDragState State, bool &Accept)
{
    TPoint p = I2adlView->ScreenToClient(ClientToScreen(TPoint(X, Y)));
    I2adlEditor::DragOver(this, Source, p.x, p.y, State, Accept);
}


// Need to send this event to I2adlView at appropriate coordinates:
void __fastcall MyTShape::DragDrop(System::TObject* Source, int X, int Y)
{
    TPoint p = I2adlView->ScreenToClient(ClientToScreen(TPoint(X, Y)));
    MainForm->I2adlViewDragDrop(this, Source, p.x, p.y);
}


namespace Mytshape
{
    void __fastcall PACKAGE Register()
    {
        RegisterComponent(__classid(MyTShape));
    }
}


/*****************************************************************
*                       My TShape I2adl                          *
*****************************************************************/

__fastcall MyTShapeI2adl::MyTShapeI2adl(I2adl *_OwningI2adl)
    : MyTShape(I2adlView)
{
    HasObjectRef = false;
    OwningI2adl = _OwningI2adl;

    ObjectAttributes = new TStringList;
    ObjectAttributes->Sorted = true;
    ObjectAttributes->Duplicates = dupAccept;

    // Transports perform their own special initializtion.
    if (OwningI2adl == NULL)
        return;

    OwningI2adl->I2adlShape = this;

    // Put all of the attributes into our own list and the Hint (tiptooltext):
    if (OwningI2adl->NameCode != NAME_TEXT)
    {
        ObjectAttributes->Text = OwningI2adl->Attributes->Text;

        if (OwningI2adl->AttSystem != SystemstrNull)
            ObjectAttributes->Add(strSystem + "=" + *OwningI2adl->AttSystem);

        if (OwningI2adl->AttChildAtt != SystemstrNull)
            ObjectAttributes->Add(strChildAttribute + "=" + *OwningI2adl->AttChildAtt);

        if (OwningI2adl->AttResource != ResourcestrNull)
            ObjectAttributes->Add(strResource + "=" + *OwningI2adl->AttResource);

        if (OwningI2adl->AttProvidesRes != ResourcestrNull)
            ObjectAttributes->Add(strProvidesResource + "=" + *OwningI2adl->AttProvidesRes);

        if (OwningI2adl->AttResourceQty != 1) // 1 is the default value.
			ObjectAttributes->Add(strResourceQty + "=" + OwningI2adl->AttResourceQty);

        Hint = ObjectAttributes->Text.Trim();
    }

	_OwningI2adl->Conformalize();

    SetStandardFont(Canvas->Font);
    Visible = false;  // Prevents time-consuming Window updates
    Moving = false;
    FSelected = OwningI2adl->Selected;    
    Parent = (MyTScrollingWinControl *) Owner;
    Width = _OwningI2adl->Width;
    Left = _OwningI2adl->X;
    Top  = _OwningI2adl->Y;

    if (_OwningI2adl->NameCode == NAME_TEXT)
    {
        InitFont();
        Height = _OwningI2adl->Height;

        // Set the width and height if it is a Text Object freshly loaded from file:
        SetCanvasText();

        // To keep Viva from shimmering, find this font's memo object.
        MyTMemo		*ZoomTextBoxMemo = NULL;
        TFont		*ShapeFont = GetCanvas()->Font;

        for (int j = 0; j < MainForm->ZoomTextBoxMemos->Count; j++)
        {
            MyTMemo		*Memo = (MyTMemo *) MainForm->ZoomTextBoxMemos->Items[j];

            if (Memo->FontsAreSame(ShapeFont))
            {
                ZoomTextBoxMemo = Memo;
                break;
            }
        }

        if (ZoomTextBoxMemo == NULL)
        {
            ZoomTextBoxMemo = new MyTMemo(MainForm);
            MainForm->ZoomTextBoxMemos->Add(ZoomTextBoxMemo);
        }

        ZoomTextBoxMemo->Initialize(this, Left + MEMO_OFFSET_LEFT, Top + MEMO_OFFSET_TOP);
        Width = ZoomTextBoxMemo->Width + MEMO_OFFSET_LEFT * 2;
        Height = ZoomTextBoxMemo->Height + MEMO_OFFSET_TOP;
    }
    else
    {
        NodeList    *_IONodes = OwningI2adl->Inputs;

        for (int IO = 0; IO < 2; IO++) // for each NodeList
        {
            for (int i = 0; i < _IONodes->Count; i++)
            {
                Node *CurrNode = _IONodes->Items[i];
                CurrNode->Left = CurrNode->Left;
                CurrNode->Top = CurrNode->Top;
            }

            _IONodes = OwningI2adl->Outputs;
        }

        WidenIcon();
        SetInitialHeight();

        // Does the object have an ObjectRef attribute?
        AnsiString	ObjectRef;

        OwningI2adl->GetAttribute(strObjectRef, ObjectRef);

        if (ObjectRef != strNull)
        	HasObjectRef = true;
    }

    Visible = true;
}


// Destructor
void __fastcall MyTShapeI2adl::BeforeDestruction()
{
    if (OwningI2adl != NULL)  // Safety check
	{
        // Remove the node names from the back canvas.
        PaintNames(WorkSpaceColor);
        OwningI2adl->I2adlShape = NULL;
    }

	delete ObjectAttributes;

    ObjectAttributes = NULL;
}

int __fastcall MyTShapeI2adl::GetLeft()
{
	return TGraphicControl::Left;
}

int __fastcall MyTShapeI2adl::GetTop()
{
    return TGraphicControl::Top;
}

int __fastcall MyTShapeI2adl::GetWidth()
{
	return TGraphicControl::Width;
}

int __fastcall MyTShapeI2adl::GetHeight()
{
	return TGraphicControl::Height;
}


void __fastcall MyTShapeI2adl::SetSelected(bool Value)
{
	FSelected = Value;

    OwningI2adl->Selected = Value;
}


// The 4 follwing functions Set the left/top/width/height values in the Custom
//      Control and OwningI2adl; also forces them to the Grid:
void __fastcall MyTShapeI2adl::SetLeft(int Data)
{
    OwningI2adl->CalculateHorzGridSnap(Data);
    // Scroll bar adjustment:
    OwningI2adl->X = EnsureScrollRange(I2adlView->HorzScrollBar, Data
	    + I2adlView->HorzScrollBar->Position);
    EnsureScrollRange(I2adlView->HorzScrollBar, OwningI2adl->X + OwningI2adl->Width);
    TGraphicControl::Left = Data;
}


void __fastcall MyTShapeI2adl::SetTop(int Data)
{
    AlignToGrid(Data);
    // Scroll bar adjustment:
    OwningI2adl->Y = EnsureScrollRange(I2adlView->VertScrollBar, Data
    	+ I2adlView->VertScrollBar->Position);
    EnsureScrollRange(I2adlView->VertScrollBar, OwningI2adl->Y + OwningI2adl->Height);

    int 	i = Data;
    TGraphicControl::Top = i;
}


void __fastcall MyTShapeI2adl::SetWidth(int Data)
{
    OwningI2adl->Width = Data;
    TGraphicControl::Width = OwningI2adl->Width;

	int 	Extent = OwningI2adl->Width + OwningI2adl->X;

	if (OwningI2adl->NameCode == NAME_OUTPUT && OwningI2adl->Inputs->Items[0]->ShowNodeName())
		Extent += OwningI2adl->Inputs->NodeNameWidth();

    EnsureScrollRange(I2adlView->HorzScrollBar, Extent);
}


void __fastcall MyTShapeI2adl::SetHeight(int Data)
{
    OwningI2adl->Height = Data;
    TGraphicControl::Height = OwningI2adl->Height;
    EnsureScrollRange(I2adlView->VertScrollBar, OwningI2adl->Height + OwningI2adl->Y);
}


// Sets the height so that the shape encompasses both the Icon and the Node shapes:
void MyTShapeI2adl::SetInitialHeight()
{
    int    NewHeight = OwningI2adl->Height;

    VivaIcon    *Icon = OwningI2adl->GetIconInfo();

    // Make sure the object height is largest enough to display the entire icon.
    if (Icon->Bitmap != NULL)
    {
        int    IconHeight = (Icon->Bitmap->Height * ViewScale) / OLD_GRID_SNAP;

        IconHeight = (IconHeight * ViewScale) / ViewScale;

        if (NewHeight < IconHeight)
            NewHeight = IconHeight;
    }

    Height = NewHeight;
}


// Determines which, if any, node the mouse is over.
//     This allows object event handlers to call node event handlers.
//     SnapZone (in pixels) allows clicks close to a node to be considered as on the node.
Node *MyTShapeI2adl::GetNodeAt(TPoint P, int SnapZone)
{
    NodeList    *IONodes = OwningI2adl->Inputs;
    int         NodeSize = OwningI2adl->NodeSize();

    for (int IO = 0; IO < 2; IO++) // for each NodeList
    {
        for (int i = 0; i < IONodes->Count; i++)
        {
            Node    *IONode = IONodes->Items[i];

            if (P.x >= IONode->Left - SnapZone &&
                P.x <  IONode->Left + SnapZone + NodeSize &&
                P.y >= IONode->Top - SnapZone &&
                P.y <  IONode->Top + SnapZone + NodeSize)
            {
                return IONode;
            }
        }

        IONodes = OwningI2adl->Outputs;
    }

    return NULL;
}


// Draws the box for the I2adl object (if required).
void __fastcall MyTShapeI2adl::PaintBox()
{
    I2adl    *Object = OwningI2adl;  // One level of indirection.
    TRect    TheRect;

    // Should the box be blue or black?
    if (Selected)
        Canvas->Brush->Color = SelectionColor;
    else
        Canvas->Brush->Color = clBlack;

    // Input/Output objects do not have a box.
    if (Object->NameCode == NAME_INPUT  ||  Object->NameCode == NAME_OUTPUT)
    {
        // Unless they are a system object that does not have an icon.
        if (Object->GetIconInfo()->Bitmap != NULL)
            return;
    }

    if (Object->NameCode == NAME_TEXT)
    {
        // Text object have their box on the out side.
        TheRect.Left = 0;
        TheRect.Top = 0;
        TheRect.Right = Width;
        TheRect.Bottom = Height;

        if (Object->AttSystem != SystemstrNull)
        {
            // Text box contains text.  Only print the box when the object is selected.
            if (!Object->I2adlShape->Selected && // Object->AttSystem != strNull &&
              (*Object->AttSystem)[1] != '<' )
                Canvas->Brush->Color = WorkSpaceColor;
        }
        else
        {
            // Text box is empty.  Print a box and the word "Text".
            TColor OldColor = Canvas->Brush->Color;
            PaintNames(DrawColor);
            Canvas->Brush->Color = OldColor;
        }
    }
    else
    {
        // Other objects have an indented box.
        TheRect.Left = ViewScale;
        TheRect.Top = ViewScale;
        TheRect.Right = Width - ViewScale;
        TheRect.Bottom = Height;

        // Objects with transports in the icon need no box (unless displaying control nodes).
        if (Object->GetIconInfo()->Bitmap != NULL)
            if (Object->NonTransparentEdge()) // >= 3
                return;
    }

    // Draw the box.
    Canvas->FrameRect(TheRect);
}


// Paints the nodes on a MyTShapeI2adl.
void __fastcall MyTShapeI2adl::PaintNodes()
{
    // Pass #1 Inputs;  Pass #2 Outputs.
    NodeList    *IONodes = OwningI2adl->Inputs;

    for (int IO = 0; IO < 2; IO++)
    {
        for (int i = 0; i < IONodes->Count; i++)
            IONodes->Items[i]->Paint();

        IONodes = OwningI2adl->Outputs;
    }
}


// Initializes the font for Text Objects, based on what was stored in its Attributes list:
void __fastcall MyTShapeI2adl::InitFont()
{
    TFont	*CFont = Canvas->Font;

    CFont->Size = OwningI2adl->CalculateFontSize();

    // If nothing has been stored (this is a new text object), then default:
    if (OwningI2adl->Attributes->Count < 4)
    {
        CFont->Name = "Arial";
        CFont->Color = clBlack;
        return;
    }

    // Otherwise, set the font as the Attributes indicate:
    CFont->Name = OwningI2adl->Attributes->Strings[0];
    CFont->Color = (TColor) VStrToInt(OwningI2adl->Attributes->Strings[2]);
    short IStyle = (short) OwningI2adl->Attributes->Strings[3].ToInt();
    CFont->Style = *(TFontStyles *) &IStyle;
}


// For Text Objects, prints the text on the Canvas of the Shape:
void __fastcall MyTShapeI2adl::SetCanvasText()
{
    InitFont();
    // If there is anything to print, then decode and print to the canvas line by line:
    if (OwningI2adl->AttSystem != SystemstrNull)
    {
        Canvas->Brush->Color = WorkSpaceColor;
        Canvas->Brush->Style = bsClear;
        // Need to assign to a list and print one line at a time, so that
        //      "\r\n" Shows up as a return, rather than as a solid black block:
        TStringList *TempList = new TStringList;
        // Ascii 1 is our code for a 'Soft' newline in the stored version:
        TempList->Text = ReplaceAll(*OwningI2adl->AttSystem, (char) 1, strCRLF);
        int TextHeight = Canvas->TextHeight(TempList->Text);
        int TextTop = MEMO_OFFSET_TOP;

        for (int i = 0; i < TempList->Count; i++, TextTop += TextHeight)
        {
            Canvas->TextOut(MEMO_OFFSET_LEFT, TextTop,
                 TempList->Strings[i]);
        }

        delete TempList;
    }
}


// This function paints the object names and node names on the TShape.
//     The "color" parameter is used to erase the names printed on the back canvas.
void __fastcall MyTShapeI2adl::PaintNames(TColor color, bool SkipObjectRef)
{
    // If the "text" object contains text, then don't print the object name.
    if (OwningI2adl->NameCode == NAME_TEXT)
        if (OwningI2adl->AttSystem != SystemstrNull)
            return;

    Canvas->Brush->Color = WorkSpaceColor;
    Canvas->Brush->Style = bsClear;

	// Print the name of the object.
    if (OwningI2adl->NameCode != NAME_TRANSPORT &&
        OwningI2adl->NameCode != NAME_JUNCTION &&
        OwningI2adl->NameCode != NAME_INPUT &&
        OwningI2adl->NameCode != NAME_OUTPUT)
    {
        // Setting the pen color twice ensures object names do not have a line through them.
	    Canvas->Pen->Color = (TColor) !WorkSpaceColor;
	    Canvas->Pen->Color = WorkSpaceColor;

        // The following fixes several bugs (#167) with text boxes when they are first dragged
        //     into the I2adl Editor.
        if (OwningI2adl->NameCode != NAME_TEXT)
	       	SetStandardFont(Canvas->Font, ViewScale);

    	// Find the largest font that will print the object name in the space available.
        int		NameWidth = Canvas->TextWidth(OwningI2adl->NameInfo->Name);
        int		FontViewScale = ViewScale;
        int		IndentAmount = ViewScale * 1.61;
        int		BlankAmount = ViewScale * .21;

        while (FontViewScale > 1)
        {
            if (NameWidth + IndentAmount + BlankAmount <= Width)
            	break;

			FontViewScale--;
	       	SetStandardFont(Canvas->Font, FontViewScale);
			NameWidth = Canvas->TextWidth(OwningI2adl->NameInfo->Name);
        }

        // Erase part of the object box and write out the object name.
        Canvas->MoveTo(IndentAmount, ViewScale);
        Canvas->LineTo(NameWidth + IndentAmount + BlankAmount * 2, ViewScale);
        Canvas->Font->Color = Selected ? SelectionColor : clBlue;
        Canvas->TextOut(IndentAmount + BlankAmount, ViewScale * -.21, OwningI2adl->NameInfo->Name);

        // The following fixes several bugs (#167) with text boxes when they are first dragged
        //     into the I2adl Editor.
        if (OwningI2adl->NameCode == NAME_TEXT)
        	return;

        // Find the largest font that will print all of the node names in the space avaliable.
        FontViewScale = ViewScale;

        SetStandardFont(Canvas->Font, FontViewScale);

        VivaIcon    *Icon     = OwningI2adl->GetIconInfo();
        int			IconWidth = (Icon == NULL || Icon->Bitmap == NULL)
        					  ? 0
                              : double(Icon->OrigWidth) * ViewScale / OLD_GRID_SNAP;

        // Pass #1 - Inputs
        NodeList    *IONodes = OwningI2adl->Inputs;
        NodeList	*OtherNodes = OwningI2adl->Outputs;

        for (int IO = 0; IO < 2; IO++)
        {
        	// If we are at the smallest possible font, then that's good enough.
        	if (FontViewScale <= 1)
            	break;

            for (int i = 0; i < IONodes->Count; i++)
            {
                Node    *IONode = IONodes->Items[i];

                // Skip node names that are not displayed.
                if (!IONode->ShowNodeName())
                    continue;

                // Will the node name fit in the space avaliable?
                while (FontViewScale > 1)
                {
                    int		LeftLimit  = (IONode->Left <= 0)
                    				   ? IONode->GetSize() + BlankAmount
                                       : 0;
                    int		RightLimit = (IONode->Left <= 0)
                    				   ? Width
                                       : IONode->Left - BlankAmount;

                    if (IconWidth > 0)
                    {
                    	// The object has an icon.  This limits the space avaliable.
                        //     Note:  The following is based on the requirement that all icons
                        //     have margins on the right/left sides.
                        LeftLimit = 0;
                        RightLimit = (Width - IconWidth) / 2 + 2;
                    }
                    else if (OtherNodes->Count > i)
                    {
                    	// There is a node on the other size.  This limits the space avaliable.
                    	Node	*OtherNode = OtherNodes->Items[i];

                        // Skip nodes that don't show and control nodes that don't line up.
		                if (OtherNode->ShowNodeName() &&
                        	IONode->Left != OtherNode->Left)
                        {
                        	int		OtherWidth = Canvas->TextWidth(*OtherNode->Name);

                            if (IONode->Left <= 0)
                                RightLimit = OtherNode->Left - OtherWidth - BlankAmount * 2;
                            else
                                LeftLimit = OtherNode->GetSize() + OtherWidth + BlankAmount * 2;
                        }
                    }

                    // Is there room to print the node name?
                    if (Canvas->TextWidth(*IONode->Name) <= RightLimit - LeftLimit)
                        break;

                    FontViewScale--;
                    SetStandardFont(Canvas->Font, FontViewScale);
                }
            }

            // Pass #2 - Outputs
            IONodes = OwningI2adl->Outputs;
            OtherNodes = OwningI2adl->Inputs;
        }
    }

    // Display the node names.
    Canvas->Font->Color = Selected ? SelectionColor : DrawColor;

    // Pass #1 - Inputs
    NodeList    *IONodes = OwningI2adl->Inputs;

    for (int IO = 0; IO < 2; IO++)
    {
        for (int i = 0; i < IONodes->Count; i++)
        {
            Node    *IONode = IONodes->Items[i];

            // Print Input/Output node names on the back canvas (outside the TShape).
            if (OwningI2adl->NameCode == NAME_INPUT ||
                OwningI2adl->NameCode == NAME_OUTPUT ||
                OwningI2adl->NameCode == NAME_TRANSPORT)
            {
                // Only print Constant Attribute strings on Input objects.
                AnsiString      ConstValue;
                AnsiString      DefaultValue;
                bool            ShowingConstant = false;
                bool			ShowingDefault = false;
                bool 			ShowingName = IONode->ShowNodeName();
                bool 			ShowingTrap =
                					(!ShowingObjectAttributes &&
                					 OwningI2adl->NameCode == NAME_OUTPUT &&
                                	 OwningI2adl->Attributes->Values[strTrap] != strNull);

                if (!ShowingObjectAttributes &&
                	OwningI2adl->NameCode == NAME_INPUT)
                {
                    ConstValue = OwningI2adl->Attributes->Values[strConstant];

                    if (ConstValue != strNull)
                        ShowingConstant = true;

                    DefaultValue = OwningI2adl->Attributes->Values[strDefault];

                    if (DefaultValue != strNull)
                        ShowingDefault = true;
                }

                // Save some computations.
                if (!ShowingConstant && !ShowingDefault && !ShowingName && !ShowingTrap)
                    continue;

                 // Setup I2adlView's canvas for displaying text.
                HDC		dc = GetDC(I2adlView->Handle);

                I2adlView->Canvas->Handle = dc;
                ReallocateDC(I2adlView->Canvas->Font);
                I2adlView->Canvas->Font->Color = color;
                I2adlView->Canvas->Brush->Color = WorkSpaceColor;
                I2adlView->Canvas->Brush->Style = bsClear;

                I2adlView->SetDefaultFont();

                int		MaxTextWidth = 0;

                if (ShowingName)
                    MaxTextWidth = I2adlView->Canvas->TextWidth(*IONode->Name);

                if (ShowingConstant)
                    SetMax(MaxTextWidth, I2adlView->Canvas->TextWidth(EncodeString(ConstValue)));
                else if (ShowingDefault)
                    SetMax(MaxTextWidth, I2adlView->Canvas->TextWidth(EncodeString(DefaultValue)));

                // If NodesOnBothSides is true, then paint the names on the same
                //      side as the node; <NodesOnBothSides> reverses the
                //      condition under which we print names on other side:
                bool 	NodesOnBothSides = (OwningI2adl->Inputs->Count > 0
                                      		&& OwningI2adl->Outputs->Count > 0);
                int     x;
                int     NodeX = IONode->Left;

                if (OwningI2adl->NameCode == NAME_TRANSPORT)
                {
                    x = (NodeX <= 0) ? NodeX + ViewScale * 1.41		// Node on Left
                                     : NodeX - MaxTextWidth - int(ViewScale * .41); // Node on Right
                }
                else if (NodesOnBothSides)
                {
                    x = (NodeX <= 0)  ? - MaxTextWidth - ViewScale  // Node on Left
                                      : NodeX + ViewScale * 2;      // Node on Right
                }
                else
                {
                    x = (NodeX <= 0)  ? NodeX + ViewScale * 6       // Node on Left
                                      : - MaxTextWidth; // Node on Right
                }

                // Skip node names that are not displayed.
                if (ShowingName)
                {
                    if (color != WorkSpaceColor &&
                    	OwningI2adl->IsGlobalLabel())
                        	I2adlView->Canvas->Font->Color = clFuchsia;

                    I2adlView->Canvas->TextOut(Left + x, Top + IONode->Top - ViewScale
                        - (NodesOnBothSides ? ViewScale : 0), *IONode->Name);
                }

                if (ShowingConstant)
                {
                    if (color != WorkSpaceColor)
                        I2adlView->Canvas->Font->Color = clRed;

					I2adlView->Canvas->TextOut(Left + x
                    	, Top + IONode->Top + ViewScale * .61, EncodeString(ConstValue));
                    I2adlView->Canvas->Font->Color = color;
                }
                else if (ShowingDefault)
                {
                    if (color != WorkSpaceColor)
                        I2adlView->Canvas->Font->Color = clBlue;

					I2adlView->Canvas->TextOut(Left + x
                    	, Top + IONode->Top + ViewScale * .61, EncodeString(DefaultValue));
                    I2adlView->Canvas->Font->Color = color;
                }

                if (ShowingTrap)
                {
                    if (color != WorkSpaceColor)
	                    I2adlView->Canvas->Font->Color = clGreen;

                    AnsiString		text(OwningI2adl->Attributes->Values[strTrap].SubString(1, 30));

                    I2adlView->Canvas->TextOut(Left + x, Top + IONode->Top + ViewScale, text);
                    I2adlView->Canvas->Font->Color = color;
                }

                ReleaseDC(I2adlView->Handle, dc);
                continue;
            }

            // Regular object.  Print the node names on the object's canvas.
            if (!IONode->ShowNodeName())
                continue;

            int    x = (IONode->Left <= 0)
                   ? ViewScale * 1.41
                   : IONode->Left - Canvas->TextWidth(*IONode->Name) - int(ViewScale * .21);

            if (OwningI2adl->GetIconInfo()->Bitmap == NULL ||	// No icon
                OwningI2adl->NonTransparentEdge() == 0)		 	// No transport on icon
            {
                // Print the node name on the same level as the node.
                Canvas->TextOut(x, IONode->Top - int(ViewScale * .41), *IONode->Name);
            }
            else
            {
                // Print the node name below the node.
                Canvas->TextOut(x, IONode->Top + ViewScale * .61, *IONode->Name);
            }
        }

        // Pass #2 - Outputs
        IONodes = OwningI2adl->Outputs;
    }

    if (ShowingObjectAttributes &&
		!SkipObjectRef &&
        ObjectAttributes->Count > 0)
    {
    	// Display all of the object attributes below the object on the back canvas.
        //		The following fixes a crash bug when Viva is shut down.
        if (MainForm->WipTree->Selected == NULL)
            return;

        // Setup I2adlView's canvas for displaying text.
        HDC           dc = GetDC(I2adlView->Handle);

        I2adlView->Canvas->Handle = dc;
        ReallocateDC(I2adlView->Canvas->Font);
        I2adlView->SetDefaultFont();
        I2adlView->Canvas->Font->Color = WorkSpaceColor;
        I2adlView->Canvas->Brush->Color = WorkSpaceColor;
        I2adlView->Canvas->Brush->Style = bsClear;

        if (color != WorkSpaceColor)
            I2adlView->Canvas->Font->Color = clRed;

        int		PrintLeft = Left + ViewScale * 1.21;

        for (int i = 0; i < ObjectAttributes->Count; i++)
	        I2adlView->Canvas->TextOut(PrintLeft, Top + Height + ViewScale * i * 2,
            	ObjectAttributes->Strings[i]);

        ReleaseDC(I2adlView->Handle, dc);
    }
    else if (HasObjectRef &&
    		 !SkipObjectRef)
    {
        // If the object has an ObjectRef attribute, then print the global label names on the
        //		back canvas.
        //		The following fixes a crash bug when Viva is shut down.
        if (MainForm->WipTree->Selected == NULL)
            return;

        // Setup I2adlView's canvas for displaying text.
        HDC           dc = GetDC(I2adlView->Handle);
        AnsiString    ObjectRef;

        OwningI2adl->GetAttribute(strObjectRef, ObjectRef);

        AnsiString    GlobalLabelName = MainForm->WipTree->Selected->Text + strPeriod +
            OwningI2adl->NameInfo->Name + ObjectRef + strPeriod;

        I2adlView->Canvas->Handle = dc;
        ReallocateDC(I2adlView->Canvas->Font);
        I2adlView->SetDefaultFont();
        I2adlView->Canvas->Font->Color = WorkSpaceColor;
        I2adlView->Canvas->Brush->Color = WorkSpaceColor;
        I2adlView->Canvas->Brush->Style = bsClear;

        if (color != WorkSpaceColor)
            I2adlView->Canvas->Font->Color = clFuchsia;

        I2adlView->Canvas->TextOut(Left + ViewScale * 1.21, Top + Height
        	, GlobalLabelName + strIn);
        I2adlView->Canvas->TextOut(Left + ViewScale * 1.21, Top + Height + ViewScale * 2
        	, GlobalLabelName + strOut);

        ReleaseDC(I2adlView->Handle, dc);
    }
}


// If the icon has transports, then make sure it is wide enough to cover the object.
void __fastcall MyTShapeI2adl::WidenIcon()
{
    OwningI2adl->Conformalize();

    Width = OwningI2adl->Width;

    if (OwningI2adl->NameCode == NAME_JUNCTION ||
    	OwningI2adl->NameCode == NAME_INPUT ||
        OwningI2adl->NameCode == NAME_OUTPUT)
    {
        return;
    }

    // Is the current icon already wide enough?
    VivaIcon	*VIcon = OwningI2adl->GetIconInfo();
    TBitMap		*Icon = VIcon->Bitmap;

    // If both edges are transparent, then the icon does not have transports.
    int		NonTranEdge = OwningI2adl->NonTransparentEdge();

    if (NonTranEdge != 0)
    {
        // Make a copy of the original (narrow) icon.
        TBitMap		*OldIcon = new TBitMap;

        OldIcon->Assign(Icon);

        Icon->Width = max(Icon->Width, (Width * OLD_GRID_SNAP) / ViewScale);

        // Erase the icon by filling the canvas with the transparent color.
        Icon->Canvas->Brush->Color = clFuchsia;
        Icon->Canvas->FillRect(Icon->Canvas->ClipRect);
        Icon->Canvas->CopyMode = cmSrcCopy;

        // Paint a transport for each non control node.
        TBitMap      *TransportIcon = TransportHor->Bitmap;
        int          TransportWidth = (Icon->Width - ICON_CENTER_RESERVE) / 2;
        NodeList     *IONodes;

        for (int IO = 0; IO < 2; IO++)
        {
            if (IO == 0)
            {
                // Pass #1 - Input nodes.  Does the left side of the icon have transports?
                if ((NonTranEdge & trLeft) == 0)
                    continue;

                IONodes = OwningI2adl->Inputs;
            }
            else
            {
                // Pass #2 - Output nodes.  Does the right side of the icon have transports?
                if ((NonTranEdge & trRight) == 0)
                    continue;

                IONodes = OwningI2adl->Outputs;
            }

            for (int i = 0; i < IONodes->Count; i++) // Skip control nodes.
            {
                Node	*IONode = IONodes->Items[i];
                int		DrawIncrement = 50;
                int		y = int(((IONode->Top + int(ViewScale * .21)) * OLD_GRID_SNAP) / ViewScale) ;

                if (IONode->Left <= 0)
                {
                    // Paint a transport on the left side.
                    for (int j = (TransportWidth % DrawIncrement) - DrawIncrement
                    	; j < TransportWidth; j += DrawIncrement)
                        Icon->Canvas->Draw(j, y, TransportIcon);
                }
                else
                {
                    // Paint a transport on the right side.
                    for (int j = Icon->Width - TransportWidth; j < Icon->Width; j += DrawIncrement)
                        Icon->Canvas->Draw(j, y, TransportIcon);
                }
            }
        }

        // Paint the original (narrow) icon in the center of the new (wide) icon.
        Icon->Canvas->Draw((Icon->Width - OldIcon->Width) / 2, 0, OldIcon);

        delete OldIcon;
    }

    // Create the icon to be used when the object is selected.
    VIcon->CreateSelectedIcon();
}


// Draws the I2adl object graphic.
void __fastcall MyTShapeI2adl::Paint()
{
    if (OwningI2adl->NameCode == NAME_TEXT)
    {
        SetCanvasText();
        PaintBox();
        return;
    }

    PaintBox();

    if (OwningI2adl->NameCode == NAME_JUNCTION)
    {
        PaintNodes();
        return;
    }

    VivaIcon    *VIcon = OwningI2adl->GetIconInfo();
    TBitMap     *Icon = VIcon->Bitmap;

    if (Icon != NULL)
    {
        if (Selected)
            Icon = VIcon->SelectedBitmap;

        // Center the icon in the middle of the object.
        int		IconHeight = (Icon->Height * ViewScale) / OLD_GRID_SNAP;
        int		IconWidth = (Icon->Width * ViewScale) / OLD_GRID_SNAP;
        int		X = (Width - IconWidth) >> 1;

        // Canvas->Draw(X, 0, Icon);

        // The following variables are for speed (which is important here).
        int			StartColumn = (X < 0) ? -X
                                          : 0;
        int			EndColumn   = (X < 0) ? IconWidth + X
                                          : IconWidth;

        TCanvas		*IconCanvas = Icon->Canvas;
        TRect       rect;

        rect.Top = 0;
        rect.Left = X;
        rect.Right = rect.Left + IconWidth;
        rect.Bottom = IconHeight;

        // This is done because the Icon's transparent color sometimes quits
        //     working.  This was the cause of the eternal pink icon problem.
        if (!HasRedrawn)
        {
            // Check to see if the Draw() method is broken.
            if (Canvas->Pixels[StartColumn + X][0] == clFuchsia)
            {
                HasRedrawn = true;

                // Replace the Fuchsia pixels with transparent.
                for (int i = StartColumn; i < EndColumn; i++)
                    for (int j = 0; j < IconHeight; j++)
                        if (IconCanvas->Pixels[i][j] == clFuchsia)
                            Canvas->Pixels[i + X][j] = WorkSpaceColor;
            }
        }        

        Canvas->StretchDraw(rect, Icon);
    }

    if (!Moving  ||
       (OwningI2adl->NameCode != NAME_INPUT  &&
        OwningI2adl->NameCode != NAME_OUTPUT))
    {
        PaintNames(DrawColor, Moving);
    }

    if (!OwningI2adl->IsPrimitive)
    {
        // If the behavior has been changed, then put a red "X" in the middle of the rectangle.
        if (OwningI2adl->Behavior == NULL)
        {
            Canvas->Pen->Color = clRed;
            Canvas->Pen->Width = max(int(ViewScale * .41), 1);
            Canvas->MoveTo(ViewScale, ViewScale * 1.41);
            Canvas->LineTo(Width - ViewScale, Height - ViewScale);
            Canvas->MoveTo(ViewScale, Height - ViewScale);
            Canvas->LineTo(Width - ViewScale, ViewScale * 1.41);
        }
    }

    PaintNodes();
}


// Handles the actual moving of I2adl objects.
void __fastcall MyTShapeI2adl::Move(int DeltaX, int DeltaY)
{
    Left -= DeltaX;
    Top -= DeltaY;
}


// Adds the object into the MoveList.
//     Objects already in the MoveList are skipped.
//     Objects already in the DragList (and any connected transports), are moved from the
//     DragList to the MoveList.
void MyTShapeI2adl::MoveListAdd()
{
    // Is the object already in the MoveList?
    if (MoveList->IndexOf(this) >= 0)
        return;

    // Add the object into the MoveList.
    Moving = true;
    MoveList->Add(this);

    // Record the left and top most positions to prevent objects from moving off the screen.
    if (MoveLimitX > Left)
        MoveLimitX = Left;

    if (MoveLimitY > Top)
        MoveLimitY = Top;

    // If the object is already in the DragList, then remove it from the DragList.
    if (DragList->IndexOf(this) >= 0)
    {
        DragList->Remove(this);

        // Recursive call to move any connected transports from the DragList to the MoveList.
        Node    *ConnectedInput = OwningI2adl->Inputs->Items[0]->ConnectedTo;
        Node    *ConnectedOutput = OwningI2adl->Outputs->Items[0]->ConnectedTo;

        if (ConnectedInput != NULL  &&  ConnectedInput->Owner->NameCode == NAME_TRANSPORT)
            ConnectedInput->Owner->I2adlShape->MoveListAdd();

        if (ConnectedOutput != NULL  &&  ConnectedOutput->Owner->NameCode == NAME_TRANSPORT)
            ConnectedOutput->Owner->I2adlShape->MoveListAdd();
    }
}


// Adds the transport into the DragList and sets it up for dragging.
//     Transports already in the MoveList are skipped.
//     Transports already in the DragList (and any connected transports), are moved from the
//     DragList to the MoveList.
void MyTShapeI2adl::DragListAdd(Node *_MovingNode, bool _FreeFloating)
{
    // Is the transport already in the MoveList?
    if (MoveList->IndexOf(this) >= 0)
        return;

    // If the transport already in the DragList, then move it from the DragList to the MoveList.
    if (DragList->IndexOf(this) >= 0)
    {
        MoveListAdd();

        return;
    }

    // Add the transport into the DragList and set it up for dragging.
    Moving = true;
    DragList->Add(this);

//    Paint();

    ((MyTShapeTransport *) this)->FreeFloating = _FreeFloating;
    SetUpNodeForDrag(_MovingNode);
}


// Sets up the transport for dragging by setting the moving and stationary nodes.
void __fastcall MyTShapeI2adl::SetUpNodeForDrag(Node *_MovingNode)
{
    I2adl                *Transport = _MovingNode->Owner;
    MyTShapeTransport    *TransportShape = (MyTShapeTransport *) Transport->I2adlShape;

    TransportShape->MovingNode = _MovingNode;
    TransportShape->StationaryNode = Transport->OtherEnd(_MovingNode);
}


void __fastcall MyTShapeI2adl::MouseDown(TMouseButton Button, TShiftState Shift, int X, int Y)
{
    if (Moving)
        return;

    if (!Shift.Contains(ssShift) && !Shift.Contains(ssCtrl) && !Shift.Contains(ssAlt))
		CanInsertJunction = true;
	else
		CanInsertJunction = false;

    Node    *NodeOver = GetNodeAt(TPoint(X, Y));

    if (NodeOver != NULL)
    {
        NodeOver->MouseDown(Button, X, Y);
        return;
    }

    if (DidDoubleClick)
    {
        if (OwningI2adl->NameCode == NAME_TRANSPORT)
        {
            DidDoubleClick = false;
            ResetMove();
            Selected = false;
            Paint();
        }

        return;
    }

    TGraphicControl::MouseDown(Button, Shift, X, Y);

    if (I2adlView->ActiveTransport != NULL)
    {
    	// Drawing a transport:   HandleTransportDraw() handles all cases.
        POINT    SheetPoint = I2adlView->ScreenToClient(ClientToScreen(Point(X, Y)));

        HandleTransportDraw(Button, SheetPoint.x, SheetPoint.y);

        return;
    }

    // right-click:
    if (Button == mbRight)
    {
        TerminateDraw();
        ResetMove(true);

        if (OwningI2adl->NameCode == NAME_TEXT)
            TextI2adlMemo->ShowMemo(this);
        else if (OwningI2adl->NameCode != NAME_TRANSPORT  &&
                 OwningI2adl->NameCode != NAME_JUNCTION)
            AttributeDialog->EditObjectAttributes(OwningI2adl, OwningI2adl);

        return;
    }

    // Left-click:
    if ((OwningI2adl->NameCode == NAME_TRANSPORT || OwningI2adl->NameCode == NAME_JUNCTION)
        && Shift.Contains(ssAlt) && !Shift.Contains(ssCtrl))
    {
        // Select the entire chain of transports to which this transport belongs, through
        //      all junctions:
        bool 	WasSelected = Selected;

        if (!Shift.Contains(ssShift))
            Deselect();

        Node 	*UpstreamNode;

        if (OwningI2adl->NameCode == NAME_JUNCTION)
        {
            UpstreamNode = OwningI2adl->Inputs->Items[0];
        }
        else
        {
            UpstreamNode = GetUpOrDownStreamNode(OwningI2adl->Outputs->Items[0]);

            if (UpstreamNode != NULL && UpstreamNode->Owner->NameCode != NAME_TRANSPORT)
                UpstreamNode = UpstreamNode->ConnectedTo;
        }

        PropagateOperation(UpstreamNode, poSelect, (void *) !WasSelected);
    }
    else if (Shift.Contains(ssShift))
    {
        Selected = true;
        Paint();
    }
    else if (Shift.Contains(ssCtrl))
    {
        Selected = !Selected;
        Paint();
    }
    else if (!Selected) // if we are not seleted then deselect everything else:
    {
        Deselect();
        Selected = true;
        Paint();
    }

    Invalidate();
    StartToMove(Left + X, Top + Y);
}                                                                                 


// Handles the double click event on an object.
void __fastcall MyTShapeI2adl::DblClick()
{
    if (Moving)
        return;

    Node	*NodeOver = GetNodeAt(ScreenToClient(Mouse->CursorPos));

    if (NodeOver == NULL)
    {
        // The user double clicked on the object.
        DidDoubleClick = true;

        ResetMove();
    }
    else
    {
	    // The user double clicked on a node.
		NodeOver->DblClick();
    }
}


// Handles movement of I2adl objects, including multiple selections.  Does transport draw
//      instead, if there is an active transport.
void __fastcall MyTShapeI2adl::MouseMove(TShiftState Shift, int X, int Y)
{
    if (ActiveTextI2adl != NULL)
        return;

    MainForm->ActiveControl = I2adlView; // So that I2adlEditor events will trigger.

    // Need to draw tail and return if there is an active transport in I2adlView:
    if (I2adlView->ActiveTransport != NULL)
    {
        TPoint		ClientPoint = I2adlView->ScreenToClient(ClientToScreen(TPoint(X, Y)));

        ((MyTShapeTransport *)I2adlView->ActiveTransport->I2adlShape)->DrawTail(
            ClientPoint.x, ClientPoint.y);

        ScrollBarCage(ClientPoint.x, I2adlView->HorzScrollBar);
        ScrollBarCage(ClientPoint.y, I2adlView->VertScrollBar);
    }

    Node    *NodeOver = GetNodeAt(TPoint(X, Y));

    // If moving, dragging or inactive control node, then ignore the mouse move over the node.
    if (NodeOver != NULL && NodeOver->DSet != NULL && !Moving && !Shift.Contains(ssLeft))
    {
    	if (Cursor != crCross)
        {
        	// Display the node cursor (cross hairs).  Setting/resetting the screen cursor is
            //		required to have the new cursor appear on the very 1st pixel.
            Cursor = crCross;
	        Screen->Cursor = crCross;
	        Screen->Cursor = crDefault;
            Application->ProcessMessages();
        }

        NodeOver->MouseMove(Shift, X, Y);

        return;
    }

    if (Cursor != crHandPoint)
    {
       	// Display the object cursor (hand pointer).  Setting/resetting the screen cursor is
        //		required to have the new cursor appear on the very 1st pixel.
        Cursor = crHandPoint;
        Screen->Cursor = crHandPoint;
        Screen->Cursor = crDefault;
        Application->ProcessMessages();
    }

    // Is the object's information already being displayed?
    if (NewCaptionSource(OwningI2adl))
        SetStatusCaption("Object: " + OwningI2adl->NameInfo->Name + "    Location: " +
            IntToStr(OwningI2adl->XLoc) + ", " + IntToStr(OwningI2adl->YLoc) + "    " +
            OwningI2adl->InputDataSets());

    TPoint ClientPoint = I2adlView->ScreenToClient(ClientToScreen(Point(X, Y)));

    if (AreShapesMoving)
    {
        if (Moving)
        {
            ControlMoveAndDrag(ClientPoint);
        }
        else
        {
            if (IsPastMoveTreshhold(ClientPoint))
            {
                if (Shift.Contains(ssCtrl) && !CopyInProcess)
                {
                    if (Selected)
                        Deselect();

                    Selected = true;
				}

                if (Shift.Contains(ssCtrl) && !Shift.Contains(ssAlt) && !CopyInProcess)
                {
                    ControlDragCopyAndPaste();
                }
                else
                {
                    DetermineMoveDragAll(Shift);
                }
            }
        }
    }
}


void __fastcall MyTShapeI2adl::MouseUp(TMouseButton Button, TShiftState Shift, int X, int Y)
{
    if (I2adlView->ActiveTransport != NULL)
    {
        TPoint ViewPoint = I2adlView->ScreenToClient(ClientToScreen(TPoint(X, Y)));
        I2adlEditor::MouseUp(Button, Shift, ViewPoint.x, ViewPoint.y);
        return;
    }

    TGraphicControl::MouseUp(Button, Shift, X, Y);

    PaintAllNames();

    CopyInProcess = false;

    if (DidDoubleClick)
    {
        DidDoubleClick = false;

        if (OwningI2adl->NameCode == NAME_TEXT)
        {
            TextI2adlMemo->ShowMemo(this);
        }
        else
        {
            if (Shift.Contains(ssCtrl))
            {
            	// Spam out a list of overloads into the "Overloads" tab sheet grid and select
                //		said tab sheet, so that the user can have his pick of overloads for
                //		this object:
                MainForm->ClearMessageGrid(MainForm->OverloadGrid);

                VList	*list = MyProject->GetOverloadList(OwningI2adl);

                if (list != NULL)
                {
                    for (int i = 0; i < list->Count; i++)
                    {
                        I2adl		*overload = (I2adl *) list->Items[i];
                        AnsiString	message = overload->EnhancedName()       + "     "
                            				+ overload->InputDataSets(false) + "     "
                                            + overload->ExternalName();

                        ErrorTrap	e(7000, message);

                        e.Invoke();
                        e.MessageInfo->IsDef = true;
                        e.MessageInfo->I2adlObject = overload;
                    }
                }
            }
			else
            {
                I2adl	*I2adlDef = OwningI2adl->ResolveObject(false, false, NULL, true);

                if (I2adlDef != NULL && I2adlDef->Behavior != NULL)
                {
                    // Make sure the behavior is still valid (on a project object).
                    LoadAsSheet(I2adlDef);

                    // Return here because wip Sheet has changed (this TShape was destroyed).
                    return;
                }
            }
        }
    }

    ConnectAllCompatibleNodes(OwningI2adl);

    if (I2adlView->ActiveTransport == NULL)
        ResetMove(true);

    SaveSheetState();
}


namespace Mytshapei2adl
{
    void __fastcall PACKAGE Register()
    {
        RegisterComponent(__classid(MyTShapeI2adl));
    }
}


/*****************************************************************
*                     My TShape Transport                        *
*****************************************************************/
__fastcall MyTShapeTransport::MyTShapeTransport(I2adl *_OwningI2adl)
    : MyTShapeI2adl(NULL)
{
    Parent = (MyTScrollingWinControl *) Owner;

    // _OwningI2adl was not passed to MyTShapeI2adl() so transports could be different.
    OwningI2adl = _OwningI2adl;

    if (_OwningI2adl == NULL)
        return;

    // Left and Top must be set before Height and Width:
    Left = _OwningI2adl->X;
    Top = _OwningI2adl->Y;
    Selected = OwningI2adl->Selected;

    // Position all nodes to gridsnap:
    int         Data = _OwningI2adl->Width;
    NodeList    *_IONodes = OwningI2adl->Inputs;
    Node        *ThisNode;
    int         NodeSize = OwningI2adl->NodeSize();

    for (int IO = 0; IO < 2; IO++) // for each NodeList
    {
        ThisNode = _IONodes->Items[0];

        if (ThisNode->Left > 0)
        {
            Node 	*ConnectedNode = ThisNode->ConnectedTo;
            
            if (ConnectedNode != NULL)
            {
                int ScrollMod = (ConnectedNode->Owner->I2adlShape == NULL)
                              ? I2adlView->HorzScrollBar->Position
                              : 0;
                              
                Data = ConnectedNode->Left + ConnectedNode->Owner->X
                    + ScrollMod + NodeSize - OwningI2adl->X;
                AlignToGrid(Data);
                ThisNode->Left = Data - NodeSize;
                break;
            }
            else
            {
                ThisNode->Left = ThisNode->Left; // Aligns to grid.
            }
        }

        _IONodes = OwningI2adl->Outputs;
    }

    AlignToGrid(Data);
    Width = Data;
    Data = _OwningI2adl->Height;
    _IONodes = OwningI2adl->Inputs;

    for (int IO = 0; IO < 2; IO++) // for each NodeList
    {
        ThisNode = _IONodes->Items[0];

        if (ThisNode->Top > 0)
        {
            Node *ConnectedNode = ThisNode->ConnectedTo;

            if (ConnectedNode != NULL)
            {
                int    ScrollMod = (ConnectedNode->Owner->I2adlShape == NULL)
                                 ? I2adlView->VertScrollBar->Position
                                 : 0;

                Data = ConnectedNode->Top + ConnectedNode->Owner->Y
                	+ ScrollMod + NodeSize - OwningI2adl->Y;

                AlignToGrid(Data);
                ThisNode->Top = Data - NodeSize;
                break;
            }
            else
            {
                ThisNode->Top = ThisNode->Top;
            }
        }

        _IONodes = OwningI2adl->Outputs;
    }

    AlignToGrid(Data);
    Height = Data;

    if (Width >= Height)
        IsHorizontal = true;
    else
        IsHorizontal = false;
}


void __fastcall MyTShapeTransport::SetWidth(int Data)
{
    AlignToGrid(Data);
    MyTShapeI2adl::SetWidth(Data);
}


void __fastcall MyTShapeTransport::SetHeight(int Data)
{
    AlignToGrid(Data);
    MyTShapeI2adl::SetHeight(Data);
}


// Must put scroll bar position in on write:
void  MyTShapeTransport::SetAnchorPoint(POINT P)
{
    FAnchorPoint.x = P.x + I2adlView->HorzScrollBar->Position;
    FAnchorPoint.y = P.y + I2adlView->VertScrollBar->Position;
}


// And take scroll bar position back out on read:
const POINT MyTShapeTransport::GetAnchorPoint()
{
    return TPoint(FAnchorPoint.x - I2adlView->HorzScrollBar->Position
                , FAnchorPoint.y - I2adlView->VertScrollBar->Position);
}


void __fastcall MyTShapeTransport::PaintArrows(int WhichArrow)
{
    // Is the transport long enough to display an arrow?
    if (Height < ViewScale * 7 && Width < ViewScale * 7)
        return;

    // Get the color of the data set.
    Canvas->Brush->Color = (TColor) OwningI2adl->Inputs->Items[0]->DSet->ColorCode;

    // Outline the arrow in either blue or black.
    if (Selected)
        Canvas->Pen->Color = clBlue;
    else
        Canvas->Pen->Color = clBlack;

    // Polygon point array that will draw the proper arrow.
    TPoint    Arrow[6];

    if (WhichArrow == 0)
    {
        // Input node on left
        Arrow[0] = Point(ViewScale * 2, ViewScale * .81);
        Arrow[1] = Point(ViewScale * 2, ViewScale * .41);
        Arrow[2] = Point(ViewScale * 4, ViewScale * .41);
        Arrow[3] = Point(ViewScale * 4, 0);
        Arrow[4] = Point(ViewScale * 4.21, 0);
        Arrow[5] = Point(ViewScale * 5, ViewScale * .81);
    }
    else if (WhichArrow == 1)
    {
        // Input node on right
        Arrow[0] = Point(Width - ViewScale * 2, ViewScale * .81);
        Arrow[1] = Point(Width - ViewScale * 2, ViewScale * .41);
        Arrow[2] = Point(Width - ViewScale * 4, ViewScale * .41);
        Arrow[3] = Point(Width - ViewScale * 4, 0);
        Arrow[4] = Point(Width - ViewScale * 4.21, 0);
        Arrow[5] = Point(Width - ViewScale * 5, ViewScale * .81);
    }
    else if (WhichArrow == 2)
    {
        // Input node on top
        Arrow[0] = Point(-int(ViewScale * .21), ViewScale * 2);
        Arrow[1] = Point(ViewScale * .41, ViewScale * 2);
        Arrow[2] = Point(ViewScale * .41, ViewScale * 4);
        Arrow[3] = Point(ViewScale * .81, ViewScale * 4);
        Arrow[4] = Point(ViewScale * .81, ViewScale * 4.21);
        Arrow[5] = Point(-int(ViewScale * .21), ViewScale * 5.21);
    }
    else
    {
        // Input node on bottom
        Arrow[0] = Point(-int(ViewScale * .21), Height - ViewScale * 2);
        Arrow[1] = Point(ViewScale * .41, Height - ViewScale * 2);
        Arrow[2] = Point(ViewScale * .41, Height - ViewScale * 4);
        Arrow[3] = Point(ViewScale * .81, Height - ViewScale * 4);
        Arrow[4] = Point(ViewScale * .81, Height - int(ViewScale * 4.21));
        Arrow[5] = Point(-int(ViewScale * .21), Height - int(ViewScale * 5.21));
    }

    // Draw the arrow on the transport.
    Canvas->Polygon(Arrow, 5);

#if 0
    // Draw a Rect/Line to cover the black edge of the arrow with DSet Color
    if (WhichArrow == 0)
        Canvas->FillRect(Rect(ViewScale * 2.11, ViewScale * .81, ViewScale * 5, ViewScale));
    else if (WhichArrow == 1)
        Canvas->FillRect(Rect(Width - int(ViewScale * 2), ViewScale * .81
    	, Width - int(ViewScale * 4.81), ViewScale));
#endif        
}


// Paint the proper transport icon
void __fastcall MyTShapeTransport::Paint()
{
    if (!Moving && OwningI2adl != I2adlView->ActiveTransport)
        PaintNames(DrawColor); 

    int			WhichArrow;
    TBitMap		*Icon;
    Node		*InputNode = OwningI2adl->Inputs->Items[0];
    int         DrawIncrement = ViewScale * 10;
    TRect		DrawRect;

    if (IsHorizontal)
    {
        if (Selected)
            Icon = TransportHor->SelectedBitmap;
        else
            Icon = TransportHor->Bitmap;

        if (InputNode->Left == 0)
            WhichArrow = 0;
        else
            WhichArrow = 1;

        if (Icon != NULL)
        {
            DrawRect.Top = 0;
            DrawRect.Bottom = ViewScale;

            for (int i = 0; i <= Width + Icon->Width; i += DrawIncrement)
            {
                DrawRect.Left = i;
                DrawRect.Right = i + DrawIncrement;

                Canvas->StretchDraw(DrawRect, Icon);
            }
        }
    }
    else // Vertical
    {
        if (Selected)
            Icon = TransportVer->SelectedBitmap;
        else
            Icon = TransportVer->Bitmap;

        if (InputNode->Top == 0)
            WhichArrow = 2;
        else
            WhichArrow = 3;

        if (Icon != NULL)
        {
            DrawRect.Left = 0;
            DrawRect.Right = ViewScale;

            for (int i = 0; i <= Height; i += DrawIncrement)
            {
                DrawRect.Top = i;
                DrawRect.Bottom = i + DrawIncrement;

                Canvas->StretchDraw(DrawRect, Icon);
            }
        }
    }

    PaintArrows(WhichArrow);  // Paint correct arrow.

    // Might be able to remove this with TGraphicControl use update:
    PaintNodes();
}


// Sets the moving/stationary nodes.
void __fastcall MyTShapeTransport::SetMovingStatus(Node *_MovingNode)
{
    Paint();
    MovingNode = _MovingNode;
    StationaryNode = OwningI2adl->OtherEnd(MovingNode);
    AnchorPoint = TPoint(StationaryNode->Left + Left, StationaryNode->Top + Top);
}


// Get the transport as close to (NewX, NewY) as possible by making it
//      Horizontal or Vertical.
void __fastcall MyTShapeTransport::DrawTail(int NewX, int NewY, bool HorizontalBias)
{
    //  Try to not connect transports from bottom or top;
    //      For the 'if' condition, if CanChangeOrthogType is false, then
    //      use IsHorizontal, preserving its orthoginal orientation; othewise,
    //      determine orthoginal orientation by its present position in relation
    //      to the node to which we are about to connect it:
    if ((abs(AnchorPoint.x - NewX) << (HorizontalBias ? 2 : 0)) >= abs(AnchorPoint.y - NewY))
    {
        // Horizontal
        int    Length = abs(AnchorPoint.x - NewX) + ViewScale;

        Length -= Length % ViewScale;
        OwningI2adl->InitTransport(AnchorPoint.x, AnchorPoint.y
            , true, Length, NewX >= AnchorPoint.x, true);
    }
    else
    {
        // Vertical
        int    Length = abs(AnchorPoint.y - NewY) + ViewScale;

        Length -= Length % ViewScale;
        OwningI2adl->InitTransport(AnchorPoint.x, AnchorPoint.y
            , false, Length, NewY >= AnchorPoint.y, true);
    }
}


// Drags (stretches, actually) the transport graphic the delta amounts.
void __fastcall MyTShapeTransport::Drag(int DeltaX, int DeltaY)
{
    PaintNames(WorkSpaceColor);
    // Are we strecting the left (or top) end of the transport?  We are if
    //    the moving node's coordinate correllating to the transport's
    //    orthoginal type is 0 (if Horizontal, compare X; otherwise, compare Y)
    bool 	DraggingOrigin = (IsHorizontal ? (MovingNode->Left == 0)
                                           : (MovingNode->Top == 0));
    bool 	Forward; // Opposite of DraggingOrigin; prospective state, not current.
    int 	NewLength;

    if (IsHorizontal)
    {
        // Localize the Stationary Node's and Moving Node's 'Left' property:
        int 	SLeft = StationaryNode->Left; // Stationary node's Left
        int 	MLeft = MovingNode->Left;     // Moving     node's Left

        // We should draw forward if the Moving Node's destination point is
        //    greater than the Stationary Node's current point:
        Forward = (SLeft <= MLeft - DeltaX);

        // Need to know whether we just switched which way we are going relative
        //    to the Stationary Node; compare current state with it's logically
        //    inverse prospective counterpart:
        bool 	OrientationChanged = (DraggingOrigin == Forward);

        // Initialize the tranpsort's graphical properties;
        // Follow this logic closely, and you can figure it out:

        // Convert this point's x value into screen coordinates:
        int 	NewLeft = I2adlView->ScreenToClient(ClientToScreen(Point(SLeft ,0))).x;

        OwningI2adl->CalculateHorzGridSnap(NewLeft);

        int NewTop = Top - (FreeFloating ? DeltaY : 0);

        NewLength = ((OrientationChanged)  ? (-Width + ViewScale * 2)
                                           : Width        )
                  + ((Forward)             ? -DeltaX
                                           : DeltaX       );
        NewLength -= NewLength % ViewScale;

        OwningI2adl->InitTransport(NewLeft, NewTop, true, NewLength, Forward, true);
    }
    else // Same as above except that we are vertical:
    {
        int STop = StationaryNode->Top;
        int MTop = MovingNode->Top;
        Forward = (STop < MTop - DeltaY);

        bool OrientationChanged = (DraggingOrigin == Forward);

        NewLength = ((OrientationChanged) ? (-Height + ViewScale * 2)
                                          : Height        )
                  + ((Forward)            ? -DeltaY
                                          : DeltaY        );
        NewLength -= NewLength % ViewScale;

        int NewLeft = Left - (FreeFloating ? DeltaX : 0);

        OwningI2adl->CalculateHorzGridSnap(NewLeft);

        int NewTop = I2adlView->ScreenToClient(ClientToScreen(Point(0 ,STop))).y;

        OwningI2adl->InitTransport(NewLeft, NewTop, false, NewLength, Forward, true);
    }

    Invalidate();
}


// This method inserts two new transports between the original transport and the FixedNode.
void __fastcall MyTShapeTransport::BreakSegment(Node *FixedNode)
{
    bool IsInput = FixedNode->GetType() == TYPE_INPUT;

    // Create a new vertical and a horizontal transport, change their datasets
    //      to conform to that of FixedNode, and create their shapes:
    I2adl    *NewVertical = new I2adl(NAME_TRANSPORT);
    NewVertical->ChangeDataSets(OwningI2adl->Inputs->Items[0]);
    NewVertical->I2adlShape = new MyTShapeTransport(NewVertical);

    I2adl    *NewHorizontal = new I2adl(NAME_TRANSPORT);
    NewHorizontal->ChangeDataSets(OwningI2adl->Inputs->Items[0]);
    NewHorizontal->I2adlShape = new MyTShapeTransport(NewHorizontal);

    bool Before; // If we are graphically to the left or top of FixedNode
    int  Origin; // Value of Left or Top
    int  Length; // Value of Width or Height

    if (IsHorizontal)
    {
        Length = Width;
        Origin = Left;
        Before = (Left < (FixedNode->Owner->I2adlShape->Left + FixedNode->Left));
    }
    else // The transport is vertical
    {
        Length = Height;
        Origin = Top;
        Before = (Top < (FixedNode->Owner->I2adlShape->Top + FixedNode->Top));
    }

    // These 3 variables mark the locations of the ends and middle, regardless
    //      of direction or orthoginal type.  InnerEnd is the end closest to
    //      FixedNode; therefore, OuterEnd is the other end:
    int 	RightEnd 	= Origin + Length - ViewScale;
    int 	InnerEnd   	= Before  ? RightEnd
                             	  : Origin;
    int 	OuterEnd   	= Before  ? Origin
                             	  : RightEnd;
    // Running left to right, or vice versa (Or top to bottom, or vice versa)?
    //  True by default; <Before> reverses it; <IsInput> reverses that:
    bool 	Forward 	= !(Before ^ IsInput);

    // Middle is where a transport should be drawn FROM under certain conditions;
    // the calculation for where it should be drawn TO has other operands.
    // This value should never be less than that of the leftmost (or topmost) node:
    int 	Middle  	= Origin + (Length / 2) - ViewScale;

    Middle      		= max(Middle, Before ? OuterEnd : InnerEnd);
//    Middle				= Middle + (Middle % ViewScale);
	AlignToGrid(Middle);

    int 	OuterLength = abs(Middle - OuterEnd) + ViewScale;
    int 	InnerLength = Length - OuterLength + ViewScale;

    Middle 				= Middle + ViewScale * .41;

    InnerLength = max(InnerLength, ViewScale);
    OuterLength = max(OuterLength, ViewScale);

    // The rest of this function is subject to change--please do not optimize!
    I2adl *InnerTransport;
    I2adl *OuterTransport;
    I2adl *MiddleTransport;

    if (IsHorizontal)
    {
//      Please leave the following lines:
//		InnerTransport = !IsInput ? OwningI2adl : NewHorizontal;
//		OuterTransport =  IsInput ? OwningI2adl : NewHorizontal;
        InnerTransport = NewHorizontal;
        OuterTransport = OwningI2adl;
        MiddleTransport = NewVertical;

        InnerTransport->InitTransport (IsInput ? Middle : InnerEnd, Top, IsHorizontal, InnerLength, Forward);
        OuterTransport->InitTransport (IsInput ? OuterEnd : Middle, Top, IsHorizontal, OuterLength, Forward);
        MiddleTransport->InitTransport(Middle, Top, !IsHorizontal, ViewScale, Forward);
    }
    else // The transport is vertical
    {
        InnerTransport = NewVertical;
        OuterTransport = OwningI2adl;
        MiddleTransport = NewHorizontal;

        InnerTransport->InitTransport (Left, IsInput ? Middle : InnerEnd, IsHorizontal, InnerLength, Forward);
        OuterTransport->InitTransport (Left, IsInput ? OuterEnd : Middle, IsHorizontal, OuterLength, Forward);
        MiddleTransport->InitTransport(Left, Middle, !IsHorizontal, ViewScale, Forward);
    }

    if (IsInput)
    {
        InnerTransport->Outputs->Items[0]->Connect(FixedNode);
        InnerTransport->Inputs->Items[0]->Connect(MiddleTransport->Outputs->Items[0]);
        OuterTransport->Outputs->Items[0]->Connect(MiddleTransport->Inputs->Items[0]);
    }
    else
    {
        InnerTransport->Inputs->Items[0]->Connect(FixedNode);
        InnerTransport->Outputs->Items[0]->Connect(MiddleTransport->Inputs->Items[0]);
        OuterTransport->Inputs->Items[0]->Connect(MiddleTransport->Outputs->Items[0]);
    }

    CurrentSheet->Behavior->Add(NewVertical);
    CurrentSheet->Behavior->Add(NewHorizontal);
}


// Handles the double click event on a transport.
void __fastcall MyTShapeTransport::DblClick()
{
    if (Moving ||
    	I2adlView->ActiveTransport != NULL)
	        return;

    // Clicks right next to a node are consider as on the node.  This prevents a 5x5 transport
    //     that can be deleted in its own event handler.
    Node	*NodeOver = GetNodeAt(ScreenToClient(Mouse->CursorPos), ViewScale);

    if (NodeOver == NULL)
    {
    	// The user double clicked on the transport.
        if (CanInsertJunction)
        {
            // Calculate the new Junction's X and Y location.
            TPoint	P = ScreenToClient(TPoint(Mouse->CursorPos.x, Mouse->CursorPos.y));
            int		X = P.x;
            int		Y = P.y;

            // Transports and junctions are 1 pixel to the right of the grid:
            X -= ((X + Left) % ViewScale) + ViewScale * .8;
            Y -= ((Y + Top) % ViewScale) + ViewScale;

            InsertJunction(true, X, Y);
        }

        ResetMove();
    }
    else
    {
		// The user double clicked on a node.
		NodeOver->DblClick();
    }
}


void MyTShapeTransport::InsertJunction(bool ConnectItUp, int X, int Y, bool SkipActiveTran,
	I2adl *NewSegment, Node *OwningNode, Node *ConnectedNode)
{
	AlignToGrid(X);
	AlignToGrid(Y);

    // Localize pointers for the transport's input and output nodes:
    Node	*OrigInput  = OwningI2adl->Inputs->Items[0];
    Node	*OrigOutput = OwningI2adl->Outputs->Items[0];
    int 	OldLeft = Left, OldTop = Top, OldWidth = Width, OldHeight = Height;
    I2adl	*NewJunction;

	// Create a new junction and a transport and its shape:
    if (IsHorizontal)
        NewJunction = new I2adl(NAME_JUNCTION, Left + X, Top - ViewScale);
    else
        NewJunction = new I2adl(NAME_JUNCTION, Left - ViewScale, Top + Y);

    // Junction also needs initialization and a shape:
    NewJunction->ChangeDataSets(OrigInput);
    NewJunction->CreateShape();

    //        ***Move the nodes around to the correct positions***
    // How much are we going to drag the new and old transports (X, Y)?
    //                          	X               			Y
    int 	DragAmounts[4][2] = {  {Width - X - ViewScale,  	0},
                               	{0,              				Height - Y - ViewScale},
                               	{-ViewScale * 2 - X,			0},
                               	{0,              				-ViewScale * 2 - Y}		};

    //                    Now connect up the Junction:

    int NewInputPos; // Where the new input will be.
    int NewOutputPos; // Where the new output will be.
    MyTShapeTransport *HorzXShape;
    MyTShapeTransport *VertXShape;

    bool 	Forward = IsForward();

    // If applicable, then create, initialize, and add the new segment:
    if (!NewSegment)
    {
        NewInputPos = 3 - (Forward * 2) - IsHorizontal;

        // OrigOutput->ConnectedTo is about to change; save current address:
        Node    *SaveNode = OrigOutput->ConnectedTo;
        OrigOutput->Connect(NewJunction->Inputs->Items[0]);

        SetUpNodeForDrag(OrigOutput);
        Drag(DragAmounts[NewInputPos][0], DragAmounts[NewInputPos][1]);

        NewSegment = new I2adl(NAME_TRANSPORT, OldLeft, OldTop);
        NewSegment->ChangeDataSets(OrigInput);

        MyTShapeTransport 	*NewTShape = new MyTShapeTransport(NewSegment);

        NewSegment->I2adlShape = NewTShape;
        NewTShape->IsHorizontal = IsHorizontal;
        CurrentSheet->Behavior->Add(NewSegment);

        if (IsHorizontal)
        {
            // If we are drawing forward, then we start the new transport
            //  just after the junction (Left + X + ViewScale * 2).  If not, start it at
            //  the end of where the old transport was (Left + X);
            //  the computation of the length is also based on <Forward> :
            NewSegment->InitTransport((Forward) ? OldLeft + X + ViewScale * 2 : OldLeft + X
                , OldTop, true, (Forward) ? OldWidth - X - ViewScale * 2 : X + ViewScale, Forward);
        }
        else
        {
            // Same logic applies as above, except that we use Y instead of X:
            NewSegment->InitTransport(OldLeft, (Forward) ? OldTop + Y + ViewScale * 2 : OldTop + Y,
            false, (Forward) ? OldHeight - Y - ViewScale * 2 : Y + ViewScale, Forward);
        }

        NewSegment->Inputs->Items[0]->Connect(NewJunction->Outputs->Items
            [   (NewInputPos ^ 2) - (NewInputPos < 2)  ]);
        NewSegment->Outputs->Items[0]->Connect(SaveNode);
    }
    else
    {
        HorzXShape = IsHorizontal ? this
                                  : (MyTShapeTransport *) ConnectedNode->Owner->I2adlShape;
        VertXShape = IsHorizontal ? (MyTShapeTransport *) ConnectedNode->Owner->I2adlShape
                                  : this;

        // Which kind of corner we are dealing with: starting upper-left
        //      at 0 and moving clockwise:
        unsigned int nCorner;
        // use the horizontal transport for comparison of node positions:
        if (IsHorizontal)
        {
            // we want to assume that we are on corner 1;
            // add 2 if we are on the left side; add 1 if:
            //      1. The node that we are connected to is on bottom
            //  or  2. We are on the right
            //  but not if both are true.
            nCorner = 1 + (OwningNode->Left == 0) * 2;
            nCorner += ((ConnectedNode->Top == 0) ^ (nCorner <= 2));
            nCorner &= 3; // enables wrap-around after 3, for add.
        }
        else // use other node for comparison basis, identical otherwise:
        {
            nCorner = 1 + (ConnectedNode->Left == 0) * 2;
            nCorner += ((OwningNode->Top == 0) ^ (nCorner <= 2));
            nCorner &= 3;
        }

        int LeftMod = (nCorner == 0 || nCorner == 3)    ? ViewScale
                                                        : 0;
        int TopMod  = (nCorner < 2)                     ? ViewScale
                                                        : 0;

        bool HorzForward = (HorzXShape->OwningI2adl->Inputs->Items[0]->Left
            <= HorzXShape->OwningI2adl->Outputs->Items[0]->Left);
        bool VertForward = (VertXShape->OwningI2adl->Inputs->Items[0]->Top
            <= VertXShape->OwningI2adl->Outputs->Items[0]->Top);

        int NewLeft = HorzForward ? HorzXShape->Left
                                  : HorzXShape->Left + HorzXShape->Width - ViewScale * 2;

        int NewTop  = VertForward ? VertXShape->Top
                                  : VertXShape->Top + VertXShape->Height - ViewScale * 2;

        HorzXShape->OwningI2adl->InitTransport(NewLeft + LeftMod, HorzXShape->Top
            , true, HorzXShape->Width - ViewScale, HorzForward);
        VertXShape->OwningI2adl->InitTransport(VertXShape->Left, NewTop + TopMod
            , false, VertXShape->Height - ViewScale, VertForward);

        if (IsHorizontal)
            NewJunction->I2adlShape->Left -= ViewScale;
        else
            NewJunction->I2adlShape->Top -= ViewScale;

        // Is OwningNode an output?
        bool IsOutput   = (OwningNode->GetType() == TYPE_OUTPUT);

        // Is the transport having an output that will connect to the Junction horizontal?
        bool OutputHorz = !(IsOutput ^ IsHorizontal); // <^> because if one isn't, the other is.

        // The graphical positions of the nodes in question (0-3 starting at 9:00, remember)
        // is affected by 2 (180 degrees) by the direction of its data flow, and affected by
        // 1 by the orthoginal type of the transports to which they will connect:
        bool OutputForward = (OutputHorz)  ? HorzForward
                                           : VertForward;
        bool InputForward  = (!OutputHorz) ? HorzForward
                                           : VertForward;
        NewInputPos   = 2 * !OutputForward + !OutputHorz;
        NewOutputPos  = 2 * InputForward  + OutputHorz;

        // This output's Ordinal position in junction's output array is offset by 1 if the input
        // is between the first graphical position and that of the output:
        bool offset = NewOutputPos > NewInputPos;

        // Disconnect transports from each other and connect them to the appropriate
        // nodes on the new junction:
        ConnectedNode->Disconnect();
        OwningNode->Disconnect();
        if (OwningNode->GetType() == TYPE_OUTPUT)
        {
            OwningNode->Connect(NewJunction->Inputs->Items[0]);
            ConnectedNode->Connect(NewJunction->Outputs->Items[NewOutputPos - offset]);
        }
        else
        {
            OwningNode->Connect(NewJunction->Outputs->Items[NewOutputPos - offset]);
            ConnectedNode->Connect(NewJunction->Inputs->Items[0]);
        }
    }

    VivaGlobals::PlaceJunctionNodes(NewJunction->Inputs->Items[0], NewInputPos);

    CurrentSheet->Behavior->Add(NewJunction);

    Selected = OwningI2adl->Selected;
    Paint();

    // Repaint the new junction to remove four gray squares:
    NewJunction->I2adlShape->Paint();
    DidDoubleClick = true;

    // If this junction is near an unconnected node, then connect it up.
    if (ConnectItUp)
    	ConnectAllCompatibleNodes(NewJunction, 0, 0, SkipActiveTran);
}


bool MyTShapeTransport::IsForward()
{
    if (IsHorizontal)
        return OwningI2adl->Inputs->Items[0]->Left == 0;
    else
        return OwningI2adl->Inputs->Items[0]->Top == 0;
}


namespace Mytshapetransport
{
    void __fastcall PACKAGE Register()
    {
        RegisterComponent(__classid(MyTShapeTransport));
    }
}



