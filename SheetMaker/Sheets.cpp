//---------------------------------------------------------------------------

#include <fmx.h>
#pragma hdrstop

#include "Sheets.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.fmx"

TSheetsForm *SheetsForm;
bool         canModify      = true;
TStringList *selectionsList = new TStringList();
TPointF oldSize, newSize, pos;
TPointF oldMouse, newMouse, mpos;
TPointF mouseCenter;
TAlphaColor BorderColors[] =
{
	claRed,
	claDarkred,
	claBlack,
	claBlue,
	claDarkblue,
	claGreen,
	claDarkgreen,
	claOrange,
	claDarkorange,
	claPurple,
	claWhite,
	claTeal
};

TAlphaColor TextColors[] =
{
	claWhite,
	claWhite,
	claWhite,
	claWhite,
	claWhite,
	claWhite,
	claWhite,
	claWhite,
	claWhite,
	claWhite,
	claBlack,
	claWhite,
};

String colorsName[] =
{
	"Red",
	"Dark Red",
	"Black",
	"Blue",
	"Dark Blue",
	"Green",
	"Dark Green",
	"Orange",
	"Dark Orange",
	"Purple",
	"White",
	"Teal",
	""
};


//---------------------------------------------------------------------------
__fastcall TSheetsForm::TSheetsForm(TComponent* Owner)
	: TForm(Owner)
{
	gridHeight = imgGrid->ItemHeight;
	gridWidth = imgGrid->ItemWidth;
	savedGridHeight = imgGrid->ItemHeight;
	savedGridWidth = imgGrid->ItemWidth;

	//Timer settings for animated flip book
	animTime->Tag = 0;
	animTime->Value = 100;

	//Show updated data
	showInformation();

	selectionsList->Duplicates = System::Types::TDuplicates::dupIgnore;
	selectionsList->Sorted = true;

	//
	animRect->Position->Y = (SheetsForm->ClientHeight - animRect->Height) / 2;
	animRect->Position->X = (Panel1->Width / 2) + ((SheetsForm->ClientWidth - animRect->Width) / 2);
	animRect->Visible = false;
	animator->Fill->Kind = TBrushKind::Bitmap;
	animator->Fill->Bitmap->WrapMode = TWrapMode::TileOriginal;

	//
	exePath = GetCurrentDir();
}


//---------------------------------------------------------------------------
//
//Exit application
//
//---------------------------------------------------------------------------
void __fastcall TSheetsForm::exitAppClick(TObject *Sender)
{
	//Free memory
	delete selectionsList;
	SheetsForm->Close();
}


void __fastcall TSheetsForm::FormClose(TObject *Sender, TCloseAction &Action)
{
	saveSettingsClick(NULL);
}


//---------------------------------------------------------------------------
//
//Create the main form and initialize menu objects, etc.
//
//---------------------------------------------------------------------------
void __fastcall TSheetsForm::FormCreate(TObject *Sender)
{
	//Add the border colors to the menu list
	int i = -1;
	while (colorsName[++i] != "")
	{
		TMenuItem *m  = new TMenuItem(borderColorMenu);
		TBitmap *  bm = new TBitmap();

		borderColorMenu->AddObject(m);

		m->Parent = borderColorMenu;
		m->Tag = i;
		m->Text = colorsName[i];

		bm->Width = 24;
		bm->Height = 24;
		bm->Clear(BorderColors[i]);
		m->Bitmap->Assign(bm);

		m->OnClick = borderClick;
		m->GroupIndex = 5;
		m->RadioItem = true;
		//
		if (!i)
			m->IsChecked = true;
	}

	//
	TagFloat = i;
	gridParent->Stroke->Color = BorderColors[i];

	//
	reloadSettingsClick(NULL);
}


//---------------------------------------------------------------------------
//
//Change the color of the actively viewed borderfor each frame
//
//---------------------------------------------------------------------------
void __fastcall TSheetsForm::borderClick(TObject *Sender)
{
	TMenuItem *m = dynamic_cast<TMenuItem *>(Sender);

	if (m)
	{
		m->IsChecked = true;
		gridParent->Stroke->Color = BorderColors[m->Tag];
		SheetsForm->TagFloat = m->Tag;

		//Adjust all
		for (int i = 0; i < centerGrid->ChildrenCount; i++)
		{
			TPanel *obj = dynamic_cast<TPanel *>(centerGrid->Children->Items[i]);
			int color;

			if (obj)
			{
				color = 0;
				if (obj->TagFloat == 0.5)
				{
					color = ((int)gridParent->Stroke->Color & 0x00FFFFFF);
					color |= (int)(0xFF000000 * obj->TagFloat);
				}

				//
				obj->StylesData["myTintRect.Fill.Color"] = TValue::From<TAlphaColor>(color);
				obj->StylesData["myTintRect.Stroke.Color"] = TValue::From<TAlphaColor>(gridParent->Stroke->Color);
				obj->StylesData["myNumRect.Fill.Color"] = TValue::From<TAlphaColor>(gridParent->Stroke->Color);
				obj->StylesData["myText.Color"] = TValue::From<TAlphaColor>(TextColors[(int)TagFloat]);
			}
		}
	}
}


