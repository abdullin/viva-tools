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

#include "Globals.h"
#include "ErrorTrap.h"
#include "I2adl.h"
#include "I2adlEditor.h"
#include "I2adlList.h"
#include "MyTShape.h"
#include "MyTScrollingWinControl.h"
#include "Node.h"
#include "VivaIcon.h"
#include "ComI2adl.h"
#include "DataSet.h"
#include "SearchEngine.h"
#include "ComManager.h"

#pragma package(smart_init)

// Constructor - Default
I2adlList::I2adlList(int InitialCapacity)
    : VList(sizeof(I2adl))
{
    // Allocate enough room for the anticipated number of pointers upfront.
    Capacity = InitialCapacity;
    CopyOf = NULL;

    InConnections = OutConnections = NULL;
    I2adlKeyList = NULL;
    LastViewScale = 0;
    LastHorzScroll = 0;
    LastVertScroll = 0;
    ModifiedOnOpen = false;
}


// Constructor - Copy
I2adlList::I2adlList(I2adlList *_I2adlList)
    : VList(sizeof(I2adl))
{
    if (_I2adlList == NULL)
        return;

    // Calls the "Copy list via '=' operator" method below.
    *this = *_I2adlList;

    InConnections = OutConnections = NULL;
    I2adlKeyList = NULL;
}


I2adlList::~I2adlList()
{
	// The following fixes an "access into deleted memory".
    if (this == OpenBehavior)
    	OpenBehavior = NULL;

    if (InConnections)
    {
        for (int i = 0; i < InConnections->Count; i++)
            delete (ConElem *) InConnections->Items[i];
    }

    if (OutConnections)
    {
        for (int i = 0; i < OutConnections->Count; i++)
            delete (ConElem *) OutConnections->Items[i];
    }

    delete InConnections;
    delete OutConnections;
    delete I2adlKeyList;
}


// Copy list via "=" operator
I2adlList &I2adlList::operator = (I2adlList &_I2adlList)
{
    if (this == &_I2adlList)
        return *this;

    // Clear the I2adl object VList without deallocating memory.
    Count = 0;

    // Allocate enough room for all of the pointers up front.
    if (Capacity < _I2adlList.Count)
        Capacity = _I2adlList.Count;

    // Packing the "source" I2adlList saves a lot of NULL checks.
    _I2adlList.Pack();

    // Pass I - Copy the I2adl objects in the list.
    for (int i = 0; i < _I2adlList.Count; i++)
    {
        I2adl	*Object = _I2adlList[i];

        Add(new I2adl(Object, cnYes));
    }

    // Pass II - Connect the nodes to the I2adl objects in the new I2adlList.
    //     This is required because the I2adl copy constructor is on too low a level
    //     to set the connected-to pointers.
    for (int i = 0; i < _I2adlList.Count; i++)
    {
        I2adl    *I2adlOld = _I2adlList[i];
        I2adl    *I2adlNew = (*this)[i];

        // The following fixes a memory leak in ExpandObject() when canuseprecompiled is true.
		I2adlNew->SearchListIndex = MAXINT;

        // Fix the connected-to pointers on the source nodes.
        for (int j = 0; j < I2adlOld->Outputs->Count; j++)
        {
            Node    *NodeOld = I2adlOld->Outputs->Items[j];
            Node    *NodeNew = I2adlNew->Outputs->Items[j];

            if (NodeOld->ConnectedTo != NULL)
            {
                int    OwnerIndex = _I2adlList.IndexOf(NodeOld->ConnectedTo->Owner);
                
                if (OwnerIndex != -1)
                {
                    int    ConnectIndex = NodeOld->ConnectedTo->GetNodeIndex(TYPE_INPUT);

                    if (ConnectIndex < 0)
	                    NodeNew->Disconnect();
                    else
	                    NodeNew->Connect((*this)[OwnerIndex]->Inputs->Items[ConnectIndex]);
                }
                else
                {
                    // Disconnect the node because the node it is connected to is not in the
                    //     list.  This occurs when part of a wip Sheet is copied.
                    NodeNew->Disconnect();
                }
            }
        }

        // Fix the connected-to pointers on the sink nodes.
        for (int j = 0; j < I2adlOld->Inputs->Count; j++)
        {
            Node    *NodeOld = I2adlOld->Inputs->Items[j];
            Node    *NodeNew = I2adlNew->Inputs->Items[j];

            if (NodeOld->ConnectedTo != NULL)
            {
                int    OwnerIndex = _I2adlList.IndexOf(NodeOld->ConnectedTo->Owner);

                if (OwnerIndex != -1)
                {
                    int    ConnectIndex = NodeOld->ConnectedTo->GetNodeIndex(TYPE_OUTPUT);

                    if (ConnectIndex < 0)
	                    NodeNew->Disconnect();
                    else
	                    NodeNew->Connect((*this)[OwnerIndex]->Outputs->Items[ConnectIndex]);
                }
                else
                {
                    // Disconnect the node because the node it is connected to is not in the
                    //     list.  This occurs when part of a wip Sheet is copied.
                    NodeNew->Disconnect();
                }
            }
        }
    }

    CopyOf = &_I2adlList;

    // If the CurrentSheet is being copied, then record the current I2adl Editor info.
    if (CurrentSheet != NULL &&
    	CurrentSheet->Behavior == &_I2adlList)
		    CurrentSheet->Behavior->RecordGraphicInfo();

    TreeGroup = _I2adlList.TreeGroup;
    Documentation = _I2adlList.Documentation;
    RecordGraphicInfo(&_I2adlList);

    return *this;
}


