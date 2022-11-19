#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include <algorithm>

#include "tui.h"
#include "filesystem.h"
#include "sutil.h"
#include "log.h"
#include "MyGlobals.h"



static int gSaveFrame=0;

const StringRef sShow  = "show";
const StringRef sHide  = "hide";
const StringRef sFalse = "false";
const StringRef sZero  = "zero";
const StringRef sTrue  = "true";

enum TuiCommand
{
	TUI_BEGIN,
	TUI_SCRIPT_BEGIN,
	TUI_BUTTON_BEGIN,
	TUI_PAGE_BEGIN,
	TUI_CHECKBOX_BEGIN,
	TUI_SLIDER_BEGIN,
	TUI_COMBO_BEGIN,
	TUI_MULTI_SELECT_BEGIN,
	TUI_END,
	TUI_SCRIPT,
	TUI_COMMAND,
	TUI_ELEMENT,
	TUI_NAME,
	TUI_PAGE,  // set active page
	TUI_ARG,
	TUI_CHOICE,
	TUI_CHOICES,
	TUI_SLIDER_RANGE,
	TUI_DESCRIPTION,
	TUI_FILES,             // the list of options are files
	TUI_ACTORS,           // populate the list with names of actors in the scene.
	TUI_SAVE,
	TUI_SAVE_OK,
	TUI_EXECUTE_OK,
	TUI_EXECUTE_ON_LOAD,
	TUI_EXECUTE_ON_EXIT,
	TUI_RECIP,
	TUI_LIST,
	TUI_TECHNIQUES,
	TUI_COPY,
	TUI_SAVE_OPEN,							//
	TUI_COPY_OPEN,							//
	TUI_SAVE_ELEMENT,						//
	TUI_SAVE_CLOSE,							//
	TUI_TEXT_BEGIN,			// beginning of some text
	TUI_TEXT,				// what the text says
	TUI_TEXT_COLOR,			// color of text
	TUI_LOAD,        // load, but don't execute contents of combo boxes...
	TUI_CHECKBOX_ONCHECK_SHOW,
	TUI_COMBO_SELECT_SHOW,
	TUI_COMBO_SELECT_RESCRIPT,
	TUI_ELEMENT_PRIVATE,
	TUI_PHYSICS_MODE,

//
  TUI_FILE_OPEN,
  TUI_FILE_SAVE,
  TUI_FILE_CLOSE,
//

	// need to be able to reprogam the layout of a page!
	TUI_BUTTONWIDTH,
	TUI_BUTTONHEIGHT,
	TUI_BUTTONSPACING,
	TUI_LAYOUTLEFTMARGIN,
	TUI_LAYOUTWIDTH,
	TUI_SLIDERSPACING,
	TUI_SLIDERTEXTWIDTH,
	TUI_SLIDERHEIGHT,
	TUI_SLIDERWIDTH,
	TUI_COMBOWIDTH,
	TUI_COMBOSPACING,
	TUI_COMBOHEIGHT,
	TUI_COMBODROPDOWN,
	TUI_LISTWIDTH,
	TUI_TEXTSPACING,

	TUI_INT,

	TUI_LAST
};

//==================================================================================
TextUserInterface::TextUserInterface(const char *startup)
{
	gAllGlobals.gTui.set(this);
	mFph = 0;
	mShowGraphics = true;
	mCurrent = 0;
	mPage    = 0;
	mTuiLoad  = 0;
	mSlashDepth = 0;

	mEyePos[0] = 7.62f;
	mEyePos[1] = 9.71f;
	mEyePos[2] = 4.91f;

	mLookAt[0] = 0;
	mLookAt[1] = 1;
	mLookAt[2] = 0;

	AddToken("TuiSaveOpen",           TUI_SAVE_OPEN);
	AddToken("TuiCopyOpen",           TUI_COPY_OPEN);
	AddToken("TuiSaveElement",        TUI_SAVE_ELEMENT);
	AddToken("TuiSaveClose",          TUI_SAVE_CLOSE);

  AddToken("TuiFileOpen",           TUI_FILE_OPEN);
  AddToken("TuiFileSave",           TUI_FILE_SAVE);
  AddToken("TuiFileClose",          TUI_FILE_CLOSE);

	AddToken("TuiCopy",               TUI_COPY);
	AddToken("TuiList",               TUI_LIST);
	AddToken("TuiButtonBegin",        TUI_BUTTON_BEGIN);
	AddToken("TuiScriptBegin",        TUI_SCRIPT_BEGIN);
	AddToken("TuiPageBegin",          TUI_PAGE_BEGIN);
	AddToken("TuiBegin",              TUI_BUTTON_BEGIN);
	AddToken("TuiEnd",                TUI_END);
	AddToken("TuiSaveOk",             TUI_SAVE_OK); // ok to save setting
	AddToken("TuiExecuteOk",          TUI_EXECUTE_OK); // ok to execute setting when re-loaded
	AddToken("TuiExecuteOnLoad",	  TUI_EXECUTE_ON_LOAD );
	AddToken("TuiExecuteOnExit",	  TUI_EXECUTE_ON_EXIT );

	AddToken("TuiName",               TUI_NAME);
	AddToken("TuiScript",             TUI_SCRIPT);
	AddToken("TuiCommand",            TUI_COMMAND);
	AddToken("TuiElement",            TUI_ELEMENT);
	AddToken("TuiElementPrivate",     TUI_ELEMENT_PRIVATE);
	AddToken("TuiPhysicsMode",        TUI_PHYSICS_MODE );

	AddToken("TuiPage",               TUI_PAGE);

	AddToken("TuiRecip",              TUI_RECIP);

	AddToken("TuiCheckboxBegin",      TUI_CHECKBOX_BEGIN);
	AddToken("TuiSliderBegin",        TUI_SLIDER_BEGIN);
	AddToken("TuiComboBegin",         TUI_COMBO_BEGIN);
	AddToken("TuiMultiSelectBegin",   TUI_MULTI_SELECT_BEGIN);
	AddToken("TuiArg",                TUI_ARG);
	AddToken("TuiChoice",             TUI_CHOICE);
	AddToken("TuiChoices",            TUI_CHOICES);
	AddToken("TuiSliderRange",        TUI_SLIDER_RANGE);
	AddToken("TuiDescription",        TUI_DESCRIPTION);
	AddToken("TuiFiles",              TUI_FILES);
	AddToken("TuiActors",             TUI_ACTORS);
	AddToken("TuiTechniques",         TUI_TECHNIQUES);

	AddToken("TuiSave",               TUI_SAVE);

	AddToken( "TuiTextBegin",         TUI_TEXT_BEGIN );
	AddToken( "TuiText",              TUI_TEXT );
	AddToken( "TuiTextColor",         TUI_TEXT_COLOR );
	AddToken("TuiLoad",               TUI_LOAD);

 	AddToken( "TuiOnCheckShow",       TUI_CHECKBOX_ONCHECK_SHOW );
 	AddToken( "TuiOnComboSelectShow", TUI_COMBO_SELECT_SHOW );
 	AddToken( "TuiOnComboSelectReScript", TUI_COMBO_SELECT_RESCRIPT );


	AddToken("TuiButtonWidth",                   TUI_BUTTONWIDTH);
	AddToken("TuiButtonHeight",                  TUI_BUTTONHEIGHT);
	AddToken("TuiButtonSpacing",                 TUI_BUTTONSPACING);
	AddToken("TuiLayoutLeftMargin",              TUI_LAYOUTLEFTMARGIN);
	AddToken("TuiLayoutWidth",                   TUI_LAYOUTWIDTH);
	AddToken("TuiSliderSpacing",                 TUI_SLIDERSPACING);
	AddToken("TuiSliderTextWidth",               TUI_SLIDERTEXTWIDTH);
	AddToken("TuiSliderHeight",                  TUI_SLIDERHEIGHT);
	AddToken("TuiSliderWidth",                   TUI_SLIDERWIDTH);
	AddToken("TuiComboWidth",                    TUI_COMBOWIDTH);
	AddToken("TuiComboSpacing",                  TUI_COMBOSPACING);
	AddToken("TuiComboHeight",                   TUI_COMBOHEIGHT);
	AddToken("TuiComboDropdown",                 TUI_COMBODROPDOWN);
	AddToken("TuiListWidth",                     TUI_LISTWIDTH);
	AddToken("TuiTextSpacing",                   TUI_TEXTSPACING);

	AddToken("TuiInt", TUI_INT);

	CPARSER.AddFallback(this); // we are a filter for command parser fallback conditions (command not recognized)

	if ( startup )
	{
		CPARSER.Batch(startup);
	}
	mFileOpen = 0;
}

//==================================================================================
TextUserInterface::~TextUserInterface(void)
{
	TuiElementMap::iterator i;
	for (i=mTui.begin(); i!=mTui.end(); ++i)
	{
		TuiElement *ti = (*i).second;
		ti->End();
		delete ti;
	}
	mTui.clear();

	std::vector<TuiElement*>::iterator tei;
	for (tei=mTuiOthers.begin(); tei!=mTuiOthers.end(); ++tei)
	{
		TuiElement *ti = (*tei);
		ti->End();
		delete ti;
	}
	mTuiOthers.clear();
}

