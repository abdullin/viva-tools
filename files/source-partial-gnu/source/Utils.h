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

//#include <windows.hpp>
#include <vcl.h>

#ifndef VivaUtilsH
#define VivaUtilsH

#define RESIZE_LEFT             					0x01
#define RESIZE_RIGHT            					0x02
#define RESIZE_TOP              					0x04
#define RESIZE_BOTTOM           					0x08
#define RESIZE_HORIZONTAL       					(RESIZE_LEFT | RESIZE_RIGHT)
#define RESIZE_VERTICAL         					(RESIZE_TOP | RESIZE_BOTTOM)
#define RESIZE_BOTH             					(RESIZE_HORIZONTAL | RESIZE_VERTICAL)
#define RESIZE_SHRINK           					0x10
#define RESIZE_GROW             					0x20
#define RESIZE_SHRINK_GROW      					(RESIZE_SHRINK | RESIZE_GROW)
#define RESIZE_TOLERANCE        					6

#define ATT_UNSIGNED								0x01
#define ATT_SIGNED									0x02
#define ATT_FIXED									0x04
#define ATT_FLOAT									0x08
#define ATT_COMPLEX									0X10
#define ATT_UNDETERMINED							0XF0000000

//#define B2_EXP_TO_B10_EXP		3.3246753246753246753246753246753
#define B2_EXP_TO_B10_EXP		3.32

namespace VivaUtils
{
    extern TRect			DefaultMaxRect; // Default constraints for resize operation

    AnsiString GetShortFileName(const AnsiString &FName);
    AnsiString GetFilePath(const AnsiString &_Name);
	int DoResize(TControl *Con, TShiftState& Shift, int ResizeTypeAllowed, int MinSize = 8
		, TRect MaxRect = DefaultMaxRect);
	AnsiString GetFileExtension(const AnsiString &FileName);
	bool IsValidFileName(AnsiString &FileName, bool AllowSpaces = true, bool FixFileName = true);
	bool IsValidEDIFName(AnsiString &EDIFName, bool FixEDIFName = true);
	bool IsValidFileNameChar(char Candidate);
	bool IsValidEDIFNameChar(char Candidate);
	void mcpy(unsigned char *dest, unsigned char *src, int Bytes);
	void SetMax(int &Maxi, int val);
	void SetMin(int &Mini, int val);
    void DecodeString(AnsiString& EncodedString);
	AnsiString EncodeString(AnsiString DecodedString);
	void ConvertSign(Byte *DataPtr, int CappedByteLength, int BitLength);
	AnsiString BinToText(Byte *DPtr, int ByteLength, int BitLength, int AttributeFlags);
	bool OffboundMemCopy(Byte *Dest, Byte *Source, int Bitlength, int Offbounds
		, bool Read = false);
    AnsiString HexFormat(Byte *DPtr, int BitLength);

	__int64 GetExpRange(int BitLen);
	char * GetFractionalDecimalString(Byte *Data, int BitLen);
	void MakeFractionalBinaryData(Byte *Data, int BitLen, const char *String);
	AnsiString FloatDataToString(Byte *Data, int MBitLen, int EBitLen);
	AnsiString FixedPointDataToString(Byte *Data, int ILen, int FLen);
 	void ShiftDown(Byte *Data, int ByteLen, int ShiftBy);
	void ShiftUp(Byte *Data, int ByteLen, int ShiftBy);    
	__int64 ExtractExponent(Byte *Data, int MBitLen, int EBitLen);
	void InvertAndIncrement(Byte *Data, int ByteLength);
}

using namespace VivaUtils;

#endif
