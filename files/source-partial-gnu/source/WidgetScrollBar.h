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

#ifndef WidgetScrollBarH
#define WidgetScrollBarH


class WidgetScrollBar : public TWidget
{
public:
    TScrollBar		*ScrollBar;		// "Main" control
    VWidgetLabel	*MinLabel;		// States minimum input/output value (WidgetMin attribute)
    VWidgetLabel	*ValLabel;		// Inicates current input/output value
    VWidgetLabel	*MaxLabel;		// States maximum input/output value (WidgetMax attribute)

    WidgetScrollBar(TComponent *_Owner, Node *_SourceNode);
    
    virtual void SendValue();
    virtual void SetValue(int Value);
    void UpdateDisplay();
    void OnWidgetResize(bool ChangeHeight = true);
    void SetMinMaxValues(TStrings *Attributes);
    DYNAMIC __fastcall void OnScroll(System::TObject* Sender, TScrollCode ScrollCode, int
    	&ScrollPos);
};

#endif