//==================================================================================
int TextUserInterface::CommandCallback(int token,int count,const char **arglist)
{
	int ret = 0;

	switch ( token )
	{
		case TUI_FILE_OPEN:
			if ( count == 2 )
			{
				const char *fname = gAllGlobals.gFileSystem.ref()->FileOpenString( arglist[1], false );
				if ( mFileOpen )
				{
					fclose(mFileOpen);
				}
				mFileOpen = fopen(fname,"wb");
			}
			break;
		case TUI_FILE_SAVE:
			if ( mFileOpen && count >= 2)
			{

				int start = 1;
				const char *check = arglist[1];
				if ( strncmp(check,"@if_",4) == 0 )
				{
					const char *arg = ArgumentLookup( &check[4] );
					if ( arg && stricmp(arg,"true") == 0 )
					{
						start = 2;
					}
					else
					{
						start = 0;
					}
				}
				if ( start )
				{
  				for (int i=start; i<count; i++)
  				{
  					const char *str = arglist[i];
  					if ( *str == '%' )
  					{
  						str = ArgumentLookup( &str[1] );
  					}
  					if ( hasSpace(str) )
  					{
  						fprintf(mFileOpen,"%c%s%c ", 34, str, 34 );
  					}
  					else
  					{
  						fprintf(mFileOpen,"%s ", str );
  					}
  				}
  				fprintf(mFileOpen,"\r\n");
  			}
			}
			break;
		case TUI_FILE_CLOSE:
			if ( mFileOpen )
			{
				fclose(mFileOpen);
				mFileOpen = false;
			}
			break;
		case TUI_INT:
			if ( mCurrent )
			{
				mCurrent->SetInt(true);
			}
			break;
    case TUI_BUTTONWIDTH:
    	if ( count == 2 )
    	{
    		int v = atoi( arglist[1] );
    		if ( mCurrent )
    		{
    			PageLayout &p = mCurrent->getPageLayout();
    			p.mButtonWidth = v;
    		}
			}
    	break;
    case TUI_BUTTONHEIGHT:
    	if ( count == 2 )
    	{
    		int v = atoi( arglist[1] );
    		if ( mCurrent )
    		{
    			PageLayout &p = mCurrent->getPageLayout();
    			p.mButtonHeight = v;
    		}
			}
    	break;
    case TUI_BUTTONSPACING:
    	if ( count == 2 )
    	{
    		int v = atoi( arglist[1] );
    		if ( mCurrent )
    		{
    			PageLayout &p = mCurrent->getPageLayout();
    			p.mButtonSpacing = v;
    		}
			}
   		break;
    case TUI_LAYOUTLEFTMARGIN:
    	if ( count == 2 )
    	{
    		int v = atoi( arglist[1] );
    		if ( mCurrent )
    		{
    			PageLayout &p = mCurrent->getPageLayout();
    			p.mLayoutLeftMargin = v;
    		}
			}
    	break;
    case TUI_LAYOUTWIDTH:
    	if ( count == 2 )
    	{
    		int v = atoi( arglist[1] );
    		if ( mCurrent )
    		{
    			PageLayout &p = mCurrent->getPageLayout();
    			p.mLayoutWidth = v;
    		}
			}
   		break;
    case TUI_SLIDERSPACING:
    	if ( count == 2 )
    	{
    		int v = atoi( arglist[1] );
    		if ( mCurrent )
    		{
    			PageLayout &p = mCurrent->getPageLayout();
    			p.mSliderSpacing = v;
    		}
			}
   		break;
    case TUI_SLIDERTEXTWIDTH:
    	if ( count == 2 )
    	{
    		int v = atoi( arglist[1] );
    		if ( mCurrent )
    		{
    			PageLayout &p = mCurrent->getPageLayout();
    			p.mSliderTextWidth = v;
    		}
			}
    	break;
    case TUI_SLIDERHEIGHT:
    	if ( count == 2 )
    	{
    		int v = atoi( arglist[1] );
    		if ( mCurrent )
    		{
    			PageLayout &p = mCurrent->getPageLayout();
    			p.mSliderHeight = v;
    		}
			}
    	break;
    case TUI_SLIDERWIDTH:
    	if ( count == 2 )
    	{
    		int v = atoi( arglist[1] );
    		if ( mCurrent )
    		{
    			PageLayout &p = mCurrent->getPageLayout();
    			p.mSliderWidth = v;
    		}
			}
    	break;
    case TUI_COMBOWIDTH:
    	if ( count == 2 )
    	{
    		int v = atoi( arglist[1] );
    		if ( mCurrent )
    		{
    			PageLayout &p = mCurrent->getPageLayout();
    			p.mComboWidth = v;
    		}
			}
    	break;
    case TUI_COMBOSPACING:
    	if ( count == 2 )
    	{
    		int v = atoi( arglist[1] );
    		if ( mCurrent )
    		{
    			PageLayout &p = mCurrent->getPageLayout();
    			p.mComboSpacing = v;
    		}
			}
    	break;
    case TUI_COMBOHEIGHT:
    	if ( count == 2 )
    	{
    		int v = atoi( arglist[1] );
    		if ( mCurrent )
    		{
    			PageLayout &p = mCurrent->getPageLayout();
    			p.mComboHeight = v;
    		}
			}
    	break;
    case TUI_COMBODROPDOWN:
    	if ( count == 2 )
    	{
    		int v = atoi( arglist[1] );
    		if ( mCurrent )
    		{
    			PageLayout &p = mCurrent->getPageLayout();
    			p.mComboDropdown = v;
    		}
			}
    	break;
    case TUI_LISTWIDTH:
    	if ( count == 2 )
    	{
    		int v = atoi( arglist[1] );
    		if ( mCurrent )
    		{
    			PageLayout &p = mCurrent->getPageLayout();
    			p.mListWidth = v;
    		}
			}
    	break;
    case TUI_TEXTSPACING:
    	if ( count == 2 )
    	{
    		int v = atoi( arglist[1] );
    		if ( mCurrent )
    		{
    			PageLayout &p = mCurrent->getPageLayout();
    			p.mTextSpacing = v;
    		}
			}
    	break;
		case TUI_LOAD:
			if ( count == 2 )
			{
				mTuiLoad = true;
				CPARSER.Parse("run %s", arglist[1]);
				mTuiLoad = false;
			}
			break;
		case TUI_SAVE_OPEN:
		case TUI_COPY_OPEN:
			if ( count == 2 )
			{
				if ( mFph )
					fclose(mFph);
				mFph = 0;
				if ( token == TUI_COPY_OPEN )
				{
  					char temp[512];
  					strcpy(temp,arglist[1]);
  					int len = (int)strlen(temp);
  					if ( len >= 4 )
  					{
  						temp[len-4] = 0;
  						for (unsigned int i=0; i<1000; ++i)
  						{
  							char scratch[512];
  							sprintf(scratch,"%s%d.psc",temp, i+1);
								const char *fname = gAllGlobals.gFileSystem.ref()->FileOpenString(scratch,true);
  							FILE *fph = fopen(fname,"rb");
  							if ( fph )
  							{
  								fclose(fph);
  							}
  							else
  							{
									fname = gAllGlobals.gFileSystem.ref()->FileOpenString(scratch,false);
									mFph = fopen(fname,"wb");
  								mSaveName = SGET(scratch);
  								break;
  							}
  						}
  					}
  				}
  				else
  				{
  					mSaveName = SGET(arglist[1]);
						const char *fname = gAllGlobals.gFileSystem.ref()->FileOpenString(arglist[1],false);
						mFph = fopen(fname,"wb");
  				}
			}
			break;
		case TUI_SAVE_ELEMENT:
			if ( count == 2 && mFph )
			{
				const StringRef ref = SGET(arglist[1]);
				TuiElement *t = Locate(ref);
				if ( t )
				{
					t->Save(mFph);
				}
			}
			break;
		case TUI_SAVE_CLOSE:
			if ( mFph )
			{
				fclose(mFph);
				mFph = 0;

				if ( count == 2 )
				{
					CPARSER.Parse("/addchoice %s %s", arglist[1], mSaveName.Get() );
					CPARSER.Parse("/%s %s", arglist[1], mSaveName.Get() );
				}
			}
			break;
		case TUI_COPY:
		case TUI_SAVE:
			if ( count == 2 )
			{
				const char *fname = arglist[1];
				char work[512];

				if ( token == TUI_COPY )
				{
					char scratch[512];
					strcpy(scratch,fname);
					fname = 0;
					char *dot = strrchr( scratch, '.' );
					if ( dot )
					{
						*dot = 0;
						for (unsigned int i=1; i<100; ++i)
						{
							if ( strncmp(scratch,"fp_",3) != 0 )
							{
							  sprintf(work,"fp_%s_copy%d.psc", scratch, i );
							}
							else
							{
 							  sprintf(work,"%s_copy%d.psc", scratch, i );
							}
							const char *fname = gAllGlobals.gFileSystem.ref()->FileOpenString(work,true);
							FILE *fph = fopen(fname,"rb");
							if ( fph  )
							{
								fclose(fph);
							}
							else
							{
								fname = work;
								break;
							}
						}
					}
				}

				if ( fname )
				{
					gSaveFrame++;
					const char *_fname = gAllGlobals.gFileSystem.ref()->FileOpenString(fname,false);
					FILE *fph = fopen(_fname,"wb");
					if ( fph )
					{
						TuiElementMap::iterator i;
						for (i=mTui.begin(); i!=mTui.end(); ++i)
						{
							TuiElement *ti = (*i).second;
							ti->Save(fph);
						}
						fprintf(fph,"OdfLook %f %f %f %f %f %f\r\n", mEyePos[0], mEyePos[1], mEyePos[2], mLookAt[0], mLookAt[1], mLookAt[2] );
  						fclose(fph);
	  				}
				}
			}
			break;
		case TUI_PAGE:
			if ( count == 2 )
			{
				mPage = Locate( arglist[1] );
			}
			break;
		case TUI_BEGIN:
			Begin(TT_DEFAULT,count-1,&arglist[1]);
			break;
		case TUI_BUTTON_BEGIN:
			Begin(TT_BUTTON,count-1,&arglist[1]);
			break;
		case TUI_SCRIPT_BEGIN:
			Begin(TT_SCRIPT,count-1,&arglist[1]);
			break;
		case TUI_PAGE_BEGIN:
			Begin(TT_PAGE,count-1,&arglist[1]);
			break;
		case TUI_CHECKBOX_BEGIN:
			Begin(TT_CHECKBOX,count-1,&arglist[1]);
			break;
		case TUI_SLIDER_BEGIN:
			Begin(TT_SLIDER,count-1,&arglist[1]);
			break;
		case TUI_COMBO_BEGIN:
			Begin(TT_COMBO,count-1,&arglist[1]);
			break;
		case TUI_MULTI_SELECT_BEGIN:
			Begin(TT_MULTI_SELECT,count-1,&arglist[1]);
			break;
		case TUI_TEXT_BEGIN:
		{
			Begin( TT_TEXT, count-1, &arglist[1] );
		}
		break;
		case TUI_TEXT:
		{
			if ( mCurrent && ( count == 2 ) )
			{
				mCurrent->SetText( arglist[1] );
			}
		}
		break;
		case TUI_TEXT_COLOR:
		{
			if ( mCurrent )
			{
				unsigned color = 0xffffffff;
				if ( count == 4 )
				{
					color = (0xff << 24) + (atoi(arglist[1]) << 16) + (atoi(arglist[2]) << 8) + atoi(arglist[3]);
				}
				else if ( count == 5 )
				{
					color = (atoi(arglist[1]) << 24) + (atoi(arglist[2]) << 16) + (atoi(arglist[3]) << 8) + atoi(arglist[4]);
				}
				mCurrent->SetTextColor( color );
			}
		}
		break;

 		case TUI_CHECKBOX_ONCHECK_SHOW:
 		{
 			if ( mCurrent && ( mCurrent->GetType() == TT_CHECKBOX ) )
 			{
 				// k, user wants us to show the following when it is checked
 				mCurrent->AddOnCheckShow( count-1, &arglist[1] );
 			}
 		}
 		break;
 
 		case TUI_COMBO_SELECT_SHOW:
 		{
 			if ( mCurrent && ( mCurrent->GetType() == TT_COMBO ) )
 			{
 				// k, user wants us to show the following when it is selected
 				mCurrent->AddOnCheckShow( count-1, &arglist[1] );
 			}
 		}
 		break;
 
 		case TUI_COMBO_SELECT_RESCRIPT:
 		{
 			if ( mCurrent && ( mCurrent->GetType() == TT_COMBO ) )
 			{
 				// k, user wants us to show the following when it is selected
 				mCurrent->AddOnCheckRescript( count-1, &arglist[1] );
 			}
 		}
 		break;

		case TUI_END:
			End();
			break;
		case TUI_ARG:
			if ( mCurrent )
			{
				mCurrent->AddArg(count-1,&arglist[1]);
			}
			break;
		case TUI_EXECUTE_OK:
		case TUI_SAVE_OK:
			if ( mCurrent && count == 2)
			{
				if ( stricmp(arglist[1],"true") == 0 )
				{
					if ( token == TUI_EXECUTE_OK )
					  mCurrent->SetExecuteOk(true);
					else
					  mCurrent->SetSaveOk(true);
				}
				else
				{
					if ( token == TUI_EXECUTE_OK )
  						mCurrent->SetExecuteOk(false);
					else
  						mCurrent->SetSaveOk(false);
				}
			}
			break;
		case TUI_EXECUTE_ON_LOAD:
		{
			if ( mCurrent )
			{
				// k, it wants us to execute a command here
				mCurrent->AddExecuteOnLoad( &arglist[1], count-1 );
			}
		}
		break;
		case TUI_EXECUTE_ON_EXIT:
		{
			if ( mCurrent )
			{
				// k, it wants us to execute a command here
				mCurrent->AddExecuteOnExit( &arglist[1], count-1 );
			}
		}
		break;
		case TUI_RECIP:
			if ( mCurrent  )
			{
  				mCurrent->SetRecip(true);
			}
			break;
		case TUI_TECHNIQUES:
			if ( mCurrent && count == 2)
			{
				mCurrent->AddTechniques(arglist[1]);
			}
			break;
		case TUI_ACTORS:
			if ( mCurrent )
			{
				mCurrent->AddActors();
			}
			break;
		case TUI_FILES:
			if ( mCurrent )
			{
				mCurrent->AddFiles(count-1,&arglist[1]);
			}
			break;
		case TUI_CHOICE:
			if ( mCurrent )
			{
				mCurrent->AddChoice(count-1,&arglist[1]);
			}
			break;
		case TUI_CHOICES:
			if ( mCurrent )
			{
				mCurrent->AddChoices(count-1,&arglist[1]);
			}
			break;
		case TUI_SLIDER_RANGE:
			if ( count == 3 && mCurrent )
			{
				float fmin = (float) atof( arglist[1] );
				float fmax = (float) atof( arglist[2] );
				mCurrent->SetMinMax(fmin,fmax);
			}
			break;
		case TUI_SCRIPT:
			if ( mCurrent )
			{
				StringRef key = SGET("script");
				TuiElement *ts = new TuiElement(key,TT_SCRIPT,count-1,&arglist[1]);
				mTuiOthers.push_back(ts);
				mCurrent->Add(ts);
			}
			break;
		case TUI_COMMAND:
			if ( mCurrent )
			{
				mCurrent->SetTokens(count-1,&arglist[1]);
			}
			break;
		case TUI_DESCRIPTION:
			if ( mCurrent )
			{
				mCurrent->SetDescription(count-1,&arglist[1]);
			}
			break;
		case TUI_ELEMENT:
		case TUI_ELEMENT_PRIVATE:
			if ( mCurrent )
			{
				bool ok = true;

				if ( ok )
				{
  					for (int i=1; i<count; ++i)
  					{
  						StringRef key = SGET(arglist[i]);
  						TuiElement *tui = Locate(key);
  						if ( tui == 0 )
  						{
  							tui = new TuiElement(key,TT_DEFAULT,0,0);
  							mTui[key] = tui;
  						}
  						if ( tui )
  						{
  							mCurrent->Add(tui);
  						}
  					}
  				}
			}
			break;
		case TUI_NAME:
			if ( count == 2 && mCurrent )
			{
				mCurrent->SetName( arglist[1] );
			}
			break;

		case TUI_PHYSICS_MODE:
		{
			if ( mCurrent && ( count >= 2 ) && ( count % 2 ) )
			{
				// k, check if the string matches HW or SW
				for ( int i = 1; i < count; i += 2 )
				{
					if ( !stricmp( arglist[i], "SW" ) )
					{
						// SW
						if ( !stricmp( arglist[i+1], "false" ) ||
							 !stricmp( arglist[i+1], "0" ) )
						{
							mCurrent->SetSWEnabled( false );
						}
					}
					else if ( !stricmp( arglist[i], "HW" ) )
					{
						// HW
						if ( !stricmp( arglist[i+1], "false" ) ||
							 !stricmp( arglist[i+1], "0" ) )
						{
							mCurrent->SetHWEnabled( false );
						}
					}
				}
			}
		}
		break;

		case TUI_LIST:
			if ( 1 )
			{
				TuiElementMap::iterator i;

				for (i=mTui.begin(); i!=mTui.end(); ++i)
				{
					TuiElement *t = (*i).second;
					const char *name = t->GetKey().Get();
					const char *type = "unknown";

					switch ( t->GetType() )
					{
          				case TT_DEFAULT:            // default text user interface container
          					type = "default";
          					break;
          				case TT_PAGE:               // treated as a page
          					type = "page";
          					break;
          				case TT_BUTTON:
          					type = "button";
          					break;
          				case TT_SCRIPT:
          					type = "script";
          					break;
          				case TT_CHECKBOX:
          					type = "checkbox";
          					break;
          				case TT_SLIDER:
          					type = "slider";
          					break;
          				case TT_COMBO:
          					type = "combo";
          					break;
          				case TT_MULTI_SELECT:
          					type = "multiselect";
          					break;
					}

					gAllGlobals.gLog.ref()->Display("TUI=%s Type: %s\r\n", name, type );
				}
			}
			break;
	}

	return ret;
}

