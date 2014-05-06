//-----------------------------------------------------------------------------
// File: surface.h
//
// Desc: DDraw surface support
//
// Copyright (c) 2002 PlainX Group
// 
//-----------------------------------------------------------------------------

#ifndef __SURFACE_H__
#define __SURFACE_H__

#pragma once


class CSurface
{
public:
    CSurface();
    virtual ~CSurface();

    LPDIRECTDRAWSURFACE7    GetDDrawSurface() { return m_pdds; }
    BOOL                    IsColorKeyed() { return m_bColorKeyed; }

    HRESULT DrawBitmap(HBITMAP hBMP, DWORD dwBMPOriginX = 0, DWORD dwBMPOriginY = 0, DWORD dwBMPWidth = 0, DWORD dwBMPHeight = 0);
    HRESULT DrawBitmap(PLWinBmp* pBmp, DWORD dwBMPWidth = 0, DWORD dwBMPHeight = 0);
    HRESULT DrawBitmap(TCHAR* strBMP, DWORD dwDesiredWidth, DWORD dwDesiredHeight);
    HRESULT DrawAny(BYTE* pImage, DWORD dwSize, DWORD dwBMPWidth, DWORD dwBMPHeight);

    BOOL GetSPixel16(DWORD dwHeight, DWORD dwWidth, SPIXEL16* psPixel);
    BOOL SetSPixel16(DWORD dwHeight, DWORD dwWidth, SPIXEL16  sPixel);
    BOOL GetSPixel32(DWORD dwHeight, DWORD dwWidth, SPIXEL32* psPixel);
    BOOL SetSPixel32(DWORD dwHeight, DWORD dwWidth, SPIXEL32  sPixel);

    HRESULT Clear(DWORD dwColor);

    HRESULT DrawText(HFONT hFont, TCHAR* strText, DWORD dwOriginX, DWORD dwOriginY, COLORREF crColor);

    HRESULT SetColorKey(DWORD dwColorKey);
    DWORD ConvertGDIColor(COLORREF dwGDIColor);

    static HRESULT GetBitMaskInfo(DWORD dwBitMask, DWORD* pdwShift, DWORD* pdwBits);

    HRESULT Create(LPDIRECTDRAW7 pDD, DDSURFACEDESC2* pddsd);
    HRESULT Create(LPDIRECTDRAWSURFACE7 pdds);
    HRESULT Destroy();

private:
    LPDIRECTDRAWSURFACE7    m_pdds;
    DDSURFACEDESC2          m_ddsd;
    BOOL                    m_bColorKeyed;
};

#endif // __SURFACE_H__


