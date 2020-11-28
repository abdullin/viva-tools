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

#pragma hdrstop

#include "Globals.h"
#include "ComI2adl.h"
#include "Node.h"
#include "ComManager.h"
#include "ErrorTrap.h"
#include "VivaIcon.h"
#include "VivaSystem.h"
#include "X86ExThread.h"
#include "DataSet.h"
#include "List.h"

#pragma package(smart_init)

// NULL is valid for pOwningObject, indicating that this represents a member of a class, not a
//      specific instance of the class (IsDynamic = true).
__fastcall I2adl::I2adl(TMemberDesc *pMemberDesc, TComObject *pOwningObject)
{
    static WideString   Names[MaxNameCount];
    UINT                NameCount;

    ITypeInfo     *TypeInfo = pMemberDesc->OwningTypeInfo;
    AnsiString    LibID = pMemberDesc->OwningObjectInfo->OwningLibInfo->LibID;

    // Gets the name of this function/property/event, and the names of each parameter:
    TypeInfo->GetNames(pMemberDesc->GetMemID(), (BSTR *) Names, MaxNameCount, &NameCount);

    if (pOwningObject != NULL)
        SetName(pOwningObject->Name + strPeriod + pMemberDesc->Name); // Static
    else
        SetName(pMemberDesc->OwningObjectInfo->ClassName + "::" + pMemberDesc->Name); // Dynamic

    I2adl_Init();

    // The arguments apply to outputs in the case of events, since they are given to us:
    NodeList    **IONodes = (pMemberDesc->Type == mtEvent) ? &Outputs
                                                           : &Inputs;

    // Index of first node which applies to a paramter is offset by 1 for "go" node; Dynamic
    //      Helpers have an extra node for the Dispatch of the Com object to which they apply.
    int FirstParamNodeIndex = (pOwningObject != NULL) ? 1 : 2;

    // If the number of paramters (NameCount) would cause the number of nodes to exceed the
    //      default number (DEFAULT_NODE_LIST_CAPACITY), then we need to kill the old ones
    //      and re-issue a list of the appropriate size:
    if (NameCount + FirstParamNodeIndex >= DEFAULT_NODE_LIST_CAPACITY)
    {
        delete *IONodes;
        *IONodes = NewNodeList(NameCount + FirstParamNodeIndex);
    }

    Outputs->Add(new Node(*NodestrDone, this, BitDataSet));

	if (pMemberDesc->Type != mtEvent)
	    Outputs->Add(new Node(*NodestrException, this, BitDataSet));

    if (pMemberDesc->Type == mtEvent)
        Inputs->Add(new Node("Reset", this, BitDataSet));
    else // Don't call us; we'll call you.
        Inputs->Add(new Node(*NodestrGo, this, BitDataSet));

    // Dynamic Helpers need a node for input of the Dispatch of what Com object they apply to:
    if (pOwningObject == NULL)
    {
        DataSet *DSet = DSForDefaultInterface(pMemberDesc->OwningObjectInfo->TypeInfo, LibID);

        Inputs->Add(new Node("MemberOf", this, DSet));
    }

    if (pMemberDesc->Type & mtFunction) // function or event.
    {
        FUNCDESC    *FuncDesc = pMemberDesc->FuncDesc;

        // Functions can be property puts or gets, even tho there is already a type made
        //      specifically for properties.  It's Microsoft's fault, as are all the ailments of modern society.
        if (FuncDesc->invkind == INVOKE_PROPERTYPUT ||
            FuncDesc->invkind == INVOKE_PROPERTYPUTREF)
            Names[NameCount++] = "Val";

        // Add a node for each paramater, with the appropriate DataSet.
        // Offset of 1 because Names[0] is the name of the member; Names[1] is the first param
        for (int i = 1; i < (int) NameCount; i++)
        {
            Node        *IONode = new Node(AnsiString(Names[i]), this, BitDataSet);
            TYPEDESC    *TypeDesc = &FuncDesc->lprgelemdescParam[i - 1].tdesc;

            IONode->DSet = GetOrMakeComDS(LibID, TypeInfo, TypeDesc);
            (*IONodes)->Add(IONode); // Outputs if an Event, Inputs otherwise.
        }

        TYPEDESC    *TypeDesc = &FuncDesc->elemdescFunc.tdesc;
        USHORT      vt = TypeDesc->vt; // Variable Type of return value.

        // Add a node for the return value, if there is one:
        if (vt != VT_VOID)
        {
            Node    *Output = new Node("Ret Val", this, BitDataSet);

            Output->DSet = GetOrMakeComDS(LibID, TypeInfo, TypeDesc);
            Outputs->Add(Output);
        }
    }
    else // Variable, not function
    {
        // This Com I2adl can be used to get and/or set this Property.
        VARDESC    *VarDesc = pMemberDesc->VarDesc;
        Node       *Input = new Node("New", this, BitDataSet);
        Node       *Output = new Node("Current", this, BitDataSet);

        Input->DSet = Output->DSet = GetOrMakeComDS(LibID, TypeInfo, &VarDesc->elemdescVar.tdesc);

        Inputs->Add(Input);
        Outputs->Add(Output);
    }

    InSystem = UISystem;
    AttSystem = UISystem->Name;
    AttResource = ResourcestrTimeSlice;
    AttResourceQty = 1;
    KeepObject = koOneSide;

    InitComHelper(pMemberDesc, pOwningObject);
    Conformalize();
}


