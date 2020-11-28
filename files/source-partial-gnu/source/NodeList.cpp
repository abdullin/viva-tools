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

#include "globals.h"
#include "ErrorTrap.h"
#include "DataSet.h"
#include "I2adl.h"
#include "Node.h"
#include "NodeList.h"
#include "MyTScrollingWinControl.h"

//  NullNodeList is created in Main.  It has a Count and Capacity of zero.
//  This is placed in all empty NodeLists so that the Count can be accessed
//     causing a a memory fault.
NodeList    *NullNodeList = NULL;


// Allocates a node list with DEFAULT_NODE_LIST_CAPACITY
//		To be used only for creating the NullNodeList
__fastcall NodeList::NodeList()
{
    Count = 0;
    Capacity = DEFAULT_NODE_LIST_CAPACITY;

    for (int j = 0; j < DEFAULT_NODE_LIST_CAPACITY; j++)
    {
        Items[j] = NULL;
    }
}


// Non-standard constructor
// Allocate a node list of arbitrary size.  Use when the size size is known.
NodeList *NewNodeList(int Count)
{
    if (Count == 0)
        return NullNodeList;

    int    Cap = Count;

    // This code prevents reallocation.
    // Use Cap for detecting how a NodeList was allocated.
    // To view NodeList with debugger, change == to <= in this equation
    //    This changes uses a lot more storage.
    if (Cap <= DEFAULT_NODE_LIST_CAPACITY)
        Cap = DEFAULT_NODE_LIST_CAPACITY + 1;

    int         size = sizeof(int) * (Cap - DEFAULT_NODE_LIST_CAPACITY) + sizeof(NodeList);
    char        *space = new char[size];
    NodeList    *Return = (NodeList *) space;

    for (int j = 0; j < Cap; j++)
        Return->Items[j] = NULL;

    // Return = new NodeList;  // Max of 20 nodes.
    Return->Count = 0;
    Return->Capacity = Cap;

    return Return;
}


// Dynamic AddNode.  Expands and reallocates the nodelist if the size requires it.
//		Pass the address of the node list.
void AddNode(NodeList **UserNodeList, Node *_Node)
{
    int    Index = 0;

    // Remove one level of indirection
    NodeList    *XList = *UserNodeList;

    while (true) // break when done reallocating
    {
        if (XList == NULL || XList->Capacity == 0)
        {
            XList = NewNodeList();
            *UserNodeList = XList;

            break;
        }

        Index = XList->Count;

        if (Index < XList->Capacity)
            break;    // We have enough room..put at end of list.

        // Increase the capacity by 10 or 5%.
        int		NodeExpansionSize = max(MIN_NODE_EXPANSION, XList->Capacity / 20);

        NodeList    *NewList = NewNodeList(XList->Capacity + NodeExpansionSize);

        *UserNodeList = XList;
        memcpy(&NewList->Items[0], &XList->Items[0], Index * 4);

        //for (int i=0;i<Index;i++)
        //    NewList->Items[i] = XList->Items[i];
        // Delete the old node list.  Use the appropriate destructor.
        //   Capacity of items created by standard constructor is always
        //      DEFAULT_NODE_LIST_CAPACITY
        //   Capacity of items created by non-standard constructor is never
        //      DEFAULT_NODE_LIST_CAPACITY
        if (XList->Capacity == DEFAULT_NODE_LIST_CAPACITY)
            delete XList;
        else
            delete[] (char *)XList;
        XList = NewList;
        break;
    }

    *UserNodeList = XList;
    XList->Count = Index + 1;
    XList->Items[Index] = _Node;
}


// Copy an existing node list
// The calling routine must fix node pointers because both sets of nodes are left connected.
NodeList *NewNodeList(NodeList *_IOPuts, I2adl *_Owner, int NewCount)
{
    int         Count = (NewCount == -1) ? _IOPuts->Count : NewCount;
    NodeList    *Return = NewNodeList(Count);
    
    for (int i = 0; i < Count; i++)
        Return->Add(new Node(_IOPuts->Items[i], _Owner));
    
    return Return;
}


