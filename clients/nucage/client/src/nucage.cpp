#include "plainx.h"
#include "level.h"
#include "gtm.h"
#include "keys.h"
#include "gmenu.h"
#include "help.h"
#include "options.h"
#include "nucage.h"

CGameMenu* g_pMenu;
CLevel* g_pLevel;
CPlayer* g_pRlPlayer;
CPlayer* g_pShPlayer;
CGameTime* g_pGtm;
CKeys* g_pKeys;
CHelpDlg* g_pHelp;
COptionsDlg* g_pOptions;

CNucAge::CNucAge() :
CEngine()
{
	g_pMenu = NULL;
	g_pLevel = NULL;
	g_pRlPlayer = NULL;
	g_pShPlayer = NULL;
	g_pGtm = NULL;
	g_pKeys = NULL;
	g_pHelp = NULL;
	g_pOptions = NULL;
}

CNucAge::~CNucAge()
{
}

BOOL CNucAge::ProcessState(E_STATE eState, DWORD dwCurrTick)
{
    switch (eState.dwStateID) {
        case ES_STARTENGINE:
            // init resources
            AddResource("data\\cursor.dat", "cursor");
            AddResource("data\\sand.dat", "sand");
            AddResource("data\\actor.dat", "actor");
            AddResource("data\\gamegui.dat", "gamegui");

            // init scenes
            AddScene("scenes\\gamegui.scn", "gamegui", "gamegui");
            AddScene("scenes\\level.scn", "level", "sand");
            AddScene("scenes\\actor.scn", "actor", "actor");

            // create game objects
            g_pLevel = new CLevel();
            g_pRlPlayer = new CPlayer();
            g_pShPlayer = new CPlayer();
            g_pGtm = new CGameTime();
            g_pKeys = new CKeys();
            g_pMenu = new CGameMenu();
			g_pHelp = new CHelpDlg();
			g_pOptions = new COptionsDlg();

            // set players name
            strcpy(g_pRlPlayer->m_szName, "rlplayer");
            strcpy(g_pShPlayer->m_szName, "shplayer");
            g_pRlPlayer->m_bIsReal = TRUE;
            g_pShPlayer->m_bIsReal = FALSE;

            // add mouse support
            CreateMouse("cursor");

            // add listeners
            GetScene("level")->AddListener(g_pLevel, "main");
            GetScene("actor")->AddListener(g_pRlPlayer, g_pRlPlayer->m_szName);
            GetScene("actor")->AddListener(g_pShPlayer, g_pShPlayer->m_szName);
            GetScene("gamegui")->AddListener(g_pGtm, "time");
            GetScene("gamegui")->AddListener(g_pKeys, "keys");
			GetScene("gamegui")->AddListener(g_pMenu, "main");
			GetScene("gamegui")->AddListener(g_pHelp, "help");
			GetScene("gamegui")->AddListener(g_pOptions, "options");

            GetScene("level")->SetVisible(FALSE);
            GetScene("actor")->SetVisible(FALSE);

			// create fonts
			GetScene("gamegui")->GetStringManager()->CreateFont("fonts\\fonts.fnt", "greenfnt");
			GetScene("gamegui")->GetStringManager()->CreateFont("fonts\\fonts.fnt", "buttonfnt");
			GetScene("gamegui")->GetStringManager()->CreateFont("fonts\\fonts.fnt", "redfnt");
			GetScene("gamegui")->GetStringManager()->CreateFont("fonts\\fonts.fnt", "labelfnt");

			// init string table
			GetStringTable()->Init("strings.dat");
			GetStringTable()->SetLocale("en");

			// init audio
			GetAudio()->LoadGroup("sfx\\sound.sfx", "sound");

			// we start game from menu screen
			SetRTIntVar(NA_RT_GAME_STATE, GS_MENU);

			// create game variable object
			AddVars("game.ini", "config");

			// get world size value
			int iLevel, iWorldSize;
			GetVars("config")->GetInt(L_GROUP, "config", "worldsize", 0, &iWorldSize);
			if (iWorldSize <= 0 || iWorldSize > 25) iWorldSize = 25;
			SetRTIntVar(NA_RT_WORLD_SIZE, iWorldSize);

			// get current level value
			GetVars("config")->GetInt(L_GROUP, "config", "curlvl", 0, &iLevel);
			if (iLevel >= NA_NUM_WORLDS*iWorldSize) iLevel = NA_NUM_WORLDS*iWorldSize - 1;
			SetRTIntVar(NA_RT_CURR_LEVEL_NUMBER, iLevel);
			SetRTIntVar(NA_RT_MINI_LEVEL_NUMBER, iLevel);

			// get maximum achieved number of levels
			GetVars("config")->GetInt(L_GROUP, "config", "maxlvl", 0, &iLevel);
			if (iLevel >= NA_NUM_WORLDS*iWorldSize) iLevel = NA_NUM_WORLDS*iWorldSize - 1;
			SetRTIntVar(NA_RT_MAX_LEVEL_NUMBER, iLevel);

			// sync current level with max level
			if (GetRTIntVar(NA_RT_CURR_LEVEL_NUMBER) > iLevel)
			{
				SetRTIntVar(NA_RT_CURR_LEVEL_NUMBER, iLevel);
				SetRTIntVar(NA_RT_MINI_LEVEL_NUMBER, iLevel);
			}

			// update audio
			int iSndVol, iMusVol;
			BOOL fMute;
			GetVars("config")->GetInt(L_GROUP, "config", "mute", 0, &fMute);
			GetVars("config")->GetInt(L_GROUP, "config", "sndvol", 0, &iSndVol);
			GetVars("config")->GetInt(L_GROUP, "config", "musvol", 0, &iMusVol);

			GetAudio()->SetSFXVolume(iSndVol);
			GetAudio()->SetMusicVolume("backlogo", iMusVol);
			GetAudio()->SetMute(fMute);

			SetRTStringVar(NA_RT_BACK_MUSIC, "backlogo");
			GetAudio()->PlayMusic("backlogo");

			//turn on music check
			PostState(ES_ENGINE_CHECKMUSIC, 0, 0);
			break;
		case ES_ENGINE_CHECKMUSIC:
			//check if back music stop then play it again
			if (!GetAudio()->IsMusicPlay(GetRTStringVar(NA_RT_BACK_MUSIC))) {
				GetAudio()->StopMusic(GetRTStringVar(NA_RT_BACK_MUSIC));
				GetAudio()->PlayMusic(GetRTStringVar(NA_RT_BACK_MUSIC));
			}
			return FALSE;
	}
	return TRUE;
}


void CNucAge::ProcessConsole(const char* pcszConCmd)
{
	if (GS_GAME == GetRTIntVar(NA_RT_GAME_STATE))
	{
		if (strcmp(pcszConCmd, "winlevel") == 0)
		{
			PostState(ES_LEVEL_SUCCEEDED, 0, 0, "level", "main");
		}
		else if (strcmp(pcszConCmd, "openall") == 0)
		{
			SetRTIntVar(NA_RT_MAX_LEVEL_NUMBER, NA_MAX_LEVELS - 1);
		}
	}
}