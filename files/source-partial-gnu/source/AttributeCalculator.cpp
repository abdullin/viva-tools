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

#include "globals.h"
#include "AttributeCalculator.h"
#include "I2adl.h"
#include "DataSet.h"
#include "ErrorTrap.h"
#include "IniFile.h"
#include "Node.h"
#include "Project.h"
#include "SearchEngine.h"
#include "VivaSystem.h"
#include "VivaIO.h"
#include "List.h"

using namespace std;

#include <math.h>
#include <float.h>
#include <limits.h>
#include <errno.h>

#pragma package(smart_init)


// Constructor - Default
AttributeCalculator::AttributeCalculator()
{
	Stack.clear();
}


AttributeCalculator::~AttributeCalculator()
{
	Stack.clear();
}


// If Attribute contains old Attribute Calculator syntax, then convert it to the new format.
//      OrigFromObject is for ErrorTrace() reporting.  If it is NULL, FromObject will be used.
void AttributeCalculator::ConvertToNewSyntax(AnsiString &Attribute, VivaSystem *FromSystem,
	I2adl *FromObject, I2adl *OrigFromObject)
{
	if (Attribute.Length() < 2)
    	return;

	if (OrigFromObject == NULL)
    	OrigFromObject = FromObject;

    // Strip off the promote/no promote "*" prefix.
    bool		HasAsterisk = (Attribute[1] == '*');
    AnsiString	ParseString = (HasAsterisk) ? Attribute.SubString(2, Attribute.Length())
                            				: Attribute;

    // Directory Prefix Macros: Convert $B\  to  $(DirName:Build)
    //									$F\  to  $(FileName:FPGA)
    //									$N\  to  $(FileName:Project)
    //									$P\  to  $(DirName:Project)
    //									$S\  to  $(DirName:VivaSystem)
    //									$U\  to  $(DirName:User)
    //									$V\  to  $(DirName:Viva)
    int     FirstNonQuote;

    for (FirstNonQuote = 1; FirstNonQuote <= ParseString.Length(); FirstNonQuote++)
    	if (ParseString[FirstNonQuote] != '"')
        		break;

    // Does the attribute contain a directory prefix macro?
    if (ParseString.Length() >= FirstNonQuote + 2 &&
    	ParseString[FirstNonQuote] == '$' &&
        ParseString[FirstNonQuote + 2] == '\\')
    {
        char    	DirectoryCode = ParseString.UpperCase()[FirstNonQuote + 1];
        AnsiString	RelativeDirectory = strNull;

        if (DirectoryCode == 'B')
        {
            RelativeDirectory = "$(DirName:Build)";
        }
        else if (DirectoryCode == 'F')
        {
            RelativeDirectory = "$(FileName:FPGA)";
        }
        else if (DirectoryCode == 'N')
        {
            RelativeDirectory = "$(FileName:Project)";
        }
        else if (DirectoryCode == 'P')
        {
            RelativeDirectory = "$(DirName:Project)";
        }
        else if (DirectoryCode == 'S')
        {
            RelativeDirectory = "$(DirName:VivaSystem)";
        }
        else if (DirectoryCode == 'U')
        {
            RelativeDirectory = "$(DirName:User)";
        }
        else if (DirectoryCode == 'V')
        {
            RelativeDirectory = "$(DirName:Viva)";
        }
        else
        {
            // Invalid relative directory code.
            ErrorTrapTrace(148, VStringList(Attribute, DirectoryCode), FromSystem, FromObject,
            	OrigFromObject);

            RelativeDirectory = ParseString.SubString(FirstNonQuote, 3);
        }

        ParseString = ParseString.SubString(1, FirstNonQuote - 1) + RelativeDirectory +
        	ParseString.SubString(FirstNonQuote + 3, ParseString.Length() - 2 - FirstNonQuote);
    }

   	// Other old macro syntax is the entire attribute and it is in double quotes.
    if (ParseString.Length() < 2 ||
    	ParseString[1] != '"' ||
        ParseString[ParseString.Length()] != '"')
    {
    	Attribute = (HasAsterisk) ? AnsiString("*" + ParseString)
        						  : ParseString;

        return;
    }

    // Remove the double quotes.
	ParseString = ParseString.SubString(2, ParseString.Length() - 2).Trim();

    // Don't try to calcualte empty strings.
    if (ParseString.Length() <= 0)
    {
		ErrorTrapTrace(34, strNull, FromSystem, FromObject, OrigFromObject);

        Attribute = "0";
    }

    // Numerical calculations:  Convert "(<macro>, <macro>, <macro>)"
    //								 to $(<macro>, <macro>, <macro>)
	else if (ParseString[1] == '(' &&
    		 ParseString[ParseString.Length()] == ')')
	{
    	// Recursive call to convert each <macro>.
        //		Strip off the parentheses.
        ParseString = ParseString.SubString(2, ParseString.Length() - 2);

        // Parse out each <macro> string.
        TStringList		*PStrings = VivaGlobals::ParseString(ParseString, ",");

		Attribute = "$(";

        for (int i = 0; i < PStrings->Count; i++)
        {
        	AnsiString	PString = "\"" + PStrings->Strings[i] + "\"";

			ConvertToNewSyntax(PString, FromSystem, FromObject, OrigFromObject);

            if (i <= 0)
	            Attribute += PString;
            else
            	Attribute += ", " + PString;

        }

        Attribute += ")";
	}

	// System attributes:    Convert "$[<SubsystemName>:]<AttributeName>[++]"
    //							     "$[<SubsystemName>=]<AttributeName>[++]"
    //							  to $(SysAtt:[<SubsystemName>]:<AttributeName>[++])
    //
	// Symbol Substitution:  Convert "_<AttributeName>"
    //							  to $(SysAtt::<AttributeName>)
	else if (ParseString[1] == '$' ||
 			 ParseString[1] == '_')
	{
        // "$(" is new syntax (not a system attribute reference)
        if (ParseString.SubString(1, 2) == "$(")
        {
            // Remove the double quotes.
            Attribute = ParseString;
        }
        else
        {
            // Change the "=" delimiter to be ":".
            int		EqualLoc = ParseString.Pos("=");

            if (EqualLoc > 1)
                ParseString[EqualLoc] = ':';

            // Was the subsystem name specified?
            Attribute = (ParseString.Pos(":") > 1) ? "$(SysAtt:"
	            								   : "$(SysAtt::";

            Attribute += ParseString.SubString(2, ParseString.Length() - 1) + ")";
        }
	}

	// "##" node macros:  Convert "##N<NodeNumber>.X"
    //							  "##N<NodeName>.X"
    //						   to $(NodeInfo:<NodeNumber>:<NodeCode>)
    //							  $(NodeInfo:<NodeName>:<NodeCode>)
	else if (ParseString[1] == '#')
	{
        // Remove the "##" prefix.
		ParseString = ParseString.SubString(2, ParseString.Length() - 1);

        if (ParseString[1] == '#')
			ParseString = ParseString.SubString(2, ParseString.Length() - 1);

       	// Strip off the "N" prefix.
        if (ParseString[1] == 'N')
        {
            ParseString = ParseString.SubString(2, ParseString.Length() - 1);

            // Extract the node name/number and function code.
			TStringList		*PStrings = VivaGlobals::ParseString(ParseString, ".");

            if (PStrings->Count != 2)
            {
                // There must be exactly one separator period.
                ErrorTrapTrace(180, Attribute, FromSystem, FromObject, OrigFromObject);

                Attribute = "0";
            }
            else
            {
                // Convert the function codes.
                char	FunctionCode = PStrings->Strings[1].UpperCase()[1];

                if (FunctionCode == 'A')
                    Attribute = "$(NodeInfo:" + PStrings->Strings[0] + ":DSNum)";
                else if (FunctionCode == 'B')
                    Attribute = "$(NodeInfo:" + PStrings->Strings[0] + ":BitLen)";
                else if (FunctionCode == 'H')
                    Attribute = "$(NodeInfo:" + PStrings->Strings[0] + ":ChildCount)";
                else if (FunctionCode == 'L')
                    Attribute = "$(NodeInfo:" + PStrings->Strings[0] + ":ByteLen)";
                else if (FunctionCode == 'T')
                    Attribute = "$(NodeInfo:" + PStrings->Strings[0] + ":WidgetHandling)";
                else
                    Attribute = "0";
            }

            delete PStrings;
        }
	}

	// Data File:  Convert ""<FilePath><FileName>""
    //					   ""<DirPrefixMacro><FileName>""
    //					to $(DataFile:<FilePath><FileName>:<IndexValue>)
    //					   $(DataFile:<DirPrefixMacro><FileName>:<IndexValue>)
	else if (ParseString[1] == '\"')
	{
    	if (ParseString.Length() < 2 ||
        	ParseString[ParseString.Length()] != '\"')
        {
        	// The trailing quote is missing.
            ErrorTrapTrace(60, Attribute, FromSystem, FromObject, OrigFromObject);

            Attribute = "0";
        }
        else
        {
            // Strip off the second set of double quotes.
            AnsiString    FileName = ParseString.SubString(2, ParseString.Length() - 2).Trim();

            if (FileName == strNull)
            {
                // The file name was not provided.
                ErrorTrapTrace(27, Attribute, FromSystem, FromObject, OrigFromObject);

                Attribute = "0";
            }
            else if (FromObject == NULL)
            {
                // We must have an object to get the Index attribute.
                ErrorTrapTrace(36, Attribute, FromSystem, FromObject, OrigFromObject);

                Attribute = "0";
            }
            else
            {
                // Get the Index attribute and put it into the syntax.
                AnsiString	IndexValue = FromObject->Attributes->Values[strIndex];

                ConvertToNewSyntax(IndexValue, FromSystem, FromObject, OrigFromObject);

                Attribute = "$(DataFile:" + FileName + ":D" + IndexValue + ")";
            }
        }
	}

    // Default case: Remove the double quotes.
    else
    {
    	Attribute = ParseString;
    }

    if (HasAsterisk)
    	Attribute = "*" + Attribute;
}


