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

#ifndef ResourceManagersH
#define ResourceManagersH

#define COL_SYSTEM_NAME     0
#define COL_RESOURCE_NAME   1
#define COL_TYPE_CODE       2
#define COL_TOTAL_QUANTITY  3
#define COL_INTRINSIC_COST  4
#define COL_MAX_INFO_RATE   5
#define COL_PIN_LOCATION    6

class ResourceManager
{
public:

    VivaSystem      *OwningSystem;			// Parent Viva system.
    ResourceList    *AvailableResources;	// All of the resources in our system.  Built by
											//     looking the prototype name up in the Resource
											//     Editor.
    AnsiString      ResourcePrototypeName;	// Prototype name from the Resource Editor.
    Resource        *AtomicTransport;		// Atomic transport resource.  Also in
											//     AvailableResources.
    int				TotalObjects;			// Number of objects in the system AssignedBehavior.
											//     Saved because FPGA sys delete their objects.
    
    ResourceManager(VivaSystem *_OwningSystem, AnsiString _ResourcePrototypeName);
    ~ResourceManager();

    virtual void __fastcall Reset();
    Resource *__fastcall GetResource(I2adl *Object);
    void GetResourceRowNumbers(int &FirstRow, int &LastRow);
    void LoadAvailableResources();
};

#endif
