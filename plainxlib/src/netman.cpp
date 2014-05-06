#define INITGUID

#include "plainx.h"
#include "netman.h"


extern GUID PXAPP_GUID;

struct NM_CONNECTIONDESC {
	char szPlayerName[MAX_BUFFER];
	char szSessionName[MAX_BUFFER];
	BOOL bIsServer;
};

LPDIRECTPLAY4A			g_pDPlay = NULL;
NM_CONNECTIONDESC		g_cDesc;
DPID					g_LocalPlayerID = NULL;
HANDLE					g_hEvent = NULL;

//forward declaration
BOOL FAR PASCAL DPConnect_EnumSessionsCallback(LPCDPSESSIONDESC2, DWORD*, DWORD, VOID*);
BOOL FAR PASCAL DPConnect_EnumStatsCallback(LPCDPSESSIONDESC2, DWORD*, DWORD, VOID*);
static BOOL nm_InitConnection(const char* pcszServerIP);


//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
BOOL Network_Init(HWND hWnd)
{
	memset(&g_cDesc, 0, sizeof(g_cDesc));
	return TRUE;
}


//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
void Network_Free()
{
	if (g_pDPlay) {
		g_pDPlay->Close();
		SAFE_RELEASE(g_pDPlay);
	}
	if (g_hEvent != 0) {
		CloseHandle(g_hEvent);
		g_hEvent = 0;
	}
}


//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
DPID Network_GetPlayerId()
{ 
	return g_LocalPlayerID;
}


//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
BOOL Network_CreateSession(const char* pszSessionName, int nMaxPlayers)
{
	DPSESSIONDESC2 dpsd;
	HRESULT hRet;

	if (!nm_InitConnection(NULL))
		return FALSE;

	strcpy(g_cDesc.szSessionName, pszSessionName);
	g_cDesc.bIsServer = TRUE;

	memset(&dpsd, 0, sizeof(dpsd));
	dpsd.dwSize = sizeof(dpsd);
	dpsd.lpszSessionNameA = g_cDesc.szSessionName;
	dpsd.guidApplication = PXAPP_GUID;
	dpsd.dwMaxPlayers = nMaxPlayers;
	dpsd.dwFlags = DPSESSION_KEEPALIVE;

	hRet = g_pDPlay->Open(&dpsd, DPOPEN_CREATE);
	if (FAILED(hRet))
		return FALSE;

	return TRUE;
}


//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
BOOL Network_ConnectToSession(const char* pszSessionName, const char* pcszServerIP)
{
	DPSESSIONDESC2 dpsd;
	HRESULT hRet;
	BOOL bCon = FALSE;

	if (!nm_InitConnection(pcszServerIP))
		return FALSE;
	
	g_cDesc.bIsServer = FALSE;
	strcpy(g_cDesc.szSessionName, pszSessionName);

	memset(&dpsd, 0, sizeof(dpsd));
	dpsd.dwSize = sizeof(dpsd);
	dpsd.guidApplication = PXAPP_GUID;

	hRet = g_pDPlay->EnumSessions(&dpsd, 0, DPConnect_EnumSessionsCallback,
									&bCon, DPENUMSESSIONS_ALL);
	if (FAILED(hRet))
		return FALSE;

	return bCon;
}


//-----------------------------------------------------------------------------
// Name: 
// Desc: create local player
//-----------------------------------------------------------------------------
BOOL Network_CreatePlayer(const char* pszPlayerName)
{
	DPNAME dpname;
	HRESULT hRet;

	if (g_pDPlay == NULL)
		return FALSE;

	strcpy(g_cDesc.szPlayerName, pszPlayerName);
	g_hEvent = CreateEvent(NULL, FALSE, FALSE, NULL);

	memset(&dpname, 0, sizeof(DPNAME));
	dpname.dwSize = sizeof(DPNAME);
	dpname.lpszShortNameA = g_cDesc.szPlayerName;

	hRet = g_pDPlay->CreatePlayer(&g_LocalPlayerID, &dpname, g_hEvent, NULL, 0, 0);
	if (FAILED(hRet))
		return FALSE;

	return TRUE;
}


//-----------------------------------------------------------------------------
// Name: 
// Desc: change local player data
//-----------------------------------------------------------------------------
BOOL Network_SetPlayerData(void* pData, DWORD dwSize)
{
	HRESULT hRet;

	if (g_pDPlay == NULL)
		return FALSE;

	hRet = g_pDPlay->SetPlayerData(g_LocalPlayerID, pData, dwSize,
									DPSET_GUARANTEED | DPSET_REMOTE);
	if (FAILED( hRet))
		return FALSE;

	return TRUE;
}


