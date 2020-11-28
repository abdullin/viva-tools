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

#define VCL_FULL
#define INC_ALLVCL_HEADERS
#include <vcl.h>
#include <process.h>

#pragma hdrstop

#include <shlobj.h>
#include <stdio.h>
#include <dir.h>
#include <io.h>

#include "Globals.h"

#include "AttributeCalculator.h"
#include "Main.h"
#include "Project.h"
#include "DataSet.h"
#include "ErrorTrap.h"
#include "I2adl.h"
#include "IniFile.h"
#include "Node.h"
#include "VivaSystem.h"
#include "ComI2adl.h"
#include "ComManager.h"
#include "I2adlList.h"
#include "I2adlEditor.h"
#include "VivaIcon.h"
#include "X86ExThread.h"
#include "WidgetForm.h"
#include "SearchEngine.h"
#include "ChipData.h"
#include "Translators.h"
#include "mmsystem.h"
#include "VivaIO.h"
#include "new.h"
#include "SynthGraphic.h"
#include "Settings.h"
#include "ComFormDesigner.h"

#pragma package(smart_init)

int dbg[10] = {0,0,0,0,0,0,0,0,0,0};

namespace VivaGlobals
{
    DataSet					*PatternDataSets[3][0x10000];

	TStringList				*TokenList					= new TStringList;

    HANDLE 					ThisProcessEvent 			= NULL;

    DeleteDSetsEnum			DeleteUnusedDataSets;
    HandleAmbigEnum			HandleAmbig_Sheet;
    HandleAmbigEnum			HandleAmbig_System;
	RunModeEnum				RunMode                 	= rmEdit;

	bool					AutoExpandObjectTree		= false;
	bool					CloseUnchangedSheets		= false;
#if VIVA_SD
	bool					IncludeCommSystems			= true;
#else
	bool					IncludeCommSystems			= false;
#endif
	bool					LoadCoreLib					= false;
    bool					StaticSystemDesc			= false;
	bool					LoadLastProject				= false;
    AnsiString				LastProjectName				= strNull;
    bool					ConfirmCloseProjects		= false;
    bool					ConfirmCloseSystems			= false;
	bool					AreShapesMoving         	= false;
	bool					IsRunning               	= false;
	bool					SortingTreeByName       	= false;
	bool					ShowingObjectAttributes   	= false;
	bool					HidingTreeGroups        	= false;
	bool					Initializing            	= false;
	bool					IsStopping              	= false;
    bool					StopSignalsSent				= false;
	bool					IsEDIFExport				= false;
	bool					MergeEquivObjects			= false;
    bool					RunAfterCompileOpt			= false;
    bool					OptimizeGatesOpt			= false;
    bool					RunAfterCompile				= true;
    bool					EnforceVariantRes			= false;
    bool					BuiltTokenList				= false;
    bool					ShouldMakeEdifIDs			= true;
    bool					CreateVexFile				= false;
	int						NonModalErrorCount			= 0;
    int						HideTreeGroupIcon;
    int						HideTreeViewIcon;
    int						PlayStopSheetIcon;
    int						OldDataSetCount;
    int						OldProjectObjectCount;
    int						MainWindowSetting;
    int						SettingFlags;
    int						NodeSnapTolerance;
    int						ExpansionCount				= 0;
    int						EmulatorSpeed				= 0;
    int						EmulatorPeriod				= 0;
    int						InitClockCycles             = 0;
    int						ColorCodeBIN				= 0xFF4040;
    int						ColorCodeLSB				= 0xBF8080;
    int						ColorCodeMSB				= 0xD76868;
    const double			TextBaseVersion				= 3.03;

    int						MaxRecentFiles[ftLast]  	= {0, 0, 0, 0, 0, 0};
    int						RecentFiles[ftLast]     	= {0, 0, 0, 0, 0, 0};
    int						FileTypeMenuOrder[ftLast]	= {0, 5, 1, 2, 3, 4};

    ExecuteTypeEnum			TimedEventType;

    VivaSystem				*BaseSystem;
    VivaSystem				*DefaultTargetSystem;
	VivaSystem				*UISystem;
    VivaSystem				*CPUSystem;
	VivaSystem				*ATIOSystem;
    VivaSystem				*XPointSystem;

    TWidgetForm				*WidgetForm 				= NULL;

    AttributeCalculator		*AttCalculator				= NULL;
    I2adlList				*X86SystemClocks			= new I2adlList;
    AnsiString				*JustCompiledObjectName		= NULL;
    TVivaTimer				*ProcessTimer;
    SearchEngine			*MainCompiler;
    VGlobalLabelTable		*GlobalLabels 				= NULL;

    VList					*RuntimeWideStrings			= new VList(sizeof(WideString));
    VList					*UntriggeredStopList 		= new VList(sizeof(I2adl));
    VList					*ComDispatchInvokeList		= new VList(sizeof(TComDispatch));

    TStringList				*ResolvedObjects;

	// Viva Icon Variables
    VivaIcon				*TransportCorners[4];
    AnsiString				IconFileName				= strNull;
    AnsiString				BackupFileName				= strNull;
    TI2adlStrTable			*I2adlStringTable			= NULL;
	VivaIcon				*CastNameInfo				= NULL;
    VivaIcon				*SysCastNameInfo			= NULL;
    VivaIcon				*ExposerIcon;
    VivaIcon				*CollectorIcon;
    VivaIcon				*ImplementorIcon;
    VivaIcon				*TransportHor;
    VivaIcon				*TransportVer;

    // Global project variables
	TProjectObjectsList		*ProjectObjects			= NULL;
	DataSetList				*ProjectDataSets;

    // Global data set variables
    DataSet					*BitDataSet;
    DataSet					*NULLDataSet;
    DataSet					*VariantDataSet;
    DataSet					*ListDataSet;

    DataSet					*SGDataSets[SGDataSetCount];

    AnsiString				SGDataSetNames[SGDataSetCount] =
    {
        "Empty",
        "NULL",
        "Int",
        "DInt",
        "Float",
        "Double",
        "Currency",
        "Date",
        "BinaryString",
        "IDispatch*",
        "Error",
        "Bool",
        "ComVariant",
        "IUnknown*",
        "Decimal",
        "[invalid]",
        "Signed Byte",
        "Byte",
		"Word",
        "DWord",
		"QInt",
        "QWord",
        "ComInt",
        "ComUInt",
        "Void",
        "HResult",
        "[pointer]",
        "SafeArray",
        "Array",
        "[user-defined]",
        "String*",
        "WideString*",  // End of COM type data sets.
        "Bit",
        "Variant",
        "???",			// Old Vector.
        "DBit",
        "Nibble",
        "3Bit",
        "7Bit",
        "9BitF",
        "11Bit",
        "12BitF",
        "20Bit",
        "23Bit",
        "36Bit",
        "52Bit",
        "WideChar",
        "Unsupported",
        "Enumeration",
        "Pointer",
        "BIN",    // GetDataSet() expects data set pattern names to be 3 characters.
        "LSB",
        "MSB",
        "List"
    };

    AnsiString				strTreeGroup				= "TreeGroup";
    AnsiString				strViewControl				= "ViewControl";
    AnsiString				strPrimitive				= "Primitive";
    AnsiString				strIconIndex				= "IconIndex";
    AnsiString				strCurrentSheet				= "CurrentSheet";
    AnsiString				strCompileSheet				= "CompileSheet";
    AnsiString				strLink						= "Link";
    AnsiString				strInfoRate					= "InfoRate";

	AnsiString				strLibrary					= "Library";
    AnsiString				strSystemFile				= "SystemFile";
    AnsiString				strDynamicSystemFile		= "DynamicSystemFile";
    AnsiString				strStaticSystemFile			= "StaticSystemFile";
    AnsiString				strMainWidgetTab			= "[Main]";
	AnsiString				strWipSheet					= "WipSheet";
	AnsiString				strClipBoard				= "ClipBoard";
	AnsiString				strCopyOf					= "CopyOf";
	AnsiString				strDefault					= "Default";
	AnsiString				strDeactivatedDefault		= "DeactivatedDefault";
	AnsiString				strCompiler					= "Compiler";
	AnsiString				strRunAfterCompile			= "RunAfterCompile";
    AnsiString				strOptimizeGates 			= "OptimizeGates";
	AnsiString				strEnforceVariantResolution	= "EnforceVariantResolution";
    AnsiString				strMakeEdifIDs				= "MakeEdifIDs";
    AnsiString				strCreateVexFile			= "CreateVexFile";
    AnsiString				strSettingsDialogTab		= "SettingsDialogTab";
    AnsiString				EdifExportFileName			= strNull;
	AnsiString				strDispatch					= "[Dispatch]";
    AnsiString				CMDLineCom              	= "";
    AnsiString				UserDir;
    AnsiString				TempDir;
    AnsiString				StartUpDir;
    AnsiString				KeyPrefixes[ftLast]     	= {"RecentProject",
	                        	                         "RecentSheet",
                                                         "RecentLibrary",
    	                        	                     "RecentSystem",
        	                        	                 "RecentComForm",
            	                        	             "RecentExecutable"};
	AnsiString				strIndex					= "Index";
    AnsiString				strViva						= "Viva";
    AnsiString				strVivaRun					= "VivaRun";
    AnsiString				strVivaSD					= "VivaSD";
    AnsiString				strX86						= "X86";
    AnsiString				strBenOne					= "BenOne";
    AnsiString				strXSK						= "XSK";

