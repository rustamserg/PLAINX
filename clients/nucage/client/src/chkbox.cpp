// chkbox.cpp: implementation of the user interface classes.
//
//////////////////////////////////////////////////////////////////////

#include "plainx.h"
#include "chkbox.h"
#include "options.h"
#include "gmenu.h"
#include "gglob.h"

extern COptionsDlg* g_pOptions;

//////////////////////////////////////////////////////////////////////
//
// Class for check box support
//
//////////////////////////////////////////////////////////////////////
CCheckBox::CCheckBox() :
CListener()
{
    m_fChecked = FALSE;
    m_pCheckBox = NULL;
    m_iLabelId = -1;
}


CCheckBox::~CCheckBox()
{
}


void CCheckBox::InitCheckBox(const char* pszSpriteName,
							 const char* pcszResName,
							 const char* pcszFontName)
{
    m_pCheckBox = GetScene()->CloneSprite(pszSpriteName);
    m_pCheckBox->SetZPos(0);
    m_pCheckBox->SetFrame(0);
    m_pCheckBox->SetVisible(FALSE);

    m_iLabelId = GET_STRING()->CreateFromTable(pcszResName, pcszFontName, 0, 0, 0);
}


void CCheckBox::DrawCheckBox(int wx, int wy, int wz)
{
    m_pCheckBox->SetWPos(wx, wy, wz);
    GET_STRING()->SetPos(m_iLabelId, wx + 25, wy - 3, wz);
    UnHideCheckBox();
}


void CCheckBox::HideCheckBox()
{
    m_pCheckBox->SetVisible(FALSE);
    GET_STRING()->SetVisible(FALSE, m_iLabelId);
}


void CCheckBox::UnHideCheckBox()
{
    m_pCheckBox->SetVisible(TRUE);
    GET_STRING()->SetVisible(TRUE, m_iLabelId);
}

void CCheckBox::SetState(BOOL fChecked)
{
	m_fChecked = fChecked;
	if (m_fChecked)
		m_pCheckBox->SetFrame(1);
	else
		m_pCheckBox->SetFrame(0);
}

//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
BOOL CCheckBox::ProcessMessage(E_MESSAGE eMessage, DWORD dwCurrTick)
{
    if (!m_pCheckBox)
        return FALSE;

    switch (eMessage.dwMessageID) 
    {
    case EM_LMOUSEUP:
        if (m_pCheckBox->IsInSprite(eMessage.lParam, eMessage.wParam)) {
			m_fChecked = !m_fChecked;
			if (m_fChecked)
				m_pCheckBox->SetFrame(1);
			else
				m_pCheckBox->SetFrame(0);
			GET_AUDIO()->PlaySample("click");
			OnClick(m_fChecked);
		}
		break;
	}
	return FALSE;
}


//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
void CCheckBox::OnClick(BOOL fChecked)
{
    //custom code here
}



///////////////////////////////////////////////////////////////////////////////
//
// Game check boxes
//
///////////////////////////////////////////////////////////////////////////////
CFullScreen::CFullScreen() :
CCheckBox()
{
}

void CFullScreen::OnClick(BOOL fChecked)
{
	PostMessage(GET_ENGINE()->GetHWnd(), WM_SYSKEYDOWN, VK_RETURN, 0);
}


///////////////////////////////////////////////////////////////////////////////
CMuteAllSnd::CMuteAllSnd() :
CCheckBox()
{
}

void CMuteAllSnd::OnClick(BOOL fChecked)
{
	GET_AUDIO()->SetMute(fChecked);
	GET_ENGINE()->GetVars("config")->SetInt(L_GROUP, "config", "mute", 0, fChecked);

	if (!fChecked) {
		GET_AUDIO()->PlayMusic(GET_ENGINE()->GetRTStringVar(NA_RT_BACK_MUSIC));
	}
}

///////////////////////////////////////////////////////////////////////////////
CRusLng::CRusLng() :
CCheckBox()
{
}

void CRusLng::OnClick(BOOL fChecked)
{
	if (fChecked) {
		g_pOptions->SetRusLng();
	} else {
		g_pOptions->SetEngLng();
	}
	GetScene()->DirtCamera();
}


///////////////////////////////////////////////////////////////////////////////
CEngLng::CEngLng() :
CCheckBox()
{
}

void CEngLng::OnClick(BOOL fChecked)
{
	if (fChecked) {
		g_pOptions->SetEngLng();
	} else {
		g_pOptions->SetRusLng();
	}
	GetScene()->DirtCamera();
}