// Calculate the value of the Attribute string passed in.
//      OrigFromObject is for ErrorTrace() reporting.  If it is NULL, FromObject will be used.
void AttributeCalculator::CalculateAttribute(AnsiString &Attribute, VivaSystem *FromSystem,
	I2adl *FromObject, I2adl *OrigFromObject)
{
	if (OrigFromObject == NULL)
    	OrigFromObject = FromObject;

    AnsiString	ParseString = Attribute;

    // Process backwards to automatically handle nested "$(" macros.
    for (int StartLoc = ParseString.Length() - 1; StartLoc >= 1; StartLoc--)
    {
    	// Find the last "$("?
        if (ParseString.SubString(StartLoc, 2) != "$(")
        	continue;

        // Find the closing ")".
        int		EndLoc;

        for (EndLoc = StartLoc + 2; EndLoc <= ParseString.Length(); EndLoc++)
        	if (ParseString[EndLoc] == ')')
            	break;

        // The following "if" test catches missing ")" even if ParseString ends with "$(".
        if (EndLoc > ParseString.Length())
        {
			ErrorTrapTrace(111, Attribute, FromSystem, FromObject, OrigFromObject);

            ParseString = "0";
            break;
        }

		// Extract the stripped attribute and calculate it.
        AnsiString	StrippedAttribute = ParseString.SubString(StartLoc + 2, EndLoc -
        	StartLoc - 2).Trim();

        CalculateStrippedAtt(StrippedAttribute, FromSystem, FromObject, OrigFromObject);

        ParseString = ParseString.SubString(1, StartLoc - 1)
        			+ StrippedAttribute
                    + ParseString.SubString(EndLoc + 1, ParseString.Length() - EndLoc);

        // Reset the next pass to handle recursive/nested macros.
        StartLoc = ParseString.Length();
    }

    Attribute = ParseString;
}