//==================================================================================
int TextUserInterface::CommandFallback(int count,const char **arglist)
{
	int ret = 0;

	if ( count )
	{
		const char *key = arglist[0];
		if ( key[0] == '/' )
		{
			gSaveFrame++;
			mSlashDepth++;

			if ( mSlashDepth <= 2 )
			{
  				ret = SlashCommand(key+1, count-1, &arglist[1] );
  			}

			mSlashDepth--;
		}
	}

	return ret;
}

//==================================================================================
void TextUserInterface::HelpAll(void)
{
	TuiElementMap::iterator i;
	for (i=mTui.begin(); i!=mTui.end(); ++i)
	{
		TuiElement *ti = (*i).second;
		if ( ti->GetType() == TT_PAGE )
		{
			ti->ShowHelp(0,false,0,false);
		}
	}
}

//==================================================================================
static bool IsDigit(const char *key,int &index)
{
	bool ret = true;

	const char *scan = key;

	while ( *scan )
	{
		if ( !isdigit(*scan) )
		{
			ret = false;
			break;
		}
		scan++;
	}
	if ( ret )
	{
		index = atoi(key);
	}
	return ret;
}

//==================================================================================
int TextUserInterface::SlashCommand(const char *key,int count,const char **arglist)
{
	int ret = 0;

	if ( stricmp(key,"help") == 0 || stricmp(key,"?") == 0 || stricmp(key,"h") == 0)
	{
		if ( count )
		{
			for (int i=0; i<count; ++i)
			{
				const char *str = arglist[i];
				if ( stricmp(str,"all") == 0 )
				{
					HelpAll();
				}
				else
				{
					TuiElementMap::iterator i;
					for (i=mTui.begin(); i!=mTui.end(); ++i)
					{
						TuiElement *ti = (*i).second;
						ti->ShowHelp(str);
					}

				}
			}
		}
		else
		{
			if ( mPage )
			{
				mPage->ShowHelp(0,false,0,false);
			}
			else
			{
				HelpAll();
			}
		}
	}
	else if ( stricmp(key,"addchoice") == 0 )
	{
		if ( count >= 2 )
		{
			TuiElement *te = Locate( arglist[0] );
			if ( te )
			{
				te->AddChoice(count-1, &arglist[1] );
				te->SetNeedsUpdating(true);
			}
		}
	}
	else if ( stricmp(key,"refresh") == 0 )
	{
		if ( count >= 1 )
		{
			for (int i=0; i<count; ++i)
			{
				const char *key = arglist[i];
				TuiElement *t = Locate( SGET(key) );
				if ( t )
				{
					t->Refresh();
					t->SetNeedsUpdating(true);
				}
			}
		}
		else
		{
  		gAllGlobals.gLog.ref()->Display("Refreshing file lists.\r\n");
	  	Refresh();
	  }
	}
	else if ( stricmp(key,"graphicsui") == 0 )
	{
		if ( count )
		{
			if ( stricmp(arglist[0],"true") == 0 )
			{
				SetShowGraphics(true);
			}
			else
			{
				SetShowGraphics(false);
			}
		}
		else
		{
			ToggleShowGraphics();
		}
	}
	else
	{
		TuiElement *texecute = 0;

		int index;
		if ( IsDigit(key,index) )
		{
			if ( index )
			{
				texecute = mPage->ExecuteElement(index-1, count, arglist, this );
			}
		}
		else
		{
			StringRef k = SGET(key);

			if ( mPage )
			{
				texecute = mPage->Command(k,count,arglist,this);
			}

			if ( !texecute ) // if not already consumed by the current page then go through all pages.
			{
				TuiElementMap::iterator i;
				for (i=mTui.begin(); i!=mTui.end(); ++i)
				{
					TuiElement *ti = (*i).second;
					texecute = ti->Command(k,count,arglist,this);
					if ( texecute ) break;
				}
			}
		}

		if ( texecute )
		{
			ret = 1; // we did process it
			if ( texecute->GetType() == TT_PAGE )
			{
				mPage = texecute;
				mPage->ShowHelp(0,false,0,false);
			}
		}
	}

	if ( 1 ) // propagate up to parent page!
	{
		TuiElementMap::iterator i;
		for (i=mTui.begin(); i!=mTui.end(); ++i)
		{
			TuiElement *t = (*i).second;
			if ( t->GetType() == TT_PAGE ) // test each page and see if any of it's children needs an update.
			{
				if ( t->ChildNeedsUpdating() ) // if the any child needs to be updated, then clear their update flag and set ours.
				{
					t->SetNeedsUpdating(true); // this will cause a 'redraw' to occur automagically
				}
			}
		}
	}

	return ret;
}

