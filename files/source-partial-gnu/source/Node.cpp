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

#include "Globals.h"
#include "AttributeEditor.h"
#include "VivaIcon.h"
#include "I2adl.h"
#include "DataSet.h"
#include "ErrorTrap.h"
#include "I2adlEditor.h"
#include "MyTScrollingWinControl.h"
#include "MyTShape.h"
#include "Node.h"
#include "SearchEngine.h"
#include "VivaSystem.h"

#pragma package(smart_init)


// Constructor - Main
Node::Node(const AnsiString &_Name, I2adl *_Owner, DataSet *_DSet, int _X, int _Y)
{
    SetName(_Name);
    Owner = _Owner;
    DSet = _DSet;
    ConnectedTo = NULL;
    DataPtr = NULL;
    InfoRate = MAX_INFO_RATE;
    HasConElem = false;

	Left = _X;
	Top = _Y;
}


// Constructor - Copy
//     Note:  If the OldNode is ConnectedTo another node, then the calling routine
//     must do one of the following:
//     1.  Disconnect the NewNode.
//     2.  Disconnect the OldNode and reconnect the NewNode.
Node::Node(Node *_Node, I2adl *_Owner)
{
    *this = *_Node;
    Owner = _Owner;

    SetName(*Name);
}


// Destructor - Will disconect the node.  Calling Routine must remove it from the Owner's
//     source/sink list.
Node::~Node()
{
    // Calling Routine must remove node from the Owner's source/sink list.
    Disconnect();
}


// Calculate the Node's graphic size.
int Node::GetSize()
{
	// The following NULL test is required for CodeGuard to validate "this" pointers. 
	if (Owner == NULL)
		return ViewScale;
    else
	    return Owner->NodeSize();
}

// Get (calculate) the node's type (returns TYPE_OUTPUT or TYPE_INPUT).
int Node::GetType()
{
    return (Owner->Outputs->IndexOf(this) >= 0)
    ?  TYPE_OUTPUT
    :  TYPE_INPUT;
}


// Calculate the node's composite index.
int Node::GetCompositeIndex()
{
    int    Index = Owner->Inputs->IndexOf(this);

    if (Index != -1)
        return Index;
    else
        return Owner->Inputs->Count + Owner->Outputs->IndexOf(this);
}


// Returns the index number Insert the parent's input/output list (or -1 if not found).
int Node::GetNodeIndex(int NodeType)
{
    if (NodeType == TYPE_INPUT)
        return Owner->Inputs->IndexOf(this);

    if (NodeType == TYPE_OUTPUT)
        return Owner->Outputs->IndexOf(this);

    ErrorTrap(26, "Node::GetNodeIndex").IntegerValue =  NodeType; // Invalid node type.

    return -1;
}


// Returns the value of the widget formated for Widget::SetValue().
int Node::GetWidgetValue()
{
	if (DataPtr == NULL)
    	return 0;

    int				bitlength = DSet->BitLength;
    unsigned int	*intdataptr = (unsigned int *) DataPtr;

    // To do:  Extend this code to sizes > 4 bytes.
    if (bitlength >= 32)
        return *intdataptr;

    unsigned int	mask = (1 << bitlength) - 1;
    unsigned int	widgetvalue = *intdataptr & mask;

    if (DSet->AttributeFlags & (ATT_SIGNED | ATT_FIXED))
		if (widgetvalue & (1 << (bitlength - 1)))
            widgetvalue |= (~mask);

    return widgetvalue;
}


// Set the node's name and name code (based on the node's name).
//     This method should always be used to change the name.
void Node::SetName(const AnsiString &_Name)
{
	StringEntry		*entry = NodeStringTable->Entry(_Name);

    NameEntry = entry;
}