// Constructor for a Dispatch I2adl, which outputs the Dispatch pointer of the Com object
//      to which it belongs.
__fastcall I2adl::I2adl(TComObject *_ComObject)
{
    SetName(_ComObject->Name + "'s Dispatch");
    I2adl_Init();

    InitDispatch(_ComObject);

    Inputs->Add(new Node(*NodestrGo, this, BitDataSet));
    Outputs->Add(new Node(*NodestrDone, this, BitDataSet));

    DataSet *DSet = DSForDefaultInterface(
        _ComObject->ObjectInfo->TypeInfo, _ComObject->ObjectInfo->OwningLibInfo->LibID);

    Outputs->Add(new Node("Dispatch", this, DSet));

    Conformalize();

    InSystem = UISystem;
    AttSystem = UISystem->Name;
    AttResource = ResourcestrTimeSlice;
    AttResourceQty = 1;

    KeepObject = koOneSide;
}


// Constructor for dynamic creators of Com objects.  Any thusly created Com object needs to
//      be released using a "Release" I2adl object.
__fastcall I2adl::I2adl(TComObjectInfo *_ObjectInfo)
{
    SetName("ComCreate: " + _ObjectInfo->VivaProgID);
    I2adl_Init();

    CreateObjectInfo = _ObjectInfo;

    NameCode = NAME_COMCREATE;
    ExecuteType = NAME_COMCREATE;

    Inputs->Add(new Node(*NodestrGo, this, BitDataSet));
    Outputs->Add(new Node(*NodestrDone, this, BitDataSet));

    DataSet *DSet = DSForDefaultInterface(
        _ObjectInfo->TypeInfo, _ObjectInfo->OwningLibInfo->LibID);

    Outputs->Add(new Node("Dispatch", this, DSet));

    Conformalize();

    InSystem = UISystem;
    AttSystem = UISystem->Name;
    AttResource = ResourcestrTimeSlice;
    AttResourceQty = 1;

    KeepObject = koOneSide;
}


// ComObject parameter may be NULL.
void I2adl::InitCom(AnsiString MemberName, TStringList *MemberList, TComObject *ComObject)
{
    // If the member which this I2adl acts upon is non-existent, then
    //      issue an error to the user and terminate its initialization:
    int		index = MemberList->IndexOf(MemberName);

    if (index == -1)
        ErrorTrap(4026, VStringList(MemberName, NameInfo->Name));
    else
        InitComHelper(new TMemberDesc((TMemberDesc *) MemberList->Objects[index]), ComObject);
}


void I2adl::InitDispatch(TComObject *_ComObject)
{
    NameCode = NAME_DISPATCH;
    ExecuteType = NAME_DISPATCH;

    // Must also identify the object as primitive because the name code was NAME_OTHER when
    //     it was origianlly set.
    IsPrimitive = true;
    ComObject = _ComObject;
	ComI2adlList->Add(this);
}


// Creates a Com Helper for this I2adl, and adds itself to the master list of Com I2adls.
void I2adl::InitComHelper(TMemberDesc *pMemberDesc, TComObject *pOwningObject)
{
    if (pMemberDesc->Type == mtEvent)
        ComHelper = new TComHelperEvent(this, pMemberDesc, pOwningObject);
    else
        ComHelper = new TComHelperFunc(this, pMemberDesc, pOwningObject);

    ExecuteType = EXECUTE_COM;

    ComI2adlList->Add(this);
}


