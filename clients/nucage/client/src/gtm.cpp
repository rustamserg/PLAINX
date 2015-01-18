#include "plainx.h"
#include "gglob.h"
#include "gtm.h"


CGameTime::CGameTime() :
CListener()
{
	m_iLevelTimeSec = 30;
	m_iTimeValueId = -1;
}


CGameTime::~CGameTime()
{
}


BOOL CGameTime::ProcessState(E_STATE sState, DWORD dwCurrTick)
{
    switch (sState.dwStateID) {
    case ES_LISTENERINIT:
		m_iTimeValueId = GET_STRING()->Create("", "greenfnt", 50, 85, 3);
		m_iLevelTimeSec = 30;
        break;
    case ES_TIME_LOOP:
        if (GetMSecDiff(dwCurrTick) > 1000) {
            UpdateTime();
            SetLastTick(dwCurrTick);
            m_iLevelTimeSec--;
            if (m_iLevelTimeSec < 0) {
                GET_ENGINE()->PostState(ES_LEVEL_FAILED, 0, 0, "level", "main");
                return TRUE;
            }
        }
        return FALSE;
    }
    return TRUE;
}


void CGameTime::SetTime(int iTimeSec)
{
    m_iLevelTimeSec = iTimeSec;
    UpdateTime();
}


void CGameTime::UpdateTime()
{
    char szBuf[MAX_BUFFER];

    sprintf(szBuf, "%i", m_iLevelTimeSec);
	GET_STRING()->Change(szBuf, m_iTimeValueId);
}


void CGameTime::StartTimer()
{
    StopTimer();
    PostState(ES_TIME_LOOP, 0, 0);

	GET_STRING()->SetVisible(TRUE, m_iTimeValueId);
}


void CGameTime::StopTimer()
{
    ClearStates();

	GET_STRING()->SetVisible(FALSE, m_iTimeValueId);
}