// Call Add to insert a new object at the end of the Items array.
//		Add returns the index of the new item,
//		where the first item in the list has an index of 0.
//		Add WILL NOT allocate more memory if the Items array already uses up the
//		Capacity of the list object.
//		If the capacity is exceeded, then an error trap will result.
//		Add increases the value of Count to reflect the addition of a new pointer.
//		Note:  Add always inserts the Item pointer at position [Count],
//		even if the Items array contains NULL pointers.
void __fastcall NodeList::Add(Node *_Node)
{
    if (Count >= Capacity)
    {
        ErrorTrap(3011);

        return;
    }

    Items[Count] = _Node;
    Count++;
}


// Deletes the first instance of the Node parameter from the Items array.
//		After the item is removed, the list is packed.
void __fastcall NodeList::Remove(Node *_Node)
{
    int    NodeIndex = IndexOf(_Node);

    if (NodeIndex < 0)
    {
        ErrorTrap(3008);

        return;
    }

    // delete _Node;  // This code causes crashes
    Delete(NodeIndex);
}


// Removes the item at a specific position from the list.
//		The index is zero-based, so the first item has an Index value of 0,
//		the second item has an Index value of 1, and so on.  Moves up all items in
//		the Items array that follow the deleted item, and reduces the Count.
void __fastcall NodeList::Delete(int NodeIndex)
{
    // Was a valid NodeIndex passed?
    if (NodeIndex >= Count)
    {
        ErrorTrap(3012);
        return;
    }

    Count--;

    for (int j = NodeIndex; j < Count; j++)
        Items[j] = Items[j + 1];
}


// Returns the index of the entry in the Items array with a specified address
//		Call IndexOf to get the index for a pointer in the Items array.
//		Indexing is zero-based. If an item is not in the list, IndexOf returns -1.
int __fastcall NodeList::IndexOf(Node *_Node)
{
    for (int j = 0; j < Count; j++)
    {
        if (_Node == Items[j])
            return j;
    }

    return -1;
}


// ToDo: add error trapping.
void NodeList::Insert(NodeList **List, Node *node, int index)
{
    AddNode(List, node);

    for (int i = (*List)->Count - 1; i > index; i--)
        (*List)->Items[i] = (*List)->Items[i - 1];

    (*List)->Items[index] = node;
}


// Removes all non-NULL from the Items list by moving all non-NULL
//		items to the front of the Items array.  Reduces the Count
//		property to properly reflect the number of items actually present.
void __fastcall NodeList::Pack()
{
    int    jd = 0;

    for (int js = 0; js < Count; js++)
    {
        if ((Items[jd] = Items[js]) != NULL)
            jd++;
    }

    Count = jd;
}


// Disconects all nodes in this list
void __fastcall NodeList::Disconnect()
{
    for (int i = 0; i < Count; i++)
        Items[i]->Disconnect();
}


// Delete the nodes and set the count to zero.
void __fastcall NodeList::Clear()
{
    for (int i = 0; i < Count; i++)
    {
        delete Items[i];
        Items[i] = NULL;
    }

    Count = 0;
}


// Change the data set of nodes to the data set of the node passed.
//		Propagate the data set of the node passed.	Apply only to Input, Output, Junction and
//		Transport objects.  Called by I2adl::ChangeDataSets(Node *_Node).
void NodeList::ChangeDataSets(DataSet *_DSet)
{
    for (int i = 0; i < Count; i++)
        Items[i]->DSet = _DSet;
}


//  Non-standard destructor.  Performs the appropriate delete depending on the
//  flavor of the NodeList allocated
void DeleteNodeList(NodeList *_NodeList)
{
    if (_NodeList == NULL)
        return;

    if (_NodeList->Capacity == 0)  // Do not delete NullNodeList here!
        return;

    for (int i = 0; i < _NodeList->Count; i++)
        delete _NodeList->Items[i];

    if (_NodeList->Capacity == DEFAULT_NODE_LIST_CAPACITY)
        delete _NodeList;
    else
        delete[] (char *)_NodeList;
}


