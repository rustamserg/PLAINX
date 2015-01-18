#include "plainx.h"
#include "gglob.h"
#include "buttons.h"
#include "chkbox.h"
#include "gmenu.h"
#include "options.h"

extern CGameMenu* g_pMenu;

COptionsDlg::COptionsDlg() :
CListener()
{
	m_pOKBut = NULL;
	m_pFullScr = NULL;
	m_pMuteAll = NULL;
	m_pRusLng = NULL;
	m_pEngLng = NULL;
	m_pMusicUp = NULL;
	m_pMusicDown = NULL;
	m_pSoundUp = NULL;
	m_pSoundDown = NULL;
}


COptionsDlg::~COptionsDlg()
{
}


BOOL COptionsDlg::ProcessState(E_STATE sState, DWORD dwCurrTick)
{
    switch (sState.dwStateID) {
    case ES_LISTENERINIT:
		m_pOKBut = new COKBut();
		m_pMusicUp = new CMusicUpBut();
		m_pMusicDown = new CMusicDownBut();
		m_pSoundUp = new CSoundUpBut();
		m_pSoundDown = new CSoundDownBut();

		m_pFullScr = new CFullScreen();
		m_pMuteAll = new CMuteAllSnd();
		m_pRusLng = new CRusLng();
		m_pEngLng = new CEngLng();

		GetScene()->AddListener(m_pOKBut, "okoptbut");
		GetScene()->AddListener(m_pMusicUp, "mupoptbut");
		GetScene()->AddListener(m_pMusicDown, "mdoptbut");
		GetScene()->AddListener(m_pSoundUp, "suoptbut");
		GetScene()->AddListener(m_pSoundDown, "sdoptbut");
		GetScene()->AddListener(m_pFullScr, "fsoptchk");
		GetScene()->AddListener(m_pMuteAll, "maoptchk");
		GetScene()->AddListener(m_pRusLng, "rusoptchk");
		GetScene()->AddListener(m_pEngLng, "engoptchk");

		m_pOKBut->InitButton("button", "but8", "buttonfnt");
		m_pMusicUp->InitButton("rarbutt", "", "buttonfnt");
		m_pMusicDown->InitButton("larbutt", "", "buttonfnt");
		m_pSoundUp->InitButton("rarbutt", "", "buttonfnt");
		m_pSoundDown->InitButton("larbutt", "", "buttonfnt");

		m_pFullScr->InitCheckBox("chkbox", "opt1", "greenfnt");
		m_pMuteAll->InitCheckBox("chkbox", "opt2", "greenfnt");
		m_pRusLng->InitCheckBox("chkbox", "opt3", "greenfnt");
		m_pEngLng->InitCheckBox("chkbox", "opt4", "greenfnt");

		m_pEngLng->SetState(TRUE);

		m_iOptionsId = GET_STRING()->CreateFromTable("opt", "labelfnt", 250, 440, 8);
		m_iMusicVolId = GET_STRING()->CreateFromTable("opt5", "greenfnt", 180, 380, 8);
		m_iMusicVolValId = GET_STRING()->Create("0", "greenfnt", 115, 380, 8);
		m_iSoundVolId = GET_STRING()->CreateFromTable("opt6", "greenfnt", 180, 330, 8);
		m_iSoundVolValId = GET_STRING()->Create("0", "greenfnt", 115, 330, 8);

		GET_STRING()->SetVisible(FALSE, m_iOptionsId);
		GET_STRING()->SetVisible(FALSE, m_iMusicVolId);
		GET_STRING()->SetVisible(FALSE, m_iMusicVolValId);
		GET_STRING()->SetVisible(FALSE, m_iSoundVolId);
		GET_STRING()->SetVisible(FALSE, m_iSoundVolValId);
		break;
	}
	return TRUE;
}


void COptionsDlg::DrawOptions()
{
	GetScene()->SetVisible(FALSE);
	GET_STRING()->SetVisible(FALSE);

	GET_STRING()->SetVisible(TRUE, m_iOptionsId);
	GET_STRING()->SetVisible(TRUE, m_iMusicVolId);
	GET_STRING()->SetVisible(TRUE, m_iMusicVolValId);
	GET_STRING()->SetVisible(TRUE, m_iSoundVolId);
	GET_STRING()->SetVisible(TRUE, m_iSoundVolValId);

	GET_ENGINE()->SetRTIntVar(NA_RT_GAME_STATE, GS_INFRA);

	GET_SPRITE("hoback")->SetVisible(TRUE);
	GET_SPRITE("hoback")->SetZPos(7);

	m_pMusicDown->DrawButton(80, 375, 8);
	m_pMusicUp->DrawButton(140, 375, 8);
	m_pSoundDown->DrawButton(80, 325, 8);
	m_pSoundUp->DrawButton(140, 325, 8);

	m_pFullScr->DrawCheckBox(100, 270, 8);
	m_pMuteAll->DrawCheckBox(100, 240, 8);
	m_pRusLng->DrawCheckBox(100, 210, 8);
	m_pEngLng->DrawCheckBox(100, 180, 8);

	m_pOKBut->DrawButton(492, 20, 8);

	int iVal;
	char szBuf[100];

	GET_ENGINE()->GetVars("config")->GetInt(L_GROUP, "config", "sndvol", 0, &iVal);
	GET_STRING()->Change(_itoa(iVal, szBuf, 10), m_iSoundVolValId);

	GET_ENGINE()->GetVars("config")->GetInt(L_GROUP, "config", "musvol", 0, &iVal);
	GET_STRING()->Change(_itoa(iVal, szBuf, 10), m_iMusicVolValId);

	GET_ENGINE()->GetVars("config")->GetInt(L_GROUP, "config", "mute", 0, &iVal);
	m_pMuteAll->SetState((BOOL)iVal);

	m_pFullScr->SetState(!GET_ENGINE()->GetWindowed());
}