//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
E_NMSG* Network_GetPlayerName(DPID idPlayer)
{
	HRESULT hr;

	if (g_pDPlay == NULL)
		return NULL;

	DWORD dwDataSize;
	E_NMSG* pNmsg = NULL;

	hr = g_pDPlay->GetPlayerName(idPlayer, NULL, &dwDataSize);
	if (SUCCEEDED(hr)) {
		char* pTempData = new char[dwDataSize];
		if (pTempData) {
			hr = g_pDPlay->GetPlayerName(idPlayer, pTempData, &dwDataSize);
			if (SUCCEEDED(hr)) {
				pNmsg = Network_CreateNMSG();
				if (pNmsg) {
					Network_FillNMSG(pNmsg, pTempData, dwDataSize);
				}
			}
			delete[] pTempData;
		}
	}
	return pNmsg;
}


//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
E_NMSG* Network_GetPlayerData(DPID idPlayer)
{
	HRESULT hr;

	if (g_pDPlay == NULL)
		return NULL;

	DWORD dwDataSize;
	E_NMSG* pNmsg = NULL;

	hr = g_pDPlay->GetPlayerData(idPlayer, NULL, &dwDataSize, DPGET_REMOTE);
	if (SUCCEEDED(hr)) {
		char* pTempData = new char[dwDataSize];
		if (pTempData) {
			hr = g_pDPlay->GetPlayerData(idPlayer, pTempData, &dwDataSize, DPGET_REMOTE);
			if (SUCCEEDED(hr)) {
				pNmsg = Network_CreateNMSG();
				if (pNmsg) {
					Network_FillNMSG(pNmsg, pTempData, dwDataSize);
				}
			}
			delete[] pTempData;
		}
	}
	return pNmsg;
}


//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
BOOL Network_Send(void* pData, DWORD dwSize)
{
	HRESULT hRet;

	if (g_pDPlay == NULL)
		return FALSE;
	
	hRet = g_pDPlay->SendEx(g_LocalPlayerID, DPID_ALLPLAYERS,
							DPSEND_ASYNC | DPSEND_GUARANTEED | DPSEND_NOSENDCOMPLETEMSG,
							pData, dwSize, 0, NULL, NULL, NULL);
	if (FAILED (hRet))
		return FALSE;

	return TRUE;
}


//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
HRESULT Network_Receive(E_NMSG* pnmsg, BOOL* bRec, DPID* pFromID)
{
	HRESULT hRet;

	pnmsg->pData = NULL;
	pnmsg->dwSize = 0;

	*bRec = FALSE;

	if (g_pDPlay == NULL)
		return S_OK;

	while (TRUE) {
		hRet = g_pDPlay->Receive(pFromID, &g_LocalPlayerID, DPRECEIVE_TOPLAYER,
									pnmsg->pData, &(pnmsg->dwSize));

		if (hRet == DPERR_BUFFERTOOSMALL) {
			if (pnmsg->pData) {
				delete[] pnmsg->pData;
				pnmsg->pData = NULL;
			}
			pnmsg->pData = new char[pnmsg->dwSize];
			if (!pnmsg->pData) return E_OUTOFMEMORY;
			continue;
		}
		if (hRet == DPERR_NOMESSAGES) return hRet;
		if (FAILED(hRet)) return hRet;

		*bRec = TRUE;
		break;
	}
	return hRet;
}


//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
E_NMSG* Network_CreateNMSG()
{
	E_NMSG* pnmsg = new E_NMSG[1];
	if (pnmsg) {
		pnmsg->pData = NULL;
		pnmsg->dwSize = 0;
	}
	return pnmsg;
}


//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
void Network_DestroyNMSG(E_NMSG* pnmsg)
{
	if (pnmsg) {
		if (pnmsg->pData) delete[] (pnmsg->pData);
		delete[] pnmsg;
	}
}


//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
BOOL Network_FillNMSG(E_NMSG* pnmsg, void* pData, DWORD dwSize)
{
	if (pnmsg) {
		if (pnmsg->pData) delete[] (pnmsg->pData);
		pnmsg->pData = new char[dwSize + 1];
		if (pnmsg->pData == 0)
			return FALSE;

		memset(pnmsg->pData, 0, dwSize + 1);
		memcpy(pnmsg->pData, pData, dwSize);
	}
	return TRUE;
}


//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
BOOL Network_GetSessionsStats(NM_SESSIONS_STATS* pStats)
{
	if (pStats == NULL)
		return FALSE;

	DPSESSIONDESC2 dpsd;
	HRESULT hRet;

	memset(&dpsd, 0, sizeof(dpsd));
	dpsd.dwSize = sizeof(dpsd);
	dpsd.guidApplication = PXAPP_GUID;

	pStats->iRealSize = 0;

	hRet = g_pDPlay->EnumSessions(&dpsd, 0, DPConnect_EnumStatsCallback,
									pStats, DPENUMSESSIONS_ALL);

	if (FAILED(hRet))
		return FALSE;

	return TRUE;
}


//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
BOOL Network_CloseSession()
{
	HRESULT hRet;

	hRet = g_pDPlay->Close();
	if (FAILED(hRet))
		return FALSE;

	return TRUE;
}


