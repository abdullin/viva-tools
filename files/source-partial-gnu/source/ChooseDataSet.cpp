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
#include "ChooseDataSet.h"
#include "DataSet.h"
#include "ErrorTrap.h"
#include "I2adl.h"
#include "IniFile.h"
#include "List.h"
#include "Node.h"

#pragma package(smart_init)
#pragma resource "*.dfm"

TChooseDataSetForm	*ChooseDataSetForm;


// Main constructor
__fastcall TChooseDataSetForm::TChooseDataSetForm(TComponent *Owner)
	: TForm(Owner)
{
	AutoCompleteDSName = true;
    LastEditBoxText1 = strNull;
    LastEditBoxText2 = strNull;
}


// Returns the number of data sets displayed in the list box.
int TChooseDataSetForm::LinesInListBox()
{
    return DSListBox->Height / DSListBox->ItemHeight;
}


// Returns the name of the data set currently selected in the list box.
AnsiString TChooseDataSetForm::GetListBoxDataSet()
{
    return (DSListBox->ItemIndex < 0)
    	  ? strNull
          : DSListBox->Items->Strings[DSListBox->ItemIndex];
}


// Returns true if Text[Location] is a "Composite Data Set Structure" delimiter or a space(s)
//		next to a delimiter (based on the Direction we are looking up/down the string).
//		IsConstant must be set to false by the calling routine.  It will be set to true if
//		the delimiter indicates the text is a constant (preceeded by "=").
//		Direction should be 1 for looking up and -1 for looking down.
bool TChooseDataSetForm::IsCSDelimiter(bool &IsConstant, AnsiString Text, int Location,
	int Direction)
{
	char	Character = Text[Location];

    // If the character is a space, then determine if it is next to a delimiter.
    if (Character == ' ')
    {
    	if (Direction >= 0)
        {
        	// We are looking up the string.
            if (Location < Text.Length())
            	return IsCSDelimiter(IsConstant, Text, Location + 1, Direction);
            else
            	return true;  // End of string.
        }
        else
        {
        	// We are looking down the string.
            if (Location > 1)
            	return IsCSDelimiter(IsConstant, Text, Location - 1, Direction);
            else
            	return true;  // Start of string.
        }
    }

    // If the text is preceeded by "=", then it is a constant (not a data set).
    if (Character == '=')
    {
    	if (Direction < 0)
        	IsConstant = true;

    	return true;
    }

    return (Character == '(' ||
    		Character == ')' ||
    		Character == ',');
}


// Returns the text in the data set edit box that is under the cursor.
//		This is all of the selected text plus preceeding/trailing characters until a
//		delimitor or start-of-field/end-of-field is reached.
//		IsConstant must be set to false by the calling routine.  It will be set to true if
//		the text under the cursor is a constant (preceeded by "=").
//		If a NewText is provided (not strNull), then it will replace the text under the cursor.
//		If SelectNewSuffix is true, then just the part of NewText that is new will be selected.
AnsiString TChooseDataSetForm::TextUnderCursor(bool &IsConstant, AnsiString NewText, bool
	SelectNewSuffix)
{
	// Find the start/end of the text under the cursor by looking for a preceeding/trailing
    //		delimiter character or start-of-field/end-of-field.
    //		Adding on "SelLength" causes all of the selected text to be included.
    //		Note:  SelStart is 0 based, while Text (AnsiString) is 1 based.
    int		StartOfText = DSEditBox->SelStart;
    int		EndOfText = DSEditBox->SelStart + DSEditBox->SelLength;

    while (StartOfText > 0)
    {
    	if (IsCSDelimiter(IsConstant, DSEditBox->Text, StartOfText, -1))
        	break;

        StartOfText--;
    }

    while (EndOfText < DSEditBox->Text.Length())
    {
    	if (IsCSDelimiter(IsConstant, DSEditBox->Text, EndOfText + 1, 1))
        	break;

        EndOfText++;
    }

    AnsiString	OldText = DSEditBox->Text.SubString(StartOfText + 1, EndOfText -
    	StartOfText);

    // If the text under the cursor is not a data set, then disable the data set list box.
    DSListBox->Enabled = !IsConstant;

    // If a new text was provided, then replace the old text under the cursor.
    if (NewText != strNull)
    {
        DSEditBox->Text =
            DSEditBox->Text.SubString(1, StartOfText) + NewText +
            DSEditBox->Text.SubString(EndOfText + 1, DSEditBox->Text.
            	Length());

		if (SelectNewSuffix)
        {
            // Select just the new suffix characters added in.
            DSEditBox->SelStart = EndOfText;

            int		SuffixLength = NewText.Length() - OldText.Length();

            DSEditBox->SelLength = (SuffixLength < 0) ? 0
            										  : SuffixLength;
        }
        else
        {
            // Select the entire NewText.
            DSEditBox->SelStart = StartOfText;
            DSEditBox->SelLength = NewText.Length();
        }

        // Make sure the correct data set is selected in the list box.
        LoadListBox(NewText);
    }

    return OldText;
}


