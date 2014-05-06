// game.cpp: implementation of game depended functions.
//
/////////////////////////////////////////////////////////////////////////

#include "plainx.h"
#include "game.h"
#include "ui.h"
#include "help.h"
#include "dialogs.h"
#include "global_game.h"

#include <stdlib.h>
#include <time.h>

CPad::CPad()
{
	for (int j=0; j<10; j++)
	{
		for (int i=0; i<4; i++)
		{
			m_pFishki[j][i] = NULL;
		}
	}
	for (int j=0; j<9; j++)
	{
		for (int i=0; i<4; i++)
		{
			m_pAFishki[j][i] = NULL;
		}
	}
	for (int j=0; j<9; j++) m_pLnFrame[j] = NULL;
	
	m_pCursor = NULL;
}

//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
void CPad::Init()
{
	char szName[MAX_BUFFER];
	
	for (int j=0; j<10; j++)
	{
		for (int i=1; i<=4; i++)
		{
			wsprintf(szName, "fishka%d%d", j, i);
			m_pFishki[j][i - 1] = GET_SPRITE2("fishki", szName);
		}
	}
	for (int j=1; j<10; j++)
	{
		for(int i=1; i<=4; i++)
		{
			wsprintf(szName, "afish%d%d", j, i);
			m_pAFishki[j - 1][i - 1] = GET_SPRITE2("afishki", szName);
		}
	}
	for (int j=0; j<9; j++)
	{
		wsprintf(szName, "lf%d", j + 1);
		m_pLnFrame[j] = GET_SPRITE2("lnframe", szName);
	}
	
	//remember cursor sprite
	m_pCursor = GET_ENGINE()->GetMouseSprite();
}

//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
BOOL CPad::ProcessState(E_STATE sState, DWORD dwCurrTick)
{
	switch (sState.dwStateID)
	{
	case ES_PAD_CLOSEHELP:
	  show_help(FALSE);
	  GET_ENGINE()->SetRTIntVar("game_state", GS_GAME);
	  show_nag();
	  break;
	case ES_PAD_RESTORECURSOR:
          restore_cursor();
          break;
        }
	return TRUE;
}


//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
BOOL CPad::ProcessMessage(E_MESSAGE eMessage, DWORD dwCurrTick)
{
	if (GET_ENGINE()->GetRTIntVar("game_state") != GS_GAME)
		return FALSE;

	switch(eMessage.dwMessageID)
	{
	case EM_KEYPRESSED:
		ProcessKey(eMessage.lParam, eMessage.wParam);
		break;
	case EM_RMOUSEUP:
		restore_cursor();
		break;
	}
	return FALSE;
}

//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
void CPad::SetColor(int nColor)
{
	m_cColor = nColor;
	GET_SPRITE2("masterdx_gui", "rotate")->SetRow(nColor);
	
	//update cursor
	if (m_pCursor)
	{
		m_pCursor->SetFrame(nColor + 1);
		POINT hs = {25, 11};
		m_pCursor->SetHotSpot(hs);
		m_pCursor->SetAlpha(100);
		m_pCursor->DirtSprite();
	}
	GET_AUDIO()->PlaySample("get");
}


//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
void CPad::PutColor(int nPos)
{
    if (m_pCursor && (m_pCursor->GetFrame()==0) && 
		(m_pFishki[m_LineC + 1][nPos]->GetVisible() == TRUE))
	{
			SetColor(m_pFishki[m_LineC + 1][nPos]->GetFrame());
			m_pFishki[m_LineC + 1][nPos]->SetVisible(FALSE);
			return;
	}
	else
	{
		if (m_nChk2 == 0)
		{
			for(int i=0; i<4; i++)
			{
				if (i == nPos)
					continue;
				if ((m_pFishki[m_LineC + 1][i]->GetVisible() == TRUE) && 
					(m_pFishki[m_LineC + 1][i]->GetFrame() == m_cColor))
					return;
			}
		}
	}
	m_pFishki[m_LineC + 1][nPos]->SetFrame(m_cColor);
	m_pFishki[m_LineC + 1][nPos]->SetVisible(TRUE);
	GET_AUDIO()->PlaySample("put");
	
	restore_cursor();
}

