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

#ifndef GlobalsH
#define GlobalsH

#include <vcl.h>
#include <ComCtrls.hpp>

#include "Utils.h"


// Compile one version of Viva by uncommenting a VERSION #define.
//
// Compile all versions of Viva by doing the following:
//		1.  If desired, change the program options to compile for release.
//		2.  Use "Export Makefile" (Project menu) to create a new Viva.mak file.
//		3.  Globals.h and all Viva*.exe programs must be (1) closed and (2) writeable.
//		4.  Close the Viva.bpr project in C++ Builder.
//		5.  Run CompileVivaPrograms.exe.
//
// Setup a new system description license by doing the following:
//		1.  Create a new VERSION_LICENSE_SD #define.
//		2.  Create a new global "str" variable (if necessary).
//		3.  Add the license logic to the OpenInputFile() routine.
//		4.  Add the license logic to the IsResourceDataAuthorized() routine.
//
#define  	VERSION_VIVA
//#define  	VERSION_VIVA_Run
//#define  	VERSION_VIVA_SD
//#define	VERSION		LICENSE_SD	strBenOne	// Skip because not being used.
//#define	VERSION		LICENSE_SD	strXSK      // Skip becaues not being used.

#ifdef VERSION_VIVA
    #define 	VIVA_IDE	  	true
    #define 	VIVA_SD	  		false
    #define 	LICENSE_SD		strNull
#else
#ifdef VERSION_VIVA_Run
    #define 	VIVA_IDE	  	false
    #define 	VIVA_SD	  		false
    #define 	LICENSE_SD		strNull
#else
#ifdef VERSION_VIVA_SD
    #define 	VIVA_IDE	  	true
    #define 	VIVA_SD	  		true
    #define 	LICENSE_SD		strNull
#else
    #define 	VIVA_IDE	  	true
    #define 	VIVA_SD	  		false
    #define 	LICENSE_SD		VERSION_LICENSE_SD
#endif
#endif
#endif


// Viva Enhancement compile parameters:
//     Turn off by prefixing with "//" (must create previous Viva.exe).
//     Must keep for at least 6 months.
//
//			Enhancement					Author		Date
  #define	IMPLIMENTATION_EXTENSION	KGilson		01/01/03
//#define	LOG_EXPANDED_OBJECTS		KTanner		03/07/05
//#define	TURN_OFF_REEXPANTION		KTanner		09/21/05
//#define	WRITE_TEXTBASE_VER_301		KTanner		08/03/06
//#define	WRITE_TEXTBASE_VER_302		KTanner		07/12/07


// DON'T REMOVE!  IMPORTANT DEBUG ARRAY!
extern int dbg[10];

typedef USHORT (CALLBACK *LPDLREADPORTUSHORT)(IN ULONG Port);
typedef USHORT (CALLBACK *LPDLWRITEPORTUCHAR)(IN ULONG Port, IN UCHAR Value);
typedef USHORT (CALLBACK *LPDLWRITEPORTUSHORT)(IN ULONG Port, IN USHORT Value);

#define DataSetList             					VList
#define DismemberList           					VList
#define I2adlListList           					VList
#define IndexList               					VList
#define IntegerList             					VList
#define IOLinkList              					VList
#define MenuItemList              					VList
#define ModalResultList         					VList
#define MyTMemoList                					VList
#define PathList                					VList
#define ResourceList            					VList
#define SConnectorList          					VList  // System Connector List
#define SystemList              					VList
#define TShapeList         							VList
#define WidgetList              					VList
#define SheetList               					VList
#define SheetStateList          					VList
#define TConnectionPointList    					VList
#define TruthTableEntryList    						VList

#define NodeString              					AnsiString
#define ResourceString          					AnsiString
#define ResourceTypeString      					AnsiString
#define SystemString            					AnsiString

#define MIN_MEMO_WIDTH          					ViewScale * 1.81
#define MIN_MEMO_HEIGHT         					5

#define MEMO_OFFSET_TOP         					1
#define MEMO_OFFSET_LEFT        					1 
#define NUM_MEMO_ATTRIBUTES     					8

#define RECENT_FILE_MIN         					0
#define RECENT_FILE_MAX         					99
#define RECENT_FILE_DEFAULT        					20

#define DEFAULT_INPUT_OUTPUT_WIDTH					(ViewScale * 5.41)

#define MIN_OBJECT_TREE_WIDTH   					180
#define CONTROL_DRAG            					(TComponent *) 1
#define SELECT_RECT_WIDTH       					3
#define CLEAN_PASS_LIMIT							5  // Must be >= 5.

#define OLD_GRID_SNAP								5
#define NODE_SEPARATION         					(ViewScale * 3)
#define DEFAULT_WORKSPACE_COLOR 					(TColor) 0xEBEBEB  // Almost white
#define DEFAULT_SELECTION_COLOR						clRed

#define SETTINGS_CONFIRM_DELETE_WIP_SHEET           0x01
#define SETTINGS_CLOSE_CHANGED_SHEETS   			0x02
#define SETTINGS_CONFIRM_CLOSE_UNNAMED_PROJECT      0x04
#define SETTINGS_AUTO_VERSION                       0x08
#define SETTINGS_SAVE_DEFAULT_NAME                  0x10
#define SETTINGS_SAVE_BEFORE_COMPILE                0x20
#define SETTINGS_SKIP_BACKUP_ON_SAVE				0x40
#define SETTINGS_SAVE_SYSTEM_OBJECTS                0x80
#define SETTINGS_MERGE_EQUIV_OBJECTS				0x100

