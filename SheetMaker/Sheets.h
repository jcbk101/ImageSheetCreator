//---------------------------------------------------------------------------

#ifndef SheetsH
#define SheetsH
//---------------------------------------------------------------------------
#include <System.Classes.hpp>
#include <FMX.Controls.hpp>
#include <FMX.Forms.hpp>
#include <FMX.Menus.hpp>
#include <FMX.Types.hpp>
#include <FMX.Dialogs.hpp>
#include <System.SysUtils.hpp>
#include <FMX.Controls.Presentation.hpp>
#include <FMX.StdCtrls.hpp>
#include <FMX.Layouts.hpp>
#include <FMX.ListBox.hpp>
#include <FMX.Objects.hpp>
#include <Data.Bind.Components.hpp>
#include <Data.Bind.EngExt.hpp>
#include <Fmx.Bind.DBEngExt.hpp>
#include <Fmx.Bind.Editors.hpp>
#include <System.Bindings.Outputs.hpp>
#include <System.Rtti.hpp>

#include <FMX.ImgList.hpp>
#include <System.ImageList.hpp>
#include <FMX.Edit.hpp>
#include <FMX.EditBox.hpp>
#include <FMX.NumberBox.hpp>
#include <FMX.SpinBox.hpp>
#include <FMX.ExtCtrls.hpp>
#include <FMX.Grid.hpp>
#include <FMX.Grid.Style.hpp>
#include <FMX.ScrollBox.hpp>
#include <FMX.Effects.hpp>
#include <FMX.Filter.Effects.hpp>
#include <FMX.Colors.hpp>

#include <System.IniFiles.hpp>
#include <System.UITypes.hpp>

#include "MessageBox.h"


//---------------------------------------------------------------------------
class TSheetsForm : public TForm
{
__published: //IDE-managed Components
	TMainMenu *        MainMenu1;
	TMenuItem *        loadImg;
	TMenuItem *        MenuItem2;
	TMenuItem *        exitApp;
	TMenuItem *        MenuItem4;
	TMenuItem *        exportImage;
	TOpenDialog *      FOpen;
	TStatusBar *       StatusBar1;
	TTrackBar *        scaleTrack;
	TPanel *           Panel1;
	TStyleBook *       StyleBook1;
	TText *            Text1;
	TText *            scaleText;
	TMenuItem *        viewMenu;
	TMenuItem *        alignToWindow;
	TImageList *       ImageList1;
	TMenuItem *        MenuItem8;
	TMenuItem *        runAnimation;
	TTimer *           Timer1;
	TSaveDialog *      FSave;
	TGridPanelLayout * GridPanelLayout1;
	TEdit *            Edit1;
	TLabel *           Label1;
	TSpinBox *         colNum;
	TLabel *           Label2;
	TSpinBox *         rowNum;
	TPanel *           Panel2;
	TGridPanelLayout * GridPanelLayout2;
	TSpeedButton *     alignTL;
	TSpeedButton *     alignTC;
	TSpeedButton *     alignTR;
	TSpeedButton *     alignLC;
	TSpeedButton *     alignC;
	TSpeedButton *     alignRC;
	TSpeedButton *     alignBL;
	TSpeedButton *     alignBC;
	TSpeedButton *     alignBR;
	TEdit *            Edit3;
	TRectangle *       gridParent;
	TGridPanelLayout * GridPanelLayout3;
	TEdit *            Edit2;
	TLabel *           Label3;
	TEdit *            sheetWidth;
	TLabel *           Label4;
	TEdit *            sheetHeight;
	TEdit *            Edit6;
	TLabel *           Label5;
	TEdit *            cellWidth;
	TLabel *           Label6;
	TEdit *            cellHeight;
	TGridLayout *      imgGrid;
	TGridLayout *centerGrid;
	TGridPanelLayout * GridPanelLayout4;
	TEdit *            Edit4;
	TLabel *           Label7;
	TSpinBox *         padTop;
	TLabel *           Label8;
	TSpinBox *         padLeft;
	TLabel *           Label9;
	TSpinBox *         padRight;
	TLabel *           Label10;
	TSpinBox *         padBottom;
	TMonochromeEffect *bcEfx;
	TMonochromeEffect *blEfx;
	TMonochromeEffect *brEfx;
	TMonochromeEffect *trEfx;
	TMonochromeEffect *tlEfx;
	TMonochromeEffect *tcEfx;
	TMonochromeEffect *rcEfx;
	TMonochromeEffect *lcEfx;
	TMonochromeEffect *cEfx;
	TMenuItem *        runMenu;
	TMenuItem *        MenuItem3;
	TMenuItem *        white;
	TMenuItem *        gray;
	TMenuItem *        black;
	TMenuItem *        chkboard;
	TPanel *           animFrame;
	TRectangle *       animRect;
	TGridPanelLayout * GridPanelLayout5;
	TEdit *            testAnimEdit;
	TLabel *           Label11;
	TSpinBox *         animTime;
	TRectangle *       animator;
	TPopupMenu *       PopupMenu1;
	TMenuItem *        selectAll;
	TMenuItem *        MenuItem6;
	TMenuItem *        delItems;
	TButton *          hintLabel;
	TMenuItem *        MenuItem5;
	TMenuItem *        imageCrop;
	TMenuItem *        borderColorMenu;
	TMenuItem *clearSelection;

	void __fastcall exitAppClick(TObject *Sender);
	void __fastcall loadImgClick(TObject *Sender);
	void __fastcall scaleTrackChange(TObject *Sender);
	void __fastcall imgListMouseMove(TObject *Sender, TShiftState Shift, float X, float Y);
	void __fastcall imgListMouseWheel(TObject *Sender, TShiftState Shift, int WheelDelta, bool &Handled);
	void __fastcall alignToWindowClick(TObject *Sender);
	void __fastcall FormResize(TObject *Sender);
	void __fastcall runAnimationClick(TObject *Sender);
	void __fastcall Timer1Timer(TObject *Sender);
	void __fastcall exportImageClick(TObject *Sender);
	void __fastcall alignTRClick(TObject *Sender);
	void __fastcall rowNumChange(TObject *Sender);
	void __fastcall padBottomChange(TObject *Sender);
	void __fastcall padTopClick(TObject *Sender);
	void __fastcall blackClick(TObject *Sender);

	void __fastcall saveSettingsClick(TObject *Sender);
	void __fastcall reloadSettingsClick(TObject *Sender);
	void __fastcall FormCreate(TObject *Sender);
	void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
	void __fastcall selectAllClick(TObject *Sender);
	void __fastcall delItemsClick(TObject *Sender);
	void __fastcall gridParentMouseDown(TObject *Sender, TMouseButton Button, TShiftState Shift, float X, float Y);
	void __fastcall imageCropClick(TObject *Sender);
	void __fastcall borderClick(TObject *Sender);
	void __fastcall clearSelectionClick(TObject *Sender);
	void __fastcall gridParentDblClick(TObject *Sender);

private: //User declarations
public:  //User declarations
	__fastcall TSheetsForm(TComponent* Owner);

	int        gridHeight;
	int        gridWidth;
	int        savedGridHeight;
	int        savedGridWidth;
	AnsiString exePath;


	void __fastcall alignImage(TRectangle *img, int alignment);
	void __fastcall equalizeGrid();
	void __fastcall showInformation();
};


#define TL 0
#define TC 1
#define TR 2
#define LC 3
#define CENTER 4
#define RC 5
#define BL 6
#define BC 7
#define BR 8

//---------------------------------------------------------------------------
extern PACKAGE TSheetsForm *SheetsForm;
//---------------------------------------------------------------------------
#endif
