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

#include "process.h"
#include "Globals.h"
#include "I2adl.h"
#include "DataSet.h"
#include "ErrorTrap.h"
#include "AttributeEditor.h"
#include "VivaIcon.h"
#include "Main.h"
#include "MyTShape.h"
#include "Node.h"
#include "Project.h"
#include "ResourceManagers.h"
#include "VivaSystem.h"
#include "WidgetForm.h"
#include "I2adlEditor.h"
#include "MyTScrollingWinControl.h"
#include "ActiveXWidgetForm.h"
#include "EditTextFile.h"

#pragma package(smart_init)
#pragma resource "*.dfm"

#define CELLTYPE_LABEL		(void*)   0
#define CELLTYPE_INPUT		(void*)   1
#define CELLTYPE_OUTPUT		(void*)   2
#define CELLTYPE_ATTRIBUTE	(void*)   9

TAttributeDialog    *AttributeDialog;

// Constructor - Main
__fastcall TAttributeDialog::TAttributeDialog(TComponent *Owner) :
    TForm(Owner)
{
    DropdownCodes = new IntegerList(sizeof(int));
    UsingMouse = true;
    bSettingCell = false;
    AttributeList = NULL;

    for (int i = 0; i < koLast; i++) // Add options in for KeepObject.
        rgKeepObject->Items->Add(KeepObjectOptionStrings[i]);
}


// Destructor
__fastcall TAttributeDialog::~TAttributeDialog()
{
    delete DropdownCodes;
		delete AttributeList;
}