// Finds the data set in the ListBoxIndex location in the list box and loads it into the
//		edit box, replacing the text under the cursor.
//		The data set is selected in the edit box and selected/centered in the list box.
void TChooseDataSetForm::LoadEditBox(int ListBoxIndex)
{
	// Only load the edit box if the cursor is over a data set.
    bool	IsConstant = false;

	TextUnderCursor(IsConstant);

    if (!IsConstant)
    {
        // Make sure the index is in bounds.
        if (ListBoxIndex < 0)
            ListBoxIndex = 0;

        if (ListBoxIndex >= DSListBox->Items->Count)
            ListBoxIndex = DSListBox->Items->Count - 1;

        // Select and center the data set in the list box.
        LoadListBox(DSListBox->Items->Strings[ListBoxIndex]);

        // Replace the text under the cursor with the new data set.
        TextUnderCursor(IsConstant, GetListBoxDataSet());
    }
}


// Finds the closest matching data set in the list box for the DataSetPrefix passed in.
//		Select, center and return it.
AnsiString TChooseDataSetForm::LoadListBox(AnsiString DataSetPrefix)
{
	// The following fixes problems with the cursor being over leading/trailing spaces.
	DataSetPrefix = DataSetPrefix.Trim();

    // If no prefix was passed in, then clear the list box.
    if (DataSetPrefix == strNull)
    {
    	DSListBox->ItemIndex = -1;
        DSListBox->TopIndex = 0;

        return strNull;
    }

	int			ListBoxIndex = 0;
	int			LargestPossibleIndex = DSListBox->Items->Count - 1;
    AnsiString	DataSetPrefixUC = DataSetPrefix.UpperCase();

    // Check the boundary conditons:
    if (DataSetPrefixUC <= DSListBox->Items->Strings[0].UpperCase())
    {
    	ListBoxIndex = 0;
    }
    else if (DataSetPrefixUC >= DSListBox->Items->Strings[LargestPossibleIndex].
    	UpperCase())
    {
    	ListBoxIndex = LargestPossibleIndex;
    }
    else
    {
        for (ListBoxIndex = 0; ListBoxIndex < LargestPossibleIndex; ListBoxIndex++)
        {
        	// Read each data set until we find one too large.
		    AnsiString	ListUC1 = DSListBox->Items->Strings[ListBoxIndex + 1].
            	UpperCase();

            if (DataSetPrefixUC > ListUC1)
                continue;

		    AnsiString	ListUC0 = DSListBox->Items->Strings[ListBoxIndex].UpperCase();

            // Choose the entry with the most matching characters.
            for (int i = 1; i <= DataSetPrefixUC.Length(); i++)
            {
                AnsiString	EditUCPrefix = DataSetPrefixUC.SubString(1, i);

                if (ListUC1.Pos(EditUCPrefix) != 1)
                    break;

                if (ListUC0.Pos(EditUCPrefix) != 1)
                {
                    ListBoxIndex++;

                    break;
                }
            }

            break;
        }
    }

    // Select, center and return the closest data set.
    int		NewTopIndex = ListBoxIndex - LinesInListBox() / 2;

    DSListBox->ItemIndex = ListBoxIndex;
    DSListBox->TopIndex = (NewTopIndex < 0) ? 0
    										: NewTopIndex;

	return GetListBoxDataSet();
}


