//-----------------------------------------------------------------------------
// File: sprite.h
//
// Desc: Sprite core
//
// Copyright (c) 2002 PlainX Group
// 
//-----------------------------------------------------------------------------

#ifndef __SPRITE_H__
#define __SPRITE_H__

#pragma once


class CSprite
{
public:
    CSprite();
    virtual ~CSprite();

    // init methods
    BOOL Init(CVars* pVars, CSceneManager* pParent, const char* pszName);
    BOOL Init(SPRITE_DATA* pData, CSceneManager* pParent, const char* pszName);
    void Free();

    // accessors
    int             GetWXPos() { return m_ptWPos.x; }
    int             GetWYPos() { return m_ptWPos.y; }
    BOOL            GetVisible() { return m_bVisible; }
    int             GetSpeed() { return m_nSpeed; }
    char*           GetName() { return m_szName; }
    char*           GetData() { return m_szData; }
    int             GetRow() { return m_nCurRow; }
    int             GetRowHeight();
    int             GetFrameWidth();
    int             GetFrameWidth(int nFrame);
    int             GetFrame() { return m_nCurFrame; }
    int             GetFrames() { return m_nFrames; }
    int             GetRows() { return m_nRows; }
    int             GetZPos() { return m_nZPos; }
    int             GetAnimCycles() { return m_nAnimCycles; }
    CSurface*       GetSurface();
    BOOL            GetUpdateRect(RECT* prcWorld, RECT* prcSurface);
    DWORD           GetAnimState() { return m_dwAnimState; }
    BOOL            GetUpFrames() { return m_bUpFrames; }
    CSceneManager*  GetScene() { return m_pParent; }
    BYTE            GetAlpha() { return m_nAlpha; }
    int             GetColorKey() { return m_iColorKey; }
    COLORREF        GetBlendColor() { return m_dwBlendColor; }
    CSurface*       GetBlendSurface();
    void*           GetUserData() { return m_pUserData; }
    void*           GetBox() { return m_pBox; }

    // mutators
    void SetUpFrames(BOOL bIsUpFrames) { m_bUpFrames = bIsUpFrames; }
    BOOL SetWXPos(int nXPos);
    BOOL SetWYPos(int nYPos);
    BOOL SetZPos(int nZPos); // set zpos to 0 for don't draw sprite
    BOOL SetWPos(int nXPos, int nYPos, int nZPos);
    void SetVisible(BOOL bVisible, BOOL bIsDirt = TRUE);
    BOOL SetSpeed(int nSpeed);
    BOOL SetRow(int nRow);
    BOOL SetFrame(int nFrame);
    void SetAnimCycles(int nAnimCycles);
    void SetName(const char* pszName) { strcpy(m_szName, pszName); }
    void SetAlpha(BYTE nAlpha);
    void SetBlendColor(COLORREF dwBlendColor);
    void SetUserData(void* pUserData) { m_pUserData = pUserData; }
    void SetBox(CPXBBox* pBox);

    // clone support
    CSprite* Clone();

    // for draw use
    inline int GetDLWXPos();
    inline int GetDLWYPos();

    // update
    void Update(DWORD dwCurrTick);
    void ReloadSurface();

    // pixel check
    BOOL IsInSprite(int nSX, int nSY);
    void DirtSprite();

    // hotspot
    POINT GetHotSpot();
    void SetHotSpot(POINT ptHotSpot);

    // inner transformation support
    BOOL CreateTransformSurface(int nMaxFrameWidth, int nMaxFrameHeight);
    void DeleteTransformSurface();

    // deformation
    BOOL ScaleFrame(float fScale);

    // rotation
    BOOL RotateFrame(int nAngle);
    BOOL SetRotation(int nRotAngle);
    int GetRotationAngle() { return m_nRotAngle; }
    int GetAngle() { return m_nAngle; }

private:
    int             m_iColorKey;                    // sprite color key = 1/0
    BOOL            m_bIsUseSysFontRM;              // TRUE if we use system font resource
    char            m_szName[MAX_BUFFER];           // sprite name
    char            m_szData[MAX_BUFFER];           // resource data name
    char            m_szBlendData[MAX_BUFFER];      // resource data name for blend
    POINT           m_ptHotSpot;                    // sprite hotspot
    POINT           m_ptWPos;                       // sprite position if world coords
    BOOL            m_bVisible;                     // is visible
    int             m_nSpeed;                       // animation speed in frames per second
    int             m_nZPos;                        // z position
    int             m_nCurFrame;                    // current frame
    int             m_nCurRow;                      // current row
    int             m_nFrames;                      // number of frames
    int             m_nRows;                        // number of rows
    int             m_nRowHeight;                   // height of row in pixels
    int             m_nFrameWidth[MAX_FRAMES];      // width of frame in pixels
    BYTE            m_nAlpha;                       // alpha value (0-255) 0 - means simple blt, 255 - hidden sprite   
    CSurface*       m_pSurface;                     // pointer to surface
    CSurface*       m_pBlendSurface;                // pointer to blend surface
    CSurface*       m_pTransSurf;                   // pointer to transformed surface
    CSurface*       m_pBlendTransSurf;              // pointer to transformed blend surface
    int             m_nRotAngle;                    // rotation angle
    int             m_nAngle;                       // result angle
    DWORD           m_dwBlendColor;                 // blend color
    CSceneManager*  m_pParent;                      // pointer to scene manager
    int             m_nAnimCycles ;                 // number of anim cycles if 0=infinity
    DWORD           m_dwAnimState;                  // current sprite state
    BOOL            m_bUpFrames;                    // true if frame animated from 0 to 30
    BOOL            m_bIsDirty;
    int             m_nTransRowHeight;
    int             m_nTransFrameWidth[MAX_FRAMES];
    DWORD           m_dwTransState;
    POINT           m_ptTransHotSpot;
    int             m_nMaxFrameWidth;
    int             m_nMaxFrameHeight;
    void*           m_pUserData;        // pointer to user data, managed by user
    DWORD           m_dwPrevTick;       // prev tick count
    DWORD           m_dwDiffTick;       // number of ticks need for frame change
    CPXBBox*        m_pBox;             // sprite bounding box

private:
    BOOL s_SetNewWPos(int nWX, int nWY);
    int s_GetFrameOffset(int nFrame);
    void s_UpdateDimension(float fScale);
};

#endif //__SPRITE_H__