// Calculate the value of the Attribute passed in stripped format (no "$(" and ")").
void AttributeCalculator::CalculateStrippedAtt(AnsiString &Attribute, VivaSystem *FromSystem,
	I2adl *FromObject, I2adl *OrigFromObject)
{
    // Don't try to calcualte empty strings.
    if (Attribute.Length() <= 0)
    {
		ErrorTrapTrace(34, strNull, FromSystem, FromObject, OrigFromObject);

        Attribute = "0";
        return;
    }

    // RPN calculations are separated by ",".
	if (Attribute.Pos(",") > 0)
	{
		Attribute = RPNCalculations(Attribute, FromSystem, FromObject, OrigFromObject);

        return;
	}

    // Parse the pieces of the macro (":" delimiter).
    TStringList		*MacroPieces = ParseString(Attribute, ":", true, false);
    AnsiString		FunctionCode = (MacroPieces->Count > 0) ? MacroPieces->Strings[0]
    														: strNull;

	// System Attribute references have the following format:
    //		$(SysAtt:[<SubsystemName>]:<AttributeName>[++][:DefaultValue])
	if (FunctionCode == "SysAtt")
	{
    	if (MacroPieces->Count < 3 ||
        	MacroPieces->Count > 4)
        {
            ErrorTrapTrace(65, Attribute, FromSystem, FromObject, OrigFromObject);

            Attribute = "0";
        }
        else
        {
            // Extract the SubsystemName, AttributeName and DefaultValue.
            AnsiString	SubsystemName = MacroPieces->Strings[1];
            AnsiString	AttributeName = MacroPieces->Strings[2];
            AnsiString	DefaultValue  = (MacroPieces->Count > 3)
            						  ? MacroPieces->Strings[3]
                                      : strNull;

            // Convert "BaseSystem" to actual base system name.
            VivaSystem	*Subsystem    = (SubsystemName == strNull)
            						  ? BaseSystem
                                      : BaseSystem->GetSystem(SubsystemName);

            if (Subsystem == NULL)
            {
                // Did not reference a valid subsystem name.
                ErrorTrapTrace(25, Attribute, FromSystem, FromObject, OrigFromObject);

                Attribute = "0";
            }
            else
            {
                // If the system doesn't have the attribute, then check every parent system
                //		up to the base system.
                AnsiString	AttributeValue = strNull;

                while (Subsystem != NULL)
                {
                    AttributeValue = Subsystem->GetAttribute(AttributeName, false, true,
                        OrigFromObject);

                    if (AttributeValue != strNull)
                        break;

                    Subsystem = Subsystem->ParentSystem;
                }

                if (AttributeValue == strNull)
                {
                    // Did not reference a valid system attribute.
                    //		If the user didn't supply a default value, then warn them.
                    if (DefaultValue == strNull)
                    {
                        ErrorTrapTrace(37, Attribute, FromSystem, FromObject, OrigFromObject);

                        Attribute = "0";
                    }
                    else
                    {
                        Attribute = DefaultValue;
                    }
                }
                else
                {
                    // We found the attribute.  Recursive call to calculate it.
                    CalculateAttribute(AttributeValue, Subsystem, FromObject, OrigFromObject);

                    Attribute = AttributeValue;
                }
            }
        }
	}

	// Parent Object Attributes have the following format:
    //		$(ObjAtt:<AttName>[:DefaultValue>])
    else if (FunctionCode == "ObjAtt")
    {
    	if (MacroPieces->Count < 2 ||
        	MacroPieces->Count > 3)
        {
            ErrorTrapTrace(66, Attribute, FromSystem, FromObject, OrigFromObject);

            Attribute = "0";
        }

        // Do we have a parent object to get node information from?
		else if (FromObject == NULL ||
        		 FromObject->MemberOf == NULL ||
        		 ParentI2adlObject == NULL)
        {
			ErrorTrapTrace(35, Attribute, FromSystem, FromObject, OrigFromObject);

            Attribute = "0";
        }

        // Extract the AttributeName and DefaultValue.
        else
        {
            AnsiString	AttributeName = MacroPieces->Strings[1];
            AnsiString	DefaultValue  = (MacroPieces->Count < 3) ? strNull
                                                                 : MacroPieces->Strings[2];

            // Does the attribute exist on the parent object?
            AnsiString	AttributeValue = strNull;

            ParentI2adlObject->GetAttribute(AttributeName, AttributeValue);

            if (AttributeValue == strNull)
            {
                // Did not reference a valid object attribute.
                //		If the user didn't supply a default value, then warn them.
                if (DefaultValue == strNull)
                {
                    ErrorTrapTrace(61, Attribute, FromSystem, FromObject, OrigFromObject);

                    Attribute = "0";
                }
                else
                {
                    Attribute = DefaultValue;
                }
            }
            else
            {
                // We found the attribute.  Recursive call to calculate it.
                CalculateAttribute(AttributeValue, FromSystem, FromObject, OrigFromObject);

                Attribute = AttributeValue;
            }
        }
    }

    // Node Information has the following format:
    //		$(NodeInfo:<NodeNumber>:<NodeCode>)
    //		$(NodeInfo:<NodeName>:<NodeCode>)
	else if (FunctionCode == "NodeInfo")
	{
    	if (MacroPieces->Count != 3)
        {
            ErrorTrapTrace(67, Attribute, FromSystem, FromObject, OrigFromObject);

            Attribute = "0";
        }

        // Do we have a parent object to get node information from?
		else if (FromObject == NULL ||
        		 ParentI2adlObject == NULL)
        {
			ErrorTrapTrace(35, Attribute, FromSystem, FromObject, OrigFromObject);

            Attribute = "0";
        }

        // Extract the node name/number and function code.
        //		Did the user specify a node number?
        else
        {
            int			NodeNumber = -1;
            Node		*FromNode = NULL;
            AnsiString	NodeNameNumber = MacroPieces->Strings[1];
            AnsiString	NodeCode = MacroPieces->Strings[2];

            try
            {
                // The "if" statement avoids a lot of C++ exceptions while running the
                //		Borland debugger.
                if (NodeNameNumber[1] >= '0' &&
                    NodeNameNumber[1] <= '9')
                        NodeNumber = StrToInt(NodeNameNumber);
            }
            catch(...)
            {
            }

            if (NodeNumber >= 0 &&
                NodeNumber < ParentI2adlObject->Inputs->Count + ParentI2adlObject->Outputs->
                	Count)
            {
                FromNode = ParentI2adlObject->GetNode(NodeNumber);
            }
            else
            {
                // Did the user specify a node name?
                //		We need to get the node names off the I2adl def (not the ref).
                I2adl	*I2adlDef = ParentI2adlObject->ResolveObject(false, false, NULL,
                    true);

                if (I2adlDef == NULL)
                    I2adlDef = ParentI2adlObject;

                // Pass #1 - Inputs.
                NodeList	*IONodes = I2adlDef->Inputs;

                for (int IO = 0; IO < 2; IO++)
                {
                    for (int i = 0; i < IONodes->Count; i++)
                    {
                        Node	*IONode = IONodes->Items[i];

                        if (*IONode->Name == NodeNameNumber)
                        {
                            FromNode = (IO == 0) ? ParentI2adlObject->Inputs->Items[i]
                            					 : ParentI2adlObject->Outputs->Items[i];

                            // The following breaks out of both "for" loops.
                            IO++;
                            break;
                        }
                    }

                    // Pass #2 - Outputs.
	                IONodes = I2adlDef->Outputs;
                }
            }

            if (FromNode == NULL)
            {
                ErrorTrapTrace(3015, Attribute, FromSystem, FromObject, OrigFromObject);

                Attribute = "0";
            }
            else if (NodeCode == "BitLen")
            {
                // Bit length
                Attribute = IntToStr(FromNode->DSet->BitLength);
            }
            else if (NodeCode == "ByteLen")
            {
                // Byte length
                Attribute = IntToStr(FromNode->DSet->ByteLength);
            }
            else if (NodeCode == "ChildCount")
            {
                // Child data set count
                Attribute = IntToStr(FromNode->DSet->ChildDataSets->Count);
            }
            else if (NodeCode == "DSNum")
            {
                // Data set number
                Attribute = IntToStr(ProjectDataSets->IndexOf(FromNode->DSet));
            }
            else if (NodeCode == "WidgetHandling")
            {
                // Widget handling type
                Attribute = IntToStr(FromNode->DSet->AttributeFlags);
            }
            else
            {
                ErrorTrapTrace(3016, VStringList(Attribute, NodeCode), FromSystem, FromObject,
                	OrigFromObject);

                Attribute = "0";
            }
        }
	}

	// Data Files have the following format:
    //		$(DataFile:<FilePath><FileName>:<IndexValue>)
    //		$(DataFile:<DirPrefixMacro><FileName>:<IndexValue>)
	else if (FunctionCode == "DataFile")
	{
    	// Prevent the ":" after a drive letter (like C:) from being a separator character.
    	if (MacroPieces->Count >= 3 &&
        	MacroPieces->Strings[1].Length() == 1)
        {
            MacroPieces->Strings[1] = MacroPieces->Strings[1] + ":" + MacroPieces->Strings[2];

            for (int i = 3; i < MacroPieces->Count; i++)
	            MacroPieces->Strings[i - 1] = MacroPieces->Strings[i];

            MacroPieces->Delete(MacroPieces->Count - 1);
        }

    	if (MacroPieces->Count != 3)
        {
            ErrorTrapTrace(68, Attribute, FromSystem, FromObject, OrigFromObject);

            Attribute = "0";
        }
        else
        {
            AnsiString	FileName = MacroPieces->Strings[1];
            AnsiString	IndexValue = MacroPieces->Strings[2];

            // Try to read the data file.
            if (FileExists(FileName))
            {
                VIniFile	*IniFile = new VIniFile(FileName, true);

                Attribute = IniFile->ReadString("romdata", IndexValue, strNull);

                if (Attribute == strNull)
                {
                    // Could not find the requested key in the file.
                    ErrorTrapTrace(22, VStringList(IndexValue, "romdata", FileName),
                    	FromSystem, FromObject, OrigFromObject);

                    Attribute = "0";
                }

                delete IniFile;
            }
            else
            {
                // The file does not exist.
                ErrorTrapTrace(58, VStringList(FileName, Attribute), FromSystem, FromObject,
                    OrigFromObject);

                Attribute = "0";
            }
        }
	}

	// Directory Names have the following format:
    //		$(DirName:<DirCode>)
	else if (FunctionCode == "DirName")
	{
    	if (MacroPieces->Count != 2)
        {
            ErrorTrapTrace(69, Attribute, FromSystem, FromObject, OrigFromObject);

            Attribute = "0";
        }
        else
        {
            AnsiString	DirCode = MacroPieces->Strings[1];

            if (DirCode == "Build")
            {
                Attribute = GetBuildDir();
            }
            else if (DirCode == "Project")
            {
                if (MyProject != NULL &&
                	MyProject->FilePath != strNull)
                {
                    Attribute = MyProject->FilePath;
                }
                else
                {
                    Attribute = UserDir;
                }
            }
            else if (DirCode == "SystemDesc")
            {
            	AnsiString	SystemName = (MyProject->DynamicSystemName == strNull)
            						   ? MyProject->SystemName
                                       : MyProject->DynamicSystemName;

				if (SystemName == strNull)
                	SystemName = "System Description";

               	// If the system description is unknown or no longer exists, then require the
                //		user to browse for a valid one.
                BrowseFile(SystemName, false, "Sys Desc Files (*.sd)|*.sd");

                // Save the name so the user will only be prompted once.
                MyProject->SystemName = SystemName;

                Attribute = GetFilePath(SystemName);
            }
            else if (DirCode == "User")
            {
                Attribute = UserDir;
            }
            else if (DirCode == "Viva")
            {
                Attribute = strVivaDir;
            }
            else if (DirCode == "VivaSystem")
            {
                Attribute = strVivaSystemDir;
            }
            else
            {
                // Invalid relative directory code.
                ErrorTrapTrace(151, VStringList(Attribute, DirCode), FromSystem, FromObject,
                    OrigFromObject);

                Attribute = "0";
                return;
            }
    	}
    }

    // File Names have the following format:
    //		$(FileName:<FileCode>)
    else if (FunctionCode == "FileName")
    {
    	if (MacroPieces->Count != 2)
        {
            ErrorTrapTrace(70, Attribute, FromSystem, FromObject, OrigFromObject);

            Attribute = "0";
        }
        else
        {
            AnsiString	FileCode = MacroPieces->Strings[1];

            if (FileCode == "FPGA")
            {
                // Calculate the batch file name from the FPGA system name.
                VivaSystem	*FPGASystem = FromSystem;

                if (FromSystem != NULL &&
                	FromSystem->Type == FPGA_TYPE)
                {
                    // A FPGA system was passed in.  Use it.
                }
                else if (FromObject != NULL)
                {
                    // Restore the system active at compile time (if any).
                    VivaSystem		*OrigInSystem = FromObject->InSystem;
                    SystemString	*OrigAttSystem = FromObject->AttSystem;
                    AnsiString		CompileTimeSystem;

                    FromObject->GetAttribute(strCompileTimeSystem, CompileTimeSystem);

                    if (CompileTimeSystem != strNull)
                    {
                        FromObject->InSystem = NULL;
                        FromObject->AttSystem = SystemStringTable->QString(CompileTimeSystem);
                    }

                    VivaSystem	*ObjectSystem = FromObject->GetSystem();

                    FromObject->InSystem = OrigInSystem;
                    FromObject->AttSystem = OrigAttSystem;

                    // Is the object's system better than the FromSystem?
                    if (ObjectSystem != NULL)
                    {
                    	if (FPGASystem == NULL ||
                        	ObjectSystem->Type == FPGA_TYPE)
                            	FPGASystem = ObjectSystem;
                    }
                }

                if (FPGASystem == NULL)
                {
                    // A FPGA system was not provided.
                    ErrorTrapTrace(149, Attribute, FromSystem, FromObject, OrigFromObject);

                    Attribute = "0";
                }
                else
                {
                    // Remove the BaseSystem name and all "\" characters from the system's
                    //		location.
                    int    FirstSlash = FPGASystem->Location.Pos(strBackSlash);

                    Attribute = MyProject->Name + "_" + FPGASystem->Location.SubString(
                    	FirstSlash + 1, FPGASystem->Location.Length() - FirstSlash);

                    Attribute = ReplaceAll(Attribute, strBackSlash, strNull);

                    IsValidEDIFName(Attribute);
                }
            }
            else if (FileCode == "Project")
            {
                // Get the "short" project file name (without path or file extention).
                Attribute = MyProject->Name;
            }
            else
            {
                // Invalid FileCode.
                ErrorTrapTrace(155, VStringList(Attribute, FileCode), FromSystem, FromObject,
                    OrigFromObject);

                Attribute = "0";
            }
        }
    }

	else
	{
    	// Default case:  Just return the original stripped Attribute.
	}

    delete MacroPieces;
}