//==================================================================================
void TextUserInterface::Begin(TuiType type,int count,const char **arglist)
{
	End();

	if ( count )
	{
		StringRef key = SGET( arglist[0] );

		TuiElementMap::iterator found;
		found = mTui.find( key );

		if ( found != mTui.end() )
		{
			mCurrent = (*found).second;
			mCurrent->Reset();
			mCurrent->SetType(type);
		}
		else
		{
			if ( stricmp(key,"trampoline") == 0 )
			{
				printf("debug me");
			}
			mCurrent = new TuiElement(key,type,count-1,&arglist[1]);
			mTui[key] = mCurrent;
		}
	}
}

//==================================================================================
void TextUserInterface::End(void)
{
	if ( mCurrent )
	{
		mCurrent->End();
		mCurrent = 0;
	}
}

//==================================================================================
TuiElement * TextUserInterface::Locate(const StringRef &name) const // locate a text user interface element by name.
{
	TuiElement *ret = 0;

	TuiElementMap::const_iterator found;
	found = mTui.find(name);

	if ( found != mTui.end() ) 
		ret = (*found).second;

	return ret;
}

//==================================================================================
TuiElement::TuiElement(const StringRef &key,TuiType type,int count,const char **commands) :
	mHWEnabled( true ),
	mSWEnabled( true )
{
	mHasActors		= false;
	mInt			= false;
	mRecip			= false;
	mSaveFrame		= 0;
	mSaveOk			= true;
	mKey			= key;
	mName			= key;
	mUserPtr		= 0;
	mUserPtr2		= 0;
	mType			= type;
	mTextColor		= 0xffffffff;
 	mNeedsUpdating	= false;
 	mVisible		= true;

	for (int i=0; i<count; ++i)
	{
		const char *cmd = commands[i];
		assert( strlen(cmd) );
		StringRef ref = SGET( cmd );
		mCommands.push_back(ref);
	}
}

//==================================================================================
TuiElement::~TuiElement(void)
{
	Reset();
}

//==================================================================================
TuiType TuiElement::GetType(void) const
{
	return mType;
};

//==================================================================================
void TuiElement::SetType(TuiType type)
{
	mType = type;
};

//==================================================================================
void TuiElement::SetTokens(int count,const char **args)
{
	for (int i=0; i<count; ++i)
	{
		StringRef cmd = SGET( args[i] );
		mTokens.push_back(cmd);
	}
}

//==================================================================================
void TuiElement::SetDescription(int count,const char **args)
{
	for (int i=0; i<count; ++i)
	{
		StringRef cmd = SGET( args[i] );
		mDescription.push_back(cmd);
	}
}

//==================================================================================
void TuiElement::SetText( const char *text )
{
	if ( text )
	{
		mText = text;
	}
}

//==================================================================================
void TuiElement::SetTextColor( unsigned int color )
{
	mTextColor = color;
}

//==================================================================================
void TuiElement::AddOnCheckShow( int count, const char **name )
{
 	if ( name )
 	{
 		TuiType type = GetType();
 
 		if ( type == TT_CHECKBOX )
 		{
 			StringRefVector *p = 0;
 			if ( !mOnCheckShow.size() )
 			{
 				p = new StringRefVector;
 			}
 
 			if ( p )
 			{
 				for ( int i = 0; i < count; ++i )
 				{
 					StringRef ref = SGET( name[i] );
 					p->push_back( ref );
 				}
 				mOnCheckShow.push_back( p );
 			}
 		}
 		else if ( type == TT_COMBO )
 		{
 			StringRefVector *p = new StringRefVector;
 
 			if ( p )
 			{
 				for ( int i = 0; i < count; ++i )
 				{
 					StringRef ref = SGET( name[i] );
 					p->push_back( ref );
 				}
 				mOnCheckShow.push_back( p );
 			}
 		}
 	}
}
 
