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
#include "AttributeCalculator.h"
#include "I2adl.h"
#include "I2adlList.h"
#include "VivaIcon.h"
#include "DataSet.h"
#include "ErrorTrap.h"
#include "Main.h"
#include "MyTShape.h"
#include "Node.h"
#include "Project.h"
#include "MyTScrollingWinControl.h"
#include "VivaSystem.h"
#include "ComI2adl.h"
#include "VivaIO.h"
#include "I2adlEditor.h"
#include "SearchEngine.h"
#include "X86ExThread.h"
#include "ComManager.h"
#include "EditTextFile.h"

typedef unsigned __int64 QWORD;

#pragma package(smart_init)

AnsiString          AsciiTrue = "true";
int                 DownwardCompatibleCount;
int                 IsVisitedKey = 1;  // Used for IsVisited Flag.  Increment to make unique.
extern I2adlList    *FunctionDescription;
const int			HighestBreakerIndex = 6;
const int 			Breakers[HighestBreakerIndex] = {0, 11, 24, 32, 60};
BYTE				EdifIDTemplate[EDIF_NAME_BUFFER_LIMIT];


#define JUNCTION_SIZE (ViewScale * 3)

#define DEFUNCT_COUNT 11
AnsiString DefunctNames[DEFUNCT_COUNT] =
{
    "ViewControl",      // Temporary attribute string - Icon (now tied to object name).
    "Location=",         // Remove any Location=?
    "IconIndex",        // Temporary attribute string - ViewControl
                        // Skip strings added for the old Attribute Editor.
    "Name=",
    "Description=",
    ".Name",
    ".DataSet",         // Remove information rate value strings (now in Node object).
    "IR",
    "CL",
    "RL",
    "PropertyIndex="    // Now computed dynamically to remove source of error
};


// Table of I2adl names corresponding to the name codes
AnsiString I2adlNames[MAX_NAMECODES] =
{
    "",                     // NAME_OTHER
    "Input",                // NAME_INPUT
    "Output",               // NAME_OUTPUT
    "Transport",            // NAME_TRANSPORT
    "AND",                  // NAME_AND
    "OR",                   // NAME_OR
    "INVERT",               // NAME_INVERT
    "Assign",               // NAME_ASSIGN
    "$Select",              // NAME_SSELECT
    "Junction",             // NAME_JUNCTION
    "Ref",                  // NAME_REF
    "DeRef",                // NAME_DEREF
    "Release",              // NAME_RELEASE
    "Text",                 // NAME_TEXT
    "VariantIn",            // NAME_VARIANT_IN
    "VariantOut",           // NAME_VARIANT_OUT
    "OPAD",                 // NAME_OPAD
    "DPAD",                 // NAME_DPAD
    "IPAD",                 // NAME_IPAD
    "IBUF",                 // NAME_IBUF
    "OBUF",                 // NAME_OBUF
    "IOTBuf",               // NAME_IOTBUF
    "GND",                  // NAME_GND
    "VCC",                  // NAME_VCC
    "MUXCY",                // NAME_MUXCY
    "XORCY",                // NAME_XORCY
    "BUFT",                 // NAME_BUFT
    "FDCE",                 // NAME_FDCE
    "FDCPE",                // NAME_FDCPE
    "FDRE",                 // NAME_FDRE
    "FDRSE",                // NAME_FDRSE
    "MULT_AND",             // NAME_MULT_AND
    "XORCY_D",              // NAME_XORCY_D
    "XORCY_L",              // NAME_XORCY_L
    "MUXCY_D",              // NAME_MUXCY_D
    "MUXCY_L",              // NAME_MUXCY_L
    "ORCY",                 // NAME_ORCY
    "SRLC16E",              // NAME_SRLC16E
    "FDDRRSE",              // NAME_FDDRRSE
    "FDDRCPE",              // NAME_FDDRCPE
    "$Cast",                // NAME_SYSCAST
    "LinkIN",               // NAME_LINKIN
    "LinkOUT",              // NAME_LINKOUT
    "GlobalClock",          // NAME_GLOBAL_CLOCK
    "Wire",                 // NAME_WIRE
    "$RegClr",              // NAME_REGCLR
    "$ADC",                 // NAME_ADC
    "MULT18X18S",			// NAME_MULT18X18S
    "Go",                   // NAME_GO
    "Stop",                 // NAME_STOP
    "$Spawn",               // NAME_SPAWN
    "Delay",                // NAME_DELAY
    "",						// NAME_DISPATCH		// Class of objects (not just 1 name)
    "",						// NAME_COMCREATE       // Class of objects (not just 1 name)
    "BitFanExp",			// NAME_BIT_FAN_EXP
    "BitFanCol",			// NAME_BIT_FAN_COL
    "RegularExp",			// NAME_REGULAR_EXP
    "RegularCol",			// NAME_REGULAR_COL
    "$RAMBDP",              // NAME_RAMBDP
//    "$RAMSDP",              // NAME_RAMSDP
//    "$RAMSSP",              // NAME_RAMSSP
    "$RAMBSP",              // NAME_RAMBSP
    "$VouchDataSet",        // NAME_VOUCH_DATA_SET
};


__fastcall I2adl::I2adl(AnsiString _Name, int InputNodeCount, int OutputNodeCount)
{
    SetName(_Name);
    I2adl_Init(InputNodeCount, OutputNodeCount);
}


// Constructor - with placement.
__fastcall I2adl::I2adl(NameCodes _NameCode, int _X, int _Y)
{
    NameCode = _NameCode;

    NameInfo = I2adlStringTable->IconEntry(I2adlNames[NameCode]);
    I2adl_Init();
    X = _X;
    Y = _Y;
}


// Constructor - Copy
//     CopyNodes determines if the node lists are copied or empty.  If copied, then the
//     calling program must fix node pointers because both sets of nodes are left connected.
//     Hence the option to disconnect the copied nodes.
//     If OnlyCopySystemAtt is true, then the only attribute that will be copied into the
//     new object will be the System attribute.
__fastcall I2adl::I2adl(I2adl *_I2adl, CopyNodesEnum CopyNodes, bool OnlyCopySystemAtt)
{
    if (_I2adl == NULL)
        return;

    *this = *_I2adl;

    for (int i = 0; i < ceLast; i++)
        FakeCNodes[i] = NULL;

    if (ExecuteType == EXECUTE_COM)
    {
        if (_I2adl->ComHelper->MemberDesc->Type == mtEvent)
            ComHelper = new TComHelperEvent(_I2adl->ComHelper, this);
        else
            ComHelper = new TComHelperFunc(_I2adl->ComHelper, this);
    }

    CopyOf = _I2adl;

    TruthTable = NULL;
	ParentI2adl = NULL;
    OwningLib = NULL;
    IsSystemObject = false;
	FlattenListIndex = -1;
    I2adlShape = NULL;
    WipSheetCode = wsNotAWipSheet;
    IsVisited = false;
    Ambiguous = ambNo;
    ObjectCost = _I2adl->ObjectCost;

    // Copy the EdifID to prevent it from being deleted twice.
    if (EdifID != NULL)
    {
    	int		ArraySize = EdifID[0] + 1;

	    EdifID = new BYTE[ArraySize];

	    memcpy(EdifID, _I2adl->EdifID, ArraySize);
    }

    // Are we copying all of the attribute strings or just the system attribute?
    Attributes = new TStringList();

    if (OnlyCopySystemAtt)
    {
    	ClearAttributes();

        AttSystem = _I2adl->AttSystem;
    }
    else
    {
	    ReplaceAttributes(_I2adl);
    }

    // Do we copy the nodes or create empty node lists?
    if (CopyNodes == cnNo)
    {
        Inputs = NullNodeList;
        Outputs = NullNodeList;
    }
    else
    {
        // Calling program must fix node pointers because both sets of nodes are left connected.
        Inputs = NewNodeList(_I2adl->Inputs, this);
        Outputs = NewNodeList(_I2adl->Outputs, this);

        if (CopyNodes == cnYesDisconnect)
            this->DisconnectObject();
    }
}


// Automatically called by C++ Builder before destructor.
I2adl::~I2adl()
{
	delete[] EdifID;
    EdifID = NULL;

    delete Attributes;
    Attributes = NULL;

    if (TruthTable != NULL)
    {
    	for (int i = 0; i < TruthTable->Count; i++)
		    delete (TruthTableEntry *) TruthTable->Items[i];

        delete TruthTable;

	    TruthTable = NULL;
    }

    DeleteNodeList(Inputs);
    Inputs = NULL;

    DeleteNodeList(Outputs);
    Outputs = NULL;

    if (I2adlShape != NULL)
    {
        I2adlShape->OwningI2adl = NULL;
	    I2adlShape = NULL;
    }

    // The following prevents an access violation.
    if (ParentI2adlObject == this)
    	ParentI2adlObject = NULL;

	if (NameCode >= NAME_RAMBDP &&
    	NameCode <= NAME_RAMBSP)
    {
    	delete[] RamData;
    }
    else if (ExecuteType == EXECUTE_COM)
    {
   	    delete ComHelper;
    }
	else if (ExecuteType == NAME_DISPATCH)
    {
	    ComI2adlList->Remove(this);
    }
}


void I2adl::I2adl_Init(int InputNodeCount, int OutputNodeCount)
{
    for (int i = 0; i < ceLast; i++)
        FakeCNodes[i] = NULL;

    TriedForceFlatten = false;

    if (NameCode == NAME_TRANSPORT)
	    ffunc = FlattenTransport;
    else
    	ffunc = FlattenObject;

    DontUse = false;
	Warned0187 = false;
	Warned0199 = false;
	Warned4061 = false;
	Warned5002 = false;
    DoneFlattening = false;
    FlattenFutility = 0;
    ArchiveCount = 0;
    EdifID = NULL;
    SequenceNumber = -1;
    Selected = false;

    CopyOf = NULL;
    MasterSource = this;
    Inputs = NullNodeList;
    Outputs = NullNodeList;
    IconInfo = NULL;
    Ambiguous = ambNo;

    Attributes = new TStringList();
    ClearAttributes();

    TruthTable = NULL;
    Behavior = NULL;
    MemberOf = NULL;
    RamData = NULL;

    ExecuteType = NAME_OTHER;

    ExposerCode = EXPOSER_NOT;
    ObjectCost = NULL;
    InSystem = NULL;
    SearchListIndex = MAXINT;
    FlattenListIndex = -1;
	ParentI2adl = NULL;
    OwningLib = NULL;
    IsSystemObject = false;

    X = 0;
    Y = 0;
    Width = ViewScale * 13.41;
    Height = ViewScale * 7.81;
    I2adlShape = NULL;
    WipSheetCode = wsNotAWipSheet;
    IsVisited = false;
    KeepObject = koBothSides;
    IsPrimitive = true;
    SuperSet = BitDataSet;
    BitFanCode = bfcNotChecked;

    switch (NameCode)
    {
        case NAME_SSELECT:
        {
            Width = ViewScale * 11.41;
            AtomicTemplate(3, 1);
            Inputs->Items[0]->SetName("#0");
            Inputs->Items[1]->SetName("#1");
            Inputs->Items[2]->SetName("S");
            Inputs->Items[0]->DSet = VariantDataSet;
            Inputs->Items[1]->DSet = VariantDataSet;

            Outputs->Items[0]->DSet = VariantDataSet;
            break;
        }

        case NAME_TEXT:
        {
        	SetDefaultTextDimensions();
            break;
        }

        case NAME_JUNCTION:
        {
            Inputs = NewNodeList(1);
            Outputs = NewNodeList(3);
            Inputs ->Add(new Node(*NodestrIn  , this, BitDataSet, 0, ViewScale));
            Outputs->Add(new Node(*NodestrOut1, this, BitDataSet, ViewScale, 0));
            Outputs->Add(new Node(*NodestrOut2, this, BitDataSet, ViewScale * 2, ViewScale));
            Outputs->Add(new Node(*NodestrOut3, this, BitDataSet, ViewScale, ViewScale * 2));
            Width = JUNCTION_SIZE;
            Height = JUNCTION_SIZE;
            break;
        }

        case NAME_ASSIGN:
        {
            AtomicTemplate(2, 1);
            break;
        }

        case NAME_AND:
        case NAME_OR:
        {
            AtomicTemplate(2, 1);
            break;
        }

        case NAME_TRANSPORT:
        {
            // Control nodes always come before the data nodes.
            Width = ViewScale;
            Height = ViewScale;
        }

        case NAME_INVERT:
        {
            AtomicTemplate(1, 1);
            break;
        }

        case NAME_LINKIN:
        {
            // Calling routine must fill data in input/output nodes.
            IsPrimitive = false;
        }

        case NAME_INPUT:
        {
            Width = DEFAULT_INPUT_OUTPUT_WIDTH;
            Height = ViewScale * 3;
            Outputs = NewNodeList(1);
            Outputs->Add(new Node(*NodestrIn, this, VariantDataSet));//, ViewScale * 4.21, ViewScale * .81));
            break;
        }

        case NAME_OUTPUT:
        {
            Width = DEFAULT_INPUT_OUTPUT_WIDTH;
            Height = ViewScale * 3;
            Inputs = NewNodeList(1);
            Inputs->Add(new Node(*NodestrOut, this, VariantDataSet, 0, ViewScale * .81));
            break;
        }

        case NAME_LINKOUT:
        {
            // Calling routine must fill data in input/output nodes.
            IsPrimitive = false;
        }

        case NAME_GLOBAL_CLOCK:
        {
            Inputs = NewNodeList(1);
            Node *NewNode = new Node(*NodestrOut, this, BitDataSet);
            NewNode->Top = ViewScale * .81;

            Inputs->Add(NewNode);

            Width = ViewScale * 5.41;
            Height = ViewScale * 3;
            break;
        }

        case NAME_GND:
        {
            AtomicTemplate(0, 1);
            //Outputs->Items[0]->SetName("G"); //this will be replacing in the near future
            Outputs->Items[0]->SetName("GROUND");
            break;
        }

        case NAME_VCC:
        {
            AtomicTemplate(0, 1);
            //this is the correct line, but crutch it for the old FPGAStrings
            //Outputs->Items[0]->SetName("P");
            Outputs->Items[0]->SetName("VCC");
            break;
        }

        case NAME_SYSCAST:
        {
            Inputs = NewNodeList(2);
            Outputs = NewNodeList(1);
            IsPrimitive = true;
            ExposerCode = EXPOSER_OUT;
            Inputs->Add(new Node(strData, this, VariantDataSet));
            Inputs->Add(new Node(strType, this, VariantDataSet));
            Outputs->Add(new Node(strOut, this, VariantDataSet));
            Inputs->Items[0]->DSet = VariantDataSet;
            Inputs->Items[1]->DSet = VariantDataSet;
            Outputs->Items[0]->DSet = VariantDataSet;
            Conformalize();
            break;
        }

        case NAME_BIT_FAN_EXP:
        case NAME_REGULAR_EXP:
        {
            ExposerCode = EXPOSER_IN;
            break;
        }

        case NAME_BIT_FAN_COL:
        case NAME_REGULAR_COL:
        {
            ExposerCode = EXPOSER_OUT;
            break;
        }

        case NAME_REF:
        {
            Inputs = NewNodeList(2);
            Outputs = NewNodeList(2);
            ExposerCode = EXPOSER_NOT;
            Inputs->Add(new Node(NodestrGo->c_str(), this, BitDataSet));
            Outputs->Add(new Node(NodestrDone->c_str(), this, BitDataSet));
            Inputs->Add(new Node(strData, this, VariantDataSet));
            Outputs->Add(new Node("Address", this, VariantDataSet));
            Conformalize();
            ExecuteType = NameCode;
            AttResource = ResourcestrTimeSlice;
            AttResourceQty = 1;

            break;
        }

        case NAME_DEREF:
        {
            Inputs = NewNodeList(2);
            Outputs = NewNodeList(2);
            ExposerCode = EXPOSER_NOT;
            Inputs->Add(new Node(NodestrGo->c_str(), this, BitDataSet));
            Outputs->Add(new Node(NodestrDone->c_str(), this, BitDataSet));
            Inputs->Add(new Node(strData, this, VariantDataSet));
            Outputs->Add(new Node("Value", this, VariantDataSet));
            Conformalize();
            ExecuteType = NameCode;
            AttResource = ResourcestrTimeSlice;
            AttResourceQty = 1;

            break;
        }

        case NAME_DELAY:
        {
            AtomicTemplate(1, 1);
            ExposerCode = EXPOSER_NOT;
            ExecuteType = NameCode;
            AttResource = ResourcestrTimeSlice;
            AttResourceQty = 1;
            InSystem = CPUSystem;

            break;
        }

        case NAME_RELEASE:
        {
            Inputs = NewNodeList(2);
            Outputs = NewNodeList(1);
            ExposerCode = EXPOSER_NOT;
            Inputs->Add(new Node(NodestrGo->c_str(), this, BitDataSet));
            Outputs->Add(new Node(NodestrDone->c_str(), this, BitDataSet));
            Inputs->Add(new Node("Dispatch", this, SGDataSets[sdsIDispatch]));
            Conformalize();
            ExecuteType = NameCode;
            AttResource = ResourcestrTimeSlice;
            AttResourceQty = 1;
            KeepObject = koOneSide;

            break;
        }

        default:
        {
            if (InputNodeCount | OutputNodeCount)
            {
                Inputs  = NewNodeList(InputNodeCount);
                Outputs = NewNodeList(OutputNodeCount);
            }
            else // Use default node list.
            {    // Calls to this code should be rooted out.
                Inputs = new NodeList;
                Outputs = new NodeList;
            }
        }
    }

    if (IconInfo == NULL)
        SetIconInfo();
}