// Used to resolve the data sets on dynamic inputs on the top-level sheet at compile time.
//		Returns the "Composite Data Set Structure" entered by the user to replace the
//		dynamic data set (or strNull if the user canceled the compile).
//		InputObject is required to display the node name and the data set name.
//		RemainingInputs is the number of input horns (including this one) for the current
//		dynamic data set that need to be assigned a data set.
AnsiString TChooseDataSetForm::ChooseDataSet(I2adl *InputObject, int RemainingInputs)
{
	DynamicInput = InputObject;

    // Put all of the data sets (except Variant) into the sorted data set list box.
    DSListBox->Clear();

    for (int i = 0; i < ProjectDataSets->Count; i++)
    {
        DataSet		*ProjectDataSet = (DataSet *) ProjectDataSets->Items[i];

        if (ProjectDataSet != VariantDataSet)
            DSListBox->Items->AddObject(ProjectDataSet->Name, (TObject *)
            	ProjectDataSet);
    }

    // Get the last selected data set for this dynamic data set out of the Viva.ini file.
    AnsiString	DynamicDSName = InputObject->Outputs->Items[0]->DSet->Name;
    AnsiString	DefaultDataSet = IniFile->ReadString(strChooseDataSets, DynamicDSName,
    	strNull);

    // Make the Edit box the active control and load/select the DefaultDataSet into it.
    ActiveControl = DSEditBox;
   	DSEditBox->Text = DefaultDataSet;
    DSEditBox->SelStart = 0;
    DSEditBox->SelLength = DefaultDataSet.Length();

	// Select and center the closest matching data set in the list box.
    LoadListBox(DefaultDataSet);

    // Load the other controls on the form.
    ApplyDSToRest->Checked = IniFile->ReadBool(strChooseDataSets, strApplyDSToRest, true);
    DSName->Text = strSpace + DynamicDSName;
    InputName->Text = strSpace + *InputObject->Outputs->Items[0]->Name;

    ApplyDSToRest->Enabled = (RemainingInputs > 1);
	ApplyDSToRest->Caption = "Apply to " + AnsiString(RemainingInputs) + " remaining " +
    	DynamicDSName + " inputs.";

    // The following prevents an "OK" on a blank data set from becoming a "Cancel".
    OkButton->Enabled = (DSEditBox->Text.Trim() != strNull);

    // Show the form.
    ShowModal();

    // Did the user quit?
    if (ModalResult != mrOk ||
    	DSEditBox->Text == strNull)
	        return strNull;

    // Save this "Composite Data Set Structure" so it can be the default next time.
    IniFile->WriteBool(strChooseDataSets, strApplyDSToRest, ApplyDSToRest->Checked);
    IniFile->WriteString(strChooseDataSets, DynamicDSName, DSEditBox->Text);

    return DSEditBox->Text;
}


// Parse the CompositeDataSet structure passed in.
//		InputObject is the input horn that needs its dynamic data set updated.
//		If CreateObjects is true, then the requested objects and transports will be created.
//		Returns true if the syntax is valid.
bool TChooseDataSetForm::ParseCompositeDataSet(AnsiString CompositeDataSet, I2adl *InputObject,
	bool CreateObjects)
{
	// Extract the data set name by finding the first delimiter.
    AnsiString	Constant = strNull;
    AnsiString	DataSetDef = strNull;
    AnsiString	CompositeDS = CompositeDataSet.Trim();

    for (int i = 1; i <= CompositeDS.Length(); i++)
    {
    	if (CompositeDS[i] == '=')
        {
        	// The data set is followed by a constant.
            Constant = CompositeDS.SubString(i + 1, CompositeDS.Length()).Trim();
            CompositeDS = CompositeDS.SubString(1, i - 1).Trim();

           	// There must be a constant value.
            if (Constant == strNull)
            {
                ErrorTrap(342, CompositeDataSet);

                return false;
            }

            break;
        }
        else if (CompositeDS[i] == '(')
        {
        	// The data set is followed by a data set definition.
            DataSetDef = CompositeDS.SubString(i, CompositeDS.Length()).Trim();
            CompositeDS = CompositeDS.SubString(1, i - 1).Trim();

            // There must be a matching ")".
            if (DataSetDef == strNull ||
            	DataSetDef[DataSetDef.Length()] != ')')
            {
                ErrorTrap(343, VStringList(CompositeDataSet, DataSetDef));

                return false;
            }

            break;
        }
    }

    // With the suffix stripped off (constant value or data set definition), CompositeDS
    //		must now be a valid data set.
    DataSet		*SubjectDS = GetDataSet(CompositeDS);

    if (SubjectDS == NULL)
    {
        ErrorTrap(341, VStringList(CompositeDataSet, "\"" + CompositeDS + "\""));

        return false;
    }

    // Is the data set compatible with the Input horn?
    DataSet		*DynamicDS = InputObject->Outputs->Items[0]->DSet;

    if (DynamicDS != SubjectDS &&
    	DynamicDS != VariantDataSet)
    {
        ErrorTrap(344, VStringList(CompositeDataSet, CompositeDS, DynamicDS->Name));

        return false;
    }

    // Replace the dynamic data set.
    if (CreateObjects)
    {
        InputObject->SuperSet = SubjectDS;
        InputObject->Outputs->Items[0]->PropagateDataSet(SubjectDS);

        // Put the constant attribute into the object.
        if (Constant != strNull)
            InputObject->SetAttribute(strConstant, Constant);
    }

    // Parse the data set definition string.
    if (DataSetDef != strNull)
        if (!ParseDataSetDef(SubjectDS, DataSetDef, InputObject, CreateObjects))
            return false;

    // The syntax is valid.
    return true;
}


