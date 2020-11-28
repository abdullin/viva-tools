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
#include <stdio.h>

#pragma hdrstop

#include "ErrorTrap.h"
#include "ErrorTrapForm.h"
#include "DataSet.h"
#include "I2adl.h"
#include "Node.h"
#include "VivaIcon.h"
#include "I2adlEditor.h"
#include "Main.h"
#include "Project.h"
#include "ReadWrite.h"
#include "MessageInfo.h"

#define ERROR_NUMBER_MAX		10000

int			MessageEntryCount = 0;
bool		ShowAllWarnings = false;

bool InitErrorMessages()
{
    try
    {
        ErrorMessages->LoadFromFile(ErrorTrapFileName);
    }
    catch (Exception &e)
    {
        Application->ShowException(&e);
        return false;
    }

	TStringList		*templist = new TStringList;

    for (int i = 0; i < ERROR_NUMBER_MAX; i++)
    	templist->Add(strNull);

	int				errornum = -1;

    for (int i = 0; i < ErrorMessages->Count; i++)
    {
        AnsiString		ErrorStr = ErrorMessages->Strings[i];

        if (ErrorStr == strNull)
        	continue;

        if (ErrorStr[1] == '#')
        {
			errornum = VStrToInt(ErrorStr.SubString(2, 4));

        	templist->Strings[errornum] = ErrorStr;
        }
        else if (errornum > -1)
        {
            char 	*cptr = ErrorStr.c_str();

	        for (int i = 0; i < 8; i++)
            {
            	if (*cptr++ == '\t')
                	break;
            }

			templist->Strings[errornum] = templist->Strings[errornum] + strCRLF + cptr;

	        continue;
        }

        int				options = 0;

        // Extract and store which options are selected; process up to three characters.
        for (int i = 0; i < 3; i++)
        {
        	if (i + 6 > ErrorStr.Length())
            	break;

            // Convert to upper case.
            char    Option = (char)(ErrorStr[6 + i] & 0x5f);

            if (Option <= ' ')
                break;

            if (Option == 'Y')
            {
                options |= moYesNo;
                options |= moModal;
            }
            else if (Option == 'C')
            {
                options |= moCancel;
                options |= moModal;
            }
            else if (Option == 'W')
            {
                options |= moWarning;
            }
            else if (Option == 'M')
            {
                options |= moModal;
            }
            else if (Option == 'U')
            {
                options |= moUserTrap;
            }
            else if (Option == 'I')
            {
                options |= moJustInfo;
            }
            else if (Option == 'S')
            {
				options |= moSearchResult;
            }
            else if (Option == 'O')
            {
				options |= moOverload;
            }
            else if (Option == 'T')
            {
            	options |= moStatus;
            }
        }

        templist->Objects[errornum] = (TObject *) options;
    }

    ErrorMessages->Assign(templist);
    MessageEntryCount = ErrorMessages->Count;

	delete templist;

    return true;
}


ErrorTrap::ErrorTrap(int _ErrorNumber, VStringList Messages)
{
    ErrorNumber = _ErrorNumber;
    MessageFragments = Messages;
    MessageInfo = NULL;
    IntegerValue = 0;
    AutoModal = false;
    YesButtonCaption = strNull;
    NoButtonCaption = strNull;
    OKButtonCaption = strNull;
    CancelButtonCaption = strNull;
    Echo = false;
    Result = mrCancel;
    Invoked = false;
    IsMessageInfoInMessageGrid = false;
}


// Destructor
ErrorTrap::~ErrorTrap()
{
    Invoke();

    // If the MessageInfo was put into the MessageGrid, then the MessageGrid will delete it.
    if (MessageInfo != NULL && !IsMessageInfoInMessageGrid)
        delete MessageInfo;
}


