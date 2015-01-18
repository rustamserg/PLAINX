#ifndef KEYS_H
#define KEYS_H

#include "listener.h"


class CKeys: public CListener
{
public:
    CKeys();
    virtual ~CKeys();

    BOOL ProcessState(E_STATE sState, DWORD dwCurrTick);

public:
    void InitKeys();
    void PopKey(KeyID key);
    void PushKey(KeyID key);
    BOOL IsKeyExist(KeyID key);

private:
    void UpdateKeys();

private:
    int m_iGreenKeyCount;
    int m_iRedKeyCount;
	int m_iKeysGreenValueId;
	int m_iKeysRedValueId;
};

#endif // LEVEL_H 
