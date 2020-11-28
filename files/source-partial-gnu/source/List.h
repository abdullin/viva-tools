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

#ifndef VListH
#define VListH

#define MAX_CAPACITY_EXP  32

typedef int (*VListSortCompare) (const void *Item1, const void *Item2);


class VList
{
    void    **FItems;          	// Main array of Item pointers--see Borland TList help
    int     FCapacity;        	// Capacity of the Items array--always a power of 2
    int     CapacityExp;      	// Exponent of the Capacity power of two
    int     *HashTable;       	// Hash table array for fast lookup of Item index numbers
                                //     Collisions record the largest index number
    bool    HashValid;        	// False means HashTable must be rebuilt before being used
    int     HashCapacity;     	// Capacity of the HashTable array--always a power of 2
    int     HashCapacityExp;  	// Exponent of the HashCapacity power of two
    int     HashMask;         	// Binary mask for quick power of 2 mod in hash algorithm
    int     ObjectSize;       	// Used to reduce collisions in the hash algorithm

    // Returns the Item in the Index location of the Items array (or NULL if none).
    inline void *GetItem(int Index)
    {
        // If the Index is invalid, then return NULL.
        //     User is not informed of invalid index numbers to be compatable with Borland's TList.
        if (Index < 0 || Index >= FCount)
            return NULL;
        else
            return FItems[Index];
    };

    void SetItem(int Index, void *Item);
    int GetHash(void *Item);
    void SetHash(void *Item, int Index);
    void SetCount(int NewCount);
    void SetCapacity(int NewCapacity);
    void IncreaseCapacity(int NewCapacityExp = 0);
    void BuildHashTable();

protected:
    int     FCount;            	// Number of active entries in the Items array

public:
    VList(int _ObjectSize);
    ~VList();

    virtual int Add(void *Item);
    virtual int Remove(void *Item);
    virtual void Delete(int Index);
    virtual int Insert(int Index, void *Item);
    void Pack();
    int IndexOf(void *Item);
    void Sort(VListSortCompare Compare);

    __property void *Items[int Index] = { read = GetItem, write = SetItem };
    __property int Count = { read = FCount, write = SetCount };
    __property int Capacity = { read = FCapacity, write = SetCapacity };
};

#endif
