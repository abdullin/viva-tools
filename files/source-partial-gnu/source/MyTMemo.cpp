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
#include "MyTMemo.h"
#include "I2adl.h"
#include "MyTShape.h"
#include "MyTScrollingWinControl.h"

#pragma package(smart_init)


void MyTMemo::InitAttributes(I2adl *ActiveText)
{
    TFontStyles		Style = Font->Style;
    int				IStyle = *(unsigned char *) &Style;
    double			FontSizeAt5 = double(Font->Size) * OLD_GRID_SNAP / ViewScale;

    // Only update the font size if the user changed it.  Otherwise keep the current precision.
    if (Font->Size == ActiveText->CalculateFontSize() &&
    	ActiveText->Attributes->Count >= 4)
        	FontSizeAt5 = ActiveText->Attributes->Strings[1].ToDouble();

    ActiveText->Attributes->Clear();
    ActiveText->Attributes->Add(Font->Name);
    ActiveText->Attributes->Add(FontSizeAt5);
    ActiveText->Attributes->Add(Font->Color);
    ActiveText->Attributes->Add(IStyle);
}


MyTMemo::MyTMemo(TWinControl *AParent) : TMemo(AParent)
{
    Visible = false;
    WantTabs = true;
    BorderStyle = bsNone;
    PopupMenu = FontEditTrigger;
    Parent = AParent;

    // The following keeps the edges of the text box visable when the I2adl Editor background
    //     color is changed to pure white.  It it half way between the default background
    //     (0xebebeb) and pure white.
	Color = (TColor) 0xf5f5f5;
}


void __fastcall MyTMemo::ShowMemo(MyTShapeI2adl *Shape)
{
    if (Visible)
        HideMemo();

    // If this is a "<Tree Group>" text box, then it is read only.
    if (strncmp(Shape->OwningI2adl->AttSystem->c_str(), str_TreeGroup_.c_str(),
    	str_TreeGroup_.Length()) == 0)
	        return;

    ActiveTextI2adl = Shape->OwningI2adl;
    Shape->SetCanvasText();

    Initialize(Shape, Shape->Left + MEMO_OFFSET_LEFT, Shape->Top + MEMO_OFFSET_TOP);

    Visible = true;
    Shape->Visible = false;
    SetFocus();
    Application->OnMessage = EventHanldlerForTextI2adlMemo;
}


void __fastcall MyTMemo::HideMemo()
{
    I2adl			*ActiveText = ActiveTextI2adl;
    AnsiString		strTemp = strNull;

    // Remove trailing blanks and blank lines incase the user tried to exit via the Enter key.
    Text = TrimRight(Text);

    InitAttributes(ActiveText);

    if (Text == strNull)
    {
        ActiveText->AttSystem = SystemstrNull;
    	ActiveText->SetDefaultTextDimensions();
        Height = ActiveText->Height;
        Width = ActiveText->Width;
    }
	else
    {
        int		pos = 1;
        int		AttSystemLength = Text.Length();

        for (int i = 0; i < Lines->Count; i++)
        {
            int		len = Lines->Strings[i].Length();

            strTemp += Text.SubString(pos, len);

            pos += Lines->Strings[i].Length();
            
            if (pos + 1 <= AttSystemLength && Text.SubString(pos, 2) == strCRLF
                && i < Lines->Count - 1)
            {
                strTemp += strCRLF;
                pos += 2;
            }
            else // only if not the last line
            {
                strTemp += ((char) 1);
            }
        }

		// AttSystem is where we store the text, cuz we're too "conservative" to make another
        //		member of I2adl.
        ActiveText->AttSystem = SystemStringTable->QString(strTemp);

        AutosizeVertical(RESIZE_SHRINK_GROW);

        TStringList		*OldLines = new TStringList;

        OldLines->Assign(Lines);
        AutosizeHorizontal(OldLines);
        delete OldLines;
    }

    MyTShapeI2adl	*Shape = ActiveText->I2adlShape;

    Shape->Left = Left - MEMO_OFFSET_LEFT;
    Shape->Top = Top - MEMO_OFFSET_TOP;
    Shape->Width = Width + MEMO_OFFSET_LEFT * 2;
    Shape->Height = Height + (MEMO_OFFSET_TOP * 2);
    Shape->GetCanvas()->Font->Assign(Font);

    Shape->Visible = true;
    Visible = false;
    Application->OnMessage = NULL;

    ActiveTextI2adl = NULL;
}