// This recursive routine parses a data set definition by stripping chars off the front of the
//		DataSetDef string passed in.  The string should have the leading "(" and trailing ")".
//		SubjectDS is the data set whose children are defined by DataSetDef.
//		InputObject is the input horn that needs its dynamic data set replaced.
//		If CreateObjects is true, then the requested objects and transports will be created.
//		Returns true if the syntax is valid.
bool TChooseDataSetForm::ParseDataSetDef(DataSet *SubjectDS, AnsiString &DataSetDef,
	I2adl *InputObject, bool CreateObjects)
{
    // To do:  Create the requested objects and transports.
    return true;
}

// The text in the edit box has changed.  Using the new text in the edit box (that is under
//		the cursor), select and center the closest matching data set in the list box.
//		If the user is typing characters before a delimiter or at the end of the field,
//		then also try to autofill the rest of the data set name (unless they are deleting
//		previously auto filled characters).
void __fastcall TChooseDataSetForm::DSEditBoxChange(TObject *Sender)
{
	// Record the edit box text incase it needs to be undone.
    if (LastEditBoxText1 != DSEditBox->Text)
    {
        LastEditBoxText2 = LastEditBoxText1;
        LastEditBoxText1 = DSEditBox->Text;
    }

    // The following prevents an "OK" on a blank data set from becoming a "Cancel".
    OkButton->Enabled = (DSEditBox->Text.Trim() != strNull);

    // Select/center the closest matching data set in the list box.
    bool		IsConstant = false;
    AnsiString	DataSetPrefix = TextUnderCursor(IsConstant);
    AnsiString	DataSetFull = (IsConstant) ? LoadListBox(strNull)
    									   : LoadListBox(DataSetPrefix);

    // Do we want to automatically complete the data set?
    if (!IsConstant &&
        AutoCompleteDSName &&
        DataSetFull.UpperCase().Pos(DataSetPrefix.UpperCase()) == 1)
    {
        // Are we typing before a delimiter or at the end of the field?
        if (DSEditBox->SelStart >= DSEditBox->Text.Length() ||
            IsCSDelimiter(IsConstant, DSEditBox->Text, DSEditBox->SelStart + 1, 1))
        {
	        // Auto load the full data set name (to fix the case of the letters).
            TextUnderCursor(IsConstant, DataSetFull, true);
        }
    }

    AutoCompleteDSName = true;
}


