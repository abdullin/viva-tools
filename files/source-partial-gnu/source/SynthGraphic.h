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

#ifndef SynthGraphicH
#define SynthGraphicH

#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <ExtCtrls.hpp>

class TSynthGraphicDisplay;

extern TSynthGraphicDisplay    *SynthGraphicDisplay;
extern bool                    LastGraphicVisible;
extern int                     GlyphSize;
extern int                     DisplayCount;


class TSynthGraphicDisplay : public TForm
{
__published:
    TPanel        *Panel1;
    TScrollBar    *ZoomScale;
    TButton       *CancelSynth;
    TButton       *HideSynthGraphic;
    
    void __fastcall CancelSynthClick(TObject *Sender);
    void __fastcall HideSynthGraphicClick(TObject *Sender);
    void __fastcall FormPaint(TObject *Sender);
    void __fastcall Resizer(TObject *Sender);
    void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
    void __fastcall FormResize(TObject *Sender);
    void __fastcall FormKeyDown(TObject *Sender, WORD &Key, TShiftState Shift);
    void __fastcall ZoomScaleChange(TObject *Sender);

public:
    int          LastCount;			//
    I2adlList    *DisplayList;		// List of I2adl objects to be displayed
    int          QWidth;			// SynthGraphicsDisplay row size
    int          OldYMax;			//
    int          XMargin;			//
    int          YMargin;			//
    bool         GraphicVisible;	// True if the graphic is being displayed
    int          DisplayIndex;		// Used for placement

    virtual __fastcall TSynthGraphicDisplay(TComponent *Owner, I2adlList *_DisplayList);

    void __fastcall PlacePixels(int X, int Y, TColor NodeColor, int Sizex, int Sizey = 0);
    void UpdateGraphic();
    void __fastcall DisplayObject(I2adl *ViewObject, int State);
    void Placement();
};

#endif