// This routine loads the AttributeDialog form so the user can edit the information on
//     the object level.  The object is updated with the changes the user makes on the form.
//
//     The following cases are handled:
//     1.  Creating an object from a new behavior (OrigObject == NULL).
//     2.  Creating/updating an object from an existing behavior (OrigObject != NewObject).
//     3.  Editing an I2adl "def" (OrigObject == NewObject && I2adlTShape == NULL).
//     4.  Editing an I2adl "ref" (OrigObject == NewObject && I2adlTShape != NULL).
//
//     Only the first two cases require NewObject to have a behavior.
//     The first two cases will either delete NewObject or turn it into a project object.
//
//     NodesReordered indicates if the order of the I/O nodes was changed while editing the
//     behavior Sheet.
//
void __fastcall TAttributeDialog::EditObjectAttributes(I2adl *OrigObject, I2adl *NewObject)
{
	EllipsisButton->Visible = false;

    DropdownCodes->Count = 0;

    // Determine what kind of editing is being done.
    if (OrigObject == NULL)
        EditCase = CreateNew;
    else if (OrigObject != NewObject)
        EditCase = CreateFromExisting;
	else if (NewObject->I2adlShape == NULL)
		EditCase = EditDef;
	else
		EditCase = EditRef;

	I2adlObject = NewObject;

    if (EditCase == EditDef &&
    	I2adlObject->OwningLib != NULL)
    {
		txtLibSysName->Text = *I2adlObject->OwningLib;
        txtLibSysName->Visible = true;
		lblLibSys->Visible = true;
    }
    else if (EditCase == EditDef &&
    		 I2adlObject->IsSystemObject &&
			 MyProject->DynamicSystemName != strNull)
    {
		txtLibSysName->Text = MyProject->DynamicSystemName;
        txtLibSysName->Visible = true;
		lblLibSys->Visible = true;
    }
    else
    {
		txtLibSysName->Text = strNull;
        txtLibSysName->Visible = false;
		lblLibSys->Visible = false;
    }

	// Don't let the user update Documentation or Treegroup on a library/system object:
    cmbTreeGroup->Enabled = true;
    DocumentationMemo->ReadOnly = false;

    if (I2adlObject->Behavior != NULL)
    {
        I2adl	*def = ProjectObjects->GetObject(I2adlObject->Behavior);

		if (def != NULL)
        {
			if (def->OwningLib != NULL
#if VIVA_SD == false
				|| def->IsSystemObject
#endif
				)
            {
		        cmbTreeGroup->Enabled = false;
    		    DocumentationMemo->ReadOnly = true;
            }
        }
    }

	if (I2adlObject->IsPrimitive)
	{
		rgKeepObject->Visible = true;
		AttribBox->Height = (rgKeepObject->Top - optCreateObject->Top) - 7;
		AttribBox->Top = optCreateObject->Top;
	}
	else
	{
		rgKeepObject->Visible = false;
		AttribBox->Height = IconBox->Top - 7
        	- (optCreateObject->Top + optCreateObject->Height + 7);
		AttribBox->Top = optCreateObject->Top + optCreateObject->Height + 7;
	}

	rgKeepObject->ItemIndex = I2adlObject->KeepObject;

	bool		NodesReordered = false;
    I2adlList	*NewBehavior = NULL;

    if (EditCase >= EditDef)
    {
	    NewBehavior = NewObject->Behavior;
    }
    else
    {
		NewBehavior = new I2adlList(NewObject->Behavior);

        I2adlList 	*realbehavior = NewObject->Behavior;

		NewObject->Behavior = NewBehavior;
        NodesReordered = NewObject->ArrangeNodes(false, false);
		NewObject->Behavior = realbehavior;
    }

	bool		HasBehavior = (NewBehavior != NULL);

    // Load the input node names/data sets into the attribute grid.
    RowNumber = 0; // Always 1 larger than the last row number loaded.

    // RowLoad() will raise Rowcount as needed.
    RowLoad("Inputs:", "Data Sets:", false);

    FirstInput = RowNumber;

    int    InputCount = NewObject->Inputs->Count;

    for (int i = 0; i < InputCount; i++)
    {
        Node          *_Node = NewObject->Inputs->Items[i];
        AnsiString    NodeName = *_Node->Name;

        if (NodeName == strIn)
            NodeName = strIn + IntToStr(i + 1);

        RowLoad(NodeName, _Node->DSet->Name);
    }

    // Load the output node names/data sets into the attribute grid.
    RowLoad("Outputs:", "Data Sets:", false);

	FirstOutput = RowNumber;
	int    OutputCount = NewObject->Outputs->Count;

	for (int i = 0; i < OutputCount; i++)
    {
        Node          *_Node = NewObject->Outputs->Items[i];
        AnsiString    NodeName = *_Node->Name;

        if (NodeName == strOut)
            NodeName = strOut + IntToStr(i + 1);

        RowLoad(NodeName, _Node->DSet->Name);
    }

    // Load the attribute strings into the grid.
    RowLoad("Attributes:", "Values:", false);

    FirstAttribute = RowNumber;

    // If there is an icon indirection, then load the "Icon" attribute string.
    //     This temporary attribute string will be removed by SetIconInfo().
    EditIcon = NewObject->GetIconInfo();

    if (NewObject->HasIconIndirection())
        RowLoad(strIcon, EditIcon->Name);

    RowLoad(strSystem, *NewObject->AttSystem, 7);
    RowLoad(strResource, *NewObject->AttResource, 7);

    if (NewObject->AttResourceQty != 1)
        RowLoad(strResourceQty, IntToStr(NewObject->AttResourceQty), 7);

    RowLoad(strChildAttribute, *NewObject->AttChildAtt, 7);
    RowLoad(strProvidesResource, *NewObject->AttProvidesRes, 7);

    // Get indirection attributes.
    for (int i = 0; i < NewObject->Attributes->Count; i++)
	{
		AnsiString    _Name = NewObject->Attributes->Names[i];
		RowLoad( _Name, NewObject->Attributes->Values[_Name]);
	}

	// Add extra rows for entering new attribute strings.
	for (int i = 0; i < 8; i++)
		RowLoad(strNull, strNull);

	// Remove the rows that were not initialized.
	AttributeGrid->RowCount = RowNumber;    // Count is 1 more than last row.

	// Don't allow the user to edit the data sets of:
	//     1. Composite I2adl "Defs"
	//     2. Existing (not in the process of being created) primitive objects, except for
    //        $Cast, Input & Output.
	//     3. Nodes where Def's corresponding node's Data Set is not Variant
	bool	AllReadOnly = (EditCase != EditRef) || (OrigObject->IsPrimitive &&
													OrigObject->NameCode != NAME_SYSCAST &&
													OrigObject->NameCode != NAME_INPUT &&
													OrigObject->NameCode != NAME_OUTPUT);

    if (EditCase == EditDef && OrigObject->IsPrimitive)
		AllReadOnly = false;  // Primitive I2adl "Def".

	int		NodeIndex;
	int		NodeIndexStart = 0;

	// Also, initialize DropdownCodes with ordinal positions of inputs, outputs,
	//      and attributes:
	for (int i = 0; i < RowNumber; i++)
	{
		bool	ThisReadOnly = false;
		void	*NodeType = CELLTYPE_LABEL; // Input, Output, or neither (CELLTYPE_LABEL).

		// Input:
		if (i >= FirstInput && i < FirstInput + InputCount)
		{
			NodeType = CELLTYPE_INPUT;
			NodeIndex = NodeIndexStart + i - FirstInput;
		} // Output:
		else if (i >= FirstOutput && i < FirstOutput + OutputCount)
		{
			NodeType = CELLTYPE_OUTPUT;
			NodeIndex = NodeIndexStart + i - FirstOutput;
		}
		else if (i >= FirstAttribute)
        {
			DropdownCodes->Add(CELLTYPE_ATTRIBUTE);
        }
		else
        {
			DropdownCodes->Add(CELLTYPE_LABEL);
        }

		if (NodeType != CELLTYPE_LABEL)
        {
			// Check this node on the I2adl's Def to see if it is Variant.  If it is not, then
            //		make this cell read-only.   Don't bother to check if AllReadOnly is true.
			if (!AllReadOnly &&
            	OrigObject != NULL &&
                OrigObject->NameCode != NAME_INPUT &&
                OrigObject->NameCode != NAME_OUTPUT)
			{
				if (OrigObject->Behavior != NULL)
				{
					I2adl	*Def = ProjectObjects->GetObject(OrigObject->Behavior);

					if (Def != NULL)
                    {
                        Node	*IONode = (NodeType == CELLTYPE_INPUT)
									 	? Def-> Inputs->Items[NodeIndex]
                                     	: Def->Outputs->Items[NodeIndex];

                        if (IONode->DSet != VariantDataSet)
							ThisReadOnly = true;
                    }
				}
				else if (OrigObject->NameCode == NAME_SYSCAST)
				{
                	// $Cast objects can have their unconnected input nodes changed.
					if (NodeType == CELLTYPE_OUTPUT ||
					   (NodeType == CELLTYPE_INPUT &&
                		OrigObject->Inputs->Items[NodeIndex]->ConnectedTo != NULL))
					{
						ThisReadOnly = true;
					}
				}
			}

			if (AllReadOnly || ThisReadOnly)
				AttributeGrid->Objects[1][i] = READ_ONLY_CELL;

			DropdownCodes->Add(NodeType);
		}
	}

	// Initialize the other AttributeDialog form data members.
	ObjectName->Text = NewObject->NameInfo->Name;

	// Are we editing a behavior or just an object?
    if (EditCase <= CreateFromExisting)
	{
		Caption = "Convert Sheet to Object";
        ObjectName->ReadOnly = false;
        ObjectName->Color = clWindow;
        ObjectName->TabStop = true;
        ActiveControl = ObjectName;
        ObjectName->SelectAll();    // User can replace name by just typing.
	}
	else
	{
        Caption = "Edit Attributes";
        ObjectName->ReadOnly = true;
        ObjectName->Color = clBtnFace;
        ObjectName->TabStop = false;
        ActiveControl = AttributeGrid;
	}

	// If the object does not have a behavior, then turn off the right half of the form.
	//     Otherwise, load the right half of the form.
	optUpdateObject->Visible = false;
	optCreateObject->Visible = false;
	CheckLeaveOpen->Visible = false;

    // If this is a library/system object, then don't allow the object to be changed.
   	OKButton->Enabled = true;

	if (EditCase == EditDef && OrigObject != NULL)
    {
    	if (OrigObject->OwningLib != NULL)
	       	OKButton->Enabled = false;

#if VIVA_SD == false
		if (OrigObject->IsSystemObject)
	       	OKButton->Enabled = false;
#endif
    }

	rgKeepObject->Enabled = OKButton->Enabled;
	DisplayImage->Enabled = OKButton->Enabled;
	btnDiscard->Enabled = OKButton->Enabled;
	btnRevert->Enabled = OKButton->Enabled;
	IconRefresh->Enabled = OKButton->Enabled;

	if (OKButton->Enabled)
       	AttributeGrid->Options << goEditing;
    else
       	AttributeGrid->Options >> goEditing;

	// These variables are only used in this next 'if' statement
	static int 		lastWidth = Width; // used to restore the correct width
	static bool 	smalled = false; // smalled meaning !HasBehavior last time we came through

	if (!HasBehavior)
	{
		if (!smalled)
		{
			lastWidth = Width;
			smalled = true;
		}

		Constraints->MaxWidth = DocBox->Left + 7;
        Constraints->MinWidth = DocBox->Left + 7;
		Width = DocBox->Left + 7;

		WindowState = wsNormal;
		DocBox->Visible = false;
		lblTreeGroup->Visible = false;
		cmbTreeGroup->Visible = false;
	}
	else
	{
		if (smalled)
		{
			smalled = false;
			// this next line is strange, but it is to get around a bug in the TControl
			Constraints->MinWidth = lastWidth;
			Width = lastWidth;
		}

		lblTreeGroup->Visible = true;
		cmbTreeGroup->Visible = true;

		// the standard sizes are 7px between objects
		DocBox->Visible = true;
		DocBox->Width = Width - DocBox->Left - 14;
		cmbTreeGroup->Width = Width - (lblTreeGroup->Left + lblTreeGroup->Width) - 21;
		cmbTreeGroup->Left = (lblTreeGroup->Left + lblTreeGroup->Width) + 7;
		DocBox->Top = AttribBox->Top;
		DocBox->Height = IconBox->Top + IconBox->Height - AttribBox->Top;
		Constraints->MaxWidth = 0;
		Constraints->MinWidth = DocBox->Left + 140;

		DocumentationMemo->Lines->Text = NewBehavior->Documentation;
		cmbTreeGroup->Items = ObjectTreeGroups;

		// Default the tree group based on the status of the object.
		if (EditCase == CreateNew)
		{
			cmbTreeGroup->Text = strNull;
		}
		else
		{
			// Existing object.  Use the old tree group name.
			cmbTreeGroup->Text = NewBehavior->TreeGroup;
		}

		// Only show the update/create object controls if the behavior has been edited.
		if (EditCase <= CreateFromExisting)
		{
			optUpdateObject->Visible = true;
			optCreateObject->Visible = true;
			CheckLeaveOpen->Visible = true;
			CheckLeaveOpen->Checked = false;
			AttribBox->Height = (IconBox->Top - (optCreateObject->Top + optCreateObject->Height + 14));
			AttribBox->Top = (optCreateObject->Top + optCreateObject->Height + 7);
			DocBox->Top = AttribBox->Top;
			DocBox->Height = AttribBox->Height + IconBox->Height + 7;

			if (EditCase == CreateNew ||
               (OrigObject != NULL && OrigObject->OwningLib != NULL)
#if VIVA_SD == false
            || (OrigObject != NULL && OrigObject->IsSystemObject)
#endif
               )
			{
				// New behavior Sheet - Must create a new object.
				optUpdateObject->Enabled = false;
				optCreateObject->Checked = true;
			}
			else
			{
				// Existing behavior Sheet - Default is to update object (unless name changed).
				optUpdateObject->Enabled = true;

				if (OrigObject->NameInfo == NewObject->NameInfo)
					optUpdateObject->Checked = true;
				else
					optCreateObject->Checked = true;
			}
		}
		else
        {
			AttribBox->Height = (IconBox->Top - optCreateObject->Top) - 7 ;//  NewGridHeight;
			AttribBox->Top = optCreateObject->Top;
			DocBox->Top = AttribBox->Top;
			DocBox->Height = (IconBox->Top - optCreateObject->Top) + IconBox->Height;
		}
	}

	DisplayImage->Enabled = !(NewObject->NameCode == NAME_JUNCTION ||
							   NewObject->NameCode == NAME_TRANSPORT);

    bool 	CancelAttributeEditor;

    while (true)
    {
        ShowModal(); // Display dialog box for user input.

        CancelAttributeEditor = (ModalResult != mrOk);

        // Did the user cancel the changes?
        if (CancelAttributeEditor)
            break;

        // If the user has entered an attribute name more than once, then issue a warning:
        for (int i = FirstAttribute + 1; i < AttributeGrid->RowCount; i++)
        {
            if (AttributeGrid->Cells[0][i].Trim() == strNull)
                continue;

            for (int j = FirstAttribute; j < i; j++)
            {
                if (AttributeGrid->Cells[0][i] == AttributeGrid->Cells[0][j])
                {
                    ErrorTrap(4005, AttributeGrid->Cells[0][i]);
                    break;
                }
            }
        }

        // Possible NodesChanged states:
        //     -1 - Creating a new object or changing I2adl "ref" (changes don't matter)
        //      0 - Still looking to see if changes have been made.
        //      1 - Changes have been made (no need to check for or approve additional changes)
        int     NodesChanged = 0;
	    bool    CreateNewObject = false;
        bool	CancelNodeChanges = false;

        if (EditCase == CreateNew ||
           (EditCase == CreateFromExisting && optCreateObject->Checked))
        		CreateNewObject = true;

             if (CreateNewObject)								NodesChanged = -1;
        else if (EditCase == EditRef)							NodesChanged = -1;
        else if (NodesReordered)								NodesChanged = 1;
        else if (OrigObject->Inputs->Count != InputCount)		NodesChanged = 1;
        else if (OrigObject->Outputs->Count != OutputCount)		NodesChanged = 1;

        // Changing the order of the nodes, the number of nodes or their data sets could
        //     invalidate any behavior page that already contains the Object.
        if (NodesChanged == 1)
            if (ErrorTrap(53) != mrYes)
				continue;  // Redisplay the form.

        // Update the Input node names/data sets in the object and the behavior.
        if (NewObject->I2adlShape)
            NewObject->I2adlShape->PaintNames(WorkSpaceColor);

        for (int i = 0; i < InputCount; i++)
        {
            Node    *InputNode = NewObject->Inputs->Items[i];

            // Update the node name in the object.
            AnsiString    UpdateName = AttributeGrid->Cells[0][FirstInput + i];

            if (UpdateName.Trim() == strNull)
            {
                // User deleted the node name.  Restore the default node name.
                if (NewObject->NameCode == NAME_OUTPUT)
                    UpdateName = *NodestrOut;
                else
                    UpdateName = *NodestrIn;
            }

            // Update the node name.
            if (UpdateName != *InputNode->Name)
                InputNode->SetName(UpdateName);

            // Update the data set in the object.
            DataSet *_DataSet = GetDataSet(AttributeGrid->Cells[1][FirstInput + i]);

            if (_DataSet == NULL)
            {
                ErrorTrap(51, AttributeGrid->Cells[1][FirstInput + i]);
                // Invalid data set name.
            }
            else
            {
                // Were the nodes changed on the behavior Sheet or in the dialog box?
                if (!CreateNewObject && i < OrigObject->Inputs->Count)
                {
                	Node	*OriginalNode = OrigObject->Inputs->Items[i];

                    if (_DataSet != OriginalNode->DSet)
                    {
                    	if (!IsChangingNodesOK(NodesChanged, CancelNodeChanges))
                        	break;

                        // Propagate the data set through attached transports/junctions.
                        InputNode->PropagateDataSet(_DataSet);
                    }

                    if (InputNode->Name != OriginalNode->Name)
                    {
                        // If either name is a control node name, then this change is
                        //		significant.
                        if (InputNode->IsControlNode() ||
                        	OriginalNode->IsControlNode())
                        {
                            if (!IsChangingNodesOK(NodesChanged, CancelNodeChanges))
                                break;
                        }
                    }
                }
            }

            // Update the node name/data set in the behavior.
            if (HasBehavior && EditCase != EditRef) // I2adl "ref" does not own the behavior.
            {
                // Input objects are first in the behavior.
                I2adl    *Object = (*NewBehavior)[i];

                if (Object->NameCode != NAME_INPUT)
                {
                    ErrorTrap(48, "I2adlEditor::ConvertSheet").IntegerValue = i;

					continue; // Object/behavior out of sync.
                }

                Object->Outputs->Items[0]->SetName(*InputNode->Name);

                // Propagate the data set through attached transports/junctions.
                Object->Outputs->Items[0]->PropagateDataSet(InputNode->DSet);
            }
        }

        if (CancelNodeChanges)
        	continue;

        // Update the Output node names/data sets in the object and the behavior.
        for (int i = 0; i < OutputCount; i++)
        {
            Node    *OutputNode = NewObject->Outputs->Items[i];

            // Update the node name in the object.
            AnsiString    UpdateName = AttributeGrid->Cells[0][FirstOutput + i];

            if (UpdateName.Trim() == strNull)
            {
                // User deleted the node name.  Restore the default node name.
                if (NewObject->NameCode == NAME_INPUT)
                    UpdateName = *NodestrIn;
                else
                    UpdateName = *NodestrOut;
            }

            // Update the node name.
            if (UpdateName != *OutputNode->Name)
                OutputNode->SetName(UpdateName);

            // Update the data set in the object.
            DataSet    *_DataSet = GetDataSet(AttributeGrid->Cells[1][FirstOutput + i]);

            if (_DataSet == NULL)
            {
                ErrorTrap(51, AttributeGrid->Cells[1][FirstOutput + i]);
                // Invalid data set name.
            }
            else
            {
                // Were the nodes changed on the behavior Sheet or in the dialog box?
                if (!CreateNewObject && i < OrigObject->Outputs->Count)
                {
                	Node	*OriginalNode = OrigObject->Outputs->Items[i];

                    // NAME_SYSCAST and NAME_SSELECT cases are handled differently in
                    //      PropagateOperation; don't undo what it does:
                    if (_DataSet != OriginalNode->DSet &&
                    	OrigObject->NameCode != NAME_SYSCAST &&
                        OrigObject->NameCode != NAME_SSELECT)
                    {
                    	if (!IsChangingNodesOK(NodesChanged, CancelNodeChanges))
                        	break;

                        // Propagate the data set through attached transports/junctions.
                        OutputNode->PropagateDataSet(_DataSet);
                    }

                    if (OutputNode->Name != OriginalNode->Name)
                    {
                        // If either name is a control node name, then this change is
                        //		significant.
                        if (OutputNode->IsControlNode() ||
                        	OriginalNode->IsControlNode())
                        {
                            if (!IsChangingNodesOK(NodesChanged, CancelNodeChanges))
                                break;
                    	}
                    }
                }
            }

            // Update the node name/data set in the behavior.
            if (HasBehavior && EditCase != EditRef) // I2adl "ref" does not own the behavior.
            {
                // Output objects are second in the behavior.
                I2adl    *Object = (*NewBehavior)[InputCount + i];

                if (Object->NameCode != NAME_OUTPUT)
                {
                    ErrorTrap(49, "I2adlEditor::ConvertSheet").IntegerValue =  i;
                    continue; // Object/behavior out of sync.
                }

                Object->Inputs->Items[0]->SetName(*OutputNode->Name);

                // Propagate the data set through attached transports/junctions.
                Object->Inputs->Items[0]->PropagateDataSet(OutputNode->DSet);
            }
        }

        // If the control nodes were changed, then fix them up.
        I2adlList 	*realbehavior = NewObject->Behavior;

		NewObject->Behavior = NewBehavior;

        if (NewObject->ArrangeNodes(false, false, true, false))
	        IsChangingNodesOK(NodesChanged, CancelNodeChanges);

		NewObject->Behavior = realbehavior;

        if (CancelNodeChanges)
        	continue;

        // If this is a Def, then remove it from the old system so that it doesn't reset any
        //		"System" attribute.
        if (EditCase == CreateFromExisting || EditCase == EditDef)
        {
	        I2adl	*orig = (EditCase == CreateFromExisting) ? NewObject->CopyOf
    	    												 : NewObject;

	        VivaSystem 		*sys = orig->GetSystem();

    	    if (sys != NULL && sys->SystemObjects != NULL)
        	    sys->SystemObjects->Remove(orig);
        }

        // Replace all of the existing attribute strings.
        NewObject->ClearAttributes();

        for (int i = FirstAttribute; i < AttributeGrid->RowCount; i++)
        {
            // Skip entire row if either cell is empty.
            AnsiString AttKey = AttributeGrid->Cells[0][i].Trim();

            if (AttKey == strNull)
                continue;

            AnsiString	AttVal = AttributeGrid->Cells[1][i];

            if (AttVal.Trim() == strNull)
                continue;

            if (AttKey == strConstant ||
            	AttKey == strDefault ||
                AttKey == strDocumentation ||
                AttKey == strTrap)
            {
                DecodeString(AttVal);
            }

            NewObject->SetAttribute(AttKey, AttVal, false);
        }

        // Update NewObject with the changes the user made on the form.
        NewObject->SetIconInfo(true);

        NewObject->KeepObject = (KeepObjectEnum) rgKeepObject->ItemIndex;
        NewObject->Conformalize();

        if (HasBehavior)
        {
            NewBehavior->Documentation = DocumentationMemo->Lines->Text;
            NewBehavior->TreeGroup = cmbTreeGroup->Text;
        }

        // Final cleanup for editing objects only (behavior Sheet was not edited).
        if (EditCase > CreateFromExisting)
        {
            if (EditCase == EditDef)
            {
                // I2adl "def" - Sync project objects because the object's system may have changed.
                BaseSystem->SyncLists();
                MyProject->BuildTreeView();
            }
            else
            {
                NewObject->Conformalize();
            }

            // Redisplay the current sheet so that the changes will appear:
            RebuildUI();

            bool	SystemChanged = EditCase == EditDef &&
            					   (NewObject->IsSystemObject ||
            						NewObject->AttSystem != SystemstrNull);

            MyProject->Modified(SystemChanged);
        }
        else
        {
            if (EditCase == CreateFromExisting && CheckLeaveOpen->Checked)
            {
                I2adlView->Invalidate();
                I2adlView->Update();

			    // Make sure the Play button is accurate.
			    MyProject->CompiledSheet = MyProject->CompiledSheet;
            }

            // Final cleanup for editing objects and their behavior Sheets.
            if (CreateNewObject)
            {
                NewObject->Behavior = NewBehavior;

                // If this new object may become a primitive, then give the user the option.
                if (IsCandidateForPrimitive(NewObject) &&
                	ErrorTrap(3020) == mrYes)
                {
                    // System primitive objects must have a Resource attribute.
                    if (NewObject->AttSystem != SystemstrNull &&
                        NewObject->AttResource == ResourcestrNull)
                    {
                        ErrorTrap(14);

                        continue;  // Redisplay the form.
                    }

                    // Change the object into a primitive by deleting the behavior.
                    NewObject->Behavior->DeleteObjects();
                    delete NewObject->Behavior;

                    NewObject->IsPrimitive = true;
                    NewObject->Behavior = NULL;
                }

                // Turn NewObject into a project object (OrigObject just goes away).
                //     Make a copy of the current Sheet because RemoveSheet deletes it.
                MyProject->ActiveWipNode->Text = NewObject->NameInfo->Name;  // In case sheet not closed.
                ProjectObjects->Add(NewObject);

                // Keep the list of Prev/Next sheets correct by replacing the old object
                //		with the one it is creating.
                int		Index = (EditCase == CreateNew)
                			  ? NavigationSheetList->IndexOf(CurrentSheet)
                              : NavigationSheetList->IndexOf(OrigObject);

                if (Index > -1)
                	NavigationSheetList->Items[Index] = NewObject;
            }
            else
            {
                // Update the original object with the changes that are now in NewObject.
                OrigObject->Height = NewObject->Height;
                OrigObject->Width = NewObject->Width;
                OrigObject->ReplaceAttributes(NewObject);

                // Replace the node lists in OrigObject with the node lists in New Object.
                //     The following code is much faster than DeleteNodeList() & NewNodeList().

				// Before we tamper with a Def, remove it from the ProjectObjects list.
                int 	ind = ProjectObjects->Remove(OrigObject);

                DeleteNodeList(OrigObject->Inputs);
                OrigObject->Inputs = NewObject->Inputs;
                OrigObject->Inputs->SetOwner(OrigObject);
                NewObject->Inputs = NullNodeList;

                DeleteNodeList(OrigObject->Outputs);
                OrigObject->Outputs = NewObject->Outputs;
                OrigObject->Outputs->SetOwner(OrigObject);
                NewObject->Outputs = NullNodeList;

				ProjectObjects->Insert(ind, OrigObject);

				// Replace the behavior without invalidating pointers to it.
                OrigObject->Behavior->DeleteObjects();

                *OrigObject->Behavior = *NewBehavior;

				NewBehavior->DeleteObjects();
				delete NewBehavior;
                delete NewObject;

                NewObject = OrigObject;
            }

            BaseSystem->SyncLists();
            MyProject->BuildTreeView();

            bool	SystemChanged = (NewObject->IsSystemObject ||
            						 NewObject->AttSystem != SystemstrNull);

            MyProject->Modified(SystemChanged);

            // Does the user want to leave the behavior sheet open for more editing?
            if (CheckLeaveOpen->Checked)
            {
                RemoveSheet(CurrentSheet);
                LoadAsSheet(NewObject);
            }
            else
            {
                OpenBehavior = NULL;

                I2adl	*LastSheet = CurrentSheet;

                NavigateSheet(false);
                RemoveSheet(LastSheet);

				if (NewObject->Behavior != NULL)
	                NewObject->Behavior->CopyOf = NULL;
            }
        }

        break;
    }

    if (CancelAttributeEditor)
    {
    	if (EditCase <= CreateFromExisting)
        {
            NewBehavior->DeleteObjects();
            delete NewBehavior;
	        delete NewObject;
        }
    }
    else
    {
    	if (EditCase <= CreateFromExisting)
        {
        	if (NewObject->ResolveObject(false, false) != NewObject)
            	ErrorTrap(3051, NewObject->NameInfo->Name);

            if (NewObject->Behavior != NULL)
	            for (int i = 0; i < NewObject->Behavior->Count; i++)
	                ((I2adl *) NewObject->Behavior->Items[i])->ParentI2adl = NewObject;
        }
    }
}


