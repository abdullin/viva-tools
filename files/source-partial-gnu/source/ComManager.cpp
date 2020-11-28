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

#include "ComManager.h"
#include "ComI2adl.h"
#include "Main.h"
#include "DataSet.h"
#include "Project.h"
#include "ErrorTrap.h"
#include "I2adlList.h"
#include "VivaSystem.h"
#include "process.h"

#pragma package(smart_init)


TComObjectInfo::TComObjectInfo(ITypeInfo *_TypeInfo, TLibInfo *_OwningLibInfo)
{
    OwningLibInfo = _OwningLibInfo; 
    TypeInfo = _TypeInfo;
    TypeInfo->GetTypeAttr(&TypeAttr);

    // Check both the Windows Registry, and the Type Flags for this class, to determine if it
    //      is an ActiveX Control:
    IsControl = (ComCategoryInformation->IsClassOfCategories(TypeAttr->guid, 1,
        &ControlCategory, 0, NULL) == S_OK)  ||
        (TypeAttr->wTypeFlags & TYPEFLAG_FCONTROL);

    // Must use a BSTR (Binary String) as a mediary for acquisition of the Class Name, since
    //      it is the string type that Com functions pass around:
    BSTR    bstrTypeName;

    TypeInfo->GetDocumentation(-1, &bstrTypeName, NULL, NULL, NULL);
    ClassName = bstrTypeName;
    SysFreeString(bstrTypeName);

    // Gets the Programmatic ID for this class (used by CreateObject()):
    ProgIDFromCLSID(TypeAttr->guid, &ProgID);

    // Initialize and load the list of members (functions, properties, and events):
    MemberList = new TStringList;
    MemberList->Sorted = true; // Sorted for speedy lookup purposes.
    MemberList->Duplicates = dupAccept;
    GetMemberList(TypeInfo, false, MemberList); // loads member info from Type Library.
}


TComObjectInfo::~TComObjectInfo()
{
    for (int i = 0; i < MemberList->Count; i++)
        delete (TMemberDesc *) MemberList->Objects[i];
    delete MemberList;
    
    TypeInfo->ReleaseTypeAttr(TypeAttr);
}


// Populates the MemberList (TStringList), which includes events, functions, variables,
//      and Get and Set Properties, with member with names and TMemberDesc pointers.
void TComObjectInfo::GetMemberList(ITypeInfo *TypeInfo, bool AreEvents, TStringList *MemberList)
{
    TYPEATTR    *TypeAttr;

    TypeInfo->GetTypeAttr(&TypeAttr);

    switch (TypeAttr->typekind)
    {
        case TKIND_INTERFACE:  // A type that has virtual functions, all of which are pure.
        case TKIND_DISPATCH:   // A set of methods and properties that are accessible through IDispatch::Invoke.
        case TKIND_COCLASS:    // Set of implemented component object interfaces.
        case TKIND_ENUM:       // A set of enumerators.
        case TKIND_MODULE:     // A module which can only have static functions and data (for instance, a DLL).
        case TKIND_RECORD:     // A struct with no methods.
        case TKIND_UNION:      // A union, all of whose members have offset zero.
        {
            for (int h = 0; h < 2; h++) // Once for functions and once for "variables"
            {
                int    Count = (h == 0) ? TypeAttr->cFuncs
                                        : TypeAttr->cVars;

                for (int i = 0; i < Count; i++)
                {
                    BSTR            bstrMemberName = NULL;
                    unsigned int    NameCount;
                    TMemberDesc    *MemberDesc = new TMemberDesc(this, TypeInfo);

                    MemberDesc->Index = i;

                    if (h == 0)
                    {
                        MemberDesc->Type = mtFunction;

                        if (AreEvents)
                            MemberDesc->Type = (MemberTypeEnum)(MemberDesc->Type | mtEvent);

                        TypeInfo->GetFuncDesc(i, &MemberDesc->FuncDesc);

                        FUNCDESC    *FuncDesc = MemberDesc->FuncDesc;

                        // We should not load restricted and hidden members:
                        if (FuncDesc->wFuncFlags & FUNCFLAG_FRESTRICTED  ||
                            FuncDesc->wFuncFlags & FUNCFLAG_FHIDDEN)
                        {
                            delete MemberDesc;
                            continue;
                        }

                        // GetNames can return both function and parameter names
                        //      ; just want function name this time:
                        TypeInfo->GetNames(FuncDesc->memid, &bstrMemberName, 1, &NameCount);

                        MemberDesc->RawName = bstrMemberName;
                        MemberDesc->Name = bstrMemberName;  // Copy, not pointer assignment.

                        if (MemberDesc->Type == mtEvent)
                            MemberDesc->Name = strEvent + MemberDesc->Name;

                        if (FuncDesc->invkind == INVOKE_PROPERTYGET)
                            MemberDesc->Name = strGet + MemberDesc->Name;
                        else if (FuncDesc->invkind == INVOKE_PROPERTYPUT)
                            MemberDesc->Name = strSet + MemberDesc->Name;

                        // Add any types to which any parameters/return value refer, which are
                        //      not defined in our owning Type Library, thereinto.
                        for (int j = 0; j < FuncDesc->cParams; j++)
                            MemberDesc->ResolveRefs(MemberDesc->OwningTypeInfo,
	                            &FuncDesc->lprgelemdescParam[j].tdesc);

                        MemberDesc->ResolveRefs(MemberDesc->OwningTypeInfo,
	                        &FuncDesc->elemdescFunc.tdesc);
                    }
                    else
                    {
                        TypeInfo->GetVarDesc(i, &MemberDesc->VarDesc);
                        TypeInfo->GetNames(MemberDesc->VarDesc->memid, &bstrMemberName, 1
                        	, &NameCount);
                        MemberDesc->RawName = bstrMemberName;
                        MemberDesc->Name = bstrMemberName;  // Copy, not pointer assignment.

                        MemberDesc->ResolveRefs(MemberDesc->OwningTypeInfo,
	                        &MemberDesc->VarDesc->elemdescVar.tdesc);                        
                    }

                    MemberList->AddObject(MemberDesc->Name, (TObject *) MemberDesc);
                }
            }
        }

        default:
            break;
    };

    TypeInfo->ReleaseTypeAttr(TypeAttr);
}


void TComObjectInfo::RegisterTypeLib()
{
    char            WinSysPath[MAX_PATH];

	GetSystemDirectory(WinSysPath, MAX_PATH);

    AnsiString      RegSvrFileName(AnsiString(WinSysPath) + "\\Regsvr32.exe");

    spawnlp(P_WAIT, RegSvrFileName.c_str(), RegSvrFileName.c_str(), AnsiString(
        "/s \"" + OwningLibInfo->FileName + "\"").c_str(), 0);

    Sleep(300); // this is a band-aid; i just don't know how else to give RegSvr time to complete.

    ProgIDFromCLSID(TypeAttr->guid, &ProgID);
}