//---------------------------------------------------------------------------
//
//Automatically save application settings, IE: colors, Position
//
//---------------------------------------------------------------------------
void __fastcall TSheetsForm::saveSettingsClick(TObject *Sender)
{
	if (runAnimation->IsChecked)
		return;

	TIniFile *ini = new TIniFile(exePath + "\\settings.ini");

	if (ini)
	{
		ini->WriteInteger("Main Window", "X", Left);
		ini->WriteInteger("Main Window", "Y", Top);
		ini->WriteInteger("Main Window", "Width", Width);
		ini->WriteInteger("Main Window", "Height", Height);
		ini->WriteInteger("Main Window", "WindowState", (int)WindowState);
		ini->WriteInteger("Main Window", "BG", Tag);
		ini->WriteInteger("Main Window", "BorderColor", TagFloat);

		//Always clean up after yourself
		delete ini;
	}
}


//---------------------------------------------------------------------------
//
//Automatically load application settings, IE: colors, Position
//
//---------------------------------------------------------------------------
void __fastcall TSheetsForm::reloadSettingsClick(TObject *Sender)
{
	if (runAnimation->IsChecked)
		return;

	TIniFile *ini = new TIniFile(exePath + "\\settings.ini");

	if (ini)
	{
		//Main window
		Left = ini->ReadInteger("Main Window", "X", 0);
		Top = ini->ReadInteger("Main Window", "Y", 0);
		Width = ini->ReadInteger("Main Window", "Width", 800);
		Height = ini->ReadInteger("Main Window", "Height",600);
		int i = ini->ReadInteger("Main Window", "WindowState", 0);

		TWindowState ws[3] =
		{
			TWindowState::wsNormal,
			TWindowState::wsMinimized,
			TWindowState::wsMaximized
		};
		WindowState = ws[i];

		//
		if (WindowState == TWindowState::wsMaximized)
		{
			Height = 740;
			Width = 1080;
			Position = TFormPosition::ScreenCenter;
		}

		//Get the BG to use
		Tag = ini->ReadInteger("Main Window", "BG", 11);

		if (Tag > 0)
		{
			SheetsForm->Fill->Bitmap->Bitmap->Assign
				(
				ImageList1->Bitmap(TSize(16, 16), Tag)
				);

			if (Tag == white->ImageIndex)
				white->IsChecked = true;
			if (Tag == gray->ImageIndex)
				gray->IsChecked = true;
			if (Tag == black->ImageIndex)
				black->IsChecked = true;
			if (Tag == chkboard->ImageIndex)
				chkboard->IsChecked = true;
		}

		//Get the border color to use
		TagFloat = ini->ReadInteger("Main Window", "BorderColor", 0);

		if (TagFloat < 0)
			TagFloat = 0;
		//
		gridParent->Stroke->Color = BorderColors[(int)TagFloat];
		((TMenuItem *)(borderColorMenu->Items[TagFloat]))->IsChecked = true;

		//Always clean up after yourself
		delete ini;
	}
}


