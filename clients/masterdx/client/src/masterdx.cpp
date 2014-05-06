// masterdx.cpp: implementation of the CMasterDX class.
//
//////////////////////////////////////////////////////////////////////

#include "plainx.h"
#include "masterdx.h"
#include "dialogs.h"
#include "buttons.h"
#include "ui.h"
#include "game.h"
#include "global_game.h"


//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
BOOL CMasterDX::ProcessState(E_STATE eState, DWORD dwCurrTick)
{
	COptBut*       pButton       = NULL;
	CCloseBut*     pClose        = NULL;
	CMinBut*       pMinim        = NULL;
	CDialogLW*     pDialogLW     = NULL;
	CDialogNagOpt* pDialogNagOpt = NULL;
	CDialogRegRes* pDialogRegRes = NULL;
	CDialogLW*     pDialogNewOpt = NULL;
	CDialogNag*    pDialogNag    = NULL;
	CPad*          pPad          = NULL;
	COptions*      pOpt          = NULL;
	CCheck*        pCheck        = NULL;
	CNewBtn*       pNew          = NULL;
	CFCell*        pFCell        = NULL;
	CTCell*        pTCell        = NULL;

	switch (eState.dwStateID) 
	{
	case ES_STARTENGINE:
		//init resources
		AddResource("data\\masterdx.dat", "masterdx");
		AddResource("data\\options.dat",  "options");
		AddResource("data\\dialog.dat",   "dialog");
		
		//init scenes
		AddScene("scene\\mmpad.scn",     "mmpad",          "masterdx");
		AddScene("scene\\masterdx.scn",  "masterdx_frame", "masterdx");
		AddScene("scene\\masterdx2.scn", "masterdx_gui",   "masterdx");
		AddScene("scene\\dlglw.scn",     "dlglw",          "dialog");
		AddScene("scene\\dlgnagopt.scn", "dlgnagopt",      "dialog");
		AddScene("scene\\dlgnewopt.scn", "dlgnewopt",      "dialog");
		AddScene("scene\\dlgreg.scn",    "dlgreg",         "dialog");
		AddScene("scene\\dlgregrs.scn",  "dlgregres",      "dialog");
		AddScene("scene\\dlgnag.scn",    "dlgnag",         "dialog");
		AddScene("scene\\fishki.scn",    "fishki",         "masterdx");
		AddScene("scene\\afishki.scn",   "afishki",        "masterdx");
		AddScene("scene\\kozyirek.scn",  "kozyirek",       "masterdx");
		AddScene("scene\\selframe.scn",  "selframe",       "masterdx");
		AddScene("scene\\lnframe.scn",   "lnframe",        "masterdx");
		AddScene("scene\\options.scn",   "options",        "options");

		//create game objects
		pButton       = new COptBut();
		pClose        = new CCloseBut();
		pMinim        = new CMinBut();
		pDialogLW     = new CDialogLW();
		pDialogNagOpt = new CDialogNagOpt();
		pDialogNewOpt = new CDialogLW();
		pDialogNag    = new CDialogNag();
		pPad          = new CPad();
		pOpt          = new COptions();
		pCheck        = new CCheck();
		pNew          = new CNewBtn();
		pFCell        = new CFCell();
		pTCell        = new CTCell();

		//add mouse support
		CreateMouse("masterdx");
		
		//add listeners
		GetScene("mmpad")->AddListener(pPad, "mmpad");
		GetScene("mmpad")->AddListener(pFCell, "fcell");
		GetScene("mmpad")->AddListener(pTCell, "tcell");
		GetScene("masterdx_gui")->AddListener(pButton, "button");
		GetScene("masterdx_gui")->AddListener(pClose, "close");
		GetScene("masterdx_gui")->AddListener(pMinim, "minim");
		GetScene("masterdx_gui")->AddListener(pCheck, "check");
		GetScene("masterdx_gui")->AddListener(pNew, "new");
		GetScene("dlglw")->AddListener(pDialogLW, "dialog");
		GetScene("dlgnagopt")->AddListener(pDialogNagOpt, "dialog");
		GetScene("dlgnewopt")->AddListener(pDialogNewOpt, "dialog");
		GetScene("dlgnag")->AddListener(pDialogNag, "dialog");
		GetScene("options")->AddListener(pOpt, "options");

		// init listeners ///////////////////////////////////////

		//init buttons
		pButton->SetButton("button");
		pClose->SetButton("close");
		pMinim->SetButton("minim");
		pCheck->SetButton("check");
		pNew->SetButton("new");

		//init dialogs
		pDialogLW->SetDialog("dialog");
		pDialogLW->SetOk("okcenter");
		pDialogLW->Disable();

		pDialogNagOpt->SetDialog("dialog");
		pDialogNagOpt->SetOk("okcenter");
		pDialogNagOpt->Disable();
		
		pDialogNewOpt->SetDialog("dialog");
		pDialogNewOpt->SetOk("okcenter");
		pDialogNewOpt->Disable();

		pDialogNag->SetDialog("dialog");
		pDialogNag->SetOk("okcenter");
		pDialogNag->Disable();

		//init cells
		pFCell->Init();
		pTCell->Init();

		//init game settings
		AddVars("game.ini", "config");

		//init audio
		GetAudio()->LoadGroup("sfx\\sound.sfx", "sound");

        //restore audio settings
        RestoreAudio();

		//init game
		pPad->Init();
				
		//turn on music check
		PostState(ES_ENGINE_CHECKMUSIC, 0, 0);
		
		//go on!
		pPad->master_new();
		break;
	case ES_ENGINE_CHECKMUSIC:
	  //check if back music stop then play it again
          if (!GetAudio()->IsMusicPlay("back")) {
            GetAudio()->StopMusic("back");
	    GetAudio()->PlayMusic("back");
          }
          return FALSE;
	}
	return TRUE;
}


//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
void CMasterDX::RestoreAudio()
{
	int nSVol = 100;
	int nMVol = 70;
        int nMute;
	
        GetVars("config")->GetInt(L_GROUP, "config", "soundvol", 0, &nSVol);
	GetVars("config")->GetInt(L_GROUP, "config", "musicvol", 0, &nMVol);
	if (nSVol>255) nSVol = 255;
	if (nSVol<0) nSVol = 0;
	if (nMVol>255) nMVol = 255;
	if (nMVol<0) nMVol = 0;
	
        GetAudio()->SetSFXVolume(nSVol);
        GetAudio()->SetMusicVolume("back", nMVol);
        GetAudio()->PlayMusic("back");
        
        GetVars("config")->GetInt(L_GROUP, "config", "mute", 0, &nMute);
        GetAudio()->SetMute(nMute);
}

