//-----------------------------------------------------------------------------
// File: effects.h
//
// Desc: Effects support
//
// Copyright (c) 2002 PlainX Group
// 
//-----------------------------------------------------------------------------

#ifndef __EFFECTS_H__
#define __EFFECTS_H__

#pragma once

#include "listener.h"

#define EFS_MAX_SPRITES 10


class CEffects : public CListener
{
public:
    CEffects();
    virtual ~CEffects();

    BOOL Add(const char* pcszConfig, const char* pcszEffectName);
    void Del(const char* pcszEffectName);
    void Draw(const char* pcszEffectName, int iWX, int iWY, int iWZ);

    BOOL ProcessState(E_STATE sState, DWORD dwCurrTick);

    void Free();

private:
    struct EFS {
        char szName[MAX_BUFFER];
        char szSprite[MAX_BUFFER];
        char szSample[MAX_BUFFER];
        int iCycles;
        int iSpeed;
    };

    typedef std::map<std::string, EFS*> EFS_HASH_LIST;
    typedef EFS_HASH_LIST::iterator EFS_HASH_LIST_ITERATOR;

private:
    EFS_HASH_LIST m_effects;
    CSprite* m_pSprites[EFS_MAX_SPRITES];
    int m_iNextPos;
};

#endif // __EFFECTS_H__