//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
void CPad::ProcessKey(LPARAM lParam, WPARAM wParam)
{
  if (lParam == 'm') {
    GET_AUDIO()->SetMute(!GET_AUDIO()->GetMute());
    GET_ENGINE()->GetVars("config")->SetInt(L_GROUP, "config", "mute", 0, GET_AUDIO()->GetMute());
  }
			
	if (wParam == DIK_F1)
	{
		if (GET_ENGINE()->GetRTIntVar("game_state") == GS_GAME)
		{
			GET_ENGINE()->SetRTIntVar("game_state", GS_HELP);
                        restore_cursor();
			show_help(TRUE);
		}
		return;
	}
	if (wParam == DIK_ESCAPE)
	{
		PostMessage(GET_ENGINE()->GetHWnd(), WM_CLOSE, 0, 0);
		return;
	}
	if (GET_ENGINE()->GetRTIntVar("die")) 
	{
		show_nag();
		return;
	}
	
	if(wParam == DIK_F2)
	{
		master_new();
		show_nag();
		return;
	}
	if (!m_bEndGame)
	{
          if (wParam == DIK_SPACE) 
          {
            line_ready();
          } 
          else 
          {
            switch(lParam) {
              case '1':
		SetColor(0);
		break;
	      case '2':
	        SetColor(1);
		break;
	      case '3':
	        SetColor(2);
		break;
	      case '4':
	        SetColor(3);
		break;
	      case '5':
	        SetColor(4);
		break;
	      case '6':
	        SetColor(5);
		break;
	      case '7':
	        PutColor(0);
		break;
	      case '8':
	        PutColor(1);
		break;
	      case '9':
	        PutColor(2);
		break;
	      case '0':
	        PutColor(3);
		break;
	    }
          }
	}
}

//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
void CPad::line_ready()
{
	if (m_nChk3==0)
	{
		for (int i=0; i<4; i++)
		{
			if (!(m_pFishki[m_LineC + 1][i]->GetVisible()))
				return;
		}
	}
	
	GET_AUDIO()->PlaySample("check");

	for (int i=0; i<4; i++)
	{
		m_flag[i][0] = 0;
		m_flag[i][1] = 0;
	}
	if (color_check())
	{
		GET_SPRITE2("kozyirek", "kozyirek")->SetVisible(FALSE);
		for (int i=0; i<4; i++)
		{
			m_pFishki[0][i]->SetFrame(m_random_num[i]);
			m_pFishki[0][i]->SetVisible(TRUE);
		}
		show_dialoglw(TRUE);
		GET_ENGINE()->SetRTIntVar("game_state", GS_MODALDLG);
		m_bEndGame = TRUE;
	}
	else
	{
		m_pLnFrame[m_LineC]->SetVisible(FALSE);
		m_LineC++;
		if (m_LineC < 9)
			m_pLnFrame[m_LineC]->SetVisible(TRUE);

		if (m_LineC >= 9)
		{
			GET_SPRITE2("kozyirek", "kozyirek")->SetVisible(FALSE);
			for (int i=0; i<4; i++)
			{
				m_pFishki[0][i]->SetFrame(m_random_num[i]);
				m_pFishki[0][i]->SetVisible(TRUE);
			}
			show_dialoglw(FALSE);
			GET_ENGINE()->SetRTIntVar("game_state", GS_MODALDLG);
			m_bEndGame = TRUE;
		}
	}
}