// Delete all the I2adl objects in the list.  DeleteObjects is not part of the
//     destructor because an I2adl object can appear in more than one list.
void I2adlList::DeleteObjects()
{
    if (this == NULL)
        return;

    for (int i = 0; i < Count; i++)
        delete (I2adl *) Items[i];

    Count = 0;
}


// Behavioral equality
bool I2adlList::Equals(I2adlList *Behavior, bool CompareListGraphicInfo, bool
	CompareObjectGraphicInfo, bool TestPrimitive)
{
    if (this == Behavior)
        return true;

    if (this == NULL || Behavior == NULL)
        return false;

    if (TreeGroup != Behavior->TreeGroup)
    	return false;

    if (Documentation != Behavior->Documentation)
    	return false;

    if (Count != Behavior->Count)
        return false;

    for (int i = 0; i < Count; i++)
        if (!(*this)[i]->Equals((*Behavior)[i], CompareObjectGraphicInfo, TestPrimitive))
            return false;

    // Are we checking the graphic info?
    if (CompareListGraphicInfo)
    {
    	if (LastViewScale != Behavior->LastViewScale ||
			LastHorzScroll != Behavior->LastHorzScroll ||
			LastVertScroll != Behavior->LastVertScroll)
        {
        	return false;
        }
    }

    return true;
}


// Returns the (X, Y) locations of the unconnected transports in the behavior.  If there
//     aren't any, then strNull is returned.
AnsiString I2adlList::GetUnconnectedTransportLocs()
{
    bool		FirstTransport = true;
	AnsiString	TransportLocations = strNull;

    for (int i = 0; i < Count; i++)
    {
        I2adl    *Transport = (*this)[i];

        if (Transport->NameCode != NAME_TRANSPORT)
        	continue;

        // Pass #1 - Inputs
        NodeList	*IONodes = Transport->Inputs;

        for (int IO = 0; IO < 2; IO++)
        {
            for (int j = 0; j < IONodes->Count; j++)
            {
            	if (IONodes->Items[j]->ConnectedTo == NULL)
                {
                	// We found an unconnected transport.  Save its location.
                	if (FirstTransport)
                    {
                    	FirstTransport = false;

                        // Unselect all other objects now.
                       	Deselect();
                    }
                    else
                    {
                    	TransportLocations += ", ";
                    }

                    TransportLocations += "(" + IntToStr(Transport->XLoc) + ", " +
                    	IntToStr(Transport->YLoc) + ")";

                    // Select the unconnected transport.
                    if (Transport->I2adlShape != NULL)
	                   	Transport->I2adlShape->Selected = true;

                	IO++;  // If this is Pass #1, then skip pass #2.
                    break;
                }
            }

            // Pass #2 - Outputs
            IONodes = Transport->Outputs;
        }
    }

    // If objects were selected/deselected, then redraw the sheet so the colors will be correct.
    if (TransportLocations != strNull)
	    RedrawSheet();

    return TransportLocations;
}