// Get the IconInfo for this object.  If there is an icon indirection, then it is handled.
VivaIcon *I2adl::GetIconInfo()
{
    VivaIcon    *_IconInfo = IconInfo;

    while (_IconInfo->Bitmap == NULL && _IconInfo->SelectedBitmap != NULL)
        _IconInfo = (VivaIcon *) _IconInfo->SelectedBitmap;

    return _IconInfo;
}


// Returns what the $Cast object's output node should be based on the input nodes.
//     Handles data set patterns by creating any required building block data sets.
//
//     Warning!  This routine does not call BuildTreeView() because it is called by the
//               SearchEngine.  Other calling routines should rebuild the tree view.
DataSet *I2adl::BuildCastOutputDataSet()
{
    // Fix up old save files because the Data input node used to stay Variant.
    Node    *DataNode = Inputs->Items[0];

    if (DataNode->DSet == VariantDataSet  &&  DataNode->ConnectedTo != NULL)
        DataNode->DSet = DataNode->ConnectedTo->DSet;

    DataSet    *DataNodeDataSet = DataNode->DSet;
    DataSet    *TypeNodeDataSet = Inputs->Items[1]->DSet;

    // If not a data set pattern, then the output node always matches the Type node.
    if (TypeNodeDataSet != SGDataSets[sdsBIN]  &&
        TypeNodeDataSet != SGDataSets[sdsLSB]  &&
        TypeNodeDataSet != SGDataSets[sdsMSB])
    {
        return TypeNodeDataSet;
    }

    // If the Data node is Variant or contextual, then can't do data set pattern yet.
    int    DataSetBitLength = DataNodeDataSet->CalcBitLength();

    if (DataSetBitLength >= 0x10000)
        return VariantDataSet;

    // Get or make the requested pattern data set.
    if (TypeNodeDataSet == SGDataSets[sdsBIN])
        return GetOrMakePatternDataSet(sdsBIN, DataSetBitLength);
    else if (TypeNodeDataSet == SGDataSets[sdsLSB])
        return GetOrMakePatternDataSet(sdsLSB, DataSetBitLength);
    else
        return GetOrMakePatternDataSet(sdsMSB, DataSetBitLength);
}


// Load IconInfo with the name of the icon to be used by this object as follows:
//     1.  Exposer/collectors get the exposer/collector icon.
//     2.  All objects can get their icon from the "Icon" attribute string.
//     3.  Otherwise, the icon is the object name.  However, if some other object with the same
//         name has an "Icon" attribute string, then all objects receive the indirection.
//
//     ClearIndirection defaults to "false" because the indirection may be set by any object,
//     but should only be reset when the user removes the "Icon" attribute string.
void I2adl::SetIconInfo(bool ClearIndirection)
{
    IconInfo = NameInfo;

    // #1 Exposer/collector objects get the exposer/collector icon.
    if (ExposerCode == EXPOSER_IN)
    {
        IconInfo = ExposerIcon;
        return;
    }
    else if (ExposerCode == EXPOSER_OUT)
    {
        // $Cast is an exception.
        if (NameCode != NAME_SYSCAST)
        {
            IconInfo = CollectorIcon;
            return;
        }
    }

    // Non looping "while" loop allows "breaking".
    while (Attributes->Count > 0)
    {
        // 2.  All objects can get their icon from the "Icon" attribute string.
        AnsiString    IconAttribute = Attributes->Values[strIcon];

        if (IconAttribute.Length() <= 0)
            break;

        // Delete the "Icon" attribute string because the indirection will rule.
        int    idx = Attributes->IndexOfName(strIcon);

        if (idx >= 0)
            Attributes->Delete(idx);

        // Establish the indirection by setting ObjectIcon to NULL and SelectedIcon to the
        //     indirection IconInfo.
        VivaIcon    *IndirectIconInfo = I2adlStringTable->IconEntry(IconAttribute);

        // If creating this indirection would cause a circular reference, then remove it.
        IconInfo->ClearIcon();

        VivaIcon    *SearchIconInfo = IndirectIconInfo;

        while (SearchIconInfo->Bitmap == NULL && SearchIconInfo->SelectedBitmap != NULL)
            SearchIconInfo = (VivaIcon *) SearchIconInfo->SelectedBitmap;

        if (SearchIconInfo == IconInfo)
        {
        	// Inform the user of the circular reference.
            ErrorTrap(97, VStringList(IconAttribute, NameInfo->Name));

            IconInfo->LoadIcon();
        }
        else
        {
            IconInfo->SelectedBitmap = (TBitMap *) IndirectIconInfo;
        }

        return;
    }

    // 3.  Otherwise, the icon is the object name.  However, if some other object with the same
    //     name has an "Icon" attribute string, then all objects receive the indirection.
    // Should the indirection be removed?
    if (ClearIndirection  &&  HasIconIndirection())
        IconInfo->LoadIcon();
}


// Returns "true" if the IconInfo has an indirection to another iconInfo.
bool I2adl::HasIconIndirection()
{
   return (IconInfo->Bitmap == NULL && IconInfo->SelectedBitmap != NULL);
}


// Returns True if the object has an "IOBuf=True" attribute.
bool I2adl::HasIOBufAttribute()
{
    AnsiString	IOBufAttribute;

    GetAttribute(strIOBuf, IOBufAttribute);

    return !(IOBufAttribute == strNull ||
	         IOBufAttribute[1] == 'F' ||
	         IOBufAttribute[1] == 'f');
}


// Conformal computations for Object size and Node placement.
void I2adl::Conformalize()
{
    if (NameCode == NAME_TRANSPORT || NameCode == NAME_TEXT)
        return;

    if (NameCode == NAME_JUNCTION)
	{
        Width = JUNCTION_SIZE;
        Height = JUNCTION_SIZE;

        Node	*input = Inputs->Items[0];
        int 	nodepos = (input->Left > input->Top)
        ?   (input->Top == 0)
            ?  1
            :  2
        :   (input->Left == 0)
            ?  0
            :  3;

        PlaceJunctionNodes(input, nodepos);

    	return;
    }

#if VIVA_IDE
    SetIconInfo();
#endif

	// Recalculate the object width.
    if (NameCode == NAME_INPUT || NameCode == NAME_OUTPUT)
    {
		Width = DEFAULT_INPUT_OUTPUT_WIDTH;
    }
    else
    {
#if VIVA_IDE
		// Calculate the object width in the standard view scale (old grid snap) so the object
        //     width (in grid snaps) will not change from zoom percent to zoom percent.
		int			OrigViewScale = ViewScale;

        ViewScale = OLD_GRID_SNAP;

        // Make the object wide enough for the node names, the icon and object name.
        int			NewWidth = 0;
        double		InputNodeNameWidth = Inputs->NodeNameWidth();
        double		OutputNodeNameWidth = Outputs->NodeNameWidth();
        VivaIcon    *icon = GetIconInfo();

        if (icon != NULL && icon->Bitmap != NULL)
        {
            if (icon->OrigWidth == 0)
                icon->OrigWidth = icon->Bitmap->Width;

            int		iconwidth = double(icon->OrigWidth) * ViewScale / OLD_GRID_SNAP + .5;

            NewWidth = max(InputNodeNameWidth, OutputNodeNameWidth) * 2	+ iconwidth;
        }
        else
        {
            NewWidth = InputNodeNameWidth + OutputNodeNameWidth + ViewScale * 3.81;
        }

        SetMax(NewWidth, ViewScale * 11.81);
        SetMax(NewWidth, short(NameInfo->Object) + ViewScale * 4);

        // Round the width size so that vertical transports don't split and so that nodes are
        //     flush up against right edge.
        //     This rounding gives objects the same width they had in Viva 2.4.1.
		NewWidth -= NewWidth % ViewScale;
        Width = NewWidth;

        ViewScale = OrigViewScale;
#endif
    }

    // Calculate the height of the object.
    // If only 1 input and 1 output (INVERT object), then add ViewScale * 2 extra pixels.
    int    	NodeCount = max(Inputs->Count, Outputs->Count);
	bool    IsIO = (NameCode == NAME_INPUT || NameCode == NAME_OUTPUT);

	if (IsIO)
        Height = max(NodeCount * NODE_SEPARATION, NODE_SEPARATION);
    else
        Height = max(NodeCount * NODE_SEPARATION, NODE_SEPARATION + ViewScale * 2)
        	+ ViewScale * 2;

    // Conformalize node placement for each node list.
    Node   *ControlNodes[4] = {NULL, NULL, NULL, NULL};

    if (!IsPrimitive || NameCode == NAME_GO || NameCode == NAME_STOP || NameCode == NAME_SPAWN)
    {
        Node    **pNodes = &ControlNodes[0];

        GetControlNodes(pNodes);
    }

    NodeList    *IONodes = Inputs;

    for (int IO = 0; IO < 2; IO++, IONodes = Outputs)
    {
        // Fix up the nodes on the Input/Output objects:
        if (IONodes->Count == 1 &&
            (NameCode == NAME_INPUT || NameCode == NAME_OUTPUT))
        {
            Node    *IONode = IONodes->Items[0];

            IONode->Left = ViewScale * 4.01 * (IONodes == Outputs);
            IONode->Top = ViewScale;
        }
        else
        {
            int     NodeCount = IONodes->Count;
            int     NodeToggle = NodeCount - 1;
            int     NodeNr = 0;
            int     noncontrol = 0;

            for (int i = 0; i < NodeCount; i++)
            {
                Node    *IONode = IONodes->Items[i];

                if (IONode == ControlNodes[ceGo] || IONode == ControlNodes[ceWait]
                    || IONode == ControlNodes[ceDone] || IONode == ControlNodes[ceBusy])
                {
                }
                else
                {
                    noncontrol++;
                }
            }

            for (int i = 0; i < NodeCount; i++)
            {
                Node    *IONode = IONodes->Items[i];

                IONode->Owner = this;

                int     NodePos;
                int     Side = IO;

                if (IONode == ControlNodes[ceGo] || IONode == ControlNodes[ceWait]
                    || IONode == ControlNodes[ceDone] || IONode == ControlNodes[ceBusy])
                {
                }
                else
                {
                    int     usenodenr = (ExposerCode == EXPOSER_NOT) ? NodeNr : NodeToggle - NodeNr;

                    // If one output node, center it.
                    int     TopNodePos = ViewScale * 2;

                    if (noncontrol == 1)
                        TopNodePos = NODE_SEPARATION;

                    NodePos = usenodenr * NODE_SEPARATION + TopNodePos;
                    NodeNr++;
                }

                IONode->Top = NodePos;
                IONode->Left = Side * (Width - int(ViewScale * 1.41));
            }

            PlaceControlNodes(ControlNodes);
        }
    }
}


// Places the first nodes having control node names at the bottom of the I2adl object.
void I2adl::PlaceControlNodes(Node *Nodes[4])
{
    int         RightNonControlNodeCount = Outputs->Count;
    int         LeftNonControlNodeCount = Inputs->Count;

    if (Nodes != FakeCNodes)
    {
        LeftNonControlNodeCount -= (Nodes[ceDone] != NULL);
        LeftNonControlNodeCount -= (Nodes[ceWait] != NULL);
        RightNonControlNodeCount -= (Nodes[ceGo] != NULL);
        RightNonControlNodeCount -= (Nodes[ceBusy] != NULL);
    }

    int     ncc = max(LeftNonControlNodeCount, RightNonControlNodeCount);

    for (int i = 0; i < ceLast; i++)
    {
        Node    *IONode = Nodes[i];

        if (IONode)
        {
            int     NodePos = ncc * NODE_SEPARATION + ViewScale * (1 + (ncc == 1)) + ViewScale;
            int     Side = (i >= 2);

            if (i == ceWait || i == ceBusy)
            {
                Side = 1 - Side;
                NodePos += NODE_SEPARATION;
            }

            IONode->Top = NodePos;
            IONode->Left = Side * (Width - int(ViewScale * 1.41));

            Height = max(Height, NodePos + NODE_SEPARATION);
        }
    }
}


// Copy the hierarchy information from the Object passed into "this" object.
void I2adl::CopyHierarchyInfo(I2adl *Object)
{
    MemberOf = Object->MemberOf;
    X = Object->X;
    Y = Object->Y;

    // Record the source object's name.
    AnsiString	Documentation = strFromObject + Object->NameInfo->Name;

    SetAttribute(strDocumentation, Documentation);
}