//==================================================================================
void TuiElement::AddOnCheckRescript( int count, const char **script )
{
	if ( script )
 	{
 		TuiType type = GetType();
 
 		if ( type == TT_COMBO )
 		{
 			StringRefVector *p = new StringRefVector;
 
 			if ( p )
 			{
 				for ( int i = 0; i < count; ++i )
 				{
 					StringRef ref = SGET( script[i] );
 					p->push_back( ref );
 				}
 				mOnCheckRescript.push_back( p );
 			}
 		}
 	}
 }

//==================================================================================
void TuiElement::Add(TuiElement *tui)
{
	mElements.push_back(tui);
}

//==================================================================================
void TuiElement::SetName(const StringRef &name)
{
	mName = name;
};

//==================================================================================
void TuiElement::SetKey(const StringRef &key)
{
	mKey     = key;
};

//==================================================================================
void TuiElement::Reset(void)
{
	mFileSpecs.clear();
	mArgs.clear();
	mChoices.clear();
	mDescription.clear();
	mTokens.clear();
	mCommands.clear();
	mElements.clear();

 	StringRefVectorVct::iterator i;
 	for ( i = mOnCheckShow.begin(); i != mOnCheckShow.end(); ++i )
 	{
 		delete (*i);
 	}
 	for ( i = mOnCheckRescript.begin(); i != mOnCheckRescript.end(); ++i )
 	{
 		delete (*i);
 	}
 	mOnCheckShow.clear();
 	mOnCheckRescript.clear();
}

//==================================================================================
void TuiElement::End(void)
{
  if ( mFileSpecs.size() )
  {
    Refresh(); // refresh the choices based on the file spec passed.
  }
}

//==================================================================================
void TuiElement::Indent(int indent)
{
	for (int j=0; j<indent; ++j)
	{
		gAllGlobals.gLog.ref()->Display("  ");
	}
}

//==================================================================================
void TuiElement::ShowHelp(int indent,bool detailed,int index,bool showhelp)
{
	if ( 1 )
	{
		Indent(indent);

		StringRef command = mKey;

		if ( mTokens.size() )
			command = mTokens[0];

		if ( showhelp )
		{
			gAllGlobals.gLog.ref()->Display("[%2d] ", index+1);
		}

		gAllGlobals.gLog.ref()->Display("/%-20s (%s)\r\n", command.Get(), mName.Get() );

		if ( detailed && mTokens.size() > 1 )
		{
			StringRefVector::iterator i;
			bool first = true;
			for (i=mTokens.begin(); i!=mTokens.end(); ++i)
			{
				if ( first )
				{
					first = false;
				}
				else
				{
					Indent(indent);
					gAllGlobals.gLog.ref()->Display("/%-20s (%s)\r\n", (*i).Get(), mName.Get() );
				}
			}
		}

		if ( detailed & !mDescription.empty() )
		{
			gAllGlobals.gLog.ref()->Display("\r\n");
			StringRefVector::iterator i;
			for (i=mDescription.begin(); i!=mDescription.end(); ++i)
			{
				Indent(indent+1);
				gAllGlobals.gLog.ref()->Display("%s\r\n", (*i).Get() );
			}
			gAllGlobals.gLog.ref()->Display("\r\n");
		}

		if ( detailed && !mChoices.empty() )
		{
			gAllGlobals.gLog.ref()->Display("\r\n");
			TuiChoiceVector::iterator i;
			int index = 0;
			for (i=mChoices.begin(); i!=mChoices.end(); ++i)
			{
				index++;
				Indent(indent+2);
				gAllGlobals.gLog.ref()->Display("[%2d] : %s\r\n", index, (*i).GetChoice().Get() );
			}
			gAllGlobals.gLog.ref()->Display("\r\n");
		}
	}

	if ( indent == 0 && !detailed )
	{
		bool showcount = false;
		if ( mType == TT_PAGE ) 
			showcount = true;

		int index = 0;
		TuiElementVector::iterator i;
		for (i=mElements.begin(); i!=mElements.end(); ++i)
		{
			TuiElement *te = (*i);
			te->ShowHelp(indent+1,false,index,showcount);
			index++;
		}
	}
}

//==================================================================================
bool TuiElement::ShowHelp(const char *str)
{
	bool ret = false;

	if ( !mTokens.empty() )
	{
		StringRefVector::iterator i;
		for (i=mTokens.begin(); i!=mTokens.end(); ++i)
		{
			if ( stricmp(str,(*i).Get() ) == 0 )
			{
				ShowHelp(1,true,0,false);
				ret = true;
				break;
			}
		}
	}
	return ret;
}

//==================================================================================
TuiElement * TuiElement::Command(const StringRef &c,int count,const char **arglist,TextUserInterface *tui)
{
	TuiElement *ret = 0;

	bool execute = false;

	if ( stricmp(c,mKey) == 0 )
		execute = true;
	else
	{
	  StringRefVector::iterator i;
	  for (i=mTokens.begin(); i!=mTokens.end(); ++i)
	  {
		  if ( stricmp(c,(*i)) == 0 )
		  {
				execute = true;
				break;
			}
		}
	}
	if ( execute )
	{
		ret = this;
		if ( GetType() != TT_PAGE )
		{
			Execute(count,arglist,this,tui);
		}
		else
		{
			if ( count )
			{
				int index;
				if (  IsDigit(arglist[0],index) )
				{
					if ( index )
					{
						ret = ExecuteElement(index-1, count-1, &arglist[1], tui );
					}
				}
			}
		}
	}

	return ret;
}

//==================================================================================
bool TuiElement::NeedSpace(const char *foo)
{
	bool ret = false;
	if ( strlen(foo) > 1 )
	{
		while ( *foo )
		{
			char c = *foo++;
			if ( c == 32 || c == ',' || c == '(' || c == ')' )
			{
				ret = true;
				break;
			}
		}
	}
	return ret;
}

//==================================================================================
const char * TuiElement::GetCommand(const StringRef &c,int count,const char **arglist,TuiElement *parent,TextUserInterface *tui)
{
	static char scratch[2048];

	if ( NeedSpace(c) )
	{
		sprintf(scratch,"%c%s%c", 34, c.Get(), 34 );
	}
	else
	{

		const char *foo = c.Get();
		char cp = *foo;

		if ( cp == '%' || cp == '!' )
		{
			if ( foo[1] >= '1' && foo[1] <= '9' )
			{
				int c = foo[1] - '1';
				if ( c >= 0 && c < count )
				{
					foo = arglist[c];
					parent->SetArg(c,foo);
				}
				else
				{
					foo = parent->GetToggleArg(c);
				}
			}
			else
			{
				foo = tui->ArgumentLookup( &foo[1] );
			}

			if ( cp == '!' )
			{
				if ( stricmp(foo,"true") == 0 )
					foo = "false";
				else if ( stricmp(foo,"false") == 0 )
					foo = "true";
			}

		}
		if(strchr(foo, ' '))
		{
			sprintf(scratch, "\"%s\"", foo);
		}
		else
		{
			strcpy(scratch, foo );
		}
	}

	return scratch;
}

//==================================================================================
void TuiElement::Execute(int count,const char **arglist,TuiElement *parent,TextUserInterface *tui)
{
	if ( mSaveFrame == gSaveFrame )
	{
		return; // already executed on this frame!
	}
	else
	{
		mSaveFrame = gSaveFrame;
	}

	if ( 1 )
	{
		for (int i=0; i<count; ++i)
		{
			if ( i < (int)mArgs.size() )
			{
				mArgs[i] = arglist[i];
			}
		}
	}

	if ( !mCommands.empty() && !tui->IsTuiLoad() ) // don't execute command if loading the tui file!
	{
		bool ok = true;
		if ( tui->IsTuiLoad() ) // ok..we are reloading a save file..
		{
			ok = mExecuteOk;
		}

		if ( ok )
		{
			char scratch[2048];
			scratch[0] = 0;

			StringRefVector::iterator i;
			for (i=mCommands.begin(); i!=mCommands.end(); ++i)
			{
				strcat( scratch, GetCommand( (*i), count, arglist, parent, tui) );
				strcat( scratch, "  " );
			}

			CPARSER.Parse(scratch);
			//gAllGlobals.gLog.ref()->Display("TUI COMMAND=> %s\r\n", scratch );
		}
	}

	if ( 1 )
	{
		TuiElementVector::iterator i;
		for (i=mElements.begin(); i!=mElements.end(); ++i)
		{
			TuiElement *ti = (*i);
			if ( ti->GetType() != TT_PAGE )
			{
				ti->Execute(count,arglist,this,tui);
			}
		}
	}
}

//==================================================================================
TuiElementVector& TuiElement::GetElements(void)
{
	return mElements;
};

//==================================================================================
void * TuiElement::GetUserPtr(void)
{
	return mUserPtr;
};

//==================================================================================
void * TuiElement::GetUserPtr2(void)
{
	return mUserPtr2;
};

//==================================================================================
void TuiElement::SetUserPtr(void *ptr)
{
	mUserPtr = ptr;
};

//==================================================================================
void TuiElement::SetUserPtr2(void *ptr)
{
	mUserPtr2 = ptr;
};