	AnsiString				strAppVersion				= " 3.1.2";
#if VIVA_IDE
    #if VIVA_SD
        AnsiString			strAppTitle					= strVivaSD + LICENSE_SD + strAppVersion;
    #else
        AnsiString			strAppTitle					= strViva + LICENSE_SD + strAppVersion;
    #endif
#else
	AnsiString				strAppTitle					= strVivaRun + LICENSE_SD + strAppVersion;
#endif
    AnsiString				strVivaDir;
	AnsiString				strVivaSystemDir;
	AnsiString				strHelpDir;
	AnsiString				strObjectDocDir;
	AnsiString				strMessageDocDir;
    AnsiString				ErrorTrapFileName;
	AnsiString				strIconsDir;
    AnsiString				DefaultSystemDesc;
    AnsiString				VivaRunFileName;
    AnsiString				strResourceData				= "ResourceData";
    AnsiString				DefaultResourceData			= "ResourceData.txt";
    AnsiString				strBasicDataSets			= "Basic Data Sets";
    AnsiString				strCOMDataSets				= "COM Data Sets";
    AnsiString				strPrimitiveObjects			= "Primitive Objects";
    AnsiString				strPrimitiveEDIFs			= "Primitive EDIFs";
    AnsiString				strCompositeObjects			= "Composite Objects";
    AnsiString				strLibraries				= "Libraries";
    AnsiString				strSystem					= "System";
    AnsiString				strResource					= "Resource";
    AnsiString				strResourceQty				= "ResourceQty";
    AnsiString				strChildAttribute			= "ChildAttribute";
    AnsiString				strObjectRef				= "ObjectRef";
    AnsiString				strProvidesResource			= "ProvidesResource";
    AnsiString				strConstant					= "Constant";
    AnsiString				strDeactivatedConstant		= "DeactivatedConstant";
    AnsiString				strWidget					= "Widget";
    AnsiString				strActiveXWidget			= "ActiveXWidget";
    AnsiString				strWidgetProperty			= "WidgetProperty";
    AnsiString				strWidgetEvent				= "WidgetEvent";
    AnsiString				strWidgetLeft				= "WidgetLeft";
    AnsiString				strWidgetTop				= "WidgetTop";
    AnsiString				strWidgetWidth				= "WidgetWidth";
    AnsiString				strWidgetHeight				= "WidgetHeight";
    AnsiString				strWidgetMin				= "WidgetMin";
    AnsiString				strWidgetMax				= "WidgetMax";
    AnsiString				strWidgetHex				= "WidgetHex";
    AnsiString				strWidgetTabSheet			= "WidgetTabSheet";
    AnsiString				strWidgetTextBox			= "WidgetTextBox";
    AnsiString				strTrue						= "True";
    AnsiString				strFalse					= "False";
    AnsiString				strIOAddress				= "IOAddress";
#ifdef IMPLIMENTATION_EXTENSION
    AnsiString				strTranslator				= "Translator";
    AnsiString				strTranslatorExclusive		= "TranslatorExclusive";
    AnsiString				strInitializer				= "Initializer";
    AnsiString				strInitializerExclusive		= "InitializerExclusive";
    AnsiString				strPostInitializer			= "PostInitializer";
    AnsiString				strPostInitializerExclusive = "PostInitializerExclusive";
    AnsiString				strExecutor					= "Executor";
#endif
    AnsiString				strPadLoc					= "PadLoc";
    AnsiString				strInst						= "Inst";
    AnsiString				strNet						= "Net";
    AnsiString				strTIG						= "TIG";
    AnsiString				strTNM						= "TNM";
	AnsiString				strLibRef					= "LibRef";
    AnsiString				strInstanceName				= "InstanceName";
	AnsiString				strEdifFile					= "EdifFile";
    AnsiString				strTIMESPEC					= "TIMESPEC";
    AnsiString				strPartType					= "PartType";
    AnsiString				strIORef					= "IORef";
    AnsiString				strIcon						= "Icon";
    AnsiString				strEndOfSystems				= "END OF SYSTEMS";
    AnsiString				strNewProject				= "New Project";
    AnsiString				strBaseSystem				= "BaseSystem";
    AnsiString				strDisplayPrototype			= "Display Prototype";
    AnsiString				strDisplayUsage				= "Display Usage";
    AnsiString				strFileName					= "FileName";
    AnsiString				strTrap						= "Trap";
    AnsiString				strExclusive				= "Exclusive";
    AnsiString				strDocumentation			= "Documentation";
    AnsiString				strNull						= "";
    AnsiString				strIniSet					= "IniSet";
    AnsiString				strSettings					= "Settings";
    AnsiString				strEnabledMessages			= "EnabledMessages";
    AnsiString				strShowAllWarnings			= "ShowAllMessages";
    AnsiString				strMaxRecentFiles			= "MaxRecentFiles";
    AnsiString				strAutoExpandObjectTree		= "AutoExpandObjectTree";
    AnsiString				strSettingFlags				= "ConfirmationFlags";
    AnsiString				strCloseUnchangedSheets		= "CloseUnchangedSheets";
    AnsiString				strLoadCoreLib				= "LoadCoreLib";
    AnsiString				strStaticSystemDesc			= "StaticSystemDesc";
    AnsiString				strLoadLastProject			= "LoadLastProject";
    AnsiString				strLastProjectName			= "LastProjectName";
    AnsiString				strReverseBitFans			= "ReverseBitFans";
    AnsiString				strMin						= "Min";
    AnsiString				strMax						= "Max";
    AnsiString				strDefaultTargetSystem		= "DefaultTargetSystem";
    AnsiString				strJoinGlobals				= "JoinGlobals";
    AnsiString				strJoinGlobalsDeletePri		= "JoinGlobalsDeletePri";
    AnsiString				str_NONE_					= "<NONE>";
    AnsiString				strPromoteToParentObject	= "PromoteToParentObject";
    AnsiString				strConstTruthFile			= "ConstTruthFile";
    int						strConstTruthFileLen		= strConstTruthFile.Length();
    AnsiString				strConstTruthTable			= "ConstTruthTable";
    int						strConstTruthTableLen		= strConstTruthTable.Length();
    AnsiString				strConfirmCloseProjects		= "ConfirmCloseNamedProjects";
    AnsiString				strConfirmCloseSystems		= "ConfirmCloseSystems";
    AnsiString				strStartMaximized			= "StartMaximized";
    AnsiString				strWindow					= "Window";
    AnsiString				strWindowColor				= "WindowColor";
    AnsiString				strSelectionColor			= "SelectionColor";
    AnsiString				strMainForm					= "MainForm";
    AnsiString				strMainWindowRect			= "MainWindowRect";
    AnsiString				strWipTreeHeight			= "WipTreeHeight";
    AnsiString				strWipTreeWidth				= "WipTreeWidth";
    AnsiString				strMainWindowSetting		= "MainWindowSetting";
	AnsiString				strWidgetFormLeft			= "WidgetFormLeft";
	AnsiString				strWidgetFormTop			= "WidgetFormTop";
    AnsiString				strMessageGridHeight		= "MessageGridHeight";
    AnsiString				strNodeSnapTolerance		= "NodeSnapTolerance";
    AnsiString				strDeleteUnusedDataSets		= "DeleteUnusedDataSets";
    AnsiString				strHandleAmbig_Sheet		= "HandleAmbiguousObjects_SheetLoad";
    AnsiString				strHandleAmbig_System		= "HandleAmbiguousObjects_SystemLoad";
    AnsiString				strHideTreeGroups			= "Hide $Tree &Groups";
    AnsiString				strShowTreeGroups			= "Show $Tree &Groups";
	AnsiString				strHideTreeView				= "Hide Tree &View";
    AnsiString				strShowTreeView				= "Show Tree &View";
	AnsiString				strChevronButton			= "ChevronButton";
    AnsiString				strKeepObject				= "KeepObject";
    AnsiString				strIn						= "In";
    AnsiString				strOut						= "Out";
    AnsiString				strData						= "Data";
    AnsiString				strType						= "Type";
    AnsiString				strWarnOnRemoval			= "WarnOnRemoval";
    AnsiString				strParentDataSets			= "ParentDataSets";
    AnsiString				strCast						= "Cast";
    AnsiString				strSpace					= " ";
    AnsiString				strPeriod					= ".";
    AnsiString				strBackSlash				= "\\";
    AnsiString				strDollarSign				= "$";
    AnsiString				strBackSlashDollarSign		= "\\$";
    AnsiString				strCRLF						= "\r\n";
    AnsiString				strBuildDir					= "BuildDir";
    AnsiString				strCompileTimeSystem		= "CompileTimeSystem";
    AnsiString				strIOBuf					= "IOBuf";
    AnsiString				strMessage					= "<Message>";
    AnsiString				str_TreeGroup_				= "<TreeGroup>";
    AnsiString				strFromObject				= "FromObject: ";
    int						strFromObjectLen			= strFromObject.Length();
    AnsiString				strStatic					= "System\\Static";
    AnsiString				strComPredefined			= "System\\Com Predefined";
    AnsiString				strCannotLoad				= "Cannot load: ";
    AnsiString				strSystemBIN				= "System\\$BIN";
    AnsiString				strSystemLSB				= "System\\$LSB";
    AnsiString				strSystemMSB				= "System\\$MSB";
    AnsiString				strInputHornName			= "InputHornName";
    AnsiString				strOutputHornName			= "OutputHornName";
	AnsiString				strChooseDataSets			= "ChooseDataSets";
	AnsiString				strApplyDSToRest			= "ApplyDSToRest";
	AnsiString				strI2adlEditorClipBoard		= "I2adlEditorClipBoard";

    AnsiString				FilterStrings[ftLast + 1][2] =
							{   {"Viva Project Files",          "idl"},
								{"Viva Behavior Sheets",        "ipg"},
								{"Viva Libraries",        		"ipg"},                                
								{"Viva System Descriptions",    "sd"},
								{"Viva COM Forms",              "cfm"},
								{"Viva Executable Files",       "vex"},
								{"All Files",                   "*" },    };

    AnsiString				KeepObjectOptionStrings[koLast] =
							{   "Both sides",
								"One side",
								"Always"                                };

    AnsiString           	OrigOpenAsSheetCaption		= strNull;
    AnsiString           	OrigEditAttributesCaption	= strNull;
    AnsiString           	OrigDeleteObjectCaption		= strNull;
	AnsiString				OrigLocateObjectCaption		= strNull;

    TNodeStrTable			*NodeStringTable			= NULL;
    NodeString				*NodestrNull				= NULL;
    NodeString				*NodestrGo					= NULL;
    NodeString				*NodestrDone				= NULL;
	NodeString				*NodestrBusy				= NULL;
    NodeString				*NodestrWait				= NULL;
	NodeString				*NodestrClock				= NULL;
    NodeString				*NodestrClockState			= NULL;
    NodeString				*NodestrSysClock			= NULL;
    NodeString				*NodestrAddConstant			= NULL;
    NodeString				*NodestrHold				= NULL;
    NodeString				*NodestrIn					= NULL;
    NodeString				*NodestrIn0					= NULL;
    NodeString				*NodestrIn1					= NULL;
    NodeString				*NodestrIn2					= NULL;
    NodeString				*NodestrIn3					= NULL;
    NodeString				*NodestrOut					= NULL;
    NodeString				*NodestrOut1				= NULL;
    NodeString				*NodestrOut2				= NULL;
    NodeString				*NodestrOut3				= NULL;
    NodeString				*NodestrType				= NULL;
	NodeString				*NodestrException			= NULL;

    StringTable				*ResourceStringTable		= NULL;
    ResourceString			*ResourcestrNull			= NULL;
    ResourceString			*ResourcestrComposite		= NULL;
    ResourceString			*ResourcestrGlobal			= NULL;
    ResourceString			*ResourcestrGlobalIS		= NULL;
    ResourceString			*ResourcestrAtomicTransport	= NULL;
    ResourceString			*ResourcestrTempConnectObject = NULL;
    ResourceString			*ResourcestrCLB				= NULL;
    ResourceString			*ResourcestrTimeSlice		= NULL;

    StringTable				*ResourceTypeStringTable	= NULL;
    ResourceTypeString		*ResourceTypestrTDM			= NULL;
    ResourceTypeString		*ResourceTypestrPINS		= NULL;

    StringTable				*SystemStringTable			= NULL;
    SystemString			*SystemstrNull				= NULL;
    SystemString			*SystemstrWidgetSystem		= NULL;
    SystemString			*SystemstrQMark				= NULL;

	TStringList				*DatasetTreeGroups;
    TStringList				*ObjectTreeGroups;
    TStringList				*ComLibraries;
    TStringList				*ExpandedNodes;
    Project					*MyProject					= NULL;
    VIniFile				*IniFile;
    bool					CancelCompile				= true;	// "true" means not compiling.
    VDismember				*Hierarch					= NULL;
    DismemberList			*HierarchList				= new DismemberList(sizeof(VDismember));
    MenuItemList			*LocateObjects				= new MenuItemList(sizeof(TMenuItem));
    IntegerList				*DisabledErrors				= new IntegerList(sizeof(int));
    ModalResultList			*DisabledModalResults		= new ModalResultList(sizeof(int));
    TStringList				*ErrorMessages				= new TStringList;  // Entire ErrorMessage file.
    MyTScrollingWinControl	*I2adlView;
    I2adl					*ParentI2adlObject			= NULL;
    I2adl					*MainInputObject			= NULL;
    I2adl					*MainOutputObject			= NULL;
    I2adl					*MainSystemCastObject		= NULL;

    AnsiString				*ControlNodeNames[ceLast]	= {NULL, NULL, NULL, NULL};

    // CompressBaseList is usually equal to function  description
    // except when preprocessing a new object behavior.
    I2adlList				*CompressBaseList			= NULL;
}


// If the FileName does not exist, then allow the user to provide the requested file in the
//		requested directory.
//		Returns true if the file existed/was provided.
bool LocateFile(const AnsiString &FileName)
{
	while (!FileExists(FileName))
    {
		if (ErrorTrap(4063, FileName) == mrCancel)
        	return false;
    }

    return true;
}


// If the FileName does not exist, then allow the user to provide the name of another file.
//		The new file name will be returned in the FileName parameter.
//		If AllowCancel is false, then the user will be forced to provide a valid file name.
//		If the file FilterString is not provided, then any file (*.*) will be allowed.
//		Returns true if the file existed/was provided.
bool BrowseFile(AnsiString &FileName, bool AllowCancel, AnsiString FilterString)
{
	if (FileExists(FileName))
    	return true;

    // The following prevents an access violation by removing the dependenace on MainForm.
    if (MainForm == NULL)
        return LocateFile(FileName);

    // Tell the user what file is missing.
    if (AllowCancel)
    {
	    // Does the user want to browse for a different file?
        if (ErrorTrap(4064, FileName) != mrYes)
            return false;
    }
    else
    {
    	ErrorTrap(4066, FileName);
    }

    // Remove invalid Windows file name characters from FileName so that OpenDialog will
    //		execute.
    AnsiString	ShortFileName = GetShortFileName(FileName);
    AnsiString	OrigTitle = MainForm->OpenDialog->Title;

    IsValidFileName(ShortFileName);

    MainForm->OpenDialog->Filter = (FilterString == strNull)
    							 ? AnsiString("All Files (*.*)|*.*")
                                 : FilterString;
    MainForm->OpenDialog->FilterIndex = 1;
    MainForm->OpenDialog->InitialDir = GetFilePath(FileName);
    MainForm->OpenDialog->FileName = ShortFileName;

    while (true)
    {
		MainForm->OpenDialog->Title = "Browse";

        bool	FileOpened =  MainForm->OpenDialog->Execute();

        MainForm->OpenDialog->Title = OrigTitle;

        if (FileOpened)
        {
            FileName = MainForm->OpenDialog->FileName;

            bool	FoundFile = FileExists(FileName);

            if (FoundFile ||
            	AllowCancel)
		            return FoundFile;
        }
        else
        {
        	if (AllowCancel)
	            return false;
        }
    }
}


