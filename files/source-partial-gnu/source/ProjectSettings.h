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

#ifndef ProjectSettingsH
#define ProjectSettingsH

#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include "CSPIN.h"
#include <ComCtrls.hpp>
#include <ExtCtrls.hpp>

class TProjectSettingsDialog : public TForm
{
__published:
    TButton			*btnOK;
    TButton			*btnCancel;
    TPageControl	*PageControl;
    TTabSheet		*tsAttributes;
	TMemo			*mmoBaseSystemAttributes;
	TMemo			*mmoProjectAttributes;
	TLabel			*lblProjectAttributes;
	TLabel			*lblBaseSystemAttributes;

    void __fastcall btnOKClick(TObject *Sender);
	void __fastcall FormActivate(TObject *Sender);

public:
    __fastcall TProjectSettingsDialog(TComponent *Owner);
    __fastcall ~TProjectSettingsDialog();
};

extern PACKAGE TProjectSettingsDialog *ProjectSettingsDialog;

#endif
