
#include "listener.h"
#include "sprite.h"


class CPad: public CListener
{
public:
	CPad();

	void Init();
	BOOL ProcessMessage(E_MESSAGE eMessage, DWORD dwCurrTick);
	BOOL ProcessState(E_STATE sState, DWORD dwCurrTick);
	void master_new();
	void show_help(BOOL bIsShow);
	void show_nag();
	void show_dialoglw(BOOL bIsWin);
	void SetColor(int nColor);
	void PutColor(int nPos);

private:
	int  m_nChk1;
	int  m_nChk2;
	int  m_nChk3;
	int  m_LineC;
	int  m_flag[4][2];
	int  m_LineC_flag;
	int  m_random_num[4];
	int  m_cColor;
	BOOL m_bEndGame;
	
	CSprite* m_pFishki[10][4];
	CSprite* m_pAFishki[9][4];
	CSprite* m_pLnFrame[9];
	CSprite* m_pCursor;

private:
	void ProcessKey(LPARAM lParam, WPARAM wParam);
	void master_init();
	void randgen();
	BOOL color_check();
	void color_compare(int first);
	void color_search(int first);
	void color_search1(int first);
	BOOL color_result(int first);
	void line_ready();
	void restore_cursor();
	BOOL check(int num);
};

