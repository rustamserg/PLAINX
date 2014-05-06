//-----------------------------------------------------------------------------
// File: resman.h
//
// Desc: Resource managment core
//
// Copyright (c) 2002 PlainX Group
// 
//-----------------------------------------------------------------------------

#ifndef __RESMAN_H__
#define __RESMAN_H__

#pragma once


class CResourceManager
{
public:
    CResourceManager();
    CResourceManager(CEngine* pEngine);
    virtual ~CResourceManager();

    // init method
    BOOL Init(const char* pszConfig, CEngine* pEngine, const char* pszName);
    void Free();

    // surface update methods
    BOOL RecreateAll();
    BOOL ReloadAll();

    // custom surface creating
    BOOL CreateSurface(const char* pcszName, int iWidth, int iHeight, const char* pcszFilename = 0,
                        RM_TYPE eAllocationType = RMT_BOTH, int iCKey = 0, COLORREF crBack = RGB(0, 0, 0));

    // access method
    CSurface*   GetSurface(const char* pszName);
    int         GetWidth(const char* pszName);
    int         GetHeight(const char* pszName);
    int         GetColorKey(const char* pszName);

private:
    typedef struct _RM_DATA {
        CSurface*   pSurface;
        char        szPath[MAX_PATH];
        char        szName[MAX_BUFFER];
        RM_TYPE     eMemoryType;
        int         nWidth;
        int         nHeight;
        int         nCKey;
        COLORREF    crBack;
    } RM_DATA;

    typedef std::map<std::string, RM_DATA*> RM_DATA_HASH_MAP;
    typedef RM_DATA_HASH_MAP::iterator RM_DATA_HASH_MAP_ITERATOR;

private:
    RM_DATA_HASH_MAP    m_pData;
    CFileManager*       m_pFM;
    
    char        m_szGroup[MAX_BUFFER];
    char        m_szConfig[MAX_BUFFER];
    CEngine*    m_pParent;

private:
    BOOL    rm_LoadGroup(const char* pszName, CVars* pVars);
    BOOL    rm_CreateData(const char* pszName, CVars* pVars);
    BYTE*   rm_CreateDummyBitmap(int nWidth, int nHeight, COLORREF crBack, DWORD* pdwSize);
};

#endif //__RESMAN_H__