// Returns true if the Object can be changed into a primitive object.
bool TAttributeDialog::IsCandidateForPrimitive(I2adl *Object)
{
	if (EditCase != CreateNew ||
    	Object->Behavior == NULL)
	    	return false;

    // Does the object have at least one input/output node?
    if (Object->Inputs->Count <= 0 &&
    	Object->Outputs->Count <= 0)
	        return false;

    // Does the behavior have only input and output objects?
	for (int i = 0; i < Object->Behavior->Count; i++)
    {
        I2adl    *IOObject = (*Object->Behavior)[i];

		if (IOObject->NameCode != NAME_INPUT &&
        	IOObject->NameCode != NAME_OUTPUT)
	            return false;
    }

    return true;
}


// Load the AttributeGrid with the strings passed.  If the string is read only, then
//     READ_ONLY_CELL will be placed in the string's object cell.
//     Bits in ChangeSet:
//         if (ChangeSet & 1)   Column 1 is changeable
//         if (ChangeSet & 2)   Column 2 is changeable
//         if (ChangeSet & 4)   Do not display row if second string is NULL
//     ChangeSet defaults to 3 to allow both columns to be changed.
void __fastcall TAttributeDialog::RowLoad(AnsiString String0, AnsiString String1, int ChangeSet)
{
		// Do not display null attribute values.
    if (ChangeSet & 4 && String1 == strNull)
        return;

    // Save a level of indirection.
    int    rowNumber = RowNumber;  

    // Is there room for this row in the grid?
	if (AttributeGrid->RowCount < rowNumber + 1)
        AttributeGrid->RowCount = rowNumber + 5;

    // Load the two strings.
    if (String0 == strConstant ||
    	String0 == strDefault ||
        String0 == strDocumentation ||
        String0 == strTrap)
    {
		String1 = EncodeString(String1);
    }

	AttributeGrid->Cells[0][rowNumber] = String0;
    AttributeGrid->Cells[1][rowNumber] = String1;

	// If the cell is not changeable, then add a "read only" indicator as the object.
    AttributeGrid->Objects[0][rowNumber] = (ChangeSet & 1) ? NULL
                                                           : READ_ONLY_CELL;
    AttributeGrid->Objects[1][rowNumber] = (ChangeSet & 2) ? NULL
                                                           : READ_ONLY_CELL;

    RowNumber++;
}