// The build directory in entered as an attribute in the base system.  It defaults to the
//     user directory.
//     Note:  SetUserDir() must have been called at least once before this routine is called.
AnsiString VivaGlobals::GetBuildDir()
{
	if (BaseSystem == NULL)
		return strNull;

    AnsiString	BuildDir = BaseSystem->GetAttribute(strBuildDir, false);

    if (BuildDir == strNull)
    {
        BuildDir = UserDir;
    }
    else
    {
    	// Automatically add on the trailing "\" (unless has a directory prefix macro).
        if (BuildDir[BuildDir.Length()] != ')' &&
        	BuildDir[BuildDir.Length()] != '\\')
	            BuildDir += '\\';

	    // The build directory cannot contain "$(DirName:Build)" or we will go into an
        //		infinite loop.
        if (BuildDir.Pos("$(DirName:Build)") > 0)
        {
        	ErrorTrap(182, BuildDir);

            // Recover by using the project directory.
            BuildDir = "$(DirName:Project)";

			BaseSystem->SetAttribute(strBuildDir, BuildDir);
        }
    }

    AttCalculator->CalculateAttribute(BuildDir);

    return BuildDir;
}


// Sets the global user directory variable to either (1) the user profile directory (normally
//		C:\Documents and Settings\username) or (2) the Viva\VivaSystem directory.
void VivaGlobals::SetUserDir()
{
	char	userdir[MAX_PATH];

    SHGetFolderPath(NULL, CSIDL_PROFILE | CSIDL_FLAG_CREATE, NULL, SHGFP_TYPE_CURRENT,
    	userdir);

    // Old versions of Windows do not have a Settings directory.
    if (GetFileAttributes(userdir) == 0xFFFFFFFF)
    {
        // Can't find the user profile directory.  Inform the user that the Viva\VivaSystem
        //		directory will be used.
	    UserDir = strVivaSystemDir;

        AnsiString	struserdir = (userdir[0] == 0) ? AnsiString("<Unknown>")
        										   : AnsiString(userdir);

		ErrorTrap(150, VStringList(struserdir, strVivaSystemDir));
    }
    else
    {
        UserDir = AnsiString(userdir) + "\\";
    }
}


// Sets the global temporary directory variable name.
//     Note:  SetUserDir() must have been called at least once before this routine is called.
void VivaGlobals::SetTempDir()
{
	char	TempPath[MAX_PATH];

    if (GetTempPath(MAX_PATH, TempPath) == 0)
    	TempDir = UserDir;
    else
	    TempDir = TempPath;
}


// Save the original start up directory
void VivaGlobals::SetStartUpDir()
{
    char	Dir[MAXDIR] = "";

    getcwd(&Dir[0], MAXDIR);

    int		DirLength = strlen(Dir);

    // If the name is a root directory (example : "E:"), then we need to add the backslash:
    //     A trailing '\0' is not needed because Dir is initialized with them.
    if (Dir[DirLength - 1] != '\\')
        Dir[DirLength] = '\\';

    StartUpDir = AnsiString(Dir);
}


void VivaGlobals::RunProject()
{
    if (RunMode == rmExecute)    // Pre-compiled project
        StartExecution();
    else if (RunMode == rmCompile)
        MainForm->cmdSynthesizeClick(NULL);
}


AnsiString VivaGlobals::ReplaceAll(AnsiString Text, const AnsiString &Out, const AnsiString &In)
{
    int pos;
    while ((pos = Text.Pos(Out)) > 0)
    {
        Text.Delete(pos, Out.Length());
        Text.Insert(In, pos);
    }

    return Text;
}


void VivaGlobals::ReallocateDC(TFont *Font)
{
//    ReleaseDC(Font, Font->Handle);
    Font->Handle = 0; // GetDC(Font->Handle);
}


void VivaGlobals::AlignToGrid(int &Data)
{
	// Add a large number before rounding, in case the number is negative.
	int 	mod = (Data + ViewScale * MAXSHORT) % ViewScale;

	Data = Data - mod;

	if ((double(mod) / ViewScale) > .5)
    	Data += ViewScale;
}


void VivaGlobals::ColorChange(TEdit *Edit, TEdit *ColorEdit, TColor &NewColor)
{
    // Do some string checking, to allow flexibility in user entry;
    //      Otherwise, the user is given a diagnasty while typing:
    if (Edit->Text.Trim().Length() == 0) // Nothing yet entered
        return;
    // "0x" is not a valid integer value, but is the start of one:
    if (Edit->Text.LowerCase().AnsiPos("x") == Edit->Text.Trim().Length())
        return;

    // Update the NewColor and txtNodeColor with the entered value:
    NewColor = (TColor) AnsiString(Edit->Text).ToInt(); // fails on invalid value
    ColorEdit->Color = (TColor) NewColor;
}


void VivaGlobals::UpdateColor(TEdit *Edit, TColor &NewColor)
{
    TColorDialog *ColorDialog = MainForm->ColorDialog;
    // NewColor is the prospective color for the current node; assign to dialog:
    ColorDialog->Color = (TColor) NewColor;
    // Execute the dialog and update NewColor with its contents (selected color)
    if (ColorDialog->Execute())
    {
        NewColor = ColorDialog->Color;
        UpdateColorFormat(Edit, NewColor); // Updates text box with the color in Hex format.
    }
}


void VivaGlobals::UpdateColorFormat(TEdit *Edit, TColor &NewColor)
{
    TVarRec Color[1] = {NewColor};
    Edit->Text = "0x" + Format("%0.6x", Color, 0); // 6 digit precision.
}


// StrToInt() wrapper method to handle invalid strings.
//		String - the string to be converted into an integer.
//		DefaultValue - the value returned for an empty/invalid string.
int VivaGlobals::VStrToInt(AnsiString String, int DefaultValue)
{
    if (String == strNull)
    	return DefaultValue;

	int		ReturnInt;

    try
    {
        ReturnInt = StrToInt(String);
    }
    catch(...)
    {
    	// Inform the user that the string is invalid.
    	ErrorTrap(214, VStringList(String, IntToStr(DefaultValue)));

		ReturnInt = DefaultValue;
    }

    return ReturnInt;
}


// Rebuild the Resource, Resource Type, System and Node string tables.  Even though the
//     string tables do not take up very much memory, they prevent large blocks of memory
//     from being reused.
void VivaGlobals::RebuildStringTables()
{
    // Rebuild the Resource string table.
    delete ResourceStringTable;

    ResourceStringTable = new StringTable();
    ResourceStringTable->Capacity = 50;

    ResourcestrNull            		= (ResourceString *) ResourceStringTable->Items[0];
    ResourcestrGlobal          		= ResourceStringTable->QString("Global");
    ResourcestrGlobalIS				= ResourceStringTable->QString("GlobalIncludeSystem");
    ResourcestrTimeSlice       		= ResourceStringTable->QString("TIMESLICE");
    ResourcestrComposite       		= ResourceStringTable->QString("Composite");
    ResourcestrAtomicTransport		= ResourceStringTable->QString("Atomic Transport");
    ResourcestrTempConnectObject	= ResourceStringTable->QString("TemporaryConnectionObject");
    ResourcestrCLB					= ResourceStringTable->QString("CLB");

    // Rebuild the Resource Type string table.
    delete ResourceTypeStringTable;

    ResourceTypeStringTable = new StringTable();
    ResourceTypeStringTable->Capacity = 10;

    ResourceTypestrTDM         = ResourceTypeStringTable->QString("TDM");
    ResourceTypestrPINS        = ResourceTypeStringTable->QString("PINS");

    // Rebuild the System string table.
    delete SystemStringTable;

    SystemStringTable = new StringTable();
    SystemStringTable->Capacity = 50;

    SystemstrNull           = (SystemString *) SystemStringTable->Items[0];
	SystemstrWidgetSystem   = SystemStringTable->QString("$(SysAtt::WidgetSystem)");
	SystemstrQMark			= SystemStringTable->QString("?");

    // Rebuild the Node string table.
    delete NodeStringTable;

    NodeStringTable = new TNodeStrTable();
    NodeStringTable->Capacity = 200;

    NodestrNull                = (NodeString *) NodeStringTable->Items[0];
    NodestrGo                  = NodeStringTable->QString("Go");
    NodestrDone                = NodeStringTable->QString("Done");
    NodestrBusy                = NodeStringTable->QString("Busy");
    NodestrWait                = NodeStringTable->QString("Wait");
    NodestrClock               = NodeStringTable->QString("Clock");
    NodestrClockState          = NodeStringTable->QString("ClockState");
    NodestrSysClock            = NodeStringTable->QString("$Clock");
    NodestrAddConstant         = NodeStringTable->QString("ADD Constant");
    NodestrHold                = NodeStringTable->QString("Hold");
    NodestrIn                  = NodeStringTable->QString(strIn);
    NodestrIn0                 = NodeStringTable->QString("In0");
    NodestrIn1                 = NodeStringTable->QString("In1");
    NodestrIn2                 = NodeStringTable->QString("In2");
    NodestrIn3                 = NodeStringTable->QString("In3");
    NodestrOut                 = NodeStringTable->QString(strOut);
    NodestrOut1                = NodeStringTable->QString("Out1");
    NodestrOut2                = NodeStringTable->QString("Out2");
    NodestrOut3                = NodeStringTable->QString("Out3");
    NodestrType                = NodeStringTable->QString(strType);
	NodestrException		   = NodeStringTable->QString("Exception");

    ControlNodeNames[0] = NodestrGo;
    ControlNodeNames[1] = NodestrWait;
    ControlNodeNames[2] = NodestrDone;
    ControlNodeNames[3] = NodestrBusy;
}


void VivaGlobals::PlaceJunctionNodes(Node *InputNode, int InputPos)
{
	// New node X and Y positions, starting at 9:00 and moving clockwise:
	int		NodeXYPositions[4][2] = { {0, ViewScale}, {ViewScale, 0}
    		, {ViewScale << 1, ViewScale}, {ViewScale, ViewScale << 1} };

    InputNode->Left = NodeXYPositions[InputPos][0];
    InputNode->Top = NodeXYPositions[InputPos][1];

    // Was this the Junction's input?
    I2adl 	*Junction = InputNode->Owner;

    if (InputNode != Junction->Inputs->Items[0])
        return;

    // We now need to place the Junction's input and output nodes;
    //      the values are in the NodeXYPositions[] array:
    int CurrOutput = 0;

    for (int i = 0; i < 4; i++)
    {
        if (i != InputPos) // that position is already used.
        {
            Node *CurrNode = Junction->Outputs->Items[CurrOutput];
            CurrNode->Left = NodeXYPositions[i][0];
            CurrNode->Top = NodeXYPositions[i][1];
            CurrOutput++;
        }
    }
}


// Load X86SystemClocks with all of the Input objects that have a node name of $Clock from
//     all of the X86UI/CPU type systems.
void VivaGlobals::LoadX86SystemClocks()
{
	X86SystemClocks->Count = 0;

    BaseSystem->LoadX86SystemClocks();
}


// Start to execute the current compiled behavior.  If a VexFileName is passed in, then it
//		will be executed (not the current sheet, Bugzilla #925).
void VivaGlobals::StartExecution(AnsiString VexFileName)
{
    // If a behavior is running, then stop it.
    if (IsRunning)
    {
        HaltExecution();
        return;
    }

    // Do we execute by running a Vex file or in the traditional way?
    if (RunMode != rmExecute &&
    	MyProject->FilePath != strNull &&
    	CreateVexFile)
    {
        // Use VivaRun.exe to execute the Vex file (if it exists).
        if (VexFileName == strNull)
	    	VexFileName = MyProject->GetVexFileName();

        if (FileExists(VexFileName))
        {
	        AnsiString	VexFileNameInQuotes("\"" + VexFileName + "\"");

	        spawnlp(P_DETACH, VivaRunFileName.c_str(), VivaRunFileName.c_str(),
            	VexFileNameInQuotes.c_str(), 0);

	        HaltExecution();
            return;
        }

        // The vex file does not exist.  If the project contains persistence information
        //		then execute it.  Otherwise, warn the user because he probably delete it.
        if (MyProject->CompiledSheet == NULL)
        {
			ErrorTrap(23, VexFileName);

	        HaltExecution();
            return;
        }
    }

    // No behavior is running, so start it.
    IsRunning = true;
    UntriggeredStopList->Count = 0;

    // The following keeps Viva from aborting when an Input on a top level sheet has a
    //     Constant attribute string of "##N1.C" and another behavior has been run.
    ParentI2adlObject = NULL;

    if (MyProject->FirstRun)
        TimedEventType = etTranslate;    // First run since project read in.
    else
        TimedEventType = etInitialize;

    CancelCompile = false;

    ProcessTimer->Enabled = true;
}


void VivaGlobals::HaltExecution()
{
    if (IsRunning)
    {
		TimedEventType = etExecute;
		IsStopping = true;

    	do
	    {
			Application->ProcessMessages();
	    	Sleep(40);
	    }
    	while(IsStopping);
    }
    else
    {
		TryHaltExecution();    
    }
}