// Attempts to instantiate this COM object, registering the DLL or OCX in which it exists, if
//      necessary.
IDispatch *TComObjectInfo::Instantiate()
{
    IDispatch 	*ComInstance = NULL;

    // If, at first, you don't succeed, then register the Type Library and try again:
    for (int i = 0; i < 2; i++)
    {
        HRESULT hr = TypeInfo->CreateInstance(NULL, IID_IDispatch, (void **) &ComInstance);

        if (hr == S_OK)
        {
            break;
        }
        else
        {
            // Only on first pass, call RegSvr32.exe:
            if (i == 0)
                RegisterTypeLib();
        }
    }

    return ComInstance;
}


bool TComObjectInfo::Implements(ITypeInfo *_TypeInfo)
{
    TYPEATTR			*TypeAttr;

    TypeInfo->GetTypeAttr(&TypeAttr);

    for (int i = 0; i < TypeAttr->cImplTypes; i++)
    {
        TImplTypeInfo	ImplTypeInfo(i, TypeInfo);

        if (ImplTypeInfo.TypeInfo == NULL)
            continue;

        if (ImplTypeInfo.TypeInfo == _TypeInfo)
        	return true;
    }

    TypeInfo->ReleaseTypeAttr(TypeAttr);

    return false;
}


// Attempts to load the library information from Type Library file <_FileName>:
TLibInfo::TLibInfo(AnsiString _FileName, bool bPromptOnError)
{
    FileName = _FileName.UpperCase();
    OrigFileName = FileName;
    ObjectList = new TStringList;
    TypeLib = NULL;

    if (!FileExists(FileName))
    {
        if (bPromptOnError)
            ErrorTrap(4032, FileName);

        return;
    }

    LoadTypeLib(WideString(FileName), &TypeLib); // Windows SDK call.

    if (TypeLib == NULL)
    {
        if (bPromptOnError)
            ErrorTrap(4033, FileName);

        return;
    }

    // We want an AnsiString for our member LibID, but GetDocumentation requires a BSTR, so we
    //      get to play musical chairs with a BSTR; same with HelpFileName:
    BSTR    bstrLibID, bstrHelpFileName;

    TypeLib->GetDocumentation(-1, &bstrLibID, NULL, &HelpContext, &bstrHelpFileName);

    HelpFileName = bstrHelpFileName;
    LibID = bstrLibID;

    SysFreeString(bstrHelpFileName);
    SysFreeString(bstrLibID);
}


TLibInfo::TLibInfo(TLibInfo *LibInfo)    // Copy constructor
{
    *this = *LibInfo;

    // Gotta have yer own copy.  Otherwise, which TLibInfo instance owns the ObjectList?
    ObjectList = new TStringList;
    GetObjectList();
}


// Since we have our own copy of an ObjectList, we get to clean it up:
TLibInfo::~TLibInfo()
{
    for (int i = 0; i < ObjectList->Count; i++)
        delete (TComObjectInfo *) ObjectList->Objects[i];
        
    delete ObjectList;

	if (TypeLib != NULL)
		TypeLib->Release();
}


// Populates this TLibInfo's Object List (Classes, Records, Enumerations, etc)
void TLibInfo::GetObjectList()
{
    ObjectList->Sorted = true;

    if (TypeLib == NULL)
    {
        ObjectList->AddObject("[Error in loading Type Library]", NULL);
        return;
    }

    int    TypeInfoCount = TypeLib->GetTypeInfoCount();

    for (int j = 0; j < TypeInfoCount; j++)
    {
        ITypeInfo    *TypeInfo;

        if (TypeLib->GetTypeInfo(j, &TypeInfo) != S_OK)
            continue;

        TYPEATTR    *CurrTypeAttr;

        TypeInfo->GetTypeAttr(&CurrTypeAttr);

        // Do not load the "hidden" types:
        if (CurrTypeAttr->wTypeFlags & TYPEFLAG_FHIDDEN)
            continue;

        TComObjectInfo    *CurrObjectInfo = new TComObjectInfo(TypeInfo, this);

        for (int i = 0; i < CurrObjectInfo->TypeAttr->cImplTypes; i++)
        {
            TImplTypeInfo	ImplTypeInfo(i, TypeInfo);

            if (ImplTypeInfo.TypeInfo == NULL)
                continue;

            TStringList    *MemberList = new TStringList;

            CurrObjectInfo->GetMemberList(ImplTypeInfo.TypeInfo
            , (ImplTypeInfo.ImplFlags & IMPLTYPEFLAG_FSOURCE) != 0, MemberList);

            CurrObjectInfo->MemberList->AddStrings(MemberList);
            delete MemberList;
        }

        ObjectList->AddObject(CurrObjectInfo->ClassName, (TObject *) CurrObjectInfo);
    }
}


// Acquires data for an implemented interface using Windows SDK calls:
TImplTypeInfo::TImplTypeInfo(int Index, ITypeInfo *_TypeInfo)
{
    _TypeInfo->GetRefTypeOfImplType(Index, &RefType);
    _TypeInfo->GetRefTypeInfo(RefType, &TypeInfo);
    _TypeInfo->GetImplTypeFlags(Index, &ImplFlags);
}


namespace ComManager
{
    AnsiString    strEvent = "(Event)";
    AnsiString    strGet = "(Get)";
    AnsiString    strSet = "(Set)";
    AnsiString    strComLibrary = "ComLibrary";
    AnsiString    strComForm = "ComForm";
    AnsiString    strComObject = "ComObject";
    AnsiString    strProgID = "ProgID";
    AnsiString    strDispatchIndicator = "'s Dispatch";
    AnsiString    strComCreateIndicator = "[Create]";
    AnsiString    FuncKindNames[ ] =
    {
        "virtual",
        "purevirtual",
        "",
        "static",
        "DISPATCH"
    };
    AnsiString    VarKindNames[ ] =
    {
        "member",
        "static",
        "const",
        "DISPATCH"
    };
    AnsiString    ObjectTypeNames[ ] =
    {
        "enum",
        "record",
        "module",
        "interface",
        "dispatch",
        "class",
        "alias",
        "union"
    };

    DISPPARAMS    dpNoArgs =
    {
        NULL,
        NULL,
        0,
        0
    };

    ICatInformation    *ComCategoryInformation;
    _GUID              ControlCategory = CATID_Control;

