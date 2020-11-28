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
#include <algorithm>     // Required for min() function
#pragma hdrstop

#include "stdio.h"
#include "Globals.h"
#include "I2adl.h"
#include "DataSet.h"
#include "Asm.h"
#include "I2adlList.h"
#include "Main.h"
#include "Node.h"
#include "Project.h"
#include "ErrorTrap.h"
#include "VivaSystem.h"
#include "I2adlEditor.h"

#pragma package(smart_init)


// Recursive ChildDataSets are automatically loaded (Bit and Variant).
DataSet::DataSet(AnsiString _Name, AnsiString _TreeGroup, int _ColorCode, unsigned int
    _AttributeFlags, SystemGeneratedEnum _SystemGenerated)
{
    Name = _Name;
    TreeGroup = _TreeGroup;
    BitLength = 0;
    ByteLength = 0;
    ColorCode = _ColorCode;
    VarType = VT_NULL;
    AttributeFlags = _AttributeFlags;
    Used = false;
    ChildDataSets = new DataSetList(sizeof(DataSet));
    PointsToDS = NULL;
    Exposer = NULL;
    Collector = NULL;
    SystemGenerated = _SystemGenerated;

    // Bit, NULL and Variant child data sets are recursive.
    if (Name == SGDataSetNames[sdsBit])
    {
        ChildDataSets->Add(this);
    }
    else if (Name == SGDataSetNames[sdsNULL])
    {
        ChildDataSets->Add(this);
    }
    else if (Name == SGDataSetNames[sdsVariant])
    {
        ChildDataSets->Add(this);
        ChildDataSets->Add(this);
    }

    // Add the data set into the master data set list.
    ProjectDataSets->Add(this);
}


// Desctructor
DataSet::~DataSet()
{
    delete ChildDataSets;
}


// Returns true if the data set has been used anywhere in the project.
bool DataSet::IsUsed(bool ReportProblems)
{
    // Has the data set been used on a project object/behavior?
    I2adl    *ParentObject = NULL;
    I2adl    *Object = ProjectObjects->IsDataSetUsed(this, true, &ParentObject);

    if (Object != NULL)
    {
    	if (ReportProblems)
    	    UsageTrap(85, Object, ParentObject);

        return true;
    }

    // Has the data set been used on a Wip sheet?
    for (int i = 0; i < MyProject->Wip->Count; i++)
    {
        I2adl	*WipSheet = (*MyProject->Wip)[i];

        Object = WipSheet->Behavior->IsDataSetUsed(this, false, &ParentObject);

        if (Object != NULL)
        {
	    	if (ReportProblems)
	            UsageTrap(85, Object, ParentObject, " in Wip sheet " + MainForm->WipTree->Items->
                	Item[i + 1]->Text);

            return true;
        }
    }

    // Is the data set in the current compile information?
    Object = BaseSystem->IsDataSetUsed(this, &ParentObject);

    if (Object != NULL)
    {
    	if (ReportProblems)
    	    UsageTrap(85, Object, ParentObject, " in the current compile information");

        return true;
    }

    return false;
}


// Returns true if "this" data set is a parent of the DataSetBeingChecked.
bool DataSet::IsParentOf(DataSet *DataSetBeingChecked)
{
	// If the data set hasn't been created yet, then it doesn't have a parent.
	if (DataSetBeingChecked == NULL)
    	return false;

	// Bit, Null and Variant are the terminal leaf data sets.
    if (this == BitDataSet ||
    	this == NULLDataSet ||
    	this == VariantDataSet)
    {
		return false;
    }

    // If this is the data set being checked, then we have a circular reference.
    if (this == DataSetBeingChecked)
    	return true;

    for (int i = 0; i < ChildDataSets->Count; i++)
    {
    	DataSet		*ChildDataSet = (DataSet *) ChildDataSets->Items[i];

        if (ChildDataSet->IsParentOf(DataSetBeingChecked))
        	return true;
    }

    return false;
}


// Recursive routine to set the Used flag on all child data sets.
void DataSet::SetChildIsVisited()
{
    for (int i = 0; i < ChildDataSets->Count; i++)
    {
        DataSet    *ChildDataSet = (DataSet *) ChildDataSets->Items[i];

        if (!ChildDataSet->Used)
        {
            ChildDataSet->Used = true;
            ChildDataSet->SetChildIsVisited();
        }
    }
}