// Simply returns this interface, regardless of the type requested.  This is presumtuous and
//      potentially dangerous, but it works for all the cases that I have tested, which are many.
HRESULT __stdcall TComDispatch::QueryInterface(const _GUID &guid, void **intf)
{
    if (guid == IID_IUnknown || guid == IID_IDispatch || guid == OutgoingIntfGuid)
    {
        *intf = this;
        AddRef();
        return S_OK;
    }
    else
    {
        *intf = 0;
        return E_NOINTERFACE;
    }
}


HRESULT __stdcall TComDispatch::GetIDsOfNames(REFIID riid, OLECHAR FAR *FAR *rgszNamesm,
	unsigned  int cNames, LCID lcid, DISPID FAR *rgdispid)
{
    return Dispatch->GetIDsOfNames(riid, rgszNamesm, cNames, lcid, rgdispid);
};

HRESULT __stdcall TComDispatch::GetTypeInfo(unsigned int itinfo, LCID lcid,
	ITypeInfo FAR *FAR * pptinfo)
{
    return Dispatch->GetTypeInfo(itinfo, lcid, pptinfo);
};

HRESULT __stdcall TComDispatch::GetTypeInfoCount(unsigned int FAR *pctinfo)
{
    return Dispatch->GetTypeInfoCount(pctinfo);
};


void TComObject::SetPropertyValue(bool Designing, TMemberDesc *MemberDesc, Variant Value
, I2adl *Owner)
{
    IDispatch	*Dispatch = NULL;

    if (IsFormObject)
    {
        VOleControl *Control = OleControl;

#if 0
        if (MemberDesc->Type & mtAppended)
        {
            switch(MemberDesc->Index)
            {
                case apLeft:
                    if (Designing)
                        Control->Parent->Left = Value;
                    else
                        Control->Left = Value;
                    break;
                case apTop:
                    if (Designing)
                        Control->Parent->Top = Value;
                    else
                        Control->Top = Value;
                    break;
                case apWidth:
                    if (Designing)
                        Control->Parent->Width = Value;

                    Control->Width = Value;
                    break;
                case apHeight:
                    if (Designing)
                        Control->Parent->Height = Value;

                    Control->Height = Value;
                    break;
                case apVisible:
                    if (Designing)
                        Control->DesignVisible = Value;
                    else
                        Control->Visible = Value;
                    break;
                case apTag:
                    Control->Tag = Value;
                    break;

                default:
                    ErrorTrap(4047, Name);
            };
        }
        else
#endif
        {
#if 0
            if (Designing && AnsiString(MemberDesc->RawName) == strVisible)
            {
                Control->DesignVisible = Value;
                return;
            }
#endif

            Dispatch = (IDispatch *) Control->OleObject;
        }
    }
    else
    {
        Dispatch = ComDispatch->Dispatch;
    }

    if (Dispatch != NULL)
    {
        TComHelperFunc 		ComHelp(Owner, MemberDesc, this);

        DISPID MemID = NULL;

        ComHelp.pParams = new DISPPARAMS;
        ComHelp.pParams->cArgs = 1;
        ComHelp.pParams->rgvarg = new VARIANTARG[1];
        ComHelp.pParams->rgvarg[0] = Value;

        ComHelp.ComCall(Dispatch, MemID, INVOKE_PROPERTYPUT, false);

		ComHelp.MemberDesc = NULL;
    }
}


Variant TComObject::GetPropertyValue(bool Designing, TMemberDesc* MemberDesc, I2adl *Owner)
{
	IDispatch       *Dispatch = NULL;
    VOleControl		*Control = OleControl;

    if (IsFormObject)
    {
#if 0
        if (MemberDesc->Type & mtAppended)
        {
            switch(MemberDesc->Index)
            {
                case apLeft:
                    return Designing ? Control->Parent->Left : Control->Left;
                case apTop:
                    return Designing ? Control->Parent->Top : Control->Top;
                case apWidth:
                    return Control->Width;
                case apHeight:
                    return Control->Height;
                case apVisible:
                    if (Designing)
                        return Control->DesignVisible;
                    else
                        return Control->Visible;
                case apTag:
                    return Control->Tag;

                default:
                    ErrorTrap(4048, Name);
            };
        }
        else
#endif
        {
            Dispatch = (IDispatch *) Control->OleObject;
        }
    }
    else
    {
        Dispatch = ComDispatch->Dispatch;
    }

    if (Dispatch != NULL)
    {
#if 0
        if (Designing && AnsiString(MemberDesc->RawName) == strVisible)
            return Control->DesignVisible;
#endif

        TComHelperFunc 		ComHelp(Owner, MemberDesc, this);
        DISPID 				MemID = NULL;

        ComHelp.ComCall(Dispatch, MemID, INVOKE_PROPERTYGET, false);
        ComHelp.MemberDesc = NULL;
        return ComHelp.Result;
    }

    return NULL;
}


