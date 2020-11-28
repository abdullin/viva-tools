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

#ifndef WidgetH
#define WidgetH

#include "WidgetForm.h"

#define WIDGET_MARGIN 2

class VWidgetLabel;
typedef void (__closure *WidgetFuncPtr)();

class TWidget : public TCustomControl
{
private:
    bool                    fSelected;			// Selected for move/resize

__published:
    __property int          Top = { read = GetTop, write = SetTop };
    __property int          Left = { read = GetLeft, write = SetLeft };
    __property TPopupMenu	*_PopupMenu = {read = PopupMenu, write = PopupMenu};
    __property bool			Selected = {read = fSelected, write = SetSelected};

public:
    int                     OrigX;				// X position before move operation started
    int                     OrigY;				// Y position before move operation started
    int                     MinValue;			// Minimum value for user input entry
    int                     MaxValue;			// Maximum value for user input entry
    bool                    HexFormatting;		// Output display in hex
    int                     OutputFileHandle;	// File into which all output values are spammed
    int                     WidgetNameCode;		// Code for which type of widget
    bool					DisableOutputs;		// If true, then output widgets will be
    											//		disabled unless selected.
    Node                    *SourceNode;		// Node on IO horn with which this communcates
    TStringList             *PopupMenuList;     // Captions for commands on pop-up menu 
    WidgetFuncPtr           PopupFuncs[POPUP_MENU_LIMIT]; // Commands on pop-up menu
    TWidgetForm             *ParentForm;        // Widget form on which this resides
    TShape                  *Box;               // Outer rim that appears when this is selected
    VWidgetLabel            *WidgetLabel;     // Caption of the widget's name
    											//		(from source IO horn's node)
    TControl                *MainControl;		// Control responsible for IO

                            TWidget(TComponent *_Owner, Node *_SourceNode);
    __fastcall              ~TWidget();

    int __fastcall          GetTop();
    int __fastcall          GetLeft();
    void __fastcall         SetTop(int Value);
    void __fastcall         SetLeft(int Value);
    void __fastcall         SetSelected(bool Value);
    void                    BeginResize();
    void                    PlaceWidget(int _Left, int _Top);
    void                    MarkState();
    void                    RestoreState();
    virtual void            OnWidgetResize(bool ChangeHeight = true);
    virtual void __fastcall OnWidgetChange(TObject *Sender);
    virtual void            SetMinMaxValues(TStrings *Attributes);
    virtual void            UpdateDisplay(){};
    virtual void			SetValue(int);
    virtual void 			SendValue();
    void					SendValue(int);
    void					SendValue(BYTE *Value, int Max);
	void					MakeSpace();
    void __fastcall         OnWidgetEnter(TObject *Sender);
    void                    ResizeWidget(int _Width, int _Height);
    void                    EditAttributes();
    void __fastcall         AddPopupMenuItem(AnsiString MenuName, void (__closure *Function)());
    void                    ToggleHexFormatting();
    AnsiString              FormatData(unsigned char *DPtr = NULL);
    void                    SaveDataToDisk();
    void                    SaveAttribute(AnsiString AttName, AnsiString Value);

    DYNAMIC __fastcall void MouseMove(Classes::TShiftState Shift, int X, int Y);
    DYNAMIC __fastcall void MouseDown(TMouseButton Button, Classes::TShiftState Shift, int X,
        int Y);
    DYNAMIC __fastcall void MouseUp(TMouseButton Button, TShiftState Shift, int X, int Y);

    void __fastcall         WidgetWndProc(Messages::TMessage &Message); // May use in future.
};


// A label that exists almost exclusively for caption; doesn't intercept most events, like a
//		normal label does.
class VWidgetLabel : public TLabel
{
    // Sends most mouse messages to the owning Widget:
    void __fastcall CovertLabelWndProc(Messages::TMessage &Message)
    {
        switch (Message.Msg)
        {
            case WM_MOUSEMOVE:
            case WM_LBUTTONDOWN:
            case WM_RBUTTONDOWN:
            case WM_LBUTTONUP:
            case WM_LBUTTONDBLCLK:
            case WM_RBUTTONDBLCLK:
                // If enabled, then owning Widget will send command back here, causing an
                //      infinite loop:
                Enabled = false;
                Parent->Perform(Message.Msg, Message.WParam, Message.LParam);
                Enabled = true;
                break;
            default:
                WndProc(Message);
                break;
        }
    }

public:
    __fastcall virtual VWidgetLabel(Classes::TComponent *AOwner) :TLabel(AOwner)
    {
        WindowProc = CovertLabelWndProc;
    }
};


#endif
