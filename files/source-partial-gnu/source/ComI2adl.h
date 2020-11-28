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

#ifndef ComI2adlH
#define ComI2adlH

#include <map>

#include "I2adl.h"
#include "Widget.h"
#include "WidgetActiveX.h"

typedef VList   TMessageMap;

class TComHelperEvent;

// IDispatch is the class (defined in the Windows SDK) which controls the invokation of
//      functionality of a Com object instance, both outgoing (calls to Functions and Property
//      gets and sets) and incoming (Event notifications).
//      TComDispatch inherits IDispatch as its incoming interface, and implements a pointer
//      to another Dispatch Interface for its outgoing usage.  Thus, event notifications are
//      accomplished by the corresponding Com object via calls to the TComDispatch's
//      overridden Invoke function.  Other [outgoing] functionality of the Com object is
//      utilized via calls to the Invoke method of the member, "Dispatch".
class TComDispatch : public IDispatch
{
public:
    ULONG    nRefs; // Number of references [in other processes] to this interface.

    struct TMessageEntry
    {
        DISPID				Key;				// What event this entry is for
        										//		(click, keypress, etc)
        TComHelper			*hlpEvent;          // Links this dispatch container to a
        										//		com object instance.
    };

    IDispatch				*Dispatch;			// Incoming dispatch interface
    _GUID					OutgoingIntfGuid;	// Windows GUID for event dispatch interface
    TConnectionPointList	*ConnectedPoints;	// Keeps track of the event hookups
    TMessageMap				*MessageMap;		// Populated with entries for all I2adl objects
    											//		that require notification from events on
                                                //		this object, listed by Dispatch ID
                                                //		(DISPID), which in this case identifies
                                                //		the event.

    __stdcall virtual HRESULT QueryInterface(const _GUID &, void **);
    __stdcall virtual ULONG AddRef();
    __stdcall virtual ULONG Release();
    __stdcall virtual HRESULT GetIDsOfNames(REFIID riid, OLECHAR FAR *FAR *rgszNamesm, unsigned
        int cNames, LCID lcid, DISPID FAR *rgdispid);
    __stdcall virtual HRESULT GetTypeInfo(unsigned int itinfo, LCID lcid, ITypeInfo FAR *FAR *
            pptinfo);
    __stdcall virtual HRESULT GetTypeInfoCount(unsigned int FAR *pctinfo);
    __stdcall virtual HRESULT Invoke(DISPID dispidMember, REFIID riid, LCID lcid, unsigned short
            wFlags, DISPPARAMS FAR *pdispparams, VARIANT FAR *pvarResult, EXCEPINFO FAR *
            pexcepinfo, unsigned int FAR *puArgErr);

    TComDispatch(TComObjectInfo *pObjectInfo, IDispatch *pDispatch, bool Spoony = false);
    ~TComDispatch();

    void AddEntry(TComHelper *ComHelper);
};


// This is probably not the best name for this class, since it wraps a Com object, and is not
//      actually one, nor inherits from one.  Can represent either an ActiveX control on a
//      Viva Com Form, or a general Com object, which may or may not be an ActiveX control.
class TComObject
{
public:
    VOleControl		*OleControl;	// ActiveX that this object wraps; is NULL if this object
    								//		doesn't wrap a control.
    bool			IsFormObject;	// Control instantiated on a Viva com form
    TComDispatch	*ComDispatch;   // For incoming and outgoing event dispatch interfaces
    AnsiString		Name;			// To identify the com instance that this wraps.  At
    								//		present, must be unique per project even for ActiveX
                                    //		controls on form, since Text Base doesn't use form
                                    //		name to qualify controls.
    TComObjectInfo	*ObjectInfo;	// Everything you'd need to know about the com instance.

    TComObject(TComObjectInfo *pObjectInfo, AnsiString _Name, VOleControl *_Control = NULL,
    	bool AddToList = true);
    ~TComObject();

    void SetPropertyValue(bool Designing, TMemberDesc *MemberDesc, Variant Value, I2adl *Owner);
    Variant GetPropertyValue(bool Designing, TMemberDesc *MemberDesc, I2adl *Owner);
};


// This class was established largely as an alternative method of polymorphism: deriving a class
//      from I2adl would have required code changes in too many places.  This class holds the
//      member variables and functions that would have gone into the inheriting class.  Its use
//      is exclusively to be implemented in I2adl.
//      Contains information relevant to the runtime utilization of one member of a Com object.
//      Some of its data members are not populated until behavior "translation".
class TComHelper
{
public:

    ByteBool       IsDynamic;	// Dynamic instances do not apply to a particular Com instance.
    DISPPARAMS     *pParams;	// Holders for parameters to function calls, both incoming and
    						  	//		outgoing.
    VARIANT        Result;		// Return value of function or property get.
    TMemberDesc    *MemberDesc; // Data and interface of the member to which this applies.

    union
    {
        I2adl           *OwningI2adl;	// I2adl object that we are tying to a member of a
        								//		com instance.
        WidgetActiveX   *OwningWidget;	// Only used on Widget Form.
    };
    union
    {
        TComObject      *OwningObject;	// for static instances.
        TComDispatch    *ComDispatch;	// for dynamic instances.
    };

    TComHelper();
    TComHelper(TComHelper *ComHelper, I2adl *_OwningI2adl);
    ~TComHelper();
    virtual HRESULT Invoke() = 0;
};


// Brand of Com Helper that applies specifically to functions and properties.
class TComHelperFunc : public TComHelper
{
protected:

    int            nFirstParamNode;		// Index of first node, on applicable I2adl, that is
    									//		a parameter, not a Go, Dispatch, etc.
public:

    TComHelperFunc(I2adl *pI2adlMember, TMemberDesc *pMemberDesc, TComObject *pOwningObject);
    TComHelperFunc(TComHelper *ComHelper, I2adl *_OwningI2adl);
    static void InitParam(VARIANTARG &Arg, Node *ConnectedTo);
    void InitParams();
    HRESULT ComCall(IDispatch *Dispatch, DISPID MemId, INVOKEKIND InvKind, bool IsVoid);
    virtual HRESULT Invoke();
};


// Brand of Com Helper that applies specifically to events.
class TComHelperEvent : public TComHelper
{
public:
    TComHelperEvent(I2adl *I2adlObject, TMemberDesc *pMemberDesc, TComObject *pOwningObject);
    TComHelperEvent(TComHelper *ComHelper, I2adl *_OwningI2adl);
    virtual HRESULT Invoke();
};


class TComHelperEventWidget : public TComHelper
{
public:
    TComHelperEventWidget(WidgetActiveX *pOwningWidget, TMemberDesc *pMemberDesc
    	, TComObject *pOwningObject);
    virtual HRESULT Invoke();
};

#endif