// Returns true if the node is on a system atomic (primitive) Input/Output object or
//     an unattached junction.  If IncludeTransports == true, then will return true on all
//     transports and junctions.
bool Node::IsOnAtomicIO(bool IncludeTransports)
{
    if (!Owner->IsPrimitive)
        return false;
    
    if (Owner->NameCode == NAME_INPUT)
        return true;
    
    if (Owner->NameCode == NAME_OUTPUT)
        return true;
    
    if (Owner->NameCode == NAME_TRANSPORT)
        return IncludeTransports;
    
    // Junctions that are not attached to any transports can have their nodes changed.
    if (Owner->NameCode == NAME_JUNCTION)
    {
        if (IncludeTransports)
            return true;

        for (int i = 0; i < Owner->Inputs->Count; i++)
            if (Owner->Inputs->Items[i]->ConnectedTo != NULL)
                return false;

        for (int i = 0; i < Owner->Outputs->Count; i++)
            if (Owner->Outputs->Items[i]->ConnectedTo != NULL)
                return false;

        return true;
    }

    return false;
}


// Returns true if node data sets are, or can be changed to be, compatible.
bool Node::IsCompatible(Node *NodeToConnect)
{
    Node		*UpstreamNodeToConnect = GetUpOrDownStreamNode(NodeToConnect);
    Node		*UpstreamOwningNode = GetUpOrDownStreamNode(this, false);
    NameCodes	NameCodeFrom = UpstreamOwningNode->Owner->NameCode;
    NameCodes	NameCodeTo = UpstreamNodeToConnect->Owner->NameCode;
	I2adl		*obj1;
    I2adl		*obj2;

    obj1 = Owner;
    obj2 = NodeToConnect->Owner;

	bool 	IsXPortOrJunction =
    	(obj1->NameCode == NAME_TRANSPORT || obj1->NameCode == NAME_JUNCTION) &&
    	(obj2->NameCode == NAME_TRANSPORT || obj2->NameCode == NAME_JUNCTION);

    if (obj1 == obj2 && IsXPortOrJunction)
    	return false;

    obj2 = GetUpOrDownStreamNode(NodeToConnect, false)->Owner;

    if (obj1 == obj2 && IsXPortOrJunction)
    	return false;

    obj2 = GetUpOrDownStreamNode(NodeToConnect, true)->Owner;

    if (obj1 == obj2 && IsXPortOrJunction)
    	return false;

    obj1 = NodeToConnect->Owner;
    obj2 = GetUpOrDownStreamNode(this, false)->Owner;

    if (obj1 == obj2 && IsXPortOrJunction)
    	return false;

    obj2 = GetUpOrDownStreamNode(this, true)->Owner;

    if (obj1 == obj2 && IsXPortOrJunction)
    	return false;

    // These variables are used to check both nodes both ways.
    Node    *Node1 = this;
    Node    *Node2 = UpstreamNodeToConnect;

    // The node must be compatible with ALL the nodes to which it eventually hooks;
    //      call this function recursively for all junctions:
    if (UpstreamNodeToConnect->Owner->NameCode == NAME_JUNCTION)
    {
        if (UpstreamOwningNode->GetType() == TYPE_OUTPUT)
        {
            NodeList *Outputs = UpstreamNodeToConnect->Owner->Outputs;

            for (int i = 0; i < Outputs->Count; i++)
            {
                Node *ConnectedTo = Outputs->Items[i]->ConnectedTo;

                if (ConnectedTo != NULL && !UpstreamOwningNode->IsCompatible(ConnectedTo))
                    return false;
            }

            return true;
        }
    }

    // The datasets of a transport are always overriden, so anything can hook up to them:
    if (NameCodeTo == NAME_TRANSPORT)
        return true;

    if (NameCodeFrom == NAME_DEREF)
    {
        if (UpstreamOwningNode == UpstreamOwningNode->Owner->Inputs->Items[1])
            return (UpstreamNodeToConnect->DSet->PointsToDS != NULL);
    }

    if (NameCodeTo == NAME_DEREF)
    {
        if (UpstreamNodeToConnect == UpstreamNodeToConnect->Owner->Inputs->Items[1])
            return (DSet->PointsToDS != NULL);
    }

    if (UpstreamNodeToConnect->DSet->VarType != VT_NULL &&
            UpstreamNodeToConnect->DSet->VarType == UpstreamOwningNode->DSet->VarType)
        return true;

    if (UpstreamNodeToConnect->DSet == NULLDataSet
    	&& UpstreamNodeToConnect->GetType() == TYPE_OUTPUT)
    	return true;

    if (UpstreamOwningNode->DSet == NULLDataSet
    	&& UpstreamOwningNode->GetType() == TYPE_OUTPUT)
    	return true;        

    if (DSet == NodeToConnect->DSet)
        return true;

    if (UpstreamOwningNode->DSet == UpstreamNodeToConnect->DSet)
        return true;

    // These variables are used to check both nodes both ways.
    Node1 = UpstreamOwningNode;
    Node2 = UpstreamNodeToConnect;

    for (int i = 0; i < 2; i ++)
    {
        // Any COM type can plug into a ComVariant on a Com I2adl:
        if (Node1->DSet == SGDataSets[sdsComVariant] &&
            Node1->GetType() == TYPE_INPUT &&
            Node1->Owner->ExecuteType == EXECUTE_COM &&
            Node2->DSet->VarType != VT_NULL)
        return true;

        Node1 = UpstreamNodeToConnect;
        Node2 = UpstreamOwningNode;
    }

    // Can always hook to a "Ref" object's main input.  Can only hook to its main output if the
    //      output's Dataset points to another Dataset.
    if (NameCodeFrom == NAME_REF)
    {
        if (UpstreamOwningNode == UpstreamOwningNode->Owner->Inputs->Items[1])
            return true;
        else if (UpstreamOwningNode == UpstreamOwningNode->Owner->Outputs->Items[1]
                 && UpstreamOwningNode->DSet->PointsToDS == NULL)
            return false;
    }
    else if (NameCodeTo == NAME_REF)
    {
        if (UpstreamNodeToConnect == UpstreamNodeToConnect->Owner->Inputs->Items[1])
            return true;
        else if (UpstreamNodeToConnect == UpstreamNodeToConnect->Owner->Outputs->Items[1]
                 && UpstreamNodeToConnect->DSet->PointsToDS == NULL)
            return false;
    }

    if ((NameCodeFrom == NAME_SYSCAST && UpstreamOwningNode->GetType() == TYPE_INPUT)
        || (NameCodeTo == NAME_SYSCAST && UpstreamNodeToConnect->GetType() == TYPE_INPUT))
        return true;

    // Ouput objects can hook up to anything; will be overridden by any known type.
    if (NameCodeFrom == NAME_OUTPUT || NameCodeTo == NAME_OUTPUT)
        return true;

    // This should be illegal, but variant Inputs/Ouputs will be overridden later.
    if (UpstreamNodeToConnect->IsOnVariantIO())
        return true;

    // Can hook to any variant node, except a bit to a variant exposer/collector.
    if (UpstreamOwningNode->DSet == VariantDataSet ||
    	UpstreamNodeToConnect->DSet == VariantDataSet)
    {
        // These variables are used to check both nodes both ways.
        Node1 = UpstreamOwningNode;
        Node2 = UpstreamNodeToConnect;

        for (int i = 0; i < 2; i ++)
        {
            if (Node1->DSet == BitDataSet)
            {
                if (Node2->Owner->ExposerCode == EXPOSER_IN)
                {
                    if (Node2->GetType() == TYPE_INPUT)
                        return false;
                }
                else if (Node2->Owner->ExposerCode == EXPOSER_OUT)
                {
                    if (Node2->GetType() == TYPE_OUTPUT &&
                    	Node2->Owner->NameCode != NAME_SYSCAST)
	                        return false;
                }
            }

            Node1 = UpstreamNodeToConnect;
            Node2 = UpstreamOwningNode;
        }

        return true;
    }

    // Nodes on an unattached junction object can be changed.
    return (UpstreamOwningNode->IsOnAtomicIO()
        && (UpstreamOwningNode->Owner->NameCode == NAME_JUNCTION ));
}