// Arrange the Input/Output horns on the behavior sheet so that the control nodes are always
//     the last objects on the sheet.
bool I2adl::ArrangeNodes(bool ConvertOldFile, bool Reconnect, bool SetDataSet,
	bool UpdateParentNodeLists)
{
    bool	NodesReordered = false;

    if (Behavior != NULL)
    {
    	// Move the I/O horns that define the control nodes to the bottom of the behavior sheet.
        TStringList		*InputHorns  = Behavior->GetInputs();
        TStringList		*OutputHorns = Behavior->GetOutputs();

        // Are the node counts correct (being rebuilt)?
        if (Reconnect)
        {
            // If the behavior doesn't match, then don't try to change anything.  This happens when
            //		an I2adl def refers to a behavior that has been changed (Bugzilla #897).
            if (Inputs->Count != InputHorns->Count ||
                Outputs->Count != OutputHorns->Count)
            {
        	    delete InputHorns;
	        	delete OutputHorns;

	        	return NodesReordered;
            }
        }

        // Pass #1 do Inputs (Go & Wait)
        TStringList		*IOHorns  = InputHorns;
        AnsiString		*StringGoDone = NodestrGo;
        AnsiString		*StringBusyWait = NodestrWait;

        for (int IO = 0; IO < 2; IO++)
        {
            // Must have at least 2 nodes to be out of order.
            if (IOHorns->Count < 2)
                continue;

            // Find the last Input/Output objects with a control node name.
            int		IndexGoDone = -1;
            int		IndexBusyWait = -1;

            for (int i = 0; i < IOHorns->Count; i++)
            {
                I2adl   	*IOHorn = (I2adl *) IOHorns->Objects[i];
                AnsiString	*NodeName = (IO <= 0) ? IOHorn->Outputs->Items[0]->Name
                                                  : IOHorn->Inputs->Items[0]->Name;

                if (NodeName == StringGoDone)
                    IndexGoDone = i;
                else if (NodeName == StringBusyWait)
                    IndexBusyWait = i;
            }

            // If the control nodes are already last, then don't change anything.
            int		LastIndex = IOHorns->Count - 1;
            I2adl   *LastObject = (I2adl *) IOHorns->Objects[LastIndex];
            int		LastObjectY = LastObject->Y;  // Localize so won't change.

            if (IndexGoDone <= -1)
            {
                // There is no Go/Done node.  Does the Busy/Wait node need to be moved?
                if (IndexBusyWait > -1 &&
                	IndexBusyWait < LastIndex)
                {
                    I2adl	*ObjectBusyWait = (I2adl *) IOHorns->Objects[IndexBusyWait];

                    ObjectBusyWait->Y = LastObjectY + ViewScale * 5;
                }
            }
            else if (IndexBusyWait <= -1)
            {
                // There is no Busy/Wait node.  Does the Go/Done node need to be moved?
                if (IndexGoDone < LastIndex)
                {
                    I2adl	*ObjectGoDone = (I2adl *) IOHorns->Objects[IndexGoDone];

                    ObjectGoDone->Y = LastObjectY + ViewScale * 5;
                }
            }
            else if (IndexGoDone != LastIndex - 1 ||
                     IndexBusyWait != LastIndex)
            {
                // There is a Go/Done and a Busy/Wait node and they need to be moved.
                I2adl	*ObjectGoDone = (I2adl *) IOHorns->Objects[IndexGoDone];
                I2adl	*ObjectBusyWait = (I2adl *) IOHorns->Objects[IndexBusyWait];

                ObjectGoDone->Y = LastObjectY + ViewScale * 5;
                ObjectBusyWait->Y = LastObjectY + ViewScale * 10;
            }

            // Pass #2 do Outputs (Done & Busy)
            IOHorns = OutputHorns;
            StringGoDone = NodestrDone;
            StringBusyWait = NodestrBusy;
        }

        delete InputHorns;
        delete OutputHorns;

        InputHorns  = Behavior->GetInputs();
        OutputHorns = Behavior->GetOutputs();

        TStringList     *InputListOrig = NULL;
        TStringList     *InputListNew = NULL;
        TStringList     *OutputListOrig = NULL;
        TStringList     *OutputListNew = NULL;

        // Save the original node locations.
        if (Reconnect)
        {
        	// This extra call to Conformalize() is required to have "node->Top" be accurate.
            //     It fixes the bug that messed up the control nodes in TestObjectRef.idl.
	        Conformalize();

            InputListOrig = new TStringList;
            InputListNew = new TStringList;
            OutputListOrig = new TStringList;
            OutputListNew = new TStringList;

            InputListOrig->Sorted = true;
            InputListNew->Sorted = true;
            OutputListOrig->Sorted = true;
            OutputListNew->Sorted = true;

            for (int i = 0; i < Inputs->Count; i++)
            {
                Node    *InputNode = Inputs->Items[i];

                InputListOrig->AddObject(100000 + InputNode->Top, (TObject *) InputNode);
            }

            for (int i = 0; i < Outputs->Count; i++)
            {
                Node    *OutputNode = Outputs->Items[i];

                OutputListOrig->AddObject(100000 + OutputNode->Top, (TObject *) OutputNode);
            }
        }

        // Rebuild the Input/Output node lists.
        NodeList    *OldInputs = Inputs;
        NodeList    *OldOutputs = Outputs;
        int         BehaviorPlacement = 0;

        Inputs = NewNodeList(InputHorns->Count);
        Outputs = NewNodeList(OutputHorns->Count);

        // Pass #1 - Inputs
        NodeList    *IONodes = Inputs;

        IOHorns  = InputHorns;

        for (int IO = 0; IO < 2; IO++)
        {
            for (int i = 0; i < IOHorns->Count; i++, BehaviorPlacement++)
            {
                I2adl    *Object = (I2adl *) IOHorns->Objects[i];

                int      j = Behavior->IndexOf(Object);

                // Note if the order of the nodes change so the user can be warned if he tries to
                //    update the original object.
                if (BehaviorPlacement != j)
                {
                    NodesReordered = true;
                    Behavior->Items[j] = Behavior->Items[BehaviorPlacement];
                    Behavior->Items[BehaviorPlacement] = Object;
                }

                // Add the node to the output node list.
                Node    *NewNode = (IO == 0) ? Object->Outputs->Items[0]
                                             : Object->Inputs->Items[0];

                NewNode = new Node(NewNode, this);
                NewNode->ConnectedTo = NULL;
                IONodes->Add(NewNode);
            }

            // Pass #2 = Outputs
            IONodes = Outputs;
            IOHorns = OutputHorns;
        }

        Conformalize();

        if (Reconnect)
        {
            for (int i = 0; i < Inputs->Count; i++)
            {
                Node    *node = Inputs->Items[i];

                InputListNew->AddObject(100000 + node->Top, (TObject *) node);
            }

            for (int i = 0; i < Outputs->Count; i++)
            {
                Node    *node = Outputs->Items[i];

                OutputListNew->AddObject(100000 + node->Top, (TObject *) node);
            }

            for (int i = 0; i < InputListOrig->Count; i++)
            {
                int     index = InputListNew->IndexOf(InputListOrig->Strings[i]);

                // The following is for objects that have had their node locations changed
                //		because a control node was added/removed.  Without it nodes may be
                //		moved, disconnected and renamed.
                if (index == -1 &&
                	!ConvertOldFile &&
                    NameCode != NAME_TRANSPORT)
                {
                	index = i;
                }

                if (index > -1)
                {
                    Node    *OldNode = (Node *) InputListOrig->Objects[i];
                    Node    *NewNode = (Node *) InputListNew->Objects[index];

                    *NewNode = *OldNode;
                    NewNode->Connect(OldNode->ConnectedTo);
                }
            }

            for (int i = 0; i < OutputListOrig->Count; i++)
            {
                int     index = OutputListNew->IndexOf(OutputListOrig->Strings[i]);

                // The following is for objects that have had their node locations changed
                //		because a control node was added/removed.  Without it nodes may be
                //		moved, disconnected and renamed.
                if (index == -1 &&
                	!ConvertOldFile &&
                    NameCode != NAME_TRANSPORT)
                {
                	index = i;
                }

                if (index > -1)
                {
                    Node    *OldNode = (Node *) OutputListOrig->Objects[i];
                    Node    *NewNode = (Node *) OutputListNew->Objects[index];

                    *NewNode = *OldNode;
                    NewNode->Connect(OldNode->ConnectedTo);
                }
            }

            delete InputListOrig;
            delete InputListNew;
            delete OutputListOrig;
            delete OutputListNew;

            if (ConvertOldFile)
            {
            	// Pass #1 - Inputs.
            	IONodes = Inputs;

                for (int IO = 0; IO < 2; IO++)
                {
                    for (int i = 0; i < IONodes->Count; i++)
                    {
                        Node    *IONode = IONodes->Items[i];

                        for (int j = 0; j < ceLast; j++)
                        {
                            if (FakeCNodes[j] != NULL)
                            {
                                if (IONode->Name == ControlNodeNames[j])
                                {
                                    DataSet		*OldDSet = (FakeCNodes[j]->ConnectedTo != NULL)
                                                         ? FakeCNodes[j]->ConnectedTo->DSet
                                                         : VariantDataSet;

                                    IONode->Connect(FakeCNodes[j]->ConnectedTo);

                                    if (SetDataSet)
                                        IONode->PropagateDataSet(IONode->DSet);
                                    else
                                        IONode->DSet = OldDSet;
                                }
                            }
                        }
                    }

                    // Pass #2 - Outputs.
                    IONodes = Outputs;
                }

                Conformalize();
            }
        }

        if (UpdateParentNodeLists)
        {
	        DeleteNodeList(OldInputs);
	        DeleteNodeList(OldOutputs);
        }
        else
        {
	        DeleteNodeList(Inputs);
	        DeleteNodeList(Outputs);

            Inputs = OldInputs;
            Outputs = OldOutputs;
        }

        delete InputHorns;
        delete OutputHorns;
    }

    for (int j = 0; j < ceLast; j++)
    {
        delete FakeCNodes[j];
        FakeCNodes[j] = NULL;
    }

    return NodesReordered;
}


// Create the requested number of input/output nodes using the Bit data set.  The input
//     node names will be "In1", "In2", etc.  All output nodes will be named "Out".
void I2adl::AtomicTemplate(int InputCount, int OutputCount)
{
    if (Inputs == NullNodeList)
        Inputs = NewNodeList(InputCount);
    if (Outputs == NullNodeList)
        Outputs = NewNodeList(OutputCount);

    char    InputLabel[4] = "In0";

    for (int i = 0; i < InputCount; i++)
    {
        InputLabel[2]++;

       	Inputs->Add(new Node(AnsiString(InputLabel), this, BitDataSet));
    }

    for (int i = 0; i < OutputCount; i++)
        Outputs->Add(new Node(*NodestrOut, this, BitDataSet));

    if (NameCode == NAME_ASSIGN)
    {
        Inputs->Items[0]->SetName(*NodestrClock);
        Inputs->Items[1]->SetName(*NodestrHold);
    }

    Conformalize();
}


int I2adl::NumberOfNodesConnected()
{
    NodeList    *List = Inputs;
    int         nConnected = 0;

    for (int nList = 0; nList < 2; nList++)
    {
        for (int i = 0; i < List->Count; i++)
            nConnected += List->Items[i]->ConnectedTo != NULL;

        List = Outputs;
    }

    return nConnected;
}


// Behavioral equality
bool I2adl::Equals(I2adl *Object, bool CompareGraphicInfo, bool TestPrimitive)
{
    if (this == Object)
        return true;

    if (this == NULL || Object == NULL)
        return false;

    if (NameInfo != Object->NameInfo)
        return false;

    if (IsPrimitive != Object->IsPrimitive && TestPrimitive)
        return false;

	if (CompareGraphicInfo)
        if (X != Object->X || Y != Object->Y)
            return false;

    if (!NodeListEqual(Inputs, Object->Inputs, CompareGraphicInfo || !TestPrimitive) ||
    	!NodeListEqual(Outputs, Object->Outputs, CompareGraphicInfo || !TestPrimitive))
        	return false;

    if (AttSystem != Object->AttSystem ||
    	AttResource != Object->AttResource ||
    	AttChildAtt != Object->AttChildAtt ||
    	AttProvidesRes != Object->AttProvidesRes ||
    	AttResourceQty != Object->AttResourceQty)
    {
        return false;
    }

	if (Attributes->Count != Object->Attributes->Count)
    	return false;

    for (int i = 0; i < Attributes->Count; i++)
		if (Attributes->Strings[i] != Object->Attributes->Strings[i])
    		return false;

    return true;
}


// Returns a pointer to the node at position [NodeIndex].  Used to retreive an input or output
//		node; output nodes use the uppper values as index.
//		If NodeIndex in invalid, then NULL is returned.
Node *I2adl::GetNode(int NodeIndex)
{
    if (NodeIndex < 0)
    {
        ErrorTrap(28, "I2adl::GetNode").IntegerValue = NodeIndex;
        return NULL;
    }

    if (NodeIndex < Inputs->Count)
        return Inputs->Items[NodeIndex];

    NodeIndex -= Inputs->Count;

    if (NodeIndex < Outputs->Count)
        return Outputs->Items[NodeIndex];

    ErrorTrap(29, "I2adl::GetNode").IntegerValue =  NodeIndex;

    return NULL;
}


// Returns the input/output node for the NodeCode passed in.
//		NodeCode is either "I99" (input node #99) or "O99" (output node #99).
//		NodeCode numbers are zero based.
//		If NodeCode is invalid, then NULL is returned.
//		The user is not warned about invalid NodeCodes.
Node *I2adl::GetNode(AnsiString NodeCode)
{
    if (NodeCode.Length() >= 2)
    {
        try
        {
            int		NodeNumber = StrToInt(NodeCode.SubString(2, NodeCode.Length() - 1));

			// Is the code "I99" (input node #99)?
            if (NodeCode[1] == 'I')
            {
                if (NodeNumber >= 0 &&
                    NodeNumber < Inputs->Count)
	                    return Inputs->Items[NodeNumber];
            }

            // Is the code "O99" (output node #99)?
            else if (NodeCode[1] == 'O')
            {
                if (NodeNumber >= 0 &&
                    NodeNumber < Outputs->Count)
	                    return Outputs->Items[NodeNumber];
            }
        }
        catch(...)
        {
            // "I" or "O" was not followed by a number.
        }
    }

    return NULL;
}


// Set the object's name and name code (based on the object's name).
//     This method should always be used to change the name.
void I2adl::SetName(AnsiString &_Name)
{
    NameInfo = I2adlStringTable->IconEntry(_Name);
    NameCode = NAME_OTHER;

    for (int j = 0; j < MAX_NAMECODES; j++)
    {
        if (I2adlNames[j] == _Name)
        {
            NameCode = (NameCodes) j;
            break;
        }
    }
}


// Returns true if the object has one or more variant nodes.
bool I2adl::IsVariant()
{
    if (Inputs->IsDataSetUsed(VariantDataSet))
        return true;

    if (Outputs->IsDataSetUsed(VariantDataSet))
        return true;

    return false;
}


// Returns true if the object is a global label.
//		Global labels are input/output objects that have a Resource attribute of Global or
//		GlobalIncludeSystem.
//		If AttributeString is NULL (not ResourcestrNull), then the object's Resource
//		attribute will be checked.
bool I2adl::IsGlobalLabel(AnsiString *AttributeString)
{
	if (AttributeString == NULL)
    {
        if (NameCode != NAME_INPUT &&
            NameCode != NAME_OUTPUT)
                return false;

    	AttributeString = AttResource;
    }
    else
    {
    	AttributeString = ResourceStringTable->QString(*AttributeString);
    }

    return (AttributeString == ResourcestrGlobal ||
    		AttributeString == ResourcestrGlobalIS);
}


// Returns true if the object is a constant input.
bool I2adl::IsConstant()
{
    return (NameCode == NAME_INPUT &&
    		Attributes->Values[strConstant] != strNull);
}


// Returns true if the object has a "PromoteToParentObject=False" attribute.
bool I2adl::IsNoPromote()
{
    return (Attributes->Values[strPromoteToParentObject].SubString(1, 1).UpperCase() == "F");
}


// Returns "true" if the object is a privitive EDIF object.
//		Identified by (1) primitive, (2) no System attribute and (3) has EdifFile attribute.
bool I2adl::IsPrimitiveEDIF()
{
	if (IsPrimitive &&
    	AttSystem == SystemstrNull)
    {
    	AnsiString	EdifFileName;

	    GetAttribute(strEdifFile, EdifFileName);

	    return (EdifFileName != strNull);
    }

	return false;
}


// Returns the name of the global label (with or without the system name included).
AnsiString I2adl::GetGlobalLabelName()
{
	// Are we including the system name?
	AnsiString	SystemNamePrefix = strNull;

    if (AttResource == ResourcestrGlobalIS)
    {
    	VivaSystem	*ObjectSystem = GetSystem();

        if (ObjectSystem != NULL)
        	SystemNamePrefix = *ObjectSystem->Name + "_";
        else if (DefaultTargetSystem != NULL)
        	SystemNamePrefix = *DefaultTargetSystem->Name + "_";
    }

    // Add in the proper node name.
	if (NameCode == NAME_INPUT)
    {
    	if (Outputs->Count > 0)
			return (SystemNamePrefix + *Outputs->Items[0]->Name);
    }
    else if (NameCode == NAME_OUTPUT)
    {
    	if (Inputs->Count > 0)
			return (SystemNamePrefix + *Inputs->Items[0]->Name);
    }

   	return strNull;
}


// Disconect all the input/output nodes on the object.
void I2adl::DisconnectObject()
{
	Inputs->Disconnect();
    Outputs->Disconnect();
}


// This routine will delete the object after removing it from the project objects list.
//     Warning!  The calling program needs to delete the object's behavior.
void I2adl::DeleteProjectObject()
{
    if (this != NULL)
    {
        ProjectObjects->Remove(this);
        delete this;
    }
}


// Returns the node at the other end of a transport.
Node *I2adl::OtherEnd(Node *_Node)
{
    if (NameCode != NAME_TRANSPORT)
    {
        ErrorTrap(30, "I2adl::OtherEnd;Input count = " + IntToStr(Inputs->Count)
        + ";Output count = " + IntToStr(Outputs->Count));
        return _Node;
    }

    if (Inputs->Items[0] == _Node)
        return Outputs->Items[0];
    else
        return Inputs->Items[0];
}


// Change the data set of nodes to the data set of the node passed.
void I2adl::ChangeDataSets(DataSet *DSet)
{
    // Only Input, Output, Junction and Transport objects are changed.
    if (NameCode == NAME_INPUT ||
        NameCode == NAME_OUTPUT ||
        NameCode == NAME_JUNCTION ||
        NameCode == NAME_TRANSPORT)
    {
        Inputs->ChangeDataSets(DSet);
        Outputs->ChangeDataSets(DSet);
    }
}


// Change the data set of non control nodes to the data set of the node passed.
//     Only Input, Output, Junction and Transport objects are changed.
void I2adl::ChangeDataSets(Node *_Node)
{
    ChangeDataSets(_Node->DSet);
}


// Add an output node to the Transport and connect it to the NodeToConnect.
void I2adl::AddOutput(Node *NodeToConnect)
{
    // Persistence requires all of the data sets on a Transport to match.
    Node    *NewOutputNode = new Node(*NodestrOut, this, Inputs->Items[0]->DSet);

    AddNode(&Outputs, NewOutputNode);
    NewOutputNode->Connect(NodeToConnect);
}


// Get the attribute and return its value (string).
void __fastcall I2adl::GetAttribute(AnsiString Attribute, AnsiString &Value)
{
         if (Attributes->IndexOfName(Attribute) > -1)  Value = Attributes->Values[Attribute];
    else if (Attribute == strSystem)                   Value = *AttSystem;
    else if (Attribute == strResource)                 Value = *AttResource;
    else if (Attribute == strResourceQty)              Value = IntToStr(AttResourceQty);
    else if (Attribute == strChildAttribute)           Value = *AttChildAtt;
    else if (Attribute == strProvidesResource)         Value = *AttProvidesRes;
    else                                               Value = strNull;
}


// Get the full tree view name of the object (with number of input/output nodes).
AnsiString I2adl::GetExtendedName()
{
    // Add preceeding space so the object names will sort numerically.
    AnsiString    InputCount  = (Inputs->Count  < 10) ? strSpace + IntToStr(Inputs->Count)
                                                      :            IntToStr(Inputs->Count);
    AnsiString    OutputCount = (Outputs->Count < 10) ? strSpace + IntToStr(Outputs->Count)
                                                      :            IntToStr(Outputs->Count);

    return NameInfo->Name + "(" + InputCount + "," + OutputCount + ")";
}


// Set the attribute to the Value passed (Attribute & Value in one string)
void __fastcall I2adl::SetAttribute(AnsiString &AttributeValue, bool UpdateExisting)
{
    int    Loc = AttributeValue.Pos("=");
    
    if (Loc < 2)
    {
        // No "=" delimiter.
        Attributes->Add(AttributeValue);
    }
    else
    {
        // Separate out the attribute name from the attribute value.
        AnsiString Key = AttributeValue.SubString(1, Loc - 1);
        AnsiString Value = AttributeValue.SubString(Loc + 1,
                AttributeValue.Length()- Loc);
        // Separate out the attribute name from the attribute value.
        SetAttribute(Key, Value, UpdateExisting);
    }
}


