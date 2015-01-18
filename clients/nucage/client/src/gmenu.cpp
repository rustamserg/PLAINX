#include "plainx.h"
#include "gglob.h"
#include "buttons.h"
#include "map.h"
#include "gmenu.h"
#include "options.h"


extern COptionsDlg* g_pOptions;


CGameMenu::CGameMenu() :
CListener()
{
    m_pPLBut = NULL;
    m_pRLBut = NULL;
    m_pNLBut = NULL;
    m_pBTMBut = NULL;
    m_pQBut = NULL;
	m_pNextLvl = NULL;
	m_pPrevLvl = NULL;
	m_pNextWorld = NULL;
	m_pPrevWorld = NULL;
	m_pOptBut = NULL;
	m_pHelpBut = NULL;
	m_pOKBut = NULL;
}


CGameMenu::~CGameMenu()
{
}


BOOL CGameMenu::ProcessState(E_STATE sState, DWORD dwCurrTick)
{
    switch (sState.dwStateID) {
    case ES_LISTENERINIT:
        m_pPLBut = new CPlayLevel();
        m_pRLBut = new CRetryLevel();
        m_pNLBut = new CNextBut();
        m_pBTMBut = new CBackToMenu();
        m_pQBut = new CQuitMenu();
		m_pNextLvl = new CNextMiniLevel();
		m_pPrevLvl = new CPrevMiniLevel();
		m_pNextWorld = new CNextWorld();
		m_pPrevWorld = new CPrevWorld();
		m_pOptBut = new COptionsBut();
		m_pHelpBut = new CHelpBut();
		m_pOKBut = new COKBut();

        GetScene()->AddListener(m_pPLBut, "playbut");
        GetScene()->AddListener(m_pRLBut, "retrybut");
        GetScene()->AddListener(m_pNLBut, "nextbut");
        GetScene()->AddListener(m_pBTMBut, "backmenubut");
        GetScene()->AddListener(m_pQBut, "quitbut");
		GetScene()->AddListener(m_pNextLvl, "nextlvlbut");
		GetScene()->AddListener(m_pPrevLvl, "prevlvlbut");
		GetScene()->AddListener(m_pNextWorld, "nextworldbut");
		GetScene()->AddListener(m_pPrevWorld, "prevworldbut");
		GetScene()->AddListener(m_pOptBut, "optbut");
		GetScene()->AddListener(m_pHelpBut, "helpbut");
		GetScene()->AddListener(m_pOKBut, "okbut");

        m_pPLBut->InitButton("button", "but1", "buttonfnt");
        m_pRLBut->InitButton("button", "but2", "buttonfnt");
        m_pNLBut->InitButton("button", "but3", "buttonfnt");
        m_pBTMBut->InitButton("button", "but4", "buttonfnt");
        m_pQBut->InitButton("button", "but5", "buttonfnt");
		m_pNextLvl->InitButton("rarbutt", "", "buttonfnt");
		m_pPrevLvl->InitButton("larbutt", "", "buttonfnt");
		m_pNextWorld->InitButton("rarbutt", "", "buttonfnt");
		m_pPrevWorld->InitButton("larbutt", "", "buttonfnt");
		m_pOptBut->InitButton("button", "but6", "buttonfnt");
		m_pHelpBut->InitButton("button", "but7", "buttonfnt");
		m_pOKBut->InitButton("button", "but8", "buttonfnt");

		m_iWorldId = GET_STRING()->CreateFromTable("world1", "buttonfnt", 360, 388, 8);
		m_iLevelId = GET_STRING()->CreateFromTable("level", "buttonfnt", 380, 168, 8);
		m_iLevelValueId = GET_STRING()->Create("", "buttonfnt", 430, 168, 8);
		m_iLockedId = GET_STRING()->CreateFromTable("locked", "redfnt", 360, 280, 9);
		m_iTryAgainId = GET_STRING()->CreateFromTable("tryagain", "labelfnt", 400, 415, 8);
		m_iSuccessId = GET_STRING()->CreateFromTable("success", "labelfnt", 400, 415, 8);
		m_iWinId = GET_STRING()->CreateFromTable("win", "labelfnt", 80, 200, 8);

		char buf[MAX_BUFFER];

		for (int i = 0; i < NA_WORLD_DESC_NUM_LINES; i++) {
			wsprintf(buf, "wdesc1%i", i + 1);
			m_iWorldDesc[i] = GET_STRING()->CreateFromTable(buf, "greenfnt", 10, 400 - i*25, 8);
		}
        DrawStartMenu();
		PostState(ES_ENGINE_INITLANG, 0, 0);
        break;
	case ES_ENGINE_INITLANG:
		InitLng();
		break;
    }
    return TRUE;
}