// This function is called by the corresponding Com object, if we have established a connection
//      with it, when an event is fired.
HRESULT __stdcall TComDispatch::Invoke(DISPID dispidMember, REFIID riid, LCID lcid, unsigned
        short wFlags, DISPPARAMS FAR *pdispparams, VARIANT FAR *pvarResult, EXCEPINFO FAR *
        pexcepinfo, unsigned int FAR *puArgErr)
{
    if (IsRunning) // Ignore events if we are not in "Run" mode.
    {
        // Look for all entries whose Dispatch IDs are identical to that of this event:
        int 	Count = MessageMap->Count;

        for (int i = 0; i < Count; i++)
        {
            TMessageEntry	*MessageEntry = (TMessageEntry *) MessageMap->Items[i];

            // Found an entry?  Invoke the corresponding Com Helper.
            if (MessageEntry->Key == dispidMember)
            {
                // Prevents X86MainThread::Event() from deleting our "this" pointer.
                ComDispatchInvokeList->Add(this);

            	// Save the original pParams so the destructor will not try to delete the
                //		temporary one.
            	DISPPARAMS	*OrigpParams = MessageEntry->hlpEvent->pParams;

                MessageEntry->hlpEvent->pParams = pdispparams;
                MessageEntry->hlpEvent->Invoke();

                ComDispatchInvokeList->Remove(this);
                MessageEntry->hlpEvent->pParams = OrigpParams;
            }
        }
    }

    return S_OK;
}


// Increments the number of references to this interface, not that we ever use it.
ULONG __stdcall TComDispatch::AddRef()
{
    return ++nRefs;
}


// A standard implementation of this function might delete this object once nRefs hit 0,
//      but this particular interface was built to last for the lifetime of the Viva behavior,
//      and should never be revealed to other processes.
ULONG __stdcall TComDispatch::Release()
{
	nRefs--;
    return nRefs;
}


TComDispatch::TComDispatch(TComObjectInfo *pObjectInfo, IDispatch *pDispatch
	, bool EstablishedLinks)
{
    Dispatch = pDispatch;

    nRefs = 0;
    MessageMap = NULL;
	ConnectedPoints = NULL;

    if (EstablishedLinks)
    	return;

    ConnectedPoints = new TConnectionPointList(sizeof(TConnectionPointEntry));

    // This is where we attempt to establish an "Advisory Connection" with the Com object, which
    //      will notify us when events occur thereon.  Such connections are established via
    //      "Connection Points", giving the "Server" a pointer to an IDispatch-derived interface
    //      on the client, to call when events occur.
    ITypeInfo                    *TypeInfo = pObjectInfo->TypeInfo;

    // Attempt to acquire a pointer to the "Container" for connection points.
    IConnectionPointContainer    *cpc;

    Dispatch->QueryInterface(IID_IConnectionPointContainer, (void **) &cpc);

    if (cpc == NULL)
        return;

    // Iterate thru the implemented interfaces of this Com Object, looking for a one which
    //      is a source of events (IMPLTYPEFLAG_FSOURCE):
    for (int h = 0; h < pObjectInfo->TypeAttr->cImplTypes; h++)
    {
        TImplTypeInfo   ImplTypeInfo(h, TypeInfo);
        TYPEATTR        *ImplTypeAttr;

        if (ImplTypeInfo.TypeInfo == NULL)
            continue;

        ImplTypeInfo.TypeInfo->GetTypeAttr(&ImplTypeAttr);

        if (ImplTypeInfo.ImplFlags & IMPLTYPEFLAG_FSOURCE)
        {
            IConnectionPoint    *CP;

            // We found an event "Source" interface; ask the connection point container(cpc)
            //      for a connection point whose interface corresponds to this one:
            if (cpc->FindConnectionPoint(ImplTypeAttr->guid, &CP) == S_OK)
            {
				OutgoingIntfGuid = ImplTypeAttr->guid;

                if (MessageMap == NULL) // Many Com objects do not generate events.
                    MessageMap = new TMessageMap(sizeof(TMessageEntry));

                TConnectionPointEntry    *CPEntry = new TConnectionPointEntry;

                CP->Advise(this, &CPEntry->ConnectionID);

                CPEntry->CP = CP;
                ConnectedPoints->Add(CPEntry); // So that we can disconnect later.
            }
        }

        ImplTypeInfo.TypeInfo->ReleaseTypeAttr(ImplTypeAttr);
    }

    cpc->Release();
}


