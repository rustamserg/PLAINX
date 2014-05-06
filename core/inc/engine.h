//-----------------------------------------------------------------------------
// File: engine.h
//
// Desc: Engine core
//
// Copyright (c) 2002 PlainX Group
// 
//-----------------------------------------------------------------------------

#ifndef __ENGINE_H__
#define __ENGINE_H__

#pragma once


class CEngine
{
public:
    CEngine();
    virtual ~CEngine();

    // init methods
    BOOL InitEngine(const char* pszRoot, BOOL bWindowed = TRUE);
    void FreeEngine();

    // accessors
    BOOL                GetWindowed();
    DWORD               GetLastTick();
    char*               GetRoot();
    CVideoManager*      GetVideo();
    CSoundManager*      GetAudio();
    CVars*              GetVars();
    HWND                GetHWnd();
    CResourceManager*   GetResource(const char* pszName);
    CSceneManager*      GetScene(const char* pszName);
    CVars*              GetVars(const char* pszName);
    BOOL                GetActive();
    RECT                GetClientRect();
    BOOL                GetSafeMode();

    // mutators
    void SetWindowed(BOOL bWindowed);
    void SetLastTick(DWORD dwLastTick);
    void SetActive(BOOL bActive);
    void SetHWnd(HWND hWnd);
    void SetSafeMode(BOOL bIsSafeMode);

    // mouse support
    BOOL CreateMouse(const char* pcszResourceName);
    void SetUseMouse(BOOL bIsUseMouse);
    BOOL GetUseMouse();
    void SetMouseSpeed(BYTE nMouseSpeed);
    BYTE GetMouseSpeed();
    CSprite* GetMouseSprite();
    void SetMouseSprite(CSprite* pSprite);

    // surface update methods
    void ReloadAll();
    void RecreateAll();

    // input update method
    void ReloadInput();

    // engine update methods
    HRESULT UpdateEngine();
    void AddDirtScreenRect(RECT* prcRect);
    void OnChangeScenePos(const char* pcszName);

    // init directx methods
    BOOL InitVideo(DWORD* pdwWidth = NULL, DWORD* pdwHeight = NULL);
    BOOL InitInput();
    BOOL InitNetwork();
    BOOL InitAudio();

    // process directx
    void ProcessMouse();
    void ProcessKeyboard();
    void ProcessNetwork(); 

    // process engine states
    void PostState(DWORD dwStateID, LPARAM lParam, WPARAM wParam);
    E_STATE GetState();
    virtual BOOL ProcessState(E_STATE eState, DWORD dwCurrTick);

    // state support
    void PostState(DWORD dwStateID, LPARAM lParam, WPARAM wParam, const char* pszSceneName, const char* pszListenerName = NULL);

    // resource support
    BOOL AddResource(const char* pszConfig, const char* pszName);
    BOOL DelResource(const char* pszName);

    // scene support
    BOOL AddScene(const char* pszConfig, const char* pszName, const char* pszResourceName);
    BOOL DelScene(const char* pszName);

    // vars support
    BOOL AddVars(const char* pszConfig, const char* pszName);
    BOOL DelVars(const char* pszName);

    // message support
    void PostMessage(DWORD dwMessageID, LPARAM lParam, WPARAM wParam);
    BOOL GetMessage(E_MESSAGE* peMessage);
    void ClearMessages();

    // real-time vars support
    char* GetRTStringVar(const char* pszVarName);
    void SetRTStringVar(const char* pszVarName, char* pszVarValue);
    int GetRTIntVar(const char* pszVarName);
    void SetRTIntVar(const char* pszVarName, int nVarValue);
    void* GetRTPtrVar(const char* pszVarName);
    void SetRTPtrVar(const char* pszVarName, void* pVarValue);

    // tick support
    void SetCurrTick(DWORD dwCurrTick);
    DWORD GetCurrTick();

    // console support
    virtual void ProcessConsole(const char* pcszConCmd);
    void SetConsoleStatus(const char* pcszConStatus);

