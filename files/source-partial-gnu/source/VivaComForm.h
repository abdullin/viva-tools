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

#ifndef VivaCOMFormH
#define VivaCOMFormH

#include <Forms.hpp>
#include <OleCtrls.hpp>

#include "Globals.h"

class PACKAGE VOleControl : public TOleControl
{
public:
    TPopupMenu		*OleMenu;       // Menu on which we place commands available on the OLE
    								//		control
    TComObject		*OwningObject;  // Viva com object wrapper for this control
    TControlData	CData;			// Control data to be used by OLE in creation of this
    								//		ActiveX/OLE control.

    virtual __fastcall VOleControl(TComponent *AOwner);
    virtual __fastcall ~VOleControl();

    virtual void __fastcall InitControlData();

    void __fastcall OleMenuClick(TObject *Sender);
    virtual void __fastcall WndProc(TMessage &Message);
};


class TComForm : public TForm
{
__published:
    void __fastcall FormDragDrop(TObject *Sender, TObject *Source, int X, int Y);
    void __fastcall FormDragOver(TObject *Sender, TObject *Source, int X, int Y, TDragState
            State, bool &Accept);
    void __fastcall FormClick(TObject *Sender);
    void __fastcall FormClose(TObject *Sender, TCloseAction &Action);

public:
    VOleControl		*SelectedControl;	// The control that has focus; can be NULL
    bool            IsDesigning;        // In design mode (not run mode)
    bool            DeleteSelected;  	// Needed cuz the selected control can't delete itself.
    AnsiString      FilePath;        	// Wherefrom this form was loaded; default save target
    TStringList     *UsedObjectNames;	// Names already used for controls on this form.
    TComForm		*CopyOf;		 	// The design-time form from which a runtime form was
    									//		created
    bool			Modified;           // Form or any control thereon has been changed since
    									//		last compile

    __fastcall TComForm(TComponent *Owner);
    __fastcall ~TComForm();
    
    bool SaveToFile(bool AutoPrompt = false, bool SaveWithProject = false);
    void SaveToStream(TStream *Stream);
    bool LoadFromFile(AnsiString _FileName);
    void LoadFromStream(TStream *Stream);
    AnsiString GetFileName();

    __property AnsiString FileName = { read = GetFileName };
};


#endif