// Calculate BitLength (total of all child DataSet BitLengths).
unsigned long DataSet::CalcBitLength()
{
    if (this == BitDataSet)
    {
        BitLength = 1;
        ByteLength = 1;
    }
    else if (this == NULLDataSet)
    {
        BitLength = 0;
        ByteLength = 0;
    }
    else if (this == VariantDataSet)
    {
        // Treat Variants as amorphous "blocks" of data.
        BitLength  = 0x10000;
        ByteLength = 0x10000;
    }
    else
    {
    	// This pattern data set code fixes a stack overflow on LSB1024.
        int		PatternSize = 0;

        if (GetPatternType(Name, PatternSize) == sdsNULL)
        {
            BitLength = 0;

            for (int i = 0; i < ChildDataSets->Count; i++)
                BitLength += ((DataSet *) ChildDataSets->Items[i])->CalcBitLength();
        }
        else
        {
            BitLength = PatternSize;
        }

        ByteLength = (BitLength + 7) >>3;
    }

    return BitLength;
}


// Build the data set exposer and collector.
void __fastcall DataSet::BuildExposerAndCollector()
{
    // Now that the child data sets have all been added, calcualte the bit length.
    CalcBitLength(); 

    // Skip the Bit and NULL data sets.
    if (this == BitDataSet  ||  this == NULLDataSet)
        return;

    // Have the exposer and collector already been built?
    if (Exposer != NULL)
        return;

    I2adl    *NewIn = new I2adl(Name + *NodestrIn,1,2);
    I2adl    *NewOut = new I2adl(Name + *NodestrOut,2,1);

    NewIn->Height = ChildDataSets->Count * ViewScale * 3 + ViewScale * 2;
    NewOut->Height = NewIn->Height;
    NewIn->Width = ViewScale * 11.41;
    NewOut->Width = ViewScale * 11.41;
    NewIn->IsPrimitive = true;
    NewOut->IsPrimitive = true;
    NewIn->ExposerCode = EXPOSER_IN;
    NewOut->ExposerCode = EXPOSER_OUT;

    NewIn->Inputs->Add(new Node(*NodestrIn, NewIn, this));
    NewOut->Outputs->Add(new Node(*NodestrOut, NewOut, this));

    for (int i = 0; i < ChildDataSets->Count; i++)
    {
        DataSet    *ChildDataSet = (DataSet *) ChildDataSets->Items[i];

        AddNode(&NewIn->Outputs, new Node(*NodestrOut + IntToStr(i + 1), NewIn, ChildDataSet));
        AddNode(&NewOut->Inputs, new Node(*NodestrIn + IntToStr(i + 1), NewOut, ChildDataSet));
    }

    NewIn->Conformalize();
    NewOut->Conformalize();

    ProjectObjects->Add(NewIn);
    ProjectObjects->Add(NewOut);

    Exposer = NewIn;
    Collector = NewOut;
}