// Set the attribute to the Value passed (string).
//     Existing attribute values will only be changed if UpdateExisting is true.
void __fastcall I2adl::SetAttribute(AnsiString &Attribute, AnsiString &Value, bool UpdateExisting)
{
    if (Attribute == strSystem)
    {
        if (UpdateExisting || AttSystem == SystemstrNull)
            AttSystem = SystemStringTable->QString(Value);
    }
    else if (Attribute == strResource)
    {
        if (UpdateExisting || AttResource == ResourcestrNull)
            AttResource = ResourceStringTable->QString(Value);
    }
    else if (Attribute == strResourceQty)
    {
        if (UpdateExisting || AttResourceQty == 1)
        {
            try
            {
                AttResourceQty = StrToInt(Value);
            }
            catch(...)
            {
                // Invalid resource quantity.
                ErrorTrap(107, VStringList(NameInfo->Name, Value));

                AttResourceQty = 1;
            }

            if (AttResourceQty < 0)
            {
                // Negative resource quantity.
                ErrorTrap(108, NameInfo->Name).IntegerValue =  AttResourceQty;
                
                AttResourceQty = 1;
            }
        }
    }
    else if (Attribute == strChildAttribute)
    {
        if (UpdateExisting || AttChildAtt == SystemstrNull)
        {
            AttChildAtt = SystemStringTable->QString(Value);

        	// Is the new ChildAttribute in the proper format?
		    ExtractChildAttributes();
        }
    }
    else if (Attribute == strProvidesResource)
    {
        if (UpdateExisting || AttProvidesRes == ResourcestrNull)
            AttProvidesRes = ResourceStringTable->QString(Value);
    }
    else
    {
        if (!UpdateExisting)
            if (Attributes->IndexOfName(Attribute) != -1)
                return;

        Attributes->Values[Attribute] = Value;
    }
}


// Set the attribute to the Value passed (int)
void __fastcall I2adl::SetAttribute(AnsiString Attribute, int Value, bool UpdateExisting)
{
    AnsiString		AValue = IntToStr(Value);
    SetAttribute(Attribute, AValue, UpdateExisting);
}


void I2adl::SetAttribute(char *cAttribute, AnsiString &Value)
{
    AnsiString		Attribute = cAttribute;

    SetAttribute(Attribute, Value);
}


// Clear all of the attributes strings.
void I2adl::ClearAttributes()
{
    Attributes->Clear();

    AttSystem      = SystemstrNull;
    AttChildAtt    = SystemstrNull;
    AttResource    = ResourcestrNull;
    AttResourceQty = 1;
    AttProvidesRes = ResourcestrNull;
}


// Replace the attributes on "this" object with Object's attributes.
void I2adl::ReplaceAttributes(I2adl *Object)
{
    ClearAttributes();

    Attributes->AddStrings(Object->Attributes);

    AttSystem      = Object->AttSystem;
    AttChildAtt    = Object->AttChildAtt;
    AttResource    = Object->AttResource;
    AttResourceQty = Object->AttResourceQty;
    AttProvidesRes = Object->AttProvidesRes;
}


// Merge the object's Attribute string lists together.
//		New attributes are added.  Existing attributes remain unchanged (retain existing).
//		If CopyNodeNames is true, then the node names will also be copied into the object.
//		If CalculateAtt is true, then all of the attribute macros on the merged object will
//		be calculated.
void I2adl::MergeAttributes(I2adl *Object, bool CopyNodeNames, bool CalculateAtt)
{
    for (int i = 0; i < Object->Attributes->Count; i++)
    {
        AnsiString    AttributeName = Object->Attributes->Names[i];
        AnsiString    AttributeValue = Object->Attributes->Values[AttributeName];

        SetAttribute(AttributeName, AttributeValue, false);
    }

    if (AttChildAtt    == SystemstrNull)    AttChildAtt    = Object->AttChildAtt;
    if (AttSystem      == SystemstrNull)    AttSystem      = Object->AttSystem;
    if (AttResource    == ResourcestrNull)  AttResource    = Object->AttResource;
    if (AttResourceQty == 1)                AttResourceQty = Object->AttResourceQty;
    if (AttProvidesRes == ResourcestrNull)  AttProvidesRes = Object->AttProvidesRes;

	// Are we also copying the node names?
    //     We cannot copy node names on GND and VCC objects because they are wrong in the
    //     system description files.  Also, Inputs and Outputs need to keep user entered names.
    if (CopyNodeNames &&
    	NameCode != NAME_INPUT &&
    	NameCode != NAME_OUTPUT &&
    	NameCode != NAME_GND &&
    	NameCode != NAME_VCC)
    {
    	// Pass #1 - Inputs.
        NodeList	*IONodes = Inputs;
        NodeList	*ObjectIONodes = Object->Inputs;

        for (int IO = 0; IO < 2; IO++)
        {
        	if (IONodes->Count == ObjectIONodes->Count)
            	for (int i = 0; i < IONodes->Count; i++)
                	IONodes->Items[i]->SetName(*ObjectIONodes->Items[i]->Name);

            // Pass #2 - Outputs.
            IONodes = Outputs;
            ObjectIONodes = Object->Outputs;
        }
    }

    if (CalculateAtt)
	    CalculateAttributes();
}


// Calculate all of the attribute macros on the object.
void I2adl::CalculateAttributes()
{
    AnsiString	Attribute;

    for (int i = 0; i < Attributes->Count; i++)
    {
        Attribute = Attributes->Strings[i];

        // The following allows constant binary strings to be calculated at run time
        //		(instead of compile time).
        if (Attribute.Pos(strConstant + "=") != 1)
        {
            AttCalculator->CalculateAttribute(Attribute, NULL, this);
            Attributes->Strings[i] = Attribute;
        }
    }

    Attribute = *AttSystem;
    AttCalculator->CalculateAttribute(Attribute, NULL, this);
    AttSystem = SystemStringTable->QString(Attribute);

    Attribute = *AttChildAtt;
    AttCalculator->CalculateAttribute(Attribute, NULL, this);
    AttChildAtt = SystemStringTable->QString(Attribute);

    Attribute = *AttResource;
    AttCalculator->CalculateAttribute(Attribute, NULL, this);
    AttResource = ResourceStringTable->QString(Attribute);

    Attribute = *AttProvidesRes;
    AttCalculator->CalculateAttribute(Attribute, NULL, this);
    AttProvidesRes = ResourceStringTable->QString(Attribute);
}


// Inherit an IO horn name send from a parent composite IO horn (if existent):
//		If the main node has a displayable name, then prepend the inherited name to the existent
//		name.  Otherwise, replace existing name.
AnsiString& I2adl::InheritHornName()
{
    Node			*namenode = (NameCode == NAME_INPUT) ? Outputs->Items[0]
                                                         : Inputs->Items[0];
    AnsiString		newname = *namenode->Name;
    AnsiString		&AttributeName = (NameCode == NAME_INPUT) ? strInputHornName
														: strOutputHornName;
    AnsiString		parentname = Attributes->Values[AttributeName];

    if (namenode->ShowNodeName())
    {
        if (parentname != strNull)
            newname = parentname + "." + *namenode->Name;
        else
            newname = *namenode->Name;
    }
    else
    {
        if (parentname != strNull)
            newname = parentname;
    }

    namenode->SetName(newname);

    return AttributeName;
}


// Pass the ChildAttribute and System attribute strings onto child objects.
//		Also, composite Input/Output objects pass down their node name.
//		If JustApplyToSelf is true, then the child attribute will just be applied to the
//		Object itself.
void I2adl::PropagateChildAttributes(bool JustApplyToSelf)
{
    // Extract the child attributes into the ChildAttributes list, stripping off the "directed
    //		child objects" strings.
    TStringList		*ChildAttributes = new TStringList;
    TStringList		*DirectedChildObjectss = new TStringList;

    ExtractChildAttributes(ChildAttributes, DirectedChildObjectss);

	// Are we just applying the child attributes to the Object itself?
    if (JustApplyToSelf)
    {
        for (int i = 0; i < DirectedChildObjectss->Count; i++)
            if (DoesChildAttributeApply(DirectedChildObjectss->Strings[i]))
				SetAttribute(ChildAttributes->Strings[i], false);
    }
    else if (Behavior == NULL)
    {
    	// We don't have any child objects.
    }
    else
    {
        // If this is a composite Input/Output object, then we pass down the horn (node) name
        //		by automatically creating a ChildAttribute.
        if (NameCode == NAME_INPUT ||
            NameCode == NAME_OUTPUT)
        {
            AnsiString	&AttributeName = InheritHornName();
            Node		*IONode = (NameCode == NAME_INPUT) ? Outputs->Items[0]
                                                           : Inputs->Items[0];
            AnsiString	NewChildAttribute = (AttChildAtt == SystemstrNull)
            							  ? strNull
                                          : *AttChildAtt + ", ";

            NewChildAttribute += "\"" + AttributeName + "=" + *IONode->Name + "\"";

            SetAttribute(strChildAttribute, NewChildAttribute);
        }

        // Is there a system or child attribute to pass down?
        if (ChildAttributes->Count > 0 || AttSystem != SystemstrNull)
        {
            for (int i = 0; i < Behavior->Count; i++)
            {
                I2adl    *ChildObject = (*Behavior)[i];

                if (ChildObject == NULL ||
                    ChildObject->NameCode == NAME_TRANSPORT ||
                    ChildObject->NameCode == NAME_JUNCTION ||
                    ChildObject->ExposerCode != EXPOSER_NOT)
                {
                    continue;
                }

                // Pass down the System attribute (unless already has one).
                if (ChildObject->AttSystem == SystemstrNull)
                    ChildObject->AttSystem = AttSystem;

                // Pass down the Child attribute (unless already has one).
                if (ChildAttributes->Count <= 0  ||
                    ChildObject->AttChildAtt != SystemstrNull)
                        continue;

                if (!ChildObject->IsPrimitive)
                     ChildObject->AttChildAtt = AttChildAtt;

                for (int j = 0; j < ChildAttributes->Count; j++)
                {
                    AnsiString	ChildAttribute = ChildAttributes->Strings[j];
                    AnsiString	DirectedChildObjects = DirectedChildObjectss->Strings[j];

                    // Do not allow a ChildAttribute to change the number of input/output
                    //		nodes on the parent object.  This messes up ExpandObject().
                    if (ChildAttribute.Pos(strPromoteToParentObject) == 1)
                    {
                        if (ChildObject->MemberOf == NULL)
                            ChildObject->MemberOf = Hierarch;

	                	ErrorTrace(89, ChildObject, *AttChildAtt);
                    }

                    // Should this child receive the attribute?
                    else if (ChildObject->DoesChildAttributeApply(DirectedChildObjects))
                        ChildObject->SetAttribute(ChildAttribute);
                }
            }
        }
    }

	delete	ChildAttributes;
    delete	DirectedChildObjectss;
}


// Extract the child attributes and add them onto the end of the ChildAttributes List.
//     Each attribute is in double quotes.  There is normally ", " between each attribute but
//     this is optional.
//     ChildAttributes may be NULL.  Will just display warning messages.
//     If DirectedChildObjectss != NULL, then the "DCO,DCO,DCO:Attribute=Value" syntax will be
//     parsed to stip out the directed child object list ("DCO,DCO,DCO").
void I2adl::ExtractChildAttributes(TStringList *ChildAttributes, TStringList *DirectedChildObjectss)
{
	// Does the object have a child attribute string?
    if (AttChildAtt->Length() <= 0)
    	return;

    // Child attribute strings must start and end with double quotes.
    if (AttChildAtt->Length() <= 2 ||
		AttChildAtt->SubString(1, 1) != "\"" ||
        AttChildAtt->SubString(AttChildAtt->Length(), 1) != "\"")
    {
        ErrorTrace(145, this, *AttChildAtt);
        return;
    }

    // Each pass through the loop extracts one child attribute.
    AnsiString	TempChildAtt = *AttChildAtt;

    while (true)
    {
        // Find the first quote.
        int     FullLength = TempChildAtt.Length();
        int		Quote1 = TempChildAtt.Pos("\"");

        if (Quote1 <= 0 || Quote1 >= FullLength)
        {
            // Because we know the last character is a quote, this must mean we had an
            //     odd number of quotes.
            ErrorTrace(146, this, *AttChildAtt);
            return;
        }

        // Find the second quote.
        int		Quote2 = TempChildAtt.SubString(Quote1 + 1, FullLength - Quote1).Pos("\"");

        if (Quote2 <= 1)
        {
            // Because we know the last character is a quote, this must be an empty string.
            ErrorTrace(147, this, *AttChildAtt);
        }
        else
        {
        	// Are we loading the ChildAttributes list?
        	if (ChildAttributes != NULL)
            {
				// Are we parsing the "DCO,DCO,DCO:Attribute=Value" syntax to stip out the list
                //		of directed child objects?
                AnsiString	TChildAtt = TempChildAtt.SubString(Quote1 + 1, Quote2 - 1);

                if (DirectedChildObjectss != NULL)
                {
                	int		ColonPos = TChildAtt.Pos(':');
                	int		EqualPos = TChildAtt.Pos('=');

                    if (ColonPos > 1 &&
                    	ColonPos < EqualPos - 1)
                    {
                    	DirectedChildObjectss->Add(TChildAtt.SubString(1, ColonPos - 1));

                        TChildAtt = TChildAtt.SubString(ColonPos + 1, TChildAtt.Length() -
                        	ColonPos);
                    }
                    else
                    {
                    	DirectedChildObjectss->Add(strNull);
                    }
                }

	            ChildAttributes->Add(TChildAtt);
            }
        }

        // Is Quote2 the last character in the string?
        int		NextStartPosition = Quote1 + Quote2 + 1;

        if (NextStartPosition > FullLength)
            return;

        TempChildAtt = TempChildAtt.SubString(NextStartPosition, FullLength -
            NextStartPosition + 1);
    }
}


// Determine if the DirectedChildObjects string prevents this object from receiving child
//		attributes.
bool I2adl::DoesChildAttributeApply(AnsiString DirectedChildObjects)
{
	// If the string is empty, then there are no restrictions.
	if (DirectedChildObjects == strNull)
	   	return true;

    // Parse out all of the object names from the "DCO,DCO,DCO" syntax.
    TStringList		*DirectedChildObjectList = ParseString(DirectedChildObjects, ",");

    for (int i = 0; i < DirectedChildObjectList->Count; i++)
    {
    	// Is our object name in the list?
        if (DirectedChildObjectList->Strings[i] == NameInfo->Name)
        {
            delete DirectedChildObjectList;

			return true;
        }
    }

    delete DirectedChildObjectList;

    return false;
}


// See if OK to connect a variant to this node with type Bit.
//  Illegal if node connects with a splitter.
//  Returns try if signal is split.
bool SplitNode(Node *InputNode);

bool SplitTransport(NodeList *XPorts)
{
    for (int i = 0; i < XPorts->Count; i++)
    {
        Node *Connection = (Node *) XPorts->Items[i]->ConnectedTo;

        if (Connection == NULL)
            continue;

        if (SplitNode(Connection))
            return true;
    }

    return false;
}


// See if OK to connect a variant to this node with type Bit.
//     Illegal if node connects with a splitter.
//     Returns true if signal is split.
bool SplitNode(Node *InputNode)
{
    I2adl    *Owner = InputNode->Owner;

    if (Owner->IsVisited == IsVisitedKey)
        return true;

    Owner->IsVisited = IsVisitedKey;

    if (Owner->ExposerCode == EXPOSER_IN)
        return true;

    if (Owner->NameCode == NAME_JUNCTION)
        return SplitTransport(Owner->Outputs);

    I2adlList    *Behavior = Owner->Behavior;

    if (Behavior == NULL)
        return false;

    int    NodeIndex = Owner->Inputs->IndexOf(InputNode);

    if (NodeIndex < 0)
        return false;

    TStringList    *InputHorns = Behavior->GetInputs();
    Node           *Connection = NULL;

    if (NodeIndex < InputHorns->Count)
    {
        I2adl    *Object = (I2adl *) InputHorns->Objects[NodeIndex];

        Connection = Object->Outputs->Items[0]->ConnectedTo;
    }

    delete InputHorns;

    if (Connection == NULL)
        return false;

    return SplitTransport(Connection->Owner->Outputs);
}


