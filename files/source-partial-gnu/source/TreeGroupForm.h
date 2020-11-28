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

#ifndef TreeGroupFormH
#define TreeGroupFormH

#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Buttons.hpp>

class TTreeGroupDialog : public TForm
{
__published:    // IDE-managed Components
    
    TBitBtn		*BitBtn1;
    TBitBtn		*BitBtn2;
	TComboBox	*cmbTreeGroup;
    TLabel		*TreeGroupLabel;

public:    // User declarations
    
    __fastcall TTreeGroupDialog(TComponent *Owner);
};

extern PACKAGE TTreeGroupDialog    *TreeGroupDialog;

#endif