//---------------------------------------------------------------------------
//
//Load the images to be processed
//
//---------------------------------------------------------------------------
void __fastcall TSheetsForm::loadImgClick(TObject *Sender)
{
	FOpen->InitialDir = GetCurrentDir();

	if (FOpen->Execute())
	{
		//Clear old data
		imgGrid->DeleteChildren();
		centerGrid->DeleteChildren();
		selectionsList->Clear();

		//REset sizes
		gridWidth = gridHeight = 0;

		//Build the grid size defaults
		for (int i = 0; i < FOpen->Files->Count; i++)
		{
			TBitmap *bm = new TBitmap(FOpen->Files->Strings[i]);

			if (bm != NULL)
			{
				if (bm->Width > gridWidth)
					gridWidth = bm->Width;
				if (bm->Height > gridHeight)
					gridHeight = bm->Height;
			}

			//
			delete bm;
		}


		//
		if (gridHeight > -1 && gridWidth > -1)
		{
			//
			imgGrid->ItemWidth = gridWidth;
			imgGrid->ItemHeight = gridHeight;
			centerGrid->ItemWidth = gridWidth;
			centerGrid->ItemHeight = gridHeight;

			//
			gridParent->Width = (gridWidth * colNum->Value);
			gridParent->Height = (gridHeight * rowNum->Value);
			centerGrid->Width = gridParent->Width;
			centerGrid->Height = gridParent->Height;

			//Size data collecetd
			for (int i = 0; i < FOpen->Files->Count; i++)
			{
				//-------------------------------------
				//
				//Convert the pointers to tiles
				//
				//-------------------------------------
				//Create a temp Bitmap to hold image that will be processed
				TBitmap *    bm    = new TBitmap(FOpen->Files->Strings[i]);
				TRectangle * rect  = new TRectangle(SheetsForm); //getRectangle(parent);
				TPanel *     cross = new TPanel(SheetsForm);

				//
				rect->Fill->Kind = TBrushKind::Bitmap;
				rect->Stroke->Kind = TBrushKind::Solid;
				rect->Stroke->Color = claBlue;
				rect->Stroke->Thickness = 1;
				rect->HitTest = false;
				rect->Tag = i;
				rect->Fill->Bitmap->Bitmap->Assign(bm);
				rect->Fill->Bitmap->WrapMode = TWrapMode::TileOriginal;
				rect->Align = TAlignLayout::None;

				//Add the BMP to the list
				imgGrid->AddObject(rect);

				//Align to the CENTER ( Adding a margin )
				rect->TagFloat = CENTER;
				alignImage(rect, CENTER);

				//
				//Show the center point of the image using a crosshair
				//
				cross->StyleLookup = "myLayoutStyle";
				centerGrid->AddObject(cross);
				cross->Parent = centerGrid;
				cross->HitTest = false;
				cross->Tag = i;

				//Tinting
				cross->StylesData["myTintRect.Fill.Color"] = TValue::From<TAlphaColor>(0);
				cross->StylesData["myTintRect.Stroke.Color"] = TValue::From<TAlphaColor>(gridParent->Stroke->Color);

				//
				char buf[32];
				sprintf(buf, "%003d", i + 1);

				cross->StylesData["myText.Text"] = TValue::From<AnsiString>(AnsiString(buf));
				cross->StylesData["myText.Color"] = TValue::From<TAlphaColor>(TextColors[(int)TagFloat]);
				cross->StylesData["myNumRect.Fill.Color"] = TValue::From<TAlphaColor>(gridParent->Stroke->Color);
			}
		}

		//Adjust the rows to match columns and image count
		equalizeGrid();

		gridParent->Position->X = Panel1->Width;
		gridParent->Position->Y = 0;

		//Show updated data
		showInformation();
	}
}


//---------------------------------------------------------------------------
//
//
//
//---------------------------------------------------------------------------
void __fastcall TSheetsForm::equalizeGrid()
{
	rowNum->Value = (imgGrid->ChildrenCount / 8);
	colNum->Value = (imgGrid->ChildrenCount / rowNum->Value);

	//
	if (imgGrid->ChildrenCount > 0)
	{
		gridParent->Width = (gridWidth * colNum->Value);
		gridParent->Height = (gridHeight * rowNum->Value);
		centerGrid->Width = gridParent->Width;
		centerGrid->Height = gridParent->Height;
	}
	else
	{
		colNum->Value = 1;
		rowNum->Value = 1;
		//fix these numbers
		gridParent->Width = (gridWidth * colNum->Value);
		gridParent->Height = (gridHeight * rowNum->Value);
		centerGrid->Width = gridParent->Width;
		centerGrid->Height = gridParent->Height;
	}
}


//---------------------------------------------------------------------------
//
//Scale the main window for easier viewing
//
//---------------------------------------------------------------------------
void __fastcall TSheetsForm::scaleTrackChange(TObject *Sender)
{
	char buf[32];
	float oldScale = gridParent->Scale->X;

	//Zooms to center at any X, Y location
	//oldSize = TPointF(gridParent->Width * oldScale, gridParent->Height * oldScale);
	//newSize = TPointF(gridParent->Width * scaleTrack->Value, gridParent->Height * scaleTrack->Value);
	//
	//pos = (newSize - oldSize) / 2;
	//gridParent->Position->Point -= pos;


	//Need to figure out zoom to pixel
	gridParent->Scale->X = scaleTrack->Value;
	gridParent->Scale->Y = scaleTrack->Value;
	//
	oldMouse = mouseCenter * oldScale;
	newMouse = mouseCenter * scaleTrack->Value;
	//
	mpos = (newMouse - oldMouse);
	gridParent->Position->Point -= mpos;

	//
	animRect->Scale->X = scaleTrack->Value;
	animRect->Scale->Y = scaleTrack->Value;
	animRect->Position->Y = (SheetsForm->ClientHeight - (animRect->Height * scaleTrack->Value)) / 2;
	animRect->Position->X = (Panel1->Width / 2) + ((SheetsForm->ClientWidth - (animRect->Width * scaleTrack->Value)) / 2);

	//
	sprintf(buf, "%d%%", (int)(scaleTrack->Value * 100));
	scaleText->Text = AnsiString(buf);
}


