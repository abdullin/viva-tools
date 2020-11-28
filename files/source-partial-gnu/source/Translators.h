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

#ifndef TranslatorsH
#define TranslatorsH

#include "Globals.h"

class Translator
{
public:
    
    VivaSystem    *OwningSystem;         // Parent system that this translator is tied to.
    int           Type;                  // Parent system's type code.
    AnsiString    FPGAFileName;          // Project name + parent system's full system tree
    									 //		tree name.
    AnsiString    BuildDir;              // Directory in which Xylinx files are placed.  
    char          *RunImage;             // Contains the execution input/output node data.
    int           *InputDeRefTable;      //
    int           *OutputDeRefTable;     //
    int           RunImageSize;          // Size (in bytes) of run image.  Used for memset(...).

    Translator::Translator(VivaSystem *_OwningSystem);
 
    ~Translator();
    
	void TranslatePrep();
	void Translate();
    bool CreateEdifExport(TFileStream **EdifFileStream);
    Node *GetSourceWidget(I2adl *Object, int Direction, int &BitOffset);
    void DeleteUIForms();
	void DeleteRuntimeGlobals();
	void __fastcall CreateRunImage();
	bool __fastcall AssignUIHelpers();
	void __fastcall AssignWidgetPHelper(I2adl *Object);

	// FPGA system methods.
	void __fastcall ConnectIOLinks();
	void __fastcall TrimResourceProviders();
	void PropagateTimingAttributes(I2adl *Object);
	bool CreateEdifImage(TFileStream **EdifFileStream);
	bool __fastcall CallVendorTools(AnsiString FullFPGAFileName);
	I2adl *GetProvidesResourceObject(AnsiString ResourceName, IOLink *SystemLink);
    void WriteNetList(TFileStream **EdifFileStream, bool NewStyleBoard);
    AnsiString FormatPortRef(Node *ConnectedNode, int NodeType, bool NewStyleBoard);
	bool Proxec(bool IWait, const AnsiString &FileName);
    void MakeEdifIDs();
	void InsertDelayObject(I2adlList *AssignedBehavior, I2adl *Object, Node *OutputNode);
};

#endif