TComDispatch::~TComDispatch()
{
    // All of the connections that we set up for events need to now be unadvised, since we are
    //      about to be non-existent; failure to do this could result in memory access violations:
	if (ConnectedPoints != NULL)
	{
        for (int i = 0; i < ConnectedPoints->Count; i++)
        {
            TConnectionPointEntry    *CPEntry = (TConnectionPointEntry *) ConnectedPoints->Items[i];

            CPEntry->CP->Unadvise(CPEntry->ConnectionID);
            CPEntry->CP->Release();

            delete CPEntry;
        }
    }

    // Tell any corresponding Com object that we are done using the Dispatch Interface:
    if (Dispatch != NULL)
        Dispatch->Release();

    delete ConnectedPoints;

    if (MessageMap != NULL)
        for (int i = 0; i < MessageMap->Count; i++)
            delete (TMessageEntry *) MessageMap->Items[i];
            
    delete MessageMap;
}


// Sets up an internal advisory correlation between event Dispatch IDs and ComHelpers to use:
void TComDispatch::AddEntry(TComHelper *ComHelper)
{
    TMessageEntry *MessageEntry = new TMessageEntry;
    MessageEntry->Key = ComHelper->MemberDesc->FuncDesc->memid;
    MessageEntry->hlpEvent = ComHelper;
    MessageMap->Add(MessageEntry);
}


// Sole constructor for our Com object wrapper.  "_Control" can be NULL.
TComObject::TComObject(TComObjectInfo *pObjectInfo, AnsiString _Name, VOleControl *_Control,
	bool AddToList)
{
    TStringList    *UsedObjectNames;

    if (_Control != NULL)
    {
        // Kluge only until Control Editor re-application from obsolete 2.5.
		TComForm	*OwningComForm =
        	(_Control->Parent->Parent != NULL &&
            (TComForm *) _Control->Parent->Parent->Parent->Parent->Parent == WidgetForm)
        	? (TComForm *) _Control->Parent->Parent->Parent->Parent->Parent
            : (TComForm *) _Control->Parent;

        IsFormObject = true;
        _Control->OwningObject = this;
        UsedObjectNames = OwningComForm->UsedObjectNames;
    }
    else
    {
        IsFormObject = false;
        UsedObjectNames = UsedGlobalObjectNames;
    }

    UsedObjectNames->Add(_Name);

    OleControl = _Control;

    ObjectInfo = pObjectInfo;
    Name = _Name;
    ComDispatch = NULL;     // Created by Translator.
    
	if (AddToList)
	    ComObjects->Add(this);
}


TComObject::~TComObject()
{
	delete OleControl;
    delete ComDispatch;
    ComObjects->Remove(this);
}


TComHelper::TComHelper()
{
    pParams = NULL;
}


// Copy constructor; assigns a new OwningI2adl.  Called from InitComHelper.
TComHelper::TComHelper(TComHelper *ComHelper, I2adl *_OwningI2adl)
{
    OwningI2adl = _OwningI2adl;
    IsDynamic = ComHelper->IsDynamic;

    if (IsDynamic)
    	ComDispatch = NULL;
    else
	    OwningObject = ComHelper->OwningObject;

    MemberDesc = new TMemberDesc(ComHelper->MemberDesc);

	if (ComHelper->pParams == NULL)
	{
		pParams = NULL;
    }
    else
    {
        pParams = new DISPPARAMS;
        pParams->cArgs = ComHelper->pParams->cArgs;
        pParams->rgvarg = new VARIANTARG[pParams->cArgs];

        memcpy(pParams->rgvarg, ComHelper->pParams->rgvarg, sizeof(VARIANTARG) * pParams->cArgs);
    }

    ComI2adlList->Add(OwningI2adl);
}