DWORD StartTime = timeGetTime();

bool VivaGlobals::TryHaltExecution()
{
    // Release the garbage collected Event objects.  This fixes the problem with MulPipe3 so
    //     it will compile twice in a row without running out of memory.  They do not take a
    //     lot of memory, but they prevent the FunctionDescription memory from being reused.
    while (HeadRecycle != NULL)
    {
        EventLink    *SaveHeadRecycle = HeadRecycle;

        HeadRecycle = HeadRecycle->Next;
		delete SaveHeadRecycle;
    }

    if (!MyProject->Compiling)
    {
        if (!StopSignalsSent)
        {
			StopSignalsSent = true;

		    if (UISystem != NULL)
		        UISystem->SysTranslate->DeleteUIForms();

            IsStopping = true;

            if (CPUSystem != NULL && CPUSystem->SysThread != NULL)
	            CPUSystem->SysThread->SendStopSignals();

            StartTime = timeGetTime();
        }

        if (UntriggeredStopList->Count > 0)
        {
            // Have we timed out, but still have unprocessed events?
            //		To do:  Make the timeout limit be a system attribute (not hard coded).
            if (timeGetTime() > StartTime + TIMEOUT_DEFAULT)
            {
	            I2adl	*StopObject = (I2adl *) UntriggeredStopList->Items[0];

                if (ErrorTrace(4046, StopObject) == mrYes) // Do they want to continue?
                {
                    StartTime = timeGetTime(); // Restart timeout time.
                    return false;
                }
            }
            else
            {
                return false;
            }
        }
    }

    IsRunning = false;

    if (UISystem != NULL)
        UISystem->SysTranslate->DeleteRuntimeGlobals();

#if VIVA_IDE
    MainForm->cmdPlayStopSheet->ImageIndex = PlayStopSheetIcon;
    MainForm->cmdSynthesize->ImageIndex = 45;
    MainForm->MenuBtnSync(MainForm->cmdPlayStopSheet);
    MainForm->MenuBtnSync(MainForm->cmdSynthesize);

    SetProgPos(0);

    if (MainForm->WindowState == wsMinimized)
        MainForm->WindowState = wsNormal;

    // Reset the status bar message.
    SetStatusCaption(strNull, false);

    // Rebuild the object tree incase pattern data sets or an EDIF Export object were created.
    if (OldDataSetCount != ProjectDataSets->Count
    	|| (OldProjectObjectCount != ProjectObjects->Count))
	    MyProject->BuildTreeView();

    OldDataSetCount = ProjectDataSets->Count;
	OldProjectObjectCount = ProjectObjects->Count;    
#endif

    // Make sure that the small SynthGraphic windows have been removed.
    MainCompiler->RemoveDisplayGraphics();

    ProcessTimer->Enabled = false;

	BaseSystem->Stop();

    TimedEventType = etNoEvent;
    CancelCompile = true;
    IsStopping = false;
	StopSignalsSent = false;

	if (MyProject->Compiling)
    {
		MainCompiler->Reset();
		MyProject->Compiling = false;
    }

    // Only executing one compiled project.  Terminate Viva.
    if (RunMode != rmEdit)
#if VIVA_IDE
        MainForm->Close();
#else
        UnInitProgram();
#endif

    return true;
}


FileTypeEnum VivaGlobals::GetFileType(AnsiString FileName)
{
    AnsiString		extension = GetFileExtension(FileName);

    if (extension != strNull)
    {
        for (int i = 0; i < ftLast; i++)
            if (extension == FilterStrings[i][1])
                return (FileTypeEnum) i;
    }

    // If none of these fit the bill, then return code for invalid entry.
    return ftUndetermined;
}


// Execute a Viva Program
bool VivaGlobals::VivaExec(bool IWait, AnsiString FileName, UINT Param, int *ExitCode)
{
    STARTUPINFO    lpStartUpInfo;

    memset(&lpStartUpInfo, 0, sizeof(STARTUPINFO));
	lpStartUpInfo.wShowWindow = SW_SHOWNORMAL;
    lpStartUpInfo.cb = sizeof(STARTUPINFO);

    AttCalculator->CalculateAttribute(FileName);

    PROCESS_INFORMATION		procinfo;
    AnsiString      		VivaFileName
		= (GetFileType(ReplaceAll(FileName, "\"", strNull)) == ftCompiledProject)
		? VivaRunFileName
		: Application->ExeName;
    AnsiString				strCmd = VivaFileName + " " + FileName + " /" + Param;

    if (!CreateProcess(NULL, strCmd.c_str(), 0, 0, 0, 0, 0, 0, &lpStartUpInfo, &procinfo))
    {
        ErrorTrap(4045, FileName);
        return false;
    }

    if (!IWait)
    {
        // return the process information
        PROCESS_INFORMATION    *StaticProcessInfo = new PROCESS_INFORMATION();

        *StaticProcessInfo = procinfo;
        *ExitCode = (int)StaticProcessInfo;
        return true;
    }

    WaitForSingleObject(procinfo.hProcess, INFINITE);

    if (!GetExitCodeProcess(procinfo.hProcess, (unsigned long *) ExitCode))
        return false;

    CloseHandle(procinfo.hProcess);
    CloseHandle(procinfo.hThread);
    return true;
}


void VivaGlobals::GetUniqueEventName(char *EventName, unsigned long ProcessID)
{
    sprintf(EventName,"VivaEvent%u",ProcessID);
}


void VivaGlobals::InitProgram()
{
#if VIVA_IDE
	SettingsDialog->LoadEnabledMessages();

    OrigOpenAsSheetCaption = MainForm->OpenAsSheet->Caption;
    OrigEditAttributesCaption = MainForm->EditAttributes->Caption;
    OrigDeleteObjectCaption = MainForm->DeleteObject->Caption;
	OrigLocateObjectCaption = MainForm->MessageLocateObject01->Caption;
#endif

    DWORD	CurrentProcessID = GetCurrentProcessId();
    char	EventName[SYNCRO_EVENT_NAME_SIZE];

    GetUniqueEventName(&EventName[0], (unsigned long) CurrentProcessID);
    Sleep(5);
    ThisProcessEvent = OpenEvent(EVENT_ALL_ACCESS, true, EventName);

    ExitCode = 0;

    AttCalculator = new AttributeCalculator();

    ProcessTimer = new TVivaTimer(NULL);
    ProcessTimer->Enabled = False;
    ProcessTimer->Interval = 33;

    CoCreateInstance(CLSID_StdComponentCategoriesMgr, NULL, CLSCTX_INPROC_SERVER
    , IID_ICatInformation, (void **) &ComCategoryInformation);

    Screen->MenuFont->Name = "Lucida Console"; // 'cuz it's fixed width.
    										   // 	Others are "Courier" "Terminal" "FixedSys"

    NullNodeList = new NodeList;
    NullNodeList->Capacity = 0;

    ResolvedObjects = new TStringList;
    ResolvedObjects->Sorted = true;
    ResolvedObjects->Duplicates = dupAccept;

	// Load portio library and initialize pointers
    hDlPortIO = LoadLibrary("Dlportio.dll");
    
    if (hDlPortIO != NULL)
    {
        lpDlReadPortUshort = (LPDLREADPORTUSHORT)GetProcAddress(
                                        hDlPortIO, "DlPortReadPortUshort");
        if (!lpDlReadPortUshort)
        {
            // handle the error
            FreeLibrary(hDlPortIO);
            ErrorTrap(3047);
        }

        lpDlWritePortUchar = (LPDLWRITEPORTUCHAR)GetProcAddress(
                                        hDlPortIO, "DlPortWritePortUchar");
        if (!lpDlWritePortUchar)
        {
            // handle the error
            FreeLibrary(hDlPortIO);
            ErrorTrap(3047);
        }

        lpDlWritePortUshort = (LPDLWRITEPORTUSHORT)GetProcAddress(
                                        hDlPortIO, "DlPortWritePortUshort");
        if (!lpDlWritePortUshort)
        {
            // handle the error
            FreeLibrary(hDlPortIO);
            ErrorTrap(3047);
        }
    }

#if VIVA_IDE
    MainForm->InitializeRecentFileMenu();
    MainForm->PopulateRecentFileMenu();
#endif

	MyProject = NULL;

    DatasetTreeGroups = new TStringList;
    DatasetTreeGroups->Sorted = true;
    DatasetTreeGroups->Duplicates = dupIgnore;

    ObjectTreeGroups = new TStringList;
    ObjectTreeGroups->Sorted = true;
    ObjectTreeGroups->Duplicates = dupIgnore;

    ExpandedNodes = new TStringList;
    ExpandedNodes->Sorted = true;
    ExpandedNodes->Duplicates = dupIgnore;

    ComLibraries = new TStringList;
    ComLibraries->Sorted = true;

    TimedEventType = etNoEvent;

#if VIVA_IDE == FALSE
    if (_argc < 2)
    {
        ErrorTrap(4014);  // no file specifed on command line
        UnInitProgram();  // Shuts VivaRun down.
    }
#endif

    AnsiString	strArg = strNull;

    //  Look at command-line parameters for file names and codes:
    for (int i = 1; i < _argc; i++)
    {
        char    *uarg = _argv[i];

        if (uarg[0] == '/')
        {
            CMDLineCom.sprintf(uarg);
            CMDLineCom = CMDLineCom.SubString(2, CMDLineCom.Length() - 1);
        }
        else
        {
        	// If there is more than one non-paramater, then try to recover by assuming
            //		the file name contained a space and the file name was not put is quotes.
        	if (strArg == strNull)
            	strArg = uarg;
            else
	            strArg += strSpace + uarg;
        }
    }

	AttCalculator->CalculateAttribute(strArg);

    // If Viva is run on a ".bit" file, then initialize the requested chip(s).
    if (GetFileExtension(strArg) == "bit")
    {
        RunMode = rmProgramChips;

        // Extract the ChipSelect parameter.
        int		ChipSelect = VStrToInt(CMDLineCom);

        if (ChipSelect > 0)
        {
			// Send the ".bit" file to the requested chip(s).
	        ProgramNewStyleBoard(NULL, strArg, ChipSelect);
        }
        else
        {
			// Must provide a positive integer ChipSelect parameter.
            ErrorTrap(134, CMDLineCom);
        }

		// All done!  Close down the program.
        UnInitProgram();
	    exit(EXIT_SUCCESS);
    }

#if VIVA_IDE
	// Does the user want to open the last project at Viva start up?
    if (LoadLastProject &&
	    strArg == strNull &&
        LastProjectName != strNull)
    {
    	// If the last file no longer exists, then warn the user.
    	if (FileExists(LastProjectName))
			strArg = LastProjectName;
        else
			ErrorTrap(23, LastProjectName).AutoModal = true;
    }

    if (FileExists(strArg))
    {
        FileTypeEnum    FileType = GetFileType(strArg);

        if (FileType == ftProject && CMDLineCom != strNull)
        {
            RunMode = rmExecute;
        }
		else if (FileType == ftCompiledProject)
        {
            RunMode = rmExecute;
        }

        if (FileType == ftProject || FileType == ftCompiledProject || FileType == ftSheet)
        {
            MainForm->OpenDialog->FileName = strArg;
            MainForm->OpenFile(FileType, true);
        }
    }
    else if (strArg != strNull)
    {
		ErrorTrap(23, strArg).AutoModal = true;  // invalid compiled behavior
        UnInitProgram();
    }

    if (MyProject == NULL)
        new Project(1);

    if (RunMode == rmExecute &&
    	MyProject->CompiledSheet == NULL)
	        RunMode = rmCompile;

#else
    if (GetFileType(strArg) != ftCompiledProject)
    {
    	// VivaRun can only execute .vex files.
        ErrorTrap(4013, strArg);
        UnInitProgram();
    }
    else if (!FileExists(strArg))
    {
    	// File does not exist.
        ErrorTrap(4076, strArg);
        UnInitProgram();
    }
    else
    {
    	bool	InputFileWasOpened = false;

    	// The extra set of brackets fixes 2 memory leaks by deallocating ProjectLoader before
        //		UnInitProgram() is called.
    	{
            VTextBase	ProjectLoader(ftCompiledProject);

            if (ProjectLoader.OpenInputFile(strArg))
            {
                InputFileWasOpened = true;

            	new Project(0, &ProjectLoader, strArg, true);
            }
        }

        if (!InputFileWasOpened)
			UnInitProgram();

        if (MyProject != NULL &&
        	MyProject->CompiledSheet != NULL)
        {
            RunMode = rmExecute;
        }
        else
        {
            ErrorTrap(4015);  // invalid compiled behavior
            UnInitProgram();
        }
    }
#endif
}


