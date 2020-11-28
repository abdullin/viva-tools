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

#ifndef ComManagerH
#define ComManagerH
#include "VivaComForm.h"
#include "ComCat.h"

#define DEFAULT_COM_DATASET_COLOR 0xAA00FF
#define COM_GLOBALS_ROOT_NODE (void *) -1

enum MemberTypeEnum
{
    mtProperty = 0,
    mtFunction = 0x1,
    mtEvent = 0x3       // 3 rather than 2 so that mtEvent "contains" mtFunction.
};


enum ComResourceEnum
{
    crsLibrary,
    crsForm,
    crsObject
};


// Class to wrap the ITypeLib interface provided by the Windows API, as well as external
//		components (file name, ID, etc) of a com library.
//		Localizes a lot of data availabe from the ITypeLib interface.
class TLibInfo
{
public:
    AnsiString		FileName;		// Physical location of the com library.
    AnsiString      OrigFileName;   // File name specifed in the Text Base file, whether correct
    								//		or not.  Needed as a placeholder.
    AnsiString		LibID;          // Windows ID for library.  Ex.: VIVASTANDARDOBJECTSLib
    AnsiString		HelpFileName;   // Location of help file, if existent.
    ULONG			HelpContext;	// ID for help context, as for a specific object or member
    ITypeLib		*TypeLib;		// The com interface into the type library, once loaded.
    								//		This is a WINAPI interface, on which all data
                                    //		on the library can be queried.
    TStringList		*ObjectList;    // Extracted list of types (classes, interfaces, records,
    								//		etc) defined in the library.

    TLibInfo(AnsiString _FileName, bool bPromptOnError = false);
    TLibInfo(TLibInfo *LibInfo);
    TLibInfo(ITypeLib *_TypeLib, AnsiString _LibID);
    ~TLibInfo();
    void GetObjectList();
};


// Holds all data necessary for both runtime and design-time management of global
//      instances of COM Objects.
class TComObjectInfo
{
public:
    __property AnsiString VivaProgID = {read = GetVivaProgID};

    BSTR           ProgID;          // Actual Progammatic ID, used in CreateObject and
    								//		ITypeInfo::CreateInstance
    AnsiString     ClassName;       // Name of COM class of this object (from the type library)
    TLibInfo       *OwningLibInfo;  // All com classes reside in a library; this points to ours.
    ITypeInfo      *TypeInfo;       // Interface for acquisition of data of this type
    								//		and any type that it uses.
    ByteBool       IsControl;       // Is or is not an ActiveX Control.
    TYPEATTR       *TypeAttr;       // Attributes of the class; managed by constructor and
    								//		destructor.
    TStringList    *MemberList;     // List of member names; [Data] member is a TMemberDesc *

    TComObjectInfo(ITypeInfo *_TypeInfo, TLibInfo *_OwningLibInfo);
    ~TComObjectInfo();
    void GetMemberList(ITypeInfo *TypeInfo, bool AreEvents, TStringList *MemberList);
	void RegisterTypeLib();
	IDispatch *Instantiate();
	bool Implements(ITypeInfo *_TypeInfo);

    AnsiString GetVivaProgID()
    {
        return OwningLibInfo->LibID + "::" + ClassName;
    }
};


// Contains all information necessary for governing of a specific Com Object member
//		(property, function, or event).
//		This class isn't as self-managed as would be optimal.
class TMemberDesc
{
public:
    union
    {
        FUNCDESC    *FuncDesc; // This MemeberDesc wraps a function or event.
        VARDESC     *VarDesc;  // This MemeberDesc wraps a property.
    };

    BSTR              RawName;  			// Member name as given by the ITypeInfo interface.
    AnsiString        Name;					// More usable copy of the member name.
    MemberTypeEnum    Type;     			// Property, Function, or Event (a type of Function)
    TComObjectInfo    *OwningObjectInfo;	// Need to know where we came from.
    ITypeInfo         *OwningTypeInfo;      // Used for to extract and localize data about
    										//		this member
    int               Index;                // Index in owning object info's member list

    TMemberDesc(TComObjectInfo *ObjectInfo, ITypeInfo *TypeInfo);
    TMemberDesc(TMemberDesc *MemberDesc);
    ~TMemberDesc();
    DISPID GetMemID();
	TYPEDESC *GetTypeDesc();
	void ResolveRefs(ITypeInfo *TypeInfo, TYPEDESC *TypeDesc);
};


// Holds information for Connection Points, which are to be deactivated upon cleanup.
struct TConnectionPointEntry
{
    IConnectionPoint    *CP;			// Connection Point in a com event interface.
    ULONG               ConnectionID;   // ID tag for one particular connection
};


// Maintains information for interfaces implemented by Com Class Objects:
struct TImplTypeInfo
{
    ULONG       RefType;		// ID Tag for this class
    ITypeInfo   *TypeInfo;      // Interface for querying this class
    int         ImplFlags;      // Flags for what this interface is implented as.  We use in
    							// 		order to see if this is the main interface for a class.

    TImplTypeInfo(int Index, ITypeInfo *TypeInfo);
};