TComHelper::~TComHelper()
{
    delete MemberDesc;
    ComI2adlList->Remove(OwningI2adl);

    if (pParams != NULL)
    {
        delete[] pParams->rgvarg;
        delete pParams;
    }

    if (IsDynamic && ComDispatch != NULL) // Dynamic helpers may have their own ComDispatch.
    {
        // Can't have ~TComDispatch() releasing the Dispatch Interface; reset it.
        ComDispatch->Dispatch = NULL;
        delete ComDispatch;
    }
}


ProcessGoEnum I2adl::ProcessGoSignal(Node *EventNode)
{
    if (EventNode->Name != NodestrGo)
        return goNot;

    if (*EventNode->DataPtr == 0 && Outputs->Count > 0)
    {
        Node	*DoneNode = Outputs->Items[0];

        if (DoneNode->DataPtr != NULL)
        {
            *DoneNode->DataPtr = 0;
            X86MainThread::AddTransportEvents(DoneNode->ConnectedTo);
        }

		if (Outputs->Count > 1)
        {
	        Node	*ExceptionNode = Outputs->Items[1];

            if (ExceptionNode->Name == NodestrException
            	&& ExceptionNode->DataPtr != NULL)
            {
                *ExceptionNode->DataPtr = 0;
                X86MainThread::AddTransportEvents(ExceptionNode->ConnectedTo);
            }
        }
    }

    if (Initializing)
        return goInitializing;

    if (*EventNode->DataPtr == 1)
        return goHigh;

    return goLow;
}


TComHelperFunc::TComHelperFunc(TComHelper *ComHelper, I2adl *_OwningI2adl) :
    TComHelper(ComHelper, _OwningI2adl)
{
    nFirstParamNode = (IsDynamic) ? 2 : 1;
}


TComHelperFunc::TComHelperFunc(I2adl *pI2adl, TMemberDesc *pMemberDesc, TComObject
	*pOwningObject) : TComHelper()
{
    OwningI2adl = pI2adl;
    MemberDesc = pMemberDesc;
    OwningObject = pOwningObject;
    IsDynamic = (pOwningObject == NULL);
    nFirstParamNode = (IsDynamic) ? 2 : 1;
}


// ConnectedTo may be NULL.
void TComHelperFunc::InitParam(VARIANTARG &Arg, Node *ConnectedTo)
{
    if (ConnectedTo == NULL)
    {
        Arg.vt = VT_ERROR;
        Arg.scode = DISP_E_PARAMNOTFOUND;
    }
    else
    {
        Arg.vt = ConnectedTo->DSet->VarType;
    }
}


// Called during behavior "translation" to set up the appropriate number of arguments,
//      and to determine the correct Variable Type for each other them, and to do other
//      various appropriate initilization.
void TComHelperFunc::InitParams()
{
    pParams = new DISPPARAMS;
    pParams->cArgs = (MemberDesc->Type & mtFunction) ? MemberDesc->FuncDesc->cParams
                                                     : 1;
    pParams->rgvarg = new VARIANTARG[pParams->cArgs];

    NodeList	*Inputs = OwningI2adl->Inputs;

    // Have to stuff parameters in backwards:
    if (MemberDesc->Type & mtFunction)
    {
        for (UINT i = 1; i <= pParams->cArgs && ((int) i) < Inputs->Count; i++)
            InitParam(pParams->rgvarg[pParams->cArgs - i]
            , Inputs->Items[i - 1 + nFirstParamNode]->ConnectedTo);
    }
    else
    {
        InitParam(pParams->rgvarg[0], Inputs->Items[nFirstParamNode]->ConnectedTo);
    }
}


