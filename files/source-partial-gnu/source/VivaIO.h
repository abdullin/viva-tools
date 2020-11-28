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

#ifndef VivaIOH
#define VivaIOH

#include "Globals.h"

#define INVALID_FILE		0.0

#define TEXT_PRECEDENCE		249
#define EOL_PRECEDENCE		250
#define EOL_CHARACTER		252
#define CMD_LEN				2000

const AnsiString		ASpaces =
"\r\n                                                                         ";


class VTextBase
{
protected:
    FileTypeEnum		FileType;			// Code for type of Viva file is being read
											//		(project, sheet, etc)
    int					InFileHandle;		// Universal input file handle.
    TCursor				OldCursor;          // Screen cursor before load/save started
    bool				WasEnabled;

public:
	char				Delimiter;
	char				*InputPtr;
	char				*IOEnd;
	char				cmd[CMD_LEN];
	int					Syntax;				// 0 = OK, >= 0 error, < 0 means eof
	int					LineNr;
	char				*LinePtr;

    int					OutFileHandle;		// Universal output file handle.
    AnsiString			OutFileName;		// Name of the text base file being writen to.
    int             	Indent;
    int					GUI_X;
    int					GUI_Y;
    bool				SaveCompileInfo;	// Indicates whether or not the AssignedBehaviors,
    										//		Xilinx ".bit" files, and IOLinks are being
                                            //		saved in the project save file.
    bool				ReadOK;				//
    bool				Relinked;
    static const char	null = 0;			// Null char.  Nuff said.
    char				AttSeparator;
    double				FileVersion;		// File revision number, stated at the top of a Viva
    										//		save file.  Needed in order to know how to
											//		interpret various clauses in the file.
    NodeList			*UserInputNodes;	// Where input nodes go, once read in from file,
											//		before they are assigned to an object
    NodeList			*UserOutputNodes;	// Where input nodes go, once read in from file,
											// 		before they are assigned to an object
    VivaSystem			*SubjectSystem;
    VList				*CornerList;		// Transport corners specified in GUI info in file

    AnsiString			*Library;
    AnsiString			FileName;
    AnsiString			ShortLibName;
    int					ShortLibNameLen;

    VTextBase(FileTypeEnum _FileType);
    ~VTextBase();

    static AnsiString AutoVersion(AnsiString FileName);
    int ReadInt();
    void WriteString(const char *OutString, int Length = 0);
    void WriteString(AnsiString OutString);
    void WriteChar(char chr);
    void WriteCRLF();
    int ErrorTax(int ErrorNumber,AnsiString Msg = "");
    void WriteEncapsulated(AnsiString &Name);
    void WriteSemiEnd();
    void WriteQualifier(int jv);
    void WriteNodeList(NodeList *IOList, int &NodePrintCount);
    void WriteQualifiedName(I2adl  *Object);
    void WriteIndented(const char *OutString = &null);
    void WriteAttribute(AnsiString Attribute);
    void WriteAttribute(int Attribute);
    void WriteObjectAtt(AnsiString *AttName, AnsiString *AttValue);
    void WriteGUICoordinates(int X, int Y, int Continuation = -1);
    bool TextLegacy(TStringList *Attributes);
    void WriteTextAttributes(I2adl *Object);
    void WriteObject(I2adl *Object, WriteModeEnum WriteMode = wmGUIandAttributes);
    void WriteQualifiedName(Node *IONode, int Index);
    void NodeRename(NodeList *IOList, char *Prefix);
    void WriteDocumentation (AnsiString &Documentation);
    DataSet *GetDataSetMake(AnsiString DSName);
    bool ReadNodeList(NodeList **IONodes);
    void UpgradeNodeList(NodeList **ObjectNodel, NodeList *UserNodes);
    void CleanUp(NodeList *IOList);
    void CleanUp();
    int ReadQualifier();
    I2adl *ReadObjectHeader();
    char *AttributeScan();
    void ReadObjectAtt(I2adl *RObject, AnsiString &TreeGroup);
    void ReadAttributes(TStringList *Attributes, VivaSystem *FromSystem = NULL);
    int AttributeDefinition();
    bool ReadCoordinates();
    bool GUISpec();
    void ReadTextObject(I2adl *RObject);
    I2adl *ReadObject(AnsiString &TreeGroup);
    void TransportJustify(I2adl *XPort);
    void CoordinateJunction(Node *JunctionNode, int DeltaX, int DeltaY);
    void ReadDataSet();
    void ReadDocumentation(AnsiString &Documentation);
    void SyncBehavior(I2adlList *Behavior, I2adl *ParentObject);
    int SyncObjects(bool NewWipSheets = true);
    void ReadSystem();
    bool ReadSystemHeader();
	void ReadSystemBehavior();
    bool ReadI2adl(VivaSystem *_SubjectSystem = NULL, AnsiString *SheetFileName = NULL
    	, bool NewWipSheets = true);
    IOLink *MatchingLink(IOLinkList *CLinks, IOLink *Link);
	void ReadI2adlList(I2adlList *Behavior, int ReadBehavior, I2adl *ForObject = NULL);
    void ReadTax(I2adlList *Behavior);
    void WriteBehavior(I2adlList *Behavior, WriteModeEnum WriteMode = wmGUIandAttributes);
    void Sequence(I2adlList *Behavior);
    void Write(I2adlList *Behavior, FileTypeEnum FileType, AnsiString FootPrint = "");
    Node *ReadNodeRef(I2adlList *Behavior, bool typeOutput = true);
    bool OpenInputFile(AnsiString _FileName);
    void CloseInputFile(bool DisplayMessage = true);
    void CloseOutputFile();
    void Write(VivaSystem *System);
    void MergeIOLinks(VivaSystem *System);
	void SaveSystem(VivaSystem *System, bool SaveDependencies);
    void WriteProject(bool SavingANewProject);

    double ReadVersion(int FileHandle);
    void AddControlNodes(I2adl *Object);

	void Delimit();
	bool NextLine();
	void SkipLine();
	void ExtractString(char *ReturnString);
	void ExtractString(AnsiString &ReturnString);
	bool SkipWhiteSpace();

	void LoadLibrary(AnsiString *_Library);
};

#endif