// Routine to handle Viva error, warning, info and trap messages.
//     ErrorNumber--Unique error number as set up in the ErrorMessage.txt file.
//     Message--Optional string message to be included in addition to the message in the
//         ErrorMessage.txt file.  If Message contains ";", then the text after it will be
//         placed on its own line in the message box.
//     IntegerValue--Optional integer debugging value
//     AutoModal--Set to "true" to force the message to always be modal (not just if the
//         ErrorMessage.txt file says it should)
//
//     Returns mrOK, mrYes, mrNo or mrCancel based on user entry.
//		[Messages] can be NULL.
void ErrorTrap::Invoke()
{
	if (Invoked)
    	return;

    Invoked = true;

    // The try/catch makes sure Viva recovers from running out of system resources (bug #979).
    try
    {
        if (MessageEntryCount < 0)
        {
            Result = mrCancel;
            return;
        }

        int		messageoptions = int(ErrorMessages->Objects[ErrorNumber]);

        if ((messageoptions & moDisabled) && !AutoModal && !ShowAllWarnings)
        {
            Result = mrOk;
            return;
        }

        // Does the error number exist in the ErrorMessage file?
        AnsiString		ErrorPrefix = AnsiString(ERROR_NUMBER_MAX + ErrorNumber);
        AnsiString		ErrorMessage(ErrorMessages->Strings[ErrorNumber]);

        ErrorPrefix[1] = '#';

        if (ErrorMessage == strNull)
        {
            AnsiString		Message = "Error number " + ErrorPrefix +
                " not found in ErrorMessage.txt!";

            Application->MessageBox(Message.c_str(), "Unknown Error Number", MB_OK);
            Result = mrCancel;
            return;
        }

        AnsiString		EMessage = ErrorMessage + "          ";
        int				DisabledIndex = DisabledErrors->IndexOf((void *) ErrorNumber);

        // If the user has disabled this error message, then use the original modal result.
        if (DisabledIndex >= 0)
        {
            int			OriginalModalResult = (int) DisabledModalResults->Items[DisabledIndex];

            if (OriginalModalResult != mrNone)
            {
                Result = OriginalModalResult;
                return;
            }
        }

        char			*ErrorStr = ErrorMessage.c_str();

        // Do not display the #[number].
        for (int i = 0; i < 8; i++)
        {
            if (*(ErrorStr++) == '\t')
                break;
        }

        AnsiString		TotalMessage(ErrorStr);
        TReplaceFlags	ReplaceFlags = TReplaceFlags() << rfReplaceAll;

        // Insert the Messages list into the total message.  Tack any message entry onto the end
        //		if there is no insertion point.  Each instance of [strMessage] is a new entry
        //		point.  You can't use it to replicate the same message entry.
        for (int i = 0; i < MessageFragments->Count; i++)
        {
            int		messageinsertpos = TotalMessage.Pos(strMessage);

            if (MessageFragments->Strings[i] == strNull)
                continue;

            if (messageinsertpos > 0)
            {
                int		secondpos = messageinsertpos + strMessage.Length();

                TotalMessage = TotalMessage.SubString(1, messageinsertpos - 1)
                    + MessageFragments->Strings[i] + TotalMessage.SubString(secondpos
                    , TotalMessage.Length() - secondpos + 1);
            }
            else
            {
                TotalMessage += strCRLF + strCRLF + MessageFragments->Strings[i];
            }
        }

        // Add a trailing blank line.
        TotalMessage += strCRLF;

        // Load the integer/double debug values.
        if (IntegerValue != 0)
            TotalMessage += "Integer Debug Value: " + AnsiString(IntegerValue) + strCRLF;

        // Display the error message form to the user.
        int		CloseButton;

    #if VIVA_IDE == false
        messageoptions |= moModal;
    #endif

        bool	domodal = (messageoptions & moModal) || AutoModal || RunMode != rmEdit;

        if (domodal && !Echo)
        {
            // Load the error information into the error message display form.
            TErrorTrapForm    	*TrapForm = new TErrorTrapForm(Application);
            TStrings    		*MemoLines = TrapForm->MemoMessage->Lines;

            TrapForm->CheckDisableNext->Checked = (DisabledIndex >= 0);
            TrapForm->EditErrorNumber->Caption = ErrorNumber;

            // If the Cancel button is visible, then have it respond to the "escape" key.
            TrapForm->BtnCancel->Visible = (messageoptions & moCancel);
            TrapForm->BtnNo->Cancel = !(messageoptions & moCancel);

            if (YesButtonCaption != strNull)
                TrapForm->BtnYes->Caption = YesButtonCaption;

            if (NoButtonCaption != strNull)
                TrapForm->BtnNo->Caption = NoButtonCaption;

            if (OKButtonCaption != strNull)
                TrapForm->BtnOK->Caption = OKButtonCaption;

            if (CancelButtonCaption != strNull)
                TrapForm->BtnCancel->Caption = CancelButtonCaption;

            if ((messageoptions & moWarning))
                TrapForm->Caption = "Viva Warning Message";

            if ((messageoptions & moYesNo))
            {
                TrapForm->BtnOK->Visible = false;
            }
            else
            {
                TrapForm->BtnYes->Visible = false;
                TrapForm->BtnNo->Visible = false;

                if (TrapForm->BtnCancel->Visible == true)
                    TrapForm->BtnCancel->Left -= 96;
                else
                    TrapForm->BtnOK->Left += 45;
            }

            // If less than 7 lines, then don't need a scroll bar.
            MemoLines->Text = TotalMessage;

            if (MemoLines->Count <= 6)
                TrapForm->MemoMessage->ScrollBars = ssNone;

            // Enable the controls so the user can close the modal form.
    #if VIVA_IDE
            bool	OrigControlsEnabled = MainForm->ControlsEnabled;
    #endif

			// The following NULL check allows CodeGuard to validate "this" pointers.
		    if (MainForm != NULL)
	            MainForm->EnableDisableControls(true);

            CloseButton = TrapForm->ShowModal();

    #if VIVA_IDE
            MainForm->EnableDisableControls(OrigControlsEnabled);
    #endif

            // Did the user disable this error messages?
            if (DisabledIndex >= 0)
            {
                if (TrapForm->CheckDisableNext->Checked)
                {
                    // Save the new ModalResult.
                    DisabledModalResults->Items[DisabledIndex] = (void *) CloseButton;
                }
                else
                {
                    // User turned off the DisableNext flag.
                    DisabledErrors->Delete(DisabledIndex);
                    DisabledModalResults->Delete(DisabledIndex);
                }
            }
            else if (TrapForm->CheckDisableNext->Checked)
            {
                // User turned on the DisableNext flag.
                DisabledErrors->Add((void *) ErrorNumber);
                DisabledModalResults->Add((void *) CloseButton);
            }

            delete TrapForm;

            // The node names under the ErrorTrapForm need to be repainted.
            PaintAllNames();
        }
        else
        {
            TotalMessage = StringReplace(TotalMessage, strCRLF, "  ", ReplaceFlags);

            AnsiString 		Prefix;
            TColor			MessageColor;

            if (messageoptions & moWarning)
            {
                Prefix = "Warning";
                MessageColor = clBlack;
            }
            else if (messageoptions & moUserTrap)
            {
                Prefix = "User Trap";
                MessageColor = clGreen;
            }
            else if (messageoptions & moJustInfo)
            {
                Prefix = "Info   ";
                MessageColor = clBlue;
            }
            else if (messageoptions & moSearchResult)
            {
                Prefix = "Search Result";
                MessageColor = clPurple;
            }
            else if (messageoptions & moOverload)
            {
                Prefix = "Search Result";
                MessageColor = clBlue;
            }
            else if (messageoptions & moStatus)
            {
                Prefix = "Status ";
                MessageColor = TColor(0x00004090); // brownish
            }
            else
            {
                Prefix = "Error  ";
                MessageColor = clRed;
            }

            if (MessageInfo == NULL)
                MessageInfo = new VMessageInfo;

            MessageInfo->MessageColor = MessageColor;

            if (!(messageoptions & (moUserTrap | moSearchResult | moOverload)))
                Prefix += "(" + AnsiString(ErrorNumber) + ")";

            for (int i = Prefix.Length(); i < 14; i++)
                Prefix += " ";

            Prefix += ": ";

            TotalMessage = Prefix + TotalMessage;

            // Count how many error and warning messages are issued during a compile.
            if (MyProject != NULL &&
                MyProject->Compiling &&
              !(messageoptions & (moUserTrap | moJustInfo | moStatus)))
            {
                NonModalErrorCount++;
            }

            TStringGrid		*grid = MainForm->MessageGrid;

            if (messageoptions & moSearchResult)
                grid = MainForm->SearchGrid;
            else if (messageoptions & moOverload)
                grid = MainForm->OverloadGrid;

            if (MainForm->MessageTabControl->ActivePage != (TTabSheet *) grid->Parent)
                MainForm->MessageTabControl->ActivePage = (TTabSheet *) grid->Parent;

            // Use the "Objects" array property to indicate what color to use in MessageGridDrawCell
            grid->Objects[0][grid->RowCount - 1] = (TObject *) MessageInfo;

            // Indicate that the MessageGrid will delete the MessageInfo.
            IsMessageInfoInMessageGrid = true;

            if (MainForm->MessagePanel->Height < grid->DefaultRowHeight * 6)
                MainForm->MessagePanel->Height = grid->DefaultRowHeight * 6;

            // Make sure that the columns are wide enough.  Can use a constant multiplier of
            //		7 * message length because the font is fixed-width
            grid->DefaultColWidth =	max(grid->DefaultColWidth, TotalMessage.Length() * 7);
            grid->Cells[0][grid->RowCount - 1] = TotalMessage;
            grid->RowCount = grid->RowCount + 1;

            TGridRect 	rect;

            rect.Left = 0;
            rect.Right = rect.Left;
            rect.Top = grid->RowCount - 1;
            rect.Bottom = rect.Top;

            grid->Selection = rect;

            grid->Perform(WM_KEYDOWN, VK_DOWN, VK_DOWN);
            grid->Perform(WM_KEYDOWN, VK_UP, VK_UP);
            grid->Perform(WM_KEYDOWN, VK_DOWN, VK_DOWN);

            CloseButton = mrOk;
        }

    #if VIVA_IDE
        if (MainForm != NULL && MainForm->ErrorFileHandle != -1)
        {
            try
            {
                WriteString2(MainForm->ErrorFileHandle, TotalMessage + strCRLF);
            }
            catch(...)
            {
            }
        }
    #endif

        Result = CloseButton;
    }
    catch(...)
    {
        AnsiString	Message = "Unable to display message #" + IntToStr(ErrorNumber) + ".";

		Application->MessageBox(Message.c_str(), "Message Display Error", MB_OK);
    }
}


