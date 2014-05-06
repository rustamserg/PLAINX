//-----------------------------------------------------------------------------
// File: display.h
//
// Desc: DDraw support
//
// Copyright (c) 2002 PlainX Group
// 
//-----------------------------------------------------------------------------

#ifndef __DISPLAY_H__
#define __DISPLAY_H__

#pragma once


class CDisplay
{
public:
    CDisplay();
    virtual ~CDisplay();

    // Access functions
    HWND                    GetHWnd() { return m_hWnd; }
    LPDIRECTDRAW7           GetDirectDraw() { return m_pDD; }
    LPDIRECTDRAWSURFACE7    GetFrontBuffer() { return m_pddsFrontBuffer; }
    LPDIRECTDRAWSURFACE7    GetBackBuffer() { return m_pddsBackBuffer; }

    // Status functions
    BOOL IsWindowed() { return m_bWindowed; }
    BOOL IsStereo() { return m_bStereo; }

    // video and blt functions
    BOOL GetUseVideoMemory() { return m_bUseVideoMemory; }
    BOOL GetUseHardwareBlt() { return m_bUseHardwareBlt; }
    void SetUseVideoMemory(BOOL bv);
    void SetUseHardwareBlt(BOOL bv);

    // Creation/destruction methods
    HRESULT CreateFullScreenDisplay(HWND hWnd, DWORD dwWidth, DWORD dwHeight, DWORD dwBPP);
    HRESULT CreateWindowedDisplay(HWND hWnd, DWORD dwWidth, DWORD dwHeight);
    HRESULT InitClipper();
    HRESULT UpdateBounds();
    HRESULT DestroyObjects();

    // Methods to create child objects
    HRESULT CreateSurface(CSurface** ppSurface, int nWidth, int nHeight, BOOL bIsVideoMemory = TRUE);
    HRESULT CreateSurfaceFromAny(CSurface** ppSurface, BYTE* pImage, DWORD dwSize, int* pnWidth, int* pnHeight, BOOL bIsVideoMemory = TRUE);

    // Display methods
    HRESULT Clear(DWORD dwColor = 0L);
    HRESULT ClearEx(DWORD dwColor = 0L, RECT* prc = NULL);
    HRESULT Blt(DWORD x, DWORD y, CSurface* pSurface, RECT* prc = NULL);
    HRESULT BltAlpha(DWORD x, DWORD y, CSurface* pSurface, BYTE nAlpha, RECT* prc);
    HRESULT BltAlphaBlend(DWORD x, DWORD y, CSurface* pSurface, COLORREF dwBlendColor, RECT* prc);
    HRESULT BltStretch(RECT* prcDest, CSurface* pDestSurface, RECT* prcSourceSurface, CSurface* pSourceSurface);
    HRESULT BltRotate(RECT* prcDest, CSurface* pDestSurface, CSurface* pSourceSurface, POINT ptO, int nAngle);

    HRESULT Present();

    DWORD GetScreenDepth();

    HRESULT CreateDirectDraw();

private:
    LPDIRECTDRAW7           m_pDD;
    LPDIRECTDRAWSURFACE7    m_pddsFrontBuffer;
    LPDIRECTDRAWSURFACE7    m_pddsBackBuffer;
    LPDIRECTDRAWSURFACE7    m_pddsEffectsBuffer;

    HWND        m_hWnd;
    RECT        m_rcWindow;
    BOOL        m_bWindowed;
    BOOL        m_bStereo;
    RECT        m_rcScreen;
    BOOL        m_bIsCalcMath;
    BOOL        m_bIsDrawLogo;
    BYTE        m_nAlpha[256][256][256];
    DWORD       m_dwBitMask[3];
    DWORD       m_dwShift[3];
    double      m_nSin[360];
    double      m_nCos[360];
    BOOL        m_bUseVideoMemory;
    BOOL        m_bUseHardwareBlt;

private:
    HRESULT BltHard(DWORD x, DWORD y, LPDIRECTDRAWSURFACE7 pdds, RECT* prc, DWORD dwFlags);
    HRESULT BltSoft16(DWORD x, DWORD y, LPDIRECTDRAWSURFACE7 pdds, RECT* prc, DWORD dwFlags);
    HRESULT BltSoft32(DWORD x, DWORD y, LPDIRECTDRAWSURFACE7 pdds, RECT* prc, DWORD dwFlags);
    HRESULT BltRotate16(RECT* prcDest, CSurface* pDestSurface, CSurface* pSourceSurface, POINT ptO, int nAngle);
    HRESULT BltRotate32(RECT* prcDest, CSurface* pDestSurface, CSurface* pSourceSurface, POINT ptO, int nAngle);
    HRESULT BltAlpha16(DWORD x, DWORD y, CSurface* pSurface, BYTE nAlpha, RECT* prc);
    HRESULT BltAlpha32(DWORD x, DWORD y, CSurface* pSurface, BYTE nAlpha, RECT* prc);
    HRESULT BltAlphaBlend16(DWORD x, DWORD y, CSurface* pSurface, COLORREF dwBlendColor, RECT* prc);
    HRESULT BltAlphaBlend32(DWORD x, DWORD y, CSurface* pSurface, COLORREF dwBlendColor, RECT* prc);

    void DrawLogo();
    void CalcMath();
};

#endif // __DISPLAY_H__

