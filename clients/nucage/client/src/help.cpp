#include "plainx.h"
#include "gglob.h"
#include "buttons.h"
#include "help.h"

static char* s_sBonus[] = {"hclock",
						   "hkey",
						   "hteleport",
						   "holdstone",
						   "hgate",
						   "henter",
						   "hexit"};


CHelpDlg::CHelpDlg() :
CListener()
{
	m_pNextBut = NULL;
	m_pOKBut = NULL;
}


CHelpDlg::~CHelpDlg()
{
}


BOOL CHelpDlg::ProcessState(E_STATE sState, DWORD dwCurrTick)
{
    switch (sState.dwStateID) {
    case ES_LISTENERINIT:
		m_pNextBut = new CNextBut();
		m_pOKBut = new COKBut();

		GetScene()->AddListener(m_pNextBut, "nexthlpbut");
		GetScene()->AddListener(m_pOKBut, "okhelpbut");

		m_pNextBut->InitButton("button", "but3", "buttonfnt");
		m_pOKBut->InitButton("button", "but8", "buttonfnt");

		m_iHelpId = GET_STRING()->CreateFromTable("help", "labelfnt", 300, 440, 8);
		m_iAuthorsId = GET_STRING()->CreateFromTable("authors", "labelfnt", 270, 440, 8);

		char buf[MAX_BUFFER];

		for (int i = 0; i < 6; i++) {
			wsprintf(buf, "help%i", i + 1);
			m_iHelpDesc[i] = GET_STRING()->CreateFromTable(buf, "greenfnt", 50, 400 - i*25, 8);
			GET_STRING()->SetVisible(FALSE, m_iHelpDesc[i]);
		}
		for (int i = 0; i < 7; i++) {
			wsprintf(buf, "bonus%i", i + 1);
			m_iBonusDesc[i] = GET_STRING()->CreateFromTable(buf, "greenfnt", 100, 240 - i*35, 8);
			GET_STRING()->SetVisible(FALSE, m_iBonusDesc[i]);
		}
		for (int i = 0; i < 14; i++) {
			wsprintf(buf, "authors%i", i + 1);
			m_iAuthorsDesc[i] = GET_STRING()->CreateFromTable(buf, "greenfnt", 100, 400 - i*25, 8);
			GET_STRING()->SetVisible(FALSE, m_iAuthorsDesc[i]);
		}
		GET_STRING()->SetVisible(FALSE, m_iHelpId);
		GET_STRING()->SetVisible(FALSE, m_iAuthorsId);
		break;
	}
	return TRUE;
}


void CHelpDlg::DrawHelp()
{
	GetScene()->SetVisible(FALSE);
	GET_STRING()->SetVisible(FALSE);

	GET_STRING()->SetVisible(TRUE, m_iHelpId);

	GET_ENGINE()->SetRTIntVar(NA_RT_GAME_STATE, GS_INFRA);

	GET_SPRITE("hoback")->SetVisible(TRUE);
	GET_SPRITE("hoback")->SetZPos(7);

	for (int i = 0; i < 6; i++) {
		GET_STRING()->SetVisible(TRUE, m_iHelpDesc[i]);
	}
	for (int i = 0; i < 7; i++) {
		GET_STRING()->SetVisible(TRUE, m_iBonusDesc[i]);
		GET_SPRITE(s_sBonus[i])->SetVisible(TRUE);
		GET_SPRITE(s_sBonus[i])->SetWPos(50, 240 - i*35 - 10, 8);
	}
	m_pNextBut->DrawButton(492, 20, 8);
}

#define AOFFST 400
#define ADIFF1 25
#define ADIFF2 10

void CHelpDlg::DrawAuthors()
{
	GetScene()->SetVisible(FALSE);
	GET_STRING()->SetVisible(FALSE);

	GET_STRING()->SetVisible(TRUE, m_iAuthorsId);

	GET_ENGINE()->SetRTIntVar(NA_RT_GAME_STATE, GS_INFRA);

	GET_SPRITE("hoback")->SetVisible(TRUE);
	GET_SPRITE("hoback")->SetZPos(7);

	for (int i = 0; i < 14; i++) {
		GET_STRING()->SetVisible(TRUE, m_iAuthorsDesc[i]);
	}

	// programming
	GET_STRING()->SetPos(m_iAuthorsDesc[0], 70, AOFFST, 8);
	GET_STRING()->SetPos(m_iAuthorsDesc[1], 70, AOFFST - ADIFF1, 8);

	// game design
	GET_STRING()->SetPos(m_iAuthorsDesc[2], 70, AOFFST - ADIFF1*2 - ADIFF2, 8);
	GET_STRING()->SetPos(m_iAuthorsDesc[3], 70, AOFFST - ADIFF1*3 - ADIFF2, 8);
	GET_STRING()->SetPos(m_iAuthorsDesc[4], 70, AOFFST - ADIFF1*4 - ADIFF2, 8);

	// levels
	GET_STRING()->SetPos(m_iAuthorsDesc[5], 70, AOFFST - ADIFF1*5 - ADIFF2*2, 8);
	GET_STRING()->SetPos(m_iAuthorsDesc[6], 70, AOFFST - ADIFF1*6 - ADIFF2*2, 8);

	// music
	GET_STRING()->SetPos(m_iAuthorsDesc[7], 70, AOFFST - ADIFF1*7 - ADIFF2*3, 8);
	GET_STRING()->SetPos(m_iAuthorsDesc[8], 70, AOFFST - ADIFF1*8 - ADIFF2*3, 8);

	// plainx
	GET_STRING()->SetPos(m_iAuthorsDesc[9], 70, AOFFST - ADIFF1*9 - ADIFF2*4, 8);
	GET_STRING()->SetPos(m_iAuthorsDesc[10], 70, AOFFST - ADIFF1*10 - ADIFF2*4, 8);
	GET_STRING()->SetPos(m_iAuthorsDesc[11], 70, AOFFST - ADIFF1*11 - ADIFF2*4, 8);

	// other libraries
	GET_STRING()->SetPos(m_iAuthorsDesc[12], 70, AOFFST - ADIFF1*12 - ADIFF2*5, 8);

	// link to our site
	GET_STRING()->SetPos(m_iAuthorsDesc[13], 260, AOFFST - ADIFF1*13 - ADIFF2*5, 8);

	m_pOKBut->DrawButton(492, 20, 8);
}