    // The TOleControl achitecture forces us to make a static instance of TControlData.  Control
    //		data must be populated before the VOleControl constructor is called.
    TControlData       ControlDataTemplate =
    {
      // GUID of CoClass and Event Interfaces of the control, respectively
      {0x3B7C8860, 0xD78F, 0x101B,{ 0xB9, 0xB5, 0x04, 0x02, 0x1C, 0x00, 0x94, 0x02} },
      {0x3B7C8862, 0xD78F, 0x101B,{ 0xB9, 0xB5, 0x04, 0x02, 0x1C, 0x00, 0x94, 0x02} },
      0, NULL,
      NULL,  // HRESULT(0x00000000)
      // Flags for OnChanged PropertyNotification
      0,
      0, // (IDE Version)
      // Count of Font Prop and array of their DISPIDs
      0, NULL,
      // Count of Pict Prop and array of their DISPIDs
      0, NULL,
      0, // Reserved
      0, // Instance count (used internally)
      0, // List of Enum descriptions (internal)
    };

    VList          *ComObjects = new VList(sizeof(TComObject));
    VList          *GlobalComObjects = new VList(sizeof(TComObject));
    TStringList	   *GlobalComObjectStubs = new TStringList; 
    VList          *ComForms = new VList(sizeof(TComForm));
    VList          *ComI2adlList = new VList(sizeof(I2adl));
    VList          *RuntimeComForms = new VList(sizeof(TComForm));
    TStringList    *UsedGlobalObjectNames = new TStringList;
};


// Gets a name unique to the scope to which parameter UsedObjectNames belongs:
AnsiString ComManager::GetUniqueComObjectName(const AnsiString &ObjectName, TStringList *
        UsedObjectNames)
{
    AnsiString    Name;

    // Loop thru possibilities for names, using [Class Name] [index++]
    //      ("Object1", "Object2", ...) until you find one that isn't used:
    do
    {
        int    index = UsedObjectNames->IndexOf(ObjectName);

        if (index == -1)
        {
            index = UsedObjectNames->Add(ObjectName);
            UsedObjectNames->Objects[index] = (TObject *) 0;
        }
        else
        {
            UsedObjectNames->Objects[index] = (TObject *) ((int) UsedObjectNames->Objects
            	[index]	+ 1);
        }

        Name = UsedObjectNames->Strings[index] + AnsiString (1 + (int) UsedObjectNames->Objects
        	[index]);
    }
	while(UsedObjectNames->IndexOf(Name) != -1);

    return Name;
}


// Every Com type supported in Viva has a corresponding Dataset, get it if it exists; return
//      SGDataSets[cdsUnsupported] otherwise:
DataSet *ComManager::DSFromVarType(USHORT vt)
{
    vt = (USHORT) (vt % VT_VECTOR);  // upper bits are used as flags; need to mask out.

    if (vt > SGDataSetCount)
        return SGDataSets[sdsUnsupported];

    DataSet    *DSet = SGDataSets[vt];

    if (DSet == NULL)
        return SGDataSets[sdsUnsupported];
    else
        return DSet;
}


// Returns the data set that is a pointer to the strTypeName data set.
//     A new data set will be created if necessary.
DataSet *ComManager::GetOrMakePtrDS(AnsiString strTypeName, DataSet *_PointsToDS)
{
    strTypeName += '*';

    DataSet    *PtrDS = GetDataSet(strTypeName);

    if (PtrDS == NULL)
    {
        PtrDS = new DataSet(strTypeName, _PointsToDS->TreeGroup, DEFAULT_COM_DATASET_COLOR
        , _PointsToDS->AttributeFlags, sgCOM);

        // For now, a pointer is 32 bits (or 2 words), though this could change; this is the only
        //      code that needs to be updated in such case:
        PtrDS->ChildDataSets->Add(SGDataSets[sdsWord]);
        PtrDS->ChildDataSets->Add(SGDataSets[sdsWord]);

        // VT_BYREF flag should only be used in VARIANTARGs, which is exclusively what this
        //      DataSet will be used for.
        PtrDS->VarType = (USHORT) (_PointsToDS->VarType | VT_BYREF);

        PtrDS->PointsToDS = _PointsToDS;
        PtrDS->BuildExposerAndCollector();
    }

    return PtrDS;
}


// Gets or makes a Dataset corresponding to a Com variable type, either static or defined in
//      a Type Library.  Much of the logic in this function is similar to that of NameFromTypeDesc.
DataSet *ComManager::GetOrMakeComDS(AnsiString ProgID, ITypeInfo *TypeInfo, TYPEDESC *TypeDesc)
{
    USHORT      vt = TypeDesc->vt;
    int         nIndirections = 0;
    DataSet     *DS;

    ResolveIndirections(TypeDesc, vt, nIndirections);

    if (vt == VT_USERDEFINED)
    {
        // A VARTYPE of VT_USERDEFINED indicates that this type is defined in this Type Library,
        //      or in one that it "imports".  We have more resolving to do:
        ITypeInfo    *RefTypeInfo;

        TypeInfo->GetRefTypeInfo(TypeDesc->hreftype, &RefTypeInfo);
        if (RefTypeInfo == NULL)
        {
            DS = SGDataSets[sdsUnsupported];
        }
        else
        {
            BSTR        TypeName = NULL;
            // If the Dataset for this type name already exists, then we are done:
            RefTypeInfo->GetDocumentation(-1, &TypeName, NULL, NULL, NULL);
            DS = GetDataSet(AnsiString(TypeName));
            SysFreeString(TypeName);

            if (DS == NULL)
            {
                // Since this type is user-defined, it may well be a job for the version
                //      of this function that handles composite types rather than simple types;
                //      if not, it will call this function after resolving a few things:
                TComObjectInfo    *RefObjectInfo = new TComObjectInfo(RefTypeInfo, NULL);

                DS = GetOrMakeComDS(ProgID, RefObjectInfo);
                delete RefObjectInfo;
            }
        }
    }
    else
    {
        DS = DSFromVarType(vt);
    }

    // Now add Pointer DataSets for each level of indirection:
    for (int i = 0; i < nIndirections; i++)
    {
        DS = GetOrMakePtrDS(DS->Name, DS);
        DS->BuildExposerAndCollector();
    }

    return DS;
}


