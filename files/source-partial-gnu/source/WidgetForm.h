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

#ifndef WidgetFormH
#define WidgetFormH

#include <Menus.hpp>
#include <Controls.hpp>
#include <Classes.hpp>
#include <ExtCtrls.hpp>
#include <ComCtrls.hpp>

#include "globals.h"
#include "VivaComForm.h"

#define STATE_NORMAL             0
#define STATE_MOVING             1
#define WIDGET_GRID_SNAP         5
#define POPUP_MENU_LIMIT	    10

#define UBOUND(i) (sizeof(i) / sizeof(i[0]) - 1)

extern AnsiString   WidgetNames[];
extern int          InputWidgetOptions[];
extern int          OutputWidgetOptions[];

class VTabSheet : public TTabSheet
{
public:
	TScrollBox	*WidgetHolder;	// Adds scroll bars to each widget tab sheet.
    int			PositionHorz;
    int			PositionVert;

	__fastcall virtual VTabSheet(Classes::TComponent* AOwner) : TTabSheet(AOwner){};

	__property TKeyEvent _OnKeyDown = {read=OnKeyDown, write=OnKeyDown};
	__property TKeyEvent _OnKeyUp = {read=OnKeyUp, write=OnKeyUp};    
};


class TWidgetForm : public TComForm
{
__published:
    TPopupMenu *WidgetPopupMenu;
    TPopupMenu *WidgetFormPopupMenu;
    TMenuItem *Align;
    TMenuItem *AlignBottom;
    TMenuItem *AlignCenter;
    TMenuItem *AlignTop;
    TMenuItem *AlignRight;
    TMenuItem *AlignMiddle;
    TMenuItem *AlignLeft;
    TMenuItem *ChangeTo;
    TMenuItem *Edit1;
    TMenuItem *SelectAllCtrlA1;
	TMenuItem *cmdSelectToggle;
	TMenuItem *cmdNewTabSheet;
	TPageControl *PageControl;
	TMenuItem *cmdMoveWidgetTabSheet;
	TShape *SelectionBox;
	TMenuItem *cmdRenameTabSheet;

    void __fastcall MoveMultiple(int X, int Y, TObject *Sender);
    void __fastcall RestoreSelection();
    void __fastcall WidgetPopupMenuItemClick(TObject *Sender);
    void __fastcall AlignClick(TObject *Sender);
    void __fastcall Edit1Click(TObject *Sender);
    void __fastcall SelectAllClick(TObject *Sender);
    void __fastcall FormMouseDown(TObject *Sender, TMouseButton Button, TShiftState Shift,
        int X, int Y);
    void __fastcall FormMouseMove(TObject *Sender, TShiftState Shift, int X, int Y);
    void __fastcall FormMouseUp(TObject *Sender, TMouseButton Button, TShiftState Shift,
        int X, int Y);
    void __fastcall FormDragDrop(TObject *Sender, TObject *Source, int X, int Y);
    void __fastcall FormKeyDown(TObject *Sender, WORD &Key, TShiftState Shift);
    void __fastcall FormKeyUp(TObject *Sender, WORD &Key, TShiftState Shift);
    void __fastcall WidgetPopupMenuPopup(TObject *Sender);
    void __fastcall FormShow(TObject *Sender);
    void __fastcall FormHide(TObject *Sender);
    void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
	void __fastcall FormPaint(TObject *Sender);
	void __fastcall cmdNewTabSheetClick(TObject *Sender);
	void __fastcall cmdMoveWidgetTabSheetClick(TObject *Sender);
	void __fastcall PageControlChange(TObject *Sender);
	void __fastcall cmdSelectToggleClick(TObject *Sender);
	void __fastcall FormResize(TObject *Sender);
	void __fastcall PageControlChanging(TObject *Sender, bool &AllowChange);
	void __fastcall cmdRenameTabSheetClick(TObject *Sender);

public:
    bool        SelectingRegion;		// Selecting a region for widget selection/toggle
    int         OrigPopupMenuCount;		// Number of popup menu items that apply to all widgets
    int         SelectXOrigin;			// Initial corner of the selection box
    int         SelectYOrigin;			// Initial corner of the selection box
    int         StateOfForm;			// Indicates if widgets are being moved/resized
    TStringList *WidgetPositionList;	// Sorted list of widget Y positions, for collision
    									//		detection, and to establish tab order top-down
    TPoint      PrevMovePoint;			// Mouse position when widgets started to be moved
    WidgetList  *Widgets;				// Master list of all input/output widgets on form
    TWidget     *ActiveWidget;			// The widget that was last clicked on
    TWidget		*WidgetToDelete;		// Since a widget shouldn't be deleted on its event
										//		handler, we have to flag it for deleltion, and
										//		let the parent take care of it.
    bool        Initialized;			// Widgets have yet been placed
    bool        Closed;					// User has closed the form, which is different from
										//		the form never having been open.
                                        
    __fastcall TWidgetForm(TComponent *Owner);
    __fastcall~TWidgetForm();
    void __fastcall ShowIfNotEmpty();
    void __fastcall Reset();
    TWidget *__fastcall AddWidget(AnsiString WidgetType, I2adl *Object, int WidgetIndex);
    void __fastcall BeginMove();
    void __fastcall EndMove();
    void __fastcall ChangeToMenuItemClick(TObject *Sender);
    static void GetWidgetOptions(bool IsInput, int **WidgetOptions, int *nCount);
    void UseCorrectWidgetCursor(TShiftState& Shift);
    TTabSheet *NewTabSheet(AnsiString Name);
};
#endif