#define SYNCRO_EVENT_NAME_SIZE 						40
#define MAX_INFO_RATE 								MAXINT
#define TIMEOUT_DEFAULT     						3000
#define COMPILE_ERROR_MAX							2000
#define ROUTE_SEGMENT_PARTIAL_COST 					1000.0
#define EDIF_NAME_LIMIT								240
#define EDIF_NAME_BUFFER_LIMIT						1024
#define LIBRARY_NODE_IMAGE_INDEX					3
#define SUBSYSTEM_NODE_IMAGE_INDEX					22


// Viva forward references
class AttributeCalculator;
class ChipData;
class VCost;
class DataSet;
class FixedBusThread;
class VGlobalLabelTable;
class I2adl;
class I2adlList;
class TI2adlStrTable;
class IOLink;
class MyTMemo;
class MyTScrollingWinControl;
class MyTShapeI2adl;
class MyTShapeTransport;
class Node;
class NodeList;
class Path;
class Project;
class Resource;
class ResourceManager;
class SearchEngine;
class StringTable;
class SystemConnector;
class SystemEditor;
class TMainForm;
class Translator;
class TVivaTimer;
class TWidget;
class VivaIcon;
class VivaSystem;
class X86MainThread;
class X86UIThread;
class TComHelper;
class TComObjectInfo;
class TMemberDesc;
class VOleControl;
class TComDispatch;
class TComObject;
class TLibInfo;
class VGlobalLabelEntry;
class TWidgetForm;
class TProjectObjectsList;
class FastString;
class TruthTableEntry;
class VDismember;
class VStringList;
class TNodeStrTable;
class VIniFile;
class VList;
class TComResource;
class VTextBase;

typedef MyTScrollingWinControl VI2adlView;

extern I2adlList                *FunctionDescription;

extern HINSTANCE 				hDlPortIO;               // Handle to DlPortIO.DLL
extern LPDLREADPORTUSHORT   	lpDlReadPortUshort;
extern LPDLWRITEPORTUCHAR   	lpDlWritePortUchar;
extern LPDLWRITEPORTUSHORT  	lpDlWritePortUshort;


namespace VivaGlobals
{
    enum MainWindowSettingEnum		{mwCentered, mwMaximized, mwLast};
    enum PropagationOperations		{poInvalidate, poSelect, poDataSets};
    enum DeleteDSetsEnum			{soNo, soYes, soPrompt};
    enum RunModeEnum				{rmEdit, rmExecute, rmCompile, rmProgramChips};

    enum HandleAmbigEnum
    {
        haRetainExisting,
        haReplaceExisting,
        haPrompt
    };

    enum ViewLayerEnum
    {
        vlObjectNames = 0x01,
        vlNodeNames   = 0x02,
        vlNodeColors  = 0x04
    };

    enum ExecuteTypeEnum
    {
        etNoEvent,
        etCompile,
        etTranslate,
        etInitialize,
        etExecute
    };

    enum FileTypeEnum			// Only valid file type are allowed to have positive values!
    {
        ftProject,
        ftSheet,
        ftLibrary,
        ftVSystem,
        ftComForm,
        ftCompiledProject,
        ftLast,					// Must be directly after last valid file type.
        ftUndetermined	= -1,
        ftMergeSystem	= -2,
    };

    enum WriteModeEnum
    {
        wmGUIandAttributes,		// Object lies in a behavior page.  GUI and attribute info is
        						//		written.
        wmTreeInfo,				// Object lies in a list of sheets.  GUI is not written, but
        						//		tree info is.
        wmEssentialOnly,		// System behavior page...all non-essential info suppressed.
        wmGUI					// System behavior page with X86UI type...GUI is written.
    };

    enum KeepObjectEnum			// For when to keep object during synthesis
    {
        koBothSides,			// Must be connected on both sides			// Must be first.
        koOneSide,				// Must be connected on either side
        koAlways,				// Need not be connected at all
        koLast					// Number of enumerations (Count)			// Must be last.
    };

    enum CopyNodesEnum			// Controls the copying of nodes when an I2adl object is copied
    {
        cnNo,					// Don't copy the object's input/output nodes.
        cnYes,					// Copy the object's input/output nodes.
        cnYesDisconnect			// Copy and disconnect the object's input/output nodes.
    };

    enum ResourceEnum			// Controls the committing, releasing and costing of resources
    {
        resCommit,				// Commit the resources
        resRelease,				// Release the resources
        resCost					// Cost the resources as if they were committed
    };

    enum ExposerCodes
    {
        EXPOSER_NOT,        // Neither
        EXPOSER_IN,         // Exposer
        EXPOSER_OUT         // Collector
    };

    enum BitFanCodes
    {
        bfcNotChecked,		// Object needs to be check to see if it needs helper objects
        bfcNo,				// Object does not have any Bit Fan helper objects
        bfcYes				// Object has one or more Bit Fan helper objects
    };

    enum SystemGeneratedEnum
    {
        sgNot,              // Not automatically created
        sgPredefined,       // Predefined basic data sets required by Viva
        sgCOM               // COM library data set
    };

    enum EncodePathEnum
    {
        epProjectBeforeViva,// Try to return $(DirName:Project) before $(DirName:Viva)
        epProjectAfterViva,	// Try to return $(DirName:Viva) before $(DirName:Project)
        epProjectNever		// Never return $(DirName:Project)
    };

    enum  WidgetNameEnum
    {
        wnButton,
        wnGraph,
        wnMemo,
        wnSpinEdit,
        wnTextBox,
        wnScrollBar,
        wnPaintBox,
        wnActiveX,
        wnLast				// Must be last
    };