// Gets or makes a Dataset corresponding to a Com Class; may return SGDataSets[cdsUnsupported]:
DataSet *ComManager::GetOrMakeComDS(AnsiString ProgID, TComObjectInfo *ObjectInfo)
{
    ITypeInfo    *TypeInfo = ObjectInfo->TypeInfo;
    TYPEATTR     *TypeAttr = ObjectInfo->TypeAttr;
    DataSet      *ReturnDataSet = SGDataSets[sdsUnsupported];
    TYPEKIND     TypeKind = TypeAttr->typekind;

    if (TypeKind == TKIND_ALIAS)
    {
        ReturnDataSet = GetOrMakeComDS(ProgID, TypeInfo, &TypeAttr->tdescAlias);
    }
    // ToDo: support TKIND_UNION.
    else if (TypeKind == TKIND_RECORD  ||  TypeKind == TKIND_ENUM)
    {
        AnsiString    &TypeName = ObjectInfo->ClassName;

        ReturnDataSet = GetDataSet(TypeName);

        if (ReturnDataSet == NULL) // only create a new one if one does not exist:
        {
            ReturnDataSet = new DataSet(TypeName, ProgID, DEFAULT_COM_DATASET_COLOR,
                ATT_UNSIGNED, sgCOM);

            if (TypeAttr->typekind == TKIND_ENUM)
            {
                ReturnDataSet->ChildDataSets->Add(SGDataSets[sdsWord]);
                ReturnDataSet->ChildDataSets->Add(SGDataSets[sdsWord]);
                ReturnDataSet->VarType = VT_I4;
            }
            else
            {
                // Todo: Do something about flags in vt (Safearray, Vector, Reserved).
                TStringList    *MemberList = ObjectInfo->MemberList;

                // Add a child Dataset for each member of this Com Record (typedef struct in C):
                for (int k = 0; k < MemberList->Count; k++)
                {
                    TMemberDesc    *MemberDesc = (TMemberDesc *) MemberList->Objects[k];
                    TYPEDESC       *TypeDesc = &MemberDesc->VarDesc->elemdescVar.tdesc;
                    DataSet        *ChildDS = GetOrMakeComDS(ProgID, TypeInfo, TypeDesc);

                    ReturnDataSet->ChildDataSets->Add(ChildDS);
                }

                if (ReturnDataSet != SGDataSets[sdsUnsupported])
                    ReturnDataSet->VarType = VT_USERDEFINED;
            }

            ReturnDataSet->BuildExposerAndCollector();
        }
    }
    else if (TypeKind == TKIND_INTERFACE ||
             TypeKind == TKIND_COCLASS   ||
             TypeKind == TKIND_DISPATCH)
    {
        ReturnDataSet = GetDataSet(ObjectInfo->ClassName);

        if (ReturnDataSet == NULL)
        {
            ReturnDataSet = new DataSet(ObjectInfo->ClassName, ProgID, DEFAULT_COM_DATASET_COLOR
                , ATT_UNSIGNED, sgCOM);

            // We don't support dereferencing these types yet.  Might never:
            ReturnDataSet->ChildDataSets->Add(SGDataSets[sdsUnsupported]);
            ReturnDataSet->BuildExposerAndCollector();
        }

        if (TypeKind == TKIND_INTERFACE)
        {
            // Needs to be IUnknown *, not just IUnknown or just *
            ReturnDataSet = GetOrMakePtrDS(ReturnDataSet->Name, ReturnDataSet);
            ReturnDataSet->VarType = (USHORT) VT_UNKNOWN;
        }
        else if (TypeKind == TKIND_DISPATCH)
        {
            // Needs to be IDispatch *, not just IDispatch or just *
            ReturnDataSet = GetOrMakePtrDS(ReturnDataSet->Name, ReturnDataSet);
            ReturnDataSet->VarType = (USHORT) VT_DISPATCH;
        }
        else // TKIND_COCLASS
        {
            ReturnDataSet->VarType = (USHORT) VT_USERDEFINED;
        }
    }

    return ReturnDataSet;
}


// Gets the default dispinterface for a Class typeinfo.
//		Will allocate the return value; caller must delete it.  May return NULL.
TComObjectInfo *ComManager::GetDefaultInterface(ITypeInfo *TypeInfo)
{
    TYPEATTR			*TypeAttr;
	TComObjectInfo		*RefObjectInfo = NULL;

    TypeInfo->GetTypeAttr(&TypeAttr);

    for (int i = 0; i < TypeAttr->cImplTypes; i++)
    {
        TImplTypeInfo	ImplTypeInfo(i, TypeInfo);

        if (ImplTypeInfo.TypeInfo == NULL)
            continue;

        // Is this search thorough enough?  If the maker of this interface was dumb enough to
        //      implement more than 1 interface and not mark one of them as "default", then
        //      maybe it doesn't matter.
        if (TypeAttr->cImplTypes == 1 || (ImplTypeInfo.ImplFlags & IMPLTYPEFLAG_FDEFAULT))
        {
            RefObjectInfo = new TComObjectInfo(ImplTypeInfo.TypeInfo, NULL);
            break;
        }
    }

    TypeInfo->ReleaseTypeAttr(TypeAttr);

    return RefObjectInfo;
}


// Makes a DataSet for the default implented interface for the ITypeInfo * passed.
DataSet *ComManager::DSForDefaultInterface(ITypeInfo *TypeInfo, const AnsiString& TreeGroup)
{
    DataSet				*DSet = SGDataSets[sdsUnsupported];
    TComObjectInfo		*RefObjectInfo = GetDefaultInterface(TypeInfo);

	if (RefObjectInfo != NULL)
    {
	    DSet = GetOrMakeComDS(TreeGroup, RefObjectInfo);
		delete RefObjectInfo;
    }

    return DSet;
}


// Com type descriptions often refer to pointer types, perhaps having multiple levels of
//      indirection; this function determines the number of these and gets the VARTYPE of
//      the type at the bottom of the chain.  Paramters vt and nIndirections are both changed
//      to reflect these, if applicable.
void ComManager::ResolveIndirections(TYPEDESC *&TypeDesc, USHORT &vt, int &nIndirections)
{
    while (vt == VT_PTR)
    {
        nIndirections++;
        TypeDesc = TypeDesc->lptdesc;
        vt = TypeDesc->vt;
    }
}


