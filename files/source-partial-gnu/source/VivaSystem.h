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

#ifndef VivaSystemH
#define VivaSystemH

#include "Globals.h"

#define UNKNOWN_TYPE        0
#define DEFAULT_TYPE        1  // No behavior
#define X86MAIN_TYPE        2  // Simulator
#define X86UI_TYPE          3  // Widgets, COM
#define FPGA_TYPE           4  // Target FPGAs via EDIF thru xylinx
#define PINCOM_TYPE         5  // Program up the boards with the correct lists of .bit

// will basicly Dispatch FPGA Vendors Place and Route Tools after doing I2adl to Vender
// spacific Netlist conversion
// points to FPGA resouse calculator prototype
#define IMPLEMENTATION_TYPE 6  // Creates constrainsts file for pin:Label Location resolution

// from the list of Transports in its behavior
// points to Pin Comunication calculator resouse prototype
#define FIXED_BUS_TYPE      7  // will create run image suitable for SSP and other bianary-State driven systems

// convert I2adl Behavior into state tables or instructions
// points to IMPLEMENTATION calculator resouse prototype
#define GENERIC_FILE_TYPE   8  // encapsulate the PCI ISA VISA ETHERNET and other (i.e. generic) block transfer devices

extern I2adlList    *SaveObjects;

class VivaSystem
{
protected:
    TStringList			*FAttributes;		// Attribute strings for this system
    TStringList			*FIncrementedAtts;	// List of attributes that have been incremented
    										//		Via the "++" suffix macro during this
                                            //		compile.  Created to ensure the increments
                                            //		are temporary (never saved in a system).

public:
    SystemString		*Name;				// Name of the system
    int					Type;				// System type code (see defined constants above)
    X86MainThread		*SysThread;			// System executor thread object
    Translator			*SysTranslate;		// System translator object
    ResourceManager		*SysResourceManager;// System resource manager object
    I2adlList			*SystemObjects;		// The ProjectObjects that have a System attribute
    										//		for this system
    I2adlList			*PrimitiveEDIFs;	// Copy of primitive EDIF ProjectObjects into each
    										//		FPGA system.  This lists owns its objects!
    SystemList			*SubSystems;		// List of child subsystems
    AnsiString			Location;			// Names of all parent systems separated by "//"
    SConnectorList		*OutConnectors;		// Source system connectors
    SystemConnector		*SelfConnection;	//
    VivaSystem			*ParentSystem;		// Parent system pointer
    I2adlList			*AssignedBehavior;	// Objects assigned to this system for execution
    IOLinkList			*IOLinks;			// List of I/O Links to other systems
    AnsiString			Documentation;		// System description/documentation
    int					IOAddress;			// I/O Address as extracted from attribute strings
    int					IOOffset;			// I/O Offset of next available pin.
    bool				SystemChanged;		// Has been modified since opened/saved.  Valid
    										//		for base system only.
    bool				BeingReplaced;		// Being replaced by another system, as opposed to
    										//		project destruction; only need to remove
                                            //		references to every object if this is true
#ifdef IMPLIMENTATION_EXTENSION
    int					InitFlag;
#endif

    VivaSystem(VTextBase &Loader, AnsiString _Name);
    VivaSystem(int InitLevel, VivaSystem *_ParentSystem, int _Type = UNKNOWN_TYPE, AnsiString
        _Name = strBaseSystem);
    VivaSystem(VivaSystem *Prototype, VivaSystem *_ParentSystem);

    ~VivaSystem();