    enum SearchCriteriaEnum
    {
        scName,
        scFootprint,
        scOverload
    };

    enum SGDataSetEnum
    {
        sdsNULL = 1,
        sdsInt = 2,
        sdsDInt = 3,
        sdsFloat = 4,
        sdsDouble = 5,
        sdsCurrency = 6,
        sdsDate = 7,
        sdsBinaryString = 8,
        sdsIDispatch = 9,
        sdsBool = 11,
        sdsComVariant = 12,
        sdsIUnknown = 13,
        sdsDecimal = 14,
        sdsSignedByte = 16,
        sdsByte = 17,
        sdsWord = 18,
        sdsDWord = 19,
        sdsQInt = 20,
        sdsQWord = 21,
        sdsComInt = 22,
        sdsComUInt = 23,
        sdsVoid = 24,
        sdsHResult = 25,
        sdsStringPtr = 30,
        sdsWideStringPtr = 31,

        sdsBit = 32,
        sdsVariant = 33,

        sdsDBit = 35,
        sdsNibble = 36,

        sds3Bit = 37,
        sds7Bit = 38,
        sds9BitF = 39,
        sds11Bit = 40,
        sds12BitF = 41,
        sds20Bit = 42,
        sds23Bit = 43,
        sds36Bit = 44,
        sds52Bit = 45,

        sdsWideChar = 46,
        sdsUnsupported = 47,
        sdsEnumeration = 48,
        sdsPointer = 49,

        sdsBIN = 50,	// Must be the first of the three and the three must be consecutive.
        sdsLSB = 51,
        sdsMSB = 52,
        sdsList = 53
    };

    enum ControlNodeEnum
    {
        ceGo = 0,
        ceWait,
        ceDone,
        ceBusy,
        ceLast
    };

    enum RenameMethodEnum
    {
        rmFromPopUP,
        rmByClickTwice,
        rmByDragging
    };

    enum WipSheetEnum
    {
    	wsNotAWipSheet,		// A regular object (not a Wip sheet)
        wsRegular,			// Normal Wip sheet (not current or compiled)
        wsCurrent,			// Wip sheet to be displayed when the project is opened
        wsCompiled,			// The Wip sheet that the persistence information is for
        wsClipBoard			// I2adl Editor clipboard information	`
    };

    enum TruthTableEnum		// Truth table entry components that have codes.
    {                       //
        ttInVal = 0,        // Warning!  The order of these codes must match the order of their
        ttOutVal,			// 			 error messages (#321, #322, #323 and #324).
        ttInConn,
        ttOutConn
    };

    enum FoundObjectEnum
    {
        foNotFound,			// The object identification line has not yet been found.
        foFoundStart,		// The object identification line has been found.
        foFoundEnd			// The next object identification line has been found (signaling
        					//		the end of the attribute lines for our object).
    };

    // These enumeration codes are used to perform fast string compares on common string names.
    //     Therefore the SetName() method should be used on I2adl objects and nodes.
    //
    //                          ****** Warning ******
    //
    // Names and order of these codes is very important.  They must conform to the I2adlNames[]
    //     array in I2adl.cpp.
    //
    enum NameCodes
    {
        NAME_OTHER = 0,      	// Holder for all non standard names

        // Predefined Objects come first
        NAME_INPUT,
        NAME_OUTPUT,
        NAME_TRANSPORT,
        NAME_AND,
        NAME_OR,
        NAME_INVERT,
        NAME_ASSIGN,
        NAME_SSELECT,
        NAME_JUNCTION,
        NAME_REF,
        NAME_DEREF,
        NAME_RELEASE,
        NAME_TEXT,				// NAME_TEXT must be the very last PREDEFINED_OBJECTS.

        NAME_VARIANT_IN,
        NAME_VARIANT_OUT,
        NAME_OPAD,
        NAME_DPAD,
        NAME_IPAD,
        NAME_IBUF,
        NAME_OBUF,
        NAME_IOTBUF,
        NAME_GND,
        NAME_VCC,
        NAME_MUXCY,
        NAME_XORCY,
        NAME_BUFT,
        NAME_FDCE,
        NAME_FDCPE,
        NAME_FDRE,
        NAME_FDRSE,
        NAME_MULT_AND,
        NAME_XORCY_D,
        NAME_XORCY_L,
        NAME_MUXCY_D,
        NAME_MUXCY_L,
        NAME_ORCY,
        NAME_SRLC16E,
        NAME_FDDRRSE,
        NAME_FDDRCPE,
        NAME_SYSCAST,
        NAME_LINKIN,
        NAME_LINKOUT,
        NAME_GLOBAL_CLOCK,
        NAME_WIRE,
        NAME_REGCLR,
        NAME_ADC,
        NAME_MULT18X18S,
        NAME_GO,
        NAME_STOP,
        NAME_SPAWN,
        NAME_DELAY,
        NAME_DISPATCH,
        NAME_COMCREATE,
        NAME_BIT_FAN_EXP,
        NAME_BIT_FAN_COL,
        NAME_REGULAR_EXP,
        NAME_REGULAR_COL,
		NAME_RAMBDP,
		NAME_RAMBSP,
        NAME_VOUCH_DATA_SET,

        NAME_CASTOUT,    		// NAME_CASTOUT must be first non-object name code (flags end of
        NAME_CASTIN,     		//     objects).  When $Cast objects are processed they are
                         		//     turned into either an exposer or a collector.
        EXECUTE_DATASET_IN,
        EXECUTE_DATASET_OUT,
        EXECUTE_UILINKED_INPUT,
        EXECUTE_UILINKED_OUTPUT,
        EXECUTE_TRAP,
        EXECUTE_COM,
        EXECUTE_STRING_CONSTANT
    };