//---------------------------------------------------------------------------
//
//Move the window around the form freely
//Also used to multi select frames
//
//---------------------------------------------------------------------------
void __fastcall TSheetsForm::imgListMouseMove(TObject *Sender, TShiftState Shift, float X, float Y)
{
	char buf[16];
	hintLabel->Text = "";

	//Center scaling
	mouseCenter = TPointF(X, Y);

	if (Shift.Contains(ssShift) && Shift.Contains(ssLeft))
	{
		//Multi-Select a cells
		int adjW = (gridWidth + padLeft->Value + padRight->Value);
		int adjH = (gridHeight + padTop->Value + padBottom->Value);
		int x    = X / adjW;
		int y    = Y / adjH;

		if ((y * colNum->Value + x) < centerGrid->ChildrenCount)
		{
			TPanel *obj = dynamic_cast<TPanel *>(centerGrid->Children->Items[y * colNum->Value + x]);

			if (obj)
			{

				obj->TagFloat= 0.5;
				int color = ((int)gridParent->Stroke->Color & 0x00FFFFFF);
				color |= (int)(0xFF000000 * obj->TagFloat);
				obj->StylesData["myTintRect.Fill.Color"] = TValue::From<TAlphaColor>(color);
				//
				sprintf(buf, "%005d", obj->Tag);
				selectionsList->Add(buf);
			}
		}
	}
	else if (Shift.Contains(ssLeft) && !Shift.Contains(ssCtrl) && !Shift.Contains(ssShift))
	{
		TControl *obj = dynamic_cast<TControl *>(Sender);

		if (obj)
		{
			obj->Position->X += (X - obj->PressedPosition.x) * scaleTrack->Value;
			obj->Position->Y += (Y - obj->PressedPosition.y) * scaleTrack->Value;
		}
	}
}


//---------------------------------------------------------------------------
//
//Use the mouse wheel to Zoom the main window in and out
//
//---------------------------------------------------------------------------
void __fastcall TSheetsForm::imgListMouseWheel(TObject *Sender, TShiftState Shift, int WheelDelta, bool &Handled)
{
	if (WheelDelta > 0)
		scaleTrack->Value += scaleTrack->Frequency;
	else if (WheelDelta < 0)
		scaleTrack->Value -= scaleTrack->Frequency;
}


//---------------------------------------------------------------------------
//
//Center the main form inside the viewable parent form
//
//---------------------------------------------------------------------------
void __fastcall TSheetsForm::alignToWindowClick(TObject *Sender)
{
	gridParent->Position->Y = (SheetsForm->ClientHeight - (gridParent->Height * scaleTrack->Value)) / 2;
	gridParent->Position->X = (Panel1->Width / 2) + ((SheetsForm->ClientWidth - (gridParent->Width * scaleTrack->Value)) / 2);
}


void __fastcall TSheetsForm::FormResize(TObject *Sender)
{
	gridParent->Position->Y = (SheetsForm->ClientHeight - (gridParent->Height * scaleTrack->Value)) / 2;
	gridParent->Position->X = (Panel1->Width / 2) + ((SheetsForm->ClientWidth - (gridParent->Width * scaleTrack->Value)) / 2);
}


//---------------------------------------------------------------------------
//
//Show the user the frames being animated
//
//---------------------------------------------------------------------------
void __fastcall TSheetsForm::runAnimationClick(TObject *Sender)
{
	TMenuItem *m    = dynamic_cast<TMenuItem *>(Sender);
	int        adjW = (gridWidth + padLeft->Value + padRight->Value);
	int        adjH = (gridHeight + padTop->Value + padBottom->Value);

	if (m && imgGrid->ChildrenCount)
	{
		m->IsChecked = !m->IsChecked;

		if (m->IsChecked)
		{
			m->Text = "Stop Animation";

			//Size the animation window
			animRect->Width = adjW;
			animRect->Height = adjH;
			//Center the animation window
			animRect->Position->Y = (SheetsForm->ClientHeight - (animRect->Height * scaleTrack->Value)) / 2;
			animRect->Position->X = (Panel1->Width / 2) + ((SheetsForm->ClientWidth - (animRect->Width * scaleTrack->Value)) / 2);

			//Quick start
			Timer1->Interval = 1;//animTime->Value;

			//Swap visibilities
			animRect->Visible = true;
			gridParent->Visible = false;

			animFrame->StylesData["myTintRect.Fill.Color"] = TValue::From<TAlphaColor>(0);
			animFrame->StylesData["myTintRect.Stroke.Color"] = TValue::From<TAlphaColor>(BorderColors[(int)TagFloat]);
			animFrame->StylesData["myNumRect.Fill.Color"] = TValue::From<TAlphaColor>(BorderColors[(int)TagFloat]);
			animFrame->StylesData["myText.Color"] = TValue::From<TAlphaColor>(TextColors[(int)TagFloat]);
			animFrame->StylesData["myGlyph.Opacity"] = TValue::From<float>(0);
		}
		else
		{
			m->Text = "Run Animation";

			Timer1->Interval = 0;
			//Swap visibilities
			animRect->Visible = false;
			gridParent->Visible = true;
		}

		//Update controls
		showInformation();
	}
}


