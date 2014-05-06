//-----------------------------------------------------------------------------
// File: pxapp.h
//
// Desc: PlainX application entry
//
// Copyright (c) 2002 PlainX Group
// 
//-----------------------------------------------------------------------------

#ifndef __PXAPP_H__
#define __PXAPP_H__

#pragma once

// engine create
template <class T> BOOL PX_CreateEngine(T** ppEngine)
{
    *ppEngine = new T();
    if (*ppEngine == NULL)
        return FALSE;

    return TRUE;
}


class CPlainXApp
{
public:
    CPlainXApp();
    virtual ~CPlainXApp();

    BOOL Init(HINSTANCE hInst, int nCmdShow, CEngine* pEngine, BOOL bIsRunMoreThanOnce = FALSE);
    int Run();

    // command line parsing support
    char* GetArg(const char* pcszCmdLine, int iPos);

    LRESULT Babylon5(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

    // handle region drag area
    // xw, yw - in window coords
    // return TRUE if user in the drag area
    virtual BOOL IsDragArea(int xs, int ys);

    // create region
    virtual void OnCreateRegion(HWND hWnd);

    // accessors
    CEngine* GetEngine() { return m_pEngine; }

    // mutators
    void SetTitle(const char* pcszTitle);
    void SetRegion(HRGN hRgn) { m_hRgn = hRgn; }

private:
    char        m_szWindowName[MAX_BUFFER];
    char        m_szClassName[MAX_BUFFER];
    HWND        m_hWnd;
    HINSTANCE   m_hInst;
    HACCEL      m_hAccel;
    CEngine*    m_pEngine;
    RECT        m_rcWindow;
    BOOL        m_bActive;
    HRGN        m_hRgn;
    HANDLE      m_hGuardPipe;
    BOOL        m_bIsDrawCursor;
    BOOL        m_bIsCustomDragArea;

private:
    HRESULT     WinInit(HINSTANCE hInst, int nCmdShow, HWND* phWnd, HACCEL* phAccel);
    void        DX_OnActivate(HWND hwnd, UINT state, HWND hwndActDeact, BOOL fMinimized);
    void        DX_OnDestroy(HWND hwnd);
    void        DX_OnIdle(HWND hwnd);
    BOOL        CreateGuardPipe();
    void        DrawSystemCursor(BOOL bIsDraw);
};

#endif // __PXAPP_H__