void CGameMenu::InitLng()
{
	int iVal;

	GET_ENGINE()->GetVars("config")->GetInt(L_GROUP, "config", "lang", 0, &iVal);
	if (iVal == 0) {
		g_pOptions->SetEngLng();
	} else {
		g_pOptions->SetRusLng();
	}
}


void CGameMenu::DrawStartMenu()
{
    GetScene()->SetVisible(FALSE);
    GET_SCENE("level")->SetVisible(FALSE);
    GET_SCENE("actor")->SetVisible(FALSE);
	GET_STRING()->SetVisible(FALSE);

    GET_ENGINE()->SetRTIntVar(NA_RT_GAME_STATE, GS_MENU);

    GET_SPRITE("stmback")->SetVisible(TRUE);
    GET_SPRITE("stmback")->SetZPos(6);

    m_pPLBut->DrawButton(50, 380 ,7);
	m_pOptBut->DrawButton(50, 340, 7);
	m_pHelpBut->DrawButton(50, 300, 7);
    m_pQBut->DrawButton(50, 260, 7);

	DrawMiniLevel(GET_ENGINE()->GetRTIntVar(NA_RT_CURR_LEVEL_NUMBER));

	if (strcmp(GET_ENGINE()->GetRTStringVar(NA_RT_BACK_MUSIC), "backlogo") != 0) {
		GET_AUDIO()->StopMusic(GET_ENGINE()->GetRTStringVar(NA_RT_BACK_MUSIC));
		GET_ENGINE()->SetRTStringVar(NA_RT_BACK_MUSIC, "backlogo");

		int iMusVol;
		GET_ENGINE()->GetVars("config")->GetInt(L_GROUP, "config", "musvol", 0, &iMusVol);
		GET_AUDIO()->SetMusicVolume(GET_ENGINE()->GetRTStringVar(NA_RT_BACK_MUSIC), iMusVol);

		GET_AUDIO()->PlayMusic(GET_ENGINE()->GetRTStringVar(NA_RT_BACK_MUSIC));
	}
}