// Returns true if the node is variant and is on a regular Input/Output object.
bool Node::IsOnVariantIO()
{
    return ((Owner->NameCode == NAME_INPUT  ||
             Owner->NameCode == NAME_OUTPUT)  &&
             DSet == VariantDataSet);
}


// Returns true if the node has a control node name (Go, Done, Busy or Wait).
bool Node::IsControlNode()
{
    for (int i = 0; i < ceLast; i++)
        if (Name == ControlNodeNames[i])
            return true;

    return false;
}


// Connect the two nodes together (after removing any current connections).
void Node::Connect(Node *_ConnectedTo)
{
    Disconnect();
    ConnectedTo = _ConnectedTo;

    if (_ConnectedTo != NULL)
    {
        _ConnectedTo->Disconnect();
        _ConnectedTo->ConnectedTo = this;

	    if (Owner->NameCode == NAME_TRANSPORT && GetType() == TYPE_INPUT)
//        	PropagateDataSet(_ConnectedTo->DSet);
		    Owner->ChangeDataSets(_ConnectedTo->DSet);
	    else if (_ConnectedTo->Owner->NameCode == NAME_TRANSPORT && GetType() == TYPE_OUTPUT)
//        	_ConnectedTo->PropagateDataSet(DSet);
		    _ConnectedTo->Owner->ChangeDataSets(DSet);
    }
}