void VivaGlobals::UnInitProgram()
{
    if (BaseSystem != NULL)
        BaseSystem->Stop();

    delete DatasetTreeGroups;
    delete ObjectTreeGroups;
    delete ExpandedNodes;

    delete MyProject;
    MyProject = NULL;

    delete MainCompiler;

    delete ResolvedObjects;

    delete NullNodeList;
    delete FunctionDescription;

    delete X86SystemClocks;
    delete RuntimeWideStrings;
    delete UntriggeredStopList;
    delete ComDispatchInvokeList;

    while (HeadRecycle != NULL)
    {
        EventLink    *SaveHeadRecycle = HeadRecycle;

        HeadRecycle = HeadRecycle->Next;
		delete SaveHeadRecycle;
    }

    // Delete the master list of all Hierarch objects.
    for (int i = 0; i < HierarchList->Count; i++)
        delete (VDismember *) HierarchList->Items[i];

    delete HierarchList;
    delete LocateObjects;
    delete DisabledErrors;
    delete DisabledModalResults;
    delete ErrorMessages;
    delete NodeStringTable;
    delete ResourceStringTable;
    delete ResourceTypeStringTable;
    delete SystemStringTable;
    delete GlobalLabels;
    delete ProcessTimer;

#if VIVA_IDE
    MainForm->Enabled = false;
#else
	if (WidgetForm != NULL)
	    WidgetForm->Close(); // Will usually halt execution, since it is the only form.
#endif

	if (MainForm != NULL &&
    	MainForm->RecentFileMenuItemList != NULL)
    {
    	TCloseAction	Action;

		MainForm->FormClose(NULL, Action);

	    delete MainForm;
	    MainForm = NULL;
    }

    delete IniFile;
    delete ComLibraries;
    ComLibraries = NULL;
    delete GlobalComObjects;
    delete GlobalComObjectStubs;
    delete ComObjects; // Read-only list!  Does not own its entries.
    delete ComI2adlList; // Read-only list!  Does not own its entries.
    delete ComForms;
    delete RuntimeComForms;
    delete UsedGlobalObjectNames;
    delete AttCalculator;

    delete TokenList;

    exit(ExitCode);
}


void VivaGlobals::SetStatusCaption(AnsiString Cap, bool ProcMsg)
{
#if VIVA_IDE
	MainForm->mainStatusBar->Panels->Items[1]->Text = " " + Cap;

	if (ProcMsg)
	    Application->ProcessMessages();
#endif
}


// This routine displays/hides the compiler progress information.
//     To hide the compiler progress panel set Pos to zero.
//     To display the number of objects processed set Pos to -1.
//     To display the progress as a percent set Max to -1 (the default).
//     Otherwise, the two progress number are displayed.
void VivaGlobals::SetProgPos(int Pos, int Max)
{
#if VIVA_IDE
	if (Pos == 0)
	{
		MainForm->mainStatusBar->Panels->Items[0]->Width = 0;
	}
	else
	{
		MainForm->mainStatusBar->Panels->Items[0]->Width = 200;

        if (Pos == -1)
			MainForm->mainStatusBar->Panels->Items[0]->Text =
            	"  " + IntToStr(Max) + " objects expanded";
		else if (Max > 0)
			MainForm->mainStatusBar->Panels->Items[0]->Text =
				"  On " + IntToStr(Pos) + " of " + IntToStr(Max) + " known";
		else
			MainForm->mainStatusBar->Panels->Items[0]->Text =
            	"  " + IntToStr(Pos) + "%";
	}
#endif
}


bool VivaGlobals::NodeListEqual(NodeList *IA, NodeList *IB, bool CompareNames)
{
    int    Count = IA->Count;

    if (Count != IB->Count)
        return false;

    for (int i = 0; i < Count; i++)
    {
    	Node	*NodeA = IA->Items[i];
    	Node	*NodeB = IB->Items[i];

        // Do the data sets match?
        if (NodeA->DSet != NodeB->DSet)
			return false;

        // This allows node name mismatches to be fixed by simply reading in the new CoreLib.
        if (CompareNames)
            if (NodeA->Name != NodeB->Name)
                return false;
    }

    return true;
}


// Compute the lengths of all of the data sets.
void VivaGlobals::ComputeDataSetLengths()
{
    // Make sure the the DataSetLengths are current.
    for (int i = 0; i < ProjectDataSets->Count; i++)
        ((DataSet *) ProjectDataSets->Items[i])->CalcBitLength();
}


// Check for unused data sets.  Ask the user if they should be deleted.
//     Calling program must load OldDataSetCount with the number of data sets before
//     the merge into ProjectDataSets.
void VivaGlobals::CheckDataSetUsage()
{
    // Flag data sets as being used if their parent has been used.
	//     For loop counts backwards to save creating a recursive routine.
    if (OldDataSetCount >= ProjectDataSets->Count)
        return;

    for (int i = OldDataSetCount; i < ProjectDataSets->Count; i++)
    {
        DataSet    *ProjectDataSet = (DataSet *) ProjectDataSets->Items[i];

        if (ProjectDataSet->Used)
            ProjectDataSet->SetChildIsVisited();
    }

    // Check the new data sets to see if they have been used.
    bool    UserPrompted = (DeleteUnusedDataSets != soPrompt);
    bool    DeleteUnused = (DeleteUnusedDataSets == soYes);

    for (int i = OldDataSetCount; i < ProjectDataSets->Count; i++)
    {
        DataSet    *ProjectDataSet = (DataSet *) ProjectDataSets->Items[i];

        if (!ProjectDataSet->Used)
        {
            // When find the first unused DataSet, ask the user if it should be deleted.
            if (!UserPrompted)
            {
                UserPrompted = true;
                DeleteUnused = (ErrorTrap(3032) == mrYes);
            }

            if (DeleteUnused)
            {
                // Delete the unused data set and its exposer/collector.
                ErrorTrap(3049, ProjectDataSet->Name);

                ProjectDataSet->Exposer->DeleteProjectObject();
                ProjectDataSet->Collector->DeleteProjectObject();

                ProjectDataSets->Delete(i);
                delete ProjectDataSet;

                i--;
				continue;
            }
        }

        // Data set is to be kept.  Finish building it if necessary.
        ProjectDataSet->BuildExposerAndCollector();
    }
}


// Remove all of the nodes from the tree view without deallocating memory.
//     The tree is made invisible because large trees take a long time to reset.
void VivaGlobals::ClearTreeView(TTreeView *TreeView)
{
    bool    HoldVisible = TreeView->Visible;

    TreeView->Visible = false;
    TreeView->Selected = NULL;
    TreeView->Items->Clear();
    TreeView->Visible = HoldVisible;
}


// This routine was written because TTreeNode::CustomSort() method only sorts one level down.
void VivaGlobals::FullCustomSort(TTreeNode *TreeNode, bool ExpandNode)
{
    if (ExpandNode)
        TreeNode->Expanded = true;

    // Sort our children.
    TreeNode->CustomSort((PFNTVCOMPARE) CustomSortCompare, 0);

    TTreeNode    *ChildNode = TreeNode->getFirstChild();

    while (ChildNode != NULL)
    {
        // Recursive call to sort grandchildren.
        if (ChildNode->Data == TREE_GROUP_NODE)
            FullCustomSort(ChildNode);

        ChildNode = ChildNode->getNextSibling();
    }
}


// Custom tree sort routine to have tree group nodes listed before object nodes.
//     Handles ObjectTree and DataSetTree.
int __stdcall VivaGlobals::CustomSortCompare(TTreeNode *Node1, TTreeNode *Node2, long Dummy)
{
    int    NodeSortLevel1 = GetNodeSortLevel(Node1);
    int    NodeSortLevel2 = GetNodeSortLevel(Node2);

    if (NodeSortLevel1 == NodeSortLevel2)
    {
        return AnsiStrIComp(Node1->Text.c_str(), Node2->Text.c_str());
    }
    else
    {
        return (NodeSortLevel1 > NodeSortLevel2);
    }
}


// Returns the node's custom sort level.
//     Level 1 - Special objects/data sets that are used a lot
//     Level 2 - Tree group nodes
//     Level 3 - Regular tree nodes
int VivaGlobals::GetNodeSortLevel(TTreeNode *TreeNode)
{
    void    *TreeNodeData = TreeNode->Data;

    if (TreeNodeData == TREE_GROUP_NODE)
        return 2;

    if (TreeNodeData == FOOTPRINT_TREE_NODE)
        return 3;

    if (TreeNode->TreeView == MainForm->DataSetTree)
	{
        if (TreeNodeData == BitDataSet  ||
            TreeNodeData == NULLDataSet  ||
            TreeNodeData == VariantDataSet)
        {
            return 1;
        }
    }
    else
    {
        if (TreeNodeData == MainInputObject  ||
            TreeNodeData == MainOutputObject  ||
            TreeNodeData == MainSystemCastObject  ||
            TreeNodeData == VariantDataSet->Collector  ||
            TreeNodeData == VariantDataSet->Exposer)
        {
            return 1;
        }
    }

    return 3;
}


// Sets the ImageIndex and the SelectedIndex to the value passed.
void VivaGlobals::SetImageIndex(TTreeNode *TreeNode, int ImageIndex)
{
    TreeNode->ImageIndex = ImageIndex;
    TreeNode->SelectedIndex = ImageIndex;
}


// Returns true if TreeNode is inside a root tree node named RootTreeNodeName.
//		If it MustBeTreeGroup, then IT must be a non root TREE_GROUP_NODE.
bool VivaGlobals::InsideRootTreeNode(AnsiString &RootTreeNodeName, TTreeNode *TreeNode, bool
    MustBeTreeGroup)
{
    if (TreeNode == NULL)
        return false;

	// Does it have to be a tree group?
    if (MustBeTreeGroup)
        if (TreeNode->Parent == NULL || TreeNode->Data != TREE_GROUP_NODE)
			return false;

    while (TreeNode->Parent != NULL)
        TreeNode = TreeNode->Parent;

    return (TreeNode->Text == RootTreeNodeName);
}


// Add the object into the tree view with the correct object icon.
//     The new node is returned.
TTreeNode *VivaGlobals::AddObjectToTree(TTreeNode *ParentNode, I2adl *Object)
{
    int		NewImageIndex = (Object->IsPrimitive)		? NewImageIndex = 12  // Primitive
							: (Object->IsVariant())		? NewImageIndex = 15  // Variant
														: NewImageIndex = 14; // Composite

    TTreeNode    *NewNode = ParentNode->Owner->AddChildObject(ParentNode, Object->
        EnhancedName(), Object);

    SetImageIndex(NewNode, NewImageIndex);

    return NewNode;
}


