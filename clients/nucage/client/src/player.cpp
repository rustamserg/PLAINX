#include "plainx.h"
#include "level.h"
#include "objects.h"
#include "player.h"

extern CLevel* g_pLevel;


CPlayer::CPlayer()
: CListener()
{
    m_eDir = MOVE_LEFT;
    m_pSp = NULL;
    m_eState = PL_STATE_IDLE;
    m_iMapXPos = 0;
    m_iMapYPos = 0;
    m_iSpXPos = 0;
    m_iSpYPos = 0;
    m_iSpOldXPos = 0;
    m_iSpOldYPos = 0;
    m_bIsReal = FALSE;
}


CPlayer::~CPlayer()
{
}


BOOL CPlayer::ProcessMessage(E_MESSAGE eMessage, DWORD dwCurrTick)
{
    int iGS;

    iGS = GET_ENGINE()->GetRTIntVar(NA_RT_GAME_STATE);
    if (iGS != GS_GAME)
        return FALSE;

    // move only from idle state
    if (m_eState != PL_STATE_IDLE)
        return FALSE;

    if (eMessage.dwMessageID == EM_KEYDOWN) {

        // handle move request
        if (eMessage.wParam == DIK_LEFT) {
            m_eDir = MOVE_LEFT;
            m_eState = PL_STATE_MOVE;
            GET_ENGINE()->PostState(ES_PLAYER_START_MOVE, m_iMapXPos - 1, m_iMapYPos,
                "actor", m_szName);
        }
        if (eMessage.wParam == DIK_RIGHT) {
            m_eDir = MOVE_RIGHT;
            m_eState = PL_STATE_MOVE;
            GET_ENGINE()->PostState(ES_PLAYER_START_MOVE, m_iMapXPos + 1, m_iMapYPos,
                "actor", m_szName);
        }
        if (eMessage.wParam == DIK_UP) {
            m_eDir = MOVE_UP;
            m_eState = PL_STATE_MOVE;
            GET_ENGINE()->PostState(ES_PLAYER_START_MOVE, m_iMapXPos, m_iMapYPos - 1,
                "actor", m_szName);
        }
        if (eMessage.wParam == DIK_DOWN) {
            m_eDir = MOVE_DOWN;
            m_eState = PL_STATE_MOVE;
            GET_ENGINE()->PostState(ES_PLAYER_START_MOVE, m_iMapXPos, m_iMapYPos + 1,
                "actor", m_szName);
        }
    }
    return FALSE;
}