// What we need here is a unique footprint taking into account collectors and exposers
AnsiString I2adl::GetKeyString(bool ForCompile)
{
    static FastString string;

    if (ForCompile && (NameCode == NAME_LINKIN || NameCode == NAME_LINKOUT))
        return strNull;

    // Skip transports, exposers, collectors and casts because they morph.
    if (ExposerCode != EXPOSER_NOT ||
        NameCode == NAME_TRANSPORT ||
        NameCode == NAME_SYSCAST ||
        NameCode == NAME_CASTOUT)
    {
        return strNull;
    }

    string.Pos = 0;

    string.Add(NameInfo->Name);
    string.Add((char) 1);  // Ensures an exact match on each piece of the KeyString.

    if (AttSystem != SystemstrNull)
    {
        string.Add(*AttSystem);
	    string.Add((char) 2);
    }

    if (AttChildAtt != SystemstrNull)
    {
        string.Add(*AttChildAtt);
	    string.Add((char) 3);
    }

    if (IsPrimitive)
        string.Add((char) 4);

    for (int i = 0; i < Inputs->Count; i++)
    {
        Node        *INode = ((Node *) Inputs->Items[i]);
        DataSet     *DSet = INode->DSet;

        if (DSet == VariantDataSet)
        {
        	// Variant data sets cause a different expansion.
            if (INode->ConnectedTo == NULL ||
               	INode->ConnectedTo->DSet == VariantDataSet)
               	return strNull;
        }

        string.Add(DSet->Name);
        string.Add((char) 5);

        // This one has to be separate because of the exposers.
        if (INode->ConnectedTo != NULL)
        {
        	string.Add(INode->ConnectedTo->DSet->Name);
        	string.Add((char) 6);
        }
    }

    for (int i = 0; i < Outputs->Count; i++)
    {
        Node        *INode = ((Node *) Outputs->Items[i]);
        DataSet     *DSet = INode->DSet;

        // We need the data set name because of collectors.
        string.Add(DSet->Name);

        // Unconnected nodes cause a different expansion.
        if (INode->ConnectedTo == NULL)
	        string.Add((char) 7);
        else
			string.Add((char) 8);
    }

    return AnsiString(string.String);
}


DataSet *InputVariantDS;  // To do:  Move to Globals.h.

// Not a VariantDataSet Exposer Search ProjectObjects for Equivalancy.
//     First search ProjectObjects for explicite match.
//     If none found then use Variant Behavior to try to Synthesize one.
//
// This function returns two global values:
//		DownwardCompatibleCount gives the number of project objects with downward compatible
//		input data sets.  If this count is zero, then this object is the only possible match.
//		For Variant primitive objects, InputVariantDS returns the polymorphic input DataSet.
#define  BASE_AGREEMENT 1000000
I2adl *I2adl::ResolveObject(bool DiagnoseAmbiguity, bool UseHash, AnsiString *ParentName,
	bool UseCompileTimeResolutionRules)
{
	// Initalized global variables.
    InputVariantDS = BitDataSet;
    DownwardCompatibleCount = 0;

    if (ExecuteType == EXECUTE_COM)
        return this;

    if (UseHash)
    {
        int         found = ResolvedObjects->IndexOf(GetKeyString(false));

        if (found > -1)
            return (I2adl *) ResolvedObjects->Objects[found];
    }

    I2adl           *ReturnObject = NULL;
    int             BestAgreement = -100;
    bool            Ambiguous = false;
    SystemString    *AssignedSystemName = AttSystem;

    if (InSystem != NULL)
        AssignedSystemName = InSystem->Name;

    // To find the best fit match, check every project object that has the same name, and the
    //		same number of Inputs and Outputs.  If equal, then arbitrate.

	// ProjectObjects->KeyList is a list of ProjectObjects, all grouped by
    //		name + input count + output count.
    VList			*list = MyProject->GetOverloadList(this);

    if (list != NULL)
	{
        for (int i = 0; i < list->Count; i++)
        {
            I2adl	*Object = (I2adl *) list->Items[i];

            // If we have not done so before, check for node name misalignment.
            if (Warned4061 == false && Object != this)
            {
                // Pass #1 - Inputs
                NodeList    *IOList1 = Inputs;
                NodeList    *IOList2 = Object->Inputs;

                for (int IO = 0; IO < 2; IO++)
                {
                    if (IOList1->Count > 1)
                    {
                        for (int i = 0; i < IOList1->Count; i++)
                        {
                            Node    *node1 = IOList1->Items[i];
                            Node    *node2 = IOList2->Items[i];

                            if (node1->Name == node2->Name ||  // For speed.
								node1->Name->UpperCase() == node2->Name->UpperCase())
                            {
                            	// OK!  Node names match.
                                continue;
                            }
                            else if (node1->IsControlNode() ||
                            		 node2->IsControlNode())
                            {
                            	// Error!  Control nodes must match.
                            }
                            else if (!node1->ShowNodeName() ||
                            		 !node2->ShowNodeName())
                            {
                            	// OK!  Changed a default node name.
                               	continue;
                            }
                            else
                            {
                            	// Node names do not match.  To prevent thousands of messages,
                                //		only report if the name appears on another node.
                                bool	ExistsOnAnotherNode = false;

                                for (int i = 0; i < IOList2->Count; i++)
                                {
                                    Node    *node3 = IOList2->Items[i];

                                    if (node1->Name->UpperCase() == node3->Name->UpperCase())
                                    {
                                        ExistsOnAnotherNode = true;
                                        break;
                                    }
                                }

                                if (!ExistsOnAnotherNode)
                                    continue;
                            }

                            // Inform the user that the nodes names don't match.
                            Warned4061 = true;

                            if (ParentName == NULL)
                                ErrorTrace(4061, this, VStringList(*node1->Name, *node2->Name));
                            else
                                ErrorTrace(4074, this, VStringList(*node1->Name, *node2->Name,
                                	NameInfo->Name));
                        }
                    }

                    // Pass #2 - Outputs
                    IOList1 = Outputs;
                    IOList2 = Object->Outputs;
                }
            }

            int			Agreement = BASE_AGREEMENT + 1000;
            DataSet		*MaxInputVariantDS = BitDataSet;

            // If the system names match, then award 25 points.
            if (Object->AttSystem == AssignedSystemName)
            {
                // Don't remove (used for debugging).
                // if (Object->AttSystem != SystemstrNull)
                    Agreement += 25;
            }
            else if (Object->AttSystem != SystemstrNull)
            {
                if (AssignedSystemName != SystemstrNull)
                    continue;  // System names don't match.

                // If we don't have a default target system, then every system is a potential
                //     match for a blank system.
                if (DefaultTargetSystem != NULL)
                {
                    if (Object->AttSystem != DefaultTargetSystem->Name)
                        continue;  // System name doesn't match the default.

                    // The following changes the Viva compiler to select composite objects in
                    //     the default target system over matching on a blank system.
                    if (UseCompileTimeResolutionRules ||
                    	TimedEventType == etCompile)
	                    	Agreement += 50;
                }
            }

            NodeList	*nodes;
            NodeList	*compnodes;

            if (Inputs->Count == 0)
			{
	            nodes = Outputs;
    	        compnodes = Object->Outputs;
            }
            else
            {
	            nodes = Inputs;
    	        compnodes = Object->Inputs;
            }

            // The following fixes an access violation when an old file has been read in
            //     (control nodes are being added to the object).
            if (Inputs->Count < Object->Inputs->Count)
            	goto Reject;

            for (int j = 0; j < compnodes->Count; j++)
            {
                Node       *CheckNode = compnodes->Items[j];
                DataSet    *CheckDSet = CheckNode->DSet;
                Node       *InputNode = nodes->Items[j];
                DataSet    *InputNodeDSet = InputNode->DSet;

                if (NameCode == NAME_TRANSPORT)
                {
                    // Attaboy for communication system composite transport node name match.
                    if (InputNode->Name == CheckNode->Name)
                        Agreement += 2;

                    continue;
                }

                // If the object is connected to a transport, then consider the data set on the
                //     transport to be more accurate.
                if (InputNode->ConnectedTo != NULL)
                {
                    DataSet		*ConnectedDataSet = InputNode->ConnectedTo->DSet;

                    if (ConnectedDataSet != InputNodeDSet &&
                        ConnectedDataSet != VariantDataSet)
                    {
                        // If we are compiling, then warn the user that their non Variant data
                        //		set is being changed.
                        if (!CancelCompile &&
                            InputNodeDSet != VariantDataSet &&
                            InputNodeDSet != SGDataSets[sdsIDispatch])
                        {
                            ErrorTrace(143, this, VStringList(IntToStr(j), InputNodeDSet->
                            	Name));

                            if (EnforceVariantRes)
                            {
								CancelCompile = true;
    	    	                return NULL;
                            }
                        }

                        InputNodeDSet = ConnectedDataSet;
                    }
                }

                // Are the data sets an exact match or an unknown control node?
                if (InputNodeDSet == CheckDSet)
                {
                    Agreement += 10000;
                    continue;
                }

                // Check for Variant compatibility.  Variants don't work on the NULL data set.
                if (InputNodeDSet == NULLDataSet)
                    goto Reject;

                if (CheckDSet == VariantDataSet)
                {
                    if (Object->IsPrimitive
                    	&& MaxInputVariantDS->BitLength < InputNodeDSet->BitLength)
                    {
                        MaxInputVariantDS = InputNodeDSet;
                    }

                    if (InputNodeDSet == BitDataSet)
                    {
                        IsVisitedKey++;

                        if (SplitNode(CheckNode) && Agreement >= BASE_AGREEMENT)
                            Agreement -= BASE_AGREEMENT;
                    }

                    continue;
                }

                if (InputNodeDSet == VariantDataSet)
                {
                    // It is ok to put a variant into a List object.
                    if (Agreement >= BASE_AGREEMENT)
                        Agreement -= BASE_AGREEMENT;

                    continue;
                }

                goto Reject;
            }

            // Attaboy for UIXCPU composite transport node name match.
            if (NameCode == NAME_TRANSPORT)
	            for (int j = 0; j < Object->Outputs->Count; j++)
                    if (Outputs->Items[j]->Name == Object->Outputs->Items[j]->Name)
                        Agreement += 2;

            if (Agreement < BASE_AGREEMENT)
                DownwardCompatibleCount++;

            if (BestAgreement > Agreement)
                continue;

            if (BestAgreement == Agreement)
            {
                if (ReturnObject->AttSystem != Object->AttSystem)
                    continue;

                Ambiguous = true;
                continue;
            }

            InputVariantDS = MaxInputVariantDS;
            BestAgreement = Agreement;
            ReturnObject = Object;
            Ambiguous = false;

            Reject:
        }
    }

    // InfoRate doubles as ambiguity flag during read.
    if (Ambiguous)
    {
        // Do not display the message twice for the same object.
        // Display a diagnosis for a given object at most once
        if (!DiagnoseAmbiguity)
        {
            ReturnObject->Ambiguous = ambYes;
        }
        else if (ReturnObject->Ambiguous != ambYesDiagnosed)
        {
        	// Display the ambiguous object warning message.
		    ErrorTrace(3038, this);

            ReturnObject->Ambiguous = ambYesDiagnosed;
        }
    }
    else if (ReturnObject != NULL && ReturnObject->Ambiguous == ambYes)
    {
        ReturnObject->Ambiguous = ambNo;
    }

    if (BestAgreement < BASE_AGREEMENT)
        DownwardCompatibleCount--;

    if (UseHash && ReturnObject != NULL)
    {
        AnsiString      KeyString(GetKeyString(false));

        if (KeyString != strNull)
            ResolvedObjects->AddObject(KeyString, (TObject *) ReturnObject);
    }

    // Replace Object will change I2adl Interface with the correct Node Types
    return ReturnObject;
}


void I2adl::CreateShape()
{
    // Create the proper MyTShape.
    if (NameCode == NAME_TRANSPORT)
    {
        I2adlShape = new MyTShapeTransport(this);
    }
    else
    {
        // Does the I2adl ref still match the I2adl def?
        I2adl	*Resolver = ResolveObject(false, false, &CurrentSheet->NameInfo->Name, true);

        if (Resolver == NULL)
        {
        	Behavior = NULL;
        }
        else
        {
        	Behavior = Resolver->Behavior;
            IsPrimitive = Resolver->IsPrimitive;
        }

        I2adlShape = new MyTShapeI2adl(this);
    }
}


VivaSystem *I2adl::GetSystem()
{
    if (InSystem != NULL)
        return InSystem;

    if (AttSystem == SystemstrNull)
        return NULL;

    AnsiString	SystemAttribute = *AttSystem;

    // Calculate the attribute.
    AttCalculator->CalculateAttribute(SystemAttribute, NULL, this);

    return BaseSystem->GetSystem(SystemAttribute);
}


// Initializes top, left, width, height properties of a transport and nodes:
void I2adl::InitTransport(int X, int Y, bool Horizontal, int Length, bool Forward
	, bool UseMovingNode)
{
    MyTShapeTransport    *XShape = ((MyTShapeTransport *) I2adlShape);

    // Localize Moving and Stationary node's shapes:
    Node    *InputNode;
    Node    *OutputNode;

    if (UseMovingNode)
    {
        InputNode = XShape->StationaryNode;
        OutputNode = XShape->MovingNode;
    }
    else
    {
        InputNode = Inputs->Items[0];
        OutputNode = Outputs->Items[0];
    }

    XShape->IsHorizontal = Horizontal;

    if (Horizontal)
    {
        XShape->Height = ViewScale;
        XShape->Width = Length;

        // No offset for Top:
        InputNode->Top = 0;
        OutputNode->Top = 0;

        if (Forward) // Running from left to right
        {
            InputNode->Left = 0;
            OutputNode->Left = max(Width - ViewScale, 0);
        }
        else // Running from right to left
        {
            InputNode->Left = max(Width - ViewScale, 0);
            OutputNode->Left = 0;
        }
    }
    else // Vertical
    {
        XShape->Height = Length;
        XShape->Width = ViewScale;

        // No offset for Left:
        InputNode->Left = 0;
        OutputNode->Left = 0;

        if (Forward) // Running from top to bottom
        {
            InputNode->Top = 0;
            OutputNode->Top = max(Height - ViewScale, 0);
        }
        else // Running from bottom to top
        {
            InputNode->Top = max(Height - ViewScale, 0);
            OutputNode->Top = 0;
        }
    }

    if (Forward)
    {
        XShape->Left = X;
        XShape->Top = Y;
    }
    else
    {
        XShape->Left = X - XShape->Width + ViewScale;
        XShape->Top = Y - XShape->Height + ViewScale;
    }
}


// Returns the size of the Object's nodes.
int I2adl::NodeSize()
{
    if (this != NULL)
    	if (NameCode == NAME_TRANSPORT ||
        	NameCode == NAME_JUNCTION)
	    		return ViewScale;

    return ViewScale * 1.41;
}


AnsiString I2adl::QualifiedName(bool SingleLevel)
{
    VDismember	*_Parent = MemberOf;
    AnsiString	Diagnasty = (SingleLevel) ? strNull
    									  : AnsiString("object ");

    Diagnasty += NameInfo->Name
    		  + " (" + Attributes->Values[strDocumentation]
    		  + " " + IntToStr(XLoc)
              + "," + IntToStr(YLoc)
              + ")";

    if (_Parent == NULL && CompressBaseList != FunctionDescription)
        _Parent = ParentI2adlObject->MemberOf;

    while (!SingleLevel &&
    	   _Parent != NULL)
    {
        if (!_Parent->Equals(_Parent->MemberOf))
        {
        	// The following prevents an object on a wip sheet from appearing as a project
            //		object.  Caused by FindInstances() and before MyProject->Wip is loaded.
            if (_Parent->Documentation == SystemstrNull &&
                _Parent->XLoc == 0 &&
                _Parent->YLoc == 0 &&
                _Parent->SourceI2adl != NULL &&
               (_Parent->SourceI2adl->WipSheetCode != wsNotAWipSheet ||
            	MyProject->Wip->IndexOf(_Parent->SourceI2adl) > -1))
            {
                Diagnasty += "    in " + (*_Parent->MemberName);
            }
            else
            {
                Diagnasty += " of " + _Parent->QualifiedName();
            }
        }

        _Parent = _Parent->MemberOf;
    }

    return Diagnasty;
}


// Returns the object's input data sets in a formated string.
AnsiString I2adl::InputDataSets(bool IndicateDisconnect)
{
    bool		FirstNode = true;
    AnsiString	InputDataSets = strNull;
    NodeList	*IONodes = (NameCode == NAME_INPUT) ? Outputs  // Input object
                                                    : Inputs;

    for (int i = 0; i < IONodes->Count; i++)
    {
        Node	*IONode = IONodes->Items[i];

        if (IONode->DSet == NULL)
        	continue;

        if (FirstNode)
        {
            FirstNode = false;
            InputDataSets = "Input data sets: ";
        }
        else
        {
            InputDataSets += ", ";
        }

        if (IONode->ConnectedTo == NULL)
        {
            InputDataSets += IONode->DSet->Name;
            
            if (IndicateDisconnect)
            	 InputDataSets += " (discon)";
        }
        else
        {
            InputDataSets += IONode->DSet->Name;

            // If the transport's data set if different, then load it too.
            if (IONode->DSet != IONode->ConnectedTo->DSet)
	            InputDataSets += " (" + IONode->ConnectedTo->DSet->Name + ")";
        }
    }

    return InputDataSets;
}


QWORD GetMask(int BreakerIndex)
{
	QWORD	Mask = 0;

	for (int i = 0; i < Breakers[BreakerIndex]; i++)
    	Mask += QWORD(1) << i;

	return Mask;
}


