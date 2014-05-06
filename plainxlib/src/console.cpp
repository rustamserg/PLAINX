#include "plainx.h"
#include "console.h"

#define ES_CONSOLE_UPDATE 10


CConsole::CConsole()
: CListener()
{
	m_iFPS = 0;
	m_iFpsId = -1;
	m_iInputId = -1;
	m_iStatusId = -1;
	m_szStatus[0] = 0;
	m_bIsDrawCursor = TRUE;
	m_iCurrCmd = 0;

	for (int i = 0; i < CON_MAX_CMD; i++) {
		m_cmd[i].iCmdPos = 0;
		m_cmd[i].szCmd[0] = 0;
	}

	m_iPrevInputIds[0] = m_iPrevInputIds[1] = -1;
}


CConsole::~CConsole()
{
}


BOOL CConsole::ProcessMessage(E_MESSAGE eMessage, DWORD dwCurrTick)
{
	switch (eMessage.dwMessageID) {
	case EM_KEYPRESSED:
		if (eMessage.wParam == DIK_BACKSPACE) {

			if (m_cmd[m_iCurrCmd].iCmdPos > 0) {
				m_cmd[m_iCurrCmd].iCmdPos--;
				m_cmd[m_iCurrCmd].szCmd[m_cmd[m_iCurrCmd].iCmdPos] = 0;
			}

		} else if (eMessage.wParam == DIK_RETURN) {

			GET_ENGINE()->ProcessConsole(m_cmd[m_iCurrCmd].szCmd);
				
			m_iCurrCmd++;
			if (m_iCurrCmd >= CON_MAX_CMD)
				m_iCurrCmd = 0;

			m_cmd[m_iCurrCmd].szCmd[0] = 0;
			m_cmd[m_iCurrCmd].iCmdPos = 0;

		} else if (eMessage.wParam == DIK_UP) {

			if (m_iCurrCmd > 0) 
				m_iCurrCmd--;

		} else if (eMessage.wParam == DIK_DOWN) {

			if (m_iCurrCmd < CON_MAX_CMD - 1)
				m_iCurrCmd++;

		} else if (eMessage.wParam != DIK_GRAVE && eMessage.lParam != ASCII_NULL_CODE) {

			if (m_cmd[m_iCurrCmd].iCmdPos < sizeof(m_cmd[m_iCurrCmd].szCmd) - 1) {
				m_cmd[m_iCurrCmd].szCmd[m_cmd[m_iCurrCmd].iCmdPos++] = eMessage.lParam;
				m_cmd[m_iCurrCmd].szCmd[m_cmd[m_iCurrCmd].iCmdPos] = 0;
			}

		}
		break;
	}
	return FALSE;
}


BOOL CConsole::ProcessState(E_STATE sState, DWORD dwCurrTick)
{
	switch (sState.dwStateID) {
	case ES_LISTENERINIT:
		m_iFpsId = GET_STRING()->Create("", L_ENGINE_CON_FONT, 1, 1, 1);
		m_iStatusId = GET_STRING()->Create("", L_ENGINE_CON_FONT, 1, 21, 1);
		m_iInputId = GET_STRING()->Create("", L_ENGINE_CON_FONT, 1, 41, 1);
		m_iPrevInputIds[0] = GET_STRING()->Create("", L_ENGINE_CON_FONT, 1, 61, 1);
		m_iPrevInputIds[1] = GET_STRING()->Create("", L_ENGINE_CON_FONT, 1, 81, 1);

		PostState(ES_CONSOLE_UPDATE, 0, 0);
		break;
	case ES_CONSOLE_UPDATE:
		UpdateConsole(dwCurrTick);
		return FALSE;
	}
	// return TRUE if we process state and want to process next state
	return TRUE;
}


void CConsole::SetStatus(const char* pcszStatus)
{
	strncpy(m_szStatus, pcszStatus, sizeof(m_szStatus) - 1);
	m_szStatus[sizeof(m_szStatus) - 1] = 0;
}


void CConsole::UpdateConsole(DWORD dwCurrTick)
{
	char szOut[MAX_BUFFER];

	wsprintf(szOut, "FPS: %i", m_iFPS);
	GET_STRING()->Change(szOut, m_iFpsId);

	if (m_szStatus[0] == 0) {
		wsprintf(szOut, "Status: []");
	} else {
		wsprintf(szOut, "Status: [%s]", m_szStatus);
	}
	GET_STRING()->Change(szOut, m_iStatusId);

	if (GetMSecDiff(dwCurrTick) > 500) { // flick twice per second
		SetLastTick(dwCurrTick);
		m_bIsDrawCursor = !m_bIsDrawCursor;
	}
	if (m_bIsDrawCursor) {
		if (m_cmd[m_iCurrCmd].szCmd[0] == 0) {
			wsprintf(szOut, ">_");
		} else {
			wsprintf(szOut, ">%s_", m_cmd[m_iCurrCmd].szCmd);
		}
	} else {
		if (m_cmd[m_iCurrCmd].szCmd[0] == 0) {
			wsprintf(szOut, ">    ", m_cmd[m_iCurrCmd].szCmd);
		} else {
			wsprintf(szOut, ">%s    ", m_cmd[m_iCurrCmd].szCmd);
		}
	}
	GET_STRING()->Change(szOut, m_iInputId);

	GET_STRING()->Change("", m_iPrevInputIds[0]);
	GET_STRING()->Change("", m_iPrevInputIds[1]);

	if (m_iCurrCmd > 0) {
		GET_STRING()->Change(m_cmd[m_iCurrCmd - 1].szCmd, m_iPrevInputIds[0]);
	}
	if (m_iCurrCmd > 1) {
		GET_STRING()->Change(m_cmd[m_iCurrCmd - 2].szCmd, m_iPrevInputIds[1]);
	}
}