BOOL CPlayer::ProcessState(E_STATE sState, DWORD dwCurrTick)
{
    CGObj* pObj;

    switch (sState.dwStateID) {
    case ES_LISTENERINIT:
        GetScene()->AddEffect("effects\\actor.efs", "teleport");
        GetScene()->AddEffect("effects\\actor.efs", "key");
        GetScene()->AddEffect("effects\\actor.efs", "door");
        GetScene()->AddEffect("effects\\actor.efs", "clock");
        break;
    case ES_PLAYER_START_MOVE:

        // check if we will be moved by arrow platform
        pObj = g_pLevel->GetMapObj(m_iMapXPos, m_iMapYPos);
        if (pObj->IsCanMove(m_eDir) == TRUE) {
            // notify that we are going to leaving cell
            GET_ENGINE()->PostState(ES_LEVEL_PLAYER_MOVED_BY_CELL,
                m_iMapXPos, m_iMapYPos, "level", "main");
            break;
        }

        // check for shadow player
        if (m_bIsReal == FALSE) {
            m_eState = PL_STATE_IDLE;
            break;
        }

        // check dest cell
        pObj = g_pLevel->GetMapObj(sState.lParam, sState.wParam);
        if (pObj == NULL || !pObj->IsCanEnter()) {
            m_eState = PL_STATE_IDLE;
            break;
        }

        // init sprite pos
        m_iSpXPos = m_pSp->GetWXPos();
        m_iSpYPos = m_pSp->GetWYPos();
        m_iSpOldXPos = m_iSpXPos;
        m_iSpOldYPos = m_iSpYPos;

        // init sprite anim
        StartMoveAnim(m_eDir);

        // start moving animation
        GET_ENGINE()->PostState(ES_PLAYER_SELF_MOVING, sState.lParam, sState.wParam,
            "actor", m_szName);

        // notify leaving cell
        GET_ENGINE()->PostState(ES_LEVEL_PLAYER_LEVING_CELL, m_iMapXPos, m_iMapYPos,
            "level", "main");
        break;
    case ES_PLAYER_SELF_MOVING:

		// 8 frames per one moving
		if (GetMSecDiff(dwCurrTick) > PLAYER_MOVE_DELTA) {
			if (m_bIsReal) {
				if (m_pSp->GetFrame() == 7) {
					m_pSp->SetFrame(0);
				} else {
					m_pSp->SetFrame(m_pSp->GetFrame() + 1);
				}
			}
			SetLastTick(dwCurrTick);
		} else {
			return FALSE;
		}

        // moving
        if (m_eDir == MOVE_RIGHT) {
            m_iSpXPos += PLAYER_MOVE_DIF;
        }
        if (m_eDir == MOVE_LEFT) {
            m_iSpXPos -= PLAYER_MOVE_DIF;
        }
        if (m_eDir == MOVE_UP) {
            m_iSpYPos += PLAYER_MOVE_DIF;
        }
        if (m_eDir == MOVE_DOWN) {
            m_iSpYPos -= PLAYER_MOVE_DIF;
        }

        // check if we already come
        if (abs(m_iSpXPos - m_iSpOldXPos) > MAP_CELL_SIZE ||
            abs(m_iSpYPos - m_iSpOldYPos) > MAP_CELL_SIZE)
        {
            // notify that 
            GET_ENGINE()->PostState(ES_PLAYER_STOP_MOVE, sState.lParam, sState.wParam,
                "actor", m_szName);
            return TRUE;
        }

        // render sprite on new pos
        m_pSp->SetWXPos(m_iSpXPos);
        m_pSp->SetWYPos(m_iSpYPos);
        return FALSE;
    case ES_PLAYER_STOP_MOVE:

        // set map pos
        m_iMapXPos = sState.lParam;
        m_iMapYPos = sState.wParam;

        // set sprite pos
        m_pSp->SetWXPos(g_pLevel->GetWXPos(m_iMapXPos));
        m_pSp->SetWYPos(g_pLevel->GetWYPos(m_iMapYPos));

        // reset state
        m_eState = PL_STATE_IDLE;

        // start idle anim
        StartIdleAnim();

        // notify that we come to cell
        GET_ENGINE()->PostState(ES_LEVEL_PLAYER_ENTER_CELL, sState.lParam, sState.wParam,
            "level", "main");
        break;
    case ES_PLAYER_ENTER_LEVEL:

        // check anim state, if stop when return control to player
        // and set sprite to idle anim
        if (m_pSp->GetAnimState() == SS_NONE) {
            m_eState = PL_STATE_IDLE;
            StartIdleAnim();
            return TRUE;
        }
        return FALSE;
    case ES_PLAYER_EXIT_LEVEL:

        // check anim state, if stop when send request for load next level
        if (m_pSp->GetAnimState() == SS_NONE) {
            m_eState = PL_STATE_IDLE;
            GET_ENGINE()->PostState(ES_LEVEL_SUCCEEDED, 0, 0, "level", "main");
            Disable();
            return TRUE;
        }
        return FALSE;
    }
    return TRUE;
}


void CPlayer::StartEnterAnim()
{
    if (m_bIsReal == FALSE)
        return;

    m_pSp->SetRow(PL_ROW_ENTER);
    m_pSp->SetFrame(0);
    m_pSp->SetAnimCycles(1);
    m_pSp->SetUpFrames(TRUE);
    m_pSp->SetSpeed(8);
    m_eState = PL_STATE_MOVE;

    PostState(ES_PLAYER_ENTER_LEVEL, 0, 0);
}


void CPlayer::StartExitAnim()
{
    if (m_bIsReal == FALSE)
        return;

    m_pSp->SetRow(PL_ROW_EXIT);
    m_pSp->SetFrame(0);
    m_pSp->SetAnimCycles(1);
    m_pSp->SetUpFrames(TRUE);
    m_pSp->SetSpeed(8);
    m_eState = PL_STATE_MOVE;

    PostState(ES_PLAYER_EXIT_LEVEL, 0, 0);
}


void CPlayer::StartIdleAnim()
{
    if (m_bIsReal == FALSE)
        return;

    m_pSp->SetRow(PL_ROW_IDLE);
    m_pSp->SetFrame(0);
    m_pSp->SetAnimCycles(0); // infinity loop
    m_pSp->SetUpFrames(TRUE);
    m_pSp->SetSpeed(8);
}


void CPlayer::StartMoveAnim(MoveID eDir)
{
    if (m_bIsReal == FALSE)
        return;

    if (eDir == MOVE_UP) m_pSp->SetRow(PL_ROW_UP);
    if (eDir == MOVE_DOWN) m_pSp->SetRow(PL_ROW_DOWN);
    if (eDir == MOVE_LEFT) m_pSp->SetRow(PL_ROW_LEFT);
    if (eDir == MOVE_RIGHT) m_pSp->SetRow(PL_ROW_RIGHT);

    m_pSp->SetFrame(0);
    m_pSp->SetAnimCycles(0);
    m_pSp->SetUpFrames(TRUE);
    m_pSp->SetSpeed(0);
}


void CPlayer::StartEffect(const char* pcszEffectName, int x, int y)
{
    if (m_bIsReal == FALSE)
        return;

    // under player
    GetScene()->DrawEffect(pcszEffectName,
        g_pLevel->GetWXPos(x), g_pLevel->GetWYPos(y), 1);
}