// Finds the position of a string in the Atttibute combo box, if existent:
int __fastcall TAttributeDialog::FindAttributeComboBoxItemPos(const AnsiString &Item)
{
    for (int i = 0; i < AttributeComboBox->Items->Count; i++)
        if (Item == AttributeComboBox->Items->Strings[i])
            return i;

    return -1;
}


// Returns true if the mouse is within the bounds of AttributeComboBox
bool __fastcall TAttributeDialog::InACBBounds()
{
    int    x = Mouse->CursorPos.x - Left;
    int    y = Mouse->CursorPos.y - Top;

    int    NumItems = (AttributeComboBox->DropDownCount < AttributeComboBox->Items->Count)
                    ? AttributeComboBox->DropDownCount
                    : AttributeComboBox->Items->Count;

	int    left = AttributeComboBox->BoundsRect.Left;
    int    top = AttributeComboBox->BoundsRect.Top + (2 * AttributeComboBox->ItemHeight);
    int    right = AttributeComboBox->BoundsRect.Right + 8;
    int    bottom = AttributeComboBox->BoundsRect.Bottom + 28 + NumItems * AttributeComboBox->
           ItemHeight;
    int    loc = AttributeComboBox->ClientToScreen(TPoint(0, 0)).y + bottom - top;

    if (loc > Screen->Height)
    {
        int    temp = bottom;

        bottom = top + AttributeComboBox->ItemHeight;
        top -= (temp - top);
    }

    return !(x < left || x > right || y < top || y > bottom + 10);
}


// Performs operations common to all appearances of AttributeComboBox:
void __fastcall TAttributeDialog::ShowAttributeComboBox(int Column, TComboBoxStyle Style
    , bool DroppedDown)
{
	// The following fixes problems with the Windows "Hide pointer while typing" option
    //		(Bugzilla #905).
	Screen->Cursor = crArrow;
	Screen->Cursor = crDefault;

    for (int i = FirstAttribute; i < AttributeGrid->RowCount; i++)
    {
        if (i == AttributeGrid->Row || AttributeGrid->Cells[0][i].Trim() == strNull)
            continue;

        for (int j = 0; j < AttributeComboBox->Items->Count; j++)
		{
        	if (AttributeGrid->Cells[0][i] == AttributeComboBox->Items->Strings[j])
            {
                AttributeComboBox->Items->Delete(j);
                break;
            }
        }
    }

	bool NeedToScroll = false;
    // Adjust the position to fit the currently active cell:
    AttributeComboBox->Top = AttributeGrid->CellRect(ACol, ARow).Top
        + AttributeGrid->Top + AttribBox->Top + 1;

    AttributeComboBox->Left = Column * AttributeGrid->ColWidths[0] + AttributeGrid->Left + 8;

    // Look for the text that is currently in the cell in the Combo Box; if found, set the
    //     Combo Box item index to match it:
    int    Found = FindAttributeComboBoxItemPos(AttributeGrid->Cells[ACol][ARow]);

    if (AttributeComboBox->Items->Count > 0 &&
        AttributeComboBox->Items->Count >= AttributeComboBox->DropDownCount &&
		Found < AttributeComboBox->DropDownCount)
	{
		NeedToScroll = true;
	}
	else
	{
		if (Found == - 1)
            AttributeComboBox->Text = AttributeGrid->Cells[ACol][ARow];
        else
            AttributeComboBox->ItemIndex = Found;
    }

    // Show the Combo Box; drop down if appropriate:
    AttributeComboBox->Visible = true;

    // Beleive it or not, this really is necessary; if you change the style of
	//      the Combo Box, you better also reset it after the next appearance:
		if (AttributeComboBox->Style != Style)
    {
        AttributeComboBox->Style = Style;
        AttributeComboBox->Visible = false;
        ShowAttributeComboBox(Column, Style, DroppedDown);
    }

    // Want to add a blank line to attributes, so that the user can select <NOTHING>.
	else if (DropdownCodes->Items[ARow] == CELLTYPE_ATTRIBUTE)
        AttributeComboBox->Items->Add(strSpace);

    AttributeComboBox->DroppedDown = DroppedDown;
    AttributeComboBox->SetFocus();

    if (NeedToScroll)
    {
        //int OldIndex = AttributeComboBox->ItemIndex;
        AttributeComboBox->ItemIndex = 0;
        TPoint OldPoint = Mouse->CursorPos;
        for (int i = 0; i < Found; i++)
        {
            AttributeComboBox->Perform(WM_KEYDOWN, VK_DOWN, VK_DOWN);
        }

        Mouse->CursorPos = OldPoint;
    }

		AttributeComboBox->Text = AttributeComboBox->Items->Strings[Found];
    AttributeComboBox->UpdateControlState();
    AttributeComboBox->Text = AttributeGrid->Cells[ACol][ARow];
}


