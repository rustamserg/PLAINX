#ifndef HELPDLG_H
#define HELPDLG_H

#include "listener.h"

class CHelpDlg: public CListener
{
public:
    CHelpDlg();
    virtual ~CHelpDlg();

    BOOL ProcessState(E_STATE sState, DWORD dwCurrTick);

public:
	void DrawHelp();
	void DrawAuthors();

private:
	CButton* m_pNextBut;
	CButton* m_pOKBut;
	int m_iHelpId;
	int m_iAuthorsId;
	int m_iHelpDesc[6];
	int m_iBonusDesc[7];
	int m_iAuthorsDesc[14];
};

#endif // LEVEL_H 
