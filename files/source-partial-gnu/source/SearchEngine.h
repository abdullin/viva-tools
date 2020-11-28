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

#ifndef SearchEngineH
#define SearchEngineH

void FlattenObject(I2adl *Object);
void FlattenTransport(I2adl *Object);


// Manages a sorted string table of global label names.
class VGlobalLabelTable : public StringTable
{
public:
	void __fastcall BeforeDestruction();

	StringEntry *NewEntry(const AnsiString &UString);
};


class VGlobalLabelEntry : public StringEntry
{
public:
	Node	*DefNode;		// Input node on the global label output (def) that defines the
    						//		source of this global label.
    VList	*RefNodes;		// List of output nodes on the global label inputs (refs) that are
    						//		waiting for the def to be found.
    I2adl	*HeadJoinGlobals;// The extra Input object at the head of the JoinGlobals network
    						//		that allows future global label defs to be inserted into
                            //		the network.  Deleted as late as possible in synthesis.
    int		DeletePri;		// Records the value of the JoinGlobalsDeletePri attribute.
    bool	WasDefined;		// Records if the global label was defined before it was deleted.
    						//		Used to avoid #40 error messages.
    bool	IsDeleted;		// Set to "True" when the global label input (def) is deleted.
    						//		Prevents the global label name from ever being used again.

	VGlobalLabelEntry(const AnsiString &Name);
    ~VGlobalLabelEntry();
};


enum VariantLevelEnum
{
    vlNot,
    vlVariantUnconnected,
    vlWasVariant,
    vlVariantConnected
};


class SearchEngine
{
public:

	Path			*PathsHolder;           //  
	I2adlListList	*LinkInOutBehaviors;	// Holds the temporary behaviors for the LinkIn and
    										//		LinkOut objects.
	VList			*GraphicDisplays;		// List of graphic displays showing compile progress
	VList			*PartitionTargets;      //
	int				SearchEffort;           //
	I2adl			*DefaultTransport;		//
	int				LastDataSetCount;		// Number of data sets existing when
    										//		CollectorLookupTable was loaded.
	StringTable		*CollectorLookupTable;	// A lookup table to quickly resolve collectors by
    										//		converting their child data sets into a key
                                            //		string.
	StringTable		*ExpandedBehaviors;		// A list of pre-expanded behaviors re-used in
    										//		ExpandObject
    int				CleanPass;              // Number of passes during which nothing was
    										//		resolved or changed; 1-based
    I2adlList		*FlattenList;           // List of stuff that actually needs flattening;
    										//		condensed from FunctionDescription for speed
	VList			*ExpansionIndexes;      // Number of objects before each expansion; used
    										//		by the compiler's mock recursion
    VList           *JoinGlobalss;			// The list of all JoinGlobals objects that can be
    										//		used to handle duplicate global label
                                            //		definitions.

	enum ConnectionCodes
	{
		ccOuterObject = -2,
	};

	SearchEngine();
	~SearchEngine();

	int GetInsertIndex(int TransportIndex);
	Path *GetPathInfo(I2adl *LogicalTransport);
	bool GetBestConnection(Path *ExistingPath, VivaSystem *TargetSystem, Node *SinkNode,
    	VCost &LogicalTransportCost, PathList *NewSegments, bool SystemsMatch);
	Path *GetTransportCostsForAnObject(I2adl *WhatIfObject);
	void Reset();
	void __fastcall UpdateGraphic();
	void __fastcall SetUpPartitionDisplays();
	void __fastcall RemoveDisplayGraphics();
    void RouteFixedObjects();
	void RouteFloatingObjects();
	void __fastcall BuildNetList(PathList *PathsHolder);
	void __fastcall BuildConnectedTransports(Path *PathIntersection, Node *DriverSourceNode);
	void PopBehavior(I2adl *BaseObject);
	void __fastcall AddToPathHolder(Path *PathObject);
	void __fastcall Route(Path *RoutePath);
	void ExchangeRoute(I2adl *Object, Path *NewSegments);
	bool RouteLogicalTransport(I2adl *LogicalTransport, I2adl *WhatIfObject, I2adl *FDObject,
    	I2adl *SourceObject, VCost *TotalTransportCost, Path *ObjectPathSegments);
	Path *FindLowCostPath(VivaSystem *FromSystem, VivaSystem *TargetSystem, Node *AtNode,
    	Path *StartPoint = NULL, SystemList *SystemsVisited = NULL);
	int GetPlungeList(IndexList *ConnectedList, int ZoneStart);
	void TryToRoute(I2adl *Object);
	Path *__fastcall Score(I2adl *Object);
	bool __fastcall ExecuteTranslation();
	bool __fastcall Compile();