// Shows and poplulates, or hides, the Atttibute combo box, as appropriate:
void __fastcall TAttributeDialog::ShowOrHideAttributeComboBox()
{
	// If the attribute grid is read-only, then don't show the combo box.
	if (!OKButton->Enabled)
    	return;

	EllipsisButton->Visible = false;

    // Localize the row and column; these variables save us in case we need the LAST position:
	ARow = AttributeGrid->Row;
	ACol = AttributeGrid->Col;

	AnsiString		Attribute = AttributeGrid->Cells[0][ARow];

    if (ACol == 1 &&
       (DropdownCodes->Items[ARow] == CELLTYPE_INPUT ||
        DropdownCodes->Items[ARow] == CELLTYPE_OUTPUT))
    {
        Node	*_Node = GetCorrespondingNode(ARow, I2adlObject);

		if (_Node != NULL)
        {
            Node	*ConnectedNode = _Node->ConnectedTo;

            AttributeComboBox->Clear();

        	if (_Node->Owner->NameCode == NAME_OUTPUT &&
            	ConnectedNode != NULL &&
                ConnectedNode->DSet != VariantDataSet)
            {
            	// Connected Outputs can only be the data set of their incomming transport.
           		AttributeComboBox->Items->Add(ConnectedNode->DSet->Name);
            }
            else
            {
                DataSet		*OrigDataSet = _Node->DSet;

                for (int i = 0; i < ProjectDataSets->Count; i++)
                {
                    DataSet		*ProjectDataSet = (DataSet *) ProjectDataSets->Items[i];

                    // If the corresponding node for this entry is connected, then only populate
                    //      the list with datasets that are compatible with the connected node(s):
                    if (ConnectedNode != NULL)
                    {
                        _Node->DSet = ProjectDataSet;

                        if (!_Node->IsCompatible(ConnectedNode))
                            continue;
                    }

                    AttributeComboBox->Items->Add(ProjectDataSet->Name);
                }

                _Node->DSet = OrigDataSet;
            }

            ShowAttributeComboBox(ACol, Stdctrls::csDropDownList);
        }
    }
	else if (ACol == 0 && DropdownCodes->Items[ARow] == CELLTYPE_ATTRIBUTE)
    {
        AttributeComboBox->Clear();
        GetAttributeList();
        ShowAttributeComboBox(0, Stdctrls::csDropDown);
    }
    else if (ACol == 1 && AttributeGrid->Cells[0][ARow] == strSystem)
    {
        AttributeComboBox->Clear();

        // Preload all of the important systems.
        AttributeComboBox->Items->Add(*SystemstrQMark);  // Overrides the DefaultTargetSystem.

        // Load the subsystem names that should be included in the attribute drop down list.
        BaseSystem->LoadAttributeSystems(AttributeComboBox);

        ShowAttributeComboBox();
    }
    else if (ACol == 1 && AttributeGrid->Cells[0][ARow] == strWidget)
    {
        AttributeComboBox->Clear();

        int *WidgetOptions, nCount;
        TWidgetForm::GetWidgetOptions(I2adlObject->NameCode == NAME_INPUT, &WidgetOptions, &nCount);

		for (int i = 0; i < nCount; i++)
			AttributeComboBox->Items->Add(WidgetNames[WidgetOptions[i]]);

		ShowAttributeComboBox();
	}
	else if (ACol == 1 && (AttributeGrid->Cells[0][ARow] == strResource ||
		AttributeGrid->Cells[0][ARow] == strProvidesResource))
	{
		AttributeComboBox->Clear();

        int		FirstRow = -1;
        int		LastRow = -1;

		// Does this object have a system attribute?
        for (int i = FirstAttribute; i < AttributeGrid->RowCount; i++)
        {
            AnsiString	AttKey = AttributeGrid->Cells[0][i].Trim();
	        AnsiString	AttVal = AttributeGrid->Cells[1][i];

            if (AttKey != strSystem ||
                AttVal.Trim() == strNull)
                    continue;

            VivaSystem	*AttSystem = BaseSystem->GetSystem(AttVal);

            if (AttSystem != NULL)
            {
                AttSystem->SysResourceManager->GetResourceRowNumbers(FirstRow, LastRow);

                break;
            }
        }

        if (FirstRow > -1)
        {
            // Load just the resources for our system.
            for (int RowNumber = FirstRow; RowNumber <= LastRow; RowNumber++)
				AttributeComboBox->Items->Add(CostCalc->Cells[COL_RESOURCE_NAME][RowNumber]);

            // If this is a composite transport, then "Composite" is a valid resource.
            if (I2adlObject->NameCode == NAME_TRANSPORT)
				AttributeComboBox->Items->Add(*ResourcestrComposite);
        }
        else
        {
            // Because we don't have a system, load every known resource.
            //     Start at one to skip the NULL resource string.
            for (int i = 1; i < ResourceStringTable->Count; i++)
                AttributeComboBox->Items->Add(*(ResourceString *) ResourceStringTable->Items[i]);
        }

        ShowAttributeComboBox();
    }
    else if (ACol == 1 &&
    		(AttributeGrid->Cells[0][ARow] == strExclusive ||
             AttributeGrid->Cells[0][ARow] == strWarnOnRemoval ||
             AttributeGrid->Cells[0][ARow] == strIOBuf ||
             AttributeGrid->Cells[0][ARow] == strReverseBitFans ||
             AttributeGrid->Cells[0][ARow] == strPromoteToParentObject ||
             AttributeGrid->Cells[0][ARow] == strWidgetHex))
    {
        AttributeComboBox->Clear();

        AttributeComboBox->Items->Add(strFalse);
        AttributeComboBox->Items->Add(strTrue);

        ShowAttributeComboBox();
    }
    else if (ACol == 1 && (Attribute == strActiveXWidget || Attribute == strWidgetProperty ||
             Attribute == strWidgetEvent))
    {
        TRect Rect = AttributeGrid->CellRect(AttributeGrid->Col, AttributeGrid->Row);

        Rect.Top += 2 + AttributeGrid->Top;
        Rect.Bottom += 2 + AttributeGrid->Top;
        Rect.Right += 3 + AttributeGrid->Left;
        Rect.Left = Rect.Right - 17;

        EllipsisButton->BoundsRect = Rect;
        EllipsisButton->Visible = true;
    }
    else
    {
        AttributeComboBox->Visible = false;
    }
}


// Makes sure that the user is on a writeable cell; if not, moves it until it is.
void __fastcall TAttributeDialog::NextWriteableCell(bool Forward, int Col, int Row)
{
    if (Col == -1)
        Col = AttributeGrid->Col;

    if (Row == -1)
	   	Row = AttributeGrid->Row;

    do
    {
        if (Forward)
        {
            if (++Col >= AttributeGrid->ColCount)
            {
                Col = 0;

                if (Row < AttributeGrid->RowCount - 1)
                {
                    Row++;
                }
                else
                {
					ActiveControl = FindNextControl(AttributeGrid, false, true, false);
                    return;
                }
			}
        }
        else
        {
			if (--Col < 0)
            {
                Col = AttributeGrid->ColCount - 1;
                if (--Row < 0)
                {
					ActiveControl = FindNextControl(AttributeGrid, false, true, false);
                    return;
                }
            }
        }
    }
    while (AttributeGrid->Objects[Col][Row] == READ_ONLY_CELL);

    bSettingCell = true;
    AttributeGrid->Row = Row;
    AttributeGrid->Col = Col;
    bSettingCell = false;

    if (AttributeGrid->Visible)
        AttributeGrid->SetFocus();
}


// This routine intercepts Event Messages at the application level because TComboBox hogs
//     mouse events.
void __fastcall TAttributeDialog::EventHandlerForAttributeDialog(tagMSG &Msg, bool &Handled)
{
    // The only way that I could figure out how to tell if shift was held was to store its
    //      condition in a static variable and change it on KeyUp and KeyDown:
    static bool			ShiftKeyDown = false;
    unsigned short		Key = (unsigned short) Msg.wParam; // This is the key that was pressed.

    // Get the actual control targetted:
	TWinControl		*WinCon = FindControl(Msg.hwnd);
    int				X = Msg.lParam & 0xFFFF, Y = (Msg.lParam & 0xFFFF0000) >> 16;

    if (Msg.message == WM_LBUTTONDOWN)
    {
        if (!InACBBounds() && AttributeComboBox->Visible)
            AttributeComboBox->Visible = false;

        // The mouse button was clicked (no longer using the keyboard).
        UsingMouse = true;

		// If you click on the active cell, then no event is generated.
				//      The following code tests to see if the user did this:
        if (!AttributeComboBox->Visible)
        {
			// The X coordinate is the low word in lParam; Y is the high word:
            if (WinCon != NULL && AnsiString(WinCon->ClassName()) == "TInplaceEdit")
            {
                // Call MouseDown for the grid, since the system will only
                //      call it on the In-place Editor:
				AttributeGridMouseDown(0, mbLeft, TShiftState(), X, Y);

                // If we showed the Combo Box, then don't let the system handle
                //      this event:
                if (AttributeComboBox->Showing)
                    Handled = true;
            }
        }
    }
    else if (Msg.message == WM_KEYUP)
    {
         if (Key == VK_SHIFT) // The <Shift> key was released.
            ShiftKeyDown = false;
    }
    else if (Msg.message == WM_KEYDOWN)
    {
        // The keyboard was pressed (no longer using the mouse).
        UsingMouse = false;

		if (Key == VK_SHIFT)
		{
			// The <Shift> key was pressed.
			ShiftKeyDown = true;
		}
		else if (Key == VK_TAB)
		{
			// Fix the Tab key to exit the grid when done entering attributes:
			if (ActiveControl == AttributeGrid ||
            	ActiveControl == AttributeComboBox)
            {
                // Determine if they are done editing.
                //     If they are back-tabbing, then not done.
                if (!ShiftKeyDown)
                {
                    // If one of the cells in the row contains data, then not done.
                    if (AttributeGrid->Cells[0][ARow].TrimRight().Length() < 1 &&
                        AttributeGrid->Cells[1][ARow].TrimRight().Length() < 1)
                    {
                        // "Tab" to the next control on this form:
                        ActiveControl = FindNextControl(AttributeGrid, true, true, false);
                        AttributeComboBox->Visible = false;  // Don't show it next time.

                        // This message is completly handled (lower controls won't see it).
                        Handled = true;
                        return;
                    }
                }

                // If the Attribute Combo Box is currently active, then save the contents:
                if (ActiveControl == AttributeComboBox)
                    AttributeComboBoxClick(0); // Puts user-selected data into underlying cell

                // Since we intercepted the command, make the move, since the system won't:
                NextWriteableCell(!ShiftKeyDown);
                ShowOrHideAttributeComboBox();    // Show the Combo Box, if appropriate.
                if (!AttributeComboBox->Visible);
                    AttributeGrid->EditorMode = true;

                // This message is completely handled (lower controls won't see it).
                Handled = true;
            }
            else
            {
                // If we are tabing into the attribute grid, then make the first cell active.
				TWinControl		*NextControl = FindNextControl(ActiveControl, !ShiftKeyDown,
                	true, false);

                if (NextControl == AttributeGrid)
                {
                	ActiveControl = NextControl;

					NextWriteableCell(true, 0, 0);

					// This message is completly handled (lower controls won't see it).
                    Handled = true;
                }
            }
		}
	}
}


// When the user changes the name of the object, the original object cannot be updated.
//     Also, changes the default tree group name and icon.
void __fastcall TAttributeDialog::ObjectNameChange(TObject *Sender)
{
	// Prevent the original object's name from changing.
	optCreateObject->Checked = true;

    // Fix up EditIcon.
    LoadEditIcon();
}


