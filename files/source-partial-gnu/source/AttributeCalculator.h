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

#ifndef AttributeCalculatorH
#define AttributeCalculatorH

#include <vector>
#include "VivaIO.h"

using namespace std;


class AttributeCalculator
{
public:
	vector <long double>	Stack;  // List of values previously extracted from the attribute
    								// string waiting RPN (Reverse Polish Notation) processing.

    AttributeCalculator();
    ~AttributeCalculator();

    void ConvertToNewSyntax(AnsiString &Attribute, VivaSystem *FromSystem = NULL,
    	I2adl *FromObject = NULL, I2adl *OrigFromObject = NULL);
    void CalculateAttribute(AnsiString &Attribute, VivaSystem *FromSystem = NULL,
    	I2adl *FromObject = NULL, I2adl *OrigFromObject = NULL);
    void CalculateStrippedAtt(AnsiString &Attribute, VivaSystem *FromSystem = NULL,
    	I2adl *FromObject = NULL, I2adl *OrigFromObject = NULL);
    AnsiString RPNCalculations(AnsiString Attribute, VivaSystem *FromSystem,
    	I2adl *FromObject, I2adl *OrigFromObject);
    bool Operate(char operation, VivaSystem *FromSystem, I2adl *FromObject,
    	I2adl *OrigFromObject);
    void ErrorTrapTrace(int MessageNumber, AnsiString Message, VivaSystem *FromSystem,
    	I2adl *FromObject, I2adl *OrigFromObject = NULL);
    void ErrorTrapTrace(int MessageNumber, VStringList Messages, VivaSystem *FromSystem,
    	I2adl *FromObject, I2adl *OrigFromObject = NULL);
};

#endif