// Converges various Com resource types for portability in dependency checking.
class TComResource
{
public:
    ComResourceEnum    Type;

    union
    {
        TLibInfo      *LibInfo;
        TComForm      *Form;
        TComObject    *Object;
    };

    TComResource(TLibInfo *_LibInfo)
    {
        Type = crsLibrary;
        LibInfo = _LibInfo;
    }

    TComResource(TComForm *_Form)
    {
        Type = crsForm;
        Form = _Form;
    }

    TComResource(TComObject *_Object)
    {
        Type = crsObject;
        Object = _Object;
    }

    TComResource()
    {
    };
};


namespace ComManager
{
    const DISPID              dispidPropertyPut = DISPID_PROPERTYPUT;
    const int                 MaxNameCount = 50; 		// Highest number of function parameters
    											 		//		that we'd ever care to look at.
    extern AnsiString         strEvent;
    extern AnsiString         strGet;
    extern AnsiString         strSet;
    extern AnsiString         strComLibrary;
    extern AnsiString         strComForm;
    extern AnsiString         strComObject;
    extern AnsiString         strProgID;
    extern AnsiString         strDispatchIndicator;
    extern AnsiString         strComCreateIndicator;
    extern AnsiString         FuncKindNames[];			// Names of function types
    extern AnsiString         VarKindNames[];           // Names of variable types
    extern AnsiString         ObjectTypeNames[];        // Names of types in a com library
    extern VList              *ComObjects;				// List of all existent com instances
    													//		for dependency checking, and
                                                        //		linking from a string reference
    extern VList              *ComI2adlList;			// List of all existent com I2adls,
    													//		for same purpose as ComObjects
    extern VList              *GlobalComObjects;		// Same purpose as ComObjects
	extern TStringList        *GlobalComObjectStubs;	// Placeholders for global com instances
    													//		that could not be created
    extern VList              *ComForms;				// All com forms in the project
    extern VList              *RuntimeComForms;			// All com forms at runtime
    extern TStringList        *UsedGlobalObjectNames;	// Names already used for global com
    													//		instances
    extern DISPPARAMS         dpNoArgs;					// Stub to indicate no parameters
    extern ICatInformation    *ComCategoryInformation;	// Tells us stuff like whether or not
    													//		an object is an ActiveX control
    extern _GUID              ControlCategory;          // Localization of the GUID for ActiveX
    													//		controls.
    extern TControlData       ControlDataTemplate;      // See notes on the instantiation.

    AnsiString NameFromTypeDesc(TYPEDESC *TypeDesc, ITypeInfo *OwningTypeInfo);
    void ResolveIndirections(TYPEDESC *&TypeDesc, USHORT &vt, int &nIndirections);
    void BuildComClassTree();
    void BuildComObjectTree();
    TTreeNode *BuildComTreeBranch(TTreeView *ObjectTree, TTreeNode *ParentNode
    	, TComObject *Object, TStringList *MemberList, AnsiString NodeName);
    AnsiString GetUniqueComObjectName(const AnsiString &ObjectName
    	, TStringList *UsedClassNames);
    DataSet *DSFromVarType(USHORT vt);
    DataSet *GetOrMakePtrDS(AnsiString strTypeName, DataSet *_PointsToDS);
    DataSet *GetOrMakeComDS(AnsiString ProgID, ITypeInfo *TypeInfo, TYPEDESC *TypeDesc);
    DataSet *GetOrMakeComDS(AnsiString ProgID, TComObjectInfo *ObjectInfo);
    TComObjectInfo *GetObjectInfo(AnsiString LibID, AnsiString ClassName, AnsiString
    	ObjectName = strNull);
    TComObjectInfo *GetObjectInfoForProgID(const AnsiString &ProgID);
    AnsiString ParseLibraryID(AnsiString &ProgID);
    AnsiString GetObjectName(const AnsiString &ProgID, const char *Qualifier = ".");
    AnsiString GetMemberName(const AnsiString &ProgID, const char *Qualifier = ".");
    TLibInfo *LoadComLibrary(AnsiString FileName);
    void InitComObjects();
    bool LibIsDependency(TLibInfo *LibInfo, bool ShowError = false);
    bool IsDependency(TComResource &Resource);
    DataSet *DSForDefaultInterface(ITypeInfo *TypeInfo, const AnsiString& TreeGroup);
    TComObjectInfo *GetDefaultInterface(ITypeInfo *TypeInfo);
    TComObject *FindComObject(const AnsiString &ObjectName);
    void OpenComHelpFile(AnsiString HelpFile, ULONG HelpContext, HWND Window);
    void ShowComHelp(TLibInfo *LibInfo, ITypeInfo *TypeInfo, DISPID MemID, HWND Window);
    void ShowComHelp(TLibInfo *LibInfo, HWND Window);
    void ShowComHelp(TComObjectInfo *ObjectInfo, HWND Window);
    void ShowComHelp(TMemberDesc *MemberDesc, HWND Window);
    AnsiString GetTypeName(ITypeInfo *TypeInfo, int Index = -1);
};

using namespace ComManager;


#endif
