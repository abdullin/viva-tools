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
#include "ActiveXWidgetForm.h"
#include "AttributeEditor.h"
#include "ComManager.h"
#include "DataSet.h"
#include "ErrorTrap.h"
#include "I2adl.h"
#include "Node.h"

#pragma package(smart_init)
#pragma resource "*.dfm"


TActiveXWidgetSelector	*ActiveXWidgetSelector;


// Constructor
__fastcall TActiveXWidgetSelector::TActiveXWidgetSelector(TComponent* Owner)
    : TForm(Owner)
{
    ObjectBeingEdited = NULL;
}


// Set up the ActiveX widget selection form.
void __fastcall TActiveXWidgetSelector::FormShow(TObject *Sender)
{
	// Hide the Event components for Output objects.
    lstEvent->Visible = (ObjectBeingEdited->NameCode == NAME_INPUT);
    lblEvent->Visible = lstEvent->Visible;

    if (ObjectBeingEdited->NameCode == NAME_INPUT)
        Width = lstEvent->Left + lstEvent->Width + lstProgID->Left + 10;
    else
        Width = lstProperty->Left + lstProperty->Width + lstProgID->Left + 10;

    // Get a list of ActiveX components in our COM libraries.
    lstProgID->Items->Clear();

    for (int i = 0; i < ComLibraries->Count; i++)
    {
        TLibInfo	*LibInfo = (TLibInfo *) ComLibraries->Objects[i];

        if (LibInfo == NULL ||
			LibInfo->TypeLib == NULL)
	            continue;

        for (int j = 0; j < LibInfo->ObjectList->Count; j++)
        {
            TComObjectInfo	*ObjectInfo = (TComObjectInfo *) LibInfo->ObjectList->Objects[j];

            if (ObjectInfo != NULL &&
            	ObjectInfo->IsControl)
	                lstProgID->Items->AddObject(ObjectInfo->VivaProgID, (TObject *) ObjectInfo);
        }
    }

    // If the object already has an ActiveXWidget attribute, then highlight it.
    int		index = lstProgID->Items->IndexOf(ObjectBeingEdited->Attributes->Values[strActiveXWidget]);

    lstProgID->ItemIndex = max(index, 0);

    // Load the Property and Event list boxes for this component.
    lstProgIDClick(NULL);
}


// For the ActiveX component just selected, load its list of properties and events.
void __fastcall TActiveXWidgetSelector::lstProgIDClick(TObject *Sender)
{
    lstProperty->Items->Clear();
    lstEvent->Items->Clear();

    if (lstProgID->Items->Count < 1)
        return;

    TComObjectInfo	*ObjectInfo = (TComObjectInfo *) lstProgID->Items->Objects[lstProgID->
    	ItemIndex];

    if (ObjectBeingEdited->NameCode == NAME_INPUT)
		lstProperty->Items->AddObject(strDispatch, NULL);

    for (int i = 0; i < ObjectInfo->MemberList->Count; i++)
    {
        TMemberDesc		*MemberDesc = (TMemberDesc *) ObjectInfo->MemberList->Objects[i];
        INVOKEKIND		invkind = (ObjectBeingEdited->NameCode == NAME_INPUT)
        						? INVOKE_PROPERTYGET
								: INVOKE_PROPERTYPUT;

        if (MemberDesc->Type == mtProperty ||
            MemberDesc->FuncDesc->invkind == invkind)
        {
        	// This is a property.
            lstProperty->Items->AddObject(MemberDesc->Name, (TObject *) MemberDesc);
        }
        else if (ObjectBeingEdited->NameCode == NAME_INPUT &&
        		(MemberDesc->Type & mtEvent) == mtEvent)
        {
        	// This is an event.
            lstEvent->Items->AddObject(MemberDesc->Name, (TObject *) MemberDesc);
        }
    }

    // If the object already has a WidgetProperty attribute, then highlight it.
    int		index = lstProperty->Items->IndexOf(AttributeDialog->GetAttributeValue(
    	strWidgetProperty));

    lstProperty->ItemIndex = max(index, 0);

    // If the object already has a WidgetEvent attribute, then highlight it.
    index = lstEvent->Items->IndexOf(AttributeDialog->GetAttributeValue(strWidgetEvent));

    lstEvent->ItemIndex = max(index, 0);
}


// Save the ActiveX selections into the input/output object as widget attributes.
void __fastcall TActiveXWidgetSelector::btnOKClick(TObject *Sender)
{
	// If an ActiveX component was selected, then create an ActiveXWidget attribute.
    if (lstProgID->ItemIndex > -1)
    {
        AttributeDialog->SetAttribute(strActiveXWidget, lstProgID->Items->Strings[lstProgID->
        	ItemIndex]);

        // If a property was selected, then create a WidgetProperty attribute.
        if (lstProperty->ItemIndex > -1)
        {
            AttributeDialog->SetAttribute(strWidgetProperty, lstProperty->Items->Strings[
            	lstProperty->ItemIndex]);

            // Update the input/output object's data set.
            TMemberDesc		*MemberDesc = (TMemberDesc *) lstProperty->Items->Objects
            	[lstProperty->ItemIndex];

			DataSet		*DSet = SGDataSets[sdsIDispatch];

            if (MemberDesc != NULL)
	            DSet = GetOrMakeComDS("", MemberDesc->OwningObjectInfo->TypeInfo, MemberDesc->
                	GetTypeDesc());

            int 	row = (ObjectBeingEdited->NameCode == NAME_INPUT)
            			? AttributeDialog->FirstOutput
                        : AttributeDialog->FirstInput;

            if (ObjectBeingEdited->NameCode == NAME_INPUT ||
            	ObjectBeingEdited->Inputs->Items[0]->ConnectedTo == NULL)
            {
	            AttributeDialog->AttributeGrid->Cells[1][row] = DSet->Name;
            }
            else
            {
	            // Warn the user that we couldn't change the Output's data set.
                ErrorTrap(215, DSet->Name);
            }
        }

        // If an event was selected, then create a WidgetEvent attribute.
        if (lstEvent->ItemIndex > -1)
            AttributeDialog->SetAttribute(strWidgetEvent, lstEvent->Items->Strings[lstEvent->
            	ItemIndex]);
    }
}