//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
void CPad::master_init()
{
	m_LineC = 0;
	m_bEndGame = FALSE;
	CSprite* pCursor = NULL;

	GET_SPRITE2("kozyirek", "kozyirek")->SetVisible(TRUE);
	
	restore_cursor();
	
	for (int i=0; i<4; i++)
	{
		for (int j=0; j<9; j++)
		{
			m_pFishki[j + 1][i]->SetVisible(FALSE);
			m_pAFishki[j][i]->SetVisible(FALSE);
		}
	}

	m_pLnFrame[0]->SetVisible(TRUE);
	for (int i=1; i<9; i++) m_pLnFrame[i]->SetVisible(FALSE);
	
	for (int i=0; i<4; i++) m_random_num[i] = -1;
	
	//set game_state to game
	GET_ENGINE()->SetRTIntVar("game_state", GS_GAME);
	
	//read game_type vars
	int nGT;
	GET_ENGINE()->GetVars("config")->GetInt(L_GROUP, "config", "chk1", 0, &nGT);
	GET_ENGINE()->SetRTIntVar("game_type_1", nGT);
	m_nChk1=nGT;
	GET_ENGINE()->GetVars("config")->GetInt(L_GROUP, "config", "chk2", 0, &nGT);
	GET_ENGINE()->SetRTIntVar("game_type_2", nGT);
	m_nChk2=nGT;
	GET_ENGINE()->GetVars("config")->GetInt(L_GROUP, "config", "chk3", 0, &nGT);
	GET_ENGINE()->SetRTIntVar("game_type_3", nGT);
	m_nChk3=nGT;

	
	//check if first run
	int nFRun = 0;
	GET_ENGINE()->GetVars("config")->GetInt(L_GROUP, "config", "firstrun", 0, &nFRun);
	if (nFRun) //if first run we show help
	{
		show_help(TRUE);
		GET_ENGINE()->SetRTIntVar("game_state", 2);
		GET_ENGINE()->GetVars("config")->SetInt(L_GROUP, "config", "firstrun", 0, 0);
	}
	else show_nag();
}

//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
void CPad::show_dialoglw(BOOL bIsWin)
{
	GET_ENGINE()->GetScene("dlglw")->SetVisible(TRUE);
	if (bIsWin)
		GET_SPRITE2("dlglw", "lose")->SetVisible(FALSE);
	else
		GET_SPRITE2("dlglw", "win")->SetVisible(FALSE);
	ENABLE_LISTENER2("dlglw", "dialog");
}

//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
void CPad::show_help(BOOL bIsShow)
{
	if (bIsShow)
	{
		//load data
		GET_ENGINE()->AddResource("data\\help.dat", "help");

		//load scene
		CHelpListener* pHelpListener = new CHelpListener();
		GET_ENGINE()->AddScene("scene\\help.scn", "help", "help");
		GET_ENGINE()->GetScene("help")->AddListener(pHelpListener, "helplistener");
		
		GET_SPRITE2("help", "help01")->SetVisible(TRUE);
		GET_SPRITE2("help", "arrowr")->SetVisible(TRUE);
		GET_SPRITE2("help", "cross")->SetVisible(TRUE);
		GET_SPRITE2("masterdx_gui", "rotate")->SetVisible(FALSE);
		GET_SPRITE2("masterdx_gui", "check")->SetVisible(FALSE);
		GET_SPRITE2("masterdx_gui", "new")->SetVisible(FALSE);
		GET_ENGINE()->GetScene("selframe")->SetVisible(FALSE);
		GET_ENGINE()->GetScene("dlglw")->SetVisible(FALSE);
	}
	else
	{
		//free scene
		GET_ENGINE()->DelScene("help");
		GET_ENGINE()->DelResource("help");
		GET_SPRITE2("masterdx_gui", "rotate")->SetVisible(TRUE);
		GET_SPRITE2("masterdx_gui", "check")->SetVisible(TRUE);
		GET_SPRITE2("masterdx_gui", "new")->SetVisible(TRUE);
		GET_ENGINE()->GetScene("selframe")->SetVisible(TRUE);
		GetScene()->DirtCamera();
	}
}

