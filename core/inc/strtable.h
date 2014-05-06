//-----------------------------------------------------------------------------
// File: strtable.h
//
// Desc: String table
//
// Copyright (c) 2005 PlainX Group
// 
//-----------------------------------------------------------------------------

#ifndef __STRTABLE_H__
#define __STRTABLE_H__

#pragma once

class CStringTable
{
public:
	CStringTable();
	virtual ~CStringTable();

	// initialization
	BOOL Init(const char* pcszConfig);

	// locale manage
	BOOL SetLocale(const char* pcszLocale);
	const char* GetLocale();

	// get string from table
	const char* GetString(const char* pcszVarName);

private:
	char m_szBuffer[MAX_BUFFER + 1];
	char m_szLocale[MAX_BUFFER + 1];
	CVars* m_pVars;
	BOOL m_fInited;
};

#endif // __STRTABLE_H__
