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

#ifndef NodeH
#define NodeH

#include "Globals.h"

enum NodeTypeCodes
{
    TYPE_OUTPUT = 1,
    TYPE_INPUT
};


class Node
{
protected:
    int                 m_Left;			// Internal graphical Left property
    int                 m_Top;			// Internal graphical Top property
    StringEntry			*NameEntry;		// Entry in NodeStringTable for name and name width

    int GetLeft();
    int GetTop();
    void SetLeft(int _Left);
    void SetTop(int _Top);

public:
    I2adl               *Owner;			// Parent I2adl object that the node is on
    DataSet             *DSet;			// Data Set processed by the node
    Node                *ConnectedTo;	// Node connection pointer (conections are 1-to-1)
    unsigned char       *DataPtr;		// Address of DS value in the RunImage
    bool            	HasConElem;		// Already has an associated ConElem.

    union
    {
        int				InfoRate;		// Obsolete attempt at storage if information rate
        int				HasEvent;		// There is already an event queued in the executor
    };

    Node(const AnsiString &_Name, I2adl *_Owner, DataSet *_DSet, int _X = 0, int _Y = 0);
    Node(Node *_Node, I2adl *_Owner);
    ~Node();

    int GetSize();
    int GetType();
    int GetCompositeIndex();
    int GetNodeIndex(int NodeType);
    int GetWidgetValue();
    void SetName(const AnsiString &_Name);
    short GetNameWidth()
    {
    	return short(NameEntry->Object);
    };
    AnsiString *GetName()
    {
    	return &NameEntry->Name;
    };
    bool IsOnAtomicIO(bool IncludeTransports = false);
    bool IsCompatible(Node *NodeToConnect);
    bool IsOnVariantIO();
    bool IsControlNode();
    void Connect(Node *_ConnectedTo);
    Node *InsertExposerCollector(Node *NodeToConnect);
    void CreateTransportAndConnect(Node *NodeToConnect);
    void Disconnect(bool FixSelectDataSets = false);
    void __fastcall PropagateDataSet(DataSet *_DataSet);
    bool ShowNodeName();
    void SheetCoordinates(POINT &SheetPoint);
    void Paint();
    void MouseDown(TMouseButton Button, int X, int Y);
    void MouseMove(Classes::TShiftState Shift, int X, int Y);
    void DblClick();
    void RestoreOriginalDataSet();
    Node *SignalSource();
    I2adl *CreateBitFanObject(DataSet *SourceDataSet, bool UpdateDataSet = false, bool
    	UpdateAssignedBehavior = false);
    Node *BitFanSource();

    __property int Left = {read = GetLeft, write = SetLeft};
    __property int Top = {read = GetTop, write = SetTop};
    __property short NameWidth = {read = GetNameWidth};
    __property AnsiString *Name = {read = GetName};
};


#endif