//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
BOOL Network_DestroyPlayer()
{
	HRESULT hRet;

	hRet = g_pDPlay->CancelMessage(0, 0);
	if (SUCCEEDED(hRet))
	{
		hRet = g_pDPlay->DestroyPlayer(g_LocalPlayerID);
		if (SUCCEEDED(hRet))
			return TRUE;
	}
	return FALSE;
}


//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
static BOOL nm_InitConnection(const char* pcszServerIP)
{
	HRESULT					hRet;
	LPDIRECTPLAYLOBBY3A		pDPLobby = NULL;


	// Create a ANSI DirectPlay interface
	hRet = CoCreateInstance(CLSID_DirectPlay, NULL, CLSCTX_INPROC_SERVER,
							IID_IDirectPlay4A, (VOID**)&g_pDPlay);
	if (FAILED(hRet))
		return FALSE;

	// Get ANSI DirectPlayLobby interface
	hRet = CoCreateInstance(CLSID_DirectPlayLobby, NULL, CLSCTX_INPROC_SERVER,
							IID_IDirectPlayLobby3A, (VOID**)&pDPLobby);
	if (FAILED(hRet))
		return FALSE;

	DPCOMPOUNDADDRESSELEMENT addressElements[3];
	CHAR strIPAddressString[MAX_BUFFER];
	VOID* pAddress = NULL;
	DWORD dwAddressSize = 0;
	DWORD dwElementCount = 0;

	// TCP/IP needs a service provider, an IP address

	// Service provider
	addressElements[dwElementCount].guidDataType = DPAID_ServiceProvider;
	addressElements[dwElementCount].dwDataSize = sizeof(GUID);
	addressElements[dwElementCount].lpData = (VOID*)&DPSPGUID_TCPIP;
	dwElementCount++;

	// IP address string
	if (pcszServerIP != NULL) {
		lstrcpy(strIPAddressString, pcszServerIP);
	} else {
		lstrcpy(strIPAddressString, "");
	}
	addressElements[dwElementCount].guidDataType = DPAID_INet;
	addressElements[dwElementCount].dwDataSize = lstrlen(strIPAddressString) + 1;
	addressElements[dwElementCount].lpData = strIPAddressString;
	dwElementCount++;

	// See how much room is needed to store this address
	hRet = pDPLobby->CreateCompoundAddress(addressElements, dwElementCount, NULL,
												&dwAddressSize);
	if (hRet != DPERR_BUFFERTOOSMALL) {
		SAFE_RELEASE(pDPLobby);
		return FALSE;
	}

	// Allocate space
	pAddress = GlobalAllocPtr(GHND, dwAddressSize);
	if (pAddress == NULL) {
		SAFE_RELEASE(pDPLobby);
		return FALSE;
	}

	// Create the address
	hRet = pDPLobby->CreateCompoundAddress(addressElements, dwElementCount, pAddress,
												&dwAddressSize);
	if (FAILED(hRet)) {
		SAFE_RELEASE(pDPLobby);
		GlobalFreePtr(pAddress);
		return FALSE;
	}

	// initialize connection
	hRet = g_pDPlay->InitializeConnection(pAddress, 0);
	if (FAILED(hRet)) {
		SAFE_RELEASE(pDPLobby);
		GlobalFreePtr(pAddress);
		return FALSE;
	}
	return TRUE;
}


//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
BOOL FAR PASCAL DPConnect_EnumSessionsCallback(LPCDPSESSIONDESC2 pdpsd, DWORD* pdwTimeout,
												DWORD dwFlags, VOID* pvContext)
{
	HRESULT hRet;

	if (dwFlags & DPESC_TIMEDOUT)
		return FALSE;

	if (strcmp(pdpsd->lpszSessionNameA, g_cDesc.szSessionName) == 0) {
		DPSESSIONDESC2 dpsd;
		memset(&dpsd, 0, sizeof(dpsd));
		dpsd.dwSize = sizeof(dpsd);
		dpsd.guidInstance = pdpsd->guidInstance;
		dpsd.guidApplication = PXAPP_GUID;

		hRet = g_pDPlay->Open(&dpsd, DPOPEN_JOIN);
		if (FAILED(hRet))
			return FALSE;

		*(BOOL*) pvContext = TRUE;
		return FALSE; //for stop enumerations
	}

	return TRUE;
}


//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
BOOL FAR PASCAL DPConnect_EnumStatsCallback(LPCDPSESSIONDESC2 pdpsd, DWORD* pdwTimeout,
											DWORD dwFlags, VOID* pvContext)
{
	if (dwFlags & DPESC_TIMEDOUT)
		return FALSE;

	NM_SESSIONS_STATS* pStats = (NM_SESSIONS_STATS*)pvContext;

	if (pStats->iRealSize <= pStats->iSize) {
		NM_SESSION_INFO* pInfo = &(pStats->pInfos[pStats->iRealSize]);

		strcpy(pInfo->szSessionName, pdpsd->lpszSessionNameA);
		pInfo->iMaxPlayers = pdpsd->dwMaxPlayers;
		pInfo->iCurrentPlayers = pdpsd->dwCurrentPlayers;
	}
	pStats->iRealSize++;
	return TRUE;
}