    // network support
    BOOL CreateSession(const char* pszSessionName, int nMaxPlayers = 2);
    BOOL ConnectToSession(const char* pszSessionName, const char* pcszServerIP);
    BOOL GetSessionsStats(NM_SESSIONS_STATS* pStats);
	BOOL CloseSession();

    BOOL CreateLocalPlayer(const char* pszPlayerName);
    DPID GetLocalPlayerId();
    E_NMSG* GetPlayerName(DPID idPlayer);
	BOOL DestroyPlayer();

    // communications
    BOOL SetLocalPlayerData(void* pData, DWORD dwSize);
    E_NMSG* GetPlayerData(DPID idPlayer);
    BOOL SendData(void* pData, DWORD dwSize);

    // sound core support
    virtual CSoundCore* CreateSoundCore();
    virtual void DeleteSoundCore(CSoundCore* pSndCore);

	// access to string table
	CStringTable* GetStringTable() { return m_pStrTable; }

private:
    typedef struct _E_ELEMENT {
        char  szName[MAX_BUFFER];
        char  szPath[MAX_PATH];
        void* pElement;
        BOOL  bIsGarbage;
    } E_ELEMENT;

    typedef std::map<std::string, E_ELEMENT*> E_HASH_LIST;
    typedef E_HASH_LIST::iterator E_HASH_LIST_ITERATOR;
    typedef std::vector<CSceneManager*> E_SCENES;
    typedef std::list<RECT> E_REGION;
    typedef E_REGION::iterator E_REGION_ITER;

private:
    DWORD               m_dwCurrTick;
    CInputManager*      m_pIM;
    CSoundManager*      m_pSnd;
    E_HASH_LIST         m_pRM;
    E_HASH_LIST         m_pSM;
    E_HASH_LIST         m_pVars;
    VAR_LIST            m_pRTVars; //real-time vars 
    int                 m_nNumOfVars;
    int                 m_nNumOfRM;
    int                 m_nNumOfSM;
    DWORD               m_dwLastTick;
    BOOL                m_bWindowed;
    E_REGION            m_Regions;

    CResourceManager*   m_pConRes; // console resource manager
    CSceneManager*      m_pConScn; // console scene
    CConsole*           m_pConList; // console listener

    BOOL                m_bUseMouse;
    CSceneManager*      m_pCurScn; // mouse scene
    CCursor*            m_pCurList; // mouse listener

    CVars*              m_pINIVars;
    char                m_szRoot[MAX_PATH];

	CStringTable*		m_pStrTable; // string table
    E_QSTATE            m_qState;

    CVideoManager*      m_pVideo;
    HWND                m_hWnd;
    BOOL                m_bIsActive;
    int                 m_nMouseSpeed;
    POINT               m_ptCursorPos;
    POINT               m_ptCursorPosLast;
    RECT                m_rcClientRect;

    E_QMESSAGE          m_qMessage; // message queue

    E_SCENES            m_pSMOrdered; // scenes ordered by z

    BOOL                m_bIsSafeMode; // if engine run in safe mode
    BOOL                m_bIsConsole;

    CSoundCore*         m_pSndCore; // cound system implementation

private:
    inline HRESULT      e_UpdateVideo();
    inline HRESULT      e_UpdateLogic(DWORD dwCurrTick);
    CResourceManager*   e_FindResource(const char* pszName);
    CSceneManager*      e_FindScene(const char* pszName);
    void                e_CalculateFPS();
    inline void         e_InitDirtRects();
    void                e_ProcessState(DWORD dwCurrTick);
    void                e_ClearStates();
    BOOL                e_GetState(E_STATE* peState);
    void                e_DelState();
    void                e_DelMessage();
    void                e_OptimizeRegions();
    int                 e_Fragment(RECT& r1,RECT& r2,RECT& r3);
    void                e_GetSystemMessage(E_NMSG* pnmsg);
    BOOL                e_DelResource(const char* pszName);
    BOOL                e_DelScene(const char* pszName);
    void                e_GarbageCollect();
    BOOL                e_CreateConsoleScripts();
    void                e_DestroyConsoleScripts();
    BOOL                e_CreateMouseScript();
    void                e_DestroyMouseScript();
};

#endif //__ENGINE_H__