#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#pragma warning(disable:4995)
#pragma warning(disable:4996)

#include "sutil.h"
#include "GuiTui.h"
#include "dxstdafx.h"

#define MAXMESSAGE 8

static ID3DXFont*              g_pFont = NULL;         // Font for drawing text
static ID3DXFont*              g_pLargeFont = NULL;    // Font for drawing text
static ID3DXSprite*            g_pTextSprite = NULL;   // Sprite for batching draw text calls
static ID3DXFont*              g_pTinyFont=0;          // Font for drawing text

#include "cparser.h"
#include "tui.h"
#include "log.h"
#include "PhysX.h"
#include "MyGlobals.h"

extern float gFPS;

int gWINDOW_WIDE=1024;
int gWINDOW_TALL=768;

#define STATIC_ID_START 10000



static CDXUTDialog * gConsole = 0;
static CDXUTDialog * gDialog = 0;

//==================================================================================
void CALLBACK gOnUIPageEvent( UINT nEvent, int nControlID, CDXUTControl* pControl,void *pUserContext )
{
	CDXUTDialog *dialog = pControl->m_pDialog;

	void *userdata = dialog->GetUserData();
	if ( userdata )
	{
		TuiElement *page = (TuiElement *) userdata;
		TextUserInterface *tui = gAllGlobals.gTui.ref();

		if ( page )
		{
			TuiElement *ret = page->GetElement( nControlID-1 );

			switch ( ret->GetType() )
			{
				case TT_MULTI_SELECT:
					if ( 1 )
					{
						switch( nEvent )
						{
							case EVENT_LISTBOX_ITEM_DBLCLK:
							{
								DXUTListBoxItem *pItem = ((CDXUTListBox *)pControl)->GetItem( ((CDXUTListBox *)pControl)->GetSelectedIndex( -1 ) );
								break;
							}
							case EVENT_LISTBOX_SELECTION:
							{
								CDXUTListBox *pListBox = (CDXUTListBox *)pControl;
								TuiChoiceVector &choices = ret->GetChoices();
								int count = (int)choices.size();
								for (int i=0; i<count; i++)
								{
									DXUTListBoxItem *item = pListBox->GetItem(i);
									TuiChoice  &choice = choices[i];
									assert(item);
									if ( item )
									{
										if ( choice.GetState() != item->bSelected )
										{
											choice.SetState(item->bSelected);
											const char *args[2];
											args[0] = choice.GetArg().Get();

											if ( choice.GetState() )
												args[1] = "true";
											else
												args[1] = "false";

											page->ExecuteElement( nControlID-1, 2, args, tui );
										}
									}
								}

							}
						}
					}
					break;
				case TT_SLIDER:
					if ( 1 )
					{
						CDXUTSlider *slider = (CDXUTSlider *) pControl;
						int v = slider->GetValue();
						float fv = ret->GetSliderValue(v);
						char scratch[512];
  						sprintf(scratch,"%0.3f", fv );

						const char *args[1];
						args[0] = scratch;
						page->ExecuteElement( nControlID-1, 1, args, tui );
					}
					break;
				case TT_COMBO:
					if ( 1 )
					{
						CDXUTComboBox *combo = (CDXUTComboBox *) pControl;
						DXUTComboBoxItem *pItem = combo->GetSelectedItem();
						wchar_t *string = pItem->strText;
						char scratch[512];
						wcstombs(scratch, string, 512 );
						const char *args[1];
						args[0] = scratch;
						page->ExecuteElement( nControlID-1, 1, args,tui );
					
 						// now, do we need to hide any items (or show them)?
 						page->OnComboBoxChange( scratch, nControlID-1 );
					}
					break;
				case TT_BUTTON:
				case TT_PAGE:
					page->ExecuteElement( nControlID-1, 0, 0, tui );
					break;
				case TT_CHECKBOX:
					if ( 1 )
					{
						CDXUTCheckBox *pCheck = (CDXUTCheckBox *)pControl;
						bool state = pCheck->GetChecked();
						const char *args[1];
						if ( state )
						{
							args[0] = "true";
						}
						else
						{
							args[0] = "false";
						}
						page->ExecuteElement( nControlID-1, 1, args, tui );

 						// now, do we need to hide any items (or show them)?
 						page->OnCheckboxChange( state, nControlID-1 );
					}
					break;
			}
			if ( tui )
			{
				if ( ret && ret->GetType() == TT_PAGE )
				{
					// tell the last page it is now exiting
					TuiElement *exitingPage = tui->GetPage();
					if ( exitingPage )
					{
						exitingPage->OnExit( exitingPage );
					}

					tui->SetPage(ret);

					// tell new page it is now loading
					TuiElement *loadingPage = tui->GetPage();
					if ( loadingPage )
					{
						loadingPage->OnLoad( loadingPage );
					}
				}
			}
		}
	}
	else
	{
		// it's for the console
		CDXUTEditBox *edit = (CDXUTEditBox *) pControl;
		switch ( nEvent )
		{
			case EVENT_EDITBOX_STRING:
				if ( 1 )
				{
					LPCWSTR text = edit->GetText();
					char scratch[1024];
					WideToChar(text,scratch,1024);
					gAllGlobals.gLog.ref()->Display("%s\r\n", scratch);
					CPARSER.Parse("%s",scratch);
					edit->SetText(L"",true);
				}
				break;
			case EVENT_EDITBOX_CHANGE:
				break;
			default:
				break;
		}
	}

}