QWORD GetCompressionCode(int Index, int &BitLength)
{
	QWORD		bitcode = 0;
    int	 		breakerindex = 1;
    QWORD		mask;

   	mask = GetMask(breakerindex);

    for (int i = 0; i < Index; i++)
    {
    	bitcode += (1 << Breakers[breakerindex - 1]);

        if ((bitcode & mask) == mask)
        {
            breakerindex++;

            if (breakerindex >= HighestBreakerIndex)
            {
                ErrorTrap(4080);
				BitLength = 0;
            	return 0;
            }

            mask = GetMask(breakerindex);
        }
    }

	BitLength = Breakers[breakerindex];
	return bitcode;
}


int AddToTokenList(const AnsiString &NewEntry)
{
	int 	index = TokenList->IndexOf(NewEntry);

    if (index == -1)
    {
		if (BuiltTokenList)
        	index = TokenList->AddObject(NewEntry, (TObject *) TokenList->Count);
		else
		    index = TokenList->AddObject(NewEntry, (TObject *) 1);
    }
    else if (!BuiltTokenList)
		TokenList->Objects[index] = (TObject *) (int(TokenList->Objects[index]) + 1);

	if (BuiltTokenList)
	    return int(TokenList->Objects[index]);
    else
    	return index;
}


// Appends CompressionListIndex at the byte and bit position specified.  CompressionListIndex
//		is the ordinal position of a string token within the compression map file
//		("$(DirName:Build)CompressionMap_[project name].txt").
//		Appends only the necessary number of bits; this is part of a bit compression routine.
void I2adl::AddToEdifID(int CompressionListIndex, int &BitPos, int &BytePos)
{
	int		bitlength = 0;
	QWORD	compressioncode = GetCompressionCode(CompressionListIndex, bitlength);
    int		compressioncodebytepos = 0;
    int		compressioncodebitpos = 0;

    for (int i = 0; i < bitlength; i++)
    {
		int 	currentcompressioncodebyte = ((BYTE *) &compressioncode)[compressioncodebytepos];

        if (currentcompressioncodebyte & (1 << compressioncodebitpos))
			EdifIDTemplate[BytePos] |= BYTE(1 << BitPos);

        if (BitPos >= 7)
        {
        	BitPos = 0;

            // Don't go past the end of the EdifIDTemplate array.  No need to warn the user
            //     because the calling routine will.
            if (BytePos + 1 < EDIF_NAME_BUFFER_LIMIT)
	            BytePos++;
        }
        else
        {
           	BitPos++;
        }

        if (compressioncodebitpos >= 7)
        {
			compressioncodebitpos = 0;
       		compressioncodebytepos++;
        }
        else
        {
	   		compressioncodebitpos++;
        }
    }
}


// Generates the unique, re-usable Edif ID for this object, and assigns it to the EdifID member.
//		Edif ID consists of the name, documentation, and x/y of this object and each member of
//		its compile heirarchy, bottom to top.  It is then compressed to reduce EDIF file size,
//		and to meet the 240-char name limit.  String tokens for name, documentation, and x/y of
//		each heirarchy node are created, compressed, and appended to EdifID, one at a time.
void I2adl::MakeEdifID()
{
	static FastString		str;

	// Add token for this object's info first:
    str.Pos = 0;
    str.Add(NameInfo->Name);
    str.Add('(');
    str.Add(Attributes->Values[strDocumentation]);
    str.Add(' ');
    str.Add(IntToStr(X));
    str.Add(',');
    str.Add(IntToStr(Y));
    str.Add(')');

	AnsiString		newentry(str.String);
	AnsiString		laststring = newentry;
    VDismember		*_Parent = MemberOf;
    int				TimesRepeated = 1;
    TStringList		*TokenList = new TStringList;

    TokenList->Sorted = false;
    TokenList->Duplicates = dupAccept;

	// And now add a node for each hierarchy node:
    if (_Parent == NULL && CompressBaseList != FunctionDescription)
        _Parent = ParentI2adlObject->MemberOf;

    while (_Parent != NULL)
    {
        str.Pos = 0;

		str.Add(_Parent->QualifiedName());

        AnsiString	newentry(str.String);

        if (newentry == laststring)
        {
            TimesRepeated++;
        }
        else
        {
			if (TimesRepeated > 1)
            	laststring = laststring + "*" + IntToStr(TimesRepeated);

            TokenList->Add(laststring);

            TimesRepeated = 1;

            laststring = newentry;
        }

        _Parent = _Parent->MemberOf;
	}

    TokenList->Add(laststring);

    // Now that we have the list of token strings, combine any repeating patterns.
    //		For speed purposes, only check for repeating patterns of size 2 and 3.
    for (int NumberOfTokensInRepeat = 2; NumberOfTokensInRepeat <= 3; NumberOfTokensInRepeat++)
    {
        // to simply the math, LastPossibleRepeat index is 1 based.
    	int		LastPossibleRepeat = TokenList->Count - NumberOfTokensInRepeat * 2 + 1;

    	for (int i = 0; i < LastPossibleRepeat; i++)
        {
        	// Starting with location "i", how many times does this set of tokens repeat?
            TimesRepeated = 1;

            for (int j = i; j < LastPossibleRepeat; j += NumberOfTokensInRepeat)
            {
            	bool	TokensMatch = true;

            	for (int k = 0; k < NumberOfTokensInRepeat; k++)
                {
                	if (TokenList->Strings[j + k] != TokenList->Strings[j + k +
                    	NumberOfTokensInRepeat])
                    {
                    	TokensMatch = false;
                        break;
                    }
                }

                if (!TokensMatch)
                	break;

	            TimesRepeated++;
            }

            if (TimesRepeated > 1)
            {
            	// We found a set of repeating tokens!
                //		Build a composite string and put it into location "i".
                for (int j = 1; j < NumberOfTokensInRepeat; j++)
                	TokenList->Strings[i] = TokenList->Strings[i] + TokenList->Strings[i + j];

                TokenList->Strings[i] = TokenList->Strings[i] + "**" + IntToStr(TimesRepeated);

                // Remove the other strings that were combined.
                int		RepeatBlockSize = NumberOfTokensInRepeat * TimesRepeated;

                for (int j = 1; j < RepeatBlockSize; j++)
                	TokenList->Delete(i + 1);

		    	LastPossibleRepeat = TokenList->Count - NumberOfTokensInRepeat * 2 + 1;
            }
        }
    }

    // Create the actual EDIF ID string.
    int		bitpos = 0;
    int		bytepos = 0;

    memset(&EdifIDTemplate, 0, sizeof(EdifIDTemplate));

	for (int i = 0; i < TokenList->Count; i++)
    {
    	AnsiString	Token = TokenList->Strings[i];

    	// Because pattern data sets can create thousands of identical tokens, break the
        //		trailing TimesRepeated into 2 digit tokens.
        int		TokenLen = Token.Length();
        int		TwoDigitTokenCount = 0;

    	for (int j = TokenLen; j > 2; j -= 2)
        {
        	if (Token[j - 1] < '0' || Token[j] < '0' ||
            	Token[j - 1] > '9' || Token[j] > '9')
                	break;

            // Count the number of trailing 2 digit tokens.
            TwoDigitTokenCount++;
        }

        if (TwoDigitTokenCount <= 0)
        {
	    	AddToEdifID(AddToTokenList(Token), bitpos, bytepos);
        }
        else
        {
	    	AddToEdifID(AddToTokenList(Token.SubString(1, TokenLen - TwoDigitTokenCount * 2)),
            	bitpos, bytepos);

            for (int j = TwoDigitTokenCount; j >= 1; j--)
			    AddToEdifID(AddToTokenList(Token.SubString(TokenLen + 1 - j * 2, 2)), bitpos,
                	bytepos);
        }
    }

    delete TokenList;

	// [BuiltTokenList == true] means that the compression map has been optimized. 
	if (BuiltTokenList)
    {
        // If the Edif Id is too large, then truncate it and warn the user.
        //     Divide by 2 to convert "number of chars" to " number of bytes".
        if (bytepos >= EDIF_NAME_LIMIT / 2)
        {
            bytepos = EDIF_NAME_LIMIT / 2 - 1;
            bitpos = 8;

            ErrorTrace(185, this);
        }

		// Fill the rest of the last byte with 1's, to signify that there are no more codes.
    	for (int i = bitpos; i < 8; i++)
			EdifIDTemplate[bytepos] += BYTE(1 << i);

	    EdifID = new BYTE[bytepos + 2];

		EdifID[0] = byte(bytepos + 1); // Store length in first bytes

	    memcpy(EdifID + 1, EdifIDTemplate, bytepos + 1);
    }
}


// Bring the object to the top of the Z-order.  Also bring its attched transports to the
//     front so their nodes will be visible.
void I2adl::BringShapeToFront()
{
    // Bring the object to the front.
    if (I2adlShape != NULL)
    	I2adlShape->BringToFront();

	// Bring the attached transports to the front.
	if (NameCode == NAME_TRANSPORT)
    	return;

    // Pass #1 Inputs
	NodeList	*IONodes = Inputs;

    for (int IO = 0; IO < 2; IO++)
    {
    	for (int i = 0; i < IONodes->Count; i++)
        {
        	Node	*IONode = IONodes->Items[i];

            if (IONode == NULL ||
            	IONode->ConnectedTo == NULL)
                	continue;

            MyTShapeI2adl	*TransportShape = IONode->ConnectedTo->Owner->I2adlShape;

            if (TransportShape != NULL)
                TransportShape->BringToFront();
        }

        // Pass #2 Outputs
    	IONodes = Outputs;
    }
}


// Assign/unassign the I2adl object to the System passed in.  Any connected Bit Fan objects
//     are assigned/unassigned to the same system.
void I2adl::AssignToSystem(VivaSystem *System)
{
   	// Assign/unassign the object to the system.
	InSystem = System;

	if (System == NULL)
	    AttSystem = SystemstrNull;

    // Assign/unassign any connected Bit Fan exposer/collector objects.
    if (BitFanCode == bfcYes)
    {
    	// Look upstream from the Input nodes.
    	for (int i = 0; i < Inputs->Count; i++)
        {
        	Node	*InputNode = Inputs->Items[i]->ConnectedTo;

            if (InputNode == NULL ||
            	InputNode->DSet->BitLength <= 1)
                	continue;

            Node	*UpstreamNode = InputNode->Owner->Inputs->Items[0]->ConnectedTo;

            // If the upstream object is a Bit Fan collector, then assign/unassign it to the
            //     same system.
            if (UpstreamNode == NULL ||
            	UpstreamNode->Owner->NameCode != NAME_BIT_FAN_COL)
                	continue;

            UpstreamNode->Owner->InSystem = System;
            UpstreamNode->Owner->AttResource = ResourcestrTimeSlice;  // FPGAs don't care.
        }

    	// Look downstream from the Output nodes.
    	for (int i = 0; i < Outputs->Count; i++)
        {
        	Node	*OutputNode = Outputs->Items[i]->ConnectedTo;

            if (OutputNode == NULL ||
            	OutputNode->DSet->BitLength <= 1)
                	continue;

            for (int j = 0; j < OutputNode->Owner->Outputs->Count; j++)
            {
                Node	*DownstreamNode = OutputNode->Owner->Outputs->Items[j]->ConnectedTo;

	            // If the downstream object is a Bit Fan exposer, then assign/unassign it to
                //     the same system.
                if (DownstreamNode == NULL ||
                    DownstreamNode->Owner->NameCode != NAME_BIT_FAN_EXP)
                        continue;

                DownstreamNode->Owner->InSystem = System;
	            DownstreamNode->Owner->AttResource = ResourcestrTimeSlice; // FPGAs don't care.
            }
        }
    }
}


// Assign the I2adl object to the X86 system passed in.
void I2adl::AssignToX86System(VivaSystem *X86System)
{
	AssignToSystem(X86System);

	if (X86System != NULL)
    {
        AttSystem = X86System->Name;
        AttResource = ResourcestrTimeSlice;
    }
}


// Display the external help file for the object.
void I2adl::ShowHelpFile()
{
	if (this != NULL)
	    VivaGlobals::ShowHelpFile(NameInfo->Name, true);
}


// This routine returns the object's compiler ancestry.  It is the same as QualifiedName()
//     except it returns a string pointer and the top most object is listed first.
AnsiString *I2adl::QualifiedNameSpeed()
{
	static AnsiString	Diagnasty;
	static VDismember	*_Parent;

    Diagnasty = NameInfo->Name;
    _Parent = MemberOf;

	if (_Parent == NULL && CompressBaseList != FunctionDescription)
		_Parent = ParentI2adlObject->MemberOf;

	while (_Parent != NULL)
	{
        if (!_Parent->Equals(_Parent->MemberOf));
			Diagnasty = (*_Parent->MemberName) + "->" + Diagnasty;

		_Parent = _Parent->MemberOf;
	}

	return &Diagnasty;
}


// Returns true if the user confirms that the object should be deleted.
bool I2adl::MayObjectBeDeleted()
{
	// Has the object been used?
	//     MatchState  0 checks if the object has been used.
	//     MatchState -1 checks if the object footprint has been used.
	for (int MatchState = 0; MatchState >= -1; MatchState--)
    {
    	// Check project objects.
        for (int i = 0; i < ProjectObjects->Count; i++)
        {
            I2adl    *ProjectObject = (*ProjectObjects)[i];

            if (ProjectObject == this  ||
            	ProjectObject->Behavior == NULL)
                	continue;

            if (ProjectObject->Behavior->GetObject(this, MatchState) != NULL)
            {
                int    ErrorNumber = (MatchState == 0) ? 1
                                                       : 83;

                return (ErrorTrap(ErrorNumber, ProjectObject->EnhancedName()) == mrYes);
            }
        }

        // Check wip sheets.
        for (int i = 0; i < MyProject->Wip->Count; i++)
        {
            I2adl	*WipSheet = (*MyProject->Wip)[i];

            if (WipSheet->Behavior->GetObject(this, MatchState) != NULL)
            {
                int    ErrorNumber = (MatchState == 0) ? 82
                                                       : 84;

                return (ErrorTrap(ErrorNumber, MainForm->WipTree->Items->Item[i + 1]->Text) ==
                	mrYes);
            }
        }
    }

    // Object has not been used.  Confirm the delete with the user anyway.
    return (ErrorTrap(4006) == mrYes);
}


// Returns "true" if the object is a standard primitive object.
bool I2adl::IsStandardPrimitiveObject()
{
	if (!IsPrimitive)
    	return false;

    // Is this an exposer or collector.
    if (ExposerCode != EXPOSER_NOT)
        	return true;

    // Is this a predefined object?
	return (AttSystem == SystemstrNull &&
            NameCode < PREDEFINED_OBJECTS &&
            NameCode != NAME_OTHER);
}


void EnhanceText(AnsiString &Text, AnsiString *Documentation)
{
    Text = Text + " (" + *Documentation + ")";
}


AnsiString I2adl::EnhancedName()
{
    AnsiString    Text = NameInfo->Name;

    // Enhance text if the object has information.
    if (NameCode == NAME_TRANSPORT && Behavior != NULL)
    {
        for (int i = 0; i < Behavior->Count; i++)
        {
            I2adl   *Object = (I2adl *)Behavior->Items[i];
            Node    *ConnectNode;

            if (Object->AttSystem == SystemstrNull)
                continue;

            if (Object->NameCode == NAME_INPUT)
                ConnectNode = Object->Outputs->Items[0];
            else if (Object->NameCode == NAME_OUTPUT)
                ConnectNode = Object->Inputs->Items[0];
            else
                continue;

            EnhanceText(Text,ConnectNode->Name);

            return Text;
        }
    }

    AnsiString    ValueString = Attributes->Values[strDocumentation];

    if (ValueString != strNull)
    {
        EnhanceText(Text,&ValueString);
    }
    else if (NameCode == NAME_INPUT)
    {
        NodeString    *ValueString = Outputs->Items[0]->Name;

        if (ValueString != NodestrIn)
            EnhanceText(Text, ValueString);
    }
    else if (NameCode == NAME_OUTPUT)
    {
        NodeString    *ValueString = Inputs->Items[0]->Name;

        if (ValueString != NodestrOut)
            EnhanceText(Text, ValueString);
    }

    return Text;
}


// Returns the object's external file name so the user can know where the object came from.
AnsiString I2adl::ExternalName()
{
    if (AttSystem != NULL &&
    	AttSystem != SystemstrNull)
	        return " in system " + *AttSystem;

    if (OwningLib != NULL)
        return " in library " + GetShortFileName(*OwningLib);

    if (IsSystemObject)
        return " a system composite object";

	return strNull;
}


void I2adl::GetControlNodes(Node **&Nodes)
{
    NodeList	*IONodes = Inputs;

    for (int IO = 0; IO < 2; IO++, IONodes = Outputs)
    {
        for (int i = 0; i < IONodes->Count; i++)
        {
            Node    *IONode = IONodes->Items[i];

            if (IONode == NULL)
                continue;

            for (int j = 0; j < ceLast; j++)
                if (Nodes[j] == NULL && IONode->Name == ControlNodeNames[j])
                    Nodes[j] = IONode;
        }
    }
}