// Create a new exposer/collector.  Add it and the two new transports to CompressBaseList.
//     Connect the two nodes as follows:
//
//     Change:  this ---> ConnectedTo        To:  this ---> Exposer ---> NodeToConnect
//                                                                  ---> ConnectedTo
//
//     Change:  ConnectedTo ---> this        To:  NodeToConnect ---> Collector ---> this
//                                                ConnectedTo   --->
//
//     Returns the node on the exposer/collector that the next node should be connected to.
//
Node *Node::InsertExposerCollector(Node *NodeToConnect)
{
    I2adl       *Exposer;
    NodeList    *OneNodeSide;
    NodeList    *TwoNodeSide;

    if (GetType() == TYPE_INPUT)
    {
        // Need a collector
        Exposer = new I2adl(ListDataSet->Collector, cnYesDisconnect);
        OneNodeSide = Exposer->Outputs;
        TwoNodeSide = Exposer->Inputs;
    }
    else
    {
        // Need an exposer.
        Exposer = new I2adl(ListDataSet->Exposer, cnYesDisconnect);
        OneNodeSide = Exposer->Inputs;
        TwoNodeSide = Exposer->Outputs;

        // Because data sets flow down stream, change the transport to match its new source.
        //     Fixes bug with two adjacent List exposer/collectors not being combined.
        if (ConnectedTo != NULL)
            ConnectedTo->Owner->ChangeDataSets(TwoNodeSide->Items[0]);
    }

    MainCompiler->BLAdd(CompressBaseList, Exposer);

    // Attach NodeToConnect to the top of the exposer/collector.
    //     Note:  Exposers/collectors display their nodes backwards.
    //            Items[1] is the top node.  Items[0] is the bottom node.
    if (NodeToConnect != NULL)
       TwoNodeSide->Items[1]->CreateTransportAndConnect(NodeToConnect);

    TwoNodeSide->Items[0]->Connect(ConnectedTo);
    OneNodeSide->Items[0]->CreateTransportAndConnect(this);

    return TwoNodeSide->Items[1];
}


// Create a new transport and connect the two nodes together.
//     Calling routine must determine that the nodes are compatible.
//     The new transport will be added to CompressBaseList.
void Node::CreateTransportAndConnect(Node *NodeToConnect)
{
    I2adl    *Transport = new I2adl(NAME_TRANSPORT);

    if (GetType() == TYPE_INPUT)
    {
        Transport->ChangeDataSets(NodeToConnect);
        Connect(Transport->Outputs->Items[0]);
        NodeToConnect->Connect(Transport->Inputs->Items[0]);
    }
    else
    {
        Transport->ChangeDataSets(this);
        Connect(Transport->Inputs->Items[0]);
        NodeToConnect->Connect(Transport->Outputs->Items[0]);
    }

	MainCompiler->PlaceObject(Transport, this);
    MainCompiler->BLAdd(CompressBaseList, Transport);
}