// Returns the (X, Y) location(s) where there are multiple objects in the behavior.  If there
//     aren't any, then strNull is returned.  This informs the user about objects that can't
//     be seen because there is an identical object on top of it.
AnsiString I2adlList::GetDuplicateObjectLocations()
{
    bool			FirstObject = true;
	AnsiString		ObjectLocations = strNull;
    TStringList		*SortedObjectLocs = new TStringList;

    SortedObjectLocs->Sorted = true;
    SortedObjectLocs->Duplicates = dupError;

    // Look at each object in the behavior.
    for (int i = 0; i < Count; i++)
    {
        I2adl    *Object = (*this)[i];

        // Transports can be at the same location.
        if (Object->NameCode == NAME_TRANSPORT)
        	continue;

        int			Index;
        AnsiString	ObjectLocation = IntToStr(Object->XLoc) + ", " + IntToStr(Object->YLoc);

        if (SortedObjectLocs->Find(ObjectLocation, Index))
        {
        	// Duplicate location!  Have we already reported this location?
            if (SortedObjectLocs->Objects[Index] != NULL)
            	continue;

            SortedObjectLocs->Objects[Index] = (TObject *) 1;

            // Save the new duplicate location.
            if (FirstObject)
                FirstObject = false;
            else
                ObjectLocations += ", ";

            ObjectLocations += "(" + ObjectLocation + ")";
        }
        else
        {
        	// New location.  Put it into the sorted list.
        	SortedObjectLocs->AddObject(ObjectLocation, NULL);
        }
    }

    delete SortedObjectLocs;

    return ObjectLocations;
}


// Return the first non transport/junction object up stream from this output node.
I2adl *I2adlList::SourceObject(Node *UpstreamNode)
{
    while (UpstreamNode != NULL)
    {
        I2adl    *UpstreamObject = UpstreamNode->Owner;

        if (UpstreamObject->NameCode == NAME_JUNCTION || UpstreamObject->NameCode == NAME_TRANSPORT)
            UpstreamNode = UpstreamObject->Inputs->Items[0]->ConnectedTo;
        else
            return UpstreamObject;
    }

    // Upstream connections did not lead to a source object.
    return NULL;
}


// Returns the first object that the data set is used on (or NULL).
//     CheckBehaviors is used to check the ProjectBehaviors from the ProjectObjects.  In this
//     case, the name of the ProjectObject is returned in the ParentObject.
I2adl *I2adlList::IsDataSetUsed(DataSet *DSet, bool CheckBehaviors, I2adl **ParentObject)
{
    *ParentObject = NULL;

    for (int i = 0; i < Count; i++)
    {
        I2adl    *Object = (I2adl *) Items[i];

        // The master exposer/collector objects are not considered a usage.
        if (this == ProjectObjects  &&  Object->ExposerCode != EXPOSER_NOT)
            continue;

        // Is the data set on this object?
        if (Object->Inputs->IsDataSetUsed(DSet) ||
            Object->Outputs->IsDataSetUsed(DSet))
        {
            return Object;
        }

        // Is the data set on the object's Behavior?
        if (!CheckBehaviors  ||  Object->Behavior == NULL)
            continue;

        I2adl    *BehaviorObject = Object->Behavior->IsDataSetUsed(DSet, false, ParentObject);

        if (BehaviorObject != NULL)
        {
            // Return the project object in the ParentObject parameter.
            *ParentObject = Object;

            return BehaviorObject;
        }
    }

    return NULL;
}


// Add the I2adl object to the list (if it is not already in the list).
bool __fastcall I2adlList::AddObject(I2adl *Object)
{
    if (IndexOf(Object) > - 1)
        return false;
    else
    {
        Add(Object);
        return true;
    }
}


// Returns the first I2adl object in the list whose NameCode matches (or NULL).
I2adl *I2adlList::GetObject(NameCodes _NameCode)
{
    if (this == NULL)
        return NULL;

    for (int i = 0; i < Count; i++)
    {
        I2adl    *Object = (I2adl *) Items[i];

        if (Object != NULL  &&  Object->NameCode == _NameCode)
            return Object;
    }

    return NULL;
}


// Returns the first I2adl object in the list whose name matches (or NULL).
//     MustHaveBehavior can limit the search to just objects that have a behavior.
I2adl *I2adlList::GetObject(VivaIcon *SearchNameInfo, bool MustHaveBehavior)
{
    if (this == NULL)
        return NULL;

    for (int i = 0; i < Count; i++)
    {
        I2adl    *Object = (I2adl *) Items[i];

        if (Object != NULL  &&  Object->NameInfo == SearchNameInfo)
            if (!MustHaveBehavior  ||  Object->Behavior != NULL)
                return Object;
    }

    return NULL;
}