//==================================================================================
const StringRef& TuiElement::GetKey(void) const
{
	return mKey;
};

//==================================================================================
const StringRef& TuiElement::GetName(void) const
{
	return mName;
};

//==================================================================================
TuiElement * TuiElement::ExecuteElement(int index,int count,const char **arglist,TextUserInterface *tui)
{
	TuiElement *ret = 0;
	if ( index >= 0 && index < (int)mElements.size() )
	{
		ret = mElements[index];

		if ( ret->GetType() != TT_PAGE )
		{
			gSaveFrame++;
			ret->Execute(count,arglist,this,tui);
		}
	}
	return ret;
}

//==================================================================================
void  TuiElement::AddArg(int count,const char **arglist)
{
	if ( !mArgs.empty() && !mFileSpecs.empty() )
	{
//		gAllGlobals.gLog.ref()->Display("TUIELEMENT: %s Skipping Arg: %s because it already is in the file spec.\r\n", mName.Get(), arglist[0] );
	}
	else
	{
  	for (int i=0; i<count; ++i)
  	{
  		StringRef ref = SGET(arglist[i]);
  		mArgs.push_back(ref);
  	}
  }
}

//==================================================================================
void TuiElement::AddTechniques(const char *str)
{
}

//==================================================================================
void TuiElement::AddActors(void)
{
}

//==================================================================================
void TuiElement::AddFiles(int count,const char **arglist)
{
	for (int i=0; i<count; ++i)
	{
		const char *spec = arglist[i];
		mFileSpecs.push_back( SGET(spec) );
	}
}

//==================================================================================
void  TuiElement::AddChoice(int count,const char **arglist)
{
	if ( count )
	{
		bool state    = false;
		StringRef ref = SGET(arglist[0]);
		StringRef arg = ref;

		if ( count >= 2 )
		{
			arg = SGET(arglist[1]);
			if ( count >= 3 )
			{
				if ( stricmp(arglist[2],"true") == 0 )
					state = true;
				else
					state = false;
			}
		}
		TuiChoice c(ref,arg,state);

		mChoices.push_back(c);
		mOriginalChoices.push_back(c);
	}
}

//==================================================================================
void  TuiElement::AddChoices(int count,const char **arglist)
{
	for (int i=0; i<count; ++i)
	{
		StringRef ref = SGET(arglist[i]);
		TuiChoice c(ref,ref,true);
		mChoices.push_back(c);
		mOriginalChoices.push_back(c);
	}
}

//==================================================================================
TuiElement * TuiElement::GetElement(int index)
{
	TuiElement *ret = 0;
	if ( ( index >= 0 ) && ( index < (int)mElements.size() ) )
	{
		ret = mElements[index];
	}
	return ret;
}

//==================================================================================
bool TuiElement::GetState(void)
{
	bool state = false;
	if ( mArgs.size() && ( stricmp(mArgs[0],"true") == 0 ) )
	{
		state = true;
	}
	return state;
}

//==================================================================================
const char * TextUserInterface::ArgumentLookup(const char *name)
{
	const char *foo = 0;

	if ( mPage )
	{
		foo = mPage->ArgumentLookup(name);
	}

	if ( !foo ) // if not already consumed by the current page then go through all pages.
	{
		TuiElementMap::iterator i;
		for (i=mTui.begin(); i!=mTui.end(); ++i)
		{
			TuiElement *ti = (*i).second;
			foo = ti->ArgumentLookup(name);
			if ( foo )
				break;
		}
	}

	if ( foo == 0 ) 
		foo = name;

	return foo;
}

//==================================================================================
// look up an argument by name..
//==================================================================================
const char * TuiElement::ArgumentLookup(const char *name)
{
	const char *ret = 0;

	static char rval[64]; // hold's reciprocol return value!

	if ( stricmp(name,mKey) == 0 )
	{
		if ( mArgs.size() )
		{
			if ( mArgs.size() == 1 )
			{
  				ret = mArgs[0].Get();
				if ( mRecip )
				{
					float v = (float) atof(ret);
					sprintf(rval,"%0.9f", 1.0f / v );
					ret = rval;
				}
			}
  			else
  			{
  				char scratch[512];
  				strcpy(scratch,mArgs[0].Get());
  				for (unsigned int i=1; i<mArgs.size(); ++i)
  				{
  					strcat(scratch," ");
  					strcat(scratch,mArgs[i].Get());
  				}
  				StringRef ref = SGET(scratch);
  				ret = ref.Get();
  			}
		}
	}

	if ( ret == 0 )
	{
		StringRefVector::iterator i;
		for (i=mTokens.begin(); i!=mTokens.end(); ++i)
		{
			const char *token = (*i).Get();
			if ( stricmp(name,token) == 0 )
			{
				if ( mArgs.size() )
				{
    				if ( mArgs.size() == 1 )
					{
      					ret = mArgs[0].Get();

						if ( mRecip )
						{
							float v = (float) atof(ret);
							sprintf(rval,"%0.9f", 1.0f / v );
							ret = rval;
						}
					}
      				else
      				{
      					char scratch[512];
      					strcpy(scratch,mArgs[0].Get());
      					for (unsigned int i=1; i<mArgs.size(); ++i)
      					{
      						strcat(scratch," ");
      						strcat(scratch,mArgs[i].Get());
      					}
      					StringRef ref = SGET(scratch);
      					ret = ref.Get();
		      		}
				}
				break;
			}
		}
	}

	return ret;
}

//==================================================================================
// locate this argument, toggle the result and return.
//==================================================================================
const char *      TuiElement::GetToggleArg(int count) 
{
	const char * ret = "false";

	if ( count >= 0 && count < (int)mArgs.size() )
	{
		ret = mArgs[count].Get();
		if ( stricmp(ret,"true") == 0 )
		{
			StringRef ref = SGET("false");
			mArgs[count] = ref;
			ret = ref.Get();
		}
		else if ( stricmp(ret,"false") == 0 )
		{
			StringRef ref = SGET("true");
			mArgs[count] = ref;
			ret = ref.Get();
		}
	}
	return ret;
}

//==================================================================================
bool TuiElement::GetArg(int count, char *arg) const
{
	bool ok = false;

	if ( arg )
	{
		// default val
		sprintf( arg, "0" );

		if ( ( count >= 0 ) && ( count < (int)mArgs.size() ) )
		{
			ok = true;
			if ( mType == TT_SLIDER )
			{
				if ( mInt )
				{
					// convert our value to an integer
					int val = atoi( mArgs[count].Get() );
					sprintf( arg, "%d", val );
				}
				else
				{
					sprintf( arg, mArgs[count].Get() );
				}
			}
			else
			{
				sprintf( arg, mArgs[count].Get() );
			}
		}
	}
	return ok;
}

//==================================================================================
bool TuiElement::GetArg_Actual(int count, char *arg) const
{
	bool ok = false;

	if ( arg )
	{
		// default val
		sprintf( arg, "0" );

		if ( ( count >= 0 ) && ( count < (int)mArgs.size() ) )
		{
			ok = true;
			sprintf( arg, mArgs[count].Get() );
		}
	}
	return ok;
}

//==================================================================================
void TuiElement::SetArg(int count,const char *c)
{
	if ( ( count >= 0 ) && ( count < (int)mArgs.size() ) )
	{
		mArgs[count] = SGET(c);
	}
}

//==================================================================================
// convert into a normalized tick value 0-1000
//==================================================================================
int  TuiElement::GetSliderTick(float v) const 
{
	float tmpV = v;

	if ( tmpV < mMin ) 
		tmpV = mMin;
	if ( tmpV > mMax ) 
		tmpV = mMax;

	tmpV -= mMin;

	float recip = 1.0f / (mMax - mMin );

	float modifiedV = 1000*recip*tmpV;

	int intVal = (int)modifiedV;

	//if ( !mInt )
	{
		modifiedV -= intVal;
		if ( modifiedV >= 0.50f )
		{
			++intVal;
		}
		else
		{
			--intVal;
		}
	}

	return intVal;
}


//==================================================================================
// convert integer tick (0-1000) into floating point value.
//==================================================================================
float TuiElement::GetSliderValue(int tick) const
{
	float scale = (float) tick / 1000.0f;

	float v = (scale*(mMax-mMin))+mMin;
	return v;
}

//==================================================================================
void TuiElement::Save(FILE *fph)
{
	if ( mSaveOk )
	{
  		const char *command = mKey.Get();
  		if ( mArgs.size() ) // if we have an argument
  		{
  			if ( mTokens.size() )
  			{
  				command = mTokens[0].Get();
  			}
  			fprintf(fph,"/%s ", command );
  			for (unsigned int i=0; i<mArgs.size(); ++i)
  			{
  				fprintf(fph,"%s ", mArgs[i].Get() );
  			}
  			fprintf(fph,"\r\n");
		}
	}
}