// Converts text to binary data, appropriate for this Dataset, storing the result in DPtr.
void DataSet::TextToBin(AnsiString &Text, Byte *DPtr, I2adl *Object)
{
    int			bytelen = (BitLength + 7) >> 3;
    __int64		upo;
    int			len = Text.Length();

    if (len == 0)
    {
        memset(DPtr, 0, bytelen);
        return;
    }

    // Find out if we should issue the dummy message for invalid entry:
    bool	IssueMessage = !(Text.Trim() == "-");
    Byte	*tstr = (Byte *) Text.c_str();

    while (true) // break if not hex.
    {
        if (len < 2)
            break;

        if (tstr[0] != '0')
            break;

        if ((tstr[1] & 0x5f) != 'X')
            break;

        unsigned char	*instr = &tstr[2];
    	AnsiString		OrigText((char *) tstr);  // Copy Text's string buffer.

        len -= 2;

        if (len & 1) // handle the odd case
        {
            instr--;
            instr[0] = '0';
            len++;
        }

        int		id = 0;
        int		val = 0;

        while (--len >= 0)
        {
            int		ix = instr[len];

            if (ix <= '9')
                ix -= '0'; // Number
            else	// Convert hex character to upper case.
                ix = (ix & 0x5f) - 55;

            if (ix < 0 ||
            	ix > 15)
            {
                if (Object == NULL)
                    ErrorTrap(4016, OrigText);
                else
                    ErrorTrace(4016, Object, OrigText);

                break;
            }
            else if (len & 1)
            {
                val = ix;
            }
            else
            {
                DPtr[id] = char((ix << 4) + val);
                id++;

                if (id >= bytelen)
                    break;
            }
        }

        while (id < bytelen)
        {
            DPtr[id] = 0;
            id++;
        }

		// If this dataset doesn't fall on a byte boundary, then "and out" the upper bits.
		if (BitLength % 8)
	        DPtr[id - 1] &= byte((1 << (BitLength % 8)) - 1);

        return;
	}

    if (tstr[0] == '?' || tstr[1] == '?')
    {    // Random number
        upo = IRand();

        if (BitLength > 31)
            upo <<= BitLength - 31;
        else
            upo &= (1 << BitLength) - 1;
    }
    else if (AttributeFlags & ATT_FIXED)
    {
		memset(DPtr, 0, BitLength / 8 + bool(BitLength % 8));

		// First, separate out the integer and fractional parts of the string.
    	int				dotpos = Text.Pos(strPeriod);
        int				len = Text.Length();

        if (dotpos <= 0)
			dotpos = len + 1;

        AnsiString		idatastr = Text.SubString(1, dotpos - 1);
        AnsiString		fdatastr = Text.SubString(dotpos + 1, len - dotpos);
        DataSet			*dset1 = (DataSet *) ChildDataSets->Items[0];
        DataSet			*dset2 = (DataSet *) ChildDataSets->Items[1];
        int				fbitlen = min(fdatastr.Length(), dset1->BitLength);
		AnsiString		trim = Text.Trim();
        bool			isnegative = (trim.Length() >= 1 && trim[1] == '-');

		// Now, convert the integer string part to binary data into the data pointer.
		dset2->TextToBin(idatastr, DPtr, NULL);

		// If we are dealing with a negative number, and there is fractional data, then we have
        //		to represent the integer part as being one lower than it actually is.  This is
        //		so that negative numbers less than zero can be represented, such as -0.5
        if (isnegative)
        {
	        bool			zerofraction = true;

            for (int i = 0; i < fdatastr.Length(); i++)
            {
                char 	c = fdatastr[i + 1];

                if (c >= '1' && c <= '9')
                {
                    zerofraction = false;
                    break;
                }
            }

			if (!zerofraction)
            {
                for (int i = 0; i < ByteLength; i++)
                {
                    DPtr[i]--;

                    if (DPtr[i] == 0xFF)
                        continue;
                    else
                        break;
                }
            }
        }

		// Now that we have the integer part, we have to move it up in the data pointer to make
        //		room in the data pointer for the fractional part.
		memmove(DPtr + dset1->BitLength / 8, DPtr, (dset2->BitLength + 7) / 8);
        memset(DPtr, 0, dset1->BitLength / 8);

        ShiftUp(DPtr, ByteLength, dset1->BitLength % 8);

		// Extract data for fractional part into the data pointer.
        char	*fchars = new char[dset1->BitLength];

		memset(fchars, 0, dset1->BitLength);
		memcpy(fchars, fdatastr.c_str(), fbitlen);

	    for (int i = 0; i < fbitlen; i++)
	    	fchars[i] -= '0';

        Byte	*temp = new Byte[dset1->ByteLength];

        memset(temp, 0, dset1->ByteLength);

        MakeFractionalBinaryData(temp, dset1->BitLength, fchars);

        if (isnegative)
	        InvertAndIncrement(temp, dset1->ByteLength);

		if (dset1->BitLength % 8)
	        temp[dset1->ByteLength - 1] &= Byte((1 << (dset1->BitLength % 8)) - 1);

		for (int i = 0; i < dset1->ByteLength; i++)
        	DPtr[i] |= temp[i];

		delete[] temp;
        delete[] fchars;

		return;
    }
    else if (AttributeFlags & ATT_FLOAT)
    {
        double		rov = 0;

        try
        {
            rov = StrToFloat(Text);
        }
        catch(...)
        {
            if (IssueMessage)
                ErrorTrap(4012, Text);
        }

        __int64		bits = *(__int64 *) &rov;
        __int64		lowbits = ((((__int64) 1) << 52 ) - 1) & bits;
        DataSet		*DSetL = (DataSet *) ChildDataSets->Items[0];
        int			lowbitlength = DSetL->BitLength;

        if (lowbitlength < 52)
            lowbits >>= (52 - lowbitlength);

        int		highbits = (int) ((bits & 0x7fffffffffffffff) >> 52);
        int		highbitlength = BitLength - lowbitlength;
	    int		bias = 1 << (highbitlength - 2);

        highbits += bias - 1024;

        if (highbits > 0)
            lowbits += highbits << lowbitlength;

        if (bits < 0)
            lowbits += 1 << (BitLength - 1);

        if (BitLength > 32)
            upo = *(__int64 *) &rov;
        else
            upo = lowbits;
    }
    else
    {
        int     		len         = Text.Length();
        AnsiString		copystring(Text.Trim());
        char			*copy       = copystring.c_str();
        char			*currchar   = copy;
        int				bitpos      = 0;
        BYTE			*dest       = DPtr;
        int				current;
        int				carry       = 0;
        int				loopend     = BitLength;
        bool			negative    = false;

        *dest = 0;

        if (*currchar == '-')
        {
            negative = true;
            copy = ++currchar;
            len--;
        }

        for (int i = 0; i < len; i++)
        {
			if (*currchar >= '0' && *currchar <= '9')
            {
                *currchar -= '0';
            }
            else
            {
            	// Cannot put a non-numeric character into a signed/unsigned data set.
                if (IssueMessage)
                {
                    if (Object == NULL)
                        ErrorTrap(181, Text);
                    else
                        ErrorTrace(181, Object, Text);
                }

                // Try to recover by discarding the rest of the string.
                if (i > 0)
                {
                	len = i;
                }
                else
                {
                	len = 1;
	                *currchar = 0;
                }

                break;
            }

            currchar++;
        }

        for (int h = 0; h < loopend; h++)
        {
            currchar = copy;

            for (int i = 0; i < len; i++)
            {
                current = carry * 10 + *currchar;
                carry = current % 2;
                *currchar = (BYTE) (current >> 1); // [>> 1] is slightly faster than [/ 2].
                currchar++;
            }

            if (bitpos > 7)
            {
                dest++;
                *dest = 0;
                bitpos = 0;
            }

            *dest |= (BYTE) (carry << bitpos);
            bitpos++;
            carry = 0;
        }

        if (negative)
            ConvertSign(DPtr, ByteLength, BitLength);

        return;
    }

    // Extend numbers > 32 bits.
    while (bytelen > 8)  // break when constant validated
    {
        char filler = (upo >= 0) ? (char) 0
                                 : (char) -1;

        for (int j = 8; j < bytelen; j++)
            DPtr[j] = filler;

        bytelen = 8;

        break;
    }

    memcpy(DPtr, &upo, bytelen);

    return;
}


