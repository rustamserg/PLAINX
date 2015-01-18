#ifndef OBJECTS_H
#define OBJECTS_H

#include "plainx.h"
#include "gobj.h"
#include "level.h"
#include "player.h"
#include "gtm.h"
#include "keys.h"

// class factory
CGObj* GetObj(MapID eID);

extern CLevel* g_pLevel;
extern CGameTime* g_pGtm;
extern CKeys* g_pKeys;


// game objects ///////////////////////////////////////////////////////////////
class CStone : public CGObj
{
public:
    CStone()
    {
        m_eID = ID_STONE;
        m_pszName = "stone";
    }
    BOOL IsCanEnter() { return FALSE; }
};


class CPlate : public CGObj
{
public:
    CPlate()
    {
        m_eID = ID_PLATE;
        m_pszName = "plate";
    }
    BOOL IsCanEnter() { return TRUE; }
};


class CKeyRed : public CGObj
{
public:
    CKeyRed()
    {
        m_eID = ID_KEY_RED;
        m_pszName = "redkey";
    }
    BOOL IsCanEnter() { return TRUE; }

    void OnEnter()
    {
        g_pKeys->PushKey(KEY_RED);
        g_pLevel->m_pPlayer->StartEffect("key", m_iX, m_iY);
        g_pLevel->SetMapObj(m_iX, m_iY, GetObj(ID_PLATE));
		g_pLevel->GetScene()->GetEngine()->GetAudio()->PlaySample("get");
    }
};


class CKeyGreen : public CGObj
{
public:
    CKeyGreen()
    {
        m_eID = ID_KEY_GREEN;
        m_pszName = "greenkey";
    }
    BOOL IsCanEnter() { return TRUE; }

    void OnEnter()
    {
        g_pKeys->PushKey(KEY_GREEN);
        g_pLevel->m_pPlayer->StartEffect("key", m_iX, m_iY);
        g_pLevel->SetMapObj(m_iX, m_iY, GetObj(ID_PLATE));
		g_pLevel->GetScene()->GetEngine()->GetAudio()->PlaySample("get");
    }
};


class CGateRed : public CGObj
{
public:
    CGateRed()
    {
        m_eID = ID_GATE_RED;
        m_pszName = "redgate";
    }
    BOOL IsCanEnter() { return g_pKeys->IsKeyExist(KEY_RED); }

    void OnEnter()
    {
        g_pKeys->PopKey(KEY_RED);
        g_pLevel->m_pPlayer->StartEffect("door", m_iX, m_iY);
        g_pLevel->SetMapObj(m_iX, m_iY, GetObj(ID_PLATE));
		g_pLevel->GetScene()->GetEngine()->GetAudio()->PlaySample("pass");
    }
};


class CGateGreen : public CGObj
{
public:
    CGateGreen()
    {
        m_eID = ID_GATE_GREEN;
        m_pszName = "greengate";
    }
    BOOL IsCanEnter() { return g_pKeys->IsKeyExist(KEY_GREEN); }

    void OnEnter()
    {
        g_pKeys->PopKey(KEY_GREEN);
        g_pLevel->m_pPlayer->StartEffect("door", m_iX, m_iY);
        g_pLevel->SetMapObj(m_iX, m_iY, GetObj(ID_PLATE));
		g_pLevel->GetScene()->GetEngine()->GetAudio()->PlaySample("pass");
    }
};


class CClock : public CGObj
{
public:
    CClock()
    {
        m_eID = ID_CLOCK;
        m_pszName = "clock";
    }
    BOOL IsCanEnter() { return TRUE; }

    void OnEnter()
    {
        g_pGtm->SetTime(g_pGtm->GetTime() + TIME_BONUS_SEC);
        g_pLevel->m_pPlayer->StartEffect("clock", m_iX, m_iY);
        g_pLevel->SetMapObj(m_iX, m_iY, GetObj(ID_PLATE));
		g_pLevel->GetScene()->GetEngine()->GetAudio()->PlaySample("get");
    }
};


class CTeleport : public CGObj
{
public:
    CTeleport()
    {
        m_eID = ID_TELEPORT;
        m_pszName = "teleport";
    }
    BOOL IsCanEnter() { return TRUE; }

    void OnEnter()
    {
        g_pLevel->m_pPlayer->StartEffect("teleport", m_iX, m_iY);
        g_pLevel->TeleportPlayer(m_iX, m_iY);
		g_pLevel->GetScene()->GetEngine()->GetAudio()->PlaySample("pass");
    }
};


class CBigStone : public CGObj
{
public:
    CBigStone()
    {
        m_eID = ID_STONE_BIG;
        m_pszName = "bigstone";
    }
    BOOL IsCanEnter() { return FALSE; }
};


class CBigStoneRest : public CGObj
{
public:
    CBigStoneRest()
    {
        m_eID = ID_STONE_BIG_REST;
        m_pszName = "";
    }
    BOOL IsCanEnter() { return FALSE; }
};


class CStoneOld : public CGObj
{
public:
    CStoneOld()
    {
        m_eID = ID_STONE_OLD;
        m_pszName = "oldstone";
    }
    BOOL IsCanEnter() { return TRUE; }

    void OnExit()
    {
        g_pLevel->StartOldStoneEffect(m_iX, m_iY);
        g_pLevel->SetMapObj(m_iX, m_iY, NULL);
		g_pLevel->GetScene()->GetEngine()->GetAudio()->PlaySample("pass");
    }
};


