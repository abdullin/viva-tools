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
#include "Dismember.h"
#include "I2adl.h"
#include "I2adlList.h"
#include "Project.h"
#include "VivaIcon.h"


// Main constructor.
VDismember::VDismember()
{
	MemberOf = NULL;
	SourceI2adl = NULL;
	MemberName = NULL;
	Documentation = SystemstrNull;
	XLoc = 0;
	YLoc = 0;

    // Record the top level sheet that was compiled.
    if (MyProject == NULL)
    {
	    TopLevelSheet = NULL;
    }
    else
    {
    	TopLevelSheet = (MyProject->Compiling) ? MyProject->CompilingSheet
        									   : MyProject->CompiledSheet;

        // If the top level sheet came from a project object, then record it.  This prevents
        //		it from being lost after the wip sheet has closed (which can happen
        //		automatically).
        if (TopLevelSheet != NULL)
        {
        	I2adl	*I2adlDef = ProjectObjects->GetObject(TopLevelSheet->Behavior->CopyOf);

            if (I2adlDef != NULL)
            	TopLevelSheet = I2adlDef;
        }
    }
}


// Returns "true" if the Dismembers are equal.
bool VDismember::Equals(VDismember *Dismember)
{
    if (Dismember == NULL)
    	return false;

	return (MemberName == Dismember->MemberName &&
    		Documentation == Dismember->Documentation &&
    		SourceI2adl == Dismember->SourceI2adl &&
    		XLoc == Dismember->XLoc &&
    		YLoc == Dismember->YLoc);
}


// If the objects in the hierarchy are still valid, then return them.
void VDismember::GetValidObjects(I2adl **ParentObject, I2adl **ChildObject)
{
	*ChildObject = NULL;
    *ParentObject = NULL;

    if (this == NULL)
        return;

    I2adl	*RecordedObject = (MemberOf == NULL) ? TopLevelSheet
    											 : MemberOf->SourceI2adl;

    if (RecordedObject == NULL)
        return;

    // Find the corresponding project object/Wip sheet.
    if (ProjectObjects->IndexOf(RecordedObject) > -1)
    {
        *ParentObject = RecordedObject;
    }
    else if (MyProject->Wip->IndexOf(RecordedObject) > -1)
    {
        *ParentObject = ProjectObjects->GetObject(RecordedObject->Behavior->CopyOf);

        // If we don't have a project object, then use the wip sheet.
        if (*ParentObject == NULL)
            *ParentObject = RecordedObject;
    }
    else
    {
        return;
    }

	// Find the closest matching ChildObject in the parent's behavior.
    for (int i = 0; i < (*ParentObject)->Behavior->Count; i++)
    {
        I2adl	*NextObject = (* (*ParentObject)->Behavior)[i];

        if (NextObject->XLoc == XLoc &&
            NextObject->YLoc == YLoc)
        {
            *ChildObject = NextObject;

            if (NextObject->NameInfo->Name == *MemberName)
            	return;
        }
    }
}


// Format the hierarchy information into a string.
AnsiString VDismember::QualifiedName()
{
	AnsiString	QualifiedName = *MemberName
                          	  +  " (" + *Documentation
                          	  +  " " + IntToStr(XLoc)
                          	  +  "," + IntToStr(YLoc)
                          	  +  ")";

	return QualifiedName;
}

#pragma package(smart_init)
