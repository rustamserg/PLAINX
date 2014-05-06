//-----------------------------------------------------------------------------
// File: sprman.h
//
// Desc: Sprite managment support
//
// Copyright (c) 2002 PlainX Group
// 
//-----------------------------------------------------------------------------

#ifndef __SPRMAN_H__
#define __SPRMAN_H__

#pragma once


class CSceneManager
{
public:
    CSceneManager();
    virtual ~CSceneManager();

    // init methods
    BOOL Init(const char* pszConfig, CResourceManager* pResMan, CEngine* pEngine, const char* pszName);
    void Free();

    // listener methods
    BOOL AddListener(CListener* pListener, const char* pszName);    // add listener with name
    void DelListener(const char* pszName);                          // remove listener by name
    void ClearListeners();                                          // remove all listeners

    // sprite methods
    CSprite* CreateSprite(SPRITE_DATA* pData, const char* pcszName = NULL);             // custom create sprite
    CSprite* CloneSprite(const char* pszParentName, const char* pszChildName = NULL);   // clone new sprite
    void DelSprite(const char* pszName);                                                // remove sprite from scene
    void OnChangedSpritePos(const char* pcszName);                                      // called by sprite after change position
    void Scale(float fScale);

    // update methods
    void UpdateDirtRects();
    void UpdateSprites(DWORD dwCurrTick);
    void ReloadSprites();
    void RecreateSysFonts();

    // accessors
    CSprite* GetSprite(const char* pszName);
    int GetSpritesFromPoint(int nXSPos,
                            int nYSPos,         // point in screen coords
                            CSprite** pList,    // array for return values
                            int nSize,          // size of array if 0 we return required size
                            int nZPos);         // z-order of sprite

    int                 GetZPos() { return m_nZPos; }
    CResourceManager*   GetResourceManager(BOOL bIsSysFontRM);
    CEngine*            GetEngine() { return m_pEngine; }
    int                 GetBorder() { return m_nBorder; }
    int                 GetCellSizeW() { return m_nCellSizeW; }
    int                 GetCellSizeH() { return m_nCellSizeH; }
    int                 GetWorldWidth() { return m_nWWidth; }
    int                 GetWorldHeight() { return m_nWHeight; }
    RECT                GetViewRect() { return m_rcView; }
    int                 GetCameraX() { return m_pCamera.x; }
    int                 GetCameraY() { return m_pCamera.y; }
    char*               GetName() { return m_szName; }
    char*               GetDescription() { return m_szDescription; }
    CListener*          GetListener(const char* pszName);
    BOOL                GetDrawGrid() { return m_bIsDrawGrid; }

    // mutators
    void SetVisible(BOOL bVisible); // visible all sprites in the scene
    void SetZPos(int nZPos);
    BOOL SetViewRect(RECT* prcView);
    BOOL SetCameraX(int xPos);
    BOOL SetCameraY(int yPos);
    void SetDrawGrid(BOOL bIsDrawGrid) { m_bIsDrawGrid = bIsDrawGrid; }

    // change resource manager for scene
    void SetDataResourceManager(const char* pcszName);

    // screen transformation
    BOOL WorldToScreen(int nWX, int nWY, POINT* ptScreen);
    BOOL ScreenToWorld(int nSX, int nSY, POINT* ptWorld);

    // map methods
    void ClearMap();
    void DirtCamera();
    void ClearCamera();

    // messages manipulations
    BOOL ProcessMessages(DWORD dwCurrTick);

    // state support
    void PostState(DWORD dwStateID, LPARAM lParam, WPARAM wParam, const char* pszListenerName = NULL);

    // rects support
    void DirtWorldRect(RECT* prcDirt);
    void DirtScreenRect(RECT* prcDirt);
    void InitWorldDirtRects();

    // string support
    CStringManager* GetStringManager() { return m_pStrM; }

    // effects support
    BOOL AddEffect(const char* pcszConfig, const char* pcszEffectName);
    void DelEffect(const char* pcszEffectName);
    void ClearAllEffects();
    void DrawEffect(const char* pcszEffectName, int iWX, int iWY, int iWZ);

private:
    typedef std::map<std::string, CListener*> SM_LISTENER_HASH_MAP;
    typedef SM_LISTENER_HASH_MAP::iterator SM_LISTENER_HASH_MAP_ITERATOR;

    typedef std::vector<RECT> SM_DIRT_RECTS;

private:
    char                    m_szDescription[MAX_BUFFER];    // scene description
    CMapManager*            m_pMap;                         // map
    CResourceManager*       m_pRM;
    CResourceManager*       m_pSysFontRM;
    CStringManager*         m_pStrM;                        // support string
    CVars*                  m_pVars;
    RECT                    m_rcView;                       // in screen coordinates
    POINT                   m_pCamera;                      // in world coordinates
    int                     m_nZPos;                        // depth of scene
    int                     m_nCloneCount;                  // using for clone indexing
    int                     m_nWHeight;                     // height in pixels
    int                     m_nWWidth;                      // width in pixels
    CEngine*                m_pEngine;
    int                     m_nBorder;
    int                     m_nCellSizeH;
    int                     m_nCellSizeW;
    int                     m_nClearColor;
    int                     m_nSWidth;
    int                     m_nSHeight;
    int                     m_nViewWWidth;
    int                     m_nViewWHeight;
    SM_LISTENER_HASH_MAP    m_spListenerMap;                // list of all listeners
    SM_DIRT_RECTS           m_rcDirtRects;                  // vector of dirt rects
    BOOL                    m_bFirst;                       // is first run
    char                    m_szName[MAX_BUFFER];           // scene name
    BOOL                    m_bIsDrawGrid;                  //indicate if need draw grid
    CEffects*               m_pEffects;

private:
    BOOL            sm_ClipRect(RECT* prcSrc, RECT* prcDest);
    CSprite*        sm_CreateSprite(CVars* pVars, const char* pcszName = NULL);
    inline void     sm_UpdateDirtySpriteRects(CSprite* pSprite);
    BOOL            sm_LoadScene(const char* pszScene);
    void        sm_DrawGrid();
};

#endif //__SPRMAN_H__