// Disconnect the two nodes from each other.
//		If FixSelectDataSets is true, then disconnected $Select objects will have their
//		data sets reset.  This parameter was added for speed purposes.
void Node::Disconnect(bool FixSelectDataSets)
{
    if (this == NULL || ConnectedTo == NULL)
        return;

    // The following is required because the I2adlList copy constructor uses the old connection
    //     information while the new I2adlList is in the process of being created.
    Node	*RealConnectedTo = NULL;

    if (ConnectedTo->ConnectedTo == this)
    {
		RealConnectedTo = ConnectedTo;

        ConnectedTo->ConnectedTo = NULL;
    }

    ConnectedTo = NULL;

    // Disconnecting the #0 or #1 input of a $Select object may change its output data set.
	if (FixSelectDataSets)
    {
    	Node	*InputNode = (this->GetType() == TYPE_INPUT) ? this
        													 : RealConnectedTo;

        if (InputNode != NULL)
        {
            I2adl	*InputOwner = InputNode->Owner;

            if (InputOwner->NameCode == NAME_SSELECT)
                if (InputNode == InputOwner->Inputs->Items[0] ||
                    InputNode == InputOwner->Inputs->Items[1])
                        InputNode->PropagateDataSet(InputNode->DSet);
        }
    }
}


// Set the data set on the node and try to propagate it through any attached transports,
//     junctions and I/O objects.  Returns UpdateProblems to indicate if hit an object that
//     could not be updated (composite object with non variant node).
void __fastcall Node::PropagateDataSet(DataSet *_DataSet)
{
    DSet = _DataSet;

    if (ConnectedTo != NULL && GetType() == TYPE_OUTPUT)
        PropagateOperation(ConnectedTo, poDataSets, _DataSet);
    else
        PropagateOperation(this, poDataSets, _DataSet);
}


// Returns true if the node name is different from the default.
bool Node::ShowNodeName()
{
    int		NameLen = Name->Length();

    if (NameLen >= 3)
    {
        if (Name->SubString(1, 3) == strOut)
        {
            if (NameLen == 3)
                return false;

            // Is the next character a digit?
            if ((*Name)[4] > 47 && (*Name)[4] < 58)
                return false;
        }
    }

    if (NameLen >= 2)
    {
        if (Name->SubString(1, 2) == strIn)
        {
            if (NameLen == 2)
                return false;

            // Is the next character a digit?
            if ((*Name)[3] > 47 && (*Name)[3] < 58)
                return false;
        }
    }

    return true;
}


// Calculates the Sheet coordinates of a node
void Node::SheetCoordinates(POINT &SheetPoint)
{
    // Get the coordinates of the upper left corner of the node relative to the Sheet.
    //     Adjust by 1 pixel to center Insert 7 x 7 I2adl object nodes.
    SheetPoint = I2adlView->ScreenToClient(Owner->I2adlShape->ClientToScreen(Point(Left, Top)));

    int		NodeFudge = GetSize();

    if (NodeFudge > ViewScale)
    {
        NodeFudge = (NodeFudge - ViewScale) >> 1;
        SheetPoint.x += NodeFudge;
        SheetPoint.y += NodeFudge;
    }
}


int Node::GetLeft()
{
	return m_Left * ViewScale;
}


int Node::GetTop()
{
    bool	Offset = (GetSize() - ViewScale) / 2;

    // Calculate the adjusted Left value and adjust Data to fit Grid Snap:
    int 	Data = m_Top * ViewScale - int(ViewScale * Offset * .21);

	return Data;
}


// Next 2 functions set the left/top/width/height values in the custom control and OwningNode.
void Node::SetLeft(int Data)
{
    // Only alter Data for nodes not belonging to Junctions:
    // Null check cuz VivaIO::ReadObjectHeader() creates the nodes before the object is created.
    if (Owner != NULL && Owner->NameCode != NAME_JUNCTION)
    {
        // Calculate the adjusted Left value and adjust Data to fit Grid Snap:
        AlignToGrid(Data);
    }

    m_Left = Data / ViewScale;
}