// Create the pre-defined DataSets and add them to ProjectDataSets.
void VivaGlobals::InitSGDataSets() 
{
    for (int i = 0; i < SGDataSetCount; i++)
        SGDataSets[i] = NULL;

    // Child data sets for recursive data sets (Bit & Variant) are automatically added.
    GetOrMakeDataSet(sdsBit,           strNull,     sdsNULL,   sdsNULL,   0x2B2BFF);
    GetOrMakeDataSet(sdsVariant,       strNull,     sdsNULL,   sdsNULL,   0xFF26DA);
    GetOrMakeDataSet(sdsList,          strNull,     sdsVariant,sdsVariant,16711808);

    // NULL is created after Variant and List because the data set numbers are hard coded
    //     in CoreLib (Div_Test and Mul_Test).
    GetOrMakeDataSet(sdsNULL,          strNull,     sdsNULL,   sdsNULL,   0xFFFFFF);

    // Finish creating the data sets that have special requirements.
    BitDataSet     = SGDataSets[sdsBit];
    VariantDataSet = SGDataSets[sdsVariant];
    ListDataSet    = SGDataSets[sdsList];
    NULLDataSet    = SGDataSets[sdsNULL];

    GetOrMakeDataSet(sdsBIN,           strSystemBIN,sdsVariant,sdsVariant,ColorCodeBIN);
    GetOrMakeDataSet(sdsLSB,           strSystemLSB,sdsBit,    sdsVariant,ColorCodeLSB);
    GetOrMakeDataSet(sdsMSB,           strSystemMSB,sdsVariant,sdsBit,    ColorCodeMSB);
    GetOrMakeDataSet(sdsDBit,          strStatic,   sdsBit,    sdsBit,    0x1E1ECC);
    GetOrMakeDataSet(sdsNibble,        strStatic,   sdsDBit,   sdsDBit,   0x000080);
    GetOrMakeDataSet(sdsByte,          strStatic,   sdsNibble, sdsNibble, 0x26DAFF);
    GetOrMakeDataSet(sdsWord,          strStatic,   sdsByte,   sdsByte,   0x26FF39);
    GetOrMakeDataSet(sdsDWord,         strStatic,   sdsWord,   sdsWord,   0x18A824);
    GetOrMakeDataSet(sdsQWord,         strStatic,   sdsDWord,  sdsDWord,  0x005504);
    GetOrMakeDataSet(sdsInt,           strStatic,   sdsByte,   sdsByte,   0xFFFF00, ATT_SIGNED);
    GetOrMakeDataSet(sdsDInt,          strStatic,   sdsWord,   sdsWord,   0xFF8000, ATT_SIGNED);
    GetOrMakeDataSet(sdsQInt,          strStatic,   sdsDWord,  sdsDWord,  0x954A00, ATT_SIGNED);
    GetOrMakeDataSet(sdsComInt,        strComPredefined, sdsWord,   sdsWord,   0xFF8000, ATT_SIGNED);
    GetOrMakeDataSet(sdsComUInt,       strComPredefined, sdsWord,   sdsWord,   0x18A824);

#if VIVA_IDE
    // Automatically add the first 128 pattern data sets.
	for (int i = 2; i < 129; i++)
		GetOrMakePatternDataSet(sdsBIN, i);

	GetOrMakePatternDataSet(sdsMSB, 128);
	GetOrMakePatternDataSet(sdsLSB, 128);
#endif

    SGDataSets[sdsFloat] = GetOrMakeDataSet(SGDataSetNames[sdsFloat], strStatic, sgPredefined,
    GetOrMakePatternDataSet(sdsMSB, 23), GetOrMakePatternDataSet(sdsMSB, 9), 0x700000, ATT_FLOAT);

	SGDataSets[sdsDouble] = GetOrMakeDataSet(SGDataSetNames[sdsDouble], strStatic, sgPredefined,
        GetOrMakePatternDataSet(sdsMSB, 52), GetOrMakePatternDataSet(sdsMSB, 12), 0xC00000,
        ATT_FLOAT);

    GetOrMakeDataSet(sdsBinaryString,  strComPredefined,   sdsDWord,  sdsNULL,   0x0050FF);
    GetOrMakeDataSet(sdsBool,          strComPredefined,   sdsDWord,  sdsNULL,   0xB0B0B0);
    GetOrMakeDataSet(sdsComVariant,    strComPredefined,   sdsInt,    sdsWord,   0xE02000);
    // ? Are the child data sets in the right order?
    GetOrMakeDataSet(sdsCurrency,      strComPredefined,   sdsDWord,  sdsDInt,   0x40A000, ATT_FIXED);
    // ? Is this the right child data set?
    GetOrMakeDataSet(sdsDate,          strComPredefined,   sdsQWord,  sdsNULL,   0x0070B0);
    // ? Are the child data sets the correct proportions?
    GetOrMakeDataSet(sdsEnumeration,   strComPredefined,   sdsDWord);
    GetOrMakeDataSet(sdsHResult,       strComPredefined,   sdsDInt,   sdsNULL,   0x20FF40);
    GetOrMakeDataSet(sdsIDispatch,     strComPredefined,   sdsDWord,  sdsNULL,   0x500050);
    GetOrMakeDataSet(sdsIUnknown,      strComPredefined,   sdsDWord,  sdsNULL,   0x555555);
    GetOrMakeDataSet(sdsPointer,       strComPredefined,   sdsDWord);
    GetOrMakeDataSet(sdsSignedByte,    strComPredefined,   sdsNibble, sdsNibble, 0x30FFFF, ATT_SIGNED);
    GetOrMakeDataSet(sdsUnsupported,   strComPredefined,   sdsDWord);
    GetOrMakeDataSet(sdsWideChar,      strComPredefined,   sdsWord);
    GetOrMakeDataSet(sdsWideStringPtr, strComPredefined,   sdsDWord,  sdsNULL,   0x00B0FF);

    SGDataSets[sdsBinaryString]->PointsToDS = SGDataSets[sdsWideChar];
    SGDataSets[sdsWideStringPtr]->PointsToDS = SGDataSets[sdsBinaryString];

    SGDataSets[sdsComVariant]->ChildDataSets->Add(SGDataSets[sdsWord]);
    SGDataSets[sdsComVariant]->ChildDataSets->Add(SGDataSets[sdsWord]);
    SGDataSets[sdsComVariant]->ChildDataSets->Add(SGDataSets[sdsQWord]);

    SGDataSets[sdsUnsupported]->VarType = VT_VARIANT;

    for (USHORT i = 0; i < VT_LPSTR ; i++)
    {
        DataSet    *DS = SGDataSets[i];

        if (DS != NULL)
            DS->VarType = i;
    }
}


// This routine allows a predefined data set to be created with one line of code.
void VivaGlobals::GetOrMakeDataSet(SGDataSetEnum _Name, AnsiString TreeGroup, SGDataSetEnum
    ChildDataSet1, SGDataSetEnum ChildDataSet2, int _ColorCode, unsigned int _AttributeFlags
    , SystemGeneratedEnum _SystemGenerated)
{
    SGDataSets[_Name] = GetOrMakeDataSet(SGDataSetNames[_Name], TreeGroup, _SystemGenerated,
        SGDataSets[ChildDataSet1], SGDataSets[ChildDataSet2], _ColorCode, _AttributeFlags);
}


// Returns the requested data set.  If the data set doesn't exist, then it will be created.
//     Warning!  Calling routine must call BuildDataSetExposer() after adding child data sets.
DataSet *VivaGlobals::GetOrMakeDataSet(AnsiString _Name, AnsiString TreeGroup,
    SystemGeneratedEnum _SystemGenerated, DataSet *ChildDataSet1, DataSet *ChildDataSet2,
    int _ColorCode, unsigned int _AttributeFlags)
{
    // Does the data set already exist?
    DataSet    *ReturnDataSet = GetDataSet(_Name);

    if (ReturnDataSet != NULL)
        return ReturnDataSet;

    // Create a new data set.
    ReturnDataSet = new DataSet(_Name, TreeGroup, _ColorCode, _AttributeFlags, _SystemGenerated);

    // Were child data sets provided?
    if (ChildDataSet1 != NULL && ChildDataSet1 != SGDataSets[sdsNULL])
    {
        ReturnDataSet->ChildDataSets->Add(ChildDataSet1);

        if (ChildDataSet2 != NULL && ChildDataSet2 != SGDataSets[sdsNULL])
            ReturnDataSet->ChildDataSets->Add(ChildDataSet2);
    }

    return ReturnDataSet;
}


// Get or make the requested pattern data set.
//     This routine does not call GetOrMakeDataSet() because it must see if the data set
//     already exists before the child data sets are created.
DataSet *VivaGlobals::GetOrMakePatternDataSet(SGDataSetEnum Pattern, int DataSetBitLength)
{
    // Terminate the recursion with the Bit data set.
    if (DataSetBitLength <= 1)
        return BitDataSet;

    // Make sure the bit length is in bounds.
    if (DataSetBitLength >= 0x10000)
    {
    	ErrorTrap(193, AnsiString(SGDataSetNames[Pattern] + IntToStr(DataSetBitLength)));

    	return BitDataSet;  // Attempt recovery.
    }

    // Does the data set already exist?
    //     Pad bit length with leading zeros to be at least 3 digits.
    //     Calling GetDataSet() would cause an infinite loop.
    int			PatternIndex = Pattern - sdsBIN;
	DataSet		*ReturnDataSet = PatternDataSets[PatternIndex][DataSetBitLength];

	if (ReturnDataSet != NULL)
    	return ReturnDataSet;

    AnsiString    DataSetName = (DataSetBitLength < 1000)
                  ? SGDataSetNames[Pattern] + IntToStr(DataSetBitLength + 1000).SubString(2,3)
                  : SGDataSetNames[Pattern] + IntToStr(DataSetBitLength);

    if (Pattern == sdsBIN)
    {
        // Generate a new BINary data set.
        ReturnDataSet = new DataSet(DataSetName, strSystemBIN, ColorCodeBIN, ATT_UNSIGNED,
            sgPredefined);

        int    HalfBitLength = DataSetBitLength / 2;

        // Recursive call to create and add the child data sets.
        ReturnDataSet->ChildDataSets->Add(GetOrMakePatternDataSet(Pattern, DataSetBitLength -
            HalfBitLength));
        ReturnDataSet->ChildDataSets->Add(GetOrMakePatternDataSet(Pattern, HalfBitLength));
    }
	else
    {
    	// The following fixes a stack overflow from trying to recursively create 10,000
        //		pattern data sets all at once.
        for (int i = 1; i < DataSetBitLength; i++)
			if (PatternDataSets[PatternIndex][i] == NULL)
	            GetOrMakePatternDataSet(Pattern, i);

    	if (Pattern == sdsLSB)
        {
            // Generate a new Least Significant Bit data set.
            ReturnDataSet = new DataSet(DataSetName, strSystemLSB, ColorCodeLSB, ATT_UNSIGNED,
                sgPredefined);

            // Recursive call to create and add the child data sets.
            ReturnDataSet->ChildDataSets->Add(BitDataSet);
            ReturnDataSet->ChildDataSets->Add(GetOrMakePatternDataSet(Pattern, DataSetBitLength - 1));
        }
        else
        {
            // Generate a new Most Significant Bit data set.
            ReturnDataSet = new DataSet(DataSetName, strSystemMSB, ColorCodeMSB, ATT_UNSIGNED,
                sgPredefined);

            // Recursive call to create and add the child data sets.
            ReturnDataSet->ChildDataSets->Add(GetOrMakePatternDataSet(Pattern, DataSetBitLength - 1));
            ReturnDataSet->ChildDataSets->Add(BitDataSet);
        }
    }

    PatternDataSets[PatternIndex][DataSetBitLength] = ReturnDataSet;

    ReturnDataSet->BuildExposerAndCollector();

    // Flag all pattern data sets as being used because the data set destructor does not
    //     maintain the external PatternDataSets[] lists.
	ReturnDataSet->Used = true;

    return ReturnDataSet;
}


// Returns the requested data set (or NULL if it doesn't exist).
//     Pattern data sets will be automatically created.
DataSet *VivaGlobals::GetDataSet(AnsiString &_Name)
{
    for (int i = 0; i < ProjectDataSets->Count; i++)
    {
        DataSet    *DSet = (DataSet *) ProjectDataSets->Items[i];

        if (DSet->Name == _Name)
            return DSet;
    }

	// Data set does not exist.  If it is a pattern data set then automatically create it.
    int 			PatternSize;
    SGDataSetEnum	PatternType = GetPatternType(_Name, PatternSize);

    if (PatternType == sdsNULL)
	    return NULL;
    else
        return GetOrMakePatternDataSet(PatternType, PatternSize);
}


// Returns sdsNULL if the DataSetName is not a pattern.
//     Otherwise, it returns sdsBIN, sdsLSB or sdsMSB and the size of the pattern in PatternSize.
SGDataSetEnum VivaGlobals::GetPatternType(AnsiString &DataSetName, int &PatternSize)
{
	PatternSize = 0;

    // Pattern data sets have a 3 character prefix and at least 3 digit suffix.
    if (DataSetName.Length() < 6)
	    return sdsNULL;

    // Is this a valid pattern prefix?
    AnsiString		PatternName = DataSetName.SubString(1, 3);
    SGDataSetEnum	PatternType;

    if (PatternName == SGDataSetNames[sdsBIN])
        PatternType = sdsBIN;
    else if (PatternName == SGDataSetNames[sdsLSB])
        PatternType = sdsLSB;
    else if (PatternName == SGDataSetNames[sdsMSB])
        PatternType = sdsMSB;
    else
        return sdsNULL;

    // Is the suffix numeric?
    try
    {
    	// This "if" statement catches a lot of exceptions when running in debug mode.
		if (DataSetName[DataSetName.Length()] >= '0')
			PatternSize = StrToInt(DataSetName.SubString(4, DataSetName.Length() - 3));
    }
    catch(...)
    {
        return sdsNULL;
    }

    // Is the suffix in the range of valid pattern sizes?
    if (PatternSize < 2 ||
    	PatternSize >= 0x10000)
    {
    	PatternSize = 0;
        return sdsNULL;
    }

    return PatternType;
}


// This is how we completely remove a behavior from this project.  Goes thru all behaviors, and
//      replaces references to this behavior with ResolveObject if [ResolveRefs] is true,
//      with [ReplaceWith] otherwise.
//      Call this function AFTER you have removed its I2adl from ProjectObjects.
void VivaGlobals::RemoveBehavior(I2adlList *DefunctBehavior, bool ResolveRefs
, I2adlList *ReplaceWith)
{
    if (DefunctBehavior == NULL)
        return;

    // Loop thru every behavior ProjectObjects and MyProject->Wip to find references:
    VList	*List = ProjectObjects; // Changes to MyProject->Wip in second iteration.

    for (int h = 0; h < 2; h++)
    {
        for (int i = 0; i < List->Count; i++)
        {
            I2adl		*obj = (I2adl *) List->Items[i];

            if (obj == NULL)
            	continue;

			I2adlList	*Behavior = obj->Behavior;

            if (Behavior == NULL)
                continue;

            for (int j = 0; j < Behavior->Count; j++)
            {
                I2adl	*I2adlObject = (I2adl *) Behavior->Items[j];

                if (I2adlObject == NULL)
                    continue;

                if (I2adlObject->Behavior == DefunctBehavior)
                {
                    if (ResolveRefs)
                    {
                        I2adl	*ResObj = I2adlObject->ResolveObject(false);

                        I2adlObject->Behavior = (ResObj == NULL) ? NULL
                                                                 : ResObj->Behavior;
                    }
                    else
                    {
                        I2adlObject->Behavior = ReplaceWith;
                    }
                }
            }
        }

        List = MyProject->Wip;
    }

    DefunctBehavior->DeleteObjects();
    delete DefunctBehavior;
}


