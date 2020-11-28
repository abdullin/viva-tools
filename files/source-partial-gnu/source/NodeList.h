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

#include "Globals.h"

#ifndef NodeListH
#define NodeListH

#define DEFAULT_NODE_LIST_CAPACITY 10
#define MIN_NODE_EXPANSION 10

class NodeList
{
public:

    int     Count;								// Number of active nodes in the NodeList.
    int     Capacity;   						// Capacity of the node list; can grow.
    Node    *Items[DEFAULT_NODE_LIST_CAPACITY]; // Array of node pointers

    __fastcall NodeList();

    // The following calls are compatible with standard TList calls
	void __fastcall Add(Node *_Node);
	int __fastcall IndexOf(Node *_Node);
	void __fastcall Remove(Node *_Node);
	void __fastcall Delete(int NodeIndex);
	static void Insert(NodeList **List, Node *node, int index);
    void __fastcall Pack();

    void __fastcall Disconnect();
    void __fastcall Clear();
    void __fastcall SetOwner(I2adl *_Owner);
    void ChangeDataSets(DataSet *_DSet);
    bool AnyAreConnected();
    bool IsDataSetUsed(DataSet *DSet);
    bool LoopsBackTo(I2adl *StartObject, int Depth = 0);
    double NodeNameWidth();
};

extern NodeList		*NullNodeList;	// Created in Main.  It has a Count and Capacity of zero.
								  	// 		This is placed in all empty NodeLists so that the
                                    // 		count can always be accessed (no test for NULL).

// Non-standard constructors.
NodeList *NewNodeList(int Count = DEFAULT_NODE_LIST_CAPACITY + 1);
NodeList *NewNodeList(NodeList *_IOPuts, I2adl *_Owner, int NewCount = -1);

void DeleteNodeList(NodeList *_NodeList);
void AddNode(NodeList **UserNodeList, Node *_Node);

#endif
