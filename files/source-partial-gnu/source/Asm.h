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

#ifndef AsmH
#define AsmH

#include "ChipData.h"


extern void __fastcall    outportb(WORD wPort, BYTE data);
extern void __fastcall    outport(WORD wPort, WORD data);
extern WORD __fastcall    inport(WORD wPort);

int IRand();

int IRand(int N);

void RandomStart();

class program
{
private:
    ChipData	*ChipDataObjects[10];
    WORD		ArrayMask;
    WORD		ArrayChipsPort;
    WORD		ProgChipPort;

    void InitializeProg();
    void InitializeArray(WORD, WORD);
    WORD ProgramArray();
    WORD CreateProgramWord();
    long CreateProgramMask();
    bool ConfigureProg(int);
    void __fastcall DoProgProg(WORD wBasePort, char *start1, long length);
    void __fastcall DoProgVideo(WORD wBasePort, char *start1, long length);
    void __fastcall DoProgAce2(WORD wBasePort, char *start1, char *start2, long length);

public:
    program() {};
    ~program();

    void SetPorts(WORD, WORD);
    WORD DoConfigure(WORD);
    WORD SetFormat(int, char *);
    DWORD __fastcall delay(DWORD loop);
    void __fastcall X2InitChips(WORD wBasePort);
    void __fastcall ProgX2(WORD wBasePort, char far *start1, TProgressBar *, long);
    void __fastcall ReProgX2(WORD wBasePort, char far *start1, TProgressBar *, long);
};

#endif