// Tries to encode Path into a $(DirName:<DirCode>) macro.
//		Will not check for $(DirName:SystemDesc) because it can change and is normally inside
//		the Viva directory.
AnsiString VivaGlobals::EncodePath(AnsiString Path, EncodePathEnum ProjectDirPrecedence)
{
	int			PathLen = Path.Length();
	AnsiString	UpperCasePath = Path.UpperCase();

    if (UpperCasePath.Pos(strVivaSystemDir.UpperCase()) == 1)
        return "$(DirName:VivaSystem)" + Path.SubString(strVivaSystemDir.Length() + 1, PathLen);

    if (ProjectDirPrecedence == epProjectBeforeViva &&
    	UpperCasePath.Pos(MyProject->FilePath.UpperCase()) == 1)
	        return "$(DirName:Project)" + Path.SubString(MyProject->FilePath.Length() + 1, PathLen);

    if (UpperCasePath.Pos(strVivaDir.UpperCase()) == 1)
		return "$(DirName:Viva)" + Path.SubString(strVivaDir.Length() + 1, PathLen);

    if (ProjectDirPrecedence == epProjectAfterViva &&
    	UpperCasePath.Pos(MyProject->FilePath.UpperCase()) == 1)
	        return "$(DirName:Project)" + Path.SubString(MyProject->FilePath.Length() + 1, PathLen);

    if (UpperCasePath.Pos(GetBuildDir().UpperCase()) == 1)
		return "$(DirName:Build)" + Path.SubString(GetBuildDir().Length() + 1, PathLen);

    if (UpperCasePath.Pos(UserDir.UpperCase()) == 1)
		return "$(DirName:User)" + Path.SubString(UserDir.Length() + 1, PathLen);

    return Path;
}


// Returns the fully qualified path name for the FileName passed in.
AnsiString VivaGlobals::FullPathName(const AnsiString &FileName)
{
    AnsiString	DecodedFileName = FileName;

    AttCalculator->CalculateAttribute(DecodedFileName);

	if (DecodedFileName.Length() >= 2)
    {
		// If FileName starts with "\" (root directory) then it is fully qualified.
		if (DecodedFileName[1] == '\\')
           	return DecodedFileName;

        if (DecodedFileName.Length() >= 3)
        {
            // If FileName starts with a drive letter then it is fully qualified.
            if (DecodedFileName[2] == ':')
                return DecodedFileName;
        }
    }

    // For backwards compatibility, unqualified files are in the System directory.
    return strVivaSystemDir + DecodedFileName;
}


// Register a class into the window's registry.
void RegisterComponent(TComponentClass &xclass)
{
    RegisterComponents("Samples", &xclass, 0);
    RegisterClass(xclass);
}


// Base class StringEntry..Constructor
StringEntry::StringEntry(const AnsiString &_Name)
{
    Name = _Name;
    Object = NULL;
}


// StringTable .. constructor

_fastcall StringTable::StringTable()
    : TList()
{
    // Table always has a Null string
    Add(NewEntry(""));
}


// destructor
void _fastcall StringTable::BeforeDestruction()
{
    for (int i = 0; i < Count; i++)
        delete (StringEntry *) Items[i];
        
    Count = 0;
}


// Returns a pointer to the SearchString in the string table.
//     Creates a new entry if one doesn't already exist.
StringEntry *StringTable::Entry(const AnsiString &SearchString, bool AddIfNotFound)
{
    // Warn the user if the string table is empty (not preloaded with the NULL string).
    if (Count <= 0)
    {
        ErrorTrap(115);
        return NULL;
    }

    // Set Top & Bottom to be outside the Items[] entries to bound the binary search.
    int            Bottom = -1;
    int            Middle;
    int            Top = Count;
    StringEntry    *ReturnEntry;

    while (true)
    {
        Middle = (Bottom + Top) >> 1;
		ReturnEntry = (StringEntry *) Items[Middle];

		int    CompareResult = strcmp(SearchString.c_str(), ReturnEntry->Name.c_str());
        // .AnsiCompare(

        if (CompareResult == 0)
            return ReturnEntry;

        if (CompareResult < 0)
        {
            Top = Middle;

			if (Bottom + 1 >= Top)
			{
				// Insert before middle entry.
                break;
            }
        }
        else
        {
            Bottom = Middle;

			if (Bottom + 1 >= Top)
            {
				// Insert after middle entry.
                Middle++;
				break;
            }
        }
    }

    if (AddIfNotFound)
    {
        ReturnEntry = NewEntry(SearchString);
		Insert(Middle, ReturnEntry);
        return ReturnEntry;
	}

	return NULL;
}


// Place a new string in the string table, and return pointer to entry.
StringEntry *StringTable::NewEntry(const AnsiString &NewString)
{
    return new StringEntry(NewString);
};


StringEntry *TNodeStrTable::NewEntry(const AnsiString &NewString)
{
	StringEntry		*entry = new StringEntry(NewString);

#if VIVA_IDE
    entry->Object = (void *) short(I2adlView->GetTextWidth(NewString));
#endif

	return entry;
}


void __fastcall TVivaTimer::TimerTimer(System::TObject *Sender)
{
	if (JustCompiledObjectName != NULL)
		SetStatusCaption("Step 2 of 5; Expanding " + *JustCompiledObjectName);

    static bool     AmBusy = false;

	if (AmBusy)
        return;

    AmBusy = true;

    EventCycle(false);

    AmBusy = false;
}


// Perform an event cycle.
//		Because of the recursive nature of the calls to TComHelperEvent::Invoke(),
//		the ProcessingComEvent paramater cannot be a global variable.
bool VivaGlobals::EventCycle(bool ProcessingComEvent)
{
	switch (TimedEventType)
	{
		case etTranslate:
		{
			// The following is required to update the CancelCompile variable so the
            //     compile will stop faster.
			Application->ProcessMessages();

			ProcessTimer->Enabled = false;
			EdifExportFileName = strNull;	// Should already be NULL.

			try
			{
                // Translate the AssignedBehavior in each system.
                if (CancelCompile)
                	break;

                // Put all of the system clock inputs into X86SystemClocks.
                LoadX86SystemClocks();

			    if (MyProject->Compiling)
                 	BaseSystem->TranslatePrep();

                if (ShouldMakeEdifIDs)
                {
                    TStringList		*templist = new TStringList;
                    AnsiString		CompressionMapFileName = GetBuildDir() + "CompressionMap_"
                        + MyProject->Name + ".txt";

                    templist->Sorted = false;

                    if (FileExists(CompressionMapFileName))
                    {
                        templist->LoadFromFile(CompressionMapFileName);

                        TokenList->Sorted = false;
                        TokenList->Assign(templist);

                        for (int i = 0; i < TokenList->Count; i++)
                            TokenList->Objects[i] = (TObject *) i;

                        TokenList->Sorted = true;
                        BuiltTokenList = true;

                        BaseSystem->MakeEdifIDs();

                        int		oldcount = templist->Count;

                        for (int i = oldcount; i < TokenList->Count; i++)
                            templist->Add(strNull);

                        for (int i = 0; i < TokenList->Count; i++)
                        {
                            int		index = (int) TokenList->Objects[i];

                            if (index >= oldcount)
                                templist->Strings[index] = TokenList->Strings[i];
                        }

                        templist->SaveToFile(CompressionMapFileName);
                    }
                    else
                    {
	                    TokenList->Sorted = true;
                        BuiltTokenList = false;
                        BaseSystem->MakeEdifIDs();
                        BuiltTokenList = true;

                        for (int i = 0; i < TokenList->Count; i++)
                        {
                            AnsiString		weight(int(TokenList->Objects[i]));

                            for (int j = weight.Length(); j < 11; j++)
                                weight = "0" + weight;

                            templist->AddObject(weight,
                                (TObject *) new AnsiString(TokenList->Strings[i]));
                        }

                        TokenList->Clear();
                        TokenList->Sorted = false;
                        templist->Sorted = true;

						// templist is sorted in reverse, so iterate thru it backwards,
                        //		assigning its members to TokenList, which is the compression
                        //		map; the ordinal position of each element is its priority.
                        for (int i = templist->Count - 1; i >= 0; i--)
                        {
                            AnsiString		token(*((AnsiString *) templist->Objects[i]));

                            TokenList->AddObject(token, (TObject *) TokenList->Count);

                            delete (AnsiString *) templist->Objects[i];
                        }

                        if (TokenList->Count > 0)
                        {
       				        CreateDirectory(GetBuildDir().c_str(), NULL); // May or may not yet exist.
	                        TokenList->SaveToFile(CompressionMapFileName);
                        }

                        TokenList->Sorted = true;
                        BaseSystem->MakeEdifIDs();
                    }

                    delete templist;
				}

                // Translate the AssignedBehavior in each system.
                if (!CancelCompile)
                {
					// Free up the memory that is no longer needed.
                	MainCompiler->Reset();
					BaseSystem->Translate();
                }
			}
			catch(Exception &e)
			{
				// Inform the user how to recover from errors while compiling.
				ErrorTrap(133, e.Message);
				CancelCompile = true;
			}

			if (CancelCompile)
			{
				// The translation failed.  Stop the compile.
				MyProject->CompiledSheet = NULL;
				HaltExecution();
			}
			else if (IsEDIFExport)
			{
				// We are only creating an EDIF Export file.
                if (EdifExportFileName == strNull)
                {
                	// EDIF Export file could not be created because no FPGA system was used.
                	ErrorTrap(131);
                }
                else
                {
	           		// Inform the user the EDIF Export file was created.
		           	ErrorTrap(130, EdifExportFileName);
					EdifExportFileName = strNull;
                }

				MyProject->CompiledSheet = NULL;
				HaltExecution();
			}
			else
			{
				ProcessTimer->Enabled = true;

				// [FirstRun == true] indicates that we are playing a sheet directly opened
				//      from file.
				if (MyProject->FirstRun == true)
				{
					MyProject->FirstRun = false;
					TimedEventType = etInitialize;
				}
                else
                {
                    MyProject->CompiledSheet = CurrentSheet;

                    AnsiString	VexFileName = strNull;

                	// Are we saving the compile information in a ".Vex" file?
                    if (CreateVexFile &&
				    	MyProject->FilePath != strNull)
                    {
						VexFileName = MyProject->GetVexFileName(MyProject->CompilingSheetName);
                        MyProject->VexName = VexFileName;

                     	// The following keeps the file name from being incremented.
                    	int		OrigSettingFlags = SettingFlags;

						SettingFlags = SettingFlags & ~((int) SETTINGS_AUTO_VERSION);
					    MainForm->cmdSaveExecutableAsClick(NULL);

                        SettingFlags = OrigSettingFlags;
                    }

                    if (RunAfterCompile)
                    {
                        StartExecution(VexFileName);
                    }
                    else
                    {
				        HaltExecution();
                    }
                }

				MyProject->Compiling = false;

                // Reset the ambiguous object flag.
                for (int i = 0; i < ProjectObjects->Count; i++)
                {
                    I2adl    *ProjectObject = (*ProjectObjects)[i];

                    if (ProjectObject->Ambiguous == ambYesDiagnosed)
                        ProjectObject->Ambiguous = ambNo;
                }
			}

			// Remove the small SynthGraphic windows at the end of step 3.
			MainCompiler->RemoveDisplayGraphics();

#if VIVA_IDE
            // Remove the red "X" from the "Start/Cancel Synthesis" button.
            MainForm->cmdSynthesize->ImageIndex = 45;
            MainForm->MenuBtnSync(MainForm->cmdSynthesize);
#endif      

			break;
		}

		case etInitialize:
		{
            // In case we skipped translation, reoad the X86 system clocks.
            LoadX86SystemClocks();

			ProgramPensa();
			SetStatusCaption(strNull);
			Initializing = true;
            BaseSystem->Initialize();

            // Get the EmulatorSpeed from the base system.  Default it to 35 times a second.
            //     The EmulatorPeriod is the number of miliseconds.
			EmulatorSpeed = VStrToInt(BaseSystem->GetAttribute("EmulatorSpeed"), 35);

            // Prevent a "divide by zero" error.
            if (EmulatorSpeed < 1)
				EmulatorSpeed = 1;

            EmulatorPeriod = 1000 / EmulatorSpeed;

            // Get the InitClockCycles from the base system.  Default it to 1 up/down toggle.
			InitClockCycles = VStrToInt(BaseSystem->GetAttribute("InitClockCycles"), 1);

            // If VivaRun does not have a visable form, then offer to kill it now.
            if (RunMode == rmExecute)
            {
			    if (RuntimeComForms->Count <= 0 &&
                	WidgetForm->Widgets->Count <= 0 &&
		            ThisProcessEvent == NULL &&  // non exclusive $Spawn will kill children.
					ErrorTrap(43) == mrNo)
                {
                	TryHaltExecution();
			        UnInitProgram();
	    			exit(EXIT_FAILURE);
                }
            }

	        if (!CancelCompile)
            {
#if VIVA_IDE
                MainForm->cmdPlayStopSheet->ImageIndex = PlayStopSheetIcon + 1;
                MainForm->MenuBtnSync(MainForm->cmdPlayStopSheet);
#endif

                ProcessTimer->Enabled = true;
			}

            MyProject->CompiledSheet = CurrentSheet;
        }

        case etExecute:
        {
            // If there are no Stop objects nor a visible UI forms, then stop the behavior.
            if (IsStopping)
            {
    			if (TryHaltExecution())
                    return false;
            }
            else if (WidgetForm->Closed == true && WidgetForm->Initialized == true)
            {
                if (WidgetForm->Widgets->Count > 0 || RuntimeComForms->Count > 0)
                {
                    int		i;

                    for (i = 0; i < RuntimeComForms->Count; i++)
                    {
						if (((TComForm *) RuntimeComForms->Items[i])->Visible)
							break;
                    }

                    if (i >= RuntimeComForms->Count) // means that there is no visible COM Form.
                    {
						IsStopping = true;
                    }
                }

                WidgetForm->Visible = false;
            }

            // Execute all of the pending events in each system.
            if (!IsRunning)
            	break;

            int		ClockDisable = FPGActive | X86Active;
            int		ClockChangeCount = -1;
            int		TimesClockSkipped = 0;
            DWORD	ExecuteStartTime = timeGetTime();

            while (IsRunning)
            {
                // If all of the events have been processed, then it is time to change
                //     the clock.
                if (ClockDisable == 0)
                {
                	TimesClockSkipped = 0;

                    // The clock has to change twice for a complete clock cycle.
                    ClockChangeCount++;

                    if (ClockChangeCount > 0 &&
                       (ClockChangeCount % 2) == 0)
                    {
                        // If we are initilizing, then process InitClockCycles times.
                        if (Initializing)
                        {
                            if (ClockChangeCount >= InitClockCycles * 2)
                                break;
                        }
                        else
                        {
                            // Otherwise, process as many clock cycles as EmulatorPeriod
                            //     will allow.
                            if (timeGetTime() > ExecuteStartTime + EmulatorPeriod)
                                break;
                        }
                    }

                    // If we are stopping, then count the clock cycles (so message #4046 will
                    //     fire) but don't actually toggle the clock. 
                    if (!IsStopping)
                    {
	                    // Toggle all of the X86 system clocks.
                        for (int j = 0; j < X86SystemClocks->Count; j++)
                        {
                            Node    *X86SystemClockNode = (*X86SystemClocks)[j]->Outputs->
                                Items[0];

                            *X86SystemClockNode->DataPtr ^= 1;
                            X86UIThread_->AddEvent(X86SystemClockNode);
                        }
                    }
                }
                // If we make 100 passes without toggling the clock, then the events
                //		being passed between the systems are unstable.
                else if (TimesClockSkipped++ > 100)
                {
					BaseSystem->ReportPendingEvents();

                    ErrorTrap(197);

					TryHaltExecution();

                	break;
                }

                if (BaseSystem != NULL)
                    if (BaseSystem->Execute(ProcessingComEvent))
                    	return true;

                ClockDisable = FPGActive | X86Active;
                X86Active = 0;
            }

            if (!IsRunning)
                break;

            if (Initializing)
            {
                Initializing = false;
                TimedEventType = etExecute;

                WidgetForm->ShowIfNotEmpty();

                for (int i = 0; i < RuntimeComForms->Count; i++)
                {
                    TComForm	*RuntimeComForm = (TComForm *) RuntimeComForms->Items[i];

                    RuntimeComForm->Show();
                }

                if (CPUSystem != NULL && CPUSystem->SysThread != NULL)
                    CPUSystem->SysThread->SendGoSignals();

                if (CancelCompile)
                    return false;
            }

            if (ThisProcessEvent != NULL)
            {
                DWORD    WaitStatus = WaitForSingleObject(ThisProcessEvent, 0);

                if (WaitStatus == WAIT_OBJECT_0)
                {
                    CloseHandle(ThisProcessEvent); // decrement refrence value
                    IsStopping = true;
                }
            }

            if (IsRunning)
            {
                TimedEventType = etExecute;

                // Having "Sleep(0)" fixed the 8th copy of VivaRun.exe to load 5 times faster,
                //		without slowing down the emulator.
                //		If EmulatorSpeed > 50, then the user is trying to use less CPU time.
                if (EmulatorSpeed < 51)
                	Sleep(0);
                else
                	Sleep(EmulatorSpeed - 50);
            }

            break;
        }

        case etCompile:
        	break;

        default:
        {
			TimedEventType = etNoEvent;
            break;
        }
    }

    return false;
}


