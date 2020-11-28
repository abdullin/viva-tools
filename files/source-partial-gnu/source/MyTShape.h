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

#ifndef MyTShapeH
#define MyTShapeH

#define ICON_CENTER_RESERVE		20 // ViewScale * 4 // Middle part of icon protected from transports .

#define trRight     0x01        // Flag bit set if right edge of icon contains data
#define trLeft      0x02        // Flag bit set if left edge of icon contains data

class PACKAGE MyTShape : public TGraphicControl
{
public:
	bool CanInsertJunction;

    __fastcall MyTShape(TComponent *Owner = NULL);

    DYNAMIC __fastcall void DragOver(System::TObject *Source, int X, int Y
		, TDragState State, bool &Accept);
    DYNAMIC __fastcall void DragDrop(System::TObject* Source, int X, int Y);
};


class PACKAGE MyTShapeI2adl : public MyTShape
{
    bool    FSelected;		// Selected for move, delete, etc

public:
    bool    		Moving;				// Being moved
    I2adl   		*OwningI2adl;		// The I2adl object the TShape represents
    bool			HasObjectRef;		// The OwningI2adl has an ObjectRef attribute
    TStringList		*ObjectAttributes;	// Sorted list of object attributes preformated for
    									//		displaying below the object.

    __fastcall MyTShapeI2adl(I2adl *_OwningI2adl);
    void __fastcall BeforeDestruction();

    virtual __fastcall void SetLeft(int _Left);
    virtual __fastcall void SetTop(int _Top);
    virtual __fastcall void SetWidth(int _Width);
    virtual __fastcall void SetHeight(int _Height);
    void SetInitialHeight();
    Node *GetNodeAt(TPoint P, int SnapZone = 0);
    virtual __fastcall void PaintBox();
    TCanvas *GetCanvas() { return Canvas; }
    void __fastcall SetCanvasText();
    void __fastcall PaintNames(TColor color, bool SkipObjectRef = false);
    void __fastcall PaintNodes();
    void __fastcall WidenIcon();
    virtual __fastcall void Paint();
    __fastcall void Move(int DeltaX, int DeltaY);
    void MoveListAdd();
    void DragListAdd(Node *_MovingNode, bool _FreeFloating);
    void __fastcall SetUpNodeForDrag(Node *_MovingNode);
    void __fastcall InitFont();
    void __fastcall ChangeBoundsRect(TRect& Rect){UpdateBoundsRect(Rect);}
    int __fastcall GetLeft();
    int __fastcall GetTop();
    int __fastcall GetWidth();
    int __fastcall GetHeight();
    void __fastcall SetSelected(bool Value);

	__property bool Selected = { read = FSelected, write = SetSelected };

__published:
    __property int Left   = { read = GetLeft,   write = SetLeft   };
    __property int Top    = { read = GetTop,    write = SetTop    };
    __property int Width  = { read = GetWidth,  write = SetWidth  };
    __property int Height = { read = GetHeight, write = SetHeight };

    DYNAMIC __fastcall void MouseDown(TMouseButton Button, Classes::TShiftState Shift, int X,
        int Y);
    DYNAMIC __fastcall void DblClick();
    DYNAMIC __fastcall void MouseMove(Classes::TShiftState Shift, int X, int Y);
    DYNAMIC __fastcall void MouseUp(TMouseButton Button, TShiftState Shift, int X, int Y);
};


class PACKAGE MyTShapeTransport : public MyTShapeI2adl
{
__published:
    DYNAMIC __fastcall void DblClick();

private:
    POINT    FAnchorPoint;		// The location of the stationary node.

public:
    bool     IsHorizontal;		// Is horizontal, rather than vertical
    bool     FreeFloating;		// Can move in all directions
    Node     *MovingNode;		// In a stretch operation, the node that is being moved.
    Node     *StationaryNode;	// In a stretch operation, the node that is not being moved.

    __fastcall MyTShapeTransport(I2adl *_OwningI2adl = NULL);

    virtual __fastcall void SetWidth(int _Width);
    virtual __fastcall void SetHeight(int _Height);
    virtual __fastcall void Paint();
    virtual __fastcall void DrawTail(int NewX, int NewY, bool HorizontalBias = false);
    void SetAnchorPoint(POINT P);
    const POINT GetAnchorPoint();
    void __fastcall PaintArrows(int WhichArrow);
    __fastcall void SetMovingStatus(Node *_MovingNode);
    __fastcall void Drag(int DeltaX, int DeltaY);
    void __fastcall BreakSegment(Node *FixedNode);
    void InsertJunction(bool ConnectItUp, int X, int Y, bool SkipActiveTran = false,
    	I2adl *NewSegment = NULL, Node *OwningNode = NULL, Node *ConnectedNode = NULL);
    bool IsForward();

    __property int Width  = { read = GetWidth,  write = SetWidth  };
    __property int Height = { read = GetHeight, write = SetHeight };
    __property POINT AnchorPoint = {read = GetAnchorPoint, write = SetAnchorPoint};
};

#endif