// Perform the numerical attribute calculations.  The syntax has the form
//		"Macro, Macro, Macro, ..." in RPN (Reverse Polish Notation) format.
AnsiString AttributeCalculator::RPNCalculations(AnsiString Attribute, VivaSystem *FromSystem,
	I2adl *FromObject, I2adl *OrigFromObject)
{
    // Extract the comma separated list of macro codes.
	TStringList		*MacroCodes = ParseString(Attribute, ",");

	// Process each macro.
	for (int i = 0; i < MacroCodes->Count; i++)
	{
        AnsiString	MacroCode = MacroCodes->Strings[i];

        // Is the macro a one character operator code?
        if (MacroCode.Length() == 1 &&
           (MacroCode[1] < '0' || MacroCode[1] > '9'))
        {
            if (!Operate(MacroCode[1], FromSystem, FromObject, OrigFromObject))
            {
            	// The following causes error message #3022 to be generated below.
		        Stack.clear();

                break;
            }
        }
        else
        {
        	// Recursive call to calculate the macro.  Push the result onto the stack.
            CalculateStrippedAtt(MacroCode, FromSystem, FromObject, OrigFromObject);

            try
            {
	            Stack.push_back(StrToFloat(MacroCode));
            }
            catch(...)
            {
            	// The following causes error message #3022 to be generated below.
		        Stack.clear();

                break;
            }
        }
	}

    delete MacroCodes;

    // The return value is the last number in the stack.
	if (Stack.empty())
    {
		ErrorTrapTrace(3022, Attribute, FromSystem, FromObject, OrigFromObject);

        return "0";
	}

    AnsiString	Result = FloatToStr(Stack.back());

    Stack.clear();

    return Result;
}


