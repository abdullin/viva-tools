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

#include "Cost.h"
#include "ErrorTrap.h"

#pragma package(smart_init)


// Constructor
VCost::VCost(double InitialCosts)
{
    SetCosts(InitialCosts);
}


// Returns true if there are no parital costs.
bool VCost::IsHard()
{
    return (this != NULL &&
    		PartialCost == 0);
}


// Sets both the hard cost and the partial cost to CostValue.
void VCost::SetCosts(double CostValue)
{
    HardCost = CostValue;
    PartialCost = CostValue;
}


// Operator overloads - Compare 2 costs.
//     Costs are scored by comparing the PartialCost first and the HardCost second.
bool VCost::operator<(VCost &Cost)
{
    if (PartialCost == Cost.PartialCost)
        return (HardCost < Cost.HardCost);
    else
        return (PartialCost < Cost.PartialCost);
}


bool VCost::operator>(VCost &Cost)
{
    if (PartialCost == Cost.PartialCost)
        return (HardCost > Cost.HardCost);
    else
        return (PartialCost > Cost.PartialCost);
}


bool VCost::operator==(VCost &Cost)
{
    return (PartialCost == Cost.PartialCost &&
    		HardCost == Cost.HardCost);
}


bool VCost::operator!=(VCost &Cost)
{
    return !(*this == Cost);
}


bool VCost::operator<=(VCost &Cost)
{
    return (*this < Cost ||
    		*this == Cost);
}


bool VCost::operator>=(VCost &Cost)
{
    return (*this > Cost ||
    		*this == Cost);
}


// Add, subtract and multiply costs.
void VCost::CostAdd(VCost &Cost)
{
    HardCost    = min(HardCost    + Cost.HardCost,    (double) MAXCOST);
    PartialCost = min(PartialCost + Cost.PartialCost, (double) MAXCOST);
}


void VCost::CostSub(VCost &Cost)
{
    HardCost    = max(HardCost    - Cost.HardCost,    0.0);
    PartialCost = max(PartialCost - Cost.PartialCost, 0.0);
}


VCost VCost::CostMul(int Quantity)
{
    VCost    Result;

    Result.HardCost    = max(min(HardCost    * Quantity, (double) MAXCOST), 0.0);
    Result.PartialCost = max(min(PartialCost * Quantity, (double) MAXCOST), 0.0);

    return Result;
}