class CEnter : public CGObj
{
public:
    CEnter()
    {
        m_eID = ID_ENTER;
        m_pszName = "enter";
    }
    BOOL IsCanEnter() { return TRUE; }
};


class CExit : public CGObj
{
public:
    CExit()
    {
        m_eID = ID_EXIT;
        m_pszName = "exit";
    }
    BOOL IsCanEnter() { return TRUE; }

    void OnEnter()
    {
        g_pLevel->m_pPlayer->StartExitAnim();
    }
};


class CArrow : public CGObj
{
protected:
    int m_iTX;
    int m_iTY;
    int m_iStep;

public:
    CArrow() {}

    BOOL IsLegalDir(MoveID eDir) 
    {
        if (m_eID == ID_ARROW_CROSS) return TRUE;
        if (m_eID == ID_ARROW_LEFT) {
            if (eDir == MOVE_LEFT) return TRUE;
        }
        if (m_eID == ID_ARROW_RIGHT) {
            if (eDir == MOVE_RIGHT) return TRUE;
        }
        if (m_eID == ID_ARROW_UP) {
            if (eDir == MOVE_UP) return TRUE;
        }
        if (m_eID == ID_ARROW_DOWN) {
            if (eDir == MOVE_DOWN) return TRUE;
        }
        if (m_eID == ID_ARROW_UPDOWN) {
            if (eDir == MOVE_UP || eDir == MOVE_DOWN) return TRUE;
        }
        if (m_eID == ID_ARROW_LEFTRIGHT) {
            if (eDir == MOVE_LEFT || eDir == MOVE_RIGHT) return TRUE;
        }
        return FALSE;
    }

    BOOL IsCanEnter() { return TRUE; }

    BOOL IsCanMove(MoveID eDir) 
    {
        int x, y;

        if (!IsLegalDir(eDir)) {
            return FALSE;
        }

        x = m_iX;
        y = m_iY;

        if (eDir == MOVE_UP) y--;
        if (eDir == MOVE_DOWN) y++;
        if (eDir == MOVE_LEFT) x--;
        if (eDir == MOVE_RIGHT) x++;

        if (g_pLevel->GetMapObj(x, y) != NULL)
            return FALSE;

        m_iTX = m_iX;
        m_iTY = m_iY;
        m_iStep = 0;

        return TRUE;
    }

    BOOL OnMoving(MoveID eDir)
    {
        int xd, yd;

        m_iStep += CELL_MOVE_DIF;

        if (m_iStep > MAP_CELL_SIZE) {
            m_iStep = 0;
            if (eDir == MOVE_UP) m_iTY--;
            if (eDir == MOVE_DOWN) m_iTY++;
            if (eDir == MOVE_LEFT) m_iTX--;
            if (eDir == MOVE_RIGHT) m_iTX++;
        }

        xd = yd = 0;

        if (eDir == MOVE_UP) yd = m_iStep;
        if (eDir == MOVE_DOWN) yd = -m_iStep;
        if (eDir == MOVE_LEFT) xd = -m_iStep;
        if (eDir == MOVE_RIGHT) xd = m_iStep;

        m_pSp->SetWXPos(g_pLevel->GetWXPos(m_iTX) + xd);
        m_pSp->SetWYPos(g_pLevel->GetWYPos(m_iTY) + yd);

        g_pLevel->m_pPlayer->m_pSp->SetWXPos(g_pLevel->GetWXPos(m_iTX) + xd);
        g_pLevel->m_pPlayer->m_pSp->SetWYPos(g_pLevel->GetWYPos(m_iTY) + yd);

        if (m_iStep > 0)
            return FALSE; // continue moving

        // check next cell
        if (m_iStep == 0) {
            int x = m_iTX;
            int y = m_iTY;

            if (eDir == MOVE_UP) y--;
            if (eDir == MOVE_DOWN) y++;
            if (eDir == MOVE_LEFT) x--;
            if (eDir == MOVE_RIGHT) x++;

            if (g_pLevel->GetMapObj(x, y) == NULL)
                return FALSE; // continue moving

            g_pLevel->m_map.SetElement(m_iX, m_iY, NULL);

            m_iX = m_iTX;
            m_iY = m_iTY;

            g_pLevel->m_map.SetElement(m_iX, m_iY, this);
        }
        return TRUE;
    }
};


class CArrowLeft : public CArrow
{
public:
    CArrowLeft()
    {
        m_eID = ID_ARROW_LEFT;
        m_pszName = "left";
    }
};


class CArrowUp : public CArrow
{
public:
    CArrowUp()
    {
        m_eID = ID_ARROW_UP;
        m_pszName = "up";
    }
};


class CArrowRight : public CArrow
{
public:
    CArrowRight()
    {
        m_eID = ID_ARROW_RIGHT;
        m_pszName = "right";
    }
};


class CArrowDown : public CArrow
{
public:
    CArrowDown()
    {
        m_eID = ID_ARROW_DOWN;
        m_pszName = "down";
    }
};


class CArrowCross : public CArrow
{
public:
    CArrowCross()
    {
        m_eID = ID_ARROW_CROSS;
        m_pszName = "cross";
    }
};


class CArrowLeftRight : public CArrow
{
public:
    CArrowLeftRight()
    {
        m_eID = ID_ARROW_LEFTRIGHT;
        m_pszName = "leftright";
    }
};


class CArrowUpDown : public CArrow
{
public:
    CArrowUpDown()
    {
        m_eID = ID_ARROW_UPDOWN;
        m_pszName = "updown";
    }
};

#endif // OBJECTS_H