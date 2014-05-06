//-----------------------------------------------------------------------------
// File: parser.h
//
// Desc: script reader
//
// Copyright (c) 2002 PlainX Group
// 
//-----------------------------------------------------------------------------

#ifndef __PARSER_H__
#define __PARSER_H__

#pragma once


class CParser
{
public:
    CParser();
    virtual ~CParser();

    // init methods
    BOOL Open(const char* pszFilename);
    void Close();

    // access methods
    BOOL GetVar(const char* pszGroupLabel, const char* pszGroupName, const char* pszVarName, char* pszVarValue);
    BOOL SetVar(const char* pszGroupLabel, const char* pszGroupName, const char* pszVarName, char* pszVarValue);

    BOOL GetNextGroup(const char* pszGroupLabel, char* ppszGroupName, BOOL bIsFirst = FALSE);

    int GetVarCount(const char* pszGroupLabel, const char* pszGroupName, const char* pszVarName);

private:
    FILE*                       m_pFile;
    BYTE                        m_Key[256];
    char                        m_tmpname[MAX_PATH];
    char                        m_origname[MAX_PATH];
    BOOL                        m_bOpened;
    BOOL                        m_bIsCrypted;
    GROUP_VAR_LIST              m_vars;
    GROUP_VAR_LIST_ITERATOR     m_ivars;

private:
    void p_TrimBuffer(char* buffer);

    BOOL p_Encrypt(const char* pszFilename);
    BYTE p_GetECode(BYTE nCode);

    void p_Crypt();
    void p_Flush();
    BYTE p_GetCCode(BYTE nCode);

    BOOL p_GoToSection(char* pszGroupLabel, char* pszGroupName, BOOL bFirst = FALSE);
    void p_ReadSection(const char* pszFullGroupName);

    BOOL p_CheckForCrypt(const char* pszFilename);
};

#endif //__PARSER_H__