// Load EditIcon to reflect the proper IconInfo based on the object name and attributes.
//		Also, make the EditTruthFileButton visible/invisible.
void __fastcall TAttributeDialog::LoadEditIcon()
{
    // Exposer/collector objects ignore the object name and don't have truth tables.
    if (EditIcon == ExposerIcon || EditIcon == CollectorIcon)
        return;

    // Use the new object name, unless there is an Icon attribute.
    AnsiString	IconName = ObjectName->Text;
    bool        HasConstTruthFile = false;

    for (int i = FirstAttribute; i < AttributeGrid->RowCount; i++)
    {
        if (AttributeGrid->Cells[0][i].Trim() == strIcon)
        {
            // If this Icon name is not empty, then use it.
            AnsiString    AttVal = AttributeGrid->Cells[1][i];

            if (AttVal.Trim() != strNull)
                IconName = AttVal;
        }
        else if (AttributeGrid->Cells[0][i].Trim() == strConstTruthFile)
        {
			HasConstTruthFile = true;
        }
	}

	EditIcon = I2adlStringTable->IconEntry(IconName, true);
    EditIcon->LoadIcon();

    // Make the EditTruthFileButton visible/invisible.
    EditTruthFileButton->Visible = HasConstTruthFile;

	// If the user selected "PromoteToParentObject" from the dropdown list, then default the
    //		value to "False".
    if (ACol == 0 &&
    	AttributeGrid->Cells[0][ARow] == strPromoteToParentObject &&
        AttributeGrid->Cells[1][ARow].Trim() == strNull)
    {
    	AttributeGrid->Cells[1][ARow] = strFalse;
    }
}


// If the user wants to update the original object, then reset the name of the object.
void __fastcall TAttributeDialog::optUpdateObjectClick(TObject *Sender)
{
    I2adl	*ExistingVF = ProjectObjects->GetObject(CurrentSheet->Behavior->CopyOf);

    if (ExistingVF == NULL)
		ObjectName->Text = CurrentSheet->NameInfo->Name;
    else
		ObjectName->Text = ExistingVF->NameInfo->Name;

    optUpdateObject->Checked = true;
}


// The user clicked on the graphic.  Allow it to be edited.
void __fastcall TAttributeDialog::EditGlyphClick(TObject *Sender)
{
	// If the attribute grid is read-only, then don't allow the icon to be edited.
    if (!OKButton->Enabled)
    	return;

    // Can the icon file be overwritten?
    if (CheckForWrite(IconFileName, true))
    {
		bool	FileSaved = false;

        if (EditIcon->Bitmap == NULL)
        {
            // Create an empty icon with transparent background.
            TBitMap		*tempbmp = new TBitMap;

            tempbmp->Width = 50;
            tempbmp->Height = 50;

            tempbmp->Canvas->Brush->Color = clFuchsia;
            tempbmp->Canvas->FillRect(TRect(0, 0, tempbmp->Width, tempbmp->Height));

            try
            {
                tempbmp->SaveToFile(IconFileName);

                FileSaved = true;
            }
            catch (...)
            {
                ErrorTrap(4051, IconFileName);
            }                                                                                                                                                                                     

            delete tempbmp;
        }
        else
        {
            // Create a backup copy of the original icon.
            if (CheckForWrite(BackupFileName, true))
            {
                // Use DisplayImage's bitmap because Icon may have been widened.
                try
                {
                    DisplayImage->Picture->Bitmap->SaveToFile(BackupFileName);

                    FileSaved = true;
                }
                catch (...)
                {
                    ErrorTrap(4051, BackupFileName);
                }
            }
        }

        // Open the file in the ".bmp" file editor.
        if (FileSaved)
            if (!ShellExecute(Handle, NULL, IconFileName.c_str(), "", "", SW_SHOWNORMAL))
                ErrorTrap(4007, IconFileName);

        // Because MS Paint is not modal, calling EditIcon->LoadIcon() would not change anything.
    }
}


// This method draws "read only" cells with a gray background to indicate that they can't be
//     selected/edited.
void __fastcall TAttributeDialog::AttributeGridDrawCell(TObject *Sender, int ACol, int ARow,
        TRect &Rect, TGridDrawState State)
{
    if (AttributeGrid->Objects[ACol][ARow] == READ_ONLY_CELL)
    {
        AttributeGrid->Canvas->Brush->Color = clBtnFace;
        AttributeGrid->Canvas->FillRect(Rect);
        AttributeGrid->Canvas->TextRect(Rect, Rect.Left + 2, Rect.Top + 2, AttributeGrid->
            Cells[ACol][ARow]);  // Print text in the cell.
    }
}


// Puts us on the first writeable cell, makes sure that it is hightlighted if appropriate:
void __fastcall TAttributeDialog::FormActivate(TObject *Sender)
{
    // The following ensures the first tab goes to the Data Set field (Bug #1138).
    ARow = 0;
    ACol = 0;

    bool	GoBack = ActiveControl == ObjectName;

    // The active cell of the attribute grid should be the upper-leftmost:
	AttributeGrid->SetFocus();
    NextWriteableCell(true, 0, 0);
    AttributeComboBox->Visible = false;

    //if the control, 'ObjectName' had the focus before we started, then give it back:
    if (GoBack)
        ObjectName->SetFocus();

    // This is the only way that I know how to select the text in the current cell: iterate thru
	//     all controls in AttributeGrid, and when you hit a TInplaceEdit, call SelectAll on it.
    for (int i = 0; i < AttributeGrid->ControlCount; i++)
    {
        TControl	*Con = AttributeGrid->Controls[i];

        if (AnsiString(Con->ClassName()) == "TInplaceEdit")
            ((TInplaceEdit *) Con)->SelectAll();
    }

    Application->OnMessage = EventHandlerForAttributeDialog;
}


void __fastcall TAttributeDialog::AttributeGridKeyDown(TObject *Sender,
    WORD &Key, TShiftState Shift)
{
	if (Key != VK_RETURN && Key != VK_ESCAPE)
    {
		// Could be a cell that has use of a combo box; call the function:
		ShowOrHideAttributeComboBox();
    }
	else if (Key == VK_ESCAPE)
	{
		CancelButton->Click();
	}
	else if (Key == VK_RETURN)
	{
		if (ActiveControl != DocumentationMemo)
			if (OKButton->Enabled)
        		OKButton->Click();
	}
}


void __fastcall TAttributeDialog::AttributeGridKeyUp(TObject *Sender, WORD &Key,
		TShiftState Shift)
{
	// Could be a cell that has use of a combo box; call the function:
	ShowOrHideAttributeComboBox();

	// Fix up EditIcon.
	LoadEditIcon();
}


// Same procedure as int KeyUp, except for that we need not worry about back-tabbing:
void __fastcall TAttributeDialog::AttributeGridMouseDown(TObject *Sender
		, TMouseButton Button, TShiftState Shift, int X, int Y)
{
    ShowOrHideAttributeComboBox();

    // If we did show the combo box, then we are done;
    if (AttributeComboBox->Showing)
		return;

    // Otherwise, we need to put the cursor where the user clicked in the in-place editor
    //     (instead of selecting the whole cell).
    TWinControl		*WinCon = FindVCLWindow(Mouse->CursorPos); // Control at cursor

    if (WinCon != NULL && AnsiString(WinCon->ClassName()) == "TInplaceEdit")
    {
        int 	acol;
        int     arow;

        AttributeGrid->MouseToCell(X, Y, acol, arow); // Will report [0,0] if in edit mode.

        if (acol || arow)
        {
	        TInplaceEdit 	*IE = (TInplaceEdit *) WinCon; // cast to TInplaceEdit
	        TPoint 			p = IE->ScreenToClient(AttributeGrid->ClientToScreen(TPoint(X, Y)));

            IE->SelStart = 0;

	        // Now do two left button downs, and a left button up:
	        IE->Perform(WM_LBUTTONDOWN, 0, p.x + (p.y << 16));
	        IE->Perform(WM_LBUTTONUP, 0, p.x + (p.y << 16));
    	    IE->Perform(WM_LBUTTONDOWN, 0, p.x + (p.y << 16));
        }
    }
}


// We need this event to handle empty combo boxes, since the system pukes on them:
void __fastcall TAttributeDialog::AttributeComboBoxKeyUp(TObject *Sender, WORD &Key,
    TShiftState Shift)
{
	// We just pressed a button, which may not have registered with the form first, so flag it:
    AttributeGrid->Cells[ACol][ARow] = AttributeComboBox->Text;

    // If the user hit <Back Space> or <Delete>, and the cell is empty or just spaces, then
	//     we need to flag it:
	if (Key == VK_BACK || Key == VK_DELETE)
	{
		if (AttributeComboBox->Text.TrimRight().Length() < 1)
		{
			IsEmpty = true;

			if (AttributeComboBox->Visible)
                AttributeComboBox->Visible = false;
            else
                AttributeComboBoxClick(Sender);

            AttributeGrid->SetFocus();
        }
    }
    else
    {
        IsEmpty = false; // May have been previously flagged; shouldn't be now.
    }
}


// Here, we update the underlying cell's contents to reflect those of the Combo Box.
void __fastcall TAttributeDialog::AttributeComboBoxClick(TObject *Sender)
{
    // If we aren't in bounds, then this call should never have been generated; abort:
    if (!InACBBounds())
        return;

    // If this call wasn't generated from a key press, then move to the next cell:
	if (IsEmpty)
    {
        AttributeComboBox->Visible = false;
        AttributeGrid->Cells[ACol][ARow] = strNull;
    }
    else if (ACol == AttributeGrid->Col && ARow == AttributeGrid->Row &&
             AttributeComboBox->ItemIndex != -1) // -1 means that no item is selected
    {
        // If we are on the right column and row, then put the selected text from
        //      the combo box into the corresponding cell in the grid:
        AttributeGrid->Cells[ACol][ARow] = AttributeComboBox->Text;
    }

    // If it was an actual click (not keystroke):
	if (UsingMouse)
        AttributeComboBox->Visible = false;
}