// Returns the first I2adl object in the list whose behavior is SearchBehavior.
//		Returns NULL if no match was found.
//		TestCopyOfBehavior is used to find project objects that are open as wip sheets.
I2adl *I2adlList::GetObject(I2adlList *SearchBehavior, bool TestCopyOfBehavior)
{
    if (SearchBehavior != NULL)
	{
        for (int i = 0; i < Count; i++)
        {
            I2adl    *Object = (I2adl *) Items[i];

            if (!TestCopyOfBehavior)
            {
                if (Object->Behavior == SearchBehavior)
                    return Object;
            }
            else if (Object->Behavior != NULL)
            {
                if (Object->Behavior->CopyOf == SearchBehavior)
                    return Object;
            }
        }
    }

    return NULL;
}


// Determine if the I2adl object is in the list.
//     MatchState <  0, must match object footprint (name and number of Input/Output nodes).
//     MatchState == 0, must match name and equivalent object.
//     MatchState >  0, must match name, equivalent object and exact behavior.
I2adl *I2adlList::GetObject(I2adl *SearchObject, int MatchState)
{
    if (this == NULL)
        return NULL;

    for (int i = 0; i < Count; i++)
    {
        I2adl	*Object = (I2adl *) Items[i];

        // If the name doesn't match, then nothing else matters.
        if (Object == NULL || Object->NameInfo != SearchObject->NameInfo)
            continue;

        if (Object == SearchObject)
            return Object;

        if (MatchState < 0 &&
        	Object->Inputs->Count == SearchObject->Inputs->Count &&
            Object->Outputs->Count == SearchObject->Outputs->Count)
        {
            return Object;
        }

        // Do they match on the object/behavior level?
        if (Object->Equals(SearchObject))
            if (MatchState == 0 || Object->Behavior == SearchObject->Behavior)
                return Object;
    }

    return NULL;
}


// Get the inputs of the behavior page and place them in a new list.
TStringList *I2adlList::GetInputs(bool VisibleOnly)
{
    TStringList		*SortInputs = new TStringList;

    SortInputs->Sorted = true;
    SortInputs->Duplicates = dupAccept;

    for (int i = 0; i < Count; i++)
    {
        I2adl	*Object = (I2adl *) Items[i];

        if (Object != NULL &&
        	Object->NameCode == NAME_INPUT)
        {
            // Are we skipping the Inputs that do not become nodes on the parent object?
            if (VisibleOnly &&
            	Object->IsNoPromote())
                	continue;

            SortInputs->AddObject(IntToStr(10000000000 + 10000 * Object->Y + Object->X),
                (TObject *) Object);
        }
    }

    return SortInputs;
}


// Get the outputs of the behavior page and place them in a new list.
TStringList *I2adlList::GetOutputs(bool VisibleOnly)
{
    TStringList    *SortOutputs = new TStringList;

    SortOutputs->Sorted = true;
    SortOutputs->Duplicates = dupAccept;

    for (int i = 0; i < Count; i++)
    {
        I2adl    *Object = (I2adl *) Items[i];

        if (Object != NULL &&
        	Object->NameCode == NAME_OUTPUT)
        {
        	// Are we skipping the Outputs that do not become nodes on the parent object?
            if (VisibleOnly)
            	if (Object->IsNoPromote() ||
            		TrapCheck(Object))
		                continue;

            SortOutputs->AddObject(IntToStr(10000000000 + 10000 * Object->Y + Object->X)
                , (TObject *) Object);
        }
    }

    return SortOutputs;
}


// Set IsVisited flag of member objects.  Use zero to clear.
void I2adlList::ClearVisits(int VisitKey)
{
    for (int i = 0; i < Count; i++)
    {
        I2adl    *Object = (I2adl *) Items[i];

        if (Object != NULL)
            Object->IsVisited = VisitKey;
    }
}


// Patches up missing behavior Sheets.
void I2adlList::CheckObjects(bool CheckReiterate)
{
    for (int i = 0; i < Count; i++)
    {
        I2adl    *Object = (I2adl *) Items[i];

        if (Object->IsPrimitive)
            continue;

        if (Object->Behavior == NULL)
        {
            I2adl    *RepairObject = Object->ResolveObject();

            // Repair the behavior from a project object.
            //     Note: Project constructor has already reported problem objects.
            if (RepairObject != NULL)
            {
            	Object->IsPrimitive = RepairObject->IsPrimitive;
                Object->Behavior = RepairObject->Behavior;
            }
        }
        else
        {
            if (CheckReiterate)
                Object->Behavior->CheckObjects(false);
        }
    }
}