void CGameMenu::DrawMiniLevel(int iLevel)
{
	BOOL fLocked;
	int iMaxLevel, x, y;
	char szLevelName[MAX_PATH];
	CMap aMap;
	char buf[MAX_BUFFER];

	// load map
	sprintf(szLevelName, "levels\\lvl%02d.sam", iLevel);
	if (!aMap.LoadMap(szLevelName))
		return;

	// update level configuration
	iMaxLevel = GET_ENGINE()->GetRTIntVar(NA_RT_MAX_LEVEL_NUMBER);

	if (iLevel <= iMaxLevel) {
		GET_ENGINE()->SetRTIntVar(NA_RT_CURR_LEVEL_NUMBER, iLevel);
	}
	fLocked = (iLevel <= iMaxLevel) ? FALSE : TRUE;

	// draw routine
	GET_SPRITE("wrdname")->SetVisible(TRUE);
	GET_SPRITE("wrdname")->SetZPos(7);
	GET_SPRITE("lvlnum")->SetVisible(TRUE);
	GET_SPRITE("lvlnum")->SetZPos(7);

	m_pPrevWorld->DrawButton(270, 380, 7);
	m_pNextWorld->DrawButton(515, 380, 7);
	m_pPrevLvl->DrawButton(270, 160, 7);
	m_pNextLvl->DrawButton(515, 160, 7);

	// clear mini map
	for (y = 0; y < MAP_HEIGHT; y++) {
		for (x = 0; x < MAP_WIDTH; x++) {
			GET_SCENE("gamegui")->DelSprite(GetMiniElemName(x, y));
		}
	}

	// draw current map
	for (y = 0; y < MAP_HEIGHT; y++) {
		for (x = 0; x < MAP_WIDTH; x++) {
			CGObj* pObj = aMap.GetElement(x, MAP_HEIGHT - y - 1);
			if (pObj != NULL && pObj->m_eID != ID_STONE_BIG_REST) {
				GET_SCENE("gamegui")->CloneSprite("lvlcell", GetMiniElemName(x, y));

				int wx = x*GET_SPRITE("lvlcell")->GetFrameWidth() + 250;
				int wy = y*GET_SPRITE("lvlcell")->GetRowHeight() + 200;

				GET_SPRITE(GetMiniElemName(x, y))->SetWXPos(wx);
				GET_SPRITE(GetMiniElemName(x, y))->SetWYPos(wy);
				GET_SPRITE(GetMiniElemName(x, y))->SetVisible(TRUE);
				GET_SPRITE(GetMiniElemName(x, y))->SetZPos(8);
			}
		}
	}

	// update string info and real-time mini level var
	GET_STRING()->SetVisible(fLocked, m_iLockedId);
	GET_ENGINE()->SetRTIntVar(NA_RT_MINI_LEVEL_NUMBER, iLevel);

	// update world name
	wsprintf(buf, "world%i", iLevel/GET_ENGINE()->GetRTIntVar(NA_RT_WORLD_SIZE) + 1);
	GET_STRING()->ChangeFromTable(buf, m_iWorldId);
	GET_STRING()->SetVisible(TRUE, m_iWorldId);

	// update level id
	iLevel = (iLevel + 1) % GET_ENGINE()->GetRTIntVar(NA_RT_WORLD_SIZE);
	if (iLevel == 0) iLevel = GET_ENGINE()->GetRTIntVar(NA_RT_WORLD_SIZE);
	sprintf(buf, "%i", iLevel);
	GET_STRING()->Change(buf, m_iLevelValueId);
	GET_STRING()->SetVisible(TRUE, m_iLevelId);
	GET_STRING()->SetVisible(TRUE, m_iLevelValueId);
}

const char* CGameMenu::GetMiniElemName(int x, int y)
{
	sprintf(m_szName, "me%i_%i", x, y);
	return m_szName;
}

void CGameMenu::NextMiniLevel()
{
	int iMiniLevel;

	iMiniLevel = GET_ENGINE()->GetRTIntVar(NA_RT_MINI_LEVEL_NUMBER);
	iMiniLevel++;

	if (iMiniLevel < NA_MAX_LEVELS) {
		DrawMiniLevel(iMiniLevel);
	}
}

void CGameMenu::PrevMiniLevel()
{
	int iMiniLevel;

	iMiniLevel = GET_ENGINE()->GetRTIntVar(NA_RT_MINI_LEVEL_NUMBER);
	iMiniLevel--;

	if (iMiniLevel >= 0) {
		DrawMiniLevel(iMiniLevel);
	}
}

void CGameMenu::NextWorld()
{
	int iMiniLevel;
	int iWorldNum;

	iMiniLevel = GET_ENGINE()->GetRTIntVar(NA_RT_MINI_LEVEL_NUMBER);
	iWorldNum = iMiniLevel / GET_ENGINE()->GetRTIntVar(NA_RT_WORLD_SIZE);
	iMiniLevel = (iWorldNum + 1) * GET_ENGINE()->GetRTIntVar(NA_RT_WORLD_SIZE);

	if (iMiniLevel < NA_MAX_LEVELS) {
		DrawMiniLevel(iMiniLevel);
	}
}

void CGameMenu::PrevWorld()
{
	int iMiniLevel;
	int iWorldNum;

	iMiniLevel = GET_ENGINE()->GetRTIntVar(NA_RT_MINI_LEVEL_NUMBER);
	iWorldNum = iMiniLevel / GET_ENGINE()->GetRTIntVar(NA_RT_WORLD_SIZE);
	iMiniLevel = (iWorldNum - 1) * GET_ENGINE()->GetRTIntVar(NA_RT_WORLD_SIZE);

	if (iMiniLevel >= 0) {
		DrawMiniLevel(iMiniLevel);
	}
}

