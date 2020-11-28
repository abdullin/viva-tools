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

#ifndef ResourceH
#define ResourceH


class Resource
{
public:

    ResourceManager       *Owner;          // Parent Resource Manager
    ResourceString        *ResourceName;   // Name of the resource
    ResourceTypeString    *ResourceType;   // Type code of this resource

private:

    ResourceList    *SharedResources;
    // The resources whose TotalQuantity, QuantityUsed, IntrinsicCost, CurrentCost,
    //     MaxInfoRate, SetupLatency and PinLocation are shared by the resource.  Test
    //     (SharedResources->Items[0] == this) to determine non shared resources.  Shared
    //     resources are indicated by a blank in the IntrinsicCost column.  A list of
    //     1-based index numbers into the AvaliableResource list or resource names is
    //     entered in the TotalQuantity column.  Supports multilple levels of indirection.

    IntegerList     *SharedQuantities;	// Quantity multiplyer factor for each Shared Resource
    int             TotalQuantity;		// Total number of resources avaliable
    int             QuantityUsed;		// Number of resources that have been used
    double          IntrinsicCost;		// Base cost factor for the resource
    VCost			CurrentCost;		// The current cost for this resource based on QtyUsed
    int             MaxInfoRate;		// Maximum information rate the resource can handle
    int             SetupLatency;		// Number of TDM resource units required to swap in or
										//     task switch each time a TDM resource is requried
    AnsiString      PinLocation;		// FPGA Pin/Pad location (if a Pin/Pad resource)

public:

    Resource(ResourceManager *_Owner, int RowNumber);
    ~Resource();

    bool HasBeenUsed();
    bool HasBeenUsedUp(bool SkipSingleResources);
    void GetCurrentCost(VCost *ResourceCost);
    int GetMaxInfoRate(I2adl *Object);
    AnsiString *GetPinLocation();
    void CommitReleaseOrCost(ResourceEnum ResourceMode, I2adl *Object, VCost *ObjectCost);
    void DisplayUsage(int RowNumber);
};

#endif
