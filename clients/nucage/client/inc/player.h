#ifndef PLAYER_H
#define PLAYER_H

#include "gglob.h"
#include "listener.h"


class CPlayer : public CListener
{
public:
    CPlayer();
    virtual ~CPlayer();

    BOOL ProcessMessage(E_MESSAGE eMessage, DWORD dwCurrTick);
    BOOL ProcessState(E_STATE sState, DWORD dwCurrTick);

    void StartEnterAnim();
    void StartExitAnim();
    void StartIdleAnim();
    void StartMoveAnim(MoveID eDir);
    void StartEffect(const char* pcszEffectName, int x, int y);

public:
    MoveID m_eDir;
    CSprite* m_pSp;
    PlStateID m_eState;
    int m_iMapXPos;
    int m_iMapYPos;
    BOOL m_bIsReal;
    char m_szName[100];

private:
    int m_iSpXPos;
    int m_iSpYPos;
    int m_iSpOldXPos;
    int m_iSpOldYPos;
};

#endif // PLAYER_H