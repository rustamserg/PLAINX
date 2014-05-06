//-----------------------------------------------------------------------------
// File: log.h
//
// Desc: Log support
//
// Copyright (c) 2002 PlainX Group
// 
//-----------------------------------------------------------------------------

#ifndef __LOG_H__
#define __LOG_H__

#pragma once


void PX_LogOpen(char* logname);
void PX_LogClose();
void PX_LogWriteString(const char* message);
void PX_LogWriteInteger(const char* message, int value);
void PX_ErrorHandle(const char* pszError, BOOL bTerminate = TRUE);
const char* PX_GetVersion();

#endif //__LOG_H__