// Add into this list, all supporting project objects.
//		If SaveDependencies is false, then just load the objects in the tree group (not the
//		dependencies).
//		If IncludeLibraryObjects is false, then library objects will be skipped.
void I2adlList::ExpandReferences(bool SaveDependencies, bool IncludeLibraryObjects)
{
	// Increment the IsVisitedKey to prevent cyclic searches of objects.
    IsVisitedKey++;

    ClearVisits(IsVisitedKey);

    // Get the dependencies for each object in the list.
    //		Note:  Count will grow mid loop.
    bool    SavingAllTreeGroups = false;

    for (int i = 0; i < Count; i++)
    {
        I2adl        *ListObject = (I2adl *) Items[i];
        I2adlList    *ListObjectBehavior = ListObject->Behavior;

        if (ListObjectBehavior == NULL)
            continue;

        // The objects in the behavior are dependencies.
        for (int j = 0; j < ListObjectBehavior->Count; j++)
        {
            I2adl		*Object = (I2adl *) ListObjectBehavior->Items[j];

            if (Object->NameCode == NAME_TEXT)
            {
                // Look for TreeGroup macro.
                AnsiString		text = Object->AttSystem->c_str();
			    TReplaceFlags	ReplaceFlags = TReplaceFlags() << rfReplaceAll;

    			text = StringReplace(text, "\r\n", "", ReplaceFlags);

                char 			*xtext = text.c_str();

                if (strncmp(xtext, str_TreeGroup_.c_str(), str_TreeGroup_.Length()) != 0)
                    continue;

                // Get tree group name.
                xtext += 11;

                while (*xtext == ' ') // skip spaces.
                    xtext++;

                char    *xscan = xtext;

                // trim the string.
                char	*xend = xtext; // Pointer to string end.
                char	uend;

                while ((uend = *xscan) >= ' ') // Find the end of string.
                {
                    xscan++;

                    if (uend > ' ')
                        xend = xscan;
                }

                uend = *xend;
                *xend = 0;

                AnsiString	TreeGroupName(xtext);
                int			TreeGroupLen = TreeGroupName.Length();

                *xend = uend;

                // Are we saving all of the tree groups?
                if (TreeGroupName == "All")
                    SavingAllTreeGroups = true;

                // Add all of the objects in this tree group into the save list.
                for (int k = 0; k < ProjectObjects->Count; k++)
                {
                    I2adl		*ProjectObject = (I2adl *) ProjectObjects->Items[k];
                    I2adlList	*ObjectBehavior = ProjectObject->Behavior;

                    // System objects appear in the system tree (not the project object tree).
                    if (ProjectObject->Included == IsVisitedKey ||
                        ProjectObject->AttSystem != SystemstrNull ||
                        ObjectBehavior == NULL)
                    {
                        continue;
                    }

                    // Are we skipping library objects?
                    if (!IncludeLibraryObjects &&
                    	ProjectObject->OwningLib != NULL)
                        	continue;

                    // Is the object in this tree group?
                    if (!SavingAllTreeGroups)
                    {
						int		len = ObjectBehavior->TreeGroup.Length();

                        if (len < TreeGroupLen)
                        	continue;

                        if (ObjectBehavior->TreeGroup != TreeGroupName)
                        {
                        	if (len == TreeGroupLen ||
	                        	ObjectBehavior->TreeGroup[TreeGroupLen + 1] != '\\' ||
	                        	ObjectBehavior->TreeGroup.SubString(1, TreeGroupLen) != TreeGroupName)
                            {
								continue;
                            }
                        }
                    }

                    // Is the object already in the save list?
                    Add(ProjectObject);
                    ProjectObject->Included = IsVisitedKey;
                }
            }

            if (!SaveDependencies)
            	continue;

            // No need to save standard primitive objects.
            if (Object->IsStandardPrimitiveObject())
                continue;

            // Save off any project object with same footprint.
            for (int k = 0; k < ProjectObjects->Count; k++)
            {
                I2adl    *ProjectObject = (I2adl *) ProjectObjects->Items[k];

                if (ProjectObject->Included == IsVisitedKey ||
                	ProjectObject->NameInfo != Object->NameInfo ||
                	ProjectObject->Inputs->Count != Object->Inputs->Count ||
                	ProjectObject->Outputs->Count != Object->Outputs->Count ||
	            	ProjectObject->IsStandardPrimitiveObject())
                {
	                continue;
                }

                // Are we saving system objects?
                if (ProjectObject->AttSystem != SystemstrNull &&
                	(SettingFlags & SETTINGS_SAVE_SYSTEM_OBJECTS) == 0)
                        continue;

                // Are we skipping library objects?
                if (!IncludeLibraryObjects &&
                    ProjectObject->OwningLib != NULL)
                        continue;

                 // Do not put in saved list twice.
                ProjectObject->Included = IsVisitedKey;
                Add(ProjectObject);
            }
        }
    }

    // Reset the IsVisitedFlags so the objects will not be busy.
    ClearVisits();
}


