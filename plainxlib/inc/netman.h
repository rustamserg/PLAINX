//-----------------------------------------------------------------------------
// File: netman.h
//
// Desc: DirectX DirectPlay routine
//
// Copyright (c) 2002 PlainX Group
// 
//-----------------------------------------------------------------------------

#ifndef __NETMAN_H__
#define __NETMAN_H__

#pragma once

// init
BOOL Network_Init(HWND hWnd);
void Network_Free();

// connections
BOOL Network_CreateSession(const char* pszSessionName, int nMaxPlayers = 2);
BOOL Network_ConnectToSession(const char* pszSessionName, const char* pcszServerIP);
BOOL Network_GetSessionsStats(NM_SESSIONS_STATS* pStats);
BOOL Network_CloseSession();

// players
BOOL Network_CreatePlayer(const char* pszPlayerName);
DPID Network_GetPlayerId();
E_NMSG* Network_GetPlayerName(DPID idPlayer);
BOOL Network_DestroyPlayer();

// communications
BOOL Network_SetPlayerData(void* pData, DWORD dwSize);
E_NMSG* Network_GetPlayerData(DPID idPlayer);

BOOL Network_Send(void* pData, DWORD dwSize);
HRESULT Network_Receive(E_NMSG* pnmsg, BOOL* bRec, DPID* pFromID);

// network message struct support
E_NMSG* Network_CreateNMSG();
BOOL Network_FillNMSG(E_NMSG* pnmsg, void* pData, DWORD dwSize);
void Network_DestroyNMSG(E_NMSG* pnmsg);

#endif //__NETMAN_H__
