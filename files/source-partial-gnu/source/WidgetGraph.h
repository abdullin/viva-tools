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

#ifndef WidgetGraphH
#define WidgetGraphH


class WidgetGraph : public TWidget
{
private:
    int		XPosition;		// What column to paint next
    TColor	GraphColor;		// Red or dark red, toggling thru each wraparound
    TShape	*Graph;			// Shape on which graph data is plotted

public:
    WidgetGraph(TComponent *Owner, Node *_SourceNode);

    virtual void SetValue(int Value);
};

#endif
