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

#ifndef MyTScrollingWinControlH
#define MyTScrollingWinControlH

#include "globals.h"

class PACKAGE MyTScrollingWinControl : public TScrollingWinControl
{
public:

    I2adl      *ActiveTransport;       // Existing transport that new transport will attach to
    TPoint     SelectStartPoint;       // Start point of selection rectangle
    TCanvas    *Canvas;                // The painting canvas of the I2adl View background

    __fastcall MyTScrollingWinControl(TComponent *Owner);
    __fastcall~MyTScrollingWinControl();

    virtual void __fastcall PaintWindow(HDC DC);
    void __fastcall DrawSelectRect(int X, int Y);
    void __fastcall Reset();
    void SetDefaultFont();
    double GetTextWidth(const AnsiString & String);
    void DoWndProc(TMessage &Message) {WndProc(Message);};
    I2adl *AttachedI2adl();

__published:

    __property Align;
    __property TabStop;
    __property TabOrder;
    __property Enabled;
    __property Visible;
    __property ControlStyle;
    __property DragMode;
    __property OnClick;
    __property OnDblClick;
    __property OnDragDrop;
    __property OnDragOver;
    __property OnEndDrag;
    __property OnMouseDown;
    __property OnMouseMove;
    __property OnMouseUp;
    __property OnStartDrag;
//    __property OnMouseWheel;
};

#endif