//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
void CPad::master_new()
{
	master_init();
	randgen();

	for (int i=0; i<4; i++)
	{
		m_pFishki[0][i]->SetFrame(m_random_num[i]);
		m_pFishki[0][i]->SetVisible(FALSE);
	}
	GET_ENGINE()->GetScene("dlglw")->SetVisible(FALSE);
	GET_AUDIO()->PlaySample("start");
}

//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
void CPad::show_nag()
{
	//if not registered show nag-screen
	int nV;
	GET_ENGINE()->GetVars()->GetInt(L_GROUP, "reg", "reged", 0, &nV);
	if (!nV)
	{
		GET_ENGINE()->GetScene("dlgnag")->SetVisible(TRUE);
		GET_ENGINE()->SetRTIntVar("game_state", GS_MODALDLG);
		ENABLE_LISTENER2("dlgnag", "dialog");
		GET_ENGINE()->PostState(ES_DLGNAG_INIT, 0, 0, "dlgnag");
	}
}

//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
BOOL CPad::check(int num)
{
	BOOL bRes = TRUE;

	for (int i=0; i<4; i++)
	{
		if (m_random_num[i] == num)
		{
			bRes = FALSE;
			break;
		}
	}
	return bRes;
}

//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
void CPad::randgen()
{
	int num;
	srand( (unsigned)time( NULL ) );
	rand();

	if (m_nChk1 == 0)
	{
		for (int i=0; i<4; i++)
		{
			do
			{
				num = int(6*rand()/RAND_MAX);
			}
			while (!check(num));
			m_random_num[i] = num;
		}
	}
	else
	{
		for (int i=0; i<4; i++)
			m_random_num[i] = int(6*rand()/RAND_MAX);
	}
}


//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
BOOL CPad::color_check()
{
	srand((unsigned)time(NULL));
	int first = int(4*rand()/RAND_MAX);
	color_compare(first);
	color_search(first);
	return color_result(first);
}


//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
void CPad::color_compare(int first)
{
	for (int i=first; i<=first + 3; i++)
	{
		int k = i % 4;
		if ((m_pFishki[m_LineC + 1][k]->GetVisible()) && 
			(m_pFishki[m_LineC + 1][k]->GetFrame() == m_random_num[k]))
		{
			m_flag[k][0] = 1;
			m_flag[k][1] = 1;
		}
	}
}


//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
void CPad::color_search(int first)
{
	int k;

	for (int i=first; i<=first + 3; i++)
	{
		k = (i > 3) ? i - 4 : i;
		if (m_flag[k][0] == 0) color_search1(k);
	}
}


//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
void CPad::color_search1(int first)
{
	int k;

	for(int i=first;i<=first+3;i++)
	{
		k = (i > 3) ? i - 4 : i;
		if (m_flag[k][1] == 0)
		{
			if ((m_pFishki[m_LineC + 1][first]->GetVisible()) &&
				(m_pFishki[m_LineC + 1][first]->GetFrame() == m_random_num[k]))
			{
				m_flag[k][1] = 2;
				m_flag[first][0] = 2;
				break;
			}
		}
	}
}


//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
BOOL CPad::color_result(int first)
{
	int i, j, m=0, ret=0;

	for (i=first; i<=first + 3; i++)
	{
		j = (i > 3) ? i - 4 : i;
		if (m_flag[j][0] != 0)
		{
			if (m_flag[j][1] == 1)
			{
				m_pAFishki[m_LineC][m]->SetFrame(1);
				ret++;
			}
			else
			{
				m_pAFishki[m_LineC][m]->SetFrame(0);
			}
			m_pAFishki[m_LineC][m]->SetVisible(TRUE);
			m++;
		}
	}

	if (ret == 4)
		return TRUE;
	else
		return FALSE;
}

//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
void CPad::restore_cursor()
{
	if (m_pCursor) 
	{
		POINT hs = {2, 48};
		m_pCursor->SetFrame(0);
		m_pCursor->SetHotSpot(hs);
		m_pCursor->SetAlpha(0);
		m_pCursor->DirtSprite();
	}
}


