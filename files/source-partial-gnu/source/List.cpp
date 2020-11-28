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

#include <vcl.h>
#pragma hdrstop

#include "ErrorTrap.h"
#include "Globals.h"
#include "List.h"


// Main contructor
//     _ObjectSize initializes ObjectSize.  There is no default for ObjectSize because it
//     so important in reducing HashTable collisions.
VList::VList(int _ObjectSize)
{
    FItems = NULL;
    FCount = 0;
    FCapacity = 0;
    CapacityExp = 0;
    HashTable = NULL;
    HashValid = false;
    HashCapacity = 0;
    HashCapacityExp = 0;
    HashMask = 0;
    ObjectSize = _ObjectSize;
}


// Destructor
VList::~VList()
{
    delete[] FItems;
    delete[] HashTable;
}


// Put the Item into the Index location of the Items array.
void VList::SetItem(int Index, void *Item)
{
    // Negative Index numbers are invalid.
    if (Index < 0)
    {
        ErrorTrap(117, strNull).IntegerValue = Index;

        return;
    }

    // Do we need to raise the Count/Capacity (load NULL entires).
    if (Index >= FCount)
        SetCount(Index + 1);

    // Update the Item in the Index location.
    FItems[Index] = Item;
    SetHash(Item, Index);
}


// Returns the largest Index number of all of the Items that share this Item's hash location.
//     If no Item has ever hashed to this Item's hash location, then -1 is returned.
int VList::GetHash(void *Item)
{
    // If this is the first use of the HashTable then it is time to build it.
    if (!HashValid)
    {
        // The following fixes an abort on an empty Hash table.
        if (FCount <= 0)
            return -1;

        BuildHashTable();
    }

    // HashMask performs quick power of 2 mod.
    int     Index = HashTable[((int) Item / ObjectSize) & HashMask];

    // Adjust for deleted/removed/packed items.
    if (Index >= FCount)
        return FCount - 1;
    else
        return Index;
}


// Put the Item's Index number into the HashTable if it is the largest.
//     Warning!  Calling routine must make sure Index is valid.
void VList::SetHash(void *Item, int Index)
{
    // HashMask performs quick power of 2 mod.
    if (HashValid)
        SetMax(HashTable[((int) Item / ObjectSize) & HashMask], Index);
}


// Sets the number of active entries in the Items array.
//     Increasing Count loads NULL entires and increases the Capacity if needed.
//     Reducing Count quickly removes entries off the end of the Items array.
void VList::SetCount(int NewCount)
{
    if (NewCount == 0)
    {
        // 99% of the time Count is just set to zero.
        FCount = NewCount;
    }
    else if (NewCount > FCount)
    {
        // Count is being increased.  Add NULL entries.
        do
        {
            Add(NULL);
        }
            while (NewCount > FCount);
    }
    else if (NewCount < 0)
    {
        // Do not allow the Count to be negative.
        ErrorTrap(119, strNull).IntegerValue = NewCount;
    }
    else
    {
        // Count is being reduced to a valid number.
        FCount = NewCount;
    }
}


// Rebuilds the Items array to the next power of 2 large enough to hold NewCapacity.
//     If NewCapacity is zero, then the capacity growth will be based on the size of the
//     current Capacity.
void VList::SetCapacity(int NewCapacity)
{
    // The next largest power of 2 is the first significant bit multiplied by 2.
    NewCapacity--;

    int     SiginificantBitCount = 0;

    while (NewCapacity != 0)
    {
        NewCapacity >>= 1;
        SiginificantBitCount++;
    }

    IncreaseCapacity(SiginificantBitCount);
}


// Rebuild the Items array to increase the maximum capacity.
//     If NewCapacityExp is zero, then the capacity growth will be based on the size of the
//     current Capacity.
void VList::IncreaseCapacity(int NewCapacityExp)
{
    // Calculate the new capacity power of 2.
    if (NewCapacityExp > 0)
    {
        if (NewCapacityExp <= CapacityExp)
            return;  // Already large enough.
    }
    else if (CapacityExp <= 2)
    {
        NewCapacityExp = 3;  // Initial allocation of 8 entries.
    }
    else if (CapacityExp <= 10) // MAX_CAPACITY_EXP / 3
    {
        NewCapacityExp = CapacityExp + 3;  // 8 times larger
    }
    else if (CapacityExp < 30) // MAX_CAPACITY_EXP - 2 (can't use bit 0 & 31)
    {
        NewCapacityExp = CapacityExp + 1;  // Twice as large
    }
    else
    {
        // Maximum capacity of one billion reached.
        ErrorTrap(118);

        return;
    }

    // Caculate the actual power of 2 capacity.
    FCapacity = 1 << NewCapacityExp;

    // Build the new larger Items array and copy any existing data into it.
    void    **NewItems = new void *[FCapacity];

    if (FItems != NULL)
    {
        memcpy(NewItems, FItems, FCount * sizeof(void *));

        delete[] FItems;
    }

    FItems = NewItems;
    CapacityExp = NewCapacityExp;
    HashValid = false;
}