//==================================================================================
// ok..any set of menus which has a file spec will get refreshed to see if any new items are available
//==================================================================================
void TextUserInterface::Refresh(void)
{
	TuiElementMap::iterator i;
	for (i=mTui.begin(); i!=mTui.end(); ++i)
	{
		TuiElement *t = (*i).second;
		t->Refresh();
	}
}

//==================================================================================
void TextUserInterface::NotifyAllPagesOfNewDimensions( unsigned width, unsigned height )
{
	// k, go through our map and update the items with the new width, height
	TuiElementMap::iterator i;
	for (i=mTui.begin(); i!=mTui.end(); ++i)
	{
		TuiElement *t = (*i).second;
		if ( t )
		{
			PageLayout &layout = t->getPageLayout();
			layout.Modify( width, height );
			t->SetNeedsUpdating( true );
		}
	}
}

//==================================================================================
TuiElement *TextUserInterface::GetElementOfTypeWithName( TuiType type, const char *name )
{
	TuiElement *item = 0;

	TuiElementMap::iterator i;
	for (i=mTui.begin(); !item && (i!=mTui.end()); ++i)
	{
		TuiElement *t = (*i).second;
		if ( t && (t->GetType() == type) && !stricmp( name, t->GetName().Get() ) )
		{
			item = t;
		}
	}

	return item;
}

//==================================================================================
void TuiElement::Refresh(void)
{
	if ( mHasActors )
	{
  		AddActors(); // repopulate the list!
		if ( mChoices.size() )
		{
			mArgs.clear();
			mArgs.push_back( mChoices[0].GetChoice() );
		}
		return;
	}

	if ( !mFileSpecs.empty() )
	{
		SetNeedsUpdating(true);
		mChoices.clear();
    
		// add all of the files!
		StringRefVector files;
		StringRefVector::const_iterator i;
		for (i=mFileSpecs.begin(); i!=mFileSpecs.end(); ++i)
		{
			const StringRef &ref = (*i);
//  			FileFind ff(ref.Get());
//  			ff.GetFiles(files);
		}

		if ( 1 ) // copy the original choices to the head of the list
		{
			TuiChoiceVector::iterator i;
			for (i=mOriginalChoices.begin(); i!=mOriginalChoices.end(); ++i)
			{
				TuiChoice &tc = (*i);
				mChoices.push_back(tc);
			}
		}

		std::sort(files.begin(), files.end(), StringSortRef() );

		if ( 1 )
		{
  			StringRefVector::iterator i;
  			for (i=files.begin(); i!=files.end(); ++i)
  			{
  				const StringRef &ref = (*i);
  				TuiChoice tc(ref,ref,true);
  				mChoices.push_back(tc);
  			}
  		}
	  
		if ( mChoices.size() )
		{
			mArgs.clear();
			mArgs.push_back( mChoices[0].GetChoice() );
		}
	}
}

//==================================================================================
void TuiElement::OnCheckboxChange( bool state, int id )
{
 	DetermineVisibleElements( state ? sTrue.Get() : sFalse.Get(), id );
}

//==================================================================================
void TuiElement::OnComboBoxChange( const char *selectedItem, int id )
{
 	DetermineVisibleElements( selectedItem, id );
 	DetermineRescriptedItems( selectedItem, id );
}

//==================================================================================
void TuiElement::DetermineVisibleElements( const char *selectedItem, int id )
{
 	// k, find all checkboxes first and hide/show appropriate items
 	if ( GetType() == TT_PAGE )
 	{
 		TuiElement *selectedControl = GetElement( id );
 
 		TuiElementVector::iterator i;
 		for ( i = mElements.begin(); i != mElements.end(); ++i )
 		{
 			TuiElement *elementPtr = selectedControl ? selectedControl : (*i);
 
 			if ( elementPtr )
 			{
 				TuiType type = elementPtr->GetType();
 
 				if ( type == TT_CHECKBOX )
 				{
 					bool state = true;
 					if ( selectedControl && selectedItem )
 					{
 						state = !stricmp( sTrue.Get(), selectedItem );
 					}
 					else
 					{
 						char arg[128];
						elementPtr->GetArg( 0, arg );
 						if ( ( !stricmp( arg, sFalse.Get() ) || !stricmp( arg, sZero.Get() ) ) )
 						{
 							state = false;
 						}
 					}
 
 					const StringRefVectorVct &showOnClick = elementPtr->GetOnCheckShow();
 					if ( showOnClick.size() == 1 )
 					{
 						TuiElementVector::iterator j;
 						StringRefVector::const_iterator k;
 						TuiElementVector& elements = GetElements();
 
 						// check for items we should show/hide
 						for ( k = showOnClick[0]->begin(); k != showOnClick[0]->end(); ++k )
 						{
 							const StringRef &item = (*k);
 							for ( j = elements.begin(); j != elements.end(); ++j )
 							{
 								TuiElement *element = (*j);
 								// k, if state is true (meaning it is checked) and this item is hidden, then
 								// we need to show it...OR if state is false and this item is displaying, then
 								// we need to hide it!
 								if ( element && ( element->GetKey() == item ) )
 								{
 									if ( element->IsVisible() != state )
 									{
 										element->SetVisible( state );
 
 										if ( selectedControl )
 											SetNeedsUpdating( true );
 									}
 									break;
 								}
 							}
 						}
 					}
 				}
 				else if ( type == TT_COMBO )
 				{
 					// k, get items it is to show when chosen
 					const StringRefVectorVct &showOnClick = elementPtr->GetOnCheckShow();
 					if ( showOnClick.size() )
 					{
 						// first, determine which item is chosen in the combo box
 						char arg[128];
						elementPtr->GetArg( 0, arg );
 						StringRefVectorVct::const_iterator z;
 						StringRefVector *srvPntr = 0;
 						if ( arg )
 						{
 							for ( z = showOnClick.begin(); !srvPntr && (z != showOnClick.end()); ++z )
 							{
 								StringRefVector *p = (*z);
 								if ( p && p->size() )
 								{
 									StringRefVector::const_iterator i = p->begin();
 									const StringRef &name = (*i);
 									if ( !stricmp( arg, name.Get() ) )
 									{
 										srvPntr = p;
 									}
 								}
 							}
 						}
 
 						// k, do we know which item user has chosen as the "first" item?
 						if ( srvPntr )
 						{
 							TuiElementVector::iterator j;
 							TuiElementVector& elements = GetElements();
 
 							// check for items we should show/hide
 							bool state = true;
 
 							StringRefVector::const_iterator i = srvPntr->begin();
 							for ( ; i != srvPntr->end(); ++i )
 							{
 								const StringRef &item = (*i);

 								if ( i == srvPntr->begin() )
 									continue;
 								else if ( !stricmp( sShow.Get(), item.Get() ) )
 								{
 									state = true;
 									continue;
 								}
 								else if ( !stricmp( sHide.Get(), item.Get() ) )
 								{
 									state = false;
 									continue;
 								}
 
 								for ( j = elements.begin(); j != elements.end(); ++j )
 								{
 									TuiElement *element = (*j);
 
 									if ( element && ( element->GetKey() == item ) )
 									{
 										if ( element->IsVisible() != state )
 										{
 											element->SetVisible( state );
 
 											if ( selectedControl )
 												SetNeedsUpdating( true );
 										}
 										break;
 									}
 								}
 							}
 						}
 					}
 				}
 			}
 
 			if ( selectedControl )
 				break;
 		}
 	}
}
 
//==================================================================================
void TuiElement::DetermineRescriptedItems( const char *selectedItem, int id )
{
 	// k, find all checkboxes first and hide/show appropriate items
 	if ( GetType() == TT_PAGE )
 	{
 		TuiElement *selectedControl = GetElement( id );
 
 		TuiElementVector::iterator i;
 		for ( i = mElements.begin(); i != mElements.end(); ++i )
 		{
 			TuiElement *elementPtr = selectedControl ? selectedControl : (*i);
 
 			if ( elementPtr )
 			{
 				TuiType type = elementPtr->GetType();
 
 				if ( type == TT_COMBO )
 				{
 					// k, get items it is to rescript when chosen
 					const StringRefVectorVct &showOnClick = elementPtr->GetOnCheckRescript();
 					if ( showOnClick.size() )
 					{
 						// first, determine which item is chosen in the combo box
 						char arg[128];
						elementPtr->GetArg( 0, arg );
 						StringRefVectorVct::const_iterator z;
 						StringRefVector *srvPntr = 0;
 						if ( arg )
 						{
 							for ( z = showOnClick.begin(); !srvPntr && (z != showOnClick.end()); ++z )
 							{
 								StringRefVector *p = (*z);
 								if ( p && p->size() )
 								{
 									StringRefVector::const_iterator i = p->begin();
 									const StringRef &name = (*i);
 									if ( !stricmp( arg, name.Get() ) )
 									{
 										srvPntr = p;
 									}
 								}
 							}
 						}
 
 						// k, do we know which item user has chosen as the "first" item?
 						if ( srvPntr )
 						{
 							TuiElementVector::iterator j;
 							TuiElementVector& elements = GetElements();
 
 							// check for items we should rescript and rescript them
 							bool state = true;
 
 							StringRefVector::const_iterator i = srvPntr->begin();
 							unsigned cnt = 0;
 							unsigned size = (unsigned)srvPntr->size();
 							for ( ; i != srvPntr->end(); ++i, ++cnt )
 							{
 								const StringRef &item = (*i);
 
 								if ( cnt == 0 )
 								{
 									continue;
 								}
 								else if ( !stricmp( item, "TuiName" ) && (cnt+2 < size) )
 								{
 									StringRef elementName, text;
 									++i;
 									++cnt;
 									elementName = (*i);
 									++i;
 									++cnt;
 									text = (*i);
 
 									for ( j = elements.begin(); j != elements.end(); ++j )
 									{
 										TuiElement *element = (*j);
 
 										if ( element && ( element->GetKey() == elementName ) )
										{
 											element->SetName( text );
 
 											if ( selectedControl )
 												SetNeedsUpdating( true );
 											break;
 										}
 									}
 								}
 								else
 								{
 									// something is wrong - bail
 									assert( 0 );
 									break;
 								}
 							}
 						}
 					}
 				}
 			}
 
 			if ( selectedControl )
 				break;
 		}
 	}
}

