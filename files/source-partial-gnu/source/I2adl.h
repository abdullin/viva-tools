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

#ifndef I2adlH
#define I2adlH

#include "NodeList.h"
#include "Cost.h"
#include "Dismember.h"
#include "List.h"

#define MAX_NAMECODES 		(int) NAME_CASTOUT
#define PREDEFINED_OBJECTS 	NAME_TEXT + 1

extern AnsiString	I2adlNames[MAX_NAMECODES];	// Names corresponding to NameCodes enumeration
extern int  		DownwardCompatibleCount; 	// Number of DownwardCompatibleObjects from
												//		ResolveObject()
extern DataSet		*InputVariantDS; 			// Set inside ResolveObject().  For variant,
												//		primitive objects, this is the
                                                //		polymorphic input DataSet.
extern int			IsVisitedKey;				// Used for IsVisited Flag.
												//		Increment to make unique.

enum ProcessGoEnum // used in conjuction with Com I2adl's.  The state of the "Go" node.
{
    goNot,
    goInitializing,
    goLow,
    goHigh
};

enum AmbiguousEnum
{
	ambNo = 0,
    ambYes = -1,
    ambYesDiagnosed = -2
};


class I2adl
{
protected:
	VivaIcon        *IconInfo;			// Points to the Object icon's name and bitmap.  May
										//     contain a level of indirection.  Private to
										//     ensure use of the GetIconInfo() method.
    int  			m_Left;				// X position of icon on behavior definition page
    int 			m_Top;			  	// Y position of icon on behavior definition page
										//     changes the height, the "Y" value of the first
										//     control node determines height of the object.
	int				m_Width;			// Width of icon
	int				m_Height;			// Heigth of icon.  Because viewing control nodes

	int				GetLeft();
	int				GetTop();
	int				GetXLoc();
	int				GetYLoc();
	void			SetLeft(int _Left);
	void			SetTop(int _Top);

	int				GetWidth();
	int				GetHeight();
	void			SetWidth(int _Width);
	void			SetHeight(int _Height);

public:

    __property int X		= { read = GetLeft, write = SetLeft};
    __property int Y		= { read = GetTop, write = SetTop};
    __property int XLoc		= { read = GetXLoc};
    __property int YLoc		= { read = GetYLoc};
    __property int Width	= { read = GetWidth, write = SetWidth};
    __property int Height	= { read = GetHeight, write = SetHeight};

    I2adl           *CopyOf;          // Points to a copied I2adl's source.
    I2adl			*MasterSource;	  // Object in behavior which is master copy.  Volatile.
    								  // 	Use only for pointer comparision--never access!!
    VivaIcon        *NameInfo;        // Object's name and icon info, in centralized location.
    NameCodes       NameCode;         // Enumerated code for fast name comparison
    NodeList        *Inputs;          // List of input nodes
    NodeList        *Outputs;         // List of output nodes
    TStringList     *Attributes;      // List of "Name=Value" attribute strings
                                      //     Always append to with SetAttribute().
    TruthTableEntryList *TruthTable;  // The constant truth table as extracted from the object
    								  //	attributes or hard coding.  Loaded during synthesis
                                      //	for just those objects that have constant inputs.
    SystemString    *AttSystem;       // "Value" portion of the "System=" attribute string
    SystemString    *AttChildAtt;     // "Value" portion of the "ChildAttribute=" string
    ResourceString  *AttResource;     // "Value" portion of the "Resource=" attribute string

    union
    {
    	ResourceString  *AttProvidesRes;	// "Value" portion of the "ProvidesResource=" string
    	ResourceString  *UnconnectedXport;	// Identifies temporary objects used to prevent text
        									//		base from discarding unconnected transports.
    };