void Node::SetTop(int Data)
{
    // Take Width into account; want to center this node on grid snap,
    //      rather than putting the upper-left corner on grid snap:
    bool	Offset = (GetSize() - ViewScale) / 2;

    // Calculate the adjusted Left value and adjust Data to fit Grid Snap:
    Data += int(ViewScale * Offset * .21);
    AlignToGrid(Data);

    m_Top = Data / ViewScale;
}


// Handles the mouse down event on an object's node.
void Node::MouseDown(TMouseButton Button, int X, int Y)
{
    if (I2adlView->ActiveTransport != NULL)
    {
    	// Drawing a transport:  HandleTransportDraw() handles all cases.
        POINT	SheetPoint;

        SheetCoordinates(SheetPoint);

        HandleTransportDraw(Button, SheetPoint.x, SheetPoint.y);
    }
    else if (Button == mbRight)
    {
		// Right mouse button:  Display the Attribute Editor (unless a transport/junction).
        ResetMove(true);

        if (Owner->NameCode != NAME_TRANSPORT  &&
            Owner->NameCode != NAME_JUNCTION)
        {
            AttributeDialog->EditObjectAttributes(Owner, Owner);
        }
        else if (ConnectedTo != NULL &&
                 ConnectedTo->Owner->NameCode != NAME_TRANSPORT  &&
                 ConnectedTo->Owner->NameCode != NAME_JUNCTION)
        {
            AttributeDialog->EditObjectAttributes(ConnectedTo->Owner, ConnectedTo->Owner);
        }
    }
    else
    {
        // Left mouse button:  Start drawing a new transport from this node (unless it is
        //		already connected).
        if (ConnectedTo == NULL)
            InitTransportDrawMode(this);
    }
}


// Handles mouse movements over nodes.
void Node::MouseMove(TShiftState Shift, int X, int Y)
{
    // Send this event to the appropriate handler.
    TPoint	P;
    
    SheetCoordinates(P);
    I2adlEditor::MouseMove(Shift, P.x, P.y);

    // If a transport is on top of an I2adl object, then use the I2adl object's node unless
    //     it is Variant.
    Node	*CaptionNode = this;

    if (ConnectedTo != NULL)
    {
        if (Owner->NameCode == NAME_TRANSPORT)
        {
        	if (ConnectedTo->DSet != VariantDataSet)
	            CaptionNode = ConnectedTo;
        }
        else
        {
        	if (CaptionNode->DSet == VariantDataSet)
            	CaptionNode = ConnectedTo;
        }
    }

    if (NewCaptionSource(CaptionNode))
        SetStatusCaption("Node: " + *CaptionNode->Name + "; DataSet: " + CaptionNode->DSet->
        	Name);
}


// Handles the double click event on a node.
//		The only valid action for double clicking on a node is to insert a junction on a
//		transport elbow.
void Node::DblClick()
{
    if (Owner->NameCode == NAME_TRANSPORT &&
    	ConnectedTo != NULL &&
    	ConnectedTo->Owner->NameCode == NAME_TRANSPORT)
    {
	    Owner->XShape->InsertJunction(true, Left, Top, false, ConnectedTo->Owner, this,
        	ConnectedTo);
    }
}


// If the data set is Variant, then look downstream to see if a VouchDataSet object made it
//		that way and restore the original data set.
void Node::RestoreOriginalDataSet()
{
    if (DSet == VariantDataSet && ConnectedTo != NULL)
    {
        NodeList	*TransportOutputs = ConnectedTo->Owner->Outputs;

        for (int i = 0; i < TransportOutputs->Count; i++)
        {
            Node	*ConnectedNode = TransportOutputs->Items[i]->ConnectedTo;

            if (ConnectedNode != NULL &&
                ConnectedNode->Owner->NameCode == NAME_VOUCH_DATA_SET)
            {
                // Restore the original data set.
                PropagateDataSet(ConnectedNode->Owner->Outputs->Items[0]->DSet);

                return;
            }
        }
    }
}