// Set owner & clear connections (left over from copy).
void __fastcall NodeList::SetOwner(I2adl *_Owner)
{
    for (int j = 0; j < Count; j++)
    {
        Items[j]->Owner = _Owner;
        Items[j]->ConnectedTo = NULL;
    }
}


// Returns true if any of the nodes in the list are connected.
bool NodeList::AnyAreConnected()
{
    for (int j = 0; j < Count; j++)
        if (Items[j]->ConnectedTo != NULL)
        	return true;

    return false;
}


// Returns true if the data set is used on any of the nodes.
bool NodeList::IsDataSetUsed(DataSet *DSet)
{
    for (int i = 0; i < Count; i++)
        if (Items[i]->DSet == DSet)
            return true;

    return false;
}


// Returns true if there is an "unstable" feedback loop to the original StartObject.
//     This routine operates on the output nodes of a regular (non-transport) object.
//     The IsVisitedKey should be incremented for each StartObject being checked.
bool NodeList::LoopsBackTo(I2adl *StartObject, int Depth)
{
	// The following fixes almost infinite search times on asynchronous FLOPS.
	if (Depth >= 20)
    	return false;

	// Check each of the output nodes in the list.
    for (int i = 0; i < Count; i++)
    {
        // Is this node connected to a transport?
    	Node	*TransportInput = Items[i]->ConnectedTo;

        if (TransportInput == NULL)
        	continue;

        // Check each of the Transport's output nodes.
        NodeList	*TransportOutputs = TransportInput->Owner->Outputs;

        for (int j = 0; j < TransportOutputs->Count; j++)
        {
        	// Is this node connected to a down stream object?
        	Node	*NextObjectInput = TransportOutputs->Items[j]->ConnectedTo;

            if (NextObjectInput == NULL)
            	continue;

            // If the downstream object is the original StartObject, then we found an
            //     "unstable" feedback loop.
            I2adl	*NextObject = NextObjectInput->Owner;

            if (NextObject == StartObject)
            	return true;

            // To prevent infinite searches on feedback loops, only check each object once.
            if (NextObject->FeedBackLoop == IsVisitedKey)
            	continue;

            NextObject->FeedBackLoop = IsVisitedKey;

            // Does the downstream object pass through unstable signals?
            if (NextObject->NameCode != NAME_AND &&
            	NextObject->NameCode != NAME_OR &&
            	NextObject->NameCode != NAME_INVERT &&
            	NextObject->NameCode != NAME_MUXCY &&
            	NextObject->NameCode != NAME_ADC &&
            	NextObject->NameCode != NAME_DELAY)
            {
            	// Single bit exposer, collector and $Cast objects pass through unstable
                //     signals.
                if (NextObject->ExposerCode == EXPOSER_NOT)
                	continue;

                int		TotalBits = 0;

                for (int k = 0; k < NextObject->Outputs->Count; k++)
					TotalBits += NextObject->Outputs->Items[k]->DSet->BitLength;

                if (TotalBits != 1)
	            	continue;
            }

            // Recursive call to check the objects downstream from this non-stabilizing object.
            //     If an "unstable" path is found, then we don't have to look any further.
            if (NextObject->Outputs->LoopsBackTo(StartObject, ++Depth))
				return true;
        }
    }

    return false;
}


// Find the width of the widest displayed node name.
double NodeList::NodeNameWidth()
{
    short	NameWidth = 0;

    for (int i = 0; i < Count; i++)
    {
        Node    *IONode = Items[i];

        // Skip control nodes not defined on behavior sheet and default node names.
        if (IONode->DSet != NULL  &&  IONode->ShowNodeName())
            NameWidth = max(NameWidth, IONode->NameWidth);
    }

    return NameWidth;
}