    int				AttResourceQty;   // "Value" portion of the "ResourceQty=" attribute string
    I2adlList		*Behavior;        // Lower-level behavior definition
    VDismember		*MemberOf;        // Info on parent I2adl object, from expansion.
    NameCodes       ExecuteType;      // Controls how the object's events are processed by
                                      //     X86Thread.  Normaly the same as NameCode.
   	ExposerCodes    ExposerCode;      // Identifies data set Exposer & Collector objects
    VCost			ObjectCost;       // Least cost system - Cost of resources consumed
    VivaSystem      *InSystem;        // Least cost system - Winning system
    int             SearchListIndex;  // Index in FunctionDescription.  Saves a call to IndexOf.
    int				FlattenListIndex; // Index in FlattenList.  Saves a call to IndexOf.
    Node            *FakeCNodes[ceLast]; // Used in converting old files.
    bool            IsPrimitive;      // Primitive objects terminate recursive explosion
    bool        	DontUse;          // Whether or not to add to a "pre-expanded" behavior.
    bool			TriedForceFlatten; // Already tried to flatten out of sequence, for
    								  //	speed purposes.
    bool			Warned0187;		  // Used to only issue warning #187 once per object.
    bool			Warned0199;		  // Used to only issue warning #199 once per object.
    bool			Warned4061;		  // Used to only issue warning #4061 once per object.
    bool			Warned5002;		  // Used to only issue warning #5002 once per object.
    bool			DoneFlattening;   // Archived from FlattenList, long-term.  Greatly reduces
    								  // 	redundant processing.
    bool			Selected;         // As in I2adl Editor, by user.
    int           	FlattenFutility;  // Measure of wasted effort trying to flatten.
    int				ArchiveCount;     // Count of times archived from FlattenList.
    void 			(*ffunc)(I2adl*); // Which "Flatten..." function to call.  Speed issue.
    BYTE			*EdifID;		  // Unique, re-usable ID tag for a portion of the EDIF text
    								  //		that is output by the Translator
                                      //		(where applicable).
    int				SequenceNumber;   // Tag for to make auto-generated collectors unique.
    KeepObjectEnum  KeepObject;		  // Under what conditions to keep Object during synthesis
    DataSet         *SuperSet;		  // Complex data set used for reconstruction.
    BitFanCodes		BitFanCode;		  // Set by the compiler on primitive objects to indicate
    								  //     if BitFanExp/Col helper objects were created.
    bool			StoredState;	  // for Assign object.
    I2adl			*ParentI2adl;	  // I2adl Ref in which this exists (if any)
    AnsiString		*OwningLib;		  // Object Library to which this belongs
    bool			IsSystemObject;	  // Belongs to the dynamic system description.
    WipSheetEnum	WipSheetCode;	  // Indicates if an object or a type of WIP sheet

	union
    {
	    int             IsVisited;		// Status flag used by synthesizer
        int				DontExpand;	  	// After or between matched NAME_SSELECT objects
        int				InstanceIndex;	// In the case that there is another with same name
        int				Included;		// Already included in "reference expansion", the
        								//		compilation of dependency info when saving files
        int				Busy;			// In another flatten process at present
        int				FeedBackLoop;	// Prevents infinite searches on feedback loops.
    };

    union
    {
		Byte			*RamData;			// Used by primivite RAM objects.
        IOLink          *SourceSinkLink;  	// Link information for communicating between
        									//		systems
        TComHelper      *ComHelper;       	// Contains functions and data for COM operations.
        TComObject      *ComObject;       	// Used by NAME_DISPATCH; object to which dispatch
        									//		belongs.
        TComObjectInfo  *CreateObjectInfo; 	// Used by NAME_COMCREATE for dynamic COM
        									//		instantiation.
        TWidget         *Widget;          	// Identified by ExecuteType of
        									// 		EXECUTE_UILINKED_INPUT or
                                            //		EXECUTE_UILINKED_OUTPUT.
    };

    union
    {
        MyTShapeI2adl       *I2adlShape;    // Pointer to graphical shape.  Can be NULL.
        MyTShapeTransport   *XShape;        // Unionized to avoid having to cast, in the case of
        									//		NAME_TRANSPORT.
    };

	union
    {
	    int             	InfoRate;		// Average IONS (information atoms) per second
		AmbiguousEnum		Ambiguous;      // Used in loader; flag to indicate that footprint
        									//		is identical to that of another I2adl def
    };

