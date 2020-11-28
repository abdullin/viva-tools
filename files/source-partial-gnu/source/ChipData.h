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

#ifndef ChipDataH
#define ChipDataH

#include "globals.h"

class ChipData
{
public:
    char    *DataBuffer;
    int     Length;
    int     Error;

    ChipData(char *FileName);
    ~ChipData();
    char *GetDataBuffer();
    int GetLength();
    
    int GetError()
    {
        return Error;
    };
};

void ProgramPensa();
void ProgramNewStyleBoard(VivaSystem *BoardSystem, AnsiString BitFileName, int ChipSelect);

#endif
