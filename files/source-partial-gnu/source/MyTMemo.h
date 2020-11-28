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

#ifndef MyTMemoH
#define MyTMemoH

// Multi-line text control for edit mode of Text I2adl objects.
class MyTMemo : public TMemo
{
public:
    MyTMemo(TWinControl *AParent);
    void __fastcall Initialize(MyTShapeI2adl *Shape, int x, int y);
    void __fastcall AutosizeHorizontal(TStringList *OldLines);
    void __fastcall AutosizeVertical(int ResizeAllowed);
    void __fastcall ShowMemo(MyTShapeI2adl *Shape);
    void __fastcall HideMemo();
    void __fastcall EventHanldlerForTextI2adlMemo(tagMSG &Msg, bool &Handled);
    void InitAttributes(I2adl *ActiveText);
	bool FontsAreSame(TFont *Font2);

__published:
    DYNAMIC void __fastcall MouseMove(Classes::TShiftState Shift, int X, int Y);
    DYNAMIC void __fastcall Change();
    DYNAMIC void __fastcall KeyPress(char &Key);
    DYNAMIC void __fastcall KeyDown(Word &Key, Classes::TShiftState Shift);
};

#endif