void Node::Paint()
{
    if (this == NULL)
        return;

    MyTShapeI2adl	*OwnerShape = Owner->I2adlShape;

    if (OwnerShape == NULL)
        return;

    TCanvas			*Canvas = OwnerShape->GetCanvas();

    // Draw transport corners.
    if (Owner->NameCode == NAME_TRANSPORT &&
        ConnectedTo != NULL &&
        ConnectedTo->Owner->NameCode == NAME_TRANSPORT)
    {
        // We are dealing with two connected transports..draw corners.
        MyTShapeTransport    *CShape = ConnectedTo->Owner->XShape;

        if (CShape == NULL)
            return;

        MyTShapeTransport	*XShape = Owner->XShape;
        int 				Corner = 0;

        if (XShape->Left == CShape->Left)
            Corner = 2;

        if (XShape->Top == CShape->Top)
            Corner++;

        VivaIcon	*Icon = TransportCorners[Corner];
        TRect       DrawRect;

        DrawRect.Left = Left;
		DrawRect.Top = Top;
		DrawRect.Right = Left + ViewScale;
		DrawRect.Bottom = Top + ViewScale;

        if (Owner->I2adlShape->Selected)
            Canvas->StretchDraw(DrawRect, Icon->SelectedBitmap);
        else
            Canvas->StretchDraw(DrawRect, Icon->Bitmap);

        return;
    }

    // If the node is not connected or if "View Nodes" turned on,
    //  then display colored nodes.
    Canvas->Brush->Color = (TColor) DSet->ColorCode;
    Canvas->FillRect(Rect(Left, Top, Left + GetSize(), Top + GetSize()));

    if (Owner->NameCode != NAME_TRANSPORT &&
        ConnectedTo != NULL &&
        ConnectedTo->Owner->NameCode == NAME_TRANSPORT)
    {
		ConnectedTo->Paint();
    }
}


// Find the Object creating the signal for a node.
Node *Node::SignalSource()
{
    Node *InputNode = this;

    while (InputNode != NULL)
    {
        I2adl *SourceObject = InputNode->Owner;

        if (SourceObject->NameCode != NAME_TRANSPORT)
            break;

        if (SourceObject->Behavior != NULL)
            break;

        InputNode = SourceObject->Inputs->Items[0]->ConnectedTo;
    }

    return InputNode;
}