// Returns a set of flags corresponding to trRight and trLeft.
//     These flags indicate whether transports need to be painted.
int I2adl::NonTransparentEdge()
{
    VivaIcon    *Icon = GetIconInfo();

    if (Icon->Bitmap == NULL)
        return 0;
                                                                // + 1 to round up
	int 		start = (Icon->Bitmap->Width - Icon->OrigWidth + 1) / 2;

    return (Icon->NonTransparentEdge(start) * 2) +
            Icon->NonTransparentEdge(Icon->Bitmap->Width - 1 - start);
}


// Calculate grid snap X values; Transports & Junctions are offset by 1 pixel:
void I2adl::CalculateHorzGridSnap(int& Data)
{
    // Add 1 if a Junction or a Transport:
    int		Offset = (NameCode == NAME_TRANSPORT
                   || NameCode == NAME_JUNCTION);

    Data -= int(ViewScale * Offset * .21);
    AlignToGrid(Data);
    Data += int(ViewScale * Offset * .21);
}


// Returns the text object's font size for the current view scale.
//     The font size is stored in the object and in text base at view scale 5 (old grid snap).
int I2adl::CalculateFontSize()
{
    // If font information is not available (this is a new text object), then default to 8.
    double	FontSizeAt5 = (Attributes->Count < 4) ? 8.0
    								   		      : Attributes->Strings[1].ToDouble();
    int		FontSizeAtVS = FontSizeAt5 * ViewScale / OLD_GRID_SNAP + .5;

    // Don't allow the font size to be zero or the text box will go crazy.
    return max(FontSizeAtVS, 1);
}


// Copies the return value of a function to the appropriate output node, and generates an event:
void I2adl::CopyResultData(BYTE *pVal, int nNode, int Offbounds)
{
    Node    *Output = Outputs->Items[nNode];
    BYTE    *DataPtr = Output->DataPtr;
    bool	changed = false;

    if (DataPtr != NULL)
		changed = OffboundMemCopy(DataPtr, pVal, Output->DSet->BitLength, Offbounds, true);

	if (changed || ExecuteType == EXECUTE_COM
    	|| (NameCode == NAME_INPUT && Widget->WidgetNameCode == wnActiveX))
	    X86MainThread::AddTransportEvents(Output->ConnectedTo);
}


int I2adl::GetLeft()
{
	int 	retval = m_Left * ViewScale;

    CalculateHorzGridSnap(retval);

    return retval;
}


int	I2adl::GetTop()
{
	int 	retval = m_Top * ViewScale;

    AlignToGrid(retval);

    return 	retval;
}


int I2adl::GetXLoc()
{
    return X / ViewScale;
}


int	I2adl::GetYLoc()
{
    return 	Y / ViewScale;
}


void I2adl::SetLeft(int _Left)
{
	m_Left = _Left / ViewScale;
}


void I2adl::SetTop(int _Top)
{
	m_Top = ((_Top + ViewScale * .21) / ViewScale);
}


int I2adl::GetWidth()
{
    if (NameCode == NAME_TRANSPORT || NameCode == NAME_JUNCTION)
		return m_Width * ViewScale;
	else
		return m_Width * ViewScale + int(ViewScale * .41);
}


void I2adl::SetWidth(int _Width)
{
	AlignToGrid(_Width);
	m_Width = _Width / ViewScale;
}


int	I2adl::GetHeight()
{
	return m_Height * ViewScale;
}


void I2adl::SetHeight(int _Height)
{
	AlignToGrid(_Height);
	m_Height = _Height / ViewScale;
}


void I2adl::SetDefaultTextDimensions()
{
	Width = ViewScale * 50;
	Height = max(ViewScale * 3, MIN_MEMO_HEIGHT);
}


// Load the object's TruthTable from either the object's attributes or hard coded values.
//		Only primitive objects can have a TruthTable.  If constants cannot be propagated
//		through the object, then an empty TruthTable will be created.  If the TruthTable
//		already exists, then it won't be reloaded.
void I2adl::LoadTruthTable()
{
	// Has the TruthTable has already been loaded?
	if (TruthTable != NULL)
    	return;

    // Constants can only propagate through primitive objects.  Also, composite objects may
    //		change into primitives.
	if (!IsPrimitive)
		return;

    TruthTable = new TruthTableEntryList(sizeof(TruthTableEntry));

    // Copy the constant propagation attributes from the master I2adl def object (retain
    //		existing).
	I2adl	*I2adlDef = ResolveObject(false, false, NULL, true);

    if (I2adlDef != NULL)
    {
        // The "for" loop is required to maintain the order of the attributes (can't just
        //		use the "Values" property).
    	for (int i = 0; i < I2adlDef->Attributes->Count; i++)
        {
            AnsiString	AttributeName = I2adlDef->Attributes->Names[i];

            if (AttributeName == strConstTruthFile ||
            	AttributeName == strConstTruthTable)
            {
	            AnsiString	AttributeValue = I2adlDef->Attributes->Values[AttributeName];

                SetAttribute(AttributeName, AttributeValue, false);
            }
        }
    }

    // Has the user provided the TruthTable in the attributes?
    //		The "for" loop is required to maintain the order of the attributes (can't just
    //		use the "Values" property).
    bool	FoundConstantFoldingAttributes = false;

    for (int i = 0; i < Attributes->Count; i++)
    {
    	AnsiString	AttributeName = Attributes->Names[i];

        if (AttributeName == strConstTruthFile)
        {
            FoundConstantFoldingAttributes = true;

            FoundObjectEnum		ObjectStatus = foNotFound;

            if (ParseConstTruthFileAtt(Attributes->Values[strConstTruthFile], ObjectStatus))
            {
                // The attribute is valid, but did it contain a truth table for this object?
                if (ObjectStatus == foNotFound &&
                   	Attributes->Values[strConstTruthFile].UpperCase().Pos(str_NONE_) <= 0)
                {
	                ErrorTrace(303, this, Attributes->Values[strConstTruthFile]);
                }
            }
        }
        else if (AttributeName == strConstTruthTable)
        {
            FoundConstantFoldingAttributes = true;

            ParseConstTruthTableAtt(Attributes->Values[strConstTruthTable], false, false);
        }
    }

    // If the user supplied the TruthTable, then skip the hard coded entries.
    if (FoundConstantFoldingAttributes)
    	return;

	// Quickly skip objects that we don't know how to handle.
	if (NameCode == NAME_OTHER)
		return;

    // The hard coded constant truth tables are for Bit primitives only.
    for (int i = 0; i < Inputs->Count; i++)
        if (Inputs->Items[i]->DSet != BitDataSet)
            return;

    // MUXCY - If the "S" node is a constant, then propagate the selected input (DI or CI) to
    //		each output node.
	if (NameCode == NAME_MUXCY ||
		NameCode == NAME_MUXCY_D ||
		NameCode == NAME_MUXCY_L)
	{
		if (Inputs->Count == 3)
		{
            ParseConstTruthTableAtt("X,X,0:I0");
            ParseConstTruthTableAtt("X,X,1:I1");
		}
	}

	// AND - If constant input is zero, then the output will always be 0.
    //     Otherwise, propagate the other input.
    else if (NameCode == NAME_AND ||
    		 NameCode == NAME_MULT_AND)
    {
    	if (Inputs->Count == 2)
        {
        	ParseConstTruthTableAtt("0,X:0");
        	ParseConstTruthTableAtt("X,0:0");
        	ParseConstTruthTableAtt("1,X:I1");
        	ParseConstTruthTableAtt("X,1:I0");
        }
    }

    // OR - If constant input is zero, then the output is the value of the other input.
    //     Otherwise, the output will always be 1.
    else if (NameCode == NAME_OR ||
    		 NameCode == NAME_ORCY)
    {
    	if (Inputs->Count == 2)
        {
        	ParseConstTruthTableAtt("0,X:I1");
        	ParseConstTruthTableAtt("X,0:I0");
        	ParseConstTruthTableAtt("1,X:1");
        	ParseConstTruthTableAtt("X,1:1");
        }
    }

	// INVERT - Pass through the opposite constant.
    else if (NameCode == NAME_INVERT)
    {
    	if (Inputs->Count == 1)
        {
        	ParseConstTruthTableAtt("0:1");
        	ParseConstTruthTableAtt("1:0");
		}
    }

    // XORCY - If constant input is zero, then pass through the other input.
    //     Otherwise, pass through the inverse of the other input.
    else if (NameCode == NAME_XORCY ||
    		 NameCode == NAME_XORCY_D ||
    		 NameCode == NAME_XORCY_L)
    {
    	if (Inputs->Count == 2)
        {
        	ParseConstTruthTableAtt("0,X:I1");
        	ParseConstTruthTableAtt("X,0:I0");
        	ParseConstTruthTableAtt("1,X:INVERT:I1:o0");
        	ParseConstTruthTableAtt("X,1:INVERT:I0:o0");
        }
    }

    // BUFT - If the Enable is zero, then pass through the other input.
    //		If the Enable is one, then pass through a disconect.
    else if (NameCode == NAME_BUFT)
    {
    	if (Inputs->Count == 2)
        {
        	ParseConstTruthTableAtt("0,X:I1");
        	ParseConstTruthTableAtt("1,X:D");
		}
    }

   	// REGCLR - Inputs:  Data, Clock, Enable, Clear
    else if (NameCode == NAME_REGCLR)
    {
        if (Inputs->Count == 4)
        {
            ParseConstTruthTableAtt("0,X,X,X:0");
            ParseConstTruthTableAtt("X,C,X,X:0");
            ParseConstTruthTableAtt("X,X,0,X:0");
            ParseConstTruthTableAtt("X,X,X,1:0");
            ParseConstTruthTableAtt("1,X,1,0:1");
        }
    }

    // FDCE - Inputs:  Clock, Enable, Clear, Data
    else if (NameCode == NAME_FDCE)
    {
        if (Inputs->Count == 4)
        {
            ParseConstTruthTableAtt("C,X,X,X:0");
            ParseConstTruthTableAtt("X,0,X,X:0");
            ParseConstTruthTableAtt("X,X,1,X:0");
            ParseConstTruthTableAtt("X,X,X,0:0");
            ParseConstTruthTableAtt("X,1,0,1:1");
        }
    }

    // FDRE - Inputs:  Data, Enable, Clock, Reset
    else if (NameCode == NAME_FDRE)
    {
        if (Inputs->Count == 4)
        {
            ParseConstTruthTableAtt("0,X,X,X:0");
            ParseConstTruthTableAtt("X,0,X,X:0");
            ParseConstTruthTableAtt("X,X,C,X:0");
            ParseConstTruthTableAtt("X,X,X,1:0");
            ParseConstTruthTableAtt("1,1,X,0:1");
        }
    }

    // FDCPE - Inputs:  Clock, Enable, Clear, Data, Preset
    else if (NameCode == NAME_FDCPE)
    {
        if (Inputs->Count == 5)
        {
            ParseConstTruthTableAtt("C,X,X,X,0:0");
            ParseConstTruthTableAtt("X,0,X,X,0:0");
            ParseConstTruthTableAtt("X,X,1,X,X:0");
            ParseConstTruthTableAtt("X,X,X,0,0:0");
            ParseConstTruthTableAtt("X,1,0,1,0:1");
            ParseConstTruthTableAtt("X,X,X,X,1:INVERT:I2:o0");
        }
    }

    // FDRSE - Inputs:  Data, Enable, Clock, Reset, Set
    else if (NameCode == NAME_FDRSE)
    {
        if (Inputs->Count == 5)
        {
            ParseConstTruthTableAtt("0,X,X,X,0:0");
            ParseConstTruthTableAtt("X,0,X,X,0:0");
            ParseConstTruthTableAtt("X,X,C,X,0:0");
            ParseConstTruthTableAtt("X,X,X,1,X:0");
            ParseConstTruthTableAtt("X,X,X,X,1:INVERT:I3:o0");
            ParseConstTruthTableAtt("1,1,X,0,0:1");
        }
    }

    // FDDRRSE - Inputs:  Data0, Data1, Clock0, Clock1, Enable, Reset, Set
    else if (NameCode == NAME_FDDRRSE)
    {
        if (Inputs->Count == 7)
        {
            ParseConstTruthTableAtt("X,X,X,X,X,1,X:0");
            ParseConstTruthTableAtt("X,X,X,X,X,X,1:INVERT:I5:o0");
            ParseConstTruthTableAtt("X,X,X,X,0,X,0:0");
        }
    }

    // FDDRCPE - Inputs:  Data0, Data1, Clock0, Clock1, Enable, Clear, Preset
    else if (NameCode == NAME_FDDRCPE)
    {
        if (Inputs->Count == 7)
        {
            ParseConstTruthTableAtt("X,X,X,X,X,1,X:0");
            ParseConstTruthTableAtt("X,X,X,X,X,X,1:INVERT:I5:o0");
            ParseConstTruthTableAtt("X,X,X,X,0,X,0:0");
        }
    }

    // SRLC16E - Inputs:  Data, Enable, Clock, Address0, Address1, Address2, Address3
    else if (NameCode == NAME_SRLC16E)
    {
        if (Inputs->Count == 7)
        {
            ParseConstTruthTableAtt("0,X,X,X,X,X,X:0");
            ParseConstTruthTableAtt("X,0,X,X,X,X,X:0");
            ParseConstTruthTableAtt("X,X,C,X,X,X,X:0");
        }
    }
}


// Parse the constant truth file FullAttValue to open the file and extract the attributes.
//		Invalid attributes are reported.  If the object's TruthTable has been created,
//		then valid entries are appended into it.
//		ObjectStatus is updated to indicate if we have found the start/end of the object's
//		attributes in this file.
//		If JustEditFile is true, then do not display any error/warning messages.  Just find
//		the object in the file and allow the user to edit/save the file.  The calling routine
//		can know that no file was displayed by testing (ObjectStatus == foNotFound).
//		If AutoModal is true, then at most only one error/warning message will be issued and
//		it will be in a modal window.
//		Returns "true" if all of the attributes are valid.
//
// ConstTruthFile attributes have the format:
//		ConstTruthFile=<FullyQualifiedFileName> [#comment]
//		or
//		ConstTruthFile=$(DirFile:<DirCode>)<FileName> [#comment]
//		or
//		ConstTruthFile=<None> [#comment]
//
// Note:  This routine is recursive.
//
bool I2adl::ParseConstTruthFileAtt(AnsiString FullAttValue, FoundObjectEnum &ObjectStatus,
	bool JustEditFile, bool AutoModal)
{
    // Strip off the comments by finding the left-most comment character.
    int			CommentPos = FullAttValue.Pos("#");
    AnsiString  FileName = (CommentPos <= 0)
    					 ? FullAttValue.Trim()
                         : FullAttValue.SubString(1, CommentPos - 1).Trim();

    // The "<None>" keyword is not a file.
    if (FileName.UpperCase() == str_NONE_)
    	return true;

    // A file name must be provided.
    if (FileName == strNull)
    {
    	if (!JustEditFile)
        {
            ErrorTrace(301, this, FullAttValue, AutoModal);

            // Only allow one modal error/warning message.
            if (AutoModal)
                ObjectStatus = foFoundEnd;
        }

        return false;
    }

    // Try to open and read in the file.
    FileName = FullPathName(FileName);

    TStringList		*AttributeLines = new TStringList;

    try
    {
        AttributeLines->LoadFromFile(FileName);
    }
    catch(...)
    {
        // Cannot open/read the file.
    	if (!JustEditFile)
        {
            ErrorTrace(302, this, VStringList(FullAttValue, FileName), AutoModal);

            // Only allow one modal error/warning message.
            if (AutoModal)
                ObjectStatus = foFoundEnd;
        }

        delete AttributeLines;

        return false;
    }

    // Read the file until we find the object identification line for our object.
	bool	IsAttributeValid = true;

    // Read and validate each line in the file.
    for (int i = 0; i < AttributeLines->Count; i++)
    {
    	// Extract out the attribute name and value.
    	AnsiString	FullAttributeLine = AttributeLines->Strings[i].Trim();
		int			EqualPos = FullAttributeLine.Pos("=");
        AnsiString	AttributeName;
        AnsiString	AttributeValue;

        if (EqualPos <= 1)
        {
        	AttributeName = FullAttributeLine;
            AttributeValue = strNull;
        }
        else
        {
        	AttributeName = FullAttributeLine.SubString(1, EqualPos - 1).Trim();
            AttributeValue = (EqualPos < FullAttributeLine.Length())
            			   ? FullAttributeLine.SubString(EqualPos + 1,
                           		FullAttributeLine.Length() - EqualPos).Trim()
                           : strNull;
        }

        if (AttributeName == strConstTruthFile)
        {
        	// This is a constant truth file attribute.  Is it valid?
            if (!ParseConstTruthFileAtt(AttributeValue, ObjectStatus, JustEditFile, AutoModal))
            {
		    	if (!JustEditFile)
		        {
                    IsAttributeValid = false;

                    // Only allow one modal error/warning message.
                    if (AutoModal)
                        ObjectStatus = foFoundEnd;
                }
            }

            // If we ran into the next object, then we are done with this object.
            if (ObjectStatus == foFoundEnd)
                break;
        }
        else if (AttributeName == strConstTruthTable)
        {
        	// Is this ConstTruthTable attribute valid?
            //		Skip attributes that are not for our object.
        	if (JustEditFile ||
            	ObjectStatus != foFoundStart)
	            	continue;

            if (!ParseConstTruthTableAtt(AttributeValue, AutoModal, false))
            {
                IsAttributeValid = false;

                // Only allow one modal error/warning message.
                if (AutoModal)
                {
                    ObjectStatus = foFoundEnd;
                    break;
                }
            }
        }
        else
        {
        	// This must be an object identification line.
            //		Strip off the comments by finding the left-most comment character.
            int			CommentPos = FullAttributeLine.Pos("#");
            AnsiString  ObjectIdLine = (CommentPos <= 0)
                                 	 ? FullAttributeLine.Trim()
                                 	 : FullAttributeLine.SubString(1, CommentPos - 1).Trim();

            // Skip blank lines and comment lines.
            if (ObjectIdLine == strNull)
	           	continue;

            // Object identification lines have the format:
            //		<ObjectName> (<NumberOfInputs>, <NumberOfOutputs>)
            int     LeftPos = ObjectIdLine.Pos("(");
            int     CommaPos = ObjectIdLine.Pos(",");
            int     RightPos = ObjectIdLine.Pos(")");

            if (1 >= LeftPos ||
            	LeftPos >= CommaPos ||
                CommaPos >= RightPos)
            {
		    	if (!JustEditFile)
		        {
	            	IsAttributeValid = false;

                    ErrorTrace(304, this, VStringList(AttributeValue, FileName, FullAttributeLine),
                        AutoModal);

                    // Only allow one modal error/warning message.
                    if (AutoModal)
                    {
                        ObjectStatus = foFoundEnd;
                        break;
                    }
                }

            	continue;
            }

            // Extract the object name and number of inputs/outputs.
            AnsiString	ObjectName = ObjectIdLine.SubString(1, LeftPos - 1).Trim();
            AnsiString  InputCountStr = ObjectIdLine.SubString(LeftPos + 1, CommaPos -
            	LeftPos - 1).Trim();
            AnsiString	OutputCountStr = ObjectIdLine.SubString(CommaPos + 1, RightPos -
            	CommaPos - 1).Trim();

            int		InputCount = 0;
            int		OutputCount = 0;

            try
            {
                InputCount = StrToInt(InputCountStr);
                OutputCount = StrToInt(OutputCountStr);
            }
            catch(...)
            {
		    	if (!JustEditFile)
		        {
                    IsAttributeValid = false;

                    // Inform the user that the object identification line is invalid.
                    ErrorTrace(304, this, VStringList(AttributeValue, FileName, FullAttributeLine),
                        AutoModal);

                    // Only allow one modal error/warning message.
                    if (AutoModal)
                    {
                        ObjectStatus = foFoundEnd;
                        break;
                    }
                }

                continue;
			}

            // This is a valid object identifier line.  If we have already found our
            //		object, then this marks the end of the attributes for our object.
            if (ObjectStatus == foFoundStart)
            {
                ObjectStatus = foFoundEnd;
                break;
            }
            else if (ObjectName == NameInfo->Name &&
            		 InputCount == Inputs->Count &&
                	 OutputCount == Outputs->Count)
            {
            	// Everything matches!  This is the start of our object.
                //		If we are just editing the file, then display it and tell all the
                //		recursive calls that we are done.
		    	if (JustEditFile)
		        {
					TEditTextFileForm	*EditFileForm = new TEditTextFileForm(MainForm);

                    EditFileForm->EditTextFile(FileName, AttributeLines, i);

                    delete EditFileForm;

	                ObjectStatus = foFoundEnd;
	                break;
                }
                else
                {
			   		ObjectStatus = foFoundStart;
                }
            }
        }
    }

    delete AttributeLines;

    return IsAttributeValid;
}