    void SysInit();
    void __fastcall SetType(int);
    void __fastcall GetIOLinks(VivaSystem *TargetSystem, IOLinkList *List);
    void GetUsedUpCommunSystems(AnsiString &ProblemSystems);
    VivaSystem *__fastcall GetFirstSystemOfType(int _Type);
    void __fastcall ReportSubSystems(VList *FillList);
    void __fastcall AddImplementors();
    void __fastcall AssignIO();
    void __fastcall AssignIOAttributes();
    void LoadDefaultTargetSystem(bool WarnUser = false);
    int __fastcall CalcIOLocation(I2adl *);
    AnsiString __fastcall ParsePinLocation(AnsiString *FromString, int Index);
    void PropagateLinkInfo(IOLink *Link, I2adl *StartTransport);
    void GetConnectedTransports(I2adl *StartTransport, I2adlList *Transports);
    void __fastcall Reset(bool NewTranslator);
    void TranslatePrep();
    void Translate();
	void __fastcall Initialize();
#ifdef IMPLIMENTATION_EXTENSION
    void __fastcall FinishInitialize();
#endif
    bool Execute(bool ProcessingComEvent);
    void ReportPendingEvents();
    void __fastcall Stop();
    void __fastcall CalcLocation();
    void __fastcall BuildSystemConnectors(I2adl *DefaultTransport);
    VivaSystem *ResolveLocation(I2adl *Object);
    void AssignInfoRates();
    bool GetLocationOptions(I2adl *SearchForObject, I2adlList *Options
    	, int QuitCount = 0x1000000);
    VivaSystem *GetSystemWithLocation(const AnsiString &SearchLocation);
    VivaSystem *GetSystem(const AnsiString &_Name);
	void LoadX86SystemClocks();
    AnsiString __fastcall GetSymbol(AnsiString Symbol);
    void CommitReleaseOrCost(ResourceEnum ResourceMode, I2adl *Object);
    void SyncSystemName(AnsiString &FileName);
    void __fastcall SyncLists(bool WarnUser = false);
    bool __fastcall Add(VivaSystem *);
    bool __fastcall Remove(VivaSystem *);
    void __fastcall RemoveObject(I2adl *_I2adl);
    bool __fastcall Delete(VivaSystem *);
    VivaSystem *__fastcall InSystem(VivaSystem *SearchForSystem);
    bool IsObjectInSystem(I2adl *Object, VivaSystem *ExcludedSystem = NULL);
    I2adl *IsDataSetUsed(DataSet *DSet, I2adl **ParentObject);
    void __fastcall LoadAvailableResources();
    VivaSystem *__fastcall GetSystem(AnsiString Attribute, AnsiString _Attribute);
    void LoadAttributeSystems(TComboBox *AttributeComboBox);
    AnsiString GetAttribute(AnsiString AttName, bool CheckBaseSystem = false,
		bool IncludeProjectAttributes = true, I2adl *OrigFromObject = NULL);
    void __fastcall SetAttribute(AnsiString Attribute, AnsiString Value);
    void SetAttributeText(AnsiString Text);
    AnsiString GetAttributeText();
    void __fastcall LoadTreeView(TTreeNode *AtNode);
    void MakeEdifIDs();
};


class IOLink
{
public:

    VivaSystem	*ConnectionSystem;   // The system that enables other systems to connect.
    I2adl       *CompositeTransport; // Contains information about resources used.
    VivaSystem	*FromSystem;    	 // The system that is connecting to another system
    Node		*ActiveNode;    	 // The input/output node that is connecting to another
							    	 // system.  Its is in FromSystem->AssignBehavior.
    int			IOOffset; 			 //
    int			Type;     			 // 0 - ActiveNode is input node on an output object.
									 // 1 - ActiveNode is output node on an input object.
    IOLink		*ToLink;

    IOLink(VivaSystem *_ConnectionSystem, int _IOOffset, int _Type, VivaSystem *_FromSystem,
            Node *_ActiveNode, I2adl *_CompositeTransport);
};


class SystemConnector
{
public:

    VivaSystem		*ConnectedSystem;      // The system that the system connector connects to
    I2adl			*Transport;            // The system object that we were created from
    VivaSystem		*CommunicationSystem;  // Communication system that the system object is in
    ResourceList	*Resources;            // The list of required resources

    SystemConnector(VivaSystem *_ConnectedSystem, I2adl *_Transport, VivaSystem
        *_CommunicationSystem, ResourceList *_Resources);
    ~SystemConnector();

    void CommitReleaseOrCost(ResourceEnum ResourceMode, VCost *ConnectorCost, I2adl
        *LogicalTransport);
};

#endif
