//-----------------------------------------------------------------------------
// File: vars.h
//
// Desc: Support many vars sources
//
// Copyright (c) 2002 PlainX Group
// 
//-----------------------------------------------------------------------------

#ifndef __VARS_H__
#define __VARS_H__

#pragma once


class CVars
{
public:
    CVars();
    virtual ~CVars();

    // init methods
    BOOL Init(const char* pszPath);

    // get methods
    BOOL GetInt(const char* pszGroupLabel, const char* pszGroupName, const char* pszVarName, int nPos, int* pnVarValue);
    BOOL GetString(const char* pszGroupLabel, const char* pszGroupName, const char* pszVarName, char* pszVarValue);

    // set methods
    BOOL SetInt(const char* pszGroupLabel, const char* pszGroupName, const char* pszVarName, int nPos, int nVarValue);
    BOOL SetString(const char* pszGroupLabel, const char* pszGroupName, const char* pszVarName, char* pszVarValue);

    BOOL GetNextGroup(const char* pszGroupLabel, char* pszGroupName, BOOL bIsFirst = FALSE);

private:
    CParser* m_pParser;
};

#endif // __VARS_H__