// Record the I2adl Editor graphic information (Zoom percent/location).
//     If a Behavior is passed, then its graphic information is copied.
//     Otherwise, the current values are recorded.
void I2adlList::RecordGraphicInfo(I2adlList *Behavior)
{
	if (Behavior == NULL)
    {
        LastViewScale = ViewScale;
        LastHorzScroll = I2adlView->HorzScrollBar->Position / ViewScale;
        LastVertScroll = I2adlView->VertScrollBar->Position / ViewScale;
    }
    else
    {
        LastViewScale = Behavior->LastViewScale;
        LastHorzScroll = Behavior->LastHorzScroll;
        LastVertScroll = Behavior->LastVertScroll;
    }
}


// Determines if a Com Form, Library, or Object is used by this behavior; is recursive.
I2adl *I2adlList::UsesResource(TComResource &Resource)
{
    for (int i = 0; i < Count; i++)
    {
        I2adl *Object = (I2adl *) Items[i];

        if (Object->ExecuteType == EXECUTE_COM)
        {
			// Dynamic Com members only have libraries as dependencies:
            if (Object->ComHelper->IsDynamic && Resource.Type != crsLibrary)
                continue;

            if (Resource.Type == crsForm)
            {
                TComObject 		*OwningObject = Object->ComHelper->OwningObject;

                if (OwningObject->IsFormObject && OwningObject->OleControl->Owner
                    == Resource.Form)
                    return Object;
            }
            else if (Resource.Type == crsLibrary)
            {
                if (Object->ComHelper->MemberDesc->OwningObjectInfo->OwningLibInfo == Resource.LibInfo)
                    return Object;
            }
            else if (Resource.Type == crsObject)
            {
                if (Object->ComHelper->OwningObject == Resource.Object)
                    return Object;
            }
        }
        else if (Object->ExecuteType == NAME_DISPATCH)
        {
            if (Resource.Type == crsObject)
            {
                if (Object->ComObject == Resource.Object)
                    return Object;
            }
            else if (Resource.Type == crsForm)
            {
                TComObject 		*OwningObject = Object->ComObject;

                if (OwningObject->IsFormObject && OwningObject->OleControl->Parent == Resource.Form)
                    return Object;
            }
        }
        else if (Resource.Type == crsLibrary)
        {
            if (Object->ExecuteType == NAME_COMCREATE)
            {
                if (Object->CreateObjectInfo->OwningLibInfo->FileName == Resource.LibInfo->FileName)
                    return Object;
            }
            else if (Object->NameCode == NAME_INPUT || Object->NameCode == NAME_OUTPUT)
            {
                AnsiString 		ActiveXWidget;

                Object->GetAttribute(strActiveXWidget, ActiveXWidget);

                if (ActiveXWidget != strNull)
                {
                    if (ParseLibraryID(ActiveXWidget) == Resource.LibInfo->LibID)
                        return Object;
                }
            }
        }
    }

    return NULL;
}