    __fastcall I2adl(AnsiString _Name, int InputNodeCount = 0, int OutputNodeCount = 0);
    __fastcall I2adl(NameCodes _NameCode, int _X = 0, int _Y = 0);
    __fastcall I2adl(I2adl *_I2adl, CopyNodesEnum CopyNodes, bool OnlyCopySystemAtt = false);

    void I2adl_Init(int InputNodeCount = 0, int OutputNodeCount = 0);

    // These 3 constructors are strictly for COM use:
    __fastcall I2adl(TMemberDesc *pMemberDesc, TComObject *pOwningObject);
    __fastcall I2adl(TComObject *_ComObject);
    __fastcall I2adl(TComObjectInfo *_ObjectInfo);

    ~I2adl();

    VivaIcon *GetIconInfo();
    DataSet *BuildCastOutputDataSet();
    void SetIconInfo(bool ClearIndirection = false);
    bool HasIconIndirection();
    bool HasIOBufAttribute();

    void InitTransport(int X, int Y, bool Horizontal, int Length, bool Forward, bool
    	UseMovingNode = false);
    void InitComHelper(TMemberDesc *pMemberDesc, TComObject *pOwningObject);
    void Conformalize();
    bool ArrangeNodes(bool ConvertOldFile, bool Reconnect = true, bool SetDataSet = true,
    	bool UpdateParentNodeLists = true);
    void AtomicTemplate(int InputCount, int OutputCount);
    bool Equals(I2adl *Object, bool CompareGraphicInfo = false, bool TestPrimitive = true);
    Node *GetNode(int NodeIndex);
    Node *GetNode(AnsiString NodeCode);
    void SetName(AnsiString &_Name);
    bool IsVariant();
    bool IsGlobalLabel(AnsiString *AttributeString = NULL);
    bool IsConstant();
	bool IsNoPromote();
	bool IsPrimitiveEDIF();
    AnsiString GetGlobalLabelName();
    void DisconnectObject();
    void DeleteProjectObject();
    Node *OtherEnd(Node *_Node);
    void ChangeDataSets(DataSet *DSet);
    void ChangeDataSets(Node *_Node);
    void AddOutput(Node *_Node);
    AnsiString GetExtendedName();
    void __fastcall GetAttribute(AnsiString Attribute, AnsiString &Value);
    void __fastcall SetAttribute(AnsiString &AttributeValue, bool UpdateExisting = true);
    void __fastcall SetAttribute(AnsiString Attribute, int Value, bool UpdateExisting = true);
    void __fastcall SetAttribute(AnsiString &Attribute, AnsiString &Value
    	, bool UpdateExisting = true);
    void SetAttribute(char *cAttribute, AnsiString &Value);
    void ClearAttributes();
    void ReplaceAttributes(I2adl *Object);
    void MergeAttributes(I2adl *Object, bool CopyNodeNames = true, bool CalculateAtt = true);
    void CalculateAttributes();
	AnsiString& InheritHornName();
    void PropagateChildAttributes(bool JustApplyToSelf = false);
	void ExtractChildAttributes(TStringList *ChildAttributes = NULL, TStringList
    	*DirectedChildObjectss = NULL);
    bool DoesChildAttributeApply(AnsiString DirectedChildObjects);
    I2adl *ResolveObject(bool DiagnoseAmbiguity = true, bool UseHash = false, AnsiString
    	*ParentName = NULL, bool UseCompileTimeResolutionRules = false);
    void CreateShape();
    int NumberOfNodesConnected();
    AnsiString EnhancedName();
    AnsiString ExternalName();
    VivaSystem *GetSystem();
    int NodeSize();
    AnsiString QualifiedName(bool SingleLevel = false);
	AnsiString *QualifiedNameSpeed();
    AnsiString InputDataSets(bool IndicateDisconnect = true);
	bool MayObjectBeDeleted();
    bool IsStandardPrimitiveObject();

    // Made for Com I2adl only:
    void InitCom(AnsiString MemberName, TStringList *MemberList, TComObject *ComObject);
    void InitDispatch(TComObject *_ComObject);
    void CopyResultData(BYTE *pVal, int nNode = 2, int Offbound = 0);

