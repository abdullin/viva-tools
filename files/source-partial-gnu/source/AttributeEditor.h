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

#ifndef AttributeEditorH
#define AttributeEditorH

#include <Buttons.hpp>
#include <Classes.hpp>
#include <Controls.hpp>
#include <Graphics.hpp>
#include <Grids.hpp>
#include <StdCtrls.hpp>
#include <ExtCtrls.hpp>
#include <OleCtrls.hpp>
#include <AxCtrls.hpp>

#define READ_ONLY_CELL (TObject *) -1


class TAttributeDialog : public TForm
{
__published:
    TLabel          *ObjectLabel;
	TRadioButton 	*optCreateObject;
	TRadioButton 	*optUpdateObject;
	TGroupBox       *AttribBox;
	TStringGrid     *AttributeGrid;
	TEdit           *ObjectName;
	TGroupBox       *DocBox;
	TGroupBox       *IconBox;
	TMemo *DocumentationMemo;
	TLabel *lblTreeGroup;
	TComboBox *cmbTreeGroup;
	TPanel          *PanelGraphic;
	TImage          *DisplayImage;
	TCheckBox       *CheckLeaveOpen;
	TBitBtn         *OKButton;
	TBitBtn         *CancelButton;
	TComboBox       *AttributeComboBox;
	TButton         *btnRevert;
	TButton         *btnDiscard;
	TRadioGroup     *rgKeepObject;
	TButton 		*IconRefresh;
	TButton 		*EllipsisButton;
	TEdit *txtLibSysName;
	TLabel *lblLibSys;
	TBitBtn *EditTruthFileButton;
	void __fastcall IconRefreshClick(TObject *Sender);
	void __fastcall ObjectNameChange(TObject *Sender);
	void __fastcall optUpdateObjectClick(TObject *Sender);
	void __fastcall EditGlyphClick(TObject *Sender);
	void __fastcall AttributeGridDrawCell(TObject *Sender, int ACol, int ARow, TRect &Rect,
		TGridDrawState State);
	void __fastcall AttributeGridKeyDown(TObject *Sender, WORD &Key, TShiftState Shift);
	void __fastcall AttributeGridKeyUp(TObject *Sender, WORD &Key, TShiftState Shift);
	void __fastcall AttributeGridMouseDown(TObject *Sender, TMouseButton Button, TShiftState
		Shift, int X, int Y);
	void __fastcall AttributeComboBoxKeyUp(TObject *Sender, WORD &Key, TShiftState Shift);
	void __fastcall AttributeComboBoxClick(TObject *Sender);
	void __fastcall AttributeComboBoxExit(TObject *Sender);
	void __fastcall FormActivate(TObject *Sender);
	void __fastcall FormDeactivate(TObject *Sender);
	void __fastcall AttributeComboBoxKeyDown(TObject *Sender, WORD &Key, TShiftState Shift);
	void __fastcall OKButtonClick(TObject *Sender);
	void __fastcall btnRevertClick(TObject *Sender);
	void __fastcall btnDiscardClick(TObject *Sender);
	void __fastcall AttributeGridSelectCell(TObject *Sender, int ACol, int ARow,
    	bool &CanSelect);
	void __fastcall EllipsisButtonClick(TObject *Sender);
	void __fastcall AttributeGridKeyPress(TObject *Sender, char &Key);
	void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
	void __fastcall EditTruthFileButtonClick(TObject *Sender);

public:
	int				NextTabRow;     // The row of the next cell that the tab key will go to.
	IntegerList		*DropdownCodes; // List parallel to the rows of the Attribute Grid; says
    								// 		whether rows are for inputs, outputs, attributes,
    								//		or just caption.

	__fastcall TAttributeDialog(TComponent *Owner);
	__fastcall~TAttributeDialog();

	void __fastcall EditObjectAttributes(I2adl *OrigObject, I2adl *NewObject);
	void __fastcall RowLoad(AnsiString String0 = "", AnsiString String1 = ""
    	, int ChangeSet = 3);
	void SetAttribute(AnsiString Name, AnsiString Value);
	AnsiString GetAttributeValue(AnsiString Name);

protected:
    enum EditCaseEnum
    {
        CreateNew,          // Creating an object from a new behavior
        					//		(OrigObject == NULL).
        CreateFromExisting, // Creating/updating an object from an existing behavior
        					//		(OrigObject != NewObject).
        EditDef,            // Editing an I2adl "def"
        					//		(OrigObject == NewObject && I2adlShape == NULL).
        EditRef             // Editing an I2adl "ref"
        					//		(OrigObject == NewObject && I2adlShape != NULL).
    };

	int         	ARow;			// Last row selected
	int         	ACol;			// Last column selected
	int         	RowNumber;		// The number of the row being edited
	int         	FirstAttribute;	// Row number of first attribute (past inputs/outputs)
	I2adl       	*I2adlObject;   // I2adl Object currently in edit
	VivaIcon    	*EditIcon;		// The icon that can be edited
    EditCaseEnum 	EditCase;		// Which creation or edit operation is being performed
	TStringList 	*AttributeList; // List of attributes that are valid for the selected cell
	bool    		IsEmpty;		// Need to know immediately if the Combo Box text is empty,
    								//		because when it loses focus, if the text is nothing
		                            //		more than whitespace, the TComboBox component will
                            		//		fill itself in with the last item selectd.
	bool    		UsingMouse;		// Need to know when a key was pressed on the Combo Box,
		    						// 		since its "OnClick" event Makes no distinction
		                            //		between whether it was generated by the mouse or
                                    //		keyboard.
	bool    		bSettingCell;   // In process of setting selection to a writeable cell.

	void __fastcall ShowAttributeComboBox(int Column = 1, TComboBoxStyle Style =
		Stdctrls::csDropDown, bool DroppedDown = true);
	void __fastcall ShowOrHideAttributeComboBox();
	int __fastcall FindAttributeComboBoxItemPos(const AnsiString &Item);
	void __fastcall NextWriteableCell(bool Forward = true, int Col = -1, int Row = -1);
	void __fastcall HandleTab(bool Forward = true);
	bool __fastcall InACBBounds();
	void __fastcall EventHandlerForAttributeDialog(tagMSG &Msg, bool &Handled);
	void __fastcall LoadEditIcon();
	void GetAttributeList();
	bool IsCandidateForPrimitive(I2adl *Object);
	Node *GetCorrespondingNode(int Row, I2adl *I2adlObject);
    bool IsChangingNodesOK(int &NodesChanged, bool &CancelNodeChanges);

public:
	int         FirstInput;
	int         FirstOutput;
};

extern PACKAGE TAttributeDialog    *AttributeDialog;

#endif
