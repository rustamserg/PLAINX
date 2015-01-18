#ifndef GMENU_H
#define GMENU_H

#include "listener.h"
#include "buttons.h"
#include "gglob.h"


class CGameMenu: public CListener
{
public:
    CGameMenu();
    virtual ~CGameMenu();

    BOOL ProcessState(E_STATE sState, DWORD dwCurrTick);

public:
	void DrawStartMenu();
	void NextMiniLevel();
	void PrevMiniLevel();
	void NextWorld();
	void PrevWorld();
	void DrawStartLevel();
    void DrawFailedLevel();
    void DrawSuccessedLevel();
	void DrawWorldIntroStart(int iWorldId);
	void DrawWorldIntroEnd();
	void DrawWinScreen();
	void FixLevelLabel(BOOL fRusLng);

private:
	void DrawMiniLevel(int iLevel);
	const char* GetMiniElemName(int x, int y);
	void InitLng();

private:
    CPlayLevel* m_pPLBut;
    CRetryLevel* m_pRLBut;
    CNextBut* m_pNLBut;
    CBackToMenu* m_pBTMBut;
    CQuitMenu* m_pQBut;
	CNextMiniLevel* m_pNextLvl;
	CPrevMiniLevel* m_pPrevLvl;
	CNextWorld* m_pNextWorld;
	CPrevWorld* m_pPrevWorld;
	COptionsBut* m_pOptBut;
	CHelpBut* m_pHelpBut;
	COKBut* m_pOKBut;
	char m_szName[100];
	int m_iWorldId;
	int m_iLevelId;
	int m_iLevelValueId;
	int m_iLockedId;
	int m_iTryAgainId;
	int m_iSuccessId;
	int m_iWinId;
	int m_iWorldDesc[NA_WORLD_DESC_NUM_LINES];
};

#endif // LEVEL_H 
