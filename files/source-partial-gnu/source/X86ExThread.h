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

#ifndef X86ExThreadH
#define X86ExThreadH

#include "Globals.h"

extern int    X86Active;
extern int    FPGActive;

extern X86UIThread  *X86UIThread_;

// Event Node structure:  Used to create link lists of Events.
struct EventLink
{
    Node         *Item;		// The Event stored in this node of the link list.
    EventLink    *Next;		// Pointer to the next EventLink in the link list.
};

extern EventLink    *HeadRecycle;

class ThreadHolder
{
public:
    PROCESS_INFORMATION		*ProcessInfo;
    I2adl					*ForObject;
    HANDLE					EventHandle;

    __fastcall ThreadHolder(PROCESS_INFORMATION *ExitCode, I2adl *EventObject, HANDLE EvHd);
    ~ThreadHolder();
};


class X86MainThread
{
public:

	enum MBOpCode	// Part of an abandoned attempt at multi-bit primitive object execution;
    {				//		now merely codes to indicate bit-level operation type.
    	mbAnd,
        mbOr,
        mbInvert,
        mbAdd,
    };

    VivaSystem      *OwningSystem;    // Parent system
    BYTE            *RunImage;        // Parent system's translator's RunImage buffer
    EventLink       *HeadNode;        // Pointer to the first EventLink in the "to do" list
    EventLink       *TailNode;        // Pointer to the last EventLink in the "to do" list
    VList           *ChildThreads;    // List of non exclusive child Viva processes created
    int             MaxNumberOfEvents;// Max number of events this system can process without
    								  //     being considered unstable.
    int             EventCountLeft;   // The number of additional events this system can
    								  //     can process before being considered unstable.

    __fastcall X86MainThread(VivaSystem *_OwningSystem);
    __fastcall~X86MainThread();

    virtual void __fastcall Initialize();
    virtual bool Execute(bool ProcessingComEvent);
    virtual void __fastcall InitializeEvents(I2adlList *Objects);
    virtual bool Event(Node *EventNode, bool ProcessingComEvent);
    virtual bool AddIOEvent(IOLink *SystemLink, bool ProcessingComEvent);

    void __fastcall AddEvent(Node *EventNode, bool PriorityEvent = false);
    static void AddTransportEvents(Node *ConnectNode, bool Priority = false);
    static void GenerateDoneSignal_Com(I2adl *EventObject);
    static void GenerateExceptionSignal(I2adl *EventObject);
    static void GenerateDoneSignal(I2adl *EventObject, bool State = true);
    void ProcessRamObjectSignal(I2adl *EventObject, Node *EventNode, bool DualWrite
    	, bool DualRead);
    void SendGoSignals(bool State = true);
    static void SendStopSignals(bool State = true);
    bool DoEvents(bool ProcessingComEvent);
    void ReportPendingEvents();
    void MultiByteOp(MBOpCode OpCode, int BitLength, Node *ResultNode, Byte *Op1
	    , Byte *Op2 = NULL, Byte *Op3 = NULL,  Node *ResultNode2 = NULL);

};

#ifdef IMPLIMENTATION_EXTENSION
class ImplementationThread : public X86MainThread
{
public:
    __fastcall ImplementationThread(VivaSystem *_OwningSystem);
    __fastcall ~ImplementationThread();

    virtual void __fastcall Initialize();
    virtual bool AddIOEvent(IOLink *SystemLink, bool ProcessingComEvent);
    virtual bool Event(Node *EventNode, bool ProcessingComEvent);
};
#endif

class X86UIThread : public X86MainThread
{
public:
    __fastcall X86UIThread(VivaSystem *_OwningSystem);
    __fastcall ~X86UIThread();

    virtual void __fastcall Initialize();
};


class FixedBusThread : public X86MainThread
{
public:
    __fastcall FixedBusThread(VivaSystem *_OwningSystem);
    __fastcall ~FixedBusThread();

    virtual void __fastcall Initialize();
    virtual bool Execute(bool ProcessingComEvent);
};

#endif