    extern DataSet			*PatternDataSets[3][0x10000]; // Array of all pattern datasets
    												//		for quick access.  All initialized
                                                    //		to NULL.
	extern TStringList		*TokenList;				// Collection of pre-compressed EDIF ID
    												//		fragments; object name + X + Y

    extern DeleteDSetsEnum	DeleteUnusedDataSets;   // Delete unused datasets just after load
    extern HandleAmbigEnum	HandleAmbig_Sheet;      // How to handle ambiguous objects from
    												//		loaded sheets.
    extern HandleAmbigEnum	HandleAmbig_System;     // How to handle ambiguous objects from
    												//		loaded systems.
    extern RunModeEnum		RunMode;                // For what purpose Viva has been executed
    extern bool				AutoExpandObjectTree;	// Expand Object Tree to fit contents
    extern bool				CloseUnchangedSheets;   // Close unchanged WIP sheets on exit
    extern bool				IncludeCommSystems;     //
    extern bool				ConfirmCloseProjects;	// Ask if save on close of changed project
    extern bool				ConfirmCloseSystems;	// Ask if save on close of changed system
    extern bool				AreShapesMoving;        // I2adl Shapes in are in move mode
    extern bool				IsRunning;              // A compiled behavior is running
    extern bool				HidingTreeGroups;		// Hiding '$' tree groups in object tree
    extern bool				SortingTreeByName;      // Alpha sort on project object tree
    extern bool				ShowingObjectAttributes;// Show/hides object attributes in I2adl
    												//		Editor
    extern bool				Initializing;           // In pre-execution init phase
	extern bool				IsStopping;             // Attempting to stop a running behavior
    extern bool				StopSignalsSent;        // Signals from Stop objects have been
    												//		sent to notify behavior to clean up.
	extern bool				IsEDIFExport;			// EDIF export vs. normal compile
    extern bool				MergeEquivObjects;		// Merge equivalent objects during synthesis
    extern bool			  	OptimizeGatesOpt;       // User has opted to optimize gates
    					  							//		(rearranging OR and AND object
													//		trees into be binary).
    extern bool			  	RunAfterCompileOpt;     // User has opted to hava Viva
    												//		automatically run after compile.
    extern bool				RunAfterCompile;        // Automatically run after compile; has
    												//		factors other than user setting.
    extern bool				EnforceVariantRes;      // Force variant composite objects to
													//		resolve before expansion; this is
													//		critical for ensuring predicable
													//		behavior and output	of synthesis.
                                                    //		An option ONLY to facilitate
                                                    //		backward compatability.
    extern bool				BuiltTokenList;         // EDIF ID compression map has already been
    												//		established and optimized.
    extern bool				ShouldMakeEdifIDs;      // User has opted to have Translator make
    												//		a compressed EDIF ID based on object
                                                    //		hieracrchy, rather than using an
                                                    //		incremented index, to make name
                                                    //		unique.
    extern bool				CreateVexFile;      	// User has opted to have compiler generate
    												//		a ".Vex" file instead of saving
                                                    //		persistence inside the project file.
    extern bool             LoadCoreLib;			// Should CoreLib.ipg be automatically
    												//		loaded into all new projects?
    extern bool				StaticSystemDesc;		// Should system desc info be copied into
                                                    //		the project save file?
    extern bool             LoadLastProject;		// Should the last project be reloaded at
    												//		Viva start up?
    extern AnsiString		LastProjectName;		// Name of the project opened in Viva.

	extern int				NonModalErrorCount;     // Count of all non-modal messages issued
    												//		during a compile; tells us when we
                                                    //		are likely in an infinite loop;
                                                    //		have to stop issuing messages at
                                                    //		some point.
    extern int				HideTreeGroupIcon;      //
    extern int				HideTreeViewIcon;       //
    extern int				PlayStopSheetIcon;      //
    extern int				OldDataSetCount;        // Used after loading to determine which
    												//		Datasets are new.
    extern int				OldProjectObjectCount;  // Serves same purpose as OldDataSetCount,
    												//		but for ProjectObjects.
    extern int				SettingFlags;      		// A bunch of boolean settings flags mashed
    												//		into one variable.  A mess that I'm
                                                    //		not proud of, and intend to change.
    extern int				MainWindowSetting;      // For main window to start up centered,
    												//		maximized, or last size/position.
    extern int				MaxRecentFiles[ftLast]; // Maximum number of recent file entries
    												//		for each file type.
    extern int				RecentFiles[ftLast];    // Actual number of recent file entries
    												//		for each file type.
    extern int				FileTypeMenuOrder[ftLast];// Orders the FileTypeEnum as they appear
    												//		on the main drop down menu.
    extern const double		TextBaseVersion;		// Current text base version, for file
    												//		compatibility purposes.
    extern int				NodeSnapTolerance;      // Just how far away a user can click from
    												//		a node, and still be close enough.
    extern int				ExpansionCount;       	// Number of object expansions that have
    												//		taken place during a synthesis.
    extern int				EmulatorSpeed;			// EmulatorSpeed attribute from the base
    												//		system (times to refresh per
                                                    //		second).  If 50+, then also reduces
                                                    //		the amount of CPU time used.
    extern int				EmulatorPeriod;			// EmulatorSpeed converted to miliseconds.
    extern int				InitClockCycles;		// InitClockCycles attribute from the base
    												//		system (number of clock cycles to
                                                    //		perform during initialization).
    extern ExecuteTypeEnum	TimedEventType;         // What process is being, or is to be,
    												//		performed in the compile/run process
    extern SearchEngine		*MainCompiler;          // The one and only compiler that we ever
													//		instantiate.
    extern HANDLE			ThisProcessEvent;       // 

