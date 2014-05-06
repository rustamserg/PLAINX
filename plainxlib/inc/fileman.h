//-----------------------------------------------------------------------------
// File: fileman.h
//
// Desc: File manager interface
//
// Copyright (c) 2002 PlainX Group
// 
//-----------------------------------------------------------------------------

#ifndef __FILEMAN_H__
#define __FILEMAN_H__

#pragma once
#pragma warning( disable : 4244 4700 )  // Disable warning messages


class CFileManager
{
public:
    CFileManager();
    virtual ~CFileManager();

    void* LoadArchiveFile(char* path, LPDWORD pdwSize);
    void* LoadPatchedFile(char* path, LPDWORD pdwSize);

    BOOL Init(const char* path, const char* ArName = NULL);
    BOOL CloseFiles();

    void* LoadFile(char* path, LPDWORD pdwSize = NULL);

    void AddPatchFile(const char* filename);

private:
    struct FM_VIEW {
        DWORD vppViewList;
        DWORD dwSize;
        char name[MAX_BUFFER];
    };

    typedef std::map<std::string, FM_VIEW*> FM_VIEW_HASH_MAP;
    typedef FM_VIEW_HASH_MAP::iterator FM_VIEW_HASH_MAP_ITERATOR;

    typedef std::list<std::string> FM_PATCH_LIST;
    typedef FM_PATCH_LIST::iterator FM_PATCH_LIST_ITERATOR;

private:
    char*               m_szArName;
    FM_VIEW_HASH_MAP    m_viewMap;
    char*               m_szRootPath;
    FM_PATCH_LIST       m_patchList;

private:
    void* fm_LoadFile(char* path, void* vpStartPos, void* vpEndPos, DWORD* pdwSize);
    void* fm_LoadDir(char* path, void* vpStartPos, void* vpEndPos, DWORD* pdwSize);
};

#endif // __FILEMAN_H__
