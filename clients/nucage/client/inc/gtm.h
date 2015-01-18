#ifndef GTM_H
#define GTM_H

#include "listener.h"


class CGameTime: public CListener
{
public:
    CGameTime();
    virtual ~CGameTime();

    BOOL ProcessState(E_STATE sState, DWORD dwCurrTick);

public:
    int GetTime() { return m_iLevelTimeSec; }
    void SetTime(int iTimeSec);
    void StartTimer();
    void StopTimer();

private:
    int m_iLevelTimeSec;
	int m_iTimeValueId;

private:
    void UpdateTime();
};

#endif // LEVEL_H 