void __fastcall TChooseDataSetForm::DSEditBoxKeyDown(TObject *Sender, WORD &Key,
	TShiftState Shift)
{
    if (Key == VK_UP)
    {
    	// Up arrow--previous data set.
        LoadEditBox(DSListBox->ItemIndex - 1);

        // The following prevents the cursor from moving, which unselects the data set.
        Key = VK_PRIOR;
    }
    else if (Key == VK_PRIOR)
    {
    	// Page Up--data set one page higher.
        LoadEditBox(DSListBox->ItemIndex - LinesInListBox());
    }
    else if (Key == VK_DOWN)
    {
    	// Down arrow--next data set.
        LoadEditBox(DSListBox->ItemIndex + 1);

        // The following prevents the cursor from moving, which unselects the data set.
        Key = VK_NEXT;
    }
    else if (Key == VK_NEXT)
    {
    	// Page Down--data set one page lower.
        LoadEditBox(DSListBox->ItemIndex + LinesInListBox());
    }
	else if (Key == VK_DELETE ||
    		 Key == VK_BACK)
    {
    	// Deleting characters--turn off auto completing of data set names.
    	AutoCompleteDSName = false;
    }
    else if (Key == 'A' ||
    		 Key == 'a')
    {
    	// CNTL+A--select all.
        if (GetKeyState(VK_CONTROL) < 0)
        {
            DSEditBox->SelStart = 0;
            DSEditBox->SelLength = DSEditBox->Text.Length();
        }
    }
    else if (Key == 'Z' ||
    		 Key == 'z')
    {
    	// CNTL+Z--undo the last change.
        if (GetKeyState(VK_CONTROL) < 0)
        {
            DSEditBox->Text = LastEditBoxText2;
            DSEditBox->SelStart = 0;
            DSEditBox->SelLength = DSEditBox->Text.Length();
        }
    }
    else if (Key == VK_LEFT)
    {
    	// If text is selected, then have the left arrow go to the left side of the selected
        //		text, unless more text is being selected (so it will behave like MS Word).
        if (GetKeyState(VK_SHIFT) >= 0 &&
        	DSEditBox->SelLength > 0)
        {
	    	DSEditBox->SelLength = 0;

	        // The following prevents the cursor from moving beyond the selected text.
	        Key = VK_NEXT;
        }
    }
    else if (Key == VK_RIGHT)
    {
    	// If text is selected, then have the right arrow go to the right side of the selected
        //		text, unless more text is being selected (so it will behave like MS Word).
        if (GetKeyState(VK_SHIFT) >= 0 &&
        	DSEditBox->SelLength > 0)
        {
        	DSEditBox->SelStart += DSEditBox->SelLength;
	    	DSEditBox->SelLength = 0;

	        // The following prevents the cursor from moving beyond the selected text.
	        Key = VK_NEXT;
        }
    }
}


void __fastcall TChooseDataSetForm::DSEditBoxKeyUp(TObject *Sender, WORD &Key,
	TShiftState Shift)
{
	// If the location of the cursor has changed, then reload the list box.
	//		This code is not in the "KeyDown" event handler because SelStart is not
    //		updated at that time.
    if (Key == VK_LEFT ||
    	Key == VK_RIGHT ||
        Key == VK_HOME ||
        Key == VK_END)
    {
    	AutoCompleteDSName = false;

    	DSEditBoxChange(NULL);
    }

    // The following makes sure the "auto complete" flag is always reset.
    AutoCompleteDSName = true;
}


void __fastcall TChooseDataSetForm::DSEditBoxMouseUp(TObject *Sender, TMouseButton
	Button, TShiftState Shift, int X, int Y)
{
	// If a mouse click changes the location of the cursor, then we need to reload the list
    //		box.
    bool		IsConstant = false;
    AnsiString	DataSetPrefix = TextUnderCursor(IsConstant);

    if (IsConstant)
    	LoadListBox(strNull);
    else
	    LoadListBox(DataSetPrefix);
}


// The user clicked on a data set in the list box.  Load it into the edit box by replacing
//		the data set under the cursor.
void __fastcall TChooseDataSetForm::DSListBoxClick(TObject *Sender)
{
	// Load the new data set into the data set edit box.
	LoadEditBox(DSListBox->ItemIndex);

    // Keep the Edit box the active control, keeping the same text selcted.
    int		OrigSelStart = DSEditBox->SelStart;
    int		OrigSelLength = DSEditBox->SelLength;

    ActiveControl = DSEditBox;

    DSEditBox->SelStart = OrigSelStart;
    DSEditBox->SelLength = OrigSelLength;
}


// Double clicking a data set closes the form (after the single click is handled).
void __fastcall TChooseDataSetForm::DSListBoxDblClick(TObject *Sender)
{
    ModalResult = mrOk;
}


// Prevents the "OK" button from returning an invalid "Composite Data Set Structure".
void __fastcall TChooseDataSetForm::FormClose(TObject *Sender, TCloseAction &Action)
{
    if (ModalResult == mrOk &&
    	!ParseCompositeDataSet(DSEditBox->Text, DynamicInput, false))
    {
       	Action = caNone;

        // Change the focus back to the edit box so the user can fix the syntax error.
	    ActiveControl = DSEditBox;
    }
}