void CGameMenu::DrawStartLevel()
{
    GetScene()->SetVisible(FALSE);
	GET_STRING()->SetVisible(FALSE);

    GET_SPRITE("bar")->SetVisible(TRUE);
    GET_SPRITE("bar")->SetZPos(1);

    m_pRLBut->DrawButton(20, 20, 2);
    m_pBTMBut->DrawButton(492, 20, 2);

    GET_SCENE("level")->SetVisible(TRUE);
    GET_LISTENER2("level", "main")->ClearStates();
    GET_ENGINE()->PostState(ES_LEVEL_LOAD, 0, 0, "level", "main");
}


void CGameMenu::DrawSuccessedLevel()
{
    GetScene()->SetVisible(FALSE);
    GET_SCENE("level")->SetVisible(FALSE);
    GET_SCENE("actor")->SetVisible(FALSE);

	GET_STRING()->SetVisible(FALSE);

    GET_ENGINE()->SetRTIntVar(NA_RT_GAME_STATE, GS_MENU);

    GET_SPRITE("smback")->SetVisible(TRUE);
    GET_SPRITE("smback")->SetZPos(7);

	GET_STRING()->SetVisible(TRUE, m_iSuccessId);

    m_pNLBut->DrawButton(150, 200, 8);
    m_pBTMBut->DrawButton(362, 200, 8);
}

void CGameMenu::DrawWinScreen()
{
	GetScene()->SetVisible(FALSE);
	GET_SCENE("level")->SetVisible(FALSE);
	GET_SCENE("actor")->SetVisible(FALSE);

	GET_STRING()->SetVisible(FALSE);
	
	GET_ENGINE()->SetRTIntVar(NA_RT_GAME_STATE, GS_WINSCR);

	GET_SPRITE("winscr")->SetVisible(TRUE);
	GET_SPRITE("winscr")->SetZPos(7);

	GET_STRING()->SetVisible(TRUE, m_iWinId);

	m_pOKBut->DrawButton(492, 20, 8);
}

void CGameMenu::DrawFailedLevel()
{
    GetScene()->SetVisible(FALSE);
    GET_SCENE("level")->SetVisible(FALSE);
    GET_SCENE("actor")->SetVisible(FALSE);

	GET_STRING()->SetVisible(FALSE);

    GET_ENGINE()->SetRTIntVar(NA_RT_GAME_STATE, GS_MENU);

    GET_SPRITE("fmback")->SetVisible(TRUE);
    GET_SPRITE("fmback")->SetZPos(7);

	GET_STRING()->SetVisible(TRUE, m_iTryAgainId);

    m_pRLBut->DrawButton(150, 200, 8);
    m_pBTMBut->DrawButton(362, 200, 8);
}


void CGameMenu::DrawWorldIntroStart(int iWorldId)
{
	char buf[MAX_BUFFER];

	GetScene()->SetVisible(FALSE);
	GET_SCENE("actor")->SetVisible(FALSE);
	GET_STRING()->SetVisible(FALSE);

	m_pOKBut->DrawButton(492, 20, 8);

	for (int i = 0; i < NA_WORLD_DESC_NUM_LINES; i++) {
		wsprintf(buf, "wdesc%i%i", iWorldId + 1, i + 1);
		GET_STRING()->ChangeFromTable(buf, m_iWorldDesc[i]);
		GET_STRING()->SetVisible(TRUE, m_iWorldDesc[i]);
	}
}


void CGameMenu::DrawWorldIntroEnd()
{
	GetScene()->SetVisible(FALSE);
	GET_SPRITE2("level", "wintro")->SetVisible(FALSE);

	GET_STRING()->SetVisible(FALSE);

	GET_SPRITE("bar")->SetVisible(TRUE);
	GET_SPRITE("bar")->SetZPos(1);

	m_pRLBut->DrawButton(20, 20, 2);
	m_pBTMBut->DrawButton(492, 20, 2);

	GET_ENGINE()->PostState(ES_LEVEL_START, 0, 0, "level", "main");
}

void CGameMenu::FixLevelLabel(BOOL fRusLng)
{
	if (!fRusLng)
		GET_STRING()->SetPos(m_iLevelId, 380, 168, 8);
	else
		GET_STRING()->SetPos(m_iLevelId, 350, 168, 8);
}
