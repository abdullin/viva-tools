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

int dseed = 40;

// return a random uniform number between 0 and 2^31-1
int IRand()
{
	__asm
    {
		push    edx
		mov     edx,33614
		mov     eax,dseed
		mul     edx
		add     edx,edx
		add     eax,edx
		sbb     edx,edx
		shr     eax,1
		sub     eax,edx
		mov     dseed,eax
		pop     edx
	}
    
	return dseed;
}


// return a random uniform number between 0 and N - 1
int IRand(int N)
{
    int		irand = IRand();

	__asm
    {
                mov     eax,irand
                add     eax,eax
		mov     edx,N
		mul     edx
                mov     irand,edx
	}

    return irand;
}


void RandomStart()
{
//  To get a repeatable sequence of random numbers, disable the following line:
//    dseed = time(NULL);

}