// Handles check for dupes, and syncing the contents of the cell with those
//      of the combo box:
void __fastcall TAttributeDialog::AttributeComboBoxExit(TObject *Sender)
{
    // If we were on the combo box setting for Attributes, and a the user actually entered
    //      something, then add it to the AttributesList, and sync the combo box with the cell:
    if (DropdownCodes->Items[ARow] == CELLTYPE_ATTRIBUTE && ACol == 0)
        if (AttributeGrid->Cells[ACol][ARow].TrimRight().Length() > 0)
            AttributeList->Add(AttributeGrid->Cells[ACol][ARow]);

    // If the Combo Box text was supposed to be empty then we have to undo the work of Borland's
    //      stupid Combo Box control (in case they SELECTED a blank, rather than erasing):
    if (IsEmpty)
    {
		AttributeGrid->Cells[ACol][ARow] = strNull;
		AttributeComboBox->ItemIndex = -1;
        IsEmpty = false;
    }

	// Fix up EditIcon.
    LoadEditIcon();
}


// Deals with 2 cases: mouse being in the way, and the Combo Box style changing:
void __fastcall TAttributeDialog::AttributeComboBoxKeyDown(TObject *Sender, WORD &Key,
    TShiftState Shift)
{
    // If the Style property of this object was just changed to csDropDown, then we need to
    //      ignore this event, since the control called an extra event, for whatever reason:
    if (InACBBounds())
        Mouse->CursorPos = TPoint(AttributeDialog->Left + AttributeComboBox->BoundsRect.Right +
				10, Mouse->CursorPos.y);
}


// Since the <Enter> key is intercepted at form level, AttributeComboBoxExit is not
//      naturally called if the said key is hit; we have to do it manually:
void __fastcall TAttributeDialog::OKButtonClick(TObject *Sender)
{
	if (ActiveControl == AttributeComboBox)
    {
        AttributeComboBoxClick(0);
        AttributeComboBox->OnExit(0);
    }
}


void __fastcall TAttributeDialog::FormDeactivate(TObject *Sender)
{
    // Because MS Paint is not modal, update any icon changes now.
    EditIcon->LoadIcon();

    // Terminate the low-level message interception:
    Application->OnMessage = NULL;
}

// Swaps the current image for the object being edited with its backup image.
//     Four different cases regarding the backup file:
//     1.  Current image and backup image both exist.
//     2.  Current image does NOT exist, but backup image does.
//     3.  Current image exists, but backup image does NOT.
//     4.  Neither file exists.
void __fastcall TAttributeDialog::btnRevertClick(TObject *Sender)
{
	if (FileExists(BackupFileName))
    {
        // Case 1: Swap .bmp (current) and .~mp (backup) files.
        if (FileExists(IconFileName))
        {
            RenameFile(BackupFileName, BackupFileName + ".tmp");
            RenameFile(IconFileName, BackupFileName);
            RenameFile(BackupFileName + ".tmp", IconFileName);
        }

        // Case 2: Rename the current file as the backup file.
        else
        {
            RenameFile(BackupFileName, IconFileName);
        }
    }

    // Case 3: Rename the backup file as the current file.
    else if (FileExists(IconFileName))
    {
        RenameFile(IconFileName, BackupFileName);
    }

    //Case 4: Nothing to deal with here:
    else 
    {
        return;
	}

    // Now that we got the renaming correct, reload the icon for this object:
    EditIcon->LoadIcon();
}


// If the object being edited has an icon, then it is discarded (becomes the backup image file).
void __fastcall TAttributeDialog::btnDiscardClick(TObject *Sender)
{
    // If the object doesn't have an icon, then don't touch the backup icon file.
	if (!FileExists(IconFileName))
        return;

    // If there is a backup icon file, then delete it.
    if (FileExists(BackupFileName))
    {
    	// Can the backup file be overwritten?
        if (!CheckForWrite(BackupFileName, true))
        	return;

        DeleteFile(BackupFileName);
    }

    // Rename the icon to the backup and clear the icon bitmaps.
    RenameFile(IconFileName, BackupFileName);

    EditIcon->LoadIcon();
}


void TAttributeDialog::GetAttributeList()
{
    delete AttributeList;
    AttributeList = new TStringList;
    AttributeList->Sorted = true;
    AttributeList->Duplicates = dupIgnore;

    I2adl    *Object = I2adlObject;

    AttributeList->Add(strIcon);
	AttributeList->Add("Documentation");

    if (Object->NameCode == NAME_INPUT)
    {
        AttributeList->Add(strConstant);
        AttributeList->Add(strDefault);
		AttributeList->Add(strPromoteToParentObject);
    }
    else if (Object->NameCode == NAME_OUTPUT)
    {
        AttributeList->Add(strTrap);
		AttributeList->Add(strPromoteToParentObject);
    }
    else if (Object->NameCode == NAME_SPAWN)
    {
        AttributeList->Add(strFileName);
		AttributeList->Add(strExclusive);
    }
    else if (Object->NameInfo->Name == strJoinGlobals)
    {
        AttributeList->Add(strJoinGlobalsDeletePri);
    }

    int		SysType = 0;

    for (int i = FirstAttribute; i < AttributeGrid->RowCount; i++)
    {
        AnsiString	AttKey = AttributeGrid->Cells[0][i].Trim();
        AnsiString	AttVal = AttributeGrid->Cells[1][i];

        if (AttKey == strWidget ||
        	AttKey == strActiveXWidget)
        {
            // Widget property strings should be appended if they have specified a widget:
            AnsiString	WidgetStrings = strWidgetTop + '\n' + strWidgetLeft + '\n' +
                strWidgetWidth + '\n' + strWidgetHeight + '\n' + strWidgetMin + '\n' +
                strWidgetMax + '\n' + strWidgetHex + '\n' + strWidgetTabSheet;

            AttributeList->Append(WidgetStrings);

            // Output widgets can record their values in a text file.
            if (Object->NameCode == NAME_OUTPUT)
                AttributeList->Add(strFileName);
        }
       	else if (AttKey == strResource)
        {
	        // WarnOnRemoval is valid on global label Inputs.
   	        if (Object->IsGlobalLabel(&AttVal) &&
		        Object->NameCode == NAME_INPUT)
                	AttributeList->Add(strWarnOnRemoval);
        }
        else if (AttKey == strSystem)
        {
	        // System Attribute:
            if (AttVal.Trim() != strNull)
            {
                VivaSystem	*XSystem = BaseSystem->GetSystem(AttVal);

                if (XSystem != NULL)
                    SysType = XSystem->Type;
            }
        }
    }

    if (Object->IsPrimitive)
    {
    	if (Object->Inputs->Count > 0 &&
        	Object->Outputs->Count > 0)
        {
	       	AttributeList->Add(strConstTruthFile);
	       	AttributeList->Add(strConstTruthTable);
        }

    	// If the primitive object has a larger-than-bit data set, then bits may be reversed.
        //		Pass #1 - Inputs
    	NodeList	*IONodes = Object->Inputs;

        for (int IO = 0; IO < 2; IO++)
        {
        	for (int i = 0; i < IONodes->Count; i++)
            {
            	Node	*IONode = IONodes->Items[i];

                if (IONode->DSet->BitLength > 1)
                {
			        AttributeList->Add(strReverseBitFans);
                    break;
                }
            }

        	// Pass #2 - Outputs;
            IONodes = Object->Outputs;
        }
    }
    else
    {
        AttributeList->Add(strChildAttribute);
        AttributeList->Add(strObjectRef);
    }

    if (SysType == 0 ||
    	SysType == X86UI_TYPE)
    {
        AttributeList->Add(strWidget);
        AttributeList->Add(strActiveXWidget);
    }

	if (SysType == 0)
	{
	    // Any non-system object can become a system object.
		AttributeList->Add(strSystem);
	}
	else if (Object->IsPrimitive ||
			 IsCandidateForPrimitive(Object))
    {
        // Primitive system objects can have these attributes:
        AttributeList->Add(strResource);
        AttributeList->Add(strResourceQty);

        // Is the object in a FPGA system?
        if (SysType == FPGA_TYPE)
        {
            AttributeList->Add(strEdifFile);
            AttributeList->Add(strInst);
            AttributeList->Add(strInstanceName);
            AttributeList->Add(strIOBuf);
            AttributeList->Add(strPadLoc);
            AttributeList->Add(strNet);
            AttributeList->Add(strLibRef);
            AttributeList->Add(strTIG);
            AttributeList->Add(strTIMESPEC);
            AttributeList->Add(strTNM);
        }
    }
    else
    {
        // Composite system objectes can have these attributes:
        if (Object->NameCode == NAME_TRANSPORT)
            AttributeList->Add(strResource);
	}

	if (Object->NameCode == NAME_INPUT ||
    	Object->NameCode == NAME_OUTPUT)
	{
		if (SysType == 0)
		{
			AttributeList->Add(strResource);
			AttributeList->Add(strResourceQty);
		}

		if (SysType == 0 || SysType==X86UI_TYPE)
			AttributeList->Add(strWidget);
	}
	else if (SysType == X86MAIN_TYPE)
	{
		for (int i = 0; i < Object->Inputs->Count; i++)
		{
			 Node    *Node = Object->Inputs->Items[i];

			 if (Node->DSet == VariantDataSet)
			 {
				 AttributeList->Add("MaxBits");
				 break;
			 }
		}
	}

	AttributeComboBox->Items->Text = AttributeList->Text;
}


Node *TAttributeDialog::GetCorrespondingNode(int Row, I2adl *I2adlObject)
{
	Node *ReturnNode = NULL;

	int Index = Row - FirstInput;

	if (Row >= FirstInput && Index < I2adlObject->Inputs->Count)
	{
		ReturnNode = I2adlObject->Inputs->Items[Index];
	}
	else
	{
		Index = Row - FirstOutput;
		if (Row >= FirstOutput && Index < I2adlObject->Outputs->Count)
			ReturnNode = I2adlObject->Outputs->Items[Index];
	}

	return ReturnNode;
}


// Returns true if the user confirms that changing nodes is OK.
bool TAttributeDialog::IsChangingNodesOK(int &NodesChanged, bool &CancelNodeChanges)
{
    // Has the user already approved node changes?
    if (NodesChanged == 0)
    {
        NodesChanged = 1;

        if (ErrorTrap(53) != mrYes)
        {
             CancelNodeChanges = true;
             
             return false;
        }
    }

    return true;
}