//==================================================================================
GuiTui::GuiTui(void *device,void *hwnd,void *instance,CDXUTDialogResourceManager *pManager) :
	mConsoleKeyDown( false )
{
	mConsoleIndex = 0;
	mShowConsole = false;
	mCapture     = false;
	mDtime       = 1.0f / 60.0f;
	mDialogResourceManager = pManager;
	if ( device )
	{
		IDirect3DDevice9 *pd3dDevice = (IDirect3DDevice9 *) device;
		D3DXCreateSprite( pd3dDevice, &g_pTextSprite );
		D3DXCreateFont(pd3dDevice,  9, 0, 0,       1, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Courier",     &g_pTinyFont );
		D3DXCreateFont(pd3dDevice, 15, 0, FW_BOLD, 1, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Arial",       &g_pFont );
		D3DXCreateFont(pd3dDevice, 26, 0, 0,       1, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Arial Black", &g_pLargeFont );

	  mWidth = 1024;
	  mHeight = 768;

		gAllGlobals.gGuiTui.set(this);
	}
}

//==================================================================================
GuiTui::~GuiTui(void)
{
	SAFE_RELEASE(g_pFont);
	SAFE_RELEASE(g_pLargeFont);
	SAFE_RELEASE(g_pTextSprite);
	SAFE_RELEASE(g_pTinyFont);
	delete gConsole;
	gConsole = 0;
	delete gDialog;
	gDialog = 0;
}

//==================================================================================
int GuiTui::CommandCallback(int token,int count,const char **arglist)
{
	int ret = 0;


	return ret;
}


//==================================================================================
bool GuiTui::HasCursorFocus()
{
	return true;
}

//==================================================================================
void GuiTui::HideAllGuiForTuiPages()
{
}

//==================================================================================
bool GuiTui::MsgProc(void *hwnd,unsigned int msg,unsigned int wparam,unsigned int lparam)
{
	bool ret = false;

	TextUserInterface *tui = gAllGlobals.gTui.ref();

	if ( tui && tui->GetShowGraphics() )
	{
		TuiElement *page = tui->GetPage();
		if ( page )
		{
			#if defined(WIN32)
			CDXUTDialog *dialog = (CDXUTDialog *)page->GetUserPtr();
			if ( dialog )
			{
				ret = dialog->MsgProc( (HWND) hwnd, (UINT) msg,(WPARAM) wparam, (LPARAM) lparam );
			}
			#endif
		}
	}

	if ( gConsole )
	{
		bool ignore = false;

		if ( msg == WM_KEYDOWN )
		{
			if ( (wparam == 126 || wparam == 96 || wparam == 9 ) )
			{
				ignore = true;

				if ( !mConsoleKeyDown )
				{
					mConsoleKeyDown = true;
					if ( mShowConsole )
					{
						mShowConsole = false;
					}
					else
					{
						mShowConsole = true;

						// indicate I handled this msg.
						ret = true;
					}
				}
			}
			if ( wparam == VK_F1 )
			{
				gAllGlobals.gTui.ref()->ToggleShowGraphics();
			}
		}
		else if ( msg == WM_KEYUP )
		{
			if ( (wparam == 126 || wparam == 96 || wparam == 9 ) )
			{
				ignore = true;
				mConsoleKeyDown = false;
			}
		}


		// if the user didn't hit one of the "special" keys, put key hit in the console
		if ( !ignore && mShowConsole )
		{
 			ret = gConsole->MsgProc( (HWND) hwnd, (UINT) msg,(WPARAM) wparam, (LPARAM) lparam );
		}
	}

	return ret;
}

//==================================================================================
void GuiTui::OnMouseDown(unsigned int uiX, unsigned int uiY, unsigned int uiButtons)
{
}

//==================================================================================
void GuiTui::OnMouseMove(unsigned int uiX, unsigned int uiY, unsigned int uiButtons)
{
}

//==================================================================================
void GuiTui::OnMouseUp(unsigned int uiX, unsigned int uiY, unsigned int uiButtons)
{
}

//==================================================================================
void GuiTui::PrintText(const char *str,int x,int y,unsigned int color)
{
	if ( str )
	{
		TextUserInterface *tui = 0;

		if( tui )
		{
			if ( g_pTinyFont && g_pTextSprite )
			{
				CDXUTTextHelper txtHelper( g_pTinyFont, g_pTextSprite, 30 );

				txtHelper.Begin();
				txtHelper.SetInsertionPos( x, y );
				txtHelper.SetForegroundColor( color );

				wchar_t wfoo[1024];
				CharToWide(str,wfoo,1024);

				txtHelper.DrawTextLine(wfoo);

				txtHelper.End();
			}
		}
	}
}

//==================================================================================
//*** The DirectX GUI version
//==================================================================================
void GuiTui::Render(void)
{
	int fontSize = 16;

	if (gAllGlobals.gGuiTui.ref() == 0 )
		return;

	if ( mCapture )
		return;

  if ( gAllGlobals.gLog.ref() )
  {
		const char *msg = gAllGlobals.gLog.ref()->GetLogMessage();
		if ( msg )
		{
			AddConsoleMessage(msg);
		}
  }

	if ( 1 )
	{

		if ( gConsole == 0 )
		{
			gConsole = new  CDXUTDialog;
			gConsole->Init(mDialogResourceManager);
			gConsole->SetCallback( gOnUIPageEvent );
			gConsole->SetUserData(0);
			gConsole->AddEditBox( 1, L"", 0, gWINDOW_TALL-26*2, 1000, 36, true, 0 );
		}

 		if ( mShowConsole )
 		{

 			CDXUTEditBox *edit = gConsole->GetEditBox(1);

 			if ( edit )
 			{
			  edit->SetFocused();
 			}

			gConsole->OnRender( mDtime );

  		if ( g_pTinyFont && g_pTextSprite )
 			{
 		  	CDXUTTextHelper txtHelper( g_pTinyFont, g_pTextSprite, fontSize );

 				txtHelper.Begin();
   			txtHelper.SetForegroundColor( 0xFFFFFF00 );

				int maxPos		= mHeight - 2*fontSize - 2;
				int addAmt		= fontSize;
				int menuHeight  = fontSize;
				int startPos	= maxPos - addAmt * CONSOLE_LINES - menuHeight;

				// ok..print console text!
 				for (int i=0; i<CONSOLE_LINES; i++)
 				{
 					const char *msg = GetConsoleMessage(i);
					if ( !msg )
						break;
   				wchar_t wfoo[1024];
 	  			CharToWide(msg,wfoo,1024);
					txtHelper.SetInsertionPos( 0, startPos );
					startPos += addAmt;
   				txtHelper.DrawTextLine(wfoo);
 				}
 		  	txtHelper.End();
 			}
 		}
 		else
 		{
  		if ( g_pTinyFont && g_pTextSprite )
 			{
	  		CDXUTTextHelper txtHelper( g_pTinyFont, g_pTextSprite, fontSize );

 				txtHelper.Begin();
 				txtHelper.SetForegroundColor( 0xFFD0D0D0 );

   				// ok..print console text!

				int maxPos		= mHeight - fontSize;
				int addAmt		= fontSize;
				int startPos	= maxPos - addAmt * 5;

 				for (int i=(CONSOLE_LINES-5); i<CONSOLE_LINES; i++)
 				{
 					const char *msg = GetConsoleMessage(i);
					if ( !msg )
						break;
  				wchar_t wfoo[1024];
	  			CharToWide(msg,wfoo,1024);
					txtHelper.SetInsertionPos( 0, startPos );
 					txtHelper.DrawTextLine(wfoo);
					startPos += addAmt;
 				}
	  		txtHelper.End();
 			}
 		}

		TextUserInterface *tui = gAllGlobals.gTui.ref();

		if ( tui && tui->GetShowGraphics() && !mShowConsole )
		{
			TuiElement *page = tui->GetPage();

			if ( page )
			{
				CDXUTDialog *dialog = (CDXUTDialog *)page->GetUserPtr();

 				bool dialogExists = ( dialog != 0 );

 				// k, if the dialog doesn't exist or the page needs updating, go in
 				if ( !dialogExists || page->GetNeedsUpdating() )
 				{
 					// first, let it figure out what is now visible
 					page->DetermineVisibleElements( 0, -1 );

 					// if the dialog does not exist, have it determine rescripted items
 					if ( !dialogExists )
 					{
 						page->DetermineRescriptedItems( 0, -1 );
 					}
 					else if ( page->GetNeedsUpdating() )
 					{
 						dialogExists = false;
 						page->SetUserPtr( 0 );
 						page->SetNeedsUpdating( false );
 						delete dialog;
						gDialog = 0;
 					}
 				}

 				if ( dialogExists == false )
				{
					dialog = new  CDXUTDialog;
			        dialog->Init(mDialogResourceManager);
					dialog->SetCallback( gOnUIPageEvent );
					dialog->SetUserData(page);
					gDialog = dialog;

					const PageLayout &pl   = page->getPageLayout();

					int bwidth             = pl.mButtonWidth;
					int lmargin            = pl.mLayoutLeftMargin;
					int layout_width       = pl.mLayoutWidth;
					int button_spacing     = pl.mButtonSpacing;
					int bhit               = pl.mButtonHeight;
					int checkbox_spacing   = DFLT_CHECKBOX_SPACING;

					int text_spacing       = pl.mTextSpacing;
					int slider_spacing     = pl.mSliderSpacing;
					int slider_text_width  = pl.mSliderTextWidth;
					int slider_height      = pl.mSliderHeight;
					int slider_width       = pl.mSliderWidth;

					int combo_width        = pl.mComboWidth;
					int combo_spacing      = pl.mComboSpacing;
					int combo_height       = pl.mComboHeight;
					int combo_dropdown     = pl.mComboDropdown;

					int list_width         = pl.mListWidth;

					int xloc     = lmargin;
					int yloc     = 10;
					int id       = 0;
					int staticId = STATIC_ID_START;

					page->SetUserPtr(dialog);

					TuiElementVector &elements = page->GetElements();
					TuiElementVector::iterator i;

					for (i=elements.begin(); i!=elements.end(); ++i)
					{
						++id;

						TuiElement *tui = (*i);
 						if ( tui && tui->IsVisible() )
						{
							switch ( tui->GetType() )
							{
								case TT_TEXT:
								{
									wchar_t scratch[512];
									mbstowcs(scratch, tui->GetText(), 512 );
									if ( xloc != lmargin )
									{
										xloc = lmargin;
										yloc+= text_spacing;
									}
									dialog->AddStatic( staticId, scratch, xloc, yloc, 500, 20, 0 );

									// k, now make sure it is justified to the left
									CDXUTControl *ctrl = dialog->GetControl( staticId );
									if ( ctrl && ( ctrl->GetType() == DXUT_CONTROL_STATIC ) )
									{
										// set color
										ctrl->SetUserData( (void *)tui->GetTextColor() );

										CDXUTElement *element = ctrl->GetElement( 0 );
										if ( element )
										{
											// make sure it is left-justified, and vertically centered
											element->dwTextFormat = (DT_LEFT | DT_VCENTER);
										}
									}

									// move next element down
									yloc += text_spacing;

									// increment static id
									++staticId;
								}
								break;

								case TT_CHECKBOX:
									if ( 1 )
									{
										wchar_t scratch[512];
										mbstowcs(scratch, tui->GetName().Get(), 512 );
										dialog->AddCheckBox( id, scratch, xloc, yloc, bwidth, bhit, tui->GetState(), 0);
										xloc+=(bwidth+10);
										if ( xloc > (layout_width-(bwidth+10)) )
										{
											xloc = lmargin;
											yloc += checkbox_spacing;
										}
									}
									break;
								case TT_SLIDER:
									if ( 1 )
									{
										if ( xloc != lmargin )
										{
											xloc = lmargin;
											yloc += slider_spacing;
										}
										wchar_t scratch[512];
										mbstowcs(scratch, tui->GetName().Get(), 512 );
										dialog->AddStatic(0, scratch, xloc, yloc, slider_text_width, slider_height, 0 );
										dialog->AddSlider(id, xloc+slider_text_width+10, yloc, slider_width, slider_height, 0, 1000, 0, false, 0);
										dialog->AddStatic(id+1000, L"10mhz", xloc+slider_text_width+10+slider_width+10, yloc, slider_text_width, slider_height, 0 );
										xloc = lmargin;
										yloc += slider_spacing;
									}
									break;
								case TT_COMBO:
									if ( 1 )
									{
										if ( xloc != lmargin )
										{
											xloc = lmargin;
											yloc += combo_spacing;
										}
										wchar_t scratch[512];
										mbstowcs(scratch, tui->GetName().Get(), 512 );
										dialog->AddStatic(0, scratch, xloc, yloc, combo_width, combo_height, 0 );
										CDXUTComboBox *pCombo;
										dialog->AddComboBox(id, xloc, yloc+combo_height, combo_width, combo_height, 0, false, &pCombo);
										if ( pCombo )
										{
											// ok..populate the combo box now..
											TuiChoiceVector &choices = tui->GetChoices();
											TuiChoiceVector::iterator i;
											int index = 0;
											for (i=choices.begin(); i!=choices.end(); ++i)
											{
												wchar_t scratch[512];
												mbstowcs(scratch, (*i).GetChoice().Get(), 512 );
												pCombo->AddItem( scratch, (LPVOID) index );
												index++;
											}

											pCombo->SetDropHeight(combo_dropdown);

										}
										xloc = lmargin;
										yloc += combo_spacing;
									}
									break;
								case TT_MULTI_SELECT:
									if ( 1 )
									{
										if ( xloc != lmargin )
										{
											xloc = lmargin;
											yloc+=30;
										}
										TuiChoiceVector &choices = tui->GetChoices();

										int list_height = choices.size()*15;

										if ( (yloc+list_height) > 700 )
										{
											list_height = 700-yloc;
										}

										wchar_t scratch[512];
										mbstowcs(scratch, tui->GetName().Get(), 512 );
										dialog->AddStatic(0, scratch, xloc, yloc, 180, 20, 0 );
										dialog->AddListBox(id, xloc, yloc+26, list_width, list_height, CDXUTListBox::MULTISELECTION );
										CDXUTListBox *pListBox = dialog->GetListBox(id);
										if(pListBox)
										{
											// ok..populate the combo box now..
											TuiChoiceVector::iterator i;
											int index = 0;
											for (i=choices.begin(); i!=choices.end(); ++i)
											{
												wchar_t scratch[512];
												mbstowcs(scratch, (*i).GetChoice().Get(), 512 );
												pListBox->AddItem( scratch, (LPVOID) index );
												index++;
											}
										}
										xloc = lmargin;
										yloc+=list_height;
									}
									break;
								case TT_BUTTON:
								case TT_PAGE:
									if ( 1 )
									{
										wchar_t scratch[512];
										mbstowcs(scratch, tui->GetName().Get(), 512 );
										dialog->AddButton( id, scratch, xloc, yloc, bwidth, 20, 0);
										xloc += (bwidth+button_spacing);

										if ( xloc > (layout_width-(bwidth+button_spacing)) )
										{
											xloc = lmargin;
											yloc+=24;
										}
									}
									break;
							}
						}
					}
				}

				// SRM : do this only when dialog exists
 				if ( dialogExists )
				{
					// static's that are just static's have id's of 10000+
					int id = STATIC_ID_START;

					CDXUTControl *ctrl = dialog->GetControl( id++ );
					while ( ctrl )
					{
						if ( ctrl->GetType() == DXUT_CONTROL_STATIC )
						{
							CDXUTElement *element = ctrl->GetElement( 0 );
							void *userData        = ctrl->GetUserData();
							if ( element && userData )
							{
								//element->FontColor.Current = D3DCOLOR_ARGB(255, 255, 255, 0);
								element->FontColor.Current = (unsigned)( userData );
							}
						}
						ctrl = dialog->GetControl( id++ );
					}
				}

				dialog->OnRender(mDtime);

			}
		}


		if( 1 )
		{

      if ( tui /*&& tui->GetShowGraphics()*/ )
			{

				CDXUTTextHelper txtHelper( g_pFont, g_pTextSprite, 15 );

				// Output statistics
				txtHelper.Begin();
				txtHelper.SetInsertionPos( 5, 5 );
				txtHelper.SetForegroundColor( D3DXCOLOR( 0.8f, 0.8f, 0.8f, 0.8f ) );
				txtHelper.DrawTextLine( DXUTGetFrameStats() );
				txtHelper.DrawTextLine( DXUTGetDeviceStats() );

				wchar_t scratch[512];
				const char *stats = 0;

				if (stats )
				{
					mbstowcs(scratch, stats, 512);
					txtHelper.DrawTextLine( scratch );
				}

				if ( 1 )
				{
  				txtHelper.SetForegroundColor( D3DXCOLOR( 1.0f, 1.0f, 1.0f, 1.0f ) );

  				// Draw help
  				txtHelper.SetForegroundColor( D3DXCOLOR( 1.0f, 1.0f, 1.0f, 1.0f ) );

					wchar_t temp[512];
					if(px_paused())
						StringCchPrintf(temp,512, L"FPS: %3d (Paused)!", (int)gFPS);
					else
						StringCchPrintf(temp,512, L"FPS: %3d", (int)gFPS);
  				txtHelper.DrawTextLine(temp);


  				txtHelper.DrawTextLine( L"Press F1 to toggle UI" );
  				if ( gAllGlobals.gTui.ref()->GetShowGraphics() )
  				{
	  				txtHelper.DrawTextLine( L"Press ESC to exit" );
	  				txtHelper.DrawTextLine( L"Press TAB for Console" );
	  				txtHelper.DrawTextLine( L"Press HOME camera back to default position" );
						txtHelper.DrawTextLine( L"Press R to reset scene" );
						txtHelper.DrawTextLine( L"Press P to pause/unpause physics" );
						txtHelper.DrawTextLine( L"Press 0-9 to control camera speed" );
						txtHelper.DrawTextLine( L"Press SPACE to fire sphere" );
						txtHelper.DrawTextLine( L"Press B to toggle graphics/physics view" );
						txtHelper.DrawTextLine( L"Press F7 to toggle backface culling" );
						txtHelper.DrawTextLine( L"Mouse wheel zoom in/out" );
						txtHelper.DrawTextLine( L"Mouse wheel while dragging move object in/out" );
						txtHelper.DrawTextLine( L"Left Mouse rotate view" );
						txtHelper.DrawTextLine( L"Right Mouse rotate object" );
						txtHelper.DrawTextLine( L"SHIFT+Left Mouse delete tetrahedron" );
						txtHelper.DrawTextLine( L"SHIFT+Right Mouse un-delete tetrahedron" );
					}


  			}

				txtHelper.SetForegroundColor( D3DXCOLOR( 1.0f, 1.0f, 1.0f, 1.0f ) );

				txtHelper.End();
			}
		}

	}
}

//==================================================================================
// Update the UI's state.
//==================================================================================
void GuiTui::Update(void)
{
	TextUserInterface *tui = gAllGlobals.gTui.ref();
	if(!tui)
		return;

	TuiElement *page       = tui->GetPage();
	if(!page)
		return;

#if defined(WIN32)
	CDXUTDialog *dialog    = (CDXUTDialog *)page->GetUserPtr();
	if(!dialog)
		return;
#endif

	// SRM : k, here we ask the globals if anything changed
	// and if so, we may need to update some elements!!!
	

	int id   = 0;
	TuiElementVector &elements = page->GetElements();
	TuiElementVector::iterator i;
	for (i=elements.begin(); i!=elements.end(); ++i)
	{
		id++;
		TuiElement *tui = (*i);
		if ( tui )
		{
			switch ( tui->GetType() )
			{
				case TT_CHECKBOX:
				{
					bool state = tui->GetState();
					CDXUTCheckBox *checkbox = dialog->GetCheckBox(id);
					if ( checkbox )
					{
						checkbox->SetChecked( state );
					}
					break;
				}
				case TT_SLIDER:
				{
					char actualVal[128], displayVal[128];
					tui->GetArg( 0, displayVal );
					tui->GetArg_Actual( 0, actualVal );
					CDXUTSlider *pslider = dialog->GetSlider(id);
					CDXUTStatic *label  = dialog->GetStatic(id+1000);
					if ( pslider && label )
					{
						float v = (float) atof(actualVal);
						int slider = tui->GetSliderTick(v); // convert the slider value into a tick point 0-1000

						wchar_t wvalue[512];
						CharToWide( displayVal, wvalue, 512 );
						label->SetText( wvalue );
						pslider->SetValue( slider );
					}
					break;
				}
				case TT_COMBO:
				{
					char value[128];
					tui->GetArg( 0, value );
					CDXUTComboBox *combo = dialog->GetComboBox(id);
					if(combo && !combo->IsOpened())
					{
						bool found = false;
						size_t len = strlen(value);
						wchar_t wvalue[512];
						CharToWide(value,wvalue,512);
						//combo->SetSelectedByText(wvalue);
						UINT numItems = combo->GetNumItems();
						for(UINT index=0; index<numItems; index++)
						{
							DXUTComboBoxItem *item = combo->GetItem(index);
							if(item && !wcsnicmp(wvalue, item->strText, len))
							{
								combo->SetSelectedByIndex(index);
								found = true;
								break;
							}
						}
						//if(!found) combo->SetSelectedByIndex(0); // select none?
					}
					break;
				}
				case TT_MULTI_SELECT:
				{
					CDXUTListBox *listbox = dialog->GetListBox(id);
					assert(listbox);
					break;
				}
				case TT_BUTTON:
				case TT_PAGE:
				{
					break;
				}
			}
		}
	}
}

//==================================================================================
void GuiTui::SetWindow(void* hwnd)
{
}

//==================================================================================
bool GuiTui::ToggleUI(void)
{
	bool ret = false;
	TextUserInterface *tui = gAllGlobals.gTui.ref();
	if ( tui ) 
		ret = tui->ToggleShowGraphics();

	return ret;
}

//==================================================================================
void GuiTui::NotifyWindowResized( unsigned newWidth, unsigned newHeight )
{
	mWidth	= newWidth;
	mHeight	= newHeight;

	// update our pages
	TextUserInterface *tui = gAllGlobals.gTui.ref();
	if ( tui )
	{
		tui->NotifyAllPagesOfNewDimensions( newWidth, newHeight );
	}

	if ( gConsole )
	{
		CDXUTEditBox *edit = gConsole->GetEditBox( 1 );
		if ( edit )
		{
			// modify the console's position as well
			edit->SetLocation( 0, newHeight - DFLT_CONSOLE_HEIGHT );
			edit->SetSize( newWidth, DFLT_CONSOLE_HEIGHT );
		}
	}
}

//==================================================================================
void GuiTui::NotifySceneTypeChanged( bool hardwareOn )
{
	// k, now we need to go through and items that need to be disabled when
	// they need to know about the scene type turning on/off hardware
	TextUserInterface *tui = gAllGlobals.gTui.ref();
	if ( tui )
	{
		// in addition, let it know so it can go through
		// and enable/disable appropriate items for visibility
		tui->NotifyHardwareStatus( hardwareOn );
	
		if ( hardwareOn )
		{
			// k, now we need to make sure that it is checked appropriately
			TuiElement *page = tui->GetElementOfTypeWithName( TT_PAGE, "Physics" );
			if ( page )
			{
				int index = page->GetElementIndexOfTypeWithName( TT_CHECKBOX, "Always Use 6DOF" );
				if ( index >= 0 )
				{
					const char *args[1] = { "true" };
					page->ExecuteElement( index, 1, args, tui );
				}
			}
		}
	}
}

//==================================================================================
void PageLayout::Modify( unsigned newScreenWidth, unsigned newScreenHeight )
{
	// k, we are being told what the new screen width and height is, so we might need
	// to adjust ourselves
	mLayoutWidth		= newScreenWidth;
    mLayoutLeftMargin	= newScreenWidth - 3*(DFLT_BUTTON_WIDTH+DFLT_BUTTON_SPACING) - DFLT_LAYOUT_RIGHT_MARGIN;
}

void GuiTui::onLostDevice(void)
{
	if ( g_pFont )      g_pFont->OnLostDevice();
	if ( g_pLargeFont ) g_pLargeFont->OnLostDevice();
	if ( g_pTinyFont )  g_pTinyFont->OnLostDevice();
	if ( g_pTextSprite ) g_pTextSprite->OnLostDevice();
}

void GuiTui::onResetDevice(void *device)
{
	if ( g_pFont )      g_pFont->OnResetDevice();
	if ( g_pLargeFont ) g_pLargeFont->OnResetDevice();
	if ( g_pTinyFont )  g_pTinyFont->OnResetDevice();
	if ( g_pTextSprite ) g_pTextSprite->OnResetDevice();
}

