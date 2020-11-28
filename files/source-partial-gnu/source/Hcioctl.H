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

// Maximum number of FPGA chips that can be on a board.
#define MAX_CHIP_COUNT  10

// Symbolic name used by the application to access the device driver
// Concatenate "1" to the name to form the name of the first device
#define APP_SYMBOLIC_NAME		"\\\\.\\HC62_"

// Symbolic link name given by the device driver
// Concatenate "1" to the name to form the name of the first device
#define DRVR_SYMBOLIC_NAME		"\\??\\HC62_"

// Internal Device Name
// Concatenate "0" to the name to form the name of the first device
#define DRVR_INTERNAL_NAME		"\\Device\\HC62_"

#define HCIOCTL_PROG_FPGA		CTL_CODE(FILE_DEVICE_UNKNOWN, 0x800, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define HCIOCTL_PROG_INIT		CTL_CODE(FILE_DEVICE_UNKNOWN, 0x801, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define HCIOCTL_PROG_END		CTL_CODE(FILE_DEVICE_UNKNOWN, 0x802, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define HCIOCTL_WRITE			CTL_CODE(FILE_DEVICE_UNKNOWN, 0x803, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define HCIOCTL_READ			CTL_CODE(FILE_DEVICE_UNKNOWN, 0x804, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define HCIOCTL_BLOCK_WRITE		CTL_CODE(FILE_DEVICE_UNKNOWN, 0x805, METHOD_OUT_DIRECT, FILE_ANY_ACCESS)
#define HCIOCTL_BLOCK_READ		CTL_CODE(FILE_DEVICE_UNKNOWN, 0x806, METHOD_IN_DIRECT, FILE_ANY_ACCESS)
#define HCIOCTL_CHIP_SELECT		CTL_CODE(FILE_DEVICE_UNKNOWN, 0x807, METHOD_BUFFERED, FILE_ANY_ACCESS)

// HCIOCTL_PROG_INIT and HCIOCTL_PROG_END both require a chip select as the single entry in the 
//		input buffer.  This is a WORD with a corresponding bit set for each FPGA to receive the
//		command.  FPGA0 is the LSB.
typedef WORD BM_CHIP_SELECT;


// structure definition for programming the FPGAs
// Note:  The input buffer length passed into DeviceIOControl must include the length of the actual
//		data in the progData buffer plus 2 for the size of the bmChipSelect element.
#define MAX_DATA_LENGTH	16384
typedef struct
{
	BM_CHIP_SELECT	bmChipSelect;		// Set bit to 1 to program the corresponding FPGA.  FPGA0 is the LSB
	BYTE	progData[MAX_DATA_LENGTH];	// buffer containing program data for the FPGA
}	HC_PROGRAM_STRUCT;


// The following structure defines the buffer to be passed into the HC62 driver for writing.
// Note:  The input buffer length passed into DeviceIOControl must be sizeof(HC_WRITE_STRUCT.
typedef struct
{
	WORD	bmChipSelect;					// Set bit to 1 to program the corresponding FPGA.  FPGA0 is the LSB
	WORD	address;							// lower 5 bits is the address.  A WORD is used for efficiency
	ULONG	data;								// send data to FPGAs
}	HC_WRITE_STRUCT;


// The following structure defines the buffer to be passed into the HC62 driver for reading
//		Note:  Only one FPGA can be selected for reading at a time.  If multiple are selected
//		for reading, the results are undefined.
typedef struct
{
	WORD	bmChipSelect;					// Set bit to 1 to program the corresponding FPGA.  FPGA0 is the LSB
	WORD	address;							// lower 5 bits is the address.  A WORD is used for efficiency
}	HC_READ_STRUCT;


// The following structure defines the buffer to be passed into the HC62 driver for block reads
//	Note:  Only one FPGA can be selected for reading at a time.  If multiple are selected
//		for reading, the results are undefined.
typedef struct
{
	WORD	bmChipSelect;					// Set bit to 1 to program the corresponding FPGA.  FPGA0 is the LSB
	WORD	address;							// address from which to start read
}	HC_BLOCKREAD_STRUCT;


// The following structure defines the buffer to be passed into the HC62 driver for writing.
// Note:  The input buffer length passed into DeviceIOControl must be sizeof(HC_WRITE_STRUCT) +
//		the length of the data to be written - 4 (length of data[1])..
typedef struct
{
	WORD	bmChipSelect;					// Set bit to 1 to program the corresponding FPGA.  FPGA0 is the LSB
	WORD	address;							// address at which to start write
	ULONG	data[4096];						// 16Kbyte block containing data for targeted FPGAs
}	HC_BLOCKWRITE_STRUCT;


