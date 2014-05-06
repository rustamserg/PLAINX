// ui.cpp: implementation of the user interface classes.
//
//////////////////////////////////////////////////////////////////////

#include "plainx.h"
#include "ui.h"
#include "global_game.h"


///////////////////////////////////////////////////////////////////////////////
//
// FCell
//
///////////////////////////////////////////////////////////////////////////////
CFCell::CFCell()
{
	m_cFrame = 0;
	for (int i=0; i<6; i++) m_pFCell[i] = NULL;
}

//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
void CFCell::Init()
{
	char szName[MAX_BUFFER];

	for (int i=0; i<6; i++)
	{
		wsprintf(szName, "c%d", i + 1);
		m_pFCell[i] = GET_SPRITE(szName);
	}
}

//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
BOOL CFCell::ProcessMessage(E_MESSAGE eMessage, DWORD dwCurrTick)
{
	if (GET_ENGINE()->GetRTIntVar("game_state") != GS_GAME)
		return FALSE;

	switch (eMessage.dwMessageID) 
	{
	case EM_LMOUSEDOWN:
		for (int i=0; i<6; i++)
		{
			if (m_pFCell[i] && 
				m_pFCell[i]->IsInSprite(eMessage.lParam, eMessage.wParam))
			{
				m_cFrame = i;
				OnClick(eMessage.lParam, eMessage.wParam);
				break;
			}
		}
		break;
	}
	return FALSE;
}

//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
void CFCell::OnClick(int x, int y) 
{
  E_MESSAGE mes = {EM_KEYPRESSED, '1' + m_cFrame, 0};
  GET_LISTENER("mmpad")->ProcessMessage(mes, GET_ENGINE()->GetCurrTick());
}

////////////////////////////////////////////////////////////////////////////////
//
//  TCell
//
////////////////////////////////////////////////////////////////////////////////
CTCell::CTCell()
{
	m_cPos = 0;
	
	for (int i=0; i<4; i++)
	{
		for (int j=0; j<9; j++)
		{
			m_pTCell[i][j] = NULL;
		}	
	}
}

//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
void CTCell::Init()
{
	char szName[MAX_BUFFER];

	for (int j=0; j<4; j++)
	{
		for (int i=0; i<9; i++)
		{
			wsprintf(szName, "tc%d%d", i + 1, j + 1);
			m_pTCell[j][i] = GET_SPRITE(szName);
		}
	}
}

//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
BOOL CTCell::ProcessMessage(E_MESSAGE eMessage, DWORD dwCurrTick)
{
	if (GET_ENGINE()->GetRTIntVar("game_state") != GS_GAME)
		return FALSE;

	switch (eMessage.dwMessageID) 
	{
	case EM_LMOUSEUP:
		for (int i=0; i<4; i++)
		{
			for (int j=0; j<9; j++)
			{
				if (m_pTCell[i][j] && 
					(m_pTCell[i][j]->IsInSprite(eMessage.lParam, eMessage.wParam)))
				{
					m_cPos = i;
					OnClick(eMessage.lParam, eMessage.wParam);
					break;
				}
			}
		}
		break;
	}
	return FALSE;
}

//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
void CTCell::OnClick(int x, int y) 
{
  E_MESSAGE mes = {EM_KEYPRESSED, '7' + m_cPos, 0};
  if (m_cPos == 3) {
    mes.lParam = '0';
  }
  GET_LISTENER("mmpad")->ProcessMessage(mes, GET_ENGINE()->GetCurrTick());
}

////////////////////////////////////////////////////////////////////////////////
//
// Options
//
////////////////////////////////////////////////////////////////////////////////
COptions::COptions()
{
	m_bGrab = FALSE;
	m_bGrabSoundVol = FALSE;
	m_bDirty = FALSE;
	m_nX = 0;
}

