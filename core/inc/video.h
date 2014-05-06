//-----------------------------------------------------------------------------
// File: video.h
//
// Desc: Display managment core
//
// Copyright (c) 2002 PlainX Group
// 
//-----------------------------------------------------------------------------

#ifndef __VIDEO_H__
#define __VIDEO_H__

#pragma once


class CVideoManager
{
public:
    CVideoManager();
    virtual ~CVideoManager();

    // init methods
    BOOL Init(CEngine* pEngine);
    void Free();

    // setup methods
    BOOL SetMode(HWND hWnd, int* pnSWidth = NULL, int* pnSHeight = NULL, int* pnSBPP = NULL, BOOL bWindowed = FALSE);

    BOOL IsCanBeWindowed(int nSBPP);

    // video and blt functions
    void SetUseVideoMemory(BOOL bv);
    void SetUseHardwareBlt(BOOL bv);
    BOOL GetUseVideoMemory();
    BOOL GetUseHardwareBlt();

    // update methods
    void        ToggleScreenMode(HWND hWnd , BOOL bWindowed, RECT* prcWindow);
    void        RetrieveWindowPosition();
    HRESULT     UpdateScreen();
    HRESULT     TestCooperativeLevel();

    // access methods
    CDisplay*   GetDisplay() { return m_pDisplay; }
    int         GetScreenWidth() { return m_nSWidth; }
    int         GetScreenHeight() { return m_nSHeight; }
    int         GetScreenDepth();
    int         GetFreeVideoMemory();
    int         GetTotalVideoMemory();

private:
    CEngine*    m_pParent;
    CDisplay*   m_pDisplay;
    RECT        m_rcWindow;
    int         m_nSWidth;
    int         m_nSHeight;
    int         m_nSBPP;
    BOOL        m_bNeedReload;
    BOOL        m_bNeedRecreate;
};

#endif// __VIDEO_H__