// Returns the full caption which applies to a Com Type Description.  Includes a space at the
//      end, for ease of concatenation.  For examples, use the Com Object Browser in Viva.
AnsiString ComManager::NameFromTypeDesc(TYPEDESC *TypeDesc, ITypeInfo *OwningTypeInfo)
{
    unsigned short    vt = TypeDesc->vt;
    AnsiString        Name = strNull;
    bool              bHaveName = false;
    int               nIndirections = 0;

    // ToDo: Support VT_ARRAY, VT_SAFEARRAY (and other types?)
    ResolveIndirections(TypeDesc, vt, nIndirections);

    if (vt == VT_USERDEFINED)
    {
        // A VARTYPE of VT_USERDEFINED indicates that this type is defined in this Type Library,
        //      or in one that it "imports".  We have more resolving to do:
        ITypeInfo    *RefTypeInfo;

        OwningTypeInfo->GetRefTypeInfo(TypeDesc->hreftype, &RefTypeInfo);

        if (RefTypeInfo == NULL)
        {
            Name = SGDataSetNames[sdsUnsupported];
        }
        else
        {
            TYPEATTR    *TypeAttr = NULL;

            RefTypeInfo->GetTypeAttr(&TypeAttr);

            if (TypeAttr->typekind == TKIND_ALIAS)
                vt = TypeAttr->tdescAlias.vt;

            // Test vt again because it may have changed if this type was an alias.
            if (vt == VT_USERDEFINED)
            {
                BSTR    TypeName;

                // The -1 passed as an index indicates the type itself, not one of its members:
                RefTypeInfo->GetDocumentation(-1, &TypeName, NULL, NULL, NULL);
                Name += AnsiString(TypeName);
                SysFreeString(TypeName);
                bHaveName = true;
            }

            RefTypeInfo->ReleaseTypeAttr(TypeAttr);
        }
    }

    if (!bHaveName)
    {
        // The expression, "vt % VT_VECTOR", filters out any silly flags that we don't support.
        //      When we do support these type modifiers (Safe Array, Vector), this code
        //      should change:
        if ((vt % VT_VECTOR) <= SGDataSetCount)
            Name += SGDataSetNames[vt % VT_VECTOR];
        else
            return SGDataSets[sdsUnsupported]->Name;
    }

    for (int IndCounter = 0; IndCounter < nIndirections; IndCounter++)
        Name += '*';

    Name += ' ';

    return Name;
}


// Builds the tree that contains Com class and enumeration declarations.  Could be a member of
//      MainForm, but isn't it already crowded enough?
void ComManager::BuildComClassTree()
{
    TTreeView			*ClassTree = MainForm->ComClassTree;

    ClassTree->Items->BeginUpdate(); // For speed.
    ClearTreeView(ClassTree);

    // Add a node for each Com Type Library that the user has opted to use:
    for (int i = 0; i < ComLibraries->Count; i++)
    {
        TLibInfo		*LibInfo = (TLibInfo *) ComLibraries->Objects[i];

        if (LibInfo == NULL)
            continue;

        TTreeNode		*LibNode = ClassTree->Items->Add(NULL, LibInfo->LibID);

        SetImageIndex(LibNode, 11);
        LibNode->Data = LibInfo;

        TStringList		*ObjectList = LibInfo->ObjectList;

        // To this node, add a child node for each class and enumeration in this Type Library:
        for (int j = 0; j < ObjectList->Count; j++)
        {
            TComObjectInfo		*ObjectInfo = (TComObjectInfo *) ObjectList->Objects[j];
            TYPEKIND			TypeKind = ObjectInfo->TypeAttr->typekind;

            // If this is a legitimate class, then populate this node with all its members:
            if (TypeKind == TKIND_COCLASS && ObjectInfo->ProgID != NULL)
            {
                BuildComTreeBranch(ClassTree, LibNode, NULL, ObjectInfo->MemberList
                	, ObjectInfo->ClassName)->Data = ObjectInfo;
            }
            else if (TypeKind == TKIND_DISPATCH)
            {
//				TImplTypeInfo		ImplTypeInfo(j, ObjectInfo->TypeInfo);

//				if (ImplTypeInfo.ImplFlags & IMPLTYPEFLAG_FSOURCE)
//					continue;

                bool	referenced = false;

                for (int k = 0; k < ObjectList->Count; k++)
                {
                	if (k == j)
                    	continue;

					TComObjectInfo	*compobjectinfo = (TComObjectInfo *) ObjectList->Objects[k];

                    if (compobjectinfo->Implements(ObjectInfo->TypeInfo))
                    {
						referenced = true;
                        break;
                    }
                }

                if (referenced == false)
	                BuildComTreeBranch(ClassTree, LibNode, NULL, ObjectInfo->MemberList
    	            	, ObjectInfo->ClassName)->Data = ObjectInfo;
            }
            else if (TypeKind == TKIND_ENUM)
            {
                TTreeNode		*ObjNode = ClassTree->Items->AddChild(LibNode, ObjectList->
                    Strings[j]);
                TStringList		*MemberList = ObjectInfo->MemberList;

                SetImageIndex(ObjNode, 11);
                ObjNode->Data = ObjectInfo;

                // Populate this node with each entry for this enumeration:
                for (int i = 0; i < MemberList->Count; i++)
                {
                    TTreeNode    *NewNode = ClassTree->Items->AddChild(ObjNode, MemberList->
                            Strings[i]);

                    SetImageIndex(NewNode, 12);
                    NewNode->Data = MemberList->Objects[i];
                }
            }
        }
    }

    ClassTree->Items->EndUpdate();
}

// Builds the tree that contains all globally instantiated Com objects, including ActiveX
//      controls on Viva Com Forms:
void ComManager::BuildComObjectTree()
{
    TTreeView    *ObjectTree = MainForm->ComObjectTree;

    ObjectTree->Items->BeginUpdate(); // For speed.
    ClearTreeView(ObjectTree);

    // Add entries for each Com Form first:
    for (int i = 0; i < ComForms->Count; i++)
    {
        TComForm		*ComForm = (TComForm *) ComForms->Items[i];
        TTreeNode		*FormNode = ObjectTree->Items->Add(NULL, ComForm->Name);

        SetImageIndex(FormNode, 11);

        for (int j = 0; j < ComForm->ControlCount; j++)
        {
            TComObject *Object = ((VOleControl *) ComForm->Controls[j])->OwningObject;
            BuildComTreeBranch(ObjectTree, FormNode, Object, Object->ObjectInfo->MemberList
	            , Object->Name);
        }
    }

    // And now for each Global Com Object:
    TTreeNode    *GlobalNode = ObjectTree->Items->AddObject(NULL, "Global COM Objects",
            COM_GLOBALS_ROOT_NODE);

    SetImageIndex(GlobalNode, 11);

    for (int i = 0; i < GlobalComObjects->Count; i++)
    {
        TComObject		*Object = (TComObject *) GlobalComObjects->Items[i];

        BuildComTreeBranch(ObjectTree, GlobalNode, Object, Object->ObjectInfo->MemberList
        , Object->Name);
    }

	for (int i = 0; i < GlobalComObjectStubs->Count; i++)
    {
	    TTreeNode   *ObjNode = ObjectTree->Items->AddChild
        	(GlobalNode, strCannotLoad + GlobalComObjectStubs->Strings[i]);

	    SetImageIndex(ObjNode, 11);
    }

    ObjectTree->Items->EndUpdate();
}


