#ifndef HELP_H
#define HELP_H

#include "listener.h"


class CHelpListener: public CListener
{
public:
	CHelpListener();
	BOOL ProcessMessage(E_MESSAGE eMessage, DWORD dwCurrTick);

private:
	int m_nPage;

private:
	void NextPage();
	void PrevPage();
	void ShowPage();
};

#endif //HELP_H
