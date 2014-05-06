//-----------------------------------------------------------------------------
// File: strman.h
//
// Desc: String support
//
// Copyright (c) 2002 PlainX Group
// 
//-----------------------------------------------------------------------------

#ifndef __STRMAN_H__
#define __STRMAN_H__

#pragma once

#define MAX_STRING_SIZE 4048
#define STRING_NAME_FORMAT "PXString_%i_%i"
#define MAX_SYMBOLS 256
#define ALL_STRINGS -1


class CStringManager
{
public:
    CStringManager();
    virtual ~CStringManager();

    // initialization
    void Init(CSceneManager* pParent);

    // font creating
    BOOL CreateFont(const char* pcszConfig, const char* pcszFont);

    // string drawing and creating
    int Create(const char* pcszString, const char* pcszFont,
                int nWXPos, int nWYPos, int nWZPos);
	int CreateFromTable(const char* pcszVarName, const char* pcszFont,
				int nWXPos, int nWYPos, int nWZPos);

    void Delete(int nStringID);
    void DeleteAll();
    void Change(const char* pszString, int nStringID);
	void ChangeFromTable(const char* pcszVarName, int nStringID);
    void RedrawAll();
	void OnLocaleChanged();

    // string placement
    void GetPos(int nStringID, int* pnWXPos, int *pnWYPos, int* pnWZPos);
    void SetPos(int nStringID, int nWXPos, int nWYPos, int nWZPos);

    // string hiding
    void SetVisible(BOOL bVisible, int nStringID = ALL_STRINGS);
    BOOL GetVisible(int nStringID);

private:
    typedef struct {
        char buf[MAX_STRING_SIZE];
        char old[MAX_STRING_SIZE];
        char font[MAX_BUFFER];
        int id;
        int x;
        int y;
        int z;
        BOOL vis;
		BOOL fFromTable;
		char szVarName[MAX_BUFFER];
    } PXString;

    typedef struct {
        char symbol;
        int frame;
    } PXSymbol;

    typedef struct {
        char sprite[MAX_BUFFER];
        char font[MAX_BUFFER];
        PXSymbol symbols[MAX_SYMBOLS];
        HFONT hFont;
        COLORREF crColor;
    } PXFont;

    typedef std::map<int, PXString*> STRM_HASH_MAP;
    typedef STRM_HASH_MAP::iterator STRM_HASH_MAP_ITER;

    typedef std::map<std::string, PXFont*> FONTS_HASH_MAP;
    typedef FONTS_HASH_MAP::iterator FONTS_HASH_MAP_ITER;

private:
    STRM_HASH_MAP     m_hmStrings;
    FONTS_HASH_MAP    m_hmFonts;
    CSceneManager*    m_pParent;
    int               m_nCounter;

private:
    void Free();
    void Draw(PXString* pStr);
    int GetPXString(int id, PXString** ppStr);
    int GetPXFont(const char* pcszFont, PXFont** ppFont);
    BOOL Symbol2Frame(PXFont* pFont, char symbol, int* pnFrame);
    BOOL CreateSystemFont(CVars* pVars, PXFont* pFont, 
                            const char* pcszFont, const char* pcszSystemFontName);
};

#endif // __STRMAN_H__