void COptionsDlg::MusicUp()
{
	int iVal;
	char szBuf[100];

	GET_ENGINE()->GetVars("config")->GetInt(L_GROUP, "config", "mute", 0, &iVal);
	if (iVal)
		return;

	GET_ENGINE()->GetVars("config")->GetInt(L_GROUP, "config", "musvol", 0, &iVal);
	iVal += 10;
	if (iVal > 250) iVal = 250;
	GET_ENGINE()->GetVars("config")->SetInt(L_GROUP, "config", "musvol", 0, iVal);
	GET_STRING()->Change(_itoa(iVal, szBuf, 10), m_iMusicVolValId);
	GET_AUDIO()->SetMusicVolume(GET_ENGINE()->GetRTStringVar(NA_RT_BACK_MUSIC), iVal);
}

void COptionsDlg::MusicDown()
{
	int iVal;
	char szBuf[100];

	GET_ENGINE()->GetVars("config")->GetInt(L_GROUP, "config", "mute", 0, &iVal);
	if (iVal)
		return;

	GET_ENGINE()->GetVars("config")->GetInt(L_GROUP, "config", "musvol", 0, &iVal);
	if (iVal > 10)
		iVal -= 10;
	else
		iVal = 0;
	GET_ENGINE()->GetVars("config")->SetInt(L_GROUP, "config", "musvol", 0, iVal);
	GET_STRING()->Change(_itoa(iVal, szBuf, 10), m_iMusicVolValId);
	GET_AUDIO()->SetMusicVolume(GET_ENGINE()->GetRTStringVar(NA_RT_BACK_MUSIC), iVal);
}

void COptionsDlg::SoundUp()
{
	int iVal;
	char szBuf[100];

	GET_ENGINE()->GetVars("config")->GetInt(L_GROUP, "config", "mute", 0, &iVal);
	if (iVal)
		return;

	GET_ENGINE()->GetVars("config")->GetInt(L_GROUP, "config", "sndvol", 0, &iVal);
	iVal += 10;
	if (iVal > 250) iVal = 250;
	GET_ENGINE()->GetVars("config")->SetInt(L_GROUP, "config", "sndvol", 0, iVal);
	GET_STRING()->Change(_itoa(iVal, szBuf, 10), m_iSoundVolValId);
	GET_AUDIO()->SetSFXVolume(iVal);
}

void COptionsDlg::SoundDown()
{
	int iVal;
	char szBuf[100];

	GET_ENGINE()->GetVars("config")->GetInt(L_GROUP, "config", "mute", 0, &iVal);
	if (iVal)
		return;

	GET_ENGINE()->GetVars("config")->GetInt(L_GROUP, "config", "sndvol", 0, &iVal);
	if (iVal > 10)
		iVal -= 10;
	else
		iVal = 0;
	GET_ENGINE()->GetVars("config")->SetInt(L_GROUP, "config", "sndvol", 0, iVal);
	GET_STRING()->Change(_itoa(iVal, szBuf, 10), m_iSoundVolValId);
	GET_AUDIO()->SetSFXVolume(iVal);
}

void COptionsDlg::SetRusLng()
{
	GET_TABLE()->SetLocale("ru");
	GET_STRING()->OnLocaleChanged();
	m_pEngLng->SetState(FALSE);
	m_pRusLng->SetState(TRUE);
	g_pMenu->FixLevelLabel(TRUE);
	GET_ENGINE()->GetVars("config")->SetInt(L_GROUP, "config", "lang", 0, 1);
}

void COptionsDlg::SetEngLng()
{
	GET_TABLE()->SetLocale("en");
	GET_STRING()->OnLocaleChanged();
	m_pRusLng->SetState(FALSE);
	m_pEngLng->SetState(TRUE);
	g_pMenu->FixLevelLabel(FALSE);
	GET_ENGINE()->GetVars("config")->SetInt(L_GROUP, "config", "lang", 0, 0);
}
