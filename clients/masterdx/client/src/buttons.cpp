// buttons.cpp: implementation of the user interface classes.
//
//////////////////////////////////////////////////////////////////////

#include "plainx.h"
#include "buttons.h"
#include "global_game.h"


//////////////////////////////////////////////////////////////////////
//
//	Class for button support
//
//////////////////////////////////////////////////////////////////////
CButton::CButton()
{
	m_bSetLight = FALSE;
	m_pButton = NULL;
}

CButton::~CButton()
{
}

//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
void CButton::SetButton(const char* pszName)
{
	m_pButton = GET_SPRITE(pszName);
}

//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
BOOL CButton::ProcessMessage(E_MESSAGE eMessage, DWORD dwCurrTick)
{
	if (!m_pButton) return FALSE;

	switch (eMessage.dwMessageID) 
	{
	case EM_MOUSEMOVE:
		if (m_pButton->IsInSprite(eMessage.lParam, eMessage.wParam)) 
		{
			if (!m_bSetLight) 
			{
				m_bSetLight = TRUE;
				m_pButton->SetFrame(1);
			}
		}
		else 
		{
			if (m_bSetLight) 
			{
				m_bSetLight = FALSE;
				m_pButton->SetFrame(0);
			}
		}
		break;
	case EM_LMOUSEDOWN:
		if (m_bSetLight) 
			m_pButton->SetFrame(2);
		break;
	case EM_LMOUSEUP:
		if (m_bSetLight && m_pButton->GetFrame() == 2) 
		{
			m_pButton->SetFrame(1);
			OnClick(eMessage.lParam, eMessage.wParam);
		}
		break;
	}
	return FALSE;
}


//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
void CButton::OnClick(int x, int y) 
{
	//custom code here
}


///////////////////////////////////////////////////////////////////////////////
//
// Options button
//
///////////////////////////////////////////////////////////////////////////////
COptBut::COptBut()
{
	m_bIsOpen = FALSE;
}

//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
BOOL COptBut::ProcessMessage(E_MESSAGE eMessage, DWORD dwCurrTick)
{
	int nGS = GET_ENGINE()->GetRTIntVar("game_state");
	if ((nGS != GS_GAME) && (nGS != GS_OPTIONS)) return FALSE;
	
	if (!m_pButton) return FALSE;

	switch (eMessage.dwMessageID) 
	{
	case EM_MOUSEMOVE:
		if (m_pButton->IsInSprite(eMessage.lParam, eMessage.wParam)) 
		{
			if (!m_bSetLight) 
			{
				m_bSetLight = TRUE;
				if(m_bIsOpen)
					m_pButton->SetFrame(2);
				else
					m_pButton->SetFrame(1);
			}
		}
		else 
		{
			if (m_bIsOpen)
				m_pButton->SetFrame(2);
			else
			{
				m_pButton->SetFrame(0);
				m_bSetLight = FALSE;
			}
		}
		break;
	case EM_LMOUSEDOWN:
		if (m_pButton->IsInSprite(eMessage.lParam, eMessage.wParam) && m_bSetLight) 
		{
			if (m_pButton->GetFrame()==2)
				m_pButton->SetFrame(1);
			if (m_pButton->GetFrame()==1)
				m_pButton->SetFrame(2);
		}
		break;
	case EM_LMOUSEUP:
		if (m_pButton->IsInSprite(eMessage.lParam, eMessage.wParam) && 
			m_bSetLight && (m_pButton->GetFrame() == 2)) 
		{
			OnClick(eMessage.lParam, eMessage.wParam);
		}
		break;
	}
	return FALSE;
}

//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
void COptBut::OnClick(int x, int y) 
{		
	m_bIsOpen = !(GET_SCENE("options")->GetCameraX() == 400);
	if (!m_bIsOpen)
	{
		GET_ENGINE()->PostState(ES_OPTIONS_OPEN, 0, 0, "options");
		GET_ENGINE()->SetRTIntVar("game_state", GS_OPTIONS);
		m_pButton->SetFrame(2);
                GET_ENGINE()->PostState(ES_PAD_RESTORECURSOR, 0, 0, "mmpad");
		m_bIsOpen = TRUE;
	}
	else
	{
		GET_ENGINE()->PostState(ES_OPTIONS_CLOSE, 0, 0, "options");
		m_pButton->SetFrame(1);
		m_bIsOpen = FALSE;
	}
}


///////////////////////////////////////////////////////////////////////////////
//
// Check row button
//
///////////////////////////////////////////////////////////////////////////////
CCheck::CCheck()
{
}

//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
void CCheck::OnClick(int x, int y)
{
	E_MESSAGE mes = {EM_KEYPRESSED, ASCII_NULL_CODE, DIK_SPACE};
	GET_LISTENER2("mmpad", "mmpad")->ProcessMessage(mes, GET_ENGINE()->GetCurrTick());
}

///////////////////////////////////////////////////////////////////////////////
//
// New game button
//
///////////////////////////////////////////////////////////////////////////////
CNewBtn::CNewBtn()
{
}

//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
void CNewBtn::OnClick(int x, int y)
{
	E_MESSAGE mes = {EM_KEYPRESSED, ASCII_NULL_CODE, DIK_F2};
	GET_LISTENER2("mmpad", "mmpad")->ProcessMessage(mes, GET_ENGINE()->GetCurrTick());
}


///////////////////////////////////////////////////////////////////////////////
//
// Close game button
//
///////////////////////////////////////////////////////////////////////////////
CCloseBut::CCloseBut()
{
}

//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
void CCloseBut::OnClick(int x, int y)
{
	PostMessage(GET_ENGINE()->GetHWnd(), WM_CLOSE, 0, 0);
}

///////////////////////////////////////////////////////////////////////////////
//
// Minimize game button
//
///////////////////////////////////////////////////////////////////////////////
CMinBut::CMinBut()
{
}

//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
void CMinBut::OnClick(int x, int y)
{
	PostMessage(GET_ENGINE()->GetHWnd(), WM_SYSCOMMAND, SC_MINIMIZE, 0);
}