//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
BOOL COptions::IsUnnamed()
{
	/*char* pszValue = (char*)CoTaskMemAlloc(MAX_BUFFER);
	GET_ENGINE()->GetVars("config")->GetString(L_GROUP, "reg", "regname", 0, &pszValue);
	
	BOOL bRes = FALSE;

	if (strcmp(pszValue, L_UNNAMED) == 0) 
	{
		GET_ENGINE()->GetScene("dlgnagopt")->SetVisible(TRUE);
		GET_ENGINE()->SetRTIntVar("game_state", GS_MODALDLG);
		ENABLE_LISTENER2("dlgnagopt", "dialog");
		bRes = TRUE;
	}

	CoTaskMemFree(pszValue);

	return bRes;*/
  return FALSE;
}

//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
BOOL COptions::ProcessMessage(E_MESSAGE eMessage, DWORD dwCurrTick)
{
	if (GET_ENGINE()->GetRTIntVar("game_state") != GS_OPTIONS)
		return FALSE;

	BOOL bVS;
	
	switch (eMessage.dwMessageID)
	{
	case EM_LMOUSEDOWN:
		if (GET_SPRITE("vol1")->IsInSprite(eMessage.lParam, eMessage.wParam) &&
			!GET_AUDIO()->GetMute())
		{
			m_bGrab = TRUE;
			m_bGrabSoundVol = TRUE;
			m_nX = eMessage.lParam;
		}

		if (GET_SPRITE("vol2")->IsInSprite(eMessage.lParam, eMessage.wParam) &&
			!GET_AUDIO()->GetMute())
		{
			m_bGrab = TRUE;
			m_bGrabSoundVol = FALSE;
			m_nX = eMessage.lParam;
		}

		//chk1
		bVS = GET_SPRITE("chk1")->GetVisible();
		GET_SPRITE("chk1")->SetVisible(TRUE); //for IsInSprite call
		if (GET_SPRITE("chk1")->IsInSprite(eMessage.lParam, eMessage.wParam) && 
			!IsUnnamed()) 
		{
			bVS = !bVS;
			m_bDirty = TRUE;
			GET_ENGINE()->SetRTIntVar("game_type_1", (int)bVS);
			GET_ENGINE()->GetVars("config")->SetInt(L_GROUP, "config", "chk1", 0, (int)bVS);
			GET_AUDIO()->PlaySample("put");
			if (bVS) 
			{
				GET_SPRITE("chk2")->SetVisible(TRUE);
				GET_ENGINE()->SetRTIntVar("game_type_2", 1);
				GET_ENGINE()->GetVars("config")->SetInt(L_GROUP, "config", "chk2", 0, 1);
			}
		}
		GET_SPRITE("chk1")->SetVisible(bVS);
		
		//chk2
		bVS = GET_SPRITE("chk2")->GetVisible();
		GET_SPRITE("chk2")->SetVisible(TRUE);
		if (GET_SPRITE("chk2")->IsInSprite(eMessage.lParam, eMessage.wParam) &&
			!IsUnnamed()) 
		{
			bVS = !bVS;
			m_bDirty = TRUE;
			GET_ENGINE()->SetRTIntVar("game_type_2", (int)bVS);
			GET_ENGINE()->GetVars("config")->SetInt(L_GROUP, "config", "chk2", 0, (int)bVS);
			GET_AUDIO()->PlaySample("put");
			if (!bVS) 
			{
				GET_SPRITE("chk1")->SetVisible(FALSE);
				GET_ENGINE()->SetRTIntVar("game_type_1", 0);
				GET_ENGINE()->GetVars("config")->SetInt(L_GROUP, "config", "chk1", 0, 0);
			}
		}
		GET_SPRITE("chk2")->SetVisible(bVS);

		//chk3
		bVS = GET_SPRITE("chk3")->GetVisible();
		GET_SPRITE("chk3")->SetVisible(TRUE);
		if (GET_SPRITE("chk3")->IsInSprite(eMessage.lParam, eMessage.wParam) &&
			!IsUnnamed()) 
		{
			bVS = !bVS;
			m_bDirty = TRUE;
			GET_ENGINE()->SetRTIntVar("game_type_3", (int)bVS);
			GET_ENGINE()->GetVars("config")->SetInt(L_GROUP, "config", "chk3", 0, (int)bVS);
			GET_AUDIO()->PlaySample("put");
		}
		GET_SPRITE("chk3")->SetVisible(bVS);
		break;
	case EM_MOUSEMOVE:
		if (m_bGrab)
		{
			int dx = eMessage.lParam - m_nX;
			int xc;
			if (m_bGrabSoundVol)
				xc = GET_SPRITE("vol1")->GetWXPos() + dx;
			else
				xc = GET_SPRITE("vol2")->GetWXPos() + dx;
			m_nX = eMessage.lParam;
			if ((xc > 73) && (xc < 350))
			{
				int nVol = xc - 90;
				int nVol2 = nVol;
				if (nVol2<0) nVol2 = 0;
				if (nVol2>255) nVol2 = 255;
				if (m_bGrabSoundVol)
				{
					GET_SPRITE("vol1")->SetWXPos(xc);
					GET_AUDIO()->SetSFXVolume(nVol2);
					GET_ENGINE()->GetVars("config")->SetInt(L_GROUP, "config", "soundvol", 0, nVol);
				}
				else
				{
					GET_SPRITE("vol2")->SetWXPos(xc);
					GET_AUDIO()->SetMusicVolume("back", nVol2);
					GET_ENGINE()->GetVars("config")->SetInt(L_GROUP, "config", "musicvol", 0, nVol);
				}
			}
		}
		break;
	case EM_LMOUSEUP:
		m_bGrab = FALSE;
		break;
	}
	return FALSE;
}


