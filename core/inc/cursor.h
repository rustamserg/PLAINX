//-----------------------------------------------------------------------------
// File: cursor.h
//
// Desc: Cursor support
//
// Copyright (c) 2002 PlainX Group
// 
//-----------------------------------------------------------------------------

#ifndef __CURSOR_H__
#define __CURSOR_H__

#pragma once

#include "listener.h"


class CCursor : public CListener
{
public:
    CCursor();
    virtual ~CCursor();

    BOOL ProcessMessage(E_MESSAGE eMessage, DWORD dwCurrTick);

    CSprite* GetCursor();
    void SetCursor(CSprite* pSprite);

private:
    CSprite* m_pCursor;
};

#endif // __CURSOR_H__
