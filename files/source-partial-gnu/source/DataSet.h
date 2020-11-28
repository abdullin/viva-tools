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

#ifndef DataSetH
#define DataSetH

class DataSet
{
public:
    AnsiString           Name;            // Name that uniquely identifies the DataSet
    AnsiString           TreeGroup;       // Tree group in which the data set is located
    int                  BitLength;       // Total of all BitLengths from child data sets
    int                  ByteLength;      // Number of bytes that this must occupy
    int                  ColorCode;       // 24-bit RGB representation of color
    unsigned int         AttributeFlags;  // Bit flags indicating dataset attributes
    bool         		 Used;       	  // Is or is not used in current project
    USHORT               VarType;         // Variant Type for COM
    DataSetList          *ChildDataSets;  // List of child data sets from which this DS is built
    DataSet              *PointsToDS;     // The data set that a pointer data set points to
    I2adl                *Exposer;        // The Dataset's Exposer I2adl
    I2adl                *Collector;      // The Dataset's Collector I2adl
    SystemGeneratedEnum  SystemGenerated; // Identifies automatically created data sets

    DataSet(AnsiString _Name, AnsiString _TreeGroup, int _ColorCode, unsigned int
        _AttributeFlags, SystemGeneratedEnum _SystemGenerated);
    ~DataSet();

    bool IsUsed(bool ReportProblems);
    bool IsParentOf(DataSet *DataSetBeingChecked);
    void SetChildIsVisited();
    unsigned long CalcBitLength();
    void __fastcall BuildExposerAndCollector();
    void TextToBin(AnsiString &Text, Byte *DPtr, I2adl *Object = NULL);
    AnsiString __fastcall FormatData(Byte *DPtr, bool HexFormatting = false);
    bool IsBinaryString();
};

char ListFormat(Node *InputNode, AnsiString &AnsiOut, char ExitDelimiter, bool HexFormatting);

#endif