// Display the ErrorMessage in the normal ErrorTrap message window with the Object's hierarchy,
//		sheet location and input data sets.  If AutoModal is true, then the message will be
//		displayed in a modal window.
int ErrorTrace(int ErrorNumber, I2adl *Object, VStringList Messages, bool AutoModal)
{
    // Do we have the parent object's input data sets?
    AnsiString		Diagnasty = strNull;

    Object->GetAttribute(strParentDataSets, Diagnasty);

    if (Diagnasty == strNull)
    	Diagnasty = Object->InputDataSets();

    // Display the ErrorMessage first.
	VStringList	params;

	params->Assign(Messages);
	params->Add(Diagnasty);

    AnsiString	QualName = Object->QualifiedName();
	I2adl		*parent = CurrentSheet;

    if (Object->MemberOf != NULL)
    {
    	parent = Object->MemberOf->SourceI2adl;
    }
	else if (Object->ParentI2adl != NULL)
    {
    	parent = Object->ParentI2adl;

        if (parent->WipSheetCode != wsNotAWipSheet ||
            MyProject->Wip->IndexOf(parent) > -1)
        {
        	QualName += "    in " + parent->NameInfo->Name;
        }
        else
        {
            QualName += " of " + parent->NameInfo->Name
                     +  " (" + parent->Attributes->Values[strDocumentation]
                     +  " " + IntToStr(parent->XLoc)
                     +  "," + IntToStr(parent->YLoc)
                     +  ")";
        }
    }

	params->Add(QualName);

    // Display the ErrorTrap window.
    ErrorTrap	e(ErrorNumber, params);

    e.AutoModal = AutoModal;

	if (parent != NULL)
    {
		I2adlList		*behavior = parent->Behavior;
        I2adlList		*results = behavior->FindInstances(Object, scOverload, parent);
        int				index = results->IndexOf(Object->MasterSource);
        VMessageInfo    *minfo = new VMessageInfo(Object, Object, scOverload, index);

        minfo->MemberOf = Object->MemberOf;

        if (minfo->MemberOf == NULL)
        {
            if (IsRunning)
                minfo->I2adlObject->ParentI2adl = MyProject->CompiledSheet;
            else if (MyProject != NULL && MyProject->Compiling)
                minfo->I2adlObject->ParentI2adl = MyProject->CompilingSheet;
        }

	    e.MessageInfo = minfo;

	    delete results;
    }

    int		ModalResult = e;

    if (ModalResult == mrCancel)
        CancelCompile = true;

    return ModalResult;
}


