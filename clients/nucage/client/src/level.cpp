#include "plainx.h"
#include "map.h"
#include "gglob.h"
#include "objects.h"
#include "player.h"
#include "level.h"
#include "gtm.h"
#include "keys.h"
#include "gmenu.h"


#define LVL_X_START 0
#define LVL_Y_START 480

extern CPlayer* g_pRlPlayer;
extern CPlayer* g_pShPlayer;
extern CGameTime* g_pGtm;
extern CKeys* g_pKeys;
extern CGameMenu* g_pMenu;

CLevel::CLevel()
: CListener()
{
    m_pPlayer = NULL;
    strcpy(m_szSkin, "sand");
}


CLevel::~CLevel()
{
}


BOOL CLevel::ProcessMessage(E_MESSAGE eMessage, DWORD dwCurrTick)
{
    POINT ptW;
    int iGS;

    iGS = GET_ENGINE()->GetRTIntVar(NA_RT_GAME_STATE);
    if (iGS != GS_GAME)
        return FALSE;

    switch (eMessage.dwMessageID) {
    case EM_LMOUSEDOWN:
        if (GetScene()->ScreenToWorld(eMessage.lParam, eMessage.wParam, &ptW)) {
            SetPlayer(GetXPos(ptW.x), GetYPos(ptW.y));
        }
        break;
    }
    return FALSE;
}


BOOL CLevel::ProcessState(E_STATE sState, DWORD dwCurrTick)
{
    CGObj* pObj;
    BOOL bRes;
    int iLevel;
    char szLevelName[_MAX_PATH];

    switch (sState.dwStateID) {
        case ES_LISTENERINIT:
            GetScene()->AddEffect("effects\\level.efs", "oldstone");
            InitPlayer();
            break;
        case ES_LEVEL_FAILED:
			m_pPlayer->ClearStates();
			g_pGtm->StopTimer();
            g_pMenu->DrawFailedLevel();
            break;
        case ES_LEVEL_SUCCEEDED:
            GET_AUDIO()->PlaySample("start");
			m_pPlayer->ClearStates();
			g_pGtm->StopTimer();
            iLevel = GET_ENGINE()->GetRTIntVar(NA_RT_CURR_LEVEL_NUMBER);
			if (iLevel < NA_NUM_WORLDS*GET_ENGINE()->GetRTIntVar(NA_RT_WORLD_SIZE) - 1) {
				GET_ENGINE()->SetRTIntVar(NA_RT_CURR_LEVEL_NUMBER, ++iLevel);
				UpdateConfig();
				g_pMenu->DrawSuccessedLevel();
			} else {
				g_pMenu->DrawWinScreen();
			}
            break;
        case ES_LEVEL_LOAD:
            m_map.ClearMap();
            sprintf(szLevelName, "levels\\lvl%02d.sam", GET_ENGINE()->GetRTIntVar(NA_RT_CURR_LEVEL_NUMBER));
            if (!m_map.LoadMap(szLevelName)) {
                GET_ENGINE()->SetRTIntVar(NA_RT_CURR_LEVEL_NUMBER, 0);
				UpdateConfig();
                PostState(ES_LEVEL_LOAD, 0, 0);
                break;
            }
            ClearLevel();
            InitPlayer();
            CreateLevel();
			EnterWorld();
            break;
		case ES_LEVEL_START:
			InitPlayer();
			g_pGtm->SetTime(m_map.GetTime());
			g_pGtm->StartTimer();
			g_pKeys->InitKeys();
			m_pPlayer->StartEnterAnim();
			GET_ENGINE()->SetRTIntVar(NA_RT_GAME_STATE, GS_GAME);
			GET_AUDIO()->PlaySample("start");
			UpdateBackMusic();
			break;
		case ES_LEVEL_RESTART:
			m_map.ClearMap();
			sprintf(szLevelName, "levels\\lvl%02d.sam", GET_ENGINE()->GetRTIntVar(NA_RT_CURR_LEVEL_NUMBER));
			m_map.LoadMap(szLevelName);
			ClearLevel();
			InitPlayer();
			CreateLevel();
			g_pGtm->SetTime(m_map.GetTime());
			g_pGtm->StartTimer();
			g_pKeys->InitKeys();
			m_pPlayer->StartEnterAnim();
			GET_AUDIO()->PlaySample("start");
			break;
        case ES_LEVEL_PLAYER_LEVING_CELL:
            pObj = GetMapObj(sState.lParam, sState.wParam);
            if (pObj) pObj->OnExit();
            break;
        case ES_LEVEL_PLAYER_MOVED_BY_CELL:
			if (GetMSecDiff(dwCurrTick) > CELL_MOVE_DELTA) {
				SetLastTick(dwCurrTick);
			} else {
				return FALSE;
			}
            pObj = GetMapObj(sState.lParam, sState.wParam);
            bRes = pObj->OnMoving(m_pPlayer->m_eDir);
            if (bRes == TRUE) {
                GET_ENGINE()->PostState(ES_PLAYER_STOP_MOVE, pObj->m_iX, pObj->m_iY,
                    "actor", m_pPlayer->m_szName);
            }
            return bRes;
        case ES_LEVEL_PLAYER_ENTER_CELL:
			pObj = GetMapObj(sState.lParam, sState.wParam);
			if (pObj) pObj->OnEnter();
            break;
    }
    return TRUE;
}


