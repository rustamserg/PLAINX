// buttons.cpp: implementation of the user interface classes.
//
//////////////////////////////////////////////////////////////////////

#include "plainx.h"
#include "buttons.h"
#include "gtm.h"
#include "gmenu.h"
#include "level.h"
#include "help.h"
#include "options.h"
#include "gglob.h"

extern CGameMenu* g_pMenu;
extern CGameTime* g_pGtm;
extern CLevel* g_pLevel;
extern CHelpDlg* g_pHelp;
extern COptionsDlg* g_pOptions;

//////////////////////////////////////////////////////////////////////
//
// Class for button support
//
//////////////////////////////////////////////////////////////////////
CButton::CButton() :
CListener()
{
    m_bSetLight = FALSE;
    m_pButton = NULL;
    m_iCaptionId = -1;
}


CButton::~CButton()
{
}


void CButton::InitButton(const char* pszSpriteName,
                         const char* pcszResName,
                         const char* pcszFontName)
{
    m_pButton = GetScene()->CloneSprite(pszSpriteName);
    m_pButton->SetZPos(0);
    m_pButton->SetFrame(0);
    m_pButton->SetVisible(FALSE);

    m_iCaptionId = GET_STRING()->CreateFromTable(pcszResName, pcszFontName, 0, 0, 0);
}


void CButton::DrawButton(int wx, int wy, int wz)
{
    m_pButton->SetWPos(wx, wy, wz);
    GET_STRING()->SetPos(m_iCaptionId, wx + 20, wy + 7, wz + 1);
    UnHideButton();
}


void CButton::HideButton()
{
    m_pButton->SetVisible(FALSE);
    GET_STRING()->SetVisible(FALSE, m_iCaptionId);
}


void CButton::UnHideButton()
{
    m_pButton->SetVisible(TRUE);
    GET_STRING()->SetVisible(TRUE, m_iCaptionId);
}


//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
BOOL CButton::ProcessMessage(E_MESSAGE eMessage, DWORD dwCurrTick)
{
    if (!m_pButton)
        return FALSE;

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
            GET_AUDIO()->PlaySample("click");
			OnClick();
        }
        break;
    }
    return FALSE;
}


//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
void CButton::OnClick()
{
    //custom code here
}



///////////////////////////////////////////////////////////////////////////////
//
// Play game buttons
//
///////////////////////////////////////////////////////////////////////////////
CPlayLevel::CPlayLevel() :
CButton()
{
}

void CPlayLevel::OnClick()
{
    g_pMenu->DrawStartLevel();
}


///////////////////////////////////////////////////////////////////////////////
CRetryLevel::CRetryLevel() :
CButton()
{
}

void CRetryLevel::OnClick()
{
	int iGS = GET_ENGINE()->GetRTIntVar(NA_RT_GAME_STATE);

	if (iGS == GS_GAME) {
		g_pLevel->PostState(ES_LEVEL_RESTART, 0, 0);
	} else {
		g_pMenu->DrawStartLevel();
	}
}


///////////////////////////////////////////////////////////////////////////////
CNextBut::CNextBut() :
CButton()
{
}

void CNextBut::OnClick()
{
	int iGS;

	iGS = GET_ENGINE()->GetRTIntVar(NA_RT_GAME_STATE);

	if (iGS == GS_INFRA) {
		g_pHelp->DrawAuthors();
	} else {
		g_pMenu->DrawStartLevel();
	}
}


///////////////////////////////////////////////////////////////////////////////
CBackToMenu::CBackToMenu() :
CButton()
{
}

void CBackToMenu::OnClick()
{
	g_pGtm->StopTimer();
    g_pMenu->DrawStartMenu();
}


///////////////////////////////////////////////////////////////////////////////
CQuitMenu::CQuitMenu() :
CButton()
{
}

void CQuitMenu::OnClick()
{
    PostMessage(GET_ENGINE()->GetHWnd(), WM_CLOSE, 0, 0);
}

///////////////////////////////////////////////////////////////////////////////
CNextMiniLevel::CNextMiniLevel() :
CButton()
{
}

void CNextMiniLevel::OnClick()
{
	g_pMenu->NextMiniLevel();
}

///////////////////////////////////////////////////////////////////////////////
CPrevMiniLevel::CPrevMiniLevel() :
CButton()
{
}

void CPrevMiniLevel::OnClick()
{
	g_pMenu->PrevMiniLevel();
}

///////////////////////////////////////////////////////////////////////////////
CNextWorld::CNextWorld() :
CButton()
{
}

void CNextWorld::OnClick()
{
	g_pMenu->NextWorld();
}

///////////////////////////////////////////////////////////////////////////////
CPrevWorld::CPrevWorld() :
CButton()
{
}

void CPrevWorld::OnClick()
{
	g_pMenu->PrevWorld();
}

///////////////////////////////////////////////////////////////////////////////
COptionsBut::COptionsBut() :
CButton()
{
}

void COptionsBut::OnClick()
{
	g_pOptions->DrawOptions();
}

///////////////////////////////////////////////////////////////////////////////
CHelpBut::CHelpBut() :
CButton()
{
}

void CHelpBut::OnClick()
{
	g_pHelp->DrawHelp();
}

///////////////////////////////////////////////////////////////////////////////
COKBut::COKBut() :
CButton()
{
}

void COKBut::OnClick()
{
	int iGS = GET_ENGINE()->GetRTIntVar(NA_RT_GAME_STATE);

	if (iGS == GS_MENU) {
		g_pMenu->DrawWorldIntroEnd();
	} else {
		g_pMenu->DrawStartMenu();
	}
}

///////////////////////////////////////////////////////////////////////////////
CMusicUpBut::CMusicUpBut() :
CButton()
{
}

void CMusicUpBut::OnClick()
{
	g_pOptions->MusicUp();
}


///////////////////////////////////////////////////////////////////////////////
CMusicDownBut::CMusicDownBut() :
CButton()
{
}

void CMusicDownBut::OnClick()
{
	g_pOptions->MusicDown();
}

///////////////////////////////////////////////////////////////////////////////
CSoundUpBut::CSoundUpBut() :
CButton()
{
}

void CSoundUpBut::OnClick()
{
	g_pOptions->SoundUp();
}

///////////////////////////////////////////////////////////////////////////////
CSoundDownBut::CSoundDownBut() :
CButton()
{
}

void CSoundDownBut::OnClick()
{
	g_pOptions->SoundDown();
}