// returns a list of all I2adl objects that "look like" [LookFor], according to
//		[SearchCriteria].  Also associates each item in the return list with the parent I2adl.
I2adlList *I2adlList::FindInstances(I2adl *LookFor, SearchCriteriaEnum SearchCriteria
	, I2adl *ParentI2adl, AnsiString SearchString, TStringList *NamePieces)
{
	I2adlList	*list = new I2adlList(16);

    for (int i = 0; i < Count; i++)
    {
        I2adl		*comp = (I2adl *) Items[i];

        if (comp == NULL)
        	continue;

        if (NamePieces != NULL)
        {
        	// We are doing a wildcard search.  Skip objects that are used a lot.  If they
            //		are really wanted, then they can do a non-wildcard search.
            if (comp->NameCode == NAME_INPUT ||
            	comp->NameCode == NAME_OUTPUT ||
                comp->NameCode == NAME_TRANSPORT ||
                comp->NameCode == NAME_JUNCTION ||
                comp->NameCode == NAME_TEXT ||
                comp->NameCode == NAME_VARIANT_IN ||
                comp->NameCode == NAME_VARIANT_OUT)
            {
            	continue;
            }

            bool		NameMatches = true;
            AnsiString	ParseName = comp->NameInfo->Name;

            for (int j = 0; j < NamePieces->Count; j++)
            {
                AnsiString	NamePiece = NamePieces->Strings[j];

                // Null strings always match.
                if (NamePiece == strNull)
                    continue;

                int		NamePos = ParseName.Pos(NamePiece);

                if (NamePos <= 0)
                {
                    // The name does not contain the required piece.
                }
                else if (j <= 0 &&
                         NamePos != 1)
                {
                    // The name does not start with the first piece.
                }
                else
                {
                    ParseName = ParseName.SubString(NamePos + NamePiece.Length(),
                        ParseName.Length() + 1 - NamePos - NamePiece.Length());

                    if (ParseName == strNull ||
                        j < NamePieces->Count - 1)
                            continue;

                    // The name doees not end with the last piece.
                }

                NameMatches = false;

                break;
            }

            if (!NameMatches)
            	continue;
        }
        else if (SearchString != strNull)
        {
            if (SearchString != comp->NameInfo->Name)
                continue;
        }
        else
        {
	        if (comp->NameInfo != LookFor->NameInfo)
    	    	continue;

            if (SearchCriteria > scName
                && (comp->Inputs->Count != LookFor->Inputs->Count
                    || comp->Outputs->Count != LookFor->Outputs->Count))
                continue;

            if (SearchCriteria == scOverload && comp->Behavior != LookFor->Behavior)
                continue;
        }

        if (ParentI2adl)
        {
            comp->MemberOf = MainCompiler->ObjectTrace(ParentI2adl, false);

	        if (comp->MemberOf->MemberOf != NULL)
    	    	comp->MemberOf = comp->MemberOf->MemberOf;
        }

        list->Add(comp);
    }

    return list;
}


// Fixes the output data set on all $Select objects that have a disconnected #0 or #1 input
//		node.  Reexpansion prevents this function from being performed inside the "=" operator.
void I2adlList::FixSelectDataSets()
{
    for (int i = 0; i < Count; i++)
    {
        I2adl    *Object = (*this)[i];

        if (Object->NameCode == NAME_SSELECT)
        {
            // Fix the data sets if the #0 or #1 inputs are disconnected.
            for (int j = 0; j < 2; j++)
            {
                Node    *InputNode = Object->Inputs->Items[j];

                if (InputNode->ConnectedTo == NULL)
                {
                    InputNode->PropagateDataSet(InputNode->DSet);

                    break;
                }
            }
        }
    }
}


// Fixes all of the unconnected transports in the behavior so they will be written to text
//		base by temporarily connecting them to a phony Input/Output object.
//		If RemoveTemporaryObjects is true, then the phony objects are removed (instead of
//		added in).
void I2adlList::FixUnconnectedTransports(bool RemoveTemporaryObjects)
{
    for (int i = 0; i < Count; i++)
    {
        I2adl    *Object = (*this)[i];

        // Are we deleting phony objects already added into the behavior?
        if (RemoveTemporaryObjects)
        {
        	if (Object->UnconnectedXport == ResourcestrTempConnectObject)
            {
                delete Object;
                Delete(i);

                i--;
            }
        }
        else
        {
        	// Is this a primitive transport?
            if (Object->IsPrimitive &&
            	Object->NameCode == NAME_TRANSPORT)
            {
            	for (int j = 0; j < Object->Inputs->Count; j++)
                {
                	Node	*InputNode = Object->Inputs->Items[j];

                    if (InputNode->ConnectedTo == NULL)
                    {
                    	// Connect the unconnected InputNode to a temporary Input object.
                        I2adl	*TempObject = new I2adl(NAME_INPUT, Object->X + InputNode->
                        	Left - 4 * ViewScale, Object->Y + InputNode->Top - ViewScale);

                        TempObject->Outputs->Items[0]->DSet = InputNode->DSet;
                        TempObject->Outputs->Items[0]->Connect(InputNode);
                        TempObject->UnconnectedXport = ResourcestrTempConnectObject;

                        Add(TempObject);
                    }
                }

                for (int j = 0; j < Object->Outputs->Count; j++)
                {
                	Node	*OutputNode = Object->Outputs->Items[j];

                    if (OutputNode->ConnectedTo == NULL)
                    {
                    	// Connect the unconnected OututNode to a temporary Input object.
                        I2adl	*TempObject = new I2adl(NAME_OUTPUT, Object->X + OutputNode->
                        	Left, Object->Y + OutputNode->Top - ViewScale);

                        TempObject->Inputs->Items[0]->DSet = OutputNode->DSet;
                        TempObject->Inputs->Items[0]->Connect(OutputNode);
                        TempObject->UnconnectedXport = ResourcestrTempConnectObject;

                        Add(TempObject);
                    }
                }
            }
        }
    }
}