//---------------------------------------------------------------------------
//
//Simeple animator controlled by a timer
//
//---------------------------------------------------------------------------
void __fastcall TSheetsForm::Timer1Timer(TObject *Sender)
{
	//When timer is doner, change animated image
	//and reset the timer
	animTime->Tag++;

	if (animTime->Tag >= imgGrid->ChildrenCount)
		animTime->Tag = 0;

	int index = animTime->Tag;

	//Swap the image
	AnsiString s = IntToStr(index);
	animFrame->StylesData["myText.Text"] = TValue::From<AnsiString>(s);

	TRectangle *rect= dynamic_cast<TRectangle *>(imgGrid->Children->Items[index]);

	if (rect)
	{
		animator->Fill->Bitmap->Bitmap->Assign(rect->Fill->Bitmap->Bitmap);
		animator->Margins->Rect = rect->Margins->Rect;
	}

	Timer1->Interval = animTime->Value;

}


//---------------------------------------------------------------------------
//
//Export the frames as a single file. Limits are 8192x8192
//
//---------------------------------------------------------------------------
void __fastcall TSheetsForm::exportImageClick(TObject *Sender)
{
	int adjW = (gridWidth + padLeft->Value + padRight->Value);
	int adjH = (gridHeight + padTop->Value + padBottom->Value);

	//TBitmap has size limitations
	//No larger that 8192x8192 pixels
	if (adjW > 8192 || adjH > 8192)
	{
		ShowMessage("Error: Export dimensions cannot exceed 8192 pixels for width or height.");
		return;
	}

	FSave->InitialDir = GetCurrentDir();

	if (FSave->Execute())
	{
		TBitmap *bmp = new TBitmap();
		int      x   = 0;
		int      y   = 0;
		bmp->Width = (adjW * colNum->Value);
		bmp->Height = (adjH * rowNum->Value);
		TRect src, dst;


		//bmp->Width = (gridWidth * colNum->Value);
		//bmp->Height = (gridHeight * rowNum->Value);
		bmp->Canvas->BeginScene();

		for (int i = 0; i < imgGrid->ChildrenCount; i++)
		{
			TRectangle * item = dynamic_cast<TRectangle *>(imgGrid->Children->Items[i]);

			if (item)
			{
				TBitmap *img = item->Fill->Bitmap->Bitmap;

				//Draw this on the canvas with the proper X,Y and full cell size
				src = Rect(0, 0, img->Width, img->Height);

				dst = Rect(
					item->Position->X,
					item->Position->Y,
					item->Position->X + item->Width,
					item->Position->Y + item->Height
					);

				bmp->Canvas->DrawBitmap(img, src, dst, 1, false);

				//Advance X,Y
				x += adjW;
				if (x >= bmp->Width)
				{
					x = 0;
					y += adjH;
				}
			}
		}

		bmp->Canvas->EndScene();
		//Save the Sprite sheet
		bmp->SaveToFile(FSave->FileName);

		delete bmp;

		ShowMessage("Image sheet was exported successfully");
	}
}


//---------------------------------------------------------------------------
//
//Align user selections using Margin values
//
//---------------------------------------------------------------------------
void __fastcall TSheetsForm::alignTRClick(TObject *Sender)
{
	TSpeedButton *btn = dynamic_cast<TSpeedButton *>(Sender);

	if (btn)
	{
		for (int i = 0; i < selectionsList->Count; i++)
		{
			int          index = StrToInt(selectionsList->Strings[i]);
			TRectangle * item  = dynamic_cast<TRectangle *>(imgGrid->Children->Items[index]);

			if (item)
			{
				item->TagFloat = btn->Tag;
				alignImage(item, btn->Tag);
			}
		}
	}
}


//---------------------------------------------------------------------------
//
//Alignment code used with the above method
//
//---------------------------------------------------------------------------
void __fastcall TSheetsForm::alignImage(TRectangle *img, int alignment)
{
	int index    = img->Tag;
	int rowIndex = (index / colNum->Value);
	int colIndex = (index - (rowIndex * colNum->Value));

	int adjW = (gridWidth + padLeft->Value + padRight->Value);
	int adjH = (gridHeight + padTop->Value + padBottom->Value);
	int x    = (colIndex * adjW);
	int y    = (rowIndex * adjH);

	//int width  = img->Fill->Bitmap->Bitmap->Width;
	//int height = img->Fill->Bitmap->Bitmap->Height;
	int width  = img->Width;
	int height = img->Height;


	switch(alignment)
	{
		//TL
	case 0:
		img->Margins->Rect = Rect(
			0, 0,
			(adjW - width), (adjH - height)
			);
		break;
		//TC
	case 1:
		img->Margins->Rect = Rect(
			(adjW - width) / 2, 0,
			(adjW - width) / 2, (adjH - height)
			);
		break;
		//TR
	case 2:
		img->Margins->Rect = Rect(
			(adjW - width), 0,
			0, (adjH - height)
			);
		break;
		//LC
	case 3:
		img->Margins->Rect = Rect(
			0, (adjH - height) / 2,
			(adjW - width), (adjH - height) / 2
			);
		break;
		//C
	case 4:
		img->Margins->Rect = Rect(
			(adjW - width) / 2, (adjH - height) / 2,
			(adjW - width) / 2, (adjH - height) / 2
			);
		break;
		//RC
	case 5:
		img->Margins->Rect = Rect(
			(adjW - width), (adjH - height) / 2,
			0, (adjH - height) / 2
			);
		break;
		//BL
	case 6:
		img->Margins->Rect = Rect(
			0, (adjH - height),
			(adjW - width), 0
			);
		break;
		//BC
	case 7:
		img->Margins->Rect = Rect(
			(adjW - width) / 2, (adjH - height),
			(adjW - width) / 2, 0
			);
		break;
		//BR
	case 8:
		img->Margins->Rect = Rect(
			(adjW - width), (adjH - height),
			0, 0
			);
		break;
	default:
		break;
	}
}