//==================================================================================
void TuiElement::AddExecuteOnLoad( const char **args, int count )
{
	char buff[512];
	for ( int i = 0; i < count; ++i )
	{
		if ( i == 0 )
		{
			sprintf( buff, args[i] );
		}
		else
		{
			strcat( buff, " " );
			strcat( buff, args[i] );
		}
	}
	StringRef command = SGET( buff );
	mOnLoadCommands.push_back( command );
}

//==================================================================================
void TuiElement::AddExecuteOnExit( const char **args, int count )
{
	char buff[512];
	for ( int i = 0; i < count; ++i )
	{
		if ( i == 0 )
		{
			sprintf( buff, args[i] );
		}
		else
		{
			strcat( buff, " " );
			strcat( buff, args[i] );
		}
	}
	StringRef command = SGET( buff );
	mOnExitCommands.push_back( command );
}

//==================================================================================
void TuiElement::OnLoad( TuiElement *loadingPage )
{
	// k, we are loading, so tell any of our elements to execute any "on load" scripts
	// that they may have
 	if ( ( GetType() == TT_PAGE ) && ( this == loadingPage ) )
 	{
		// k, go through any elements, and if they want to do anything when the page loads,
		// then we do that
 		TuiElementVector::iterator i;
 		for ( i = mElements.begin(); i != mElements.end(); ++i )
 		{
 			TuiElement *elementPtr = (*i);
			if ( elementPtr && (elementPtr != this) )
			{
				elementPtr->OnLoad( 0 );
			}
		}
	}
	else if ( !loadingPage )
	{
		// we are part of the loading page, so we execute any "on load" scripts
		for ( StringRefVector::iterator i = mOnLoadCommands.begin(); i != mOnLoadCommands.end(); ++i )
		{
			StringRef &command = (*i);
			char buff[512];
			GetOnLoadOnExitCommand( buff, 512, command );
			CPARSER.Parse( buff );		
		}
	}
}

//==================================================================================
void TuiElement::OnExit( TuiElement *exitingPage )
{
	// k, we are exiting, so tell any of our elements to execute any "on exit" scripts
	// that they may have
 	if ( ( GetType() == TT_PAGE ) && ( this == exitingPage ) )
 	{
		// k, go through any elements, and if they want to do anything when the page exits,
		// then we do that
 		TuiElementVector::iterator i;
 		for ( i = mElements.begin(); i != mElements.end(); ++i )
 		{
 			TuiElement *elementPtr = (*i);
			if ( elementPtr && (elementPtr != this) )
			{
				elementPtr->OnExit( 0 );
			}
		}
	}
	else if ( !exitingPage )
	{
		// we are part of the exiting page, so execute any "on exit" scripts
		for ( StringRefVector::iterator i = mOnExitCommands.begin(); i != mOnExitCommands.end(); ++i )
		{
			StringRef &command = (*i);
			char buff[512];
			GetOnLoadOnExitCommand( buff, 512, command );
			CPARSER.Parse( buff );		
		}
	}
}

//==================================================================================
int TuiElement::GetElementIndexOfTypeWithName( TuiType type, const char *name )
{
	int index = -1;
	if ( name )
	{
		int cnt = 0;
		TuiElementVector::iterator i;
		for ( i = mElements.begin(); (index == -1) && (i != mElements.end()); ++i, ++cnt )
		{
			TuiElement *elementPtr = (*i);
			if ( elementPtr && !stricmp( name, elementPtr->GetName().Get() ) )
			{
				index = cnt;
			}
		}	
	}

	return index;
}

//==================================================================================
TuiElement * TuiElement::GetElementOfTypeWithName( TuiType type, const char *name )
{
	TuiElement *ret = 0;
	if ( name )
	{
		int cnt = 0;
		TuiElementVector::iterator i;
		for ( i = mElements.begin(); !ret && (i != mElements.end()); ++i, ++cnt )
		{
			TuiElement *elementPtr = (*i);
			if ( elementPtr && !stricmp( name, elementPtr->GetName().Get() ) )
			{
				ret = elementPtr;
			}
		}	
	}

	return ret;
}

//==================================================================================
void TuiElement::GetOnLoadOnExitCommand( char *buff, int buffLen, const StringRef &command )
{
	if ( buff )
	{
		char tmp[512];
		strcpy( tmp, command.Get() );
		char *pct = strstr( tmp, "%" );
		if ( pct )
		{
			memset( buff, 0, sizeof(char)*buffLen );

			// gotta get args
			bool done = false;
			int i = 0, j = 0;
			int len = (int)strlen( tmp );
			while ( !done )
			{
				if ( i >= len )
					done = true;
				else if ( tmp[i] == '%' )
				{
					++i;
					unsigned k = 0;
					char which[10];
					while ( ( tmp[i] != ' ' ) && ( i < len ) )
					{
						which[k++] = tmp[i++];
					}
					which[k] = '\0';
					k = atoi( which );
					if ( mArgs.size() > k-1 )
					{
						char val[64];
						sprintf( val, "%s", mArgs[k-1] );
						strcat(  &buff[j], val );
						j += (int)strlen( val );
					}
				}
				else
					buff[j++] = tmp[i++];
			}
		}
		else
		{
			sprintf( buff, tmp );
		}
	}
}

//==================================================================================
void TextUserInterface::NotifyHardwareStatus( bool hwOn )
{
	TuiElementMap::iterator i;
	for (i=mTui.begin(); i!=mTui.end(); ++i)
	{
		TuiElement *t = (*i).second;
		if ( t && ( t->GetType() == TT_PAGE ) )
		{
			if ( t->NotifyHardwareStatus( hwOn ) )
			{
				t->SetNeedsUpdating( true );
			}
		}
	}

	// k, now if the current page is NOT HW enabled, then we move up until we find one that is
	if ( hwOn && mPage && !mPage->GetHWEnabled() )
	{
		TuiElement *previous	= mPage;
		TuiElement *current		= mPage;

		// k, simply go back to top
		bool found = false;
		while ( current && !found )
		{
			current = current->GetPreviousPage();
			if ( current && current->GetHWEnabled() )
			{
				// k, found it
				found = true;
			}
		}

		if ( current )
		{
			// tell the last page it is now exiting
			previous->OnExit( previous );

			// set our current page
			SetPage( current );

			// tell new page it is now loading
			current->OnLoad( current );

			// let the user know what happened
			gAllGlobals.gLog.ref()->Display( "Switched to Page '%s', as page '%s' is not available in HW mode\n",
				current->GetName().Get(), previous->GetName().Get() );
		}
	}
}

//==================================================================================
bool TuiElement::NotifyHardwareStatus( bool hwOn )
{
	bool pageNeedsUpdating = false;

	// k, hide/show appropriate items!
	TuiElementVector::iterator i;
	for ( i = mElements.begin(); i != mElements.end(); ++i )
	{
		TuiElement *elementPtr = (*i);
		if ( elementPtr )
		{
			bool visible = true;
			bool isCurrentlyVisible = elementPtr->IsVisible();

			if ( ( hwOn && !elementPtr->GetHWEnabled() ) ||
				 ( !hwOn && !elementPtr->GetSWEnabled() ) )
			{
				visible = false;
			}

			elementPtr->SetVisible( visible );

			if ( visible != isCurrentlyVisible )
			{
				pageNeedsUpdating = true;
			}
		}
	}

	return pageNeedsUpdating;
}

//==================================================================================
TuiElement * TuiElement::GetPreviousPage( void )
{
	TuiElement *prevPage = 0;

	// k, hide/show appropriate items!
	TuiElementVector::iterator i;
	for ( i = mElements.begin(); !prevPage && (i != mElements.end()); ++i )
	{
		TuiElement *elementPtr = (*i);
		if ( elementPtr && ( elementPtr->GetType() == TT_PAGE ) )
		{
			prevPage = elementPtr;
		}
	}

	return prevPage;
}
