#include "plainx.h"
#include "gglob.h"
#include "keys.h"


CKeys::CKeys() :
CListener()
{
    m_iGreenKeyCount = 0;
    m_iRedKeyCount = 0;
	m_iKeysRedValueId = -1;
	m_iKeysGreenValueId = -1;
}


CKeys::~CKeys()
{
}


BOOL CKeys::ProcessState(E_STATE sState, DWORD dwCurrTick)
{
    switch (sState.dwStateID) {
    case ES_LISTENERINIT:
		m_iKeysRedValueId = GET_STRING()->Create("", "greenfnt", 587, 59, 3);
		m_iKeysGreenValueId = GET_STRING()->Create("", "greenfnt", 587, 90, 3);
		InitKeys();
        break;
    }
    return TRUE;
}


void CKeys::InitKeys()
{
	m_iRedKeyCount = 0;
	m_iGreenKeyCount = 0;

	GET_STRING()->SetVisible(TRUE, m_iKeysRedValueId);
	GET_STRING()->SetVisible(TRUE, m_iKeysGreenValueId);

	UpdateKeys();
}


void CKeys::PushKey(KeyID key)
{
	if (key == KEY_RED) {
		m_iRedKeyCount++;
	} else if (key == KEY_GREEN) {
		m_iGreenKeyCount++;
	}
	UpdateKeys();
}


void CKeys::PopKey(KeyID key)
{
	if (key == KEY_RED && m_iRedKeyCount > 0) {
		m_iRedKeyCount--;
	} else if (key == KEY_GREEN && m_iGreenKeyCount > 0) {
		m_iGreenKeyCount--;
	}
	UpdateKeys();
}


BOOL CKeys::IsKeyExist(KeyID key)
{
	if (key == KEY_RED && m_iRedKeyCount > 0) return TRUE;
	if (key == KEY_GREEN && m_iGreenKeyCount > 0) return TRUE;
	return FALSE;
}


void CKeys::UpdateKeys()
{
	char szBuf[MAX_BUFFER];

	sprintf(szBuf, "%i", m_iRedKeyCount);
	GET_STRING()->Change(szBuf, m_iKeysRedValueId);

	sprintf(szBuf, "%i", m_iGreenKeyCount);
	GET_STRING()->Change(szBuf, m_iKeysGreenValueId);
}
