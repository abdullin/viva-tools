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

#ifndef SystemEditorH
#define SystemEditorH

class SystemEditor
{
public:
    
    TTreeView     *SystemTreeView;
    VivaSystem    *EditSystem;
    VList         *SystemTypes;
    
    void __fastcall DisplayInTree(TTreeView *, bool);
    TTreeNode *__fastcall DisplayChildren(TTreeNode *, VList *, bool);
    bool __fastcall Move(VivaSystem *Object, VivaSystem *Destination);

    SystemEditor(VivaSystem *, TTreeView *);
    ~SystemEditor();
};


#endif