TProjectObjectsList::TProjectObjectsList()
{
	KeyList = new TStringList;
	KeyList->Sorted = true;
    KeyList->Duplicates = dupIgnore;
}


TProjectObjectsList::~TProjectObjectsList()
{
	for (int i = 0; i < KeyList->Count; i++)
    	delete (VList *) KeyList->Objects[i];

	delete KeyList;
}


int TProjectObjectsList::Add(void *Item)
{
    I2adl			*object = (I2adl *) Item;
    FastString      key;

    MakeKey(key, object);

	int		index = KeyList->IndexOf(key.String);

    if (index == -1)
    {
		VList	*list = new VList(sizeof(I2adl));

        list->Add((TObject *) Item);

        KeyList->AddObject(key.String, (TObject *) list);
    }
    else
    {
    	VList	*list = (VList *) KeyList->Objects[index];

		if (list->IndexOf((TObject *) Item) == -1)
	        list->Add((TObject *) Item);
    }

    if (object->Behavior != NULL)
    	for (int i = 0; i < object->Behavior->Count; i++)
        	((I2adl *) object->Behavior->Items[i])->ParentI2adl = object;

	return VList::Add(Item);
}


int TProjectObjectsList::Remove(void *Item)
{
	int 	ind = IndexOf(Item);

    Delete(ind);

    return ind;
}


void TProjectObjectsList::Delete(int Index)
{
	// We were passed the NotInList indicator?
	if (Index <= -1)
    	return;

    I2adl			*object = (I2adl *) Items[Index];
	FastString      key;

    MakeKey(key, object);

	int				keylistindex = KeyList->IndexOf(key.String);

    // Testing for -1 fixes an access violation when a really old file has been read in.
    if (keylistindex > -1)
    {
        VList	*list = (VList *) KeyList->Objects[keylistindex];

        list->Remove(object);

        if (list->Count == 0)
        {
            delete (VList *) KeyList->Objects[keylistindex];
            KeyList->Delete(keylistindex);
        }
    }

	VList::Delete(Index);
}


int TProjectObjectsList::Insert(int Index, void *Item)
{
    I2adl			*object = (I2adl *) Item;
	FastString      key;

	if (object != NULL)
	{
	    MakeKey(key, object);

        int				keylistindex = KeyList->IndexOf(key.String);

        if (keylistindex == -1)
        {
			VList	*list = new VList(sizeof(I2adl));

    	    list->Add(object);
	        KeyList->AddObject(key.String, (TObject *) list);
        }
        else
        {
	        VList	*list = (VList *) KeyList->Objects[keylistindex];
            bool   	inserted = false;

            for (int i = 0; i < list->Count; i++)
            {
                int		keyind = IndexOf(list->Items[i]);

                if (keyind >= Index)
                {
                    list->Insert(i, object);
                    inserted = true;

                    break;
                }
            }

            if (!inserted)
                list->Insert(0, object);
        }
    }

    return VList::Insert(Index, Item);
}


void TProjectObjectsList::MakeKey(FastString &Key, I2adl *Object)
{
    Key.Add(Object->NameInfo->Name.c_str());
    Key.Add(char(1));

    char	intstr[12];

    itoa(Object->Inputs->Count, intstr, 10);
    Key.Add(intstr);
    Key.Add(char(1));
    itoa(Object->Outputs->Count, intstr, 10);
    Key.Add(AnsiString(intstr));
}