    extern VivaSystem		*BaseSystem;            // The main system of this project
    extern VivaSystem		*DefaultTargetSystem;   // System to which objects are assigned,
    												//		that do not specify a system.
    extern VivaSystem		*UISystem;				// User Interface (X86UI) System.
    extern VivaSystem		*CPUSystem;				// X86CPU System (simulator).
    extern VivaSystem		*ATIOSystem;			// ATIO System.
    extern VivaSystem		*XPointSystem;			// XPOINT System.

	extern TWidgetForm		*WidgetForm;            // The one and only widget form

    extern int				ColorCodeBIN;           // Node color for BIN Pattern Datasets
    extern int				ColorCodeLSB;           // Node color for LSB Pattern Datasets
    extern int				ColorCodeMSB;           // Node color for MSB Pattern Datasets
    extern AttributeCalculator	*AttCalculator;		// Global attribute calculator object
    extern I2adlList		*X86SystemClocks;		// All emulator clocks in the X86UI/CPU
    												//		systems
    extern AnsiString		*JustCompiledObjectName;// For status bar display
    extern TVivaTimer		*ProcessTimer;          // Facilitates asynchronous execution of
    												//		compiler and executor processes.
                                                    //		Not sure why that's useful...
    extern VGlobalLabelTable *GlobalLabels;			// List of global label names that have
    												//		been defined or referenced.
    extern VList			*RuntimeWideStrings;	// Wide strings generated during compile
    												//		and execution.
    extern VList			*UntriggeredStopList;   // Stop objects that have not yet been given
    												//		the shutdown signal
    extern VList			*ComDispatchInvokeList;	// Allows TComDispatch::Invoke() and
    												//		X86MainThread::Event() to communicate
    extern TStringList		*DatasetTreeGroups;		// For "Change Tree Group" command.
    extern TStringList		*ObjectTreeGroups;		// List of all tree groups in the project
    												//		and system object trees.
    extern TStringList		*ComLibraries;          // All libraries presently loaded for use
    												//		in project; both LibIDs and pointers
                                                    //		to TLibInfo instances.
    extern TStringList		*ExpandedNodes;         // Captions of tree nodes that are "open";
    												//		recorded for to restore tree state
                                                    //		after modification.
    extern Project			*MyProject;             // The one and only Project instance
    extern VI2adlView		*I2adlView;             // The one and only MyTScrollingWinControl
    												//		instance; the I2adl Editor.
    extern VIniFile			*IniFile;               // $(DirName:User)viva.ini, where we keep
    												//		our settings
    extern bool				CancelCompile;			// User or internal process is trying to
    												//		halt compilation
    extern I2adlList		*CompressBaseList;      //
    extern TStringList		*ResolvedObjects;       // List of all context-specific I2adl
    												//		resolutions, with associated
                                                    //		Keystring.
    extern I2adl			*ParentI2adlObject;		//
    extern I2adl			*MainInputObject;       //
    extern I2adl			*MainOutputObject;      //
    extern I2adl			*MainSystemCastObject;  //
    extern VDismember		*Hierarch;              // Dismember of last I2adl object to be
    												//		traced.  Serves as parent info for
    												//		the next trace.
    extern DismemberList	*HierarchList;          // Keep track of VDismembers, only for to
    												//		delete them.
    extern MenuItemList		*LocateObjects;			// List of dynamically created LocateObject
    												//		popup menu items.
    extern IntegerList		*DisabledErrors;        // Messages disabled by the user during this
    												//		run of viva
    extern ModalResultList	*DisabledModalResults;  // Modal results of prompts for messages
    												//		that are disabled
    extern TStringList		*ErrorMessages;			// Contains the entire ErrorMessage file

    // Viva Icon Variables
    extern AnsiString		IconFileName;			// Filename for icon of object in edit
    extern AnsiString		BackupFileName; 		// Backup filename for icon of object in
    												//		edit
    extern VivaIcon			*TransportCorners[4];   // Transport Shape corner icons
    extern TI2adlStrTable	*I2adlStringTable;      // VivaIcons for all I2adl objects
	extern VivaIcon			*CastNameInfo;			// "Cast"
    extern VivaIcon			*SysCastNameInfo;		// "$Cast"
    extern VivaIcon			*ExposerIcon;           // Important only for icon, not name
    extern VivaIcon			*CollectorIcon;         // Important only for icon, not name
    extern VivaIcon			*ImplementorIcon;       // Important only for name, not icon
    extern VivaIcon			*TransportHor;			// Horizontal transport icon
    extern VivaIcon			*TransportVer;          // Vertical transport icon

    // Global Project Variables
    extern TProjectObjectsList	*ProjectObjects;    // All I2adl object defs in this project
    extern DataSetList		*ProjectDataSets;       // All Datasets defined in project

    // Global data set variables
    const int				SGDataSetCount = 54;	// Number of system-generated Dataset types
    extern AnsiString		SGDataSetNames[SGDataSetCount]; // Names of system-generated Dataset
    														//		types.
    extern DataSet			*SGDataSets[SGDataSetCount]; // All non-pattern system-generated
    													 //		Datasets.
    extern DataSet			*BitDataSet;
    extern DataSet			*NULLDataSet;           // Note: incorrect mix of Viva and COM usage
    extern DataSet			*VariantDataSet;
    extern DataSet			*ListDataSet;