	// Synthesizer methods:
	I2adl *BuildIOLink(I2adl *ParentObject, DataSet *LinkFormat, bool Direction, AnsiString
		InCode, Node *PlacementNode = NULL);
	void WidgetSetUp(I2adl *Object);
	void DeleteObject(I2adl *Object);
	void SetSelectFlags(Node *_Node, int SelectKey);
	void MergeTransports(I2adl *SurvivingTransport, I2adl *MergedTransport);
	void DoNotPromote(I2adl *Object);
	void PreProcess(I2adlList *BaseList, AnsiString &SheetName, I2adl *ParentObject);
	I2adl *ShortCircuit(Node *DefunctNodeA, Node *DefunctNodeB);
	void RequestConstant(int Outcome, Node *SinkNode, VDismember *_MemberOf, AnsiString
    	&ConstantDocumentation = strNull);
	void RemoveSelectObject(I2adl *SelectObject, int ConstantValue);
	bool ConstantRemoval(Node *InputNode, int BitOffset, int ConstantValue);
	bool SourcedByConstant(Node *InputNode, AnsiString InputCodes);
	bool PropagateConstant(I2adl *ConstantObject, int BitOffset, int ConstantValue);
	I2adl *ResolveCollector(I2adl *CollectorObject, bool WaitForDownStreamCast);
	void RemoveSourceless(Node *TransportInputNode);
	bool UpgradeObject(I2adl *Object, I2adl *Resolver, bool DoRepropagate = true);
	bool ReplaceObject(I2adl *OldObject, I2adl *NewObject);
	VariantLevelEnum GetVariantLevel(NodeList *InputNodes);
	bool FlattenCollector(I2adl *CollectorObject);
	void RemoveSourceless(I2adl *Object);
	Node *FindTargetNode(Node *InputConnectedTo, int RequestedDataLength, int SinkOffset);
	void PropagateSuperSet(I2adl *Object, DataSet *SourceSuperSet);
	void FlattenExposer(I2adl *ExposerObject);
	void PropagateTransport(DataSet *DSet, I2adl *TransportObject);
	bool Repropagate(I2adl *Object);
	void RemoveSinkless(I2adl *DeadSink);
	void BuildConstant(unsigned char *Data, Node *TransportInputNode, int StartBit, bool
		BitReverse, VDismember *ParentHierarchy, AnsiString &ConstantDocumentation,
        int Sequence = -1);
	bool GenerateConstant(I2adl *Object, AnsiString DefaultValue = strNull,
    	bool UseGlobalHierarchy = true);
	bool AddInputObject(I2adl *InputObject, Node *Connection);
	void ResolvePrimitive(I2adl *Object);
	void CreateBitFanObjects(I2adl *Object);
	VDismember *ObjectTrace(I2adl *Object, bool UpdateGlobalVariables = true);
	void ExpandObject(I2adl *Object, I2adl *Resolver);
    bool PropagateNode(Node *OutputNode, DataSet *DSet = NULL);
	void Flatten();
	void GenerateConstants();
	bool AssignWidgets();
	bool RemoveBackToBackInverts();
	bool IsMergeableExposer(I2adl *Candidate);
	bool MergeExposerTrees();
    void LoadAndMergePossibleMergePoints(TStringList *PossibleMergePoints, I2adl *ExposerObject,
    	int BitOffset);
	void ReportUnresolvedObjects();
	void LoadGlobalLabelTable();
	void ConnectGlobalLabels(Node *DefNode, Node *RefNode);
	void InsertJoinGlobalsObject(Node *NewDefNode, VGlobalLabelEntry *GlobalLabel);
	void OptimizeGates(I2adlList *List);
	bool Synthesize();
	void AddConnectionElements(int index, I2adl *conxport, VList *InConnections);
	void ReExpand(I2adl *Object, I2adlList *matchingbehavior);
	void PackFD();
	void FDAdd(I2adl *I2adlObject, Node *Connection = NULL);
	void BLAdd(I2adlList *BaseList, I2adl *I2adlObject);
    void ResetFlattenList(bool digup = false);
    void PlaceObject(I2adl *I2adlObject, Node *Connection = NULL);
    void MakeNewHierarch(AnsiString MemberName, SystemString *Documentation);
};


class Path
{
public:

	Path            *Parent;              // Parent/higher path
	PathList        *SubPaths;            // List of child/lower paths
	SystemConnector	*SystemConnection;    // NULL identifies dummy paths
	VCost			IntermediateCost;     // Either the single level or cumulative path cost
	VCost			ThisCost;             // Cost of this path
	int             NodeIndex;            // Identifies the node that this path represents
	int             OnObjectIndex;        // Identifies the I2adl object that the node is on
	I2adl           *LogicalTransport;    // The I2adl object that the node is connected to
    int				StartPointDepth;	  // The subpath's level in the establish path tree.

	Path(SystemConnector *_Connection, VCost _IntermediateCost, int _NodeIndex
    	, int _OnObjectIndex, I2adl *_LogicalTransport);
	Path(SystemConnector *_Connection = NULL, VCost _IntermediateCost = VCost(0.0)
    	, Node *_Node =	NULL);
	Path(Path *_Path);
	~Path();

	Path *CopyPath(bool CopyDescendants);
	void __fastcall GetPathEndPoints(PathList *EndPoints);
	bool LoadSystemsVisited(SystemList *SystemsVisited, PathList *StartPoints, bool
    	LoadAllPaths, VivaSystem *MatchSystem, int PathDepth = 0);
	void AddIfNewSystem(SystemList *SystemsVisited, PathList *StartPoints);
	Path *__fastcall GetPathNode(VivaSystem *_VivaSystem);
	Path *__fastcall GetFirstEndPoint();
	Path *__fastcall GetHeadPath();
	void Instance(ResourceEnum ResourceMode);
	void Deinstance();
	void __fastcall Add(Path *_Path);
	VCost CalcTotalCost();
	void __fastcall CumulateCosts();
	void __fastcall Trim(VCost *LeastCost);
};

#endif