void CLevel::ClearLevel()
{
    for (int y = 0; y < MAP_HEIGHT; y++) {
        for (int x = 0; x < MAP_WIDTH; x++) {
            GET_SCENE("level")->DelSprite(GetElemName(x, y));
        }
    }
}

void CLevel::UpdateBackMusic()
{
	char szMusicName[100];

	if (strcmp(m_szSkin, "sand") == 0) {
		strcpy(szMusicName, "backdesr");
	} else if (strcmp(m_szSkin, "ice") == 0) {
		strcpy(szMusicName, "backice");
	} else if (strcmp(m_szSkin, "gold") == 0) {
		strcpy(szMusicName, "backgold");
	} else {
		strcpy(szMusicName, "backfors");
	}

	if (strcmp(GET_ENGINE()->GetRTStringVar(NA_RT_BACK_MUSIC), szMusicName) != 0) {
		GET_AUDIO()->StopMusic(GET_ENGINE()->GetRTStringVar(NA_RT_BACK_MUSIC));
		GET_ENGINE()->SetRTStringVar(NA_RT_BACK_MUSIC, szMusicName);

		int iMusVol;
		GET_ENGINE()->GetVars("config")->GetInt(L_GROUP, "config", "musvol", 0, &iMusVol);
		GET_AUDIO()->SetMusicVolume(GET_ENGINE()->GetRTStringVar(NA_RT_BACK_MUSIC), iMusVol);
		
		GET_AUDIO()->PlayMusic(GET_ENGINE()->GetRTStringVar(NA_RT_BACK_MUSIC));
	}
}

void CLevel::CreateLevel()
{
    // load new skin
    if (strcmp(m_map.GetSkin(), m_szSkin) != 0) {
        strcpy(m_szSkin, m_map.GetSkin());
        if (GET_ENGINE()->GetResource(m_szSkin) == NULL) {
            char szResPath[MAX_BUFFER];
            sprintf(szResPath, "data\\%s.dat", m_szSkin);
            if (!GET_ENGINE()->AddResource(szResPath, m_szSkin)) {
                strcpy(m_szSkin, "sand");
            }
        }
        GetScene()->SetDataResourceManager(m_szSkin);
    }

    for (int y = 0; y < MAP_HEIGHT; y++) {
        for (int x = 0; x < MAP_WIDTH; x++) {
            CGObj* pObj = m_map.GetElement(x, y);
            if (pObj != NULL && pObj->m_eID != ID_STONE_BIG_REST) {
                GET_SCENE("level")->CloneSprite(pObj->m_pszName, GetElemName(x, y));
                GET_SPRITE(GetElemName(x, y))->SetWXPos(GetWXPos(x));
                GET_SPRITE(GetElemName(x, y))->SetWYPos(GetWYPos(y));
                GET_SPRITE(GetElemName(x, y))->SetVisible(TRUE);
                pObj->m_pSp = GET_SPRITE(GetElemName(x, y));
                if (pObj->m_eID == ID_ENTER) {
                    // set player pos to enter cell
                    m_pPlayer->m_pSp->SetWXPos(GetWXPos(x));
                    m_pPlayer->m_pSp->SetWYPos(GetWYPos(y));
                    m_pPlayer->m_pSp->SetVisible(TRUE);
                    m_pPlayer->m_iMapXPos = x;
                    m_pPlayer->m_iMapYPos = y;
                }
            }
        }
    }
}


int CLevel::GetWXPos(int x)
{
    return LVL_X_START + x * MAP_CELL_SIZE;
}


int CLevel::GetWYPos(int y)
{
    return LVL_Y_START - y * MAP_CELL_SIZE;
}


int CLevel::GetXPos(int nWX)
{
    nWX = nWX - LVL_X_START;
    return (int)(nWX/MAP_CELL_SIZE);
}


int CLevel::GetYPos(int nWY)
{
    nWY = LVL_Y_START - nWY;
    return (int)(nWY/MAP_CELL_SIZE);
}


void CLevel::InitPlayer()
{
	GET_SCENE("actor")->SetVisible(TRUE);
    g_pRlPlayer->m_pSp = GET_SPRITE2("actor", "rlplayer");
    g_pShPlayer->m_pSp = GET_SPRITE2("actor", "shplayer");
    g_pRlPlayer->m_eState = PL_STATE_IDLE;
    g_pShPlayer->m_eState = PL_STATE_IDLE;
    g_pRlPlayer->ClearStates();
    g_pShPlayer->ClearStates();

    g_pShPlayer->m_pSp->SetVisible(FALSE);
    g_pShPlayer->Disable();

    g_pRlPlayer->m_pSp->SetVisible(TRUE);
    g_pRlPlayer->Enable();

    m_pPlayer = g_pRlPlayer;
}


