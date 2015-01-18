#ifndef NUCAGE_H
#define NUCAGE_H

#include "engine.h"


class CNucAge: public CEngine
{
public:
	CNucAge();
	~CNucAge();

public:
    BOOL ProcessState(E_STATE sState, DWORD dwCurrTick);
    void ProcessConsole(const char* pcszConCmd);
};

#endif // NUCAGE_H