// Build and load a new HashTable.
void VList::BuildHashTable()
{
    // Do we need to allocate more memory for the HashTable?
    if (HashTable == NULL || HashCapacity < FCapacity)
    {
        delete[] HashTable;

        HashCapacity = FCapacity;
        HashCapacityExp = CapacityExp;

        // Load HashMask with leading zeros and CapacityExp trailing ones.
        HashMask = HashCapacity - 1;
        HashTable = new int [HashCapacity];
    }

    // Initialize the HashTable to all -1s (invalid Index indicator).
    memset(HashTable, 0xff, sizeof(int) * HashCapacity);

    // Reload all index numbers.
    HashValid = true;

    for (int i = 0; i < FCount; i++)
        SetHash(FItems[i], i);
}


// Add the new Item onto the end of the Items array.
//     Returns the Index location where the item was added.
//     NULL entries are not recycled because the logic slowed down the SearchEngine by 33%.
int VList::Add(void *Item)
{
    // Do we need to increase the capacity?
    if (FCount >= FCapacity)
        IncreaseCapacity();

    SetHash(Item, FCount);
    FItems[FCount] = Item;

    // Because zero based, increment after returning the index.
    return FCount++;  
}


// Remove the Item from the Items array.
//     Returns the Index location where the item was deleted from (or -1 if not found).
//     All entries after the Item are slid up.
int VList::Remove(void *Item)
{
    int     Index = IndexOf(Item);

    if (Index >= 0)
        Delete(Index);

    return Index;
}


// Delete the Item in the Index location from the Items array.
//     All entries after the Index are slid up.
void VList::Delete(int Index)
{
    if (Index < 0 || Index >= FCount)
    {
        ErrorTrap(117, strNull).IntegerValue =  Index;

        return;
    }

    FCount--;

    // If the entry was not the last entry, then slide the later entries forward.
    //     Use memmove instead of memcpy because we are covering part of the same ground.
    if (Index < FCount)
        memmove(&FItems[Index], &FItems[Index + 1], (FCount - Index) * sizeof(void *));
}


// Removes all NULL entries from the Items array.
//     Count is updated to reflect the number of active entries.
void VList::Pack()
{
    // Find the first NULL entry in the Items array.  If none, then Pack doen't change anything.
    for (int FirstNullEntry = 0; FirstNullEntry < FCount; FirstNullEntry++)
    {
        if (FItems[FirstNullEntry] != NULL)
            continue;

        // We found a NULL entry.  Remove it and all other NULL entries by sliding the good
        //     entries forward.
        int     NULLCount = 1;

        for (int i = FirstNullEntry + 1; i < FCount; i++)
        {
            if (FItems[i] == NULL)
                NULLCount++;
            else
                FItems[i - NULLCount] = FItems[i];
        }

        FCount -= NULLCount;

        break;
    }
}


// Uses the HashTable to quickly return the Index location of the Item in the Items array.
//     If the Item is not found, then -1 is returned.
//     Speed is very important in this routine.
int VList::IndexOf(void *Item)
{
    int	Index = GetHash(Item);

    // Index is the largest Index of any Item with this hash value (look backwards).
    //     If no Item has ever had this hash value then Index will start out -1.
    while (Index >= 0)
    {
        if (FItems[Index] == Item)
            return Index;

        Index--;
    }

    return -1;
}


// Sort the entries in the Items array.
void VList::Sort(VListSortCompare Compare)
{
    qsort(FItems, FCount, sizeof(void *), Compare);
    
    HashValid = false;
}


// Insert the Item into the Index location of the Items array.
//     All entries after Index are slid down.
//     Returns the Index location where the Item was actually placed.
//     Warning!  Avoid using this method because it is slow and invalidates the HashTable.
int VList::Insert(int Index, void *Item)
{
    // If the Index is invalid, then inform the user.
    if (Index < 0 || Index > FCount)
    {
        ErrorTrap(117, strNull).IntegerValue = Index;

        Index = FCount;
    }

    // Are we inserting at the very end of the array?
    if (Index >= FCount)
        return Add(Item);

    if (FCount >= FCapacity)
        IncreaseCapacity();

    // Use memmove instead of memcpy because we are covering part of the same ground.
    memmove(&FItems[Index + 1], &FItems[Index], (FCount - Index) * sizeof(void *));

    FCount++;
    HashValid = false;
    FItems[Index] = Item;

    return Index;
}

