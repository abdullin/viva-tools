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

#ifndef WidgetButtonH
#define WidgetButtonH

#include <Buttons.hpp>

#include "Widget.h"


class WidgetButton : public TWidget
{
public:
    TSpeedButton    *Button;	// Toggle button serving as main control

    WidgetButton(TComponent *_Owner, Node *_SourceNode);

    virtual void SendValue();
    virtual void SetValue(int Value);
};

#endif