void __fastcall TAttributeDialog::AttributeGridSelectCell(TObject *Sender,
	  int ACol, int ARow, bool &CanSelect)
{
	CanSelect = bSettingCell || AttributeGrid->Objects[ACol][ARow] != READ_ONLY_CELL;
}


void TAttributeDialog::SetAttribute(AnsiString Name, AnsiString Value)
{
    int i = FirstAttribute;

    while (AttributeGrid->Cells[0][i].Trim() != strNull)
    {
        if (AttributeGrid->Cells[0][i].Trim() == Name)
        {
            AttributeGrid->Cells[1][i] = Value;
            return;
        }

        i++;
    }

    if (AttributeGrid->RowCount <= i)
        AttributeGrid->RowCount = i + 1;

    AttributeGrid->Cells[0][i] = Name;
    AttributeGrid->Cells[1][i] = Value;
}


AnsiString TAttributeDialog::GetAttributeValue(AnsiString Name)
{
    int i = FirstAttribute;

    while (AttributeGrid->Cells[0][i].Trim() != strNull && i < AttributeGrid->RowCount)
    {
        if (AttributeGrid->Cells[0][i].Trim() == Name)
            return AttributeGrid->Cells[1][i];

        i++;
    }

    return strNull;
}


void __fastcall TAttributeDialog::IconRefreshClick(TObject *Sender)
{
	EditIcon->LoadIcon();
}


void __fastcall TAttributeDialog::EllipsisButtonClick(TObject *Sender)
{
    ActiveXWidgetSelector->ObjectBeingEdited = I2adlObject;
    ActiveXWidgetSelector->ShowModal();
}


void __fastcall TAttributeDialog::AttributeGridKeyPress(TObject *Sender, char &Key)
{
    if (EllipsisButton->Visible)
    {
        if (Key == VK_RETURN)
            EllipsisButton->Click();

        Key = 0;
    }
}


void __fastcall TAttributeDialog::FormClose(TObject *Sender, TCloseAction &Action)
{
    if (ModalResult == mrOk)
    {
    	// Prevent more than one of each control node.
        for (int i = 0; i < ceLast; i++)
        {
        	int		ControlNodeCount = 0;

            for (int j = FirstInput; j < FirstAttribute; j++)
            {
            	if (*ControlNodeNames[i] == AttributeGrid->Cells[0][j])
                {
                    if (++ControlNodeCount > 1)
                    {
                    	// This control node name is used more than once.
                    	ErrorTrap(52, *ControlNodeNames[i]);

	                    Action = caNone;
                		break;
                    }
                }
            }
        }

        // Validate the object attributes.
        for (int i = FirstAttribute; i < AttributeGrid->RowCount; i++)
        {
        	AnsiString	AttributeName = AttributeGrid->Cells[0][i];
            AnsiString	AttributeValue = AttributeGrid->Cells[1][i].Trim();

            // If the entire AttributeValue is blank, then the Attribute Editor will
            //		delete it.  Don't check for problems.
            if (AttributeValue == strNull)
            {
            }

	        // Force Icon attributes to have valid Windows file names.
            else if (AttributeName == strIcon)
            {
                if (!IsValidFileName(AttributeValue, true, false))
                {
                	// The file name is invalid.
                    ErrorTrap(156, AttributeGrid->Cells[1][i]);
                    Action = caNone;
                }
            }

	        // Prevent invalid ConstTruthFile attributes.
            else if (AttributeName == strConstTruthFile)
            {
                // If the file does not exist, then allow the user to browse for it.
                //		Strip off the comments by finding the left-most comment character.
                bool		DoesFileExist = true;
                int			CommentPos = AttributeValue.Pos("#");
                AnsiString  FileName = (CommentPos <= 0)
                                     ? AttributeValue.Trim()
                                     : AttributeValue.SubString(1, CommentPos - 1).Trim();

                // Let ParseConstTruthFileAtt() handle missing and "<None>" file names.
                if (FileName != strNull &&
                    FileName.UpperCase() != str_NONE_)
                {
                    FileName = FullPathName(FileName);

                    if (!FileExists(FileName))
                    {
                        if (BrowseFile(FileName))
                        {
                            // Save the file name the user provided.
                            AttributeValue = (CommentPos <= 0)
                            	? EncodePath(FileName)
                                : EncodePath(FileName) + " " + AttributeValue.SubString(
                                	CommentPos, AttributeValue.Length() - CommentPos + 1);

                            AttributeGrid->Cells[1][i] = AttributeValue;
                        }
                        else
                        {
                            DoesFileExist = false;
                        }
                    }
                }

                // Did the user provide a file.
                if (DoesFileExist)
                {
                    FoundObjectEnum		ObjectStatus = foNotFound;

                    // Put errors in the message window because we don't want to force them to
                    //		fix problems in the file before allowing the attribute to be added.
                    if (I2adlObject->ParseConstTruthFileAtt(AttributeValue, ObjectStatus))
                    {
                        // Attribute is valid, but does it contain a truth table for this
                        //		object or "<None>"?
                        if (ObjectStatus == foNotFound &&
                            AttributeValue.UpperCase().Pos(str_NONE_) != 1)
                                ErrorTrap(303, AttributeValue).AutoModal = true;
                    }
                    else
                    {
                        // Modal message to tell the user to review the error mesages in the
                        //		message window.
                        ErrorTrap(305, AttributeValue);
                    }
                }
            }

	        // Prevent invalid ConstTruthTable attributes.
            else if (AttributeName == strConstTruthTable)
            {
            	// Put any error message in a modal window because problems need to be fixed
                //		now.
                if (!I2adlObject->ParseConstTruthTableAtt(AttributeValue, true, false))
                    Action = caNone;
            }
        }

        if (EditCase != EditRef)
        {
        	// Does this object have a system attribute?
            for (int i = FirstAttribute; i < AttributeGrid->RowCount; i++)
            {
                if (AttributeGrid->Cells[0][i] == strSystem &&
                	AttributeGrid->Cells[1][i].Trim() != strNull)
                {
#if VIVA_SD
					// Primitive system objects must have a Resource attribute.
                    if (I2adlObject->IsPrimitive)
                    {
                        bool	HasResourceAttribute = false;

                        for (int j = FirstAttribute; j < AttributeGrid->RowCount; j++)
                        {
                            if (AttributeGrid->Cells[0][j] == strResource &&
                            	AttributeGrid->Cells[1][j].Trim() != strNull)
                            {
                                HasResourceAttribute = true;
                                break;
                            }
                        }

                        if (!HasResourceAttribute)
                        {
                            ErrorTrap(15);
                            Action = caNone;
                        }
                    }
#else
					// Can't allow creation of new system objects, or assignment of existing
			        //		objects to a system
                    ErrorTrap(4092, AttributeGrid->Cells[1][i]);

                    AttributeGrid->Cells[0][i] = strNull;
                    AttributeGrid->Cells[1][i] = strNull;
#endif
                }
            }
        }

        // Prevent blank object name.
        if (ObjectName->Text.Trim() == strNull)
        {
            ErrorTrap(74);
            Action = caNone;
        }
        else
        {
	        I2adlObject->SetName(ObjectName->Text);
        }

        // Confirm the object name to the user if it is still the default.
        if (ObjectName->Text.SubString(1, 6) == "Sheet_" && EditCase < EditDef)
            if (ErrorTrap(75) != mrYes)
                Action = caNone;

		if (I2adlObject != NULL)
        {
            if (I2adlObject->NameCode == NAME_INPUT && I2adlObject->Outputs->Count < 1)
            {
            	ErrorTrap(4072);
                Action = caNone;
            }
            else if (I2adlObject->NameCode == NAME_OUTPUT && I2adlObject->Inputs->Count < 1)
            {
            	ErrorTrap(4073);
                Action = caNone;
            }
        }
    }
}


// Display the contents of the ConstTruthFile and allow the user to edit/save it.
void __fastcall TAttributeDialog::EditTruthFileButtonClick(TObject *Sender)
{
    // Find the ConstTruthFile attribute.
    for (int i = FirstAttribute; i < AttributeGrid->RowCount; i++)
    {
        AnsiString	AttributeName = AttributeGrid->Cells[0][i];
        AnsiString	AttributeValue = AttributeGrid->Cells[1][i];

        if (AttributeName != strConstTruthFile)
        	continue;

        // Does the attribute contain a valid file name?
        //		Strip off the comments by finding the left-most comment character.
        int			CommentPos = AttributeValue.Pos("#");
        AnsiString  FileName = (CommentPos <= 0)
                             ? AttributeValue.Trim()
                             : AttributeValue.SubString(1, CommentPos - 1).Trim();

        if (FileName != strNull &&
            FileName.UpperCase() != str_NONE_)
        {
            FileName = FullPathName(FileName);

            // Does the file name exist?
            if (FileExists(FileName))
            {
                // We have a valid ConstTruthFile.  If this object is in the file, then
                //		display the file at that point.
                FoundObjectEnum		ObjectStatus = foNotFound;

                I2adlObject->ParseConstTruthFileAtt(AttributeValue, ObjectStatus, true);

                // Otherwise, display it at the top.
                if (ObjectStatus == foNotFound)
                {
                    TStringList		*FileContent = new TStringList;

                    try
                    {
                        FileContent->LoadFromFile(FileName);
                    }
                    catch(...)
                    {
                        // Cannot open/read the file.
                        ErrorTrap(302, VStringList(AttributeValue, FileName)).AutoModal = true;

                        delete FileContent;

                        return;
                    }

					TEditTextFileForm	*EditFileForm = new TEditTextFileForm(MainForm);

                    EditFileForm->EditTextFile(FileName, FileContent, -1);

                    delete EditFileForm;
                    delete FileContent;
                }

                return;
            }
        }

        // Attribute does not contain a valid file name.
        ErrorTrap(55);

        return;
	}

    // There is no ConstTruthFile attribute.
    ErrorTrap(46);
}

