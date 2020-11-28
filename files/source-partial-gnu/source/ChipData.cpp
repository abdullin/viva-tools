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

#include <fcntl.h>
#include <io.h>
#include <winioctl.h>

#include "Globals.h"
#include "asm.h"
#include "ChipData.h"
#include "ErrorTrap.h"
#include "Hcioctl.h"
#include "Translators.h"
#include "VivaSystem.h"
#include "I2adlList.h"

#pragma package(smart_init)


ChipData::ChipData(char *FileName)
{
    DataBuffer = NULL;
    Length = 0;
    Error = 0;

    // The following fixes an "error" problem in the Memory Proof program.
    if (FileName == NULL)
        return;
    
    int    handle  = open(FileName, O_RDONLY | O_BINARY);

    if (handle == -1)
        return;

    unsigned char    Data;
    int              Result;
    int              FileLength = filelength(handle);
    int              Start = 0;

    while (true)
    {
        Result = read(handle, (void *)& Data, 1);

        if (Result == -1)
        {
            Error = 1;
            ErrorTrap(31, "ChipData::ChipData");
            break;
        }

        if (Data == 0xff)
        {
            Length = FileLength+1 - Start;
            DataBuffer = (char *) malloc(Length);
            DataBuffer[0] = Data;
            read(handle, & DataBuffer[1], Length - 1);
            break;
        }

        Start++;
    }

    close(handle);
}


ChipData::~ChipData()
{
    if (DataBuffer != NULL)
        free((void *) DataBuffer);
}


char *ChipData::GetDataBuffer()
{
    return DataBuffer;
}


int ChipData::GetLength()
{
    return Length;
}


// Temporarily reprogram the board.  Later build initialization dependancy
//     structure and do complex initialization.
void  ProgramPensa()
{
    VivaSystem    *BoardSystem = BaseSystem->GetSystem("ProgramInfo", "Board1");

    if (BoardSystem == NULL)
        return;

    // Do we need to program up the chips?
    AnsiString	ProgramChips = BoardSystem->GetAttribute("ProgramChips");

    if (ProgramChips.Length() > 0 &&
    	ProgramChips.SubString(1,1).UpperCase() == "F")
        	return;

    // Set the status bar message.
    SetStatusCaption("Step 5 of 5; Programming GateWare card...", true);

    // Get the control port numbers for the two kinds of FPGA chips.
    WORD	XPointControlPort    = (WORD) VStrToInt(BoardSystem->GetAttribute("PPort"));
    WORD	ChipArrayControlPort = (WORD) VStrToInt(BoardSystem->GetAttribute("APort"));

    if (ChipArrayControlPort == 0 || XPointControlPort == 0)
    {
        // This identifies new style boards because they do not have port numbers.
        ProgramNewStyleBoard(BoardSystem, strNull, 0);

        return;
    }

    program    *ChipProgrammer = new program;
    ChipProgrammer->SetPorts(XPointControlPort, ChipArrayControlPort);

    for (int i = 0; i < MAX_CHIP_COUNT; i++)
    {
        VivaSystem    *ChipSystem = BaseSystem->GetSystem("ChipIndex", IntToStr(i));
        char          *BitMapImage = (ChipSystem == NULL) ? NULL
                                                          : ChipSystem->SysTranslate->RunImage;

        ChipProgrammer->SetFormat(i, BitMapImage);
    }

    ChipProgrammer->DoConfigure(1);

    delete ChipProgrammer;
}


