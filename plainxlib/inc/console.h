//-----------------------------------------------------------------------------
// File: console.h
//
// Desc: Console support
//
// Copyright (c) 2002 PlainX Group
// 
//-----------------------------------------------------------------------------

#ifndef __CONSOLE_H__
#define __CONSOLE_H__

#pragma once

#define CON_MAX_CMD 10
#define L_ENGINE_CON_FONT	"cfont"

#include "listener.h"


class CConsole : public CListener
{
public:
    CConsole();
    virtual ~CConsole();

    void SetFPS(int iFPS) { m_iFPS = iFPS; }
    void SetStatus(const char* pcszStatus);

    BOOL ProcessMessage(E_MESSAGE eMessage, DWORD dwCurrTick);
    BOOL ProcessState(E_STATE sState, DWORD dwCurrTick);

private:
    struct CON_CMD {
        char szCmd[MAX_BUFFER];
        int iCmdPos;
    };

private:
    int m_iFPS;                     // fps value
    int m_iFpsId;                   // string id for draw fps in the console
    int m_iStatusId;                // string id for console status
    int m_iInputId;                 // string id for console input
    int m_iPrevInputIds[2];         // string id for previous input
    char m_szStatus[MAX_BUFFER];    // value of status field
    CON_CMD m_cmd[CON_MAX_CMD];     // list of 10 commands
    int m_iCurrCmd;                 // index of current cmd
    BOOL m_bIsDrawCursor;

private:
    void UpdateConsole(DWORD dwCurrTick);
};

#endif // __CONSOLE_H__
