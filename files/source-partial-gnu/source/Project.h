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

#ifndef ProjectH
#define ProjectH

class Project
{
protected:
    bool            IsCompiling;		// The project is compiling.
    I2adl			*FCompiledSheet;	// Last Wip sheet to be compiled.  NULL indicates the
    									//		project does not contain persistence info.
    I2adl			*FCompilingSheet;	// Sheet that is being compiled; NULL means that no
										//		compile is taking place

public:
    AnsiString		CompilingSheetName;	// The name of the sheet being compiled (incase
    									//		CompilingSheet is deleted).

    __property I2adl	*CompiledSheet = {read = GetCompiledSheet, write = SetCompiledSheet};
    __property I2adl	*CompilingSheet = {read = GetCompilingSheet, write = FCompilingSheet};
    __property bool		Compiling = {read = IsCompiling, write = SetCompiling};

    AnsiString      VexName;			// Path and filename to use when user saves a .vex file
    AnsiString      Name;				// Name of the file that the project was read in from
    AnsiString      FilePath;			// Path name of source/destination file
    VivaSystem      *EditSystem;		// System being editted in System Editor
    SystemEditor    *SysEdit;			// Editor for the current open system
    I2adlList		*Wip;				// Behavior Sheets open for editing (Work In Progress)
    TTreeNode       *ActiveWipNode;		// Node for Wip Sheet being edited
    bool            ProjectChanged;		// Has been modified since opened/saved
    bool            FirstRun;			// Has been run since compiled behavior was created.
    bool            PromptMissingBehaviors; // False when reading Executable files
    VList			*Libraries;			// File names of Object Libraries open.
    AnsiString		SystemName;			// System description file name, whether static or
    									//		dynamic.
    AnsiString		DynamicSystemName;	// File name of the external system description, if one
    									//		is in use.  Must be left NULL until after the
                                        //		system is read in so VivaIO will know it is a
                                        //		real system (not AssignBehavior system info).
    TStringList		*Attributes;		// Overrides system attributes in all subsystems

    Project(int Mode, VTextBase *Loader = NULL, AnsiString _Name = strNewProject
    	, bool Compiled = false, bool BuildObjectTree = true);
    ~Project();

    TTreeNode *GetTreeNode(TTreeNodes *SearchTree, void *Object);
    TTreeNode *GetTreeNode(TTreeNodes *SearchTree, AnsiString &NodeText);
    void BuildTreeView(bool AutoRebuild = true);
    void BuildObjectTreeView();
    void BuildDataSetTreeView();
	void AddCompositeObjectToTree(I2adl *Object, TStringList *ObjectNames
    	, TStringList *MismatchNames, TTreeNodes *TreeNodes, TTreeNode *BaseNode);
    TTreeNode *GetOrMakeTreeGroup(TTreeNode *TreeNode, AnsiString TreeGroup);
    void SetName(AnsiString &_Name);
    bool SaveChanges();
    void Modified(bool SystemChanged = false);
    void SetCompiledSheet(I2adl *Sheet);
    I2adl *GetCompiledSheet();
    I2adl *GetCompilingSheet();
	AnsiString GetTreeHierarchyString(TTreeNode *TreeNode);
	AnsiString GetVexFileName(AnsiString SheetName = strNull);
    VList *GetOverloadList(I2adl *Object);
    void SetCompiling(bool Val);
	void ReloadLibraries(AnsiString NewSystemName = strNull);
	void NewBaseSystem(VTextBase &Loader, AnsiString FileName, bool _RebuildUI);        
};

#endif
