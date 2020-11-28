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
#include "Project.h"
#include "MyTShape.h"
#include "I2adl.h"
#include "Node.h"
#include "MyTScrollingWinControl.h"
#include "I2adlEditor.h"
#include "VivaIcon.h"

#pragma package(smart_init)


__fastcall MyTScrollingWinControl::MyTScrollingWinControl(TComponent *Owner) :
    TScrollingWinControl(Owner)
{
    Canvas = new TCanvas();

    HorzScrollBar->Tracking = true;
    VertScrollBar->Tracking = true;

    SelectRectTop = new TShape(this);
    SelectRectTop->Parent = this;
    SelectRectTop->Shape = stRectangle;
    SelectRectTop->Pen->Style = psClear;
    SelectRectTop->Height = SELECT_RECT_WIDTH;
    SelectRectTop->Visible = false;

    SelectRectLeft = new TShape(this);
    SelectRectLeft->Parent = this;
    SelectRectLeft->Shape = stRectangle;
    SelectRectLeft->Pen->Style = psClear;
    SelectRectLeft->Width = SELECT_RECT_WIDTH;
    SelectRectLeft->Visible = false;

    SelectRectRight = new TShape(this);
    SelectRectRight->Parent = this;
    SelectRectRight->Shape = stRectangle;
    SelectRectRight->Pen->Style = psClear;
    SelectRectRight->Width = SELECT_RECT_WIDTH;
    SelectRectRight->Visible = false;

    SelectRectBottom = new TShape(this);
    SelectRectBottom->Parent = this;
    SelectRectBottom->Shape = stRectangle;
    SelectRectBottom->Pen->Style = psClear;
    SelectRectBottom->Height = SELECT_RECT_WIDTH;
    SelectRectBottom->Visible = false;
}


__fastcall MyTScrollingWinControl::~MyTScrollingWinControl()
{
    delete Canvas;
    delete SelectRectTop;
    delete SelectRectLeft;
    delete SelectRectRight;
    delete SelectRectBottom;
}


void __fastcall MyTScrollingWinControl::PaintWindow(HDC DC)
{
    if (!MyProject)
        return;

    Canvas->Handle = DC;

    Canvas->Brush->Color = WorkSpaceColor;
    Canvas->Brush->Style = bsSolid;

    Canvas->Font->Name = "Arial";
    Canvas->Font->Size = ViewScale * 2.41;
    Canvas->Font->Style = TFontStyles() << fsBold << fsUnderline;

    // Error message while creating a project can cause the following.
    if (CurrentSheet == NULL)
        return;

	AnsiString	SheetTitle(CurrentSheet->NameInfo->Name);
    TPoint		p1 = TPoint(10 - HorzScrollBar->Position, 5 - VertScrollBar->Position);
    TPoint		p2 = TPoint(p1.x + Canvas->TextWidth(SheetTitle), p1.y + Canvas->
    	TextHeight(SheetTitle));

    Canvas->FillRect(TRect(p1, p2));

    if (CurrentSheet != NULL && ProjectObjects != NULL)
    {
		I2adl	*copyof = CurrentSheet->CopyOf;

        // Avoid an "access into deleted memory" by making sure the object still exists.
        if (copyof != NULL &&
        	ProjectObjects->IndexOf(copyof) > -1)
        {
            AnsiString		doc(copyof->Attributes->Values[strDocumentation]);

            if (doc != strNull)
                SheetTitle += " (" + doc + ")";
        }
    }

    Canvas->Brush->Style = bsClear;
    Canvas->Font->Color = DrawColor;
    Canvas->TextOutA(p1.x, p1.y, SheetTitle);
    Canvas->Font->Style = TFontStyles();
}


// This function will calculate and display the selection rectangle shapes.
void __fastcall MyTScrollingWinControl::DrawSelectRect(int X, int Y)
{
    // The color is initialized here so the color is changed to the correct DrawColor
    SelectRectTop->Brush->Color = DrawColor;
    SelectRectLeft->Brush->Color = DrawColor;
    SelectRectRight->Brush->Color = DrawColor;
    SelectRectBottom->Brush->Color = DrawColor;

	// Get it to pan:
    SelectStartPoint.x = SelectStartPoint.x - ScrollBarCage(X, I2adlView->HorzScrollBar);
    SelectStartPoint.y = SelectStartPoint.y - ScrollBarCage(Y, I2adlView->VertScrollBar);

    // Record the outter most points.
    SelectRectLeft->Left = min(X, (int) SelectStartPoint.x);
    SelectRectRight->Left = max(X, (int) SelectStartPoint.x);
    SelectRectTop->Left = SelectRectLeft->Left;
    SelectRectBottom->Left = SelectRectLeft->Left;

    SelectRectTop->Top = min(Y, (int) SelectStartPoint.y);
    SelectRectBottom->Top = max(Y, (int) SelectStartPoint.y);
    SelectRectLeft->Top = SelectRectTop->Top;
    SelectRectRight->Top = SelectRectTop->Top;

    // Calculate the width & height.  Other 4 heights/widths remain SELECT_RECT_WIDTH pixels.
    SelectRectTop->Width = SelectRectRight->Left - SelectRectLeft->Left + SELECT_RECT_WIDTH;
    SelectRectLeft->Height = SelectRectBottom->Top - SelectRectTop->Top + SELECT_RECT_WIDTH;
    SelectRectBottom->Width = SelectRectTop->Width;
    SelectRectRight->Height = SelectRectLeft->Height;
}


void __fastcall MyTScrollingWinControl::Reset()
{
    ActiveTransport = NULL;
    TabStop = true;
    HorzScrollBar->Position = 0;
    VertScrollBar->Position = 0;
    AreShapesMoving =  false;
}


void MyTScrollingWinControl::SetDefaultFont()
{
	SetStandardFont(Canvas->Font);
}


double MyTScrollingWinControl::GetTextWidth(const AnsiString &String)
{
    HDC		dc = GetDC(Handle);
    int		tempviewscale = ViewScale;

    ViewScale = VIEW_SCALE_DEFAULT;

    Canvas->Handle = dc;

	ReallocateDC(I2adlView->Canvas->Font);
	SetDefaultFont();

    int		TWidth = Canvas->TextWidth(String);
    int		retval = double(TWidth * ViewScale) / VIEW_SCALE_DEFAULT;

    ReleaseDC(Handle, dc);

    ViewScale = tempviewscale;

    return retval;
}


// Returns the transport attached to the ActiveTransport (or NULL).
I2adl *MyTScrollingWinControl::AttachedI2adl()
{
    if (ActiveTransport != NULL)
    {
        MyTShapeTransport	*ActiveShape = (MyTShapeTransport *) ActiveTransport->I2adlShape;

        if (ActiveShape != NULL &&
            ActiveShape->StationaryNode != NULL)
        {
            Node	*ActiveConnectedTo = ActiveShape->StationaryNode->ConnectedTo;

            if (ActiveConnectedTo != NULL &&
            	ActiveConnectedTo->Owner->NameCode == NAME_TRANSPORT)
		            return ActiveConnectedTo->Owner;
        }
    }

    return NULL;
}


namespace Mytscrollingwincontrol
{
    void __fastcall PACKAGE Register()
    {
        TComponentClass classes[1] =
        {
            __classid(MyTScrollingWinControl)
        };

        RegisterComponents("Additional", classes, 0);
        RegisterClasses(classes, 0);
    }
}