    extern AnsiString		strTreeGroup;
    extern AnsiString       strViewControl;
    extern AnsiString       strPrimitive;
    extern AnsiString       strIconIndex;
    extern AnsiString       strCurrentSheet;
    extern AnsiString       strCompileSheet;
    extern AnsiString       strLink;
    extern AnsiString       strInfoRate;

    extern AnsiString		strLibrary;
    extern AnsiString		strSystemFile;			// Legacy name for backward compatibility.
    extern AnsiString		strDynamicSystemFile;
    extern AnsiString		strStaticSystemFile;
	extern AnsiString		strMainWidgetTab;
	extern AnsiString		strAppTitle;
	extern AnsiString		strWipSheet;
	extern AnsiString		strClipBoard;
	extern AnsiString		strCopyOf;
    extern AnsiString		strIndex;
	extern AnsiString		strDefault;
	extern AnsiString		strDeactivatedDefault;
	extern AnsiString		strCompiler;
	extern AnsiString		strRunAfterCompile;
    extern AnsiString		strOptimizeGates;
    extern AnsiString		strEnforceVariantResolution;
	extern AnsiString		strMakeEdifIDs;
	extern AnsiString		strCreateVexFile;
    extern AnsiString		strSettingsDialogTab;
    extern AnsiString		EdifExportFileName;
	extern AnsiString		strDispatch;
    extern AnsiString		CMDLineCom;
    extern AnsiString		UserDir;
    extern AnsiString		TempDir;
    extern AnsiString		StartUpDir;
    extern AnsiString		KeyPrefixes[ftLast];
    extern AnsiString		strBasicDataSets;
    extern AnsiString		strCOMDataSets;
    extern AnsiString		strPrimitiveEDIFs;
    extern AnsiString		strPrimitiveObjects;
    extern AnsiString		strCompositeObjects;
    extern AnsiString		strLibraries;
    extern AnsiString		strVivaSystemDir;
    extern AnsiString		strHelpDir;
    extern AnsiString		strObjectDocDir;
    extern AnsiString		strMessageDocDir;
    extern AnsiString		ErrorTrapFileName;
    extern AnsiString		strVivaDir;
    extern AnsiString		strLicenseDll;
    extern AnsiString		strResourceData;
    extern AnsiString		DefaultResourceData;
    extern AnsiString		strIconsDir;
    extern AnsiString		DefaultSystemDesc;
    extern AnsiString		VivaRunFileName;
    extern AnsiString		strSystem;
    extern AnsiString		strResource;
    extern AnsiString		strResourceQty;
    extern AnsiString		strChildAttribute;
    extern AnsiString		strObjectRef;
    extern AnsiString		strProvidesResource;
    extern AnsiString		strConstant;
    extern AnsiString		strDeactivatedConstant;
    extern AnsiString		strWidget;
    extern AnsiString		strActiveXWidget;
    extern AnsiString		strWidgetProperty;
    extern AnsiString		strWidgetEvent;
    extern AnsiString		strWidgetLeft;
    extern AnsiString		strWidgetTop;
    extern AnsiString		strWidgetWidth;
    extern AnsiString		strWidgetHeight;
    extern AnsiString		strWidgetMin;
    extern AnsiString		strWidgetMax;
    extern AnsiString		strWidgetHex;
    extern AnsiString		strWidgetTabSheet;
    extern AnsiString		strWidgetTextBox;
    extern AnsiString		strTrue;
    extern AnsiString		strFalse;
    extern AnsiString		strIOAddress;
#ifdef IMPLIMENTATION_EXTENSION
    extern AnsiString		strTranslator;
    extern AnsiString		strTranslatorExclusive;
    extern AnsiString		strInitializer;
    extern AnsiString		strInitializerExclusive;
    extern AnsiString		strPostInitializer;
    extern AnsiString		strPostInitializerExclusive;
    extern AnsiString		strExecutor;
#endif
    extern AnsiString		strPadLoc;
    extern AnsiString		strInst;
    extern AnsiString		strNet;
    extern AnsiString		strTIG;
    extern AnsiString		strTNM;
    extern AnsiString		strLibRef;
    extern AnsiString		strInstanceName;
    extern AnsiString		strEdifFile;
    extern AnsiString		strTIMESPEC;
    extern AnsiString		strPartType;
    extern AnsiString		strIORef;
    extern AnsiString		strIcon;
    extern AnsiString		strEndOfSystems;
    extern AnsiString		strNewProject;
    extern AnsiString		strBaseSystem;
    extern AnsiString		strDisplayPrototype;
    extern AnsiString		strFileName;
    extern AnsiString		strTrap;
    extern AnsiString		strExclusive;
    extern AnsiString		strDocumentation;
    extern AnsiString		strDisplayUsage;
    extern AnsiString		strNull;
    extern AnsiString		strIniSet;
    extern AnsiString		strEnabledMessages;
    extern AnsiString		strShowAllWarnings;
    extern AnsiString		strSettings;
    extern AnsiString		strMaxRecentFiles;
    extern AnsiString		strAutoExpandObjectTree;
    extern AnsiString		strSettingFlags;
    extern AnsiString		strCloseUnchangedSheets;
    extern AnsiString		strConfirmCloseProjects;
    extern AnsiString		strConfirmCloseSystems;
    extern AnsiString		strWindow;
    extern AnsiString		strWindowColor;
    extern AnsiString		strSelectionColor;
    extern AnsiString		strStartMaximized;
    extern AnsiString		strMainForm;
    extern AnsiString		strMainWindowRect;
    extern AnsiString		strWipTreeHeight;
    extern AnsiString		strWipTreeWidth;
	extern AnsiString		strWidgetFormLeft;
	extern AnsiString		strWidgetFormTop;
    extern AnsiString		strMainWindowSetting;
    extern AnsiString		strMessageGridHeight;
    extern AnsiString		strNodeSnapTolerance;
    extern AnsiString		strDeleteUnusedDataSets;
    extern AnsiString		strHandleAmbig_Sheet;
    extern AnsiString		strHandleAmbig_System;
    extern AnsiString		strHideTreeGroups;
    extern AnsiString		strShowTreeGroups;
    extern AnsiString		strHideTreeView;
    extern AnsiString		strShowTreeView;
    extern AnsiString		strChevronButton;
    extern AnsiString		strKeepObject;
    extern AnsiString		strIn;
    extern AnsiString		strOut;
    extern AnsiString		strData;
    extern AnsiString		strType;
    extern AnsiString		strWarnOnRemoval;
    extern AnsiString		strCast;
    extern AnsiString		strParentDataSets;
    extern AnsiString		strSpace;
    extern AnsiString		strPeriod;
    extern AnsiString		strBackSlash;
    extern AnsiString		strDollarSign;
    extern AnsiString		strBackSlashDollarSign;
    extern AnsiString		strCRLF;
    extern AnsiString		strBuildDir;
    extern AnsiString		strCompileTimeSystem;
    extern AnsiString		strIOBuf;
    extern AnsiString		strMessage;
    extern AnsiString		str_TreeGroup_;
    extern AnsiString		strFromObject;
    extern int				strFromObjectLen;
    extern AnsiString		strLoadCoreLib;
    extern AnsiString		strStaticSystemDesc;
    extern AnsiString		strLoadLastProject;
    extern AnsiString		strLastProjectName;
    extern AnsiString		strReverseBitFans;
    extern AnsiString		strMin;
    extern AnsiString		strMax;
    extern AnsiString		strDefaultTargetSystem;
    extern AnsiString		strJoinGlobals;
    extern AnsiString		strJoinGlobalsDeletePri;
    extern AnsiString		str_NONE_;
    extern AnsiString		strPromoteToParentObject;
    extern AnsiString		strConstTruthFile;
    extern int				strConstTruthFileLen;
    extern AnsiString		strConstTruthTable;
    extern int				strConstTruthTableLen;