//---------------------------------------------------------------------------
//
//Used to modify the size of the grid being viewed
//
//---------------------------------------------------------------------------
void __fastcall TSheetsForm::rowNumChange(TObject *Sender)
{
	TSpinBox *sb =dynamic_cast<TSpinBox *>(Sender);

	if (sb && canModify)
	{
		int row = rowNum->Value;
		int col = colNum->Value;

		canModify = false;

		if (imgGrid->ChildrenCount)
		{
			if (sb->Tag == 1)
			{
				//Rows changed. Fix columns
				col = (imgGrid->ChildrenCount / rowNum->Value);
				row = (imgGrid->ChildrenCount / col);
			}
			else
			{
				//Columns changed. Fix rows
				row = (imgGrid->ChildrenCount / colNum->Value);
				col = (imgGrid->ChildrenCount / row);
			}
		}

		//Account for overflow
		if ((row * col) < imgGrid->ChildrenCount)
			row++;

		//Adjust the rows to match columns and image count
		rowNum->Value = row;
		colNum->Value = col;

		//Resize the grid
		gridParent->Width = (gridWidth * colNum->Value);
		gridParent->Height = (gridHeight * rowNum->Value);
		centerGrid->Width = gridParent->Height;
		centerGrid->Height = gridParent->Height;

		//Show updated data
		showInformation();

		canModify = true;
	}
}


void __fastcall TSheetsForm::padBottomChange(TObject *Sender)
{
	TSpinBox *sb =dynamic_cast<TSpinBox *>(Sender);

	if (sb)
	{
		gridParent->Width = (gridWidth + padLeft->Value + padRight->Value) * colNum->Value;
		gridParent->Height = (gridHeight + padTop->Value + padBottom->Value) * rowNum->Value;
		centerGrid->Width = gridParent->Height;
		centerGrid->Height = gridParent->Height;

		imgGrid->ItemWidth = (gridWidth);
		centerGrid->ItemWidth = imgGrid->ItemWidth;
		imgGrid->ItemHeight = (gridHeight);
		centerGrid->ItemHeight = imgGrid->ItemHeight;

		//imgGrid->ItemWidth = (gridWidth + padLeft->Value + padRight->Value);
		//centerGrid->ItemWidth = imgGrid->ItemWidth;
		//imgGrid->ItemHeight = (gridHeight + padTop->Value + padBottom->Value);
		//centerGrid->ItemHeight = imgGrid->ItemHeight;

		//Show updated data
		showInformation();

		//Change alignment of all tiles accordingly
		for (int i = 0; i < imgGrid->ChildrenCount; i++)
		{
			TRectangle * item = dynamic_cast<TRectangle *>(imgGrid->Children->Items[i]);

			if (item)
			{
				alignImage(item, item->TagFloat);
			}
		}
	}
}


