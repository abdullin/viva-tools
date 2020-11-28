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

#ifndef AboutH
#define AboutH

#include <Buttons.hpp>
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <ExtCtrls.hpp>
#include <Graphics.hpp>

class TAboutDlg : public TForm
{
__published:
    TImage	*Image1;
    TLabel	*Label1;
    TLabel	*Label2;
    TLabel	*Label3;
	TLabel	*Label4;
    
    void __fastcall Image1Click(TObject *Sender);
    void __fastcall FormCreate(TObject *Sender);

public:
    __fastcall TAboutDlg(TComponent *Owner);
};

extern PACKAGE TAboutDlg    *AboutDlg;

#endif
