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

#ifndef I2adlListH
#define I2adlListH

#include "Globals.h"
#include "MessageInfo.h"
#include "List.h"

// Connection Element used in ExpandObject and ReExpand, to indicate sources and sinks, into an
//		object's behavior, for transports connected to nodes on the outer object.
struct ConElem
{
    int		num;

    union
    {
        int		objindex;
        I2adl	*obj;
    };
    union
    {
        int		nodeindex;
        Node	*node;
    };
};


class I2adlList : public VList
{
public:

    I2adlList       *CopyOf;			// The I2adlList that this object was copied from.
    AnsiString      TreeGroup;			// The name of the root ObjectTree node that this object
										//     is grouped under.
    AnsiString      Documentation;		// Multiple lines stored in TStrings::Text format (so
										//      that word wrap will work)
    AnsiString      FileName;			// Used when sheets are to be saved to file.
    VList           *InConnections;		// Connectors for pre-expanded behaviors.
    VList           *OutConnections;	// Connectors for pre-expanded behaviors.
    StringTable     *I2adlKeyList;		// Entries of I2adl Name + instance number, for quick
										//		reference resolution in loader.
    int				LastViewScale;		// Last zoom percent this sheet was viewed in.
    int				LastHorzScroll;		// Last horizontal scroll bar position for the sheet.
										//     Stored in grid snaps.
    int				LastVertScroll;		// Last vertical scroll bar position for the sheet.
										//     Stored in grid snaps.
    bool			ModifiedOnOpen;		// If was auto-saved due to transport re-alignment, etc.

    I2adlList(int InitialCapacity = 7);
    I2adlList(I2adlList *_I2adlList);
    ~I2adlList();
    I2adlList & operator = (I2adlList &_I2adlList);
    void DeleteObjects();

    // Overload "[]" to code "(I2adl *) _I2adlList->Items[i]" as "(*_I2adlList)[i]".
    inline I2adl *operator[](int i)
    {
        return ((I2adl *) this->Items[i]);
    }

    bool Equals(I2adlList *Behavior, bool CompareListGraphicInfo = false, bool
    	CompareObjectGraphicInfo = true, bool TestPrimitive = true);
    AnsiString GetUnconnectedTransportLocs();
    AnsiString GetDuplicateObjectLocations();
    I2adl *SourceObject(Node *UpstreamNode);
    I2adl *IsDataSetUsed(DataSet *DSet, bool CheckBehaviors, I2adl **ParentObject);
    bool __fastcall AddObject(I2adl *Object);
    I2adl *GetObject(NameCodes _NameCode);
    I2adl *GetObject(VivaIcon *_NameInfo, bool MustHaveBehavior = false);
    I2adl *GetObject(I2adlList *_Behavior, bool TestCopyOfBehavior = false);
    I2adl *GetObject(I2adl *_I2adl, int MatchState);
    TStringList *GetInputs(bool VisibleOnly = true);
    TStringList *GetOutputs(bool VisibleOnly = true);
    void ClearVisits(int VisitKey = 0);
    void CheckObjects(bool CheckReiterate = true);
    I2adl *UsesResource(TComResource &Resource);
    void ExpandReferences(bool SaveDependencies, bool IncludeLibraryObjects = true);
    void RecordGraphicInfo(I2adlList *Behavior = NULL);
	I2adlList *FindInstances(I2adl *LookFor, SearchCriteriaEnum SearchCriteria, I2adl
    	*ParentI2adl, AnsiString SearchString = strNull, TStringList *NamePieces = NULL);
    void FixSelectDataSets();
    void FixUnconnectedTransports(bool RemoveTemporaryObjects);
};


// Facilitates fast acquisition of a list of all overloads for an object name.
class TProjectObjectsList : public I2adlList
{
public:
	TStringList		*KeyList;				// Key strings and associated I2adls, of prior
    										//		context-specific I2adl resolutions
	TProjectObjectsList();
	~TProjectObjectsList();

    virtual int Add(void *Item);
    virtual int Remove(void *Item);
    virtual void Delete(int Index);
	int Insert(int Index, void *Item);    
	void MakeKey(FastString &Key, I2adl *Object);

    __property int Count = {read = FCount};
};


#endif