// Populates an existing node with all the entries in paramter "MemberList".
//      Parameter "Object" may be NULL, indicating that "MemberList" is a listing of enumeration
//      entries instead of class properties, functions, and events.
TTreeNode *ComManager::BuildComTreeBranch(TTreeView *ObjectTree, TTreeNode *ParentNode
, TComObject *Object, TStringList *MemberList, AnsiString NodeName)
{
    TTreeNode   *ObjNode = ObjectTree->Items->AddChild(ParentNode, NodeName);

    SetImageIndex(ObjNode, (Object != NULL) ? 11 : 12);
    ObjNode->Data = Object;

    int         Count = MemberList->Count;

    for (int i = 0; i < Count; i++)
    {
        TTreeNode    *NewNode = ObjectTree->Items->AddChild(ObjNode, MemberList->Strings[i]);

        SetImageIndex(NewNode, 12);
        NewNode->Data = MemberList->Objects[i];
    }

    return ObjNode;
}


// Attempts to find a TObjectInfo entry corresponding to a LibID and a Class Name:
TComObjectInfo *ComManager::GetObjectInfo(AnsiString LibID, AnsiString ClassName, AnsiString
	ObjectName)
{
	AnsiString	LibClass = LibID + strPeriod + ClassName;
    AnsiString	LibClassObject = LibClass + strSpace + ObjectName;
    int			index = ComLibraries->IndexOf(LibID);

    if (index < 0)
    {
    	// Library may exist, but is not "Included".  Only report once.
        if (GlobalComObjectStubs->IndexOf(LibClassObject) < 0)
        {
			GlobalComObjectStubs->Add(LibClassObject);

	        ErrorTrap(4030, LibClass);
        }

        return NULL;
    }

    TStringList    *ObjectList = ((TLibInfo *) ComLibraries->Objects[index])->ObjectList;

    index = ObjectList->IndexOf(ClassName);

    if (index < 0)
    {
    	// There is no type with this name in this library.  Only report once.
        if (GlobalComObjectStubs->IndexOf(LibClassObject) < 0)
        {
			GlobalComObjectStubs->Add(LibClassObject);

	        ErrorTrap(4031, LibClass);
        }

        return NULL;
    }

    return (TComObjectInfo *) ObjectList->Objects[index];
}


// Simply parses ProgID and passes its 2 counterparts to GetObjectInfo; returns NULL if ProgID
//      is not in the valid format ([Library ID].[Class Name] example: "Lib1.Class1".
TComObjectInfo *ComManager::GetObjectInfoForProgID(const AnsiString &ProgID)
{
    AnsiString    ObjectName = GetObjectName(ProgID);

    if (ObjectName == strNull)
        return NULL;

    return GetObjectInfo(ObjectName, GetMemberName(ProgID));
}


// Removes the library name from a class (not object) Viva Programmatic ID and returns it.
//      (example: "Lib1::Class1" returns "Lib1" and changes ProgId to be "Class1")
AnsiString ComManager::ParseLibraryID(AnsiString &ProgID)
{
    int    QualifierPos = ProgID.Pos("::");

    if (!QualifierPos)
    {
        ErrorTrap(4029, ProgID);
        return strNull;
    }

    AnsiString LibID = ProgID.SubString(1, QualifierPos - 1);

    ProgID = ProgID.SubString(QualifierPos + 2, ProgID.Length() - (QualifierPos));
    return LibID;
}


// Returns the object or class name for a Viva Programmatic ID.
//      (example: "Lib1.Object1" returns "Object1")
AnsiString ComManager::GetObjectName(const AnsiString &ProgID, const char *Qualifier)
{
    int    QualifierPos = ProgID.Pos(Qualifier);

    if (!QualifierPos)
        return strNull;

    return ProgID.SubString(1, QualifierPos - 1);
}


// Returns the object or class name for a Viva Programmatic ID.
//      (example: "Object1.Func1" returns "Func1")
AnsiString ComManager::GetMemberName(const AnsiString &ProgID, const char *Qualifier)
{
    int    DotPos = ProgID.Pos(Qualifier);

    if (!DotPos)
        return strNull;

    return ProgID.SubString(DotPos + strlen(Qualifier), ProgID.Length() - DotPos);
}


// Loads a Type Library and adds it to the ComLibraries list, if it exists and is valid:
TLibInfo *ComManager::LoadComLibrary(AnsiString FileName)
{
    // Don't load this library if it is already loaded:
    for (int i = 0; i < ComLibraries->Count; i++)
    {
    	TLibInfo	*LibInfo = (TLibInfo *) ComLibraries->Objects[i];

        if (FileName.UpperCase() == LibInfo->OrigFileName.UpperCase())
            return LibInfo;
    }

	bool		loaded = true;
    AnsiString	OrigFileName = FileName;
	TLibInfo	*LibInfo = new TLibInfo(FileName, false);

    while (LibInfo->TypeLib == NULL) // Invalid Type Library file.
    {
		loaded = false;

        // The following prevents an access violation by removing the dependenace on MainForm
        //		(Browse button).
        if (MainForm == NULL)
        {
        	ErrorTrap(4098, FileName);

			LibInfo->LibID = "Not found: " + FileName;
			break;
        }

		ErrorTrap	e(4084, FileName);

		e.OKButtonCaption = "&Browse...";
        e.CancelButtonCaption = "&Ignore";

        if (e == mrCancel)
        {
			LibInfo->LibID = "Not found: " + FileName;
			break;
        }

        MainForm->OpenDialog->Filter = "COM Library (*.dll)|*.dll|"
        "ActiveX Control Module (*.ocx)|*.ocx|COM Service (*.exe)|*.exe|All Files (*.*)|*.*";

        AnsiString		ext = GetFileExtension(FileName).LowerCase();

        if (ext == "dll")
			MainForm->OpenDialog->FilterIndex = 1;
        else if (ext == "ocx")
			MainForm->OpenDialog->FilterIndex = 2;
        else if (ext == "exe")
			MainForm->OpenDialog->FilterIndex = 3;
        else
			MainForm->OpenDialog->FilterIndex = 4;

		MainForm->OpenDialog->FileName = FileName.LowerCase();
		MainForm->OpenDialog->Execute();
        FileName = MainForm->OpenDialog->FileName;

        delete LibInfo;
		LibInfo = new TLibInfo(FileName, false);

        loaded = true;
    }

	if (loaded)
		LibInfo->GetObjectList();

    LibInfo->OrigFileName = OrigFileName;

    // Has this library ID already been loaded by another COM library?
    int		LibIdIndex = ComLibraries->IndexOf(LibInfo->LibID);

    if (LibIdIndex > -1)
    {
    	// Warn that user that the COM library cannot be loaded because its library ID is not
        //		unique.
        delete LibInfo;

    	LibInfo = (TLibInfo *) ComLibraries->Objects[LibIdIndex];

        ErrorTrap(217, VStringList(FileName, LibInfo->OrigFileName, LibInfo->LibID));
    }
    else
    {
	    ComLibraries->AddObject(LibInfo->LibID, (TObject *) LibInfo);
    }

    return LibInfo;
}