// Create and connect up a Bit Fan exposer/collector object to the input/output node using
//     the SourceDataSet.
//     If UpdateDataSet is true, then the node will be given the SourceDataSet.
//     If UpdateAssignedBehavior is true, then the new object and its transport will be
//     assigned to the node's system and added to its AssignedBehavior.
//
//     Note:  Because the node will be connected up to the new object, the calling program
//     may need to save the original connection.
I2adl *Node::CreateBitFanObject(DataSet *SourceDataSet, bool UpdateDataSet, bool
	UpdateAssignedBehavior)
{
	// Are we updating the node's data set?
    if (UpdateDataSet)
    	DSet = SourceDataSet;

	I2adl	*BitFanObject = NULL;

    if (GetType() == TYPE_INPUT)
    {
    	// This is an input node.  Create a Bit Fan collector object.
        BitFanObject = new I2adl(SGDataSets[sdsDBit]->Collector, cnYesDisconnect);
        BitFanObject->NameCode = NAME_BIT_FAN_COL;
        BitFanObject->NameInfo = I2adlStringTable->IconEntry(I2adlNames[NAME_BIT_FAN_COL]);

        // Create a bit input node for every bit in the larger-than-Bit data set.
        //     Note:  The first two nodes came from the DBit collector object.
        //     Unique node names are required for text base persistence.
        for (int i = 2; i < SourceDataSet->BitLength; i++)
            AddNode(&BitFanObject->Inputs, new Node(*NodestrIn + IntToStr(i + 1),
            	BitFanObject, BitDataSet));

        // Connect up the big data set.
        BitFanObject->Outputs->Items[0]->DSet = SourceDataSet;
        BitFanObject->Outputs->Items[0]->CreateTransportAndConnect(this);
    }
    else
    {
    	// This is an output node.  Create a Bit Fan exposer object.
        BitFanObject = new I2adl(SGDataSets[sdsDBit]->Exposer, cnYesDisconnect);
        BitFanObject->NameCode = NAME_BIT_FAN_EXP;
        BitFanObject->NameInfo = I2adlStringTable->IconEntry(I2adlNames[NAME_BIT_FAN_EXP]);

        // Create a bit output node for every bit in the larger-than-Bit data set.
        //     Note:  The first two nodes came from the DBit exposer object.
        //     Unique node names are required for text base persistence.
        for (int i = 2; i < SourceDataSet->BitLength; i++)
            AddNode(&BitFanObject->Outputs, new Node(*NodestrOut + IntToStr(i + 1),
            	BitFanObject, BitDataSet));

        // Connect up the big data set.
        BitFanObject->Inputs->Items[0]->DSet = SourceDataSet;
        BitFanObject->Inputs->Items[0]->CreateTransportAndConnect(this);
    }

    MainCompiler->FDAdd(BitFanObject);

	// Copy the FPGA attributes that need to propagate to the connected transports.
    AnsiString	AttributeValue = Owner->Attributes->Values[strNet];
    if (AttributeValue != strNull)
		BitFanObject->SetAttribute(strNet, AttributeValue);

    AttributeValue = Owner->Attributes->Values[strTIG];
    if (AttributeValue != strNull)
		BitFanObject->SetAttribute(strTIG, AttributeValue);

    AttributeValue = Owner->Attributes->Values[strTNM];
    if (AttributeValue != strNull)
		BitFanObject->SetAttribute(strTNM, AttributeValue);

    // Load the ErrorTrace information.
    BitFanObject->CopyHierarchyInfo(Owner);
    BitFanObject->BitFanCode = bfcNo;
    Owner->BitFanCode = bfcYes;

    // Are we up updating the AssignedBehavior?
    if (UpdateAssignedBehavior &&
    	Owner->InSystem != NULL)
    {
    	BitFanObject->InSystem = Owner->InSystem;
        this->ConnectedTo->Owner->InSystem = Owner->InSystem;

        Owner->InSystem->AssignedBehavior->Add(BitFanObject);
        Owner->InSystem->AssignedBehavior->Add(this->ConnectedTo->Owner);
    }

    return BitFanObject;
}


// If the node is on a Bit Fan exposer/collector object, then the node on the primitive
//     object that caused the Bit Fan to be created is returned.  Othewise, the node is its
//     own source.
Node *Node::BitFanSource()
{
	int		NodeType = GetType();

	if (NodeType == TYPE_OUTPUT &&
    	Owner->NameCode == NAME_BIT_FAN_EXP)
    {
    	// This is a Bit Fan exposer.  Return the upstream object.
        Node	*InputNode = Owner->Inputs->Items[0]->ConnectedTo;

        if (InputNode != NULL)
        {
        	Node	*SourceNode = InputNode->Owner->Inputs->Items[0]->ConnectedTo;

            if (SourceNode != NULL)
            	return SourceNode;
        }
    }
    else if (NodeType == TYPE_INPUT &&
    		 Owner->NameCode == NAME_BIT_FAN_COL)
    {
    	// This is a Bit Fan collector.  Return the downstream object.
        Node	*OutputNode = Owner->Outputs->Items[0]->ConnectedTo;

        if (OutputNode != NULL)
        {
        	NodeList	*TransportOutputs = OutputNode->Owner->Outputs;

        	for (int i = 0; i < TransportOutputs->Count; i++)
            {
                Node	*SourceNode = TransportOutputs->Items[i]->ConnectedTo;

                if (SourceNode != NULL)
                    return SourceNode;
            }
        }
    }

    return this;
}

