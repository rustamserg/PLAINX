//-----------------------------------------------------------------------------
// File: input.h
//
// Desc: DirectX input routine
//
// Copyright (c) 2002 PlainX Group
// 
//-----------------------------------------------------------------------------

#ifndef __INPUT_H__
#define __INPUT_H__

#pragma once


class CInputManager
{
public:
    CInputManager();
    virtual ~CInputManager();

    // init methods
    BOOL InitDirectInput(HWND hWnd, BOOL bIsWindowed);
    void RemoveDirectInput();

    // access methods
    void AccessMouse(BOOL isActive);
    void AccessKeyboard(BOOL isActive);

    LPDIRECTINPUTDEVICE7 GetMouse() { return m_pDIMouse; }
    LPDIRECTINPUTDEVICE7 GetKeyboard() { return m_pDIKeyboard; }

    // process methods
    BOOL ProcessKeyboard();
    BOOL ProcessMouse();
    BOOL IsMouse() { return m_bMouse; }

    // keyboards support
    BOOL KeyPressed(BYTE key);
    BOOL KeyDown(BYTE key);
    BYTE DIK2ASCII(BYTE nDIKCode, BOOL bIsShift);

    // mouse support
    int     GetlX() { return m_dims2.lX; }
    int     GetlY() { return m_dims2.lY; }
    BOOL    IsLButton() { return (m_dims2.rgbButtons[0] & 0x80); }
    BOOL    IsRButton() { return (m_dims2.rgbButtons[1] & 0x80); }

private:
    void im_FillKeyMaps();

private:
    LPDIRECTINPUT7          m_pDI;
    LPDIRECTINPUTDEVICE7    m_pDIMouse;
    LPDIRECTINPUTDEVICE7    m_pDIKeyboard;

    BOOL m_bMouse;

    BYTE m_bKeyState[256];
    BYTE m_bKeyBuffer[256];

    BYTE m_sKeyMapLR[256];
    BYTE m_sKeyMapHR[256];

    DIMOUSESTATE2 m_dims2;
};

#endif // __INPUT_H__