// Makes run-time forms for all the Com Forms that are used in the behavior assigned to X86UI,
//      and instantiates all global Com objects:
void ComManager::InitComObjects()
{
    // Attempt to create actual Com instances for each ComObject instance; any failure is
    //      a critical error, warranting abortion of synthesis:
    for (int i = 0; i < GlobalComObjects->Count; i++)
    {
        TComObject		*ComObject = (TComObject *) GlobalComObjects->Items[i];
        IDispatch		*Instance = ComObject->ObjectInfo->Instantiate();  // ToDo:  stop initializing if this call fails.

        // VComDispatch governs function calling and event dispatching:
        if (Instance)
            ComObject->ComDispatch = new TComDispatch(ComObject->ObjectInfo, Instance);
        else
        {
	        ErrorTrap(4040, AnsiString(ComObject->ObjectInfo->VivaProgID));

            CancelCompile = true;

            return;
        }
    }

    // Copy the design-time form(s) over to the run-time form(s):
    for (int i = 0; i < ComForms->Count; i++)
    {
        TComForm 		*ComForm = (TComForm *) ComForms->Items[i];
        TComResource	Resource(ComForm);
        bool			bUsed = false;
        I2adlList		*AssignedBehavior = UISystem->AssignedBehavior;

        if (AssignedBehavior->UsesResource(Resource))
            bUsed = true;

        if (!bUsed)
            continue;

        TComForm	*RuntimeComForm = new TComForm(NULL);

        RuntimeComForms->Add(RuntimeComForm);

        // SaveToStream and LoadFromStream are functions that I have created for just such a
        //      situation, and also for saving to file:
        TMemoryStream    *Stream = new TMemoryStream;

        ComForm->SaveToStream(Stream);
        Stream->Position = 0;
        RuntimeComForm->LoadFromStream(Stream);
        delete Stream;

        RuntimeComForm->CopyOf = ComForm;

        // Each control on the original form needs a new ComDispatch:
        for (int j = 0; j < ComForm->ControlCount; j++)
        {
            VOleControl    *SourceControl = (VOleControl *) ComForm->Controls[j];
            VOleControl    *DestControl = (VOleControl *) RuntimeComForm->Controls[j];

            DestControl->Cursor = crDefault; // May have been altered in design mode.

            // The ComObject instances still point to the controls on design-time forms.  Delete
            //      any ComDispatch that is left over from last run and issue a new one:
            TComObject         *ComObject = SourceControl->OwningObject;

            if (ComObject->ComDispatch != NULL)
                delete ComObject->ComDispatch;

            ComObject->ComDispatch = new TComDispatch(ComObject->ObjectInfo
            , (IDispatch *) DestControl->OleObject);
        }

        RuntimeComForm->IsDesigning = false; // true by default.
    }
}


// Reports whether a TLibInfo, ComForm, or ComObject is used, and issues a prompt if it is:
bool ComManager::IsDependency(TComResource &Resource)
{
    // Loop thru every behavior ProjectObjects and MyProject->Wip to find dependencies:
    VList    *List = ProjectObjects; // Changes to MyProject->Wip in second iteration.

    for (int h = 0; h < 2; h++)
    {
        for (int i = 0; i < List->Count; i++)
        {
            I2adlList    *Behavior = ((I2adl *) List->Items[i])->Behavior;

            if (Behavior == NULL)
                continue;

            // If this behavior uses this resource, then issue a message and return true:
            I2adl    *Dependent = Behavior->UsesResource(Resource);

            if (Dependent != NULL)
            {
                AnsiString    Message;

                if (Resource.Type == crsLibrary)
                    Message = Resource.LibInfo->LibID;
                else if (Resource.Type == crsForm)
                    Message = Resource.Form->Name;
                else if (Resource.Type == crsObject)
                    Message = Resource.Object->Name;

                ErrorTrap(4023, VStringList(Message, Dependent->QualifiedName()));
                return true;
            }
        }

        List = MyProject->Wip;
    }

    return false;
}


// Reports whether a TLibInfo instance (wraps a Type Library) is used by any ComObject instance:
bool ComManager::LibIsDependency(TLibInfo *LibInfo, bool ShowError)
{
    for (int i = 0; i < ComObjects->Count; i++)
    {
        TComObject    *Object = (TComObject *) ComObjects->Items[i];

        if (Object->ObjectInfo->OwningLibInfo->FileName == LibInfo->FileName)
        {
            AnsiString    Name(Object->Name);

            if (Object->IsFormObject)
                Name = Object->OleControl->Parent->Name + strPeriod + Name;

            if (ShowError)
	            ErrorTrap(4021, Name);

            return true;
        }
    }

    return false;
}


TMemberDesc::TMemberDesc(TComObjectInfo *ObjectInfo, ITypeInfo *TypeInfo)
{
    Type = mtProperty; // Just the initial value; can be set later if different.
    OwningObjectInfo = ObjectInfo;
    OwningTypeInfo = TypeInfo;
    RawName = NULL;
}


TMemberDesc::TMemberDesc(TMemberDesc *MemberDesc)
{
    *this = *MemberDesc;

    // Must have our own copy of FuncDesc, or VarDesc; otherwise, who owns the original?
    if (Type & mtFunction)
        OwningTypeInfo->GetFuncDesc(Index, &FuncDesc);
    else
        OwningTypeInfo->GetVarDesc(Index, &VarDesc);

    RawName = SysAllocString(MemberDesc->RawName);
}


TMemberDesc::~TMemberDesc()
{
    if (Type & mtFunction)
        OwningTypeInfo->ReleaseFuncDesc(FuncDesc);
    else
        OwningTypeInfo->ReleaseVarDesc(VarDesc);

    SysFreeString(RawName);
}


DISPID TMemberDesc::GetMemID()
{
    return (Type & mtFunction) ? FuncDesc->memid
							   : VarDesc->memid;
}