// Because we are relying on single character operators, don't make any of them numbers or
//		use any of the symbols used in the above functions.
//		Returns false if the calculation could not be made.
bool AttributeCalculator::Operate(char operation, VivaSystem *FromSystem, I2adl *FromObject,
	I2adl *OrigFromObject)
{
	if (Stack.empty())
		return false;

    // Unary operators:
    long double		ValueY = Stack.back();

    Stack.pop_back();

	switch (operation)
	{
		case 's':
		{
			Stack.push_back(sinl(ValueY));
			return true;
		}

		case 'S':
		{
			Stack.push_back(sinl(ValueY * (M_PI / 180.0)));  // Convert degrees to radians
			return true;
		}

		case 'c':
		{
			Stack.push_back(cosl(ValueY));
			return true;
		}

		case 'C':
		{
			Stack.push_back(cosl(ValueY * (M_PI / 180.0)));
			return true;
		}

		case 't':
		{
			Stack.push_back(tanl(ValueY));
			return true;
		}

		case 'T':
		{
			Stack.push_back(tanl(ValueY * (M_PI / 180.0)));
			return true;
		}

		case 'a':
		{
			Stack.push_back(atanl(ValueY));
			return true;
		}

		case 'A':
		{
			Stack.push_back(atanl(ValueY) * (180.0 / M_PI));  // Convert radians to degrees
			return true;
		}

		case 'i':
		{
			if (ValueY == 0.0)
            	return false;  // Divide by zero error.

			Stack.push_back(1.0 / ValueY);
			return true;
		}

		case 'f':
		{
			Stack.push_back(floorl(ValueY));
			return true;
		}

		case 'F':
		{
			Stack.push_back(ceill(ValueY));
			return true;
		}

		case 'r':
		{
			Stack.push_back(floorl(ValueY + 0.5));
			return true;
		}

		case '~':
		{
			Stack.push_back((long double) ~(__int64) ValueY); // Only valid for 64 bits
			return true;
		}

		case 'n':
		{
			Stack.push_back(-ValueY);
			return true;
		}

		case '!':
		{
			Stack.push_back(!ValueY);
			return true;
		}

	    case 'p':  //positive
		{
			Stack.push_back(fabsl(ValueY));
			return true;
		}

	    case 'e':
		{
			Stack.push_back(expl(ValueY));
			return true;
		}

	    case 'l':  //ln
		{
			Stack.push_back(logl(ValueY));
            return true;
		}

	    case 'L':  //log10
		{
			Stack.push_back(log10l(ValueY));
            return true;
		}

	    case 'q':
		{
			Stack.push_back(sqrtl(ValueY));
			return true;
		}

	    case 'Q':
		{
			Stack.push_back(ValueY * ValueY);
			return true;
		}
	}

    // Binary operators:
	if (Stack.empty())
		return false;

	long double		ValueX = Stack.back();

    Stack.pop_back();

	switch (operation)
	{
    	case 'E':  //ln
		{
			Stack.push_back(ValueX * (10 * ValueY));
			return true;
		}

		case '+':
		{
			Stack.push_back(ValueX + ValueY);
			return true;
		}

		case '-':
		{
			Stack.push_back(ValueX - ValueY);
			return true;
		}

		case '*':
		{
			Stack.push_back(ValueX * ValueY);
			return true;
		}

		case '/':
		{
			if (ValueY == 0)
            	return false;  // Divide by zero.

			Stack.push_back(ValueX / ValueY);
			return true;
		}

		case '=':
		{
			Stack.push_back(ValueX == ValueY);
			return true;
		}

		case '%':
		{
			Stack.push_back(fmodl(ValueX,ValueY));
			return true;
		}

		case 'm':
		{
			if (ValueX < ValueY)
				Stack.push_back(ValueX);
			else
				Stack.push_back(ValueY);

			return true;
		}

		case 'M':
		{
			if (ValueX > ValueY)
				Stack.push_back(ValueX);
			else
				Stack.push_back(ValueY);
                
			return true;
		}

		case '<':
		{
			Stack.push_back(ValueX < ValueY);
			return true;
		}

		case '>':
		{
			Stack.push_back(ValueX > ValueY);
			return true;
		}

		case 'g':
		{
			Stack.push_back(atan2l(ValueX, ValueY));

            if (errno == EDOM)
            {
				ErrorTrapTrace(166, FloatToStr(ValueX) + ", " + FloatToStr(ValueY), FromSystem,
                	FromObject, OrigFromObject);

                return false;
            }

			return true;
		}

		case 'G':
		{
			Stack.push_back(atan2l(ValueX, ValueY) * (180.0 / M_PI));

            if (errno == EDOM)
            {
				ErrorTrapTrace(166, FloatToStr(ValueX) + ", " + FloatToStr(ValueY), FromSystem,
                	FromObject, OrigFromObject);

                return false;
            }

			return true;
		}

		case '^':
		{
			Stack.push_back(powl(ValueX, ValueY));
			return true;
		}
        
		default:
		{
			return false;
		}
	}
}


// Display ErrorTrap/ErrorTrace messages for the Attribute Calculator.
void AttributeCalculator::ErrorTrapTrace(int MessageNumber, AnsiString Message,
	VivaSystem *FromSystem, I2adl *FromObject, I2adl *OrigFromObject)
{
	ErrorTrapTrace(MessageNumber, VStringList(Message), FromSystem, FromObject,
    	OrigFromObject);
}


// Display ErrorTrap/ErrorTrace messages for the Attribute Calculator.
void AttributeCalculator::ErrorTrapTrace(int MessageNumber, VStringList Messages,
	VivaSystem *FromSystem, I2adl *FromObject, I2adl *OrigFromObject)
{
	if (OrigFromObject == NULL)
    	OrigFromObject = FromObject;

    // De we have a system name to display?
    if (FromSystem != NULL)
    	Messages->Add("System: " + *FromSystem->Name);

    if (OrigFromObject == NULL)
        ErrorTrap(MessageNumber, Messages);
    else
        ErrorTrace(MessageNumber, OrigFromObject, Messages);
}