int RecurseCodeToInt(AnsiString RecurseCode)
{
	// this only converts correctly if we are doing a simple binary recursion
	// which will not be the exclusive case so Fix it (Later)
	// this will also only work on 31 bit db number of resource elements
	int length = RecurseCode.Length();
	int Result = 0;

	for (int i = 1; i < length + 1; i++)
	{
		// get next char and shift it in
		AnsiString    SBit = RecurseCode.SubString(i,1);
        int           Bit;

        if (SBit == "1")
            Bit = 1;
        else
            Bit = 0;

        Result = Result << 1;
        Result = Result | Bit;
    }

    return Result;
}


// Returns true when the source of the caption data changes.
//  Call this routine before updating Caption to increase performance and
//   reduce flutter.
bool NewCaptionSource(void *Object)
{
    static void *CaptionSource = NULL;
    // Caption setting time is noticeable..do only once.
    if (CaptionSource == Object)
        return false;

    CaptionSource = Object;
    return true;
}


void VivaGlobals::BringUIFormsToFront()
{
    if (COMDesigner != NULL &&
    	COMDesigner->Visible)
    {
    	COMDesigner->BringToFront();

        if (COMDesigner->FormInEdit != NULL)
        	COMDesigner->FormInEdit->BringToFront();
    }

    if (IsRunning)
    {
        for (int i = 0; i < RuntimeComForms->Count; i++)
        	((TComForm *) RuntimeComForms->Items[i])->BringToFront();

        if (WidgetForm != NULL)
	        WidgetForm->BringToFront();
    }
    else if (TimedEventType == etCompile && SynthGraphicDisplay != NULL
    		&& SynthGraphicDisplay->Visible)
    {
    	SynthGraphicDisplay->BringToFront();
    }
}


// Routine to convert fixed-point data into text.
// Used by both fixed-point and floating point formatting functions.
AnsiString VivaGlobals::FixData(double ff, int lowbits, int totbits, char fchar)
{
    int     bdigits = totbits + 8;
    char    fmt[12] = "%6.2e";
    int     ix = 1;

    while (true)
    {
        bdigits = (bdigits * 19728 + 0x7fff) >> 16;

        int    residu;

        do
        {
            if (bdigits >= 10)
            {
                residu = bdigits / 10;
                bdigits = bdigits % 10;
            }
            else
            {
                residu = 0;
            }

            fmt[ix++] = (char) (bdigits + '0');
        }
        while (residu > 0);

        if (lowbits < 0)
            break;

        fmt[ix++] = '.';
        bdigits = lowbits;
        lowbits = -1; // mark done
    }

    fmt[ix++] = fchar;
    fmt[ix] = 0;

    char OutString[256];
    sprintf(OutString, fmt, ff);
    return AnsiString(OutString);
}


// Checks to see if a file can be written to.
//		If the file is "read only", then the user is asked if it is ok to overwrite the file.
//		If the file exists and AutoOverwrite is false, then the user is asked if it is ok to
//		overwrite the file.
//		Returns false if the existing file should not be overwritten.
bool VivaGlobals::CheckForWrite(const AnsiString &FileName, bool AutoOverwrite)
{
	// The following prevents CodeGuard from reporting a failed _rtl_chmod() call.
    if (!FileExists(FileName))
    	return true;

    // Get the attribute flags for the file.
    int xflags = _rtl_chmod(FileName.c_str(), 0);

    // If the file does not exist, then a new file may be created.
    if (xflags == -1)
        return true;

    // If the file is "read only", then ask the user what to do.
    if (xflags & FA_RDONLY)
    {
        if (ErrorTrap(4002, FileName) == mrYes)
        {
            // Remove the "read only" flag (file may be overwritten).
            _rtl_chmod(FileName.c_str(), 1, xflags & (~FA_RDONLY));

            return true;
        }

        // Don't overwrite the "read only" file.
        return false;
    }

    // The file already exists.  Can we overwrite it?
    if (AutoOverwrite || ErrorTrap(3, FileName) == mrYes)
	    return true;
    else
        return false;
}


// Display the external help file.  If none, then inform the user.
//		HelpFileName is the "short" file name (no path or extention).
//		ObjectDocumentation distinguishes between Object and message help files.
void VivaGlobals::ShowHelpFile(AnsiString HelpFileName, bool ObjectDocumentation)
{
	// The following converts object names into valid Windows file names.
    AnsiString	ValidFileName = HelpFileName;

    IsValidFileName(ValidFileName);

    MainForm->FileListBox->Mask = ValidFileName + ".*";

    // Look inside all of the subdirectories in the object/message documentation directory.
    bool			FoundHelpFile = false;
    TStringList		*Subdirectories = new TStringList;
    AnsiString		DocDirectory = (ObjectDocumentation) ? strObjectDocDir
    													 : strMessageDocDir;

    if (DirectoryExists(DocDirectory))
	    Subdirectories->Add(DocDirectory);

    // Note:  Subdirectories->Count will grow mid loop.
	for (int i = 0; i < Subdirectories->Count; i++)
    {
        MainForm->FileListBox->Directory = Subdirectories->Strings[i];
		MainForm->DirectoryListBox->Directory = Subdirectories->Strings[i];

        // Does some type of file exist for this object name in this subdirectory?
        if (MainForm->FileListBox->Items->Count > 0)
        {
        	FoundHelpFile = true;

            ShellExecute(MainForm->Handle, NULL, MainForm->FileListBox->Items->
            	Strings[0].c_str(), "", "", SW_SHOWNORMAL);

            break;
        }

		// Add the new subdirectories onto the end of the list.
        for (int j = 0; j < MainForm->DirectoryListBox->Items->Count; j++)
        {
        	AnsiString	Subdirectory = MainForm->DirectoryListBox->Items->Strings[j];

            if (Subdirectory != "[.]" &&
            	Subdirectory != "[..]")
			        Subdirectories->Add(Subdirectories->Strings[i] + "\\" +
                    	Subdirectory.SubString(2, Subdirectory.Length() - 2));
        }
    }

    delete Subdirectories;

    // If we didn't find an external help file, then inform the user.
    if (!FoundHelpFile)
	    ErrorTrap(190, VStringList(HelpFileName, MainForm->FileListBox->Mask, DocDirectory));
}


// Parse the String into separate, trimed pieces as determined by the Separator.
//		Return the pieces in a new TStringList.
//		If SkipNullStrings is true, then null pieces will not be placed in the TStringList.
//
// Warning!  The calling routine must delete the TStringList that is returned.
//
TStringList *VivaGlobals::ParseString(AnsiString String, AnsiString Separator, bool TrimPieces,
	bool SkipNullStrings)
{
	TStringList		*StringPieces = new TStringList;
    int				SeparatorLen = Separator.Length();

    while (String != strNull)
    {
        // Extract the NextPiece by looking for the next Separator.
        AnsiString	NextPiece;
        int			SeparatorPos = String.Pos(Separator);

        if (SeparatorPos <= 0)
        {
            NextPiece = String;
            String = strNull;
        }
        else
        {
            NextPiece = String.SubString(1, SeparatorPos - 1);

            if (SeparatorPos + SeparatorLen > String.Length())
                String = strNull;
            else
                String = String.SubString(SeparatorPos + SeparatorLen,
                    String.Length() + 1 - SeparatorPos - SeparatorLen);
        }

        if (TrimPieces)
        	NextPiece = NextPiece.Trim();

        if (!SkipNullStrings ||
        	NextPiece != strNull)
            	StringPieces->Add(NextPiece);
    }

    return StringPieces;
}