    extern AnsiString		strStatic;
    extern AnsiString		strComPredefined;
    extern AnsiString		strCannotLoad;

    extern AnsiString		strSystemBIN;
    extern AnsiString		strSystemLSB;
    extern AnsiString		strSystemMSB;
    extern AnsiString		strInputHornName;
    extern AnsiString		strOutputHornName;
	extern AnsiString		strChooseDataSets;
	extern AnsiString		strApplyDSToRest;
    extern AnsiString		strI2adlEditorClipBoard;

    extern AnsiString		strViva;
    extern AnsiString		strVivaRun;
    extern AnsiString		strVivaSD;
    extern AnsiString		strX86;
    extern AnsiString		strBenOne;
    extern AnsiString		strXSK;

    extern AnsiString		FilterStrings[ftLast + 1][2];
    extern AnsiString		KeepObjectOptionStrings[koLast];
    extern AnsiString		*ControlNodeNames[ceLast]; // "Go", "Wait", "Done", "Busy"

    extern AnsiString		OrigOpenAsSheetCaption;
    extern AnsiString		OrigEditAttributesCaption;
    extern AnsiString		OrigDeleteObjectCaption;
    extern AnsiString		OrigLocateObjectCaption;

    extern TNodeStrTable	*NodeStringTable;
    extern NodeString		*NodestrNull;
    extern NodeString		*NodestrGo;
    extern NodeString		*NodestrDone;
    extern NodeString		*NodestrBusy;
    extern NodeString		*NodestrWait;
    extern NodeString		*NodestrClock;
    extern NodeString		*NodestrClockState;
    extern NodeString		*NodestrSysClock;
    extern NodeString		*NodestrAddConstant;
    extern NodeString		*NodestrHold;
    extern NodeString		*NodestrIn;
    extern NodeString		*NodestrIn0;
    extern NodeString		*NodestrIn1;
    extern NodeString		*NodestrIn2;
    extern NodeString		*NodestrIn3;
    extern NodeString		*NodestrOut;
    extern NodeString		*NodestrOut1;
    extern NodeString		*NodestrOut2;
    extern NodeString		*NodestrOut3;
    extern NodeString		*NodestrType;
	extern NodeString		*NodestrException;

    extern StringTable		*ResourceStringTable;
    extern ResourceString	*ResourcestrNull;
    extern ResourceString	*ResourcestrComposite;
    extern ResourceString	*ResourcestrTimeSlice;
    extern ResourceString	*ResourcestrAtomicTransport;
    extern ResourceString	*ResourcestrTempConnectObject;
    extern ResourceString	*ResourcestrCLB;
    extern ResourceString	*ResourcestrGlobal;
    extern ResourceString   *ResourcestrGlobalIS;

    extern StringTable		*SystemStringTable;
    extern SystemString		*SystemstrNull;
    extern SystemString		*SystemstrWidgetSystem;
    extern SystemString		*SystemstrQMark;

    extern StringTable          *ResourceTypeStringTable;
    extern ResourceTypeString   *ResourceTypestrTDM;
    extern ResourceTypeString   *ResourceTypestrPINS;