    ProcessGoEnum ProcessGoSignal(Node *EventNode);
    void GetControlNodes(Node **&Nodes);
    void PlaceControlNodes(Node *Nodes[4]);
    void CopyHierarchyInfo(I2adl *Object);
	int NonTransparentEdge();
    void CalculateHorzGridSnap(int &Data);
	int CalculateFontSize();
    AnsiString GetKeyString(bool ForCompile);
	void BringShapeToFront();
	void MakeEdifID();
    void AddToEdifID(int CompressionCode, int &BitPos, int &BytePos);
    AnsiString UncompressedEdifID();
	void AssignToSystem(VivaSystem *System);
	void AssignToX86System(VivaSystem *X86System);
	void ShowHelpFile();
    void SetDefaultTextDimensions();
    void LoadTruthTable();
	bool ParseConstTruthFileAtt(AnsiString FullAttValue, FoundObjectEnum &ObjectStatus,
    	bool JustEditFile = false, bool AutoModal = false);
	bool ParseConstTruthTableAtt(AnsiString AttributeValue, bool AutoModal = false, bool
    	DuplicateOutputCodes = true);
	bool ParseTruthTableEntry(TruthTableEntry *TableEntry, bool AutoModal, bool
    	DuplicateOutputCodes);
    bool ParseTruthTableCode(TruthTableEnum CodeType, AnsiString &TruthTableCode,
    	TruthTableEntry *TableEntry, bool AutoModal, Node *IONode = NULL);
};


class FastString
{
    #define STRING_INCREMEMNT 2000
public:

    char	*String;
    int		Capacity;
    int		Pos;

    FastString()
    {
        String = new char[STRING_INCREMEMNT];
        Capacity = STRING_INCREMEMNT;
        Pos = 0;
    }

    ~FastString()
    {
        delete[] String;
    }

    void StrAlloc(int size)
    {
        if (Pos + size >= Capacity)
        {
            Capacity = Pos + size + STRING_INCREMEMNT;

            char	*newstring = new char[Capacity];

            strcpy(newstring, String);

            delete[] String;

            String = newstring;
        }
    }

    void Add(char c)
    {
        StrAlloc(2);
        String[Pos++] = c;
        String[Pos] = 0;
    }

    void Add(const AnsiString &s)
    {
        int		len = s.Length();

        StrAlloc(len);
        strcpy(&String[Pos], s.c_str());
        Pos += len;
    }

    void Add(char *s)
    {
        char	*temp = s;

        while (*temp != 0)
        	temp++;

        int		len = int(temp) - int (s);

        StrAlloc(len);
        strcpy(&String[Pos], s);
        Pos += len;
    }
};


// This class is used to store parsed constant truth table entries which have the format:
//
//		InVal [, InVal [, ...]]: OutVal [, OutVal [, ...]]
//		or
//		InVal [, InVal [, ...]]: ReplacingObject: InConn [, InConn [, ...]]:
//			OutConn [, OutConn [, ...]]
//
class TruthTableEntry
{
public:

    AnsiString		EntryString;		// Full (unparsed) truth table entry string.
    TStringList		*EntryComponents;	// Top level parsing (":" separator).  Contains either 2
    									//		strings (format #1) or 4 strings (format #2).
    TStringList		*InVals;			// List of parsed in values

    									// Format #1:
    TStringList		*OutVals;			// List of parsed out values

    									// Format #2:
    I2adl			*ReplacingObject;	// Object name of the replacing I2adl def
    TStringList		*InConns;			// List of parsed in connections
    DataSetList		*InDataSets;		// List of data set pointers, one for every InConn
    TStringList		*OutConns;			// List of parsed out connections

    TruthTableEntry(AnsiString _EntryString)
    {
    	EntryString = _EntryString;
        EntryComponents = NULL;
        InVals = NULL;
        OutVals = NULL;
        ReplacingObject = NULL;
        InConns = NULL;
        InDataSets = NULL;
        OutConns = NULL;
    }

    ~TruthTableEntry()
    {
        delete EntryComponents;
        delete InVals;
        delete OutVals;
        delete InConns;
        delete InDataSets;
        delete OutConns;
    }
};

#endif