// Determines if an output object is a trap.
//     If CDex==1, then the error is displayed.
bool TrapCheck(I2adl *Object, int CDex)
{
    AnsiString    Trap;

    Object->GetAttribute(strTrap, Trap);

    if (Trap == strNull)
        return false;

    if (CDex == 0)
        return true;

    char	*TrapStr = Trap.c_str();
    int		ErrorNumber = 0;

    for ( ; true ; TrapStr++)
    {
        int		chr = *TrapStr;

        if (chr == ' ')
            continue;

        if (chr < '0' || chr > '9')
            break;

        ErrorNumber = ErrorNumber * 10 + chr - '0';
    }

    if (ErrorNumber == 0)
        ErrorNumber = 3000;

    AnsiString		Diagnasty = AnsiString(TrapStr);

    if (ErrorTrace(ErrorNumber, Object, Diagnasty) == mrCancel)
        CancelCompile = true;

    return true;
}


// The usage occured on the Object which is on ParentObject's behavior.
bool UsageTrap(int MessageNr, I2adl *Object, I2adl *ParentObject, AnsiString Addendum)
{
    AnsiString    Message = Object->EnhancedName();

    if (ParentObject == NULL)
        ParentObject = Object;

    if (ParentObject->NameInfo != Object->NameInfo)
        Message += " of " + ParentObject->EnhancedName();

    Message += ParentObject->ExternalName();

    if (Addendum != strNull)
        Message += Addendum;

    int    ret = ErrorTrap(MessageNr, Message);

    return (ret == mrYes  ||  ret == mrOk);
}