//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
BOOL COptions::ProcessState(E_STATE eState, DWORD dwCurrTick)
{
	int nVal = 0;
	
	switch (eState.dwStateID) 
	{
	case ES_OPTIONS_OPEN: //open
		if (GetScene()->GetCameraX()>10)
		{
			GetScene()->SetCameraX(GetScene()->GetCameraX() - 10);
			return FALSE;
		}
		GetScene()->SetCameraX(0);
		return TRUE;
	case ES_OPTIONS_CLOSE: //close
		if (GetScene()->GetCameraX()<390)
		{
			GetScene()->SetCameraX(GetScene()->GetCameraX() + 10);
			return FALSE;
		}
		GetScene()->SetCameraX(400);

		//show notify dialog for new game
		if (m_bDirty)
		{
			GET_ENGINE()->GetScene("dlgnewopt")->SetVisible(TRUE);
			GET_ENGINE()->SetRTIntVar("game_state", GS_MODALDLG);
			ENABLE_LISTENER2("dlgnewopt", "dialog");
		}
		else GET_ENGINE()->SetRTIntVar("game_state", GS_GAME);
		m_bDirty = FALSE;
		return TRUE;
	case ES_LISTENERINIT:
		GET_ENGINE()->GetVars("config")->GetInt(L_GROUP, "config", "soundvol", 0, &nVal);
		GET_SPRITE("vol1")->SetWXPos(90 + nVal);
		GET_ENGINE()->GetVars("config")->GetInt(L_GROUP, "config", "musicvol", 0, &nVal);
		GET_SPRITE("vol2")->SetWXPos(90 + nVal);
		GET_ENGINE()->GetVars("config")->GetInt(L_GROUP, "config", "chk1", 0, &nVal);
		GET_SPRITE("chk1")->SetVisible((BOOL)nVal);
		GET_ENGINE()->GetVars("config")->GetInt(L_GROUP, "config", "chk2", 0, &nVal);
		GET_SPRITE("chk2")->SetVisible((BOOL)nVal);
		GET_ENGINE()->GetVars("config")->GetInt(L_GROUP, "config", "chk3", 0, &nVal);
		GET_SPRITE("chk3")->SetVisible((BOOL)nVal);
		return TRUE;
	}
	return TRUE;
}