// Calls the associated function on the Com object, and does error trapping.
HRESULT TComHelperFunc::ComCall(IDispatch *Dispatch, DISPID MemId, INVOKEKIND InvKind, bool IsVoid)
{
    VARIANT				*pResult = (IsVoid) ? NULL      // Pass a result holder only if needed.
                                      : &Result;
    static int			hr;
    static EXCEPINFO	ExcepInfo;
    UINT				ArgErr = 0;

    memset(&ExcepInfo, 0, sizeof(EXCEPINFO));

    if (pParams)
    {
        if (InvKind & (INVOKE_PROPERTYPUT | INVOKE_PROPERTYPUTREF))
        {
            // This is COM's 'special' way of signifying a property put:
            pParams->cNamedArgs = 1;
            pParams->rgdispidNamedArgs = (DISPID *) &dispidPropertyPut;
        }
        else
        {
            // For now; I don't know that we will ever support named arguments:
            pParams->cNamedArgs = 0;
            pParams->rgdispidNamedArgs = NULL;
        }
    }

    // The following call to GetIDsOfNames is necessary because the Dispatch ID's reported by
    //      the Dispatch interface does not always match that reported from the TypeInfo
    //      interface.  We need that from Dispatch, because it is what we invoke these functions
    //      through.
    try
    {
	    Dispatch->GetIDsOfNames(IID_NULL, &MemberDesc->RawName, 1, NULL, &MemId);
    }
    catch(...)
    {
    	ErrorTrace(4089, OwningI2adl);
        return E_POINTER;
    }

    DISPPARAMS 		*Params =
    	((InvKind == INVOKE_PROPERTYGET && MemberDesc->Type == mtProperty) || pParams == NULL)
        ? &dpNoArgs
        : pParams;

	try
    {
	    hr = Dispatch->Invoke(MemId, IID_NULL, LOCALE_SYSTEM_DEFAULT, (USHORT) InvKind,
    	     Params, pResult, &ExcepInfo, &ArgErr);
    }
    catch(Exception &e)
    {
        // Inform the user how to recover from errors while compiling.
        ErrorTrace(4078, OwningI2adl, e.Message);
        return E_UNEXPECTED;
    }

    switch (hr)
    {
        case DISP_E_EXCEPTION:
			if (ExcepInfo.bstrSource != NULL)
				ErrorTrace(4019, OwningI2adl, AnsiString(ExcepInfo.bstrDescription));
            else
                ErrorTrace(4020, OwningI2adl);
            break;

        case DISP_E_BADPARAMCOUNT:
            ErrorTrace(4028, OwningI2adl,
            	"Wrong number of arguments passed.  This function object is invalid.");
            break;

        case DISP_E_BADVARTYPE:
            ErrorTrace(4028, OwningI2adl,
            	"Bad variable type for argument #" + (pParams->cArgs - ArgErr));
            break;

        case DISP_E_MEMBERNOTFOUND:    // Bad member ID (FuncDesc->Memid)
            ErrorTrace(4028, OwningI2adl,  "This function may not exist on this object");
            break;

        case DISP_E_OVERFLOW:
        case DISP_E_TYPEMISMATCH:
            ErrorTrace(4028, OwningI2adl, AnsiString("The data type of argument #")
            	+ (pParams->cArgs - ArgErr)
                + " was invalid or could not be coerced into the correct type.");
            break;

        case DISP_E_PARAMNOTOPTIONAL:
            // They failed to hook up a node which corresponds to mandatory parameter
            ErrorTrace(4028, OwningI2adl,
            	AnsiString("Parameter #") + (pParams->cArgs - ArgErr) + " is not optional.");
            break;

        case DISP_E_PARAMNOTFOUND:    // Again, internal failure
            ErrorTrace(4028, OwningI2adl,
            	AnsiString("Parameter #") + (pParams->cArgs - ArgErr) + " not found.");
            break;

        case S_OK:
            break;

        default:
            ErrorTrace(4028, OwningI2adl);
            break;
    }

    return hr;
}


