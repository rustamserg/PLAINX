//-----------------------------------------------------------------------------
// File: listener.h
//
// Desc: Listener interface
//
// Copyright (c) 2002 PlainX Group
// 
//-----------------------------------------------------------------------------

#ifndef __LISTENER_H__
#define __LISTENER_H__

#pragma once


class CListener
{
public:
    CListener();
    virtual ~CListener();

    // init function
    void Init(CSceneManager* pParent);

    // virtual function
    virtual BOOL ProcessState(E_STATE sState, DWORD dwCurrTick);
    virtual BOOL ProcessMessage(E_MESSAGE eMessage, DWORD dwCurrTick);

    // state routines
    void ClearStates();                 // remove all states
    BOOL GetState(E_STATE* peState);    // get state if no state return false
    void DelState();                    // remove current state
    E_STATE GetState();                 // get current state, do not delete from queue
    void PostState(DWORD dwStateID, LPARAM lParam, WPARAM wParam);

    // mutators
    void Enable() { m_bEnable = TRUE; }
    void Disable() { m_bEnable = FALSE; }
    void SetLastTick(DWORD dwTick) { m_dwLastTick = dwTick; }

    // accessors
    BOOL GetEnable() { return m_bEnable; }
    CSceneManager* GetScene() { return m_pParent; }
    DWORD GetMSecDiff(DWORD dwCurrTick);

private:
    E_QSTATE            m_qState;       // states queue
    BOOL                m_bEnable;      // enable flag
    CSceneManager*      m_pParent;      // pointer to scene manager
    DWORD               m_dwLastTick;
};

#endif // __LISTENER_H__

