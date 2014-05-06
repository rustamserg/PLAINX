// help.cpp: implementation of help depended functions.
//
/////////////////////////////////////////////////////////////////////////

#include "plainx.h"
#include "help.h"
#include "sprite.h"
#include "global_game.h"


CHelpListener::CHelpListener()
{
	m_nPage = 0;
}


//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
void CHelpListener::NextPage()
{
	m_nPage++;
	if (m_nPage > 1) m_nPage = 0;
	ShowPage();
}

//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
void CHelpListener::PrevPage()
{
	m_nPage--;
	if (m_nPage < 0) m_nPage = 1;
	ShowPage();
}


//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
void CHelpListener::ShowPage()
{
	GetScene()->SetVisible(FALSE);
	GET_AUDIO()->PlaySample("put");
	GET_SPRITE("cross")->SetVisible(TRUE);
	if (m_nPage == 0)
	{
		GET_SPRITE("help01")->SetVisible(TRUE);
		GET_SPRITE("arrowr")->SetVisible(TRUE);
	}
	if (m_nPage == 1)
	{
		GET_SPRITE("help02")->SetVisible(TRUE);
		GET_SPRITE("arrowl")->SetVisible(TRUE);
	}
}


//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
BOOL CHelpListener::ProcessMessage(E_MESSAGE eMessage, DWORD dwCurrTick)
{
	if (GET_ENGINE()->GetRTIntVar("game_state") != GS_HELP)
		return FALSE;

	switch 	(eMessage.dwMessageID)
	{
		case EM_LMOUSEDOWN:
			if (GET_SPRITE("arrowr")->IsInSprite(eMessage.lParam, eMessage.wParam))
				NextPage();
			if (GET_SPRITE("arrowl")->IsInSprite(eMessage.lParam, eMessage.wParam))
				PrevPage();
			if (GET_SPRITE("cross")->IsInSprite(eMessage.lParam, eMessage.wParam))
			{
				GET_AUDIO()->PlaySample("put");
				GET_ENGINE()->PostState(ES_PAD_CLOSEHELP, 0, 0, "mmpad");
			}
			break;
		case EM_KEYPRESSED:
			if (eMessage.wParam == DIK_LEFTARROW)
				PrevPage();
			if (eMessage.wParam == DIK_RIGHTARROW)
				NextPage();
			if (eMessage.wParam == DIK_ESCAPE)
				GET_ENGINE()->PostState(ES_PAD_CLOSEHELP, 0, 0, "mmpad");
			break;
	}
	return FALSE;
}