// Resolve List data signal to an output signal of a non-transport object.
//     Trace through IOLinks.
Node *SignalSourceNode(Node *InputNode)
{
    while (true)
    {
        if (InputNode == NULL)
            return NULL;

        I2adl    *SourceObject = InputNode->Owner;

        if (SourceObject->NameCode != NAME_TRANSPORT)
        {
            if (SourceObject->NameCode != NAME_INPUT ||
                SourceObject->SourceSinkLink == NULL ||
                SourceObject->SourceSinkLink->ActiveNode == NULL)
            {
                return InputNode;
            }

            SourceObject = SourceObject->SourceSinkLink->CompositeTransport;
        }

        if ((InputNode = SourceObject->Inputs->Items[0]->ConnectedTo) == NULL)
        {
            // Try to trace throught the IOLink
            if (SourceObject->SourceSinkLink == NULL)
                return NULL;

            InputNode = SourceObject->SourceSinkLink->ActiveNode;

            if (InputNode == NULL)
                return NULL;

            InputNode = InputNode->ConnectedTo;
        }
    }
}


// Ascii format of List dataset output.  Pack small items (bitlength <= 4)
char ListFormat(Node *InputNode, AnsiString &AnsiOut, char ExitDelimiter, bool HexFormatting)
{
    // Find the Source Object generating the data..It must be an exposer out.
    InputNode = SignalSourceNode(InputNode);

    if (InputNode == NULL)
    {
        AnsiOut = AnsiOut + "0";
        return 0;
    }

    I2adl	*SourceObject = InputNode->Owner;

    if (SourceObject->ExposerCode != EXPOSER_OUT)
    {
        AnsiOut = AnsiOut + "0";
        return 0;
    }

    for (int i = SourceObject->Inputs->Count - 1; i >= 0; i--)
    {
        InputNode = SignalSourceNode(SourceObject->Inputs->Items[i]->ConnectedTo);

        if (InputNode == NULL)
            continue;

        DataSet *DSet = InputNode->DSet;
        
        if (DSet == ListDataSet)
        {
            ExitDelimiter = ListFormat(InputNode, AnsiOut, ExitDelimiter, HexFormatting);
            continue;
        }

        char	PriorDelimiter = ExitDelimiter;

        if (DSet->BitLength > 4)
            ExitDelimiter = ',';
        else
            ExitDelimiter = 0;

        if (PriorDelimiter == '(')
        	;
        else if (PriorDelimiter | ExitDelimiter)
            AnsiOut += ",";

        if (ExitDelimiter == 0)
        {   // 4 bits or less .. pack output, do in hex,
            char	chr = (char) ((*InputNode->DataPtr) + '0');

            if (chr > '9')
               chr += (char) ('A' - '0' - 10);

            AnsiOut += AnsiString(chr);
        }
        else
        {
            AnsiOut += DSet->FormatData(InputNode->DataPtr, HexFormatting);
        }
    }

    return ExitDelimiter;
}


