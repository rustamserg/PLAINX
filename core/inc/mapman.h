//-----------------------------------------------------------------------------
// File: mapman.h
//
// Desc: Map manager interface
//
// Copyright (c) 2002 PlainX Group
// 
//-----------------------------------------------------------------------------

#ifndef __MAPMAN_H__
#define __MAPMAN_H__

#pragma once


class CMapManager
{
public:
    CMapManager();
    virtual ~CMapManager();

    // initialization
    BOOL Init(CSceneManager* pParent);
    void Free();

    // iteration
    CSprite* GetFirstSprite(BOOL bUseCamera = TRUE);
    CSprite* GetNextSprite(BOOL* pbIsEnd);

    // quick search
    CSprite* GetSprite(const char* pcszName);

    // sprite managment
    BOOL AddSprite(CSprite* pSprite);
    BOOL DelSprite(const char* pcszName);

    // handle sprite change position
    void OnChangedSpritePos(const char* pcszName);

private:
    typedef std::list<CSprite*> SPRITES_LIST;
    typedef SPRITES_LIST::iterator SPRITES_LIST_ITER;

    typedef struct {
        SPRITES_LIST* plist[L_DEPTH];
    } MAP_CELL; // one cell in map

    typedef struct {
        int nCXPos;
        int nCYPos;
        int nCZPos;
        CSprite* pSprite;
    } CellSprite; //spite with its cell position information

    typedef std::map<std::string, CellSprite*> SPRITES_HASH_MAP; // list of all sprites by name
    typedef SPRITES_HASH_MAP::iterator SPRITES_HASH_MAP_ITER;

private:
    int                 m_nCellSizeH;   // size of one map cell in world coords
    int                 m_nCellSizeW;   // size of one map cell in world coords
    int                 m_nBorder;      // offset border around camera view in cell coords
    int                 m_nWWidth;      // width of world in world coords
    int                 m_nWHeight;     // height of world in world coords
    int                 m_nCWidth;      // width of world in cell coords
    int                 m_nCHeight;     // height of world in cell coords
    MAP_CELL**          m_pMap;         // our map
    SPRITES_HASH_MAP    m_hmMap;        // for quick search
    CSceneManager*      m_pParent;      // pointer to scene
    RECT                m_rcIter;       // rect for iter
    int                 m_nXIterPos;    // iter x pos
    int                 m_nYIterPos;    // iter y pos
    int                 m_nZIterPos;    // iter z pos
    SPRITES_LIST_ITER   m_pIter;        // list iterator

private:
    inline int mm_WorldXToCell(int nWXPos);
    inline int mm_WorldYToCell(int nWYPos);
    void mm_CellToWorld(int nCX, int nCY, RECT* prcWorld);  
    void mm_RemoveFromList(CellSprite* pcs);
    void mm_AddToList(CellSprite* pcs);
};

#endif // __MAPMAN_H__
