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

#ifndef VivaIconH
#define VivaIconH

#define TBitMap Graphics::TBitmap

class VivaIcon : public StringEntry
{
public:

    TBitMap     *Bitmap;			// Bitmap for an I2adl shape.
    TBitMap     *SelectedBitmap;	// Bitmap for an I2adl shape, tinted by selection color
    int		   	OrigWidth;			// The width that we're supposed to be; likes to change

    __fastcall VivaIcon(const AnsiString &Name);
    __fastcall ~VivaIcon();

    void ClearIcon();
    void LoadIcon();
    void CreateSelectedIcon();
    bool NonTransparentEdge(int X);
};


class TI2adlStrTable : public StringTable
{
public:
    TI2adlStrTable();
    void __fastcall BeforeDestruction();

    StringEntry *NewEntry(const AnsiString &UString);

    // Locate the entry in the icon table.  If it doesn't exist, create it.
    inline VivaIcon *IconEntry(const AnsiString &ObjectName, bool FollowIndirection = false)
    {
        VivaIcon    *entry = (VivaIcon *) Entry(ObjectName);

        if (FollowIndirection)
            while (entry->Bitmap == NULL && entry->SelectedBitmap != NULL)
                entry = (VivaIcon *) entry->SelectedBitmap;

        return entry;
    }
};

#endif
