// dialogs.cpp: implementation of the user interface classes.
//
//////////////////////////////////////////////////////////////////////

#include "plainx.h"
#include "dialogs.h"
#include "global_game.h"

#include <time.h>

///////////////////////////////////////////////////////////////////////
//
// Dialog support
//
//////////////////////////////////////////////////////////////////////
CDialog::CDialog()
{
	m_bGrab = FALSE;
	m_nX = 0;
	m_nY = 0;
	m_pOk = NULL;
	m_pCancel = NULL;
	m_pDialog = NULL;
}


//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
void CDialog::SetOk(const char* pszName)
{
	m_pOk = GET_SPRITE(pszName);
}


//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
void CDialog::SetCancel(const char* pszName)
{
	m_pCancel = GET_SPRITE(pszName);
}


//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
void CDialog::SetDialog(const char* pszName)
{
	m_pDialog = GET_SPRITE(pszName);
}


//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
BOOL CDialog::ProcessMessage(E_MESSAGE eMessage, DWORD dwCurrTick) 
{
	int dx, dy;
	RECT rc;
	
	switch (eMessage.dwMessageID) 
	{
	case EM_KEYPRESSED:
		if (eMessage.wParam == DIK_RETURN)
		{
			OnOk();
			return TRUE;
		}
		break;
	case EM_LMOUSEDOWN:
		m_nX = eMessage.lParam;
		m_nY = eMessage.wParam;
		if (m_pDialog && m_pDialog->IsInSprite(m_nX, m_nY)) 
		{
			m_bGrab = TRUE;
			return TRUE;
		}
		break;
	case EM_MOUSEMOVE:
		if (m_bGrab) 
		{
			dx = eMessage.lParam - m_nX;
			dy = eMessage.wParam - m_nY;
			rc = GetScene()->GetViewRect();
			GetScene()->DirtCamera();
			rc.left += dx;
			rc.right += dx;
			rc.bottom += dy;
			rc.top += dy;
			GetScene()->SetViewRect(&rc);
			m_nX = eMessage.lParam;
			m_nY = eMessage.wParam;
		}
		else
		{
			//handle ok
			if (m_pOk)
			{
				if (m_pOk->IsInSprite(eMessage.lParam, eMessage.wParam))
					m_pOk->SetFrame(1);
				else
					m_pOk->SetFrame(0);
			}

			//handle cancel
			if (m_pCancel)
			{
				if (m_pCancel->IsInSprite(eMessage.lParam, eMessage.wParam))
					m_pCancel->SetFrame(1);
				else
					m_pCancel->SetFrame(0);
			}
		}
		break;
	case EM_LMOUSEUP:
		m_bGrab = FALSE;
		if (m_pOk && m_pOk->GetFrame() == 1)
		{
			m_bGrab = FALSE;
			GET_AUDIO()->PlaySample("put");
			m_pOk->SetFrame(0);
			OnOk();
			return TRUE;
		}
		if (m_pCancel && m_pCancel->GetFrame() == 1)
		{
			m_bGrab = FALSE;
			GET_AUDIO()->PlaySample("put");
			m_pCancel->SetFrame(0);
			OnCancel();
			return TRUE;
		}
		break;
    }
	return FALSE;
}

//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
void CDialog::OnCancel()
{
	//custom code here
}

//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
void CDialog::OnOk()
{
	//custom code here
}

///////////////////////////////////////////////////////////////////////////////
//
// Lose&Win dialog 
//
///////////////////////////////////////////////////////////////////////////////
CDialogLW::CDialogLW()
{
}

//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
void CDialogLW::OnOk() 
{
	GetScene()->SetVisible(FALSE);
	GET_ENGINE()->SetRTIntVar("game_state", GS_GAME);
	Disable();
}

///////////////////////////////////////////////////////////////////////////////
//
// NagOpt dialog 
//
///////////////////////////////////////////////////////////////////////////////
CDialogNagOpt::CDialogNagOpt()
{
}

//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
void CDialogNagOpt::OnOk() 
{
	GetScene()->SetVisible(FALSE);
	GET_ENGINE()->SetRTIntVar("game_state", GS_OPTIONS);
	Disable();
}


///////////////////////////////////////////////////////////////////////////////
//
// Nag-screen dialog 
//
///////////////////////////////////////////////////////////////////////////////
CDialogNag::CDialogNag()
{
}

//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
BOOL CDialogNag::ProcessState(E_STATE sState, DWORD dwCurrTick)
{
	switch (sState.dwStateID)
	{
	case ES_DLGNAG_INIT:
		GET_ENGINE()->SetRTIntVar("die", 0);
					
		//get firsttime
		char szFT[MAX_BUFFER];
		GET_ENGINE()->GetVars("config")->GetString(L_GROUP, "reg", "firsttime", szFT);
		if (strcmp(szFT, "0") == 0)
		{
			time_t curt;
			time(&curt);
			_ltoa(curt, szFT, 10);
			GET_ENGINE()->GetVars("config")->SetString(L_GROUP, "reg", "firsttime", szFT);
		}
		
		//calculate days
		long lftime = atol(szFT);
		time_t curtime = 0;
		time(&curtime);
		int nDaysPass = (curtime - lftime)/86400;
		int nDaysMax = 0;
		GET_ENGINE()->GetVars("config")->GetInt(L_GROUP, "reg", "days", 0, &nDaysMax);
		int nDaysLeft = nDaysMax - nDaysPass;
		if (nDaysLeft <= 0) GET_ENGINE()->SetRTIntVar("die", 1);

		//show left days
		if (nDaysLeft < 0) nDaysLeft = 0;
		if (nDaysLeft > 99) nDaysLeft = 99;
			
		int nF0 = nDaysLeft/10;
		int nF1 = nDaysLeft%10;
		
		if (nF0>0) GET_SPRITE("dleft0")->SetFrame(nF0);
		else GET_SPRITE("dleft0")->SetVisible(FALSE);
		GET_SPRITE("dleft1")->SetFrame(nF1);				
		break;
	}
	return TRUE;
}

//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
void CDialogNag::OnOk()
{
	GetScene()->SetVisible(FALSE);
	GET_ENGINE()->SetRTIntVar("game_state", GS_GAME);
	Disable();
}