// Grabs Com class info located in other type libraries from this MemberDesc and opens their
//		containing type libs.
void TMemberDesc::ResolveRefs(ITypeInfo *TypeInfo, TYPEDESC *TypeDesc)
{
    if (OwningObjectInfo->OwningLibInfo == NULL)
        return;

    USHORT		vt = TypeDesc->vt;
    int			nIndirections;

    ResolveIndirections(TypeDesc, vt, nIndirections);

    if (vt == VT_USERDEFINED)
    {
        // A VARTYPE of VT_USERDEFINED indicates that this type is defined in this Type Library,
        //      or in one that it "imports".  We have more resolving to do:
        ITypeInfo       *RefTypeInfo = NULL;

        TypeInfo->GetRefTypeInfo(TypeDesc->hreftype, &RefTypeInfo);

        if (RefTypeInfo == NULL)
            return;

        TYPEATTR		*TypeAttr = NULL;

        RefTypeInfo->GetTypeAttr(&TypeAttr);

        if (TypeAttr != NULL && TypeAttr->typekind == TKIND_ALIAS)
        {
            ResolveRefs(RefTypeInfo, &TypeAttr->tdescAlias);
            RefTypeInfo->ReleaseTypeAttr(TypeAttr);
            return;
        }

        RefTypeInfo->ReleaseTypeAttr(TypeAttr);

        // If this referenced type was not found in this TMemberDesc's owning Library, then
        //		include its library.
        AnsiString		RefTypeName(GetTypeName(RefTypeInfo));
        ITypeLib		*reftypelib = NULL;
        UINT			dummy;
        BSTR			libname = NULL;
        BSTR			doc = NULL;

        RefTypeInfo->GetContainingTypeLib(&reftypelib, &dummy);
        reftypelib->GetDocumentation(-1, &libname, &doc, NULL, NULL);

        if (reftypelib != OwningObjectInfo->OwningLibInfo->TypeLib)
        {
            AnsiString	slibname(libname);

            if (ComLibraries->IndexOf(slibname) == -1)
            {
                TLibInfo	*reflibinfo = new TLibInfo(reftypelib, slibname);

                reflibinfo->GetObjectList();
                ComLibraries->AddObject(slibname, (TObject *) reflibinfo);
            }
        }
    }
}


TLibInfo::TLibInfo(ITypeLib *_TypeLib, AnsiString _LibID)
{
    FileName = strNull;
    ObjectList = new TStringList;
    TypeLib = _TypeLib;

    if (TypeLib == NULL)
    {
        ErrorTrap(4033, FileName);

        return;
    }

    // We want an AnsiString for our member LibID, but GetDocumentation requires a BSTR, so we
    //      get to play musical chairs with a BSTR; same with HelpFileName:
    BSTR    bstrLibID, bstrHelpFileName;

    TypeLib->GetDocumentation(-1, &bstrLibID, NULL, &HelpContext, &bstrHelpFileName);

    HelpFileName = bstrHelpFileName;
    LibID = bstrLibID;

    SysFreeString(bstrHelpFileName);
    SysFreeString(bstrLibID);
}


TYPEDESC *TMemberDesc::GetTypeDesc()
{
/*
    if (Type & mtAppended)
        return NULL;
*/

    if (Type == mtProperty)
        return &VarDesc->elemdescVar.tdesc;

    if (Type == mtFunction)
    {
        if (FuncDesc->invkind == INVOKE_PROPERTYPUT)
            return &FuncDesc->lprgelemdescParam[0].tdesc;
        else if (FuncDesc->invkind == INVOKE_PROPERTYGET)
            return &FuncDesc->elemdescFunc.tdesc;
    }

    return NULL;
}


// Searches ComObjects list for one that has the name matching parameter "ObjectName":
TComObject *ComManager::FindComObject(const AnsiString &ObjectName)
{
    for (int i = 0; i < ComObjects->Count; i++)
    {
        TComObject		*ComObject = (TComObject *) ComObjects->Items[i];

        if (ComObject->Name == ObjectName)
            return ComObject;
    }

    return NULL;
}


void ComManager::OpenComHelpFile(AnsiString HelpFile, ULONG HelpContext, HWND Window)
{
    if (GetFileExtension(HelpFile) == "hlp")
    {
        Application->HelpFile = HelpFile;
        Application->HelpContext(HelpContext);
    }
    else
    {
        static STARTUPINFO              StartUpInfo;
        static PROCESS_INFORMATION      procinfo;
        static HANDLE                   ProcID = NULL;

        if (ProcID != NULL)
            TerminateProcess(ProcID, 0);

        static bool b;
        b = CreateProcess(NULL, (strVivaSystemDir + "invokehtmlhelp.exe" + " " +
        HelpFile + " " + HelpContext + " " + IntToStr((int) Window).c_str()).c_str()
        , 0, 0, 0, 0, 0, 0, &StartUpInfo, &procinfo);

        ProcID = procinfo.hProcess;
    }
}


void ComManager::ShowComHelp(TLibInfo *LibInfo, ITypeInfo *TypeInfo, DISPID MemID, HWND Window)
{
    if (LibInfo->HelpFileName == strNull)
    {
        ErrorTrap(4041);
        return;
    }

    ULONG HelpContext = 0;

    TypeInfo->GetDocumentation(MemID, NULL, NULL, &HelpContext, NULL);

    if (HelpContext == 0)
        ErrorTrap(4042);

    OpenComHelpFile(LibInfo->HelpFileName, HelpContext, Window);
}


void ComManager::ShowComHelp(TLibInfo *LibInfo, HWND Window)
{
    OpenComHelpFile(LibInfo->HelpFileName, LibInfo->HelpContext, Window);
}


void ComManager::ShowComHelp(TComObjectInfo *ObjectInfo, HWND Window)
{
    ShowComHelp(ObjectInfo->OwningLibInfo, ObjectInfo->TypeInfo, -1, Window);
}


void ComManager::ShowComHelp(TMemberDesc *MemberDesc, HWND Window)
{
    ShowComHelp(MemberDesc->OwningObjectInfo->OwningLibInfo
    , MemberDesc->OwningTypeInfo, MemberDesc->GetMemID(), Window);
}


// Returns the type name of the paramter #"Index" (-1 for the name of the type itself)
AnsiString ComManager::GetTypeName(ITypeInfo *TypeInfo, int Index)
{
    // Must use a BSTR (Binary String) as a mediary for acquisition of the Class Name, since
    //      it is the string type that Com functions pass around:
    BSTR    bstrTypeName;

    TypeInfo->GetDocumentation(-1, &bstrTypeName, NULL, NULL, NULL);
    AnsiString TypeName(bstrTypeName);
    SysFreeString(bstrTypeName);

    return TypeName;
}