// Parse the constant truth table AttributeValue to extract and validate the individual
//		truth table entries.  Invalid table entries are reported.  If the object's
//		TruthTable has been created, then valid entries are appended into it.
//		If AutoModal is true, then at most only one error/warning message will be issued and
//		it will be in a modal window.
//		If DuplicateOutputCodes is true and there are more output nodes than output codes,
//		then the last output code provided will be duplicated (instead of an error message).
//		Returns "true" if all of the TruthTableEntries are valid.
//
// ConstTruthTable attributes have the format:
//		ConstTruthTable=[TruthTableEntry [; TruthTableEntry [; ...]]] [#comment]
//		or
//		ConstTruthTable=<None> [#comment]
bool I2adl::ParseConstTruthTableAtt(AnsiString AttributeValue, bool AutoModal, bool
   	DuplicateOutputCodes)
{
    // Strip off the comments by finding the left-most comment character.
    int		CommentPos = AttributeValue.Pos("#");

    if (CommentPos <= 0)
    	AttributeValue = AttributeValue.Trim();
    else
        AttributeValue = AttributeValue.SubString(1, CommentPos - 1).Trim();

    // Don't try to parse the "<None>" keyword.
    if (AttributeValue.UpperCase() == str_NONE_)
    	return true;

    // Parse out all of the TableEntries in this attribute.
    TStringList		*TableEntries = ParseString(AttributeValue, ";");
	bool			IsAttributeValid = true;

    for (int i = 0; i < TableEntries->Count; i++)
    {
    	TruthTableEntry		*TableEntry = new TruthTableEntry(TableEntries->Strings[i]);

        // If the entry is valid, then save it in the TruthTable (if it has been created).
        if (ParseTruthTableEntry(TableEntry, AutoModal, DuplicateOutputCodes))
        {
            if (TruthTable != NULL)
                TruthTable->Add(TableEntry);
            else
            	delete TableEntry;
        }
        else
        {
            IsAttributeValid = false;

            delete TableEntry;

            // Prevent more than one modal error/warning message.
            if (AutoModal)
                break;
        }
    }

    delete TableEntries;

    return IsAttributeValid;
}


// Parse the truth TableEntry to extract and validate the components.
//		If AutoModal is true, then at most only one error/warning message will be issued and
//		it will be in a modal window.
//		If DuplicateOutputCodes is true and there are more output nodes than output codes,
//		then the last output code provided will be duplicated (instead of an error message).
//		Invalid TableEntries are reported.
//		Returns "true" if the truth TableEntry is valid.
//
// Truth TableEntries have the format:
//
//		InVal [, InVal [, ...]]: OutVal [, OutVal [, ...]]
//		or
//		InVal [, InVal [, ...]]: ReplacingObject: InConn [, InConn [, ...]]:
//			OutConn [, OutConn [, ...]]
//
bool I2adl::ParseTruthTableEntry(TruthTableEntry *TableEntry, bool AutoModal, bool
	DuplicateOutputCodes)
{
    TableEntry->EntryComponents = ParseString(TableEntry->EntryString, ":");

    if (TableEntry->EntryComponents->Count != 2 &&
        TableEntry->EntryComponents->Count != 4)
    {
        ErrorTrace(312, this, TableEntry->EntryString, AutoModal);

        return false;
    }

    // Is the first component valid ("InVal [, InVal [, ...]]")?
    //		Make uppercase to avoid testing for "c" & "x".
    TableEntry->InVals = ParseString(TableEntry->EntryComponents->Strings[0].UpperCase(), ",");

    // Does the TableEntry have a code for every input?
    if (TableEntry->InVals->Count != Inputs->Count)
    {
        ErrorTrace(313, this, VStringList(TableEntry->EntryString, IntToStr(TableEntry->
        	InVals->Count), IntToStr(Inputs->Count)), AutoModal);

        return false;
    }

    // Are the InVal codes valid?
    //		Also, convert the number InVal codes into Bit format (one char per bit).
    for (int i = 0; i < TableEntry->InVals->Count; i++)
    {
    	AnsiString	InVal = TableEntry->InVals->Strings[i];

        if (!ParseTruthTableCode(ttInVal, InVal, TableEntry, AutoModal, Inputs->Items[i]))
        	return false;

		TableEntry->InVals->Strings[i] = InVal;
    }

    // If the TableEntry has two components, then we are deleting (not replacing) the
    //		primitive object.
    if (TableEntry->EntryComponents->Count == 2)
    {
        // Is the second component valid ("OutVal [, OutVal [, ...]]")?
        //		Make uppercase to avoid testing for "d" and "i".
        TableEntry->OutVals = ParseString(TableEntry->EntryComponents->Strings[1].UpperCase(),
        	",");

        // Does the TableEntry have a code for every output?
        if (TableEntry->OutVals->Count != Outputs->Count)
        {
        	// No!  Are we to duplicate the last output code provided or issue an error
            //		message?
        	if (DuplicateOutputCodes &&
            	TableEntry->OutVals->Count < Outputs->Count &&
                TableEntry->OutVals->Count > 0)
            {
            	while (TableEntry->OutVals->Count < Outputs->Count)
                	TableEntry->OutVals->Add(TableEntry->OutVals->Strings[TableEntry->OutVals->
                    	Count - 1]);
            }
            else
            {
                ErrorTrace(314, this, VStringList(TableEntry->EntryString, IntToStr(TableEntry
                	->OutVals->Count), IntToStr(Outputs->Count)), AutoModal);

                return false;
            }
        }

        // Are the OutVal codes valid?
        for (int i = 0; i < TableEntry->OutVals->Count; i++)
	        if (!ParseTruthTableCode(ttOutVal, TableEntry->OutVals->Strings[i], TableEntry,
            	AutoModal))
	        		return false;

	    return true;
    }

    // The TableEntry has four components.  We are replacing the primitive object with a
    //		different (simpler) object.
    //
    // Is the third component valid ("InConn [, InConn [, ...]]")?
    //		Break off the optional "(DataSet)" parameters and then make uppercase to avoid
    //		testing for "d" and "i".
    TableEntry->InConns = ParseString(TableEntry->EntryComponents->Strings[2], ",");
    TableEntry->InDataSets = new DataSetList(sizeof(DataSet));

    for (int i = 0; i < TableEntry->InConns->Count; i++)
    {
        AnsiString	InConn = TableEntry->InConns->Strings[i];
        int			LeftPos = InConn.Pos("(");
        int			RightPos = InConn.Pos(")");

        if (1 < LeftPos &&
            LeftPos < RightPos &&
            RightPos == InConn.Length())
        {
            // Is this a valid data set name?
            AnsiString	DataSetName = InConn.SubString(LeftPos + 1, RightPos - LeftPos - 1);

            TableEntry->InDataSets->Add(GetDataSet(DataSetName));
            InConn = InConn.SubString(1, LeftPos - 1).Trim().UpperCase();

            if (TableEntry->InDataSets->Items[i] == NULL)
            {
                ErrorTrace(315, this, VStringList(TableEntry->EntryString, DataSetName),
                    AutoModal);

                return false;
            }
        }
        else
        {
            InConn = InConn.UpperCase();

            Node	*InputNode = GetNode(InConn);

            if (InputNode == NULL)
                TableEntry->InDataSets->Add(VariantDataSet);
            else
                TableEntry->InDataSets->Add(InputNode->DSet);
        }

        TableEntry->InConns->Strings[i] = InConn;

        // Is the InConn code valid?
        if (!ParseTruthTableCode(ttInConn, InConn, TableEntry, AutoModal))
            return false;
    }

    // Is the fourth component valid ("OutConn [, OutConn [, ...]]")?
    //		Make uppercase to avoid testing for "d" and "o".
    TableEntry->OutConns = ParseString(TableEntry->EntryComponents->Strings[3].UpperCase(),
        ",");

    // Are the OutConn codes valid?
    for (int i = 0; i < TableEntry->OutConns->Count; i++)
        if (!ParseTruthTableCode(ttOutConn, TableEntry->OutConns->Strings[i], TableEntry,
            AutoModal))
                return false;

    // Are the OutConn codes trying to use the same transport on two different nodes
    //		(transports only have one source).
    for (int i = 0; i < TableEntry->OutConns->Count - 1; i++)
    {
        AnsiString	OutConn1 = TableEntry->OutConns->Strings[i];

        if (OutConn1[1] == 'O')
        {
            for (int j = i + 1; j < TableEntry->OutConns->Count; j++)
            {
                AnsiString	OutConn2 = TableEntry->OutConns->Strings[j];

                if (OutConn1 == OutConn2)
                {
                    ErrorTrace(318, this, VStringList(TableEntry->EntryString, OutConn1),
                        AutoModal);

                    return false;
                }
            }
        }
    }

    // Is the second component valid ("ReplacingObject")?
    //		Create a temporary object so we can call ResolveObject().
    I2adl	*TempReplacingObject = new I2adl(TableEntry->EntryComponents->Strings[1]);

    TempReplacingObject->AttSystem = AttSystem;

    // Rebuild the node lists.
    TempReplacingObject->Inputs->Clear();
    TempReplacingObject->Outputs->Clear();

    for (int i = 0; i < TableEntry->InConns->Count; i++)
    {
        Node	*InputNode = new Node("In", this, (DataSet *) TableEntry->InDataSets->
        	Items[i]);

        AddNode(&TempReplacingObject->Inputs, InputNode);
    }

    for (int i = 0; i < TableEntry->OutConns->Count; i++)
    {
        Node	*OutputNode = new Node("Out", this, VariantDataSet);

        AddNode(&TempReplacingObject->Outputs, OutputNode);
    }

    TableEntry->ReplacingObject = TempReplacingObject->ResolveObject(false);

    // If we didn't find a matching I2adl def object, then error out.
    if (TableEntry->ReplacingObject == NULL)
    {
        ErrorTrace(316, this, VStringList(TableEntry->EntryString, TableEntry->
            EntryComponents->Strings[1]), AutoModal);

        delete TempReplacingObject;

        return false;
    }

    // If the user didn't supply all of the data sets, then error out.
    for (int i = 0; i < TableEntry->ReplacingObject->Inputs->Count; i++)
    {
        if (TableEntry->InDataSets->Items[i] == VariantDataSet &&
            TableEntry->ReplacingObject->Inputs->Items[i]->DSet == VariantDataSet)
        {
            ErrorTrace(317, this, VStringList(TableEntry->EntryString, IntToStr(i),
                TableEntry->InConns->Strings[i]), AutoModal);

            delete TempReplacingObject;

            return false;
        }
    }

    // If more than one I2adl def matched, then warn the user.
    if (TableEntry->ReplacingObject->Ambiguous == ambYes)
        ErrorTrace(311, this, VStringList(TableEntry->EntryString, TableEntry->
            EntryComponents->Strings[1]), AutoModal);

    delete TempReplacingObject;

    return true;
}


// Parse the TruthTableCode to make sure it is valid for this CodeType.  For speed purposes,
//		calling program must pass uppercase codes.  Invalid codes are reported.
//		If IONode is not NULL, then its data set will be used to convert number TruthTableCodes
//		into bit format (one character per bit).
//		Returns "true" if the code is valid.
//
// InVal   codes have the format:	Number, C or X
// OutVal  codes have the format:	Number, D or I99
// InConn  codes have the format:	Number, D or I99
// OutConn codes have the format:	D or o99
//
bool I2adl::ParseTruthTableCode(TruthTableEnum CodeType, AnsiString &TruthTableCode,
	TruthTableEntry *TableEntry, bool AutoModal, Node *IONode)
{
	// Blank codes are invalid.
    if (TruthTableCode == strNull)
    {
    }

	// Is the code "C" (any constant value)?
	else if (TruthTableCode == "C")
    {
    	if (CodeType == ttInVal)
        	return true;
    }

	// Is the code "D" (disconnect)?
	else if (TruthTableCode == "D")
    {
    	if (CodeType == ttOutVal ||
        	CodeType == ttInConn ||
            CodeType == ttOutConn)
	        	return true;
    }

	// Is the code "X" (any value, constant or not)?
	else if (TruthTableCode == "X")
    {
    	if (CodeType == ttInVal)
        	return true;
    }

	// Is the code "I99" (input node #99)?
    else if (TruthTableCode[1] == 'I')
    {
    	Node	*InputNode = GetNode(TruthTableCode);

        if (InputNode != NULL)
            if (CodeType == ttOutVal ||
                CodeType == ttInConn)
                    return true;
    }

	// Is the code "o99" (output node #99)?
    else if (TruthTableCode[1] == 'O')
    {
		Node	*OutputNode = GetNode(TruthTableCode);

        if (OutputNode != NULL)
            if (CodeType == ttOutConn)
                return true;
    }

	// Is the code a number?
	else
    {
        try
        {
            int		Number = StrToInt(TruthTableCode);

		    if (CodeType == ttInVal ||
            	CodeType == ttOutVal ||
                CodeType == ttInConn)
            {
                // Convert to bit format (one character per bit).
                if (IONode != NULL)
                {
                    TruthTableCode = strNull;

                    for (int i = 0; i < IONode->DSet->BitLength; i++)
                    {
                        int		NextBit = Number & (int) 1;

                        TruthTableCode = IntToStr(NextBit) + TruthTableCode;

                        Number >>= 1;
                    }
                }

		        return true;
            }
        }
        catch(...)
        {
        	// Code is not a number.
        }
    }

    // Report the invalid code.
    //		There are 4 error messages here, one for each type of code.
    ErrorTrace(321 + CodeType, this, VStringList(TableEntry->EntryString, TruthTableCode),
    	AutoModal);

    return false;
}