void CLevel::SetPlayer(int x, int y)
{
    CGObj* pObj = m_map.GetElement(x, y);
    if (pObj != NULL) {
        if (pObj->m_eID >= ID_ARROW_UP && pObj->m_eID <= ID_ARROW_CROSS) {

            if ((g_pRlPlayer->m_iMapXPos != x || g_pRlPlayer->m_iMapYPos != y) &&
                (g_pRlPlayer->m_eState == PL_STATE_IDLE))
            {
                g_pRlPlayer->Disable();
                g_pShPlayer->Enable();

                m_pPlayer = g_pShPlayer;
                m_pPlayer->m_pSp->SetWXPos(GetWXPos(x));
                m_pPlayer->m_pSp->SetWYPos(GetWYPos(y));
                m_pPlayer->m_pSp->SetVisible(TRUE);
                m_pPlayer->m_iMapXPos = x;
                m_pPlayer->m_iMapYPos = y;
				GET_AUDIO()->PlaySample("get");
                return;
            }
        }
    }
    if (m_pPlayer == g_pShPlayer && g_pShPlayer->m_eState == PL_STATE_IDLE) {

        g_pRlPlayer->Enable();
        g_pShPlayer->Disable();

        m_pPlayer->m_pSp->SetVisible(FALSE);
        m_pPlayer = g_pRlPlayer;
		GET_AUDIO()->PlaySample("get");
    }
}


CGObj* CLevel::GetMapObj(int x, int y)
{
    return m_map.GetElement(x, y);
}


void CLevel::SetMapObj(int x, int y, CGObj* pObj)
{
    // del old sprite
    GET_SCENE("level")->DelSprite(GetElemName(x, y));

    // del from map
    CGObj* pOldObj = m_map.GetElement(x, y);
    if (pOldObj != NULL)
        delete pOldObj;

    // clone new sprite
    if (pObj != NULL && pObj->m_eID != ID_STONE_BIG_REST) {
        GET_SCENE("level")->CloneSprite(pObj->m_pszName, GetElemName(x, y));
        GET_SPRITE(GetElemName(x, y))->SetWXPos(GetWXPos(x));
        GET_SPRITE(GetElemName(x, y))->SetWYPos(GetWYPos(y));
        GET_SPRITE(GetElemName(x, y))->SetVisible(TRUE);
        pObj->m_pSp = GET_SPRITE(GetElemName(x, y));
    }
    m_map.SetElement(x, y, pObj);
}


void CLevel::TeleportPlayer(int xFrom, int yFrom)
{
    // find another teleport
    for (int xTo = 0; xTo < MAP_WIDTH; xTo++) {
        for (int yTo = 0; yTo < MAP_HEIGHT; yTo++) {
            CGObj* pObj = m_map.GetElement(xTo, yTo);
            if (pObj != NULL && pObj->m_eID == ID_TELEPORT && xTo != xFrom && yTo != yFrom) {
                m_pPlayer->StartEffect("teleport", xTo, yTo);
                m_pPlayer->m_pSp->SetWXPos(GetWXPos(xTo));
                m_pPlayer->m_pSp->SetWYPos(GetWYPos(yTo));
                m_pPlayer->m_iMapXPos = xTo;
                m_pPlayer->m_iMapYPos = yTo;
                m_pPlayer->ClearStates();
                m_pPlayer->m_eState = PL_STATE_IDLE;
                break;
            }
        }
    }
}


const char* CLevel::GetElemName(int x, int y)
{
    sprintf(m_szBuf, "e%i_%i", x, y);
    return m_szBuf;
}


void CLevel::StartOldStoneEffect(int x, int y)
{
    GetScene()->DrawEffect("oldstone", GetWXPos(x), GetWYPos(y), 3);
}

void CLevel::UpdateConfig()
{
	int iCurrLevel, iMaxLevel;

	iCurrLevel = GET_ENGINE()->GetRTIntVar(NA_RT_CURR_LEVEL_NUMBER);
	iMaxLevel = GET_ENGINE()->GetRTIntVar(NA_RT_MAX_LEVEL_NUMBER);

	if (iCurrLevel > iMaxLevel) {
		GET_ENGINE()->GetVars("config")->SetInt(L_GROUP, "config", "maxlvl", 0, iCurrLevel);
		GET_ENGINE()->SetRTIntVar(NA_RT_MAX_LEVEL_NUMBER, iCurrLevel);
	}
	GET_ENGINE()->GetVars("config")->SetInt(L_GROUP, "config", "curlvl", 0, iCurrLevel);
}

void CLevel::EnterWorld()
{
	int iLevel;

	GET_SPRITE("wintro")->SetVisible(FALSE);

	iLevel = GET_ENGINE()->GetRTIntVar(NA_RT_CURR_LEVEL_NUMBER);
	if (iLevel % GET_ENGINE()->GetRTIntVar(NA_RT_WORLD_SIZE) != 0) {
		PostState(ES_LEVEL_START, 0, 0);
		return;
	}

	int iWorldId = iLevel / GET_ENGINE()->GetRTIntVar(NA_RT_WORLD_SIZE);
	GET_SPRITE("wintro")->SetVisible(TRUE);
	g_pMenu->DrawWorldIntroStart(iWorldId);
}