AnsiString __fastcall DataSet::FormatData(Byte *DPtr, bool HexFormatting)
{
    if (this == ListDataSet)
        return strNull;

    int		len = BitLength;

    if (HexFormatting)
    	return HexFormat(DPtr, len);

    unsigned    DSetAtt = AttributeFlags;

    if (DSetAtt & ATT_FLOAT)
    {
        if (BitLength > 32)
        {
            try
            {
				return FloatToStr(*(double *) DPtr);
            }
            catch(...)
            {
            	ErrorTrap(4077);
            	return 0;
            }
        }

        double      fpo             = 1;
        __int64     *ifpo           = (__int64 *) &fpo;
        DataSet     *DSetL          = (DataSet *) ChildDataSets->Items[0];
        int         lowbitlength    = DSetL->BitLength;
        __int64     lowbits         = *(__int64 *) DPtr;

        if (lowbitlength < 52)
            lowbits <<= (52 - lowbitlength);

        lowbits &= (((__int64) 1) << 52) - 1;

        int		highbitlength = len - lowbitlength;
        int		highbits = *(int *) &DPtr[lowbitlength >> 3];

        highbits >>= (lowbitlength & 7);

        int		bias = 1 << (highbitlength - 2);
        int		signmask = bias + bias;
        int		esigned = highbits & signmask;

        highbits &= signmask - 1;

        if (highbits == 0)
            return AnsiString(" 0");

        highbits += 1024 - bias;

        __int64		highset = ((__int64) highbits) << 52;

        lowbits += highset;

        if (esigned)
            lowbits |= 0x8000000000000000;

        *ifpo = lowbits;

        int		sigdigits = lowbitlength;
        int		lsig = highbits - 1024;
        int		rsig = sigdigits - 3 - lsig;

        if (lsig <= sigdigits && rsig <= 25)
            return FixData(fpo, max(rsig, 0), sigdigits);

        return FixData(fpo, lowbitlength, lowbitlength + 7, 'e');
    }
    else if (DSetAtt & ATT_FIXED)
    {
    	DataSet			*ds1 = (DataSet *) ChildDataSets->Items[0];
    	DataSet			*ds2 = (DataSet *) ChildDataSets->Items[1];

        return FixedPointDataToString(DPtr, ds2->BitLength, ds1->BitLength);
    }

    return BinToText(DPtr, ByteLength, BitLength, AttributeFlags);
}


bool DataSet::IsBinaryString()
{
    return (PointsToDS == SGDataSets[sdsWideChar]);
}