// Calls the Com Function associated with the Com I2adl to which this TComHelperFunc belongs,
//      after ensuring a valid Dispatch pointer; returns E_POINTER if this fails.
HRESULT TComHelperFunc::Invoke()
{
    IDispatch	*Dispatch;

    if (IsDynamic == false)
    {
        Dispatch = OwningObject->ComDispatch->Dispatch;
    }
    else
    {           
        // The Dispatch pointer should be fed in through a transport; if not, or if the value
        //      passed in is null, then we cannot call this COM function; issue error and return
        if (ComDispatch != NULL)
            Dispatch = ComDispatch->Dispatch;
        else
            Dispatch = NULL;

        if (Dispatch == NULL)
        {
            ErrorTrace(4037, OwningI2adl);
            return E_POINTER;
        }
    }

    // Load parameters up backwards; COM uses __stdcall specs:
    int		Count = pParams->cArgs;    // localize for speed

    for (int i = 0; i < Count; i++)
    {
        Node	*Input = OwningI2adl->Inputs->Items[i + nFirstParamNode];

        // The following fixes an access violation for Bruce (had left a node unconnected).
        if (Input == NULL)
            continue;

        Node	*ConnectedTo = Input->ConnectedTo;

        if (ConnectedTo != NULL)
        {
            VARIANTARG &Arg = pParams->rgvarg[Count - i - 1];

            BYTE	*val = (Arg.vt == VT_VARIANT) ? (BYTE *) &Arg : &Arg.bVal;
            BYTE	*DataPtr = ConnectedTo->DataPtr;
            int		ByteLength = min(Input->DSet->ByteLength, ConnectedTo->DSet->ByteLength);

            for (int j = 0; j < ByteLength; j++)
                *(val++) = *(DataPtr++);
        }
    }

    HRESULT		res = S_OK;
    bool		IsVoid;

    if (MemberDesc->Type & mtFunction)
    {
        FUNCDESC    *FuncDesc = MemberDesc->FuncDesc;

        IsVoid = (FuncDesc->elemdescFunc.tdesc.vt == VT_VOID);
        res = ComCall(Dispatch, FuncDesc->memid, FuncDesc->invkind, IsVoid);

        if (res == S_OK)
        {
            if (IsVoid)
            {
                Result.vt = VT_VOID;
                Result.lVal = 0;
            }
            else
            {
                OwningI2adl->CopyResultData(&Result.bVal);
            }
        }
    }
    else // Property; may need to call Get and/or Set methods:
    {
        VARDESC     *VarDesc = MemberDesc->VarDesc;

        IsVoid = (VarDesc->elemdescVar.tdesc.vt == VT_VOID);

        if (OwningI2adl->Outputs->Items[2]->ConnectedTo != NULL) // [2] is first param node.
        {
            res = ComCall(Dispatch, VarDesc->memid, INVOKE_PROPERTYGET, IsVoid);

            if (res == S_OK)
                OwningI2adl->CopyResultData(&Result.bVal);
        }

        if (OwningI2adl->Inputs->Items[nFirstParamNode]->ConnectedTo != NULL)
            ComCall(Dispatch, VarDesc->memid, INVOKE_PROPERTYPUT, IsVoid);
    }

    return res;
}


// Yes, we do need this constructor.
TComHelperEvent::TComHelperEvent(TComHelper *ComHelper, I2adl *_OwningI2adl)
    : TComHelper(ComHelper, _OwningI2adl)
{
}


TComHelperEvent::TComHelperEvent(I2adl *pI2adl, TMemberDesc *pMemberDesc, TComObject *
        pOwningObject) : TComHelper()
{
    OwningI2adl = pI2adl;
    MemberDesc = pMemberDesc;
    OwningObject = pOwningObject;
    IsDynamic = (pOwningObject == NULL);
}


HRESULT TComHelperEvent::Invoke()
{
    if (!IsRunning || TimedEventType == etNoEvent)
        return S_OK;

    // Copy data from any arguments over to the corresponding outputs:
    if (pParams != NULL && pParams->rgvarg != NULL) // Not all events give arguments.
        for (UINT i = 1; i <= pParams->cArgs; i++)
            OwningI2adl->CopyResultData(&pParams->rgvarg[pParams->cArgs - i].bVal, i);

    if (IsRunning)
        X86MainThread::GenerateDoneSignal_Com(OwningI2adl);

    // If any of the output nodes (other than "Done") are connected, then we need to
    //		process the downstream events now because as soon as the function returns, any
    //		pointers passed via pParms will be invalid.
    for (int i = 1; i < OwningI2adl->Outputs->Count; i++)
    {
    	if (OwningI2adl->Outputs->Items[i]->ConnectedTo != NULL)
        {
            if (!EventCycle(true) && !OwningI2adl->Warned0199)
            {
                // Warn the user that the COM event object did not receive a Reset signal.
                OwningI2adl->Warned0199 = true;

                ErrorTrace(199, OwningI2adl);
            }

        	break;
        }
    }

    return S_OK;
}


TComHelperEventWidget::TComHelperEventWidget(WidgetActiveX *pOwningWidget
    , TMemberDesc *pMemberDesc, TComObject *pOwningObject) : TComHelper()
{
    OwningWidget = pOwningWidget;
    MemberDesc = pMemberDesc;
    OwningObject = pOwningObject;
    IsDynamic = false;
}


HRESULT TComHelperEventWidget::Invoke()
{
    if (OwningWidget == NULL || OwningWidget->SourceNode->DataPtr == NULL)
        return E_POINTER;

    OwningWidget->OnWidgetChange(NULL);

    return S_OK;
}