    AnsiString GetBuildDir();
    void SetUserDir();
    void SetTempDir();
    void SetStartUpDir();
    void RunProject();
    AnsiString ReplaceAll(AnsiString Text, const AnsiString &Out, const AnsiString &In);
    void ReallocateDC(TFont *Font);
    void AlignToGrid(int& Data);
    void ColorChange(TEdit *Edit, TEdit *ColorEdit, TColor &NewColor);
    void UpdateColor(TEdit *Edit, TColor &NewColor);
    void UpdateColorFormat(TEdit *Edit, TColor &NewColor);
    int  VStrToInt(AnsiString String, int DefaultValue = 0);
    bool VivaExec(bool IWait, AnsiString FileName, UINT Param, int *ReturnCode);
 	void LoadX86SystemClocks();
 	void StartExecution(AnsiString VexFileName = strNull);
    void RebuildStringTables();
    void PlaceJunctionNodes(Node *InputNode, int InputPosition);
    bool EventCycle(bool ProcessingComEvent);
    void GetUniqueEventName(char *EventName,unsigned long ProcessID);
    bool TryHaltExecution();
    void HaltExecution();
    FileTypeEnum GetFileType(AnsiString FileName);
    void InitProgram();
    void UnInitProgram();
    void SetStatusCaption(AnsiString Cap, bool ProcMsg = false);
    void SetProgPos(int Pos, int Max = -1);
    bool NodeListEqual(NodeList *IA, NodeList *IB, bool CompareNames = false);
    void ComputeDataSetLengths();
    void CheckDataSetUsage();
    void ClearTreeView(TTreeView *TreeView);
    void FullCustomSort(TTreeNode *TreeNode, bool ExpandNode = false);
    int __stdcall CustomSortCompare(TTreeNode *Node1, TTreeNode *Node2, long Dummy);
    int GetNodeSortLevel(TTreeNode *TreeNode);
    void SetImageIndex(TTreeNode *TreeNode, int ImageIndex);
    bool InsideRootTreeNode(AnsiString &RootTreeNodeName, TTreeNode *TreeNode, bool
        MustBeTreeGroup = false);
    TTreeNode *AddObjectToTree(TTreeNode *ParentNode, I2adl *Object);
    void InitSGDataSets();
    void GetOrMakeDataSet(SGDataSetEnum _Name, AnsiString TreeGroup, SGDataSetEnum
        ChildDataSet1, SGDataSetEnum ChildDataSet2 = sdsNULL, int _ColorCode = clBlack,
        unsigned int _AttributeFlags = ATT_UNSIGNED, SystemGeneratedEnum _SystemGenerated
        = sgPredefined);
    DataSet *GetOrMakeDataSet(AnsiString _Name, AnsiString TreeGroup, SystemGeneratedEnum
        _SystemGenerated, DataSet *ChildDataSet1, DataSet *ChildDataSet2 = NULL, int
        _ColorCode = clBlack, unsigned int _AttributeFlags = ATT_UNSIGNED);
    DataSet *GetOrMakePatternDataSet(SGDataSetEnum Pattern, int DataSetBitLength);
    DataSet *GetDataSet(AnsiString &_Name);
    SGDataSetEnum GetPatternType(AnsiString &DataSetName, int &PatternSize);
    void RemoveBehavior(I2adlList *DefunctBehavior, bool ResolveRefs = true
    , I2adlList *ReplaceWith = NULL);
    AnsiString EncodePath(AnsiString Path, EncodePathEnum ProjectDirPrecedence =
    	epProjectAfterViva);
	AnsiString FullPathName(const AnsiString &FileName);
    void BringUIFormsToFront();
	AnsiString FixData(double ff, int lowbits, int totbits, char fchar = 'f');
    bool CheckForWrite(const AnsiString &FileName, bool AutoOverwrite);
	void ShowHelpFile(AnsiString HelpFileName, bool ObjectDocumentation);
    TStringList *ParseString(AnsiString String, AnsiString Separator, bool TrimPieces = true,
    	bool SkipNullStrings = true);
};

using namespace VivaGlobals;

bool LocateFile(const AnsiString &FileName);
bool BrowseFile(AnsiString &FileName, bool AllowCancel = true, AnsiString FilterString =
	strNull);
int RecurseCodeToInt(AnsiString RecurseCode);
void RegisterComponent(TComponentClass &xclass);


class StopException
{
};


class StringEntry
{
public:
    AnsiString      Name;
    void            *Object;

    StringEntry(const AnsiString &SearchString);
};


class StringTable : public TList
{
public:
    _fastcall StringTable();
    void __fastcall BeforeDestruction();

    StringEntry *Entry(const AnsiString &SearchString, bool AddIfNotFound = true);
    // Backward compatibility for when StringEntry is a simple AnsiString
    inline AnsiString *QString(const AnsiString &SearchString)
    {
        return &(Entry(SearchString)->Name);
    }
    virtual StringEntry *NewEntry(const AnsiString &NewString);
};


class TNodeStrTable : public StringTable
{
public:
	virtual StringEntry *NewEntry(const AnsiString &NewString);
};


class TVivaTimer : public TTimer
{
public:
    __fastcall TVivaTimer(TComponent *AOwner) : TTimer (AOwner) {OnTimer = TimerTimer;};
    void __fastcall TimerTimer(System::TObject *Sender);
};

// Returns true when the source of the caption data changes.
//     Call this routine before updating Caption to increase performance and
//     reduce flutter.
bool NewCaptionSource(void *Object);


#endif