// Program the new style board by sending the bit map file(s) through the device driver to the
//	   intended chip.
//
//	   BoardSystem is either the root FPGA system (normally HC) or NULL.  If it is NULL, then
//	   the ".bit" file to be loaded is passed in BitFileName and the chips number(s) to be
//     programmed are passed in ChipSelect as a bit mask.
//
//	   To do - When all customers are using the new style board, remove the ChipData module.
void ProgramNewStyleBoard(VivaSystem *BoardSystem, AnsiString BitFileName, int ChipSelect)
{
	// Don't open the device driver until it is needed (avoid error #124).
    HANDLE  DeviceHandle = INVALID_HANDLE_VALUE;

    // Look at each chip to see which ones need to be programed.
    for (int ChipNumber = 0; ChipNumber < MAX_CHIP_COUNT; ChipNumber++)
    {
    	// Do we get the ".bit" file name from the system or was it passed in?
    	if (BoardSystem != NULL)
        {
            VivaSystem  *ChipSystem = BoardSystem->GetSystem("ChipIndex", IntToStr(ChipNumber));

            if (ChipSystem == NULL)
            	continue;

            // For FPGA systems the RunImage contains the bit map file name.
            BitFileName = ChipSystem->SysTranslate->RunImage;

            // The following reduces the number of errors reported in the Memory Proof program.
            if (BitFileName == NULL)
                continue;
        }

        int     FileHandle = open(BitFileName.c_str(), O_RDONLY | O_BINARY);

        if (FileHandle == -1)
        {
	    	if (BoardSystem == NULL)
            {
            	// Could not open the ".bit" file.
	        	ErrorTrap(135, BitFileName);
                break;
            }
            else
            {
	            continue;
            }
        }

        // Open the device driver for the new style board the first time it is needed.
        //     "1" means the first device driver (Board1).
		if (DeviceHandle == INVALID_HANDLE_VALUE)
        {
            DeviceHandle = CreateFile(APP_SYMBOLIC_NAME "1", GENERIC_READ | GENERIC_WRITE, 0,
            	NULL, OPEN_EXISTING, FILE_FLAG_NO_BUFFERING, NULL);

            if (DeviceHandle == INVALID_HANDLE_VALUE)
            {
                // Could not open the device driver for the new style board.
                ErrorTrap(124);

                RunAfterCompile = false;
		        close(FileHandle);
                
                return;
            }
        }

        // Start out reading the bit map file one character at a time.
        while (true)
        {
            HC_PROGRAM_STRUCT   BitMapData;
            unsigned long       BytesWritten;
            int                 CharsRead = read(FileHandle, BitMapData.progData, 1);

            if (CharsRead <= 0)
            {
                // Bit map file is missing the start-of-data indicator.
                ErrorTrap(31, BitFileName);
                break;
            }

            // Did we find the start-of-data indicator.
            if (BitMapData.progData[0] != 0xff)
                continue;

            // Do we get the chip number from the system or was a chip number(s) passed in?
            //     Select our chip by putting a "1" in the proper bit location.
	    	if (BoardSystem == NULL)
	            BitMapData.bmChipSelect = (BM_CHIP_SELECT) ChipSelect;
            else
	            BitMapData.bmChipSelect = (BM_CHIP_SELECT) (1 << ChipNumber);

            // Initilize the FPGA chip.
            if (!DeviceIoControl(DeviceHandle, HCIOCTL_PROG_INIT, &BitMapData.bmChipSelect,
                sizeof(BM_CHIP_SELECT), NULL, 0, &BytesWritten, NULL))
            {
                // Could not initilize the new style board.
                ErrorTrap(125, BitFileName);

                break;
            }

            // Write out the first byte of data and then the rest of the file 16K at a time.
            while (CharsRead > 0)
            {
                if (!DeviceIoControl(DeviceHandle, HCIOCTL_PROG_FPGA, &BitMapData,
                    sizeof(BM_CHIP_SELECT) + CharsRead, NULL, 0, &BytesWritten, NULL))
                {
                    // Could not send the bit map file to the new style board.
                    ErrorTrap(126, BitFileName);

                    break;
                }

                CharsRead = read(FileHandle, BitMapData.progData, MAX_DATA_LENGTH);
            }

            // Finalize the FPGA chip.
            if (!DeviceIoControl(DeviceHandle, HCIOCTL_PROG_END, &BitMapData.bmChipSelect,
                sizeof(BM_CHIP_SELECT), NULL, 0, &BytesWritten, NULL))
            {
                // Could not finalize the new style board.
                ErrorTrap(127, BitFileName);
            }

            break;
        }

        close(FileHandle);

        // If a list of chip numbers was passed in, then we only need one pass.
        if (BoardSystem == NULL)
        	break;

        SetProgPos((ChipNumber + 1) * 10);
    }

	if (DeviceHandle != INVALID_HANDLE_VALUE)
	    CloseHandle(DeviceHandle);

    SetProgPos(0);
}
