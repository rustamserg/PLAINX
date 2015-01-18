#ifndef LEVEL_H
#define LEVEL_H

#include "listener.h"
#include "player.h"
#include "map.h"


class CLevel: public CListener
{
public:
    CLevel();
    virtual ~CLevel();

    BOOL ProcessMessage(E_MESSAGE eMessage, DWORD dwCurrTick);
    BOOL ProcessState(E_STATE sState, DWORD dwCurrTick);

public:
    int GetWXPos(int x);
    int GetWYPos(int y);
    CGObj* GetMapObj(int x, int y);
    void SetMapObj(int x, int y, CGObj* pObj);
    void TeleportPlayer(int xFrom, int yFrom);
    void StartOldStoneEffect(int x, int y);

private:
    void InitPlayer();
    int GetXPos(int nWX);
    int GetYPos(int nWY);
    void SetPlayer(int x, int y);
    void ClearLevel();
    void CreateLevel();
    const char* GetElemName(int x, int y);
	void UpdateConfig();
	void UpdateBackMusic();
	void EnterWorld();

public:
    CMap m_map;
    CPlayer* m_pPlayer;

private:
    char  m_szSkin[MAX_BUFFER];
    char  m_szBuf[MAX_BUFFER];
};

#endif // LEVEL_H 