void __fastcall TSheetsForm::showInformation()
{
	int  adjW = (gridWidth + padLeft->Value + padRight->Value);
	int  adjH = (gridHeight + padTop->Value + padBottom->Value);
	bool flag = (imgGrid->ChildrenCount > 0);

	sheetWidth->Text = (colNum->Value * adjW);
	sheetHeight->Text = (rowNum->Value * adjH);
	cellWidth->Text = IntToStr(adjW);
	cellHeight->Text = IntToStr(adjH);

	//
	//----------------------------
	//
	if (!runAnimation->IsChecked)
	{//
		alignTL->Enabled = (selectionsList->Count);
		alignTC->Enabled = (selectionsList->Count);
		alignTR->Enabled = (selectionsList->Count);
		alignLC->Enabled = (selectionsList->Count);
		alignC->Enabled = (selectionsList->Count);
		alignRC->Enabled = (selectionsList->Count);
		alignBL->Enabled = (selectionsList->Count);
		alignBC->Enabled = (selectionsList->Count);
		alignBR->Enabled = (selectionsList->Count);
		//Effects
		tlEfx->Enabled = !(selectionsList->Count);
		tcEfx->Enabled = !(selectionsList->Count);
		trEfx->Enabled = !(selectionsList->Count);
		lcEfx->Enabled = !(selectionsList->Count);
		cEfx->Enabled = !(selectionsList->Count);
		rcEfx->Enabled = !(selectionsList->Count);
		blEfx->Enabled = !(selectionsList->Count);
		bcEfx->Enabled = !(selectionsList->Count);
		brEfx->Enabled = !(selectionsList->Count);
		//
		padTop->Enabled = true;
		padLeft->Enabled = true;
		padRight->Enabled = true;
		padBottom->Enabled = true;
		//
		colNum->Enabled = true;
		rowNum->Enabled = true;
		//
		loadImg->Enabled = true;
		exportImage->Enabled = true;
		alignToWindow->Enabled = true;
	}
	else
	{
		//In animation mode
		flag = false;
		alignTL->Enabled = flag;
		alignTC->Enabled = flag;
		alignTR->Enabled = flag;
		alignLC->Enabled = flag;
		alignC->Enabled = flag;
		alignRC->Enabled = flag;
		alignBL->Enabled = flag;
		alignBC->Enabled = flag;
		alignBR->Enabled = flag;
		//Effects
		tlEfx->Enabled = !flag;
		tcEfx->Enabled = !flag;
		trEfx->Enabled = !flag;
		lcEfx->Enabled = !flag;
		cEfx->Enabled = !flag;
		rcEfx->Enabled = !flag;
		blEfx->Enabled = !flag;
		bcEfx->Enabled = !flag;
		brEfx->Enabled = !flag;
		//
		padTop->Enabled = false;
		padLeft->Enabled = false;
		padRight->Enabled = false;
		padBottom->Enabled = false;
		//
		colNum->Enabled = false;
		rowNum->Enabled = false;
		//
		loadImg->Enabled = false;
		exportImage->Enabled = false;
		alignToWindow->Enabled = false;
	}
}


void __fastcall TSheetsForm::padTopClick(TObject *Sender)
{
	TSpinBox *sb = dynamic_cast<TSpinBox *>(Sender);

	if (sb)
	{
		sb->SelStart = 0;
		sb->SelLength = sb->Text.Length();
	}
}


//---------------------------------------------------------------------------
//
//Double click the grid to select a frame
//
//---------------------------------------------------------------------------
void __fastcall TSheetsForm::gridParentDblClick(TObject *Sender)
{
	if (imgGrid->ChildrenCount)
	{
		//Select a cell
		int adjW = (gridWidth + padLeft->Value + padRight->Value);
		int adjH = (gridHeight + padTop->Value + padBottom->Value);
		int x    = gridParent->PressedPosition.x / adjW;
		int y    = gridParent->PressedPosition.y / adjH;
		char buf[16];

		if ((y * colNum->Value + x) < imgGrid->ChildrenCount)
		{
			TPanel *obj = dynamic_cast<TPanel *>(centerGrid->Children->Items[y * colNum->Value + x]);

			if (obj)
			{
				sprintf(buf, "%005d", obj->Tag);

				if (obj->TagFloat == 0)
				{
					obj->TagFloat= 0.5;
					selectionsList->Add(buf);

					int color = ((int)gridParent->Stroke->Color & 0x00FFFFFF);
					color |= (int)(0xFF000000 * obj->TagFloat);
					obj->StylesData["myTintRect.Fill.Color"] = TValue::From<TAlphaColor>(color);
				}
				else
				{
					int c = selectionsList->IndexOf(buf);

					selectionsList->Delete(c);
					obj->TagFloat= 0;
					obj->StylesData["myTintRect.Fill.Color"] = TValue::From<TAlphaColor>(0);
				}
			}

			//Updates
			showInformation();
		}
	}
}


//---------------------------------------------------------------------------
//
//Common method used by the menu system to select a background pattern
//
//---------------------------------------------------------------------------
void __fastcall TSheetsForm::blackClick(TObject *Sender)
{
	TMenuItem *m = dynamic_cast<TMenuItem *>(Sender);

	if (m)
	{
		m->IsChecked = true;
		SheetsForm->Fill->Bitmap->Bitmap->Assign
			(
			ImageList1->Bitmap(TSize(16, 16), m->ImageIndex)
			);
		//
		SheetsForm->Tag = m->ImageIndex;
	}
}


void __fastcall TSheetsForm::selectAllClick(TObject *Sender)
{
	char buf[16];

	//Select it all
	selectionsList->Clear();

	imgGrid->BeginUpdate();

	//Select it all
	for (int i = 0; i < imgGrid->ChildrenCount; i++)
	{
		TPanel *obj = dynamic_cast<TPanel *>(centerGrid->Children->Items[i]);

		if (obj)
		{
			obj->TagFloat= 0.5;
			sprintf(buf, "%005d", obj->Tag);
			selectionsList->Add(buf);
			//
			selectionsList->Add(buf);

			int color = ((int)gridParent->Stroke->Color & 0x00FFFFFF);
			color |= (int)(0xFF000000 * obj->TagFloat);
			obj->StylesData["myTintRect.Fill.Color"] = TValue::From<TAlphaColor>(color);
			//
			//obj->StylesData["myTint.Opacity"] = TValue::From<float>(obj->TagFloat);
		}
	}
	//
	imgGrid->EndUpdate();

	//Updates
	showInformation();
}


void __fastcall TSheetsForm::clearSelectionClick(TObject *Sender)
{
	char buf[16];

	//Clear selections
	selectionsList->Clear();

	imgGrid->BeginUpdate();

	//Clear it all
	for (int i = 0; i < imgGrid->ChildrenCount; i++)
	{
		TPanel *obj = dynamic_cast<TPanel *>(centerGrid->Children->Items[i]);

		if (obj)
		{
			obj->TagFloat = 0;
			obj->StylesData["myTintRect.Fill.Color"] = TValue::From<TAlphaColor>(0);
		}
	}
	//
	imgGrid->EndUpdate();

	//Updates
	showInformation();
}


//---------------------------------------------------------------------------
//
//Delete any selected items WITHOUT confirmation
//
//---------------------------------------------------------------------------
void __fastcall TSheetsForm::delItemsClick(TObject *Sender)
{
	TmsgForm *msg = new TmsgForm(Application);

	msg->messageText->Text = "Do you wish to delete the selection(s)";
	msg->Caption = "Save current project...";
	msg->ShowModal();

	//What result did we receive?
	if (msg->ModalResult == mrYes)
	{
		if (selectionsList->Count == imgGrid->ChildrenCount)
		{
			imgGrid->DeleteChildren();
			centerGrid->DeleteChildren();
		}
		else
		{

			imgGrid->BeginUpdate();

			//Delete the selection
			for (int i = selectionsList->Count - 1; i > -1; i--)
			{
				int         index = StrToInt(selectionsList->Strings[i]);
				TRectangle *obj   = dynamic_cast<TRectangle *>(imgGrid->Children->Items[index]);
				TPanel *    pnl   = dynamic_cast<TPanel *>(centerGrid->Children->Items[index]);

				//if (obj)
				{
					centerGrid->RemoveObject(index);
					SheetsForm->Caption = i;
					imgGrid->RemoveObject(index);

					delete obj;
					delete pnl;
				}
			}

			//Change the index values for each remaining object
			for (int i = 0; i < centerGrid->ChildrenCount; i++)
			{
				TPanel *    obj  = dynamic_cast<TPanel *>(centerGrid->Children->Items[i]);
				TRectangle *item = dynamic_cast<TRectangle *>(imgGrid->Children->Items[i]);

				if (obj && item)
				{
					obj->Tag = i;
					item->Tag = i;

					char buf[32];
					sprintf(buf, "%003d", i + 1);

					obj->StylesData["myText.Text"] = TValue::From<AnsiString>(AnsiString(buf));
					break;
				}

				//
				selectionsList->Clear();
				//
				imgGrid->EndUpdate();

				//Close Message box
				//msgForm->DisposeOf();
			}
		}
	}

	//Clear selections
	selectionsList->Clear();

	//Adjust the rows to match columns and image count
	equalizeGrid();

	//Updates
	showInformation();

}


//---------------------------------------------------------------------------
//
//Important for the popup menu used with frames. Since th emain form is
//scaled, this prevents that popup from being scaling when associated to the
//main window as THE popup menu.
//
//---------------------------------------------------------------------------
void __fastcall TSheetsForm::gridParentMouseDown(TObject *Sender, TMouseButton Button, TShiftState Shift, float X, float Y)
{
	if (Shift.Contains(ssRight))
	{
		TPointF pos = Screen->MousePos();
		PopupMenu1->Popup(
			pos.x,
			pos.y
			);
	}
}


//---------------------------------------------------------------------------
//
//This method will trim empty pixels from the bottoms of each selected frame
//
//---------------------------------------------------------------------------
void __fastcall TSheetsForm::imageCropClick(TObject *Sender)
{
	if (selectionsList->Count)
	{
		int lowest = 0;

		for (int i = 0; i < selectionsList->Count; i++)
		{
			int         index = StrToInt(selectionsList->Strings[i]);
			TRectangle *obj   = dynamic_cast<TRectangle *>(imgGrid->Children->Items[index]);

			if (obj)
			{
				int low = -1;
				TBitmapData bm;
				TBitmap *src = obj->Fill->Bitmap->Bitmap;
				TAlphaColor color;

				//Get bitmap data access !
				if (src && src->Map(TMapAccess::ReadWrite, bm))
				{
					unsigned int *data = (unsigned int *)bm.Data;

					//Test direct pixel access here
					for (int y = src->Height; y > -1; y--)
					{
						//Get the lowest pixel location
						for (int x = 0; x < src->Width; x++)
						{
							//Get a color to test
							color = bm.GetPixel(x, y);

							if (color)
							{
								low = y + 1;
								break;
							}
						}

						if (low > -1)
						{
							obj->Height = low;
							alignImage(obj, obj->TagFloat);
							break;
						}
					}
				}

				//Done
				src->Unmap(bm);
			}
		}
	}
}