void __fastcall MyTMemo::Initialize(MyTShapeI2adl *Shape, int x, int y)
{
    Left = x;
    Top = y;

    Lines->Clear();

    if (!FontsAreSame(Shape->GetCanvas()->Font))
    {
        WordWrap = false;
        Font->Assign(Shape->GetCanvas()->Font);
        WordWrap = true;
        Visible = true;
        Visible = false;
    }

    Text = ReplaceAll(*Shape->OwningI2adl->AttSystem, (char) 1, strCRLF);

    static bool		bInitialized = false;

    if (!bInitialized)
    {
        Visible = true;
        Visible = false;
        bInitialized = true;
    }

    I2adl			*FormerActiveTextI2adl = ActiveTextI2adl;
    TStringList		*TempList = new TStringList;

    ActiveTextI2adl = Shape->OwningI2adl;

    TempList->Text = Text;

    if (Text > strNull)
    {
        AutosizeVertical(RESIZE_SHRINK_GROW);
        AutosizeHorizontal(TempList);
        AutosizeVertical(RESIZE_SHRINK_GROW);
    }
    else
    {
        Width = Shape->Width - MEMO_OFFSET_LEFT * 2;
        Height = Shape->Height - (MEMO_OFFSET_TOP);
    }

    ActiveTextI2adl = FormerActiveTextI2adl;

    Text = ReplaceAll(*Shape->OwningI2adl->AttSystem, (char) 1, "");

    delete TempList;
}


void __fastcall MyTMemo::AutosizeVertical(int ResizeAllowed)
{
    if (!ActiveTextI2adl) 
        return;

	int		TextHeight = (ActiveTextI2adl->I2adlShape->GetCanvas()->TextHeight(Text))
		* Lines->Count + 2;

    if (Showing)
        if (Text.SubString(Text.Length() - 1, 2) == strCRLF)
            TextHeight += ActiveTextI2adl->I2adlShape->GetCanvas()->TextHeight(Text);

    SetMax(TextHeight, MIN_MEMO_HEIGHT);

    if (Height < TextHeight)
    {
        if (ResizeAllowed & RESIZE_GROW)
            Height = TextHeight;
    }
    else if (ResizeAllowed & RESIZE_SHRINK)
    {
        Height = TextHeight;
    }
}


// Shrinks TextI2adlMemo horizontally to fit the text.
void __fastcall MyTMemo::AutosizeHorizontal(TStringList *OldLines)
{
    int				MaxLength = 0;
    const int		Inc = 2;

    for(int i = 0; i < OldLines->Count; i++)
    {
        int TextWidth = ActiveTextI2adl->I2adlShape->GetCanvas()
            ->TextWidth(OldLines->Strings[i]);
        if (TextWidth > MaxLength)
            MaxLength = TextWidth;
    }

    if (Width != MaxLength)
        Width = MaxLength;

    while (Lines->Count < OldLines->Count && Width > 0)
    {
        Width -= Inc;
    }

    // Grow the memo until it lines match the old line state:
    // WordWrap must be true in order for this procedure to work:
    while(!Lines->Equals(OldLines) && Lines->Count >= OldLines->Count)
    {
        Width += Inc;
    }

    if (Width < MIN_MEMO_WIDTH)
        Width = MIN_MEMO_WIDTH;
    else
        Width += MEMO_OFFSET_LEFT;
}


bool MyTMemo::FontsAreSame(TFont *Font2)
{
    if (Font->Size != Font2->Size)         return false;
    if (Font->Color != Font2->Color)       return false;
    if (Font->Name != Font2->Name)         return false;
    if (Font->Style != Font2->Style)       return false;

    return true;
}

//                           *Event Handlers*
void __fastcall MyTMemo::EventHanldlerForTextI2adlMemo(tagMSG &Msg, bool &Handled)
{
    if (Msg.message == WM_LBUTTONDOWN || Msg.message == WM_RBUTTONDOWN)
        if (FindVCLWindow(Mouse->CursorPos) != TextI2adlMemo)
            TextI2adlMemo->HideMemo();
}


void __fastcall MyTMemo::MouseMove(Classes::TShiftState Shift, int X, int Y)
{
    int		Resized = DoResize((TControl *)this, Shift, RESIZE_HORIZONTAL, MIN_MEMO_WIDTH);

    if (Resized & RESIZE_HORIZONTAL)
        AutosizeVertical(RESIZE_SHRINK_GROW);
}


void __fastcall MyTMemo::Change()
{
    if (!Visible)
        return;
        
    AutosizeVertical(RESIZE_SHRINK_GROW);
}


void __fastcall MyTMemo::KeyPress(char &Key)
{
    if (Key == VK_ESCAPE)
        HideMemo();

    // Tabs do not print properly on TGraphicControl canvas; replace with spaces during edit:
    if (Key == VK_TAB)
    {
        // First off, delete anything that is selected
        int		OldSelStart = SelStart;

        Text = Text.Delete(SelStart + 1, SelLength);
        SelStart = OldSelStart;

        // Need to append a number of spaces sufficent to get us to the next tab stop (8)
        int		nSpacesToAppend = 8 - (SelStart % 8);
        int		NewPos = SelStart + nSpacesToAppend;

        AnsiString strSpaces = "";

        for (; nSpacesToAppend > 0; nSpacesToAppend--)
            strSpaces += strSpace;

        Text = Text.SubString(1, SelStart) + strSpaces + Text.SubString(SelStart + 1
            , Text.Length() - SelStart);
        SelStart = NewPos;
        Key = 0;
    }
}


void __fastcall MyTMemo::KeyDown(Word &Key, Classes::TShiftState Shift)
{
    if (Key == 'A' && Shift.Contains(ssCtrl))
        SelectAll();
}


