#include "plainx.h"
#include "fmodcore.h"
#include "engine.h"

HWND g_hWnd = NULL;

#define L_ENGINE_INI		"plainx.ini"
#define L_ENGINE_FONT		"px_cfont.fnt"
#define L_ENGINE_CONSOLE	"px_console.scn"
#define L_ENGINE_CURSOR		"px_cursor.scn"
#define L_ENGINE_CON_NAME	"console"
#define L_ENGINE_CUR_NAME	"cursor"


CEngine::CEngine()
{
  m_pIM = NULL;
  m_pSnd = NULL;
  m_pSndCore = NULL;
  m_nNumOfRM = 0;
  m_nNumOfSM = 0;
  m_nNumOfVars = 0;
  m_bIsSafeMode = FALSE;
  m_bIsConsole = FALSE;

  m_pConRes = NULL;
  m_pConScn = NULL;
  m_pConList = NULL;

  m_pCurList = NULL;
  m_pCurScn = NULL;
  m_bUseMouse = FALSE;

  m_pINIVars = NULL;
  m_pVideo = NULL;

  m_ptCursorPos.x = 0;
  m_ptCursorPos.y = 0;
  m_ptCursorPosLast.x = 0;
  m_ptCursorPosLast.y = 0;

  m_pStrTable = NULL;

  e_InitDirtRects();
  e_ClearStates();
  ClearMessages();
}


CEngine::~CEngine()
{
}


//-----------------------------------------------------------------------------
// Name: InitEngine()
// Desc: Initialize all engine vars and create var's source
//-----------------------------------------------------------------------------
BOOL CEngine::InitEngine(const char* pszRoot, BOOL bWindowed)
{
  m_bWindowed = bWindowed;
  m_dwLastTick = timeGetTime();

  m_nMouseSpeed = 2;
  m_bIsActive = FALSE;

  //create vars source
  m_pINIVars = new CVars();
  if (!m_pINIVars)
    return FALSE;

  // create string table object
  m_pStrTable = new CStringTable();
  if (!m_pStrTable)
	  return FALSE;

  char fp[MAX_PATH];
  _makepath(fp, NULL, pszRoot, L_ENGINE_INI, NULL);
  if (!m_pINIVars->Init(fp))
    return FALSE;

  //get mouse speed
  GetVars()->GetInt(L_GROUP, L_MS_MOUSE, L_MS_MOVESPEED, 0, &m_nMouseSpeed);

  //remember root folder for media
  strcpy(m_szRoot, pszRoot);

  //start engine
  PostState(ES_STARTENGINE, 0, 0);

  return TRUE;
}

//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
void CEngine::SetHWnd(HWND hWnd)
{
  m_hWnd = hWnd;
  g_hWnd = hWnd;
  ::GetClientRect(m_hWnd, &m_rcClientRect);
}


//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
void CEngine::SetUseMouse(BOOL bIsUseMouse)
{
	if (bIsUseMouse == m_bUseMouse)
		return;
	
	m_bUseMouse = bIsUseMouse;

	CSprite* pCursor = GetMouseSprite();
	if (pCursor) {
		pCursor->SetVisible(m_bUseMouse);
	}
}


//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
BOOL CEngine::GetUseMouse()
{
	return m_bUseMouse;
}


//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
CSprite* CEngine::GetMouseSprite()
{
	if (m_pCurList)
		return m_pCurList->GetCursor();

	return NULL;
}


//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
void CEngine::SetMouseSprite(CSprite* pSprite)
{
	if (m_pCurList) {
		m_pCurList->SetCursor(pSprite);
	}
}


//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
BOOL CEngine::CreateMouse(const char* pcszResourceName)
{
	// get resource manager
	CResourceManager* pRes = GetResource(pcszResourceName);
	if (pRes == NULL)
		return FALSE;

	if (!e_CreateMouseScript())
		return FALSE;

	// create scene
	m_pCurScn = new CSceneManager();
	if (!m_pCurScn) {
		e_DestroyMouseScript();
		return FALSE;
	}

	char fp[MAX_PATH];
	_makepath(fp, NULL, m_szRoot, L_ENGINE_CURSOR, NULL);

	// init scene
	if (!m_pCurScn->Init(fp, pRes, this, L_ENGINE_CUR_NAME)) {
		e_DestroyMouseScript();
		return FALSE;
	}

	e_DestroyMouseScript();

	// create listener
	m_pCurList = new CCursor();
	if (!m_pCurList)
		return FALSE;

	m_pCurList->SetCursor(m_pCurScn->GetSprite(L_ENGINE_CUR_NAME));
	m_pCurScn->AddListener(m_pCurList, L_ENGINE_CUR_NAME);

	SetUseMouse(TRUE);
	
	return TRUE;
}


//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
BOOL CEngine::InitVideo(DWORD* pdwWidth, DWORD* pdwHeight)
{
  //create video
  m_pVideo = new CVideoManager();
  if (!m_pVideo)
    return FALSE;
	
  int nW,nH;
  if (pdwWidth) {
    nW = *pdwWidth;
  } else {
    GetVars()->GetInt(L_GROUP, L_VID_VIDEO, L_VID_WIDTH, 0, &nW);
  }

  if (pdwHeight) {
    nH = *pdwHeight;
  } else {
    GetVars()->GetInt(L_GROUP, L_VID_VIDEO, L_VID_HEIGHT, 0, &nH);
  }
    
  int nBpp;
  GetVars()->GetInt(L_GROUP, L_VID_VIDEO, L_VID_BPP, 0, &nBpp);

  //create video surface
  if (!m_pVideo->Init(this))
    return FALSE;
  
  //check if we can run windowed
  if (m_bWindowed) 
    m_bWindowed = m_pVideo->IsCanBeWindowed(nBpp);
		
  if (!m_pVideo->SetMode(m_hWnd, &nW, &nH, &nBpp, m_bWindowed))
    return FALSE;
    
  // get free and total video memory
  int nFreeVidMem = m_pVideo->GetFreeVideoMemory();
  
  // get mininum video memory for usage
  int nMinVidMem = 1024;
  GetVars()->GetInt(L_GROUP, L_DD_DDRAW, L_DD_MIN_VIDEOMEM, 0, &nMinVidMem);

  if (nFreeVidMem > nMinVidMem) {
    SetRTIntVar(L_RT_USE_VID_MEM, 1);
  } else {
    SetRTIntVar(L_RT_USE_VID_MEM, 0);
  }

  if (!e_CreateConsoleScripts())
	  return FALSE;

	// create engine console
  m_pConScn = new CSceneManager();
  if (!m_pConScn) {
	e_DestroyConsoleScripts();
	return FALSE;
  }
		
  char fp[MAX_PATH];

  _makepath(fp, NULL, m_szRoot, L_ENGINE_CONSOLE, NULL);

  m_pConRes = new CResourceManager(this);
  if (!m_pConRes) {
	e_DestroyConsoleScripts();
	return FALSE;
  }
  
  if (!m_pConScn->Init(fp, m_pConRes, this, L_ENGINE_CON_NAME)) {
	e_DestroyConsoleScripts();
	return FALSE;
  }

  if (!m_pConScn->GetStringManager()->CreateFont(L_ENGINE_FONT, L_ENGINE_CON_FONT)) {
	e_DestroyConsoleScripts();
	return FALSE;
  }

  e_DestroyConsoleScripts();

  m_pConList = new CConsole();
  if (!m_pConList)
	return FALSE;

  m_pConScn->AddListener(m_pConList, L_ENGINE_CON_NAME);
  m_pConScn->SetVisible(FALSE);

  m_ptCursorPos.x = GetVideo()->GetScreenWidth() / 2;
  m_ptCursorPos.y = GetVideo()->GetScreenHeight() / 2;
  m_ptCursorPosLast.x = m_ptCursorPos.x;
  m_ptCursorPosLast.y = m_ptCursorPos.y;

  return TRUE;
}


//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
BOOL CEngine::InitInput()
{
  m_pIM = new CInputManager();
  if (!m_pIM)
    return FALSE;

  return m_pIM->InitDirectInput(m_hWnd, m_bWindowed);
}


//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
BOOL CEngine::InitNetwork()
{
	return Network_Init(m_hWnd);
}


//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
BOOL CEngine::InitAudio()
{
  m_pSnd = new CSoundManager();
  if (!m_pSnd)
    return FALSE;

  m_pSndCore = CreateSoundCore();
  if (m_pSndCore == NULL)
	  return FALSE;
  
  int err = m_pSnd->Init(this, m_pSndCore);
  
  if (err == SM_ERR_FAILED) {
    return FALSE;
  }
  return TRUE;
}


//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
void CEngine::FreeEngine()
{	
  for (E_HASH_LIST_ITERATOR i=m_pRM.begin(); i != m_pRM.end(); i++) {
    E_ELEMENT* elem = (E_ELEMENT*)(*i).second;
    if (elem) {
      CResourceManager* prm = (CResourceManager*)(elem->pElement);
      if (prm) {
        prm->Free();
	delete prm;
      }
      free(elem);
    }
  }
	
  for (E_HASH_LIST_ITERATOR i=m_pSM.begin(); i != m_pSM.end(); i++) {
    E_ELEMENT* elem = (E_ELEMENT*)(*i).second;
    if (elem) {
      CSceneManager* psm = (CSceneManager*)(elem->pElement);
      if (psm) {
	psm->Free();
	delete psm;
      }
      free(elem);
    }
  }
	
  for (E_HASH_LIST_ITERATOR i=m_pVars.begin(); i != m_pVars.end(); i++) {
    E_ELEMENT* elem = (E_ELEMENT*)(*i).second;
    if (elem) {
      CVars* pv = (CVars*)(elem->pElement);
      if (pv) 
        delete pv;
      free(elem);
    }
  }

  for (VAR_LIST_ITERATOR j=m_pRTVars.begin(); j != m_pRTVars.end(); j++) {
    VAR* pVar = (VAR*)(*j).second;
    if (pVar) 
      free(pVar);
  }
	
  m_pRM.clear();
  m_pSM.clear();
  m_pSMOrdered.clear();
  m_pVars.clear();
  m_pRTVars.clear();
		
  if (m_pIM) {
    m_pIM->RemoveDirectInput();
    delete m_pIM;
  }

  if (m_pSnd) {
    m_pSnd->Free();
    delete m_pSnd;
  }

  if (m_pSndCore) {
	  DeleteSoundCore(m_pSndCore);
	  m_pSndCore = NULL;
  }

  Network_Free();

  if (m_pVideo) { 
    m_pVideo->Free();
    delete m_pVideo;
  }

  if (m_pConRes) {
	  m_pConRes->Free();
	  delete m_pConRes;
  }

  if (m_pConScn) {
    m_pConScn->Free();
    delete m_pConScn;
  }
  
  if (m_pCurScn) {
	  m_pCurScn->Free();
	  delete m_pCurScn;
  }

  if (m_pINIVars) 
    delete m_pINIVars;

  if (m_pStrTable)
	delete m_pStrTable;

  m_pIM = NULL;
  m_pSnd = NULL;
  m_nNumOfRM = 0;
  m_nNumOfSM = 0;
  m_nNumOfVars = 0;

  m_pConScn = NULL;
  m_pConRes = NULL;
  m_pConList = NULL;

  m_bUseMouse = FALSE;
  m_pCurScn = NULL;
  m_pCurList = NULL;
  
  m_pINIVars = NULL;
  m_pStrTable = NULL;
  m_pVideo = NULL;

  e_InitDirtRects();
  e_ClearStates();
  e_DestroyConsoleScripts();
  e_DestroyMouseScript();
  ClearMessages();
}


//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
BOOL  CEngine::AddResource(const char* pszConfig, const char* pszName)
{
  if (pszConfig == NULL || pszName == NULL)
    return FALSE;
  
  E_ELEMENT* prm = (E_ELEMENT*)malloc(sizeof(E_ELEMENT));
  if (prm) {
    prm->pElement = NULL;
    prm->bIsGarbage = FALSE;
    strcpy(prm->szName, pszName);
			
    char fp[MAX_PATH];
    _makepath(fp, NULL, m_szRoot, pszConfig, NULL);

    strcpy(prm->szPath, fp);
    prm->pElement = new CResourceManager();
    if (((CResourceManager*)prm->pElement)->Init(fp, this, pszName)) {
      m_pRM[prm->szName] = prm;
      return TRUE;
    } else {
      ((CResourceManager*)prm->pElement)->Free();
      delete (CResourceManager*)prm->pElement;
      free(prm);
    }
  }
  return FALSE;
}


//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
BOOL CEngine::e_DelResource(const char* pszName)
{
  if (pszName == NULL)
    return FALSE;

  E_ELEMENT* elem = m_pRM[pszName];
  if (elem) {
    CResourceManager* prm = (CResourceManager*)(elem->pElement);
    if (prm) {
      prm->Free();
      delete prm;
    }
    m_pRM.erase(pszName);
    free(elem);
  }
  return TRUE;
}


//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
BOOL CEngine::DelResource(const char* pszName)
{
  if (pszName == NULL)
    return FALSE;

  E_ELEMENT* elem = m_pRM[pszName];
  if (elem) {
    elem->bIsGarbage = TRUE;
  }
  return TRUE;
}


//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
BOOL CEngine::AddVars(const char* pszConfig, const char* pszName)
{
  if (pszConfig == NULL || pszName == NULL)
    return FALSE;

  E_ELEMENT* pv = (E_ELEMENT*)malloc(sizeof(E_ELEMENT));
  if (pv) {
    pv->pElement = NULL;
    strcpy(pv->szName, pszName);
    m_pVars[pv->szName] = pv;
	
    char fp[MAX_PATH];
    _makepath(fp, NULL, m_szRoot, pszConfig, NULL);

    strcpy(pv->szPath, fp);
    pv->pElement = new CVars();
    ((CVars*)pv->pElement)->Init(fp);
  }
  return TRUE;
}

//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
BOOL CEngine::DelVars(const char* pszName)
{
  if (pszName == NULL)
    return FALSE;

  E_ELEMENT* elem = m_pVars[pszName];
  if (elem) {
    CVars* pv = (CVars*)(elem->pElement);
    if (pv) delete pv;	
      
    m_pVars.erase(pszName);
    free(elem);
  }
  return TRUE;
}


//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
CVars* CEngine::GetVars(const char* pszName)
{
  if (pszName == NULL)
    return NULL;

  E_ELEMENT* elem = m_pVars[pszName];
  if (elem) {
    CVars* pv = (CVars*)(elem->pElement);
    return pv;
  }
	
  return NULL;
}

//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
BOOL CEngine::AddScene(const char* pszConfig, 
                       const char* pszName, 
                       const char* pszResourceName)
{
  if (pszConfig == NULL || pszName == NULL || pszResourceName == NULL)
    return FALSE;

  E_ELEMENT* psm = (E_ELEMENT*)malloc(sizeof(E_ELEMENT));
  if (psm) {
    psm->pElement = NULL;
    psm->bIsGarbage = FALSE;
    strcpy(psm->szName, pszName);
		
    char fp[MAX_PATH];
    _makepath(fp, NULL, m_szRoot, pszConfig, NULL);

    strcpy(psm->szPath, fp);
    psm->pElement = new CSceneManager();

    CResourceManager* prm = e_FindResource(pszResourceName);
    if (prm == NULL) 
      return FALSE;
	
    if (((CSceneManager*)psm->pElement)->Init(fp, prm, this, pszName)) {
      //remember scene
      m_pSM[psm->szName] = psm;

      //add to z-order
      int nZ = ((CSceneManager*)psm->pElement)->GetZPos();
      int nVecPos = m_pSMOrdered.size();
      for (size_t i=0; i<m_pSMOrdered.size(); i++) {
        CSceneManager* pSM = m_pSMOrdered[i];
	if (pSM) {
	  int nZPos = pSM->GetZPos();
	  if (nZPos >= nZ) {
	    nVecPos = i;
	    break;
	  }
        }
      }

      //insert to vector in pos defined in the nPos;
      m_pSMOrdered.insert(m_pSMOrdered.begin() + nVecPos, (CSceneManager*)psm->pElement);
      return TRUE;

    } else {

      ((CSceneManager*)psm->pElement)->Free();
      delete (CSceneManager*)psm->pElement;
      free(psm);
    }
  }
  return FALSE;
}

//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
BOOL CEngine::e_DelScene(const char* pszName)
{
  if (pszName == NULL)
    return FALSE;

  E_ELEMENT* elem = m_pSM[pszName];
  if (elem) {
    CSceneManager* psm = (CSceneManager*)(elem->pElement);
    if (psm) {
      //remove from z-ordered vector
      int nVecPos = -1;
      for (size_t i=0; i<m_pSMOrdered.size(); i++) {
        CSceneManager* pSM = m_pSMOrdered[i];
	if (pSM) {
	  if (strcmp(pSM->GetName(), pszName) == 0) {
            nVecPos = i;
	    break;
	  }
        }
      }
      if (nVecPos > -1) {
        m_pSMOrdered.erase(m_pSMOrdered.begin() + nVecPos);
      }
      psm->Free();
      delete psm;
    }
    m_pSM.erase(pszName);
    free(elem);
  }	
  return TRUE;
}


//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
BOOL CEngine::DelScene(const char* pszName)
{
  if (pszName == NULL)
    return FALSE;

  E_ELEMENT* elem = m_pSM[pszName];
  if (elem) {
    elem->bIsGarbage = TRUE;
  }	
  return TRUE;
}


//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
void CEngine::OnChangeScenePos(const char* pcszName)
{
  if (pcszName == NULL)
    return;

  E_ELEMENT* elem = m_pSM[pcszName];
  if (elem) {
    CSceneManager* psm = (CSceneManager*)(elem->pElement);
    if (psm) {
      //remove from z-ordered vector
      int nVecPos = -1;
      for (size_t i=0; i<m_pSMOrdered.size(); i++) {
        CSceneManager* pSM = m_pSMOrdered[i];
	if (pSM) {
	  if (strcmp(pSM->GetName(), pcszName) == 0) {
            nVecPos = i;
	    break;
	  }
        }
      }
      if (nVecPos > -1) {
        m_pSMOrdered.erase(m_pSMOrdered.begin() + nVecPos);
        
        //add to z-order
        int nZ = psm->GetZPos();
        nVecPos = m_pSMOrdered.size();
        for (size_t i=0; i<m_pSMOrdered.size(); i++) {
          CSceneManager* pSM = m_pSMOrdered[i];
	  if (pSM) {
	    int nZPos = pSM->GetZPos();
	    if (nZPos >= nZ) {
	      nVecPos = i;
	      break;
	    }
          }
        }

        //insert to vector in pos defined in the nPos;
        m_pSMOrdered.insert(m_pSMOrdered.begin() + nVecPos, psm);
      }
    }
  }
}


//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
void CEngine::ReloadAll()
{
  //reload resource managers
  for (E_HASH_LIST_ITERATOR i=m_pRM.begin(); i != m_pRM.end(); i++) {
    E_ELEMENT* elem = (E_ELEMENT*)(*i).second;
    if (elem) {
      CResourceManager* prm = (CResourceManager*)(elem->pElement);
      if (prm)
	prm->ReloadAll();
    }
  }

  if (m_pConRes)
	  m_pConRes->ReloadAll();
  
  if (m_pConScn)
    m_pConScn->ReloadSprites();

  if (m_pCurScn)
	  m_pCurScn->ReloadSprites();
	
  //reload scene managers
  for (E_HASH_LIST_ITERATOR i = m_pSM.begin(); i != m_pSM.end(); i++) {
    E_ELEMENT* elem = (E_ELEMENT*)(*i).second;
    if (elem) {
      CSceneManager* psm = (CSceneManager*)(elem->pElement);
      if (psm) {
	psm->ReloadSprites();
	psm->ClearCamera();
	psm->DirtCamera();
      }
    }
  }
}


//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
void CEngine::ReloadInput()
{
  if (m_pIM == NULL)
    return;

  // reload inputs
  m_pIM->RemoveDirectInput();
  m_pIM->InitDirectInput(m_hWnd, m_bWindowed);
}


//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
void CEngine::RecreateAll()
{
  // recreate resource managers
  for (E_HASH_LIST_ITERATOR i=m_pRM.begin(); i != m_pRM.end(); i++) {
    E_ELEMENT* elem = (E_ELEMENT*)(*i).second;
    if (elem) {
      CResourceManager* prm = (CResourceManager*)(elem->pElement);
      if (prm)
        prm->RecreateAll();
    }
  }

  if (m_pConRes)
      m_pConRes->RecreateAll();

  if (m_pConScn) {
    m_pConScn->RecreateSysFonts();
    m_pConScn->ReloadSprites();
  }

  if (m_pCurScn) {
    m_pCurScn->RecreateSysFonts();
    m_pCurScn->ReloadSprites();
  }

  //reload scene managers
  for (E_HASH_LIST_ITERATOR i = m_pSM.begin(); i != m_pSM.end(); i++) {
    E_ELEMENT* elem = (E_ELEMENT*)(*i).second;
    if (elem) {
      CSceneManager* psm = (CSceneManager*)(elem->pElement);
      if (psm) {
        psm->RecreateSysFonts();
        psm->ReloadSprites();
        psm->ClearCamera();
        psm->DirtCamera();
      }
    }
  }
}


//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
void CEngine::SetActive(BOOL bActive)
{
  static BOOL bIsNeedMute = FALSE;
  
  m_bIsActive = bActive;

  if (m_pIM) {
    m_pIM->AccessMouse(bActive);
    m_pIM->AccessKeyboard(bActive);
  }

  if (m_pSnd) {
    if (!m_bIsActive) {
      if (!m_pSnd->GetMute()) {
        bIsNeedMute = TRUE;
      }
      m_pSnd->SetMute(TRUE);
    } else {
      if (bIsNeedMute) {
        m_pSnd->SetMute(FALSE);
      }
      bIsNeedMute = FALSE;
    }
  }

  if (!m_bIsActive) {
    // remember cursor pos
    m_ptCursorPosLast.x = m_ptCursorPos.x;
    m_ptCursorPosLast.y = m_ptCursorPos.y;
  } else {
    // restore cursor pos
    m_ptCursorPos.x = m_ptCursorPosLast.x;
    m_ptCursorPos.y = m_ptCursorPosLast.y;
	if (m_bUseMouse)
		PostMessage(EM_MOUSEMOVE, m_ptCursorPos.x, m_ptCursorPos.y);
  }
}

//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
HRESULT CEngine::UpdateEngine()
{
  HRESULT hr = S_OK;

  // Figure how much time has passed since the last time
  DWORD dwCurrTick = timeGetTime();
  DWORD dwTickDiff = dwCurrTick - m_dwLastTick;
  DWORD dwTickFPSDiff = (DWORD)(1000/MAX_FPS);

  // set current tick
  SetCurrTick(dwCurrTick);

  // check if app is active
  if (!m_bIsActive)
    return S_OK;

  // Don't update if no time has passed 
  if (dwTickDiff > dwTickFPSDiff)
  {
    // remember last tick
    SetLastTick(dwCurrTick);

	// get current fps
    e_CalculateFPS();
      
    // update video
    hr = e_UpdateVideo();
    if (FAILED(hr))
      return hr;

    // update logic
    hr = e_UpdateLogic(dwCurrTick);
    if (FAILED(hr))
      return hr;
      
    // garbage collector
    e_GarbageCollect();
  }
  return hr; 
}


//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
void CEngine::e_GarbageCollect()
{
  E_HASH_LIST_ITERATOR iter;
  BOOL bIsIter;

  bIsIter = TRUE;
  while (bIsIter) {
    bIsIter = FALSE;
    for (iter = m_pSM.begin(); iter != m_pSM.end(); iter++) {
      E_ELEMENT* elem = (E_ELEMENT*)(*iter).second;
      if (elem && elem->bIsGarbage) {
        e_DelScene(elem->szName);
        bIsIter = TRUE;
        break;
      }
    }
  }

  bIsIter = TRUE;
  while (bIsIter) {
    bIsIter = FALSE;
    for (iter = m_pRM.begin(); iter != m_pRM.end(); iter++) {
      E_ELEMENT* elem = (E_ELEMENT*)(*iter).second;
      if (elem && elem->bIsGarbage) {
        e_DelResource(elem->szName);
        bIsIter = TRUE;
        break;
      }
    }
  }
}


//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
HRESULT CEngine::e_UpdateVideo()
{
  HRESULT hr;

  // Check the cooperative level before rendering
  hr = GetVideo()->TestCooperativeLevel();
  if (FAILED(hr))
    return hr;
  
  // draw screen
  e_OptimizeRegions();
		
  // update regions
  for (unsigned int j = 0; j < m_pSMOrdered.size(); j++) {
    CSceneManager* pSM = m_pSMOrdered[j];
    if (pSM) {
      for (E_REGION_ITER k = m_Regions.begin(); k != m_Regions.end(); k++) {
        pSM->DirtScreenRect(&(*k));
      }
    }
  }

  // console and mouse
  for (E_REGION_ITER k = m_Regions.begin(); k != m_Regions.end(); k++) 
  {
  	  // console
	  if (m_pConScn && m_bIsConsole)
		m_pConScn->DirtScreenRect(&(*k));

	  // mouse
	  if (m_pCurScn)
		  m_pCurScn->DirtScreenRect(&(*k));
  }
  
  // init regions for new rects
  e_InitDirtRects();

  // Display the sprites on the screen
  for (unsigned int j = 0; j < m_pSMOrdered.size(); j++) {
    CSceneManager* pSM = m_pSMOrdered[j];
    if (pSM) 
      pSM->UpdateDirtRects();
  }

  // console
  if (m_pConScn && m_bIsConsole) {
    m_pConScn->UpdateDirtRects();
  }

  // mouse
  if (m_pCurScn) {
	  m_pCurScn->UpdateDirtRects();
  }

  // Flip or blt the back buffer onto the primary buffer
  hr = GetVideo()->UpdateScreen();
  
  return hr;
}

//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
HRESULT CEngine::e_UpdateLogic(DWORD dwCurrTick)
{
  // process keyboard
  ProcessKeyboard();

  // process mouse
  if (m_bUseMouse)
	  ProcessMouse();
		
  // process network
  ProcessNetwork();

  // process mouse
  if (m_pCurScn) {
	  m_pCurScn->ProcessMessages(dwCurrTick);
  }

  // process console
  if (m_bIsConsole && m_pConScn) {
    m_pConScn->ProcessMessages(dwCurrTick);
  }

  // process states
  e_ProcessState(dwCurrTick);

  // process all scenes
  for (int j = m_pSMOrdered.size(); j > 0; j--) {
    CSceneManager* pSM = m_pSMOrdered[j-1];
    if (pSM) {
      if (pSM->ProcessMessages(dwCurrTick))
        break;
    }
  }

  // Display the sprites on the screen
  for (size_t j = 0; j < m_pSMOrdered.size(); j++) {
    CSceneManager* pSM = m_pSMOrdered[j];
    if (pSM) 
      pSM->UpdateSprites(dwCurrTick);
  }

  // console
  if (m_pConScn && m_bIsConsole) {
	m_pConScn->UpdateSprites(dwCurrTick);
  }

  // mouse
  if (m_pCurScn) {
	  m_pCurScn->UpdateSprites(dwCurrTick);
  }

  // remove message from queue
  E_MESSAGE eMessage;

  // if there is a message
  if (GetMessage(&eMessage))
  {
    // remember message id
    DWORD dwMessageID = eMessage.dwMessageID;

    // del message until message id is the same
    do {
      // check if we have network message
      if ((dwMessageID > EM_NMSG_MIN) && (dwMessageID < EM_NMSG_MAX))
	  {
        // for network messages we need delete E_NMSG struct
		E_NMSG* pnmsg = (E_NMSG*)(eMessage.lParam);
		Network_DestroyNMSG(pnmsg);
      }
      e_DelMessage();
    } while (GetMessage(&eMessage) && (dwMessageID == eMessage.dwMessageID));
  }
  return S_OK;
}

//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
CResourceManager*  CEngine::e_FindResource(const char* pszName)
{
  if (pszName == NULL)
    return NULL;

  E_ELEMENT* elem = m_pRM[pszName];
  if (elem != NULL) {
    CResourceManager* prm = (CResourceManager*)(elem->pElement);
    return prm;
  }
  return NULL;
}

//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
CSceneManager*  CEngine::e_FindScene(const char* pszName)
{
  if (pszName == NULL)
    return NULL;

  E_ELEMENT* elem = m_pSM[pszName];
  if (elem != NULL) {
    CSceneManager* psm = (CSceneManager*)(elem->pElement);
    return psm;
  }
  return NULL;
}


//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
void CEngine::e_CalculateFPS()
{
  static const long lPeriod = 10;
  static long lFrameNumber = 0;
  static DWORD dwPrevTime;
  static DWORD dwCurTime;

  lFrameNumber++;

  if (lFrameNumber >= lPeriod) {
    dwPrevTime = dwCurTime;
    dwCurTime = timeGetTime();

    double fSeconds = double(dwCurTime - dwPrevTime)/(double)1000;
    int iFPS = (int)((double)lFrameNumber/fSeconds);

	lFrameNumber = 0;

	// update console fps
	if (m_pConList)
		m_pConList->SetFPS(iFPS);
  }
}


//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
void CEngine::e_InitDirtRects()
{
  m_Regions.clear();
}


//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
void CEngine::e_OptimizeRegions()
{
  RECT r, ri, rj;
  int res;
  E_REGION_ITER it = m_Regions.begin(), jt;
  if (m_Regions.size() > 1) {
    while (it != m_Regions.end()) {
      jt = it;
      jt++;
      res = 0;
      while (jt != m_Regions.end()) {
        ri = *it;
        rj = *jt;
        res = e_Fragment(ri, rj, r);
        *it = ri;
        *jt = rj;
        if (res == -1) {
          it = m_Regions.erase(it);
          break;
        }
        switch(res) {
          case 1:
	    jt = m_Regions.erase(jt);
	    break;
	  case 2:
	    jt++;
	    break;
	  case 3:
	    jt = m_Regions.insert(jt, r); 
            jt++;
	    break;
	  default:
	    jt++;
        }
      }
      if (res != -1) it++;
    }
    it = m_Regions.begin();
    while (it != m_Regions.end()) {
      r = (RECT)(*it);
      if ((r.left >= r.right) || (r.top >= r.bottom)) {
        it = m_Regions.erase(it);
      } else {
        it++;
      }
    }
  }
}

//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
//#define max(a,b)		(((a)>(b))?(a):(b))
//#define min(a,b)		(((a)<(b))?(a):(b))
int CEngine::e_Fragment(RECT& r1,RECT& r2,RECT& r3)
{
  if ((max(r1.left,r2.left) >= min(r1.right,r2.right)) ||
      (max(r1.top,r2.top) >= min(r1.bottom,r2.bottom)))
    return 0;

  unsigned InCode = 0x0000;
  if (r2.left <= r1.left && r1.left < r2.left)
    InCode |= 0x0009;
  if (r2.left <= r1.right && r1.right < r2.right)
    InCode |= 0x0006;
  if (r2.top <= r1.top && r1.top < r2.bottom)
    InCode |= 0x0030;
  if (r2.top <= r1.bottom && r1.bottom < r2.bottom)
    InCode |= 0x00C0;

  switch(InCode) {
    case 0x0039:
      r3.left = r1.left;
      r3.top = r2.top;
      r3.right = r2.right;
      r3.bottom = r1.top;

      r2.right = r1.left;
      return 3;

    case 0x000f:
      r3.left = r1.right;
      r3.top = r2.top;
      r3.right = r2.right;
      r3.bottom = r2.bottom;

      r2.right = r1.left;
      return 3;

    case 0x00f0:
      r3.left = r2.left;
      r3.top = r1.bottom;
      r3.right = r2.right;
      r3.bottom = r2.bottom;

      r2.bottom = r1.top;
      return 3;

    case 0x00c9:
      r3.left = r1.left;
      r3.top = r1.bottom;
      r3.right = r2.right;
      r3.bottom = r2.bottom;

      r2.right = r1.left;
      return 3;
    
    case 0x0036:
      r3.left = r1.right;
      r3.top = r1.top;
      r3.right = r2.right;
      r3.bottom = r2.bottom;

      r2.bottom = r1.top;
      return 3;
    
    case 0x00c6:
      r3.left = r2.left;
      r3.top = r1.bottom;
      r3.right = r1.right;
      r3.bottom = r2.bottom;

      r2.left = r1.right;
      return 3;

    case 0x00c0: r2.top = r1.bottom; return 2;
    case 0x0009: r2.right = r1.left; return 2;
    case 0x00f6: r1.right = r2.left; return 2;
    case 0x0030: r2.bottom = r1.top; return 2;
    case 0x00cf: r1.bottom = r2.top; return 2;
    case 0x0006: r2.left = r1.right; return 2;
    case 0x00f9: r1.left = r2.right; return 2;
    case 0x003f: r1.top = r2.bottom; return 2;
    case 0x00ff: r1 = r2; return -1;
    case 0x0000: return 1;
  }
  return 0;
}
#undef min
#undef max

//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
void CEngine::AddDirtScreenRect(RECT* prcRect) 
{
  E_REGION_ITER i = m_Regions.begin();
  while (i != m_Regions.end()) {
    if ((*i).left < prcRect->left)
      i++;
    else
      break;
  }
  m_Regions.insert(i, *prcRect);
}


//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
void CEngine::ProcessNetwork()
{
  E_NMSG* pnmsg = Network_CreateNMSG();
  BOOL    bRec;
  DPID    FromID;
  HRESULT hr;

  //receive messages
  if (pnmsg) {
    hr = Network_Receive(pnmsg, &bRec, &FromID);
    if (SUCCEEDED(hr) && bRec) {
      //check if system
      if (FromID == DPID_SYSMSG) {
        e_GetSystemMessage(pnmsg);
      } else {
		PostMessage(EM_NMSG_DATARECEIVED, (LPARAM)pnmsg, (WPARAM)FromID);
      }
    } else {
      Network_DestroyNMSG(pnmsg);
    }
  }
}

//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
void CEngine::e_GetSystemMessage(E_NMSG* pnmsg)
{
  DPMSG_GENERIC* message = (DPMSG_GENERIC*)(pnmsg->pData);

  //Create remote player
  if (message->dwType == DPSYS_CREATEPLAYERORGROUP) {
    DPMSG_CREATEPLAYERORGROUP* PlayerOrGroupData;
    PlayerOrGroupData = (DPMSG_CREATEPLAYERORGROUP*)message;

    if (PlayerOrGroupData->dwPlayerType == DPPLAYERTYPE_PLAYER) {
      DPID RemotePlayerID = PlayerOrGroupData->dpId;
      PostMessage(EM_NMSG_CREATEREMOTEPLAYER, (LPARAM)0, (WPARAM)RemotePlayerID);
    }
  }
	
  //set player data
  if (message->dwType == DPSYS_SETPLAYERORGROUPDATA) {
    DPMSG_SETPLAYERORGROUPDATA* PlayerOrGroupData;
    PlayerOrGroupData = (DPMSG_SETPLAYERORGROUPDATA*)message;

    if (PlayerOrGroupData->dwPlayerType == DPPLAYERTYPE_PLAYER) {
      DPID PlayerID = PlayerOrGroupData->dpId;
      E_NMSG* pnmsglocal = Network_CreateNMSG();
      Network_FillNMSG(pnmsglocal, PlayerOrGroupData->lpData, PlayerOrGroupData->dwDataSize);
      PostMessage(EM_NMSG_SETPLAYERDATA, (LPARAM)pnmsglocal, (WPARAM)PlayerID);
      return;
    }
  }
	
  //destroy player
  if (message->dwType == DPSYS_DESTROYPLAYERORGROUP) {
    DPMSG_DESTROYPLAYERORGROUP* PlayerOrGroupData;
    PlayerOrGroupData = (DPMSG_DESTROYPLAYERORGROUP*)message;

    if (PlayerOrGroupData->dwPlayerType == DPPLAYERTYPE_PLAYER) {
      DPID PlayerID = PlayerOrGroupData->dpId;
      PostMessage(EM_NMSG_DESTROYPLAYER, (LPARAM)0, (WPARAM)PlayerID);
    }
  }
  
  // receive chat message
  if (message->dwType == DPSYS_CHAT) {
    DPMSG_CHAT* ChatMessage;
    ChatMessage = (DPMSG_CHAT*)message;

    if (ChatMessage->dwType == DPSYS_CHAT) {
      DPID PlayerIDFrom = ChatMessage->idFromPlayer;
      LPSTR lpstrChatMessage = ChatMessage->lpChat->lpszMessageA;
      E_NMSG* pnmsglocal = Network_CreateNMSG();
      Network_FillNMSG(pnmsglocal, (void*)lpstrChatMessage, strlen(lpstrChatMessage) + 1);
      PostMessage(EM_NMSG_CHAT, (LPARAM)pnmsglocal, (WPARAM)PlayerIDFrom);
    }
  }
  Network_DestroyNMSG(pnmsg);
}


//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
void CEngine::ProcessKeyboard()
{
  if (m_pIM->ProcessKeyboard()) 
  {
    // check for shift down
    BOOL bIsShiftDown = (BOOL)(m_pIM->KeyDown(DIK_LSHIFT)) || 
                        (BOOL)(m_pIM->KeyDown(DIK_RSHIFT));

    for (int i = 0; i < 256; i++) 
    {
      // get ascii code for dik code
      BYTE nAsciiCode = m_pIM->DIK2ASCII(i, bIsShiftDown);
      
      // handle key pressed 
      if (m_pIM->KeyPressed(i)) 
      {
        // check for console
        if (i == DIK_GRAVE && m_pConScn) {
          m_bIsConsole = !m_bIsConsole;
          m_pConScn->SetVisible(m_bIsConsole);
        }

        // send ascii code and dik code
        PostMessage(EM_KEYPRESSED, nAsciiCode, i);
      }
      
      // handle key down
      if (m_pIM->KeyDown(i))
      {
		// send ascii code and dik code
        PostMessage(EM_KEYDOWN, nAsciiCode, i);
      }
    }
  }
}


//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
void CEngine::ProcessMouse()
{
  if (!m_pIM->ProcessMouse())
    return;

  static BOOL bLMouseDown = FALSE;
  static BOOL bRMouseDown = FALSE;
  static byte bLClickCount = 0;
  static byte bRClickCount = 0;
  static const int bCountForClick = 10;

  int lX = m_pIM->GetlX();
  int lY = m_pIM->GetlY();
  BOOL bIsLButton = m_pIM->IsLButton();
  BOOL bIsRButton = m_pIM->IsRButton();

  //handle mouse move
  if (lX != 0) {
    m_ptCursorPos.x += lX*m_nMouseSpeed;
    if (m_ptCursorPos.x > m_rcClientRect.left && 
        m_ptCursorPos.x < m_rcClientRect.right) {
      PostMessage(EM_MOUSEMOVE, m_ptCursorPos.x, m_ptCursorPos.y);
    } else {
      m_ptCursorPos.x -= lX*m_nMouseSpeed;
    }
  }
  if (lY != 0) {
    m_ptCursorPos.y += lY*m_nMouseSpeed;
    if (m_ptCursorPos.y > m_rcClientRect.top &&
        m_ptCursorPos.y < m_rcClientRect.bottom) {
      PostMessage(EM_MOUSEMOVE, m_ptCursorPos.x, m_ptCursorPos.y);
    } else {
      m_ptCursorPos.y -= lY*m_nMouseSpeed;
    }
  }

  //handle mouse click
  if (bIsLButton) {
    if (!bLMouseDown) {
      PostMessage(EM_LMOUSEDOWN, m_ptCursorPos.x, m_ptCursorPos.y);
      bLMouseDown = TRUE;
      bLClickCount = 0;
    } else {
      bLClickCount++;
      if (bLClickCount > bCountForClick)
        bLClickCount = bCountForClick;
    }
  } else {
    if (bLMouseDown) {
      PostMessage(EM_LMOUSEUP, m_ptCursorPos.x, m_ptCursorPos.y);
      if (bLClickCount < bCountForClick)
        PostMessage(EM_LMOUSECLICK, m_ptCursorPos.x, m_ptCursorPos.y);
      bLMouseDown = FALSE;
    }
  }
  if (bIsRButton) {
    if (!bRMouseDown) {
      PostMessage(EM_RMOUSEDOWN, m_ptCursorPos.x, m_ptCursorPos.y);
      bRMouseDown = TRUE;
      bRClickCount = 0;
    } else {
      bRClickCount++;
      if (bRClickCount > bCountForClick)
        bRClickCount = bCountForClick;
    }
  } else {
    if (bRMouseDown) {
      PostMessage(EM_RMOUSEUP, m_ptCursorPos.x, m_ptCursorPos.y);
      if (bRClickCount < bCountForClick)
        PostMessage(EM_RMOUSECLICK, m_ptCursorPos.x, m_ptCursorPos.y);
      bRMouseDown = FALSE;
    }
  }
}


//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
void CEngine::PostState(DWORD dwStateID, 
                        LPARAM lParam, 
                        WPARAM wParam, 
                        const char* pszSceneName, 
                        const char* pszListenerName)
{
  if (pszListenerName != NULL) {
    CSceneManager* psm = e_FindScene(pszSceneName);
    if (psm != NULL)
      psm->PostState(dwStateID, lParam, wParam, pszListenerName);
    return;
  }

  for (E_HASH_LIST_ITERATOR i = m_pSM.begin(); i != m_pSM.end(); i++) {
    E_ELEMENT* elem = (E_ELEMENT*)(*i).second;
    if (elem != NULL) {
      CSceneManager* psm = (CSceneManager*)(elem->pElement);
      psm->PostState(dwStateID, lParam, wParam, pszListenerName);
    }
  }
}


//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
void CEngine::e_ProcessState(DWORD dwCurrTick)
{
  E_STATE eState;
  if (e_GetState(&eState)) { //if there is state in queue
    if (ProcessState(eState, dwCurrTick)) //if we are process so del it
      e_DelState();
  }
}

//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
void CEngine::e_DelState()
{
  if (!m_qState.empty())
    m_qState.pop();
}

//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
void CEngine::e_ClearStates()
{
  while (!m_qState.empty())
    m_qState.pop();
}

//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
BOOL CEngine::e_GetState(E_STATE* peState)
{
  if (m_qState.empty())
    return FALSE;

  *peState = m_qState.front();

  return TRUE;
}


//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
void CEngine::PostState(DWORD dwStateID, LPARAM lParam, WPARAM wParam)
{
  E_STATE eState = {dwStateID, lParam, wParam};
  m_qState.push( eState );
}

//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
E_STATE CEngine::GetState() 
{
  E_STATE eState = {ES_IDLE, 0, 0};

  if (m_qState.empty())
    return eState;

  eState = m_qState.front();

  return eState;
}

//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
void CEngine::PostMessage(DWORD dwMessageID, LPARAM lParam, WPARAM wParam)
{
  E_MESSAGE eMessage = {dwMessageID, lParam, wParam};
  m_qMessage.push(eMessage);
}
	
//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
BOOL CEngine::GetMessage(E_MESSAGE* peMessage)
{
  if (m_qMessage.empty())
    return FALSE;
	
  *peMessage = m_qMessage.front();
    	
  return TRUE;
}

//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
void CEngine::e_DelMessage() 
{
  if (!m_qMessage.empty())
    m_qMessage.pop();
}


//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
void CEngine::ClearMessages()
{
  while (!m_qMessage.empty())
    m_qMessage.pop();
}

//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
BOOL CEngine::ProcessState(E_STATE eState, DWORD dwCurrTick)
{
  //custom code here
  return TRUE;
}


//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
char* CEngine::GetRTStringVar(const char* pszVarName)
{
  VAR* pVar = m_pRTVars[pszVarName];
  if (pVar) {
    return pVar->szVarValue;
  } else {
    return NULL;
  }
}


//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
void CEngine::SetRTStringVar(const char* pszVarName, char* pszVarValue)
{
  //try to find
  VAR* pVar = m_pRTVars[pszVarName];
  if (pVar) {
    strcpy(pVar->szVarValue, pszVarValue);
  } else {
    //create new
    pVar = (VAR*)malloc(sizeof(VAR));
    strcpy(pVar->szVarName, pszVarName);
    strcpy(pVar->szVarValue, pszVarValue);
    m_pRTVars[pVar->szVarName] = pVar;
  }
}


//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
int CEngine::GetRTIntVar(const char* pszVarName)
{
  char* pVar = GetRTStringVar(pszVarName);
  if (pVar) {
    return atoi(pVar);
  } else {
    return 0;
  }
}


//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
void CEngine::SetRTIntVar(const char* pszVarName, int nVarValue)
{
  char buf[MAX_BUFFER];
  _itoa(nVarValue, buf, 10);
  SetRTStringVar(pszVarName, buf);
}


//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
void* CEngine::GetRTPtrVar(const char* pszVarName)
{
  int iAddr = GetRTIntVar(pszVarName);
  return (void*)iAddr;
}


//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
void CEngine::SetRTPtrVar(const char* pszVarName, void* pVarValue)
{
  int iAddr = (int)pVarValue;
  SetRTIntVar(pszVarName, iAddr);
}


//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
BOOL CEngine::e_CreateConsoleScripts()
{
	char fp[MAX_PATH];
	_makepath(fp, NULL, m_szRoot, L_ENGINE_FONT, NULL);

	FILE* fout = fopen(fp, "w");
	if (!fout)
		return FALSE;

	/*
	BEGIN=FONT,cfont
		fontsystem=Courier
		backcolor=0,0,0
		height=20
		opaque=1
		quality=4
		weight=400
		color=0,255,0
		types=0,0,0
	END
	*/

	char szBuf[MAX_BUFFER];
	char pBuf[MAX_BUFFER];
	int i, iValue[3];

	wsprintf(szBuf, "%s=%s,%s\n", P_BEGIN, L_FONT, L_ENGINE_CON_FONT);
	fputs(szBuf, fout);
	
	if (!GetVars()->GetString(L_GROUP, L_CON_CONSOLE, L_CON_FONT, pBuf)) {
		strcpy(pBuf, "System"); // if font not defined we use System
	}
	wsprintf(szBuf, "%s=%s\n", L_FONTSYSTEM, pBuf);
	fputs(szBuf, fout);
	
	wsprintf(szBuf, "%s=%i\n", L_FONTHEIGHT, 20);
	fputs(szBuf, fout);
	
	wsprintf(szBuf, "%s=%i\n", L_FONTQUALITY, 4);
	fputs(szBuf, fout);
	
	wsprintf(szBuf, "%s=%i\n", L_FONTWEIGHT, 800);
	fputs(szBuf, fout);
	
	wsprintf(szBuf, "%s=%i,%i,%i\n", L_FONTTYPES, 0, 0, 0);
	fputs(szBuf, fout);

	// back color
	iValue[0] = iValue[1] = iValue[2] = 0;
	for (i = 0; i < 3; i++) {
		GetVars()->GetInt(L_GROUP, L_CON_CONSOLE, L_CON_BCOLOR, i, &iValue[i]);
	}
	wsprintf(szBuf, "%s=%i,%i,%i\n", L_FONTBACKCOLOR, iValue[0], iValue[1], iValue[2]);
	fputs(szBuf, fout);

	// opaque - set to 1 for non-transparent console
	iValue[0] = 0;
	GetVars()->GetInt(L_GROUP, L_CON_CONSOLE, L_CON_OPAQUE, 0, &iValue[0]);
	wsprintf(szBuf, "%s=%i\n", L_FONTOPAQUE, iValue[0]);
	fputs(szBuf, fout);

	// font color
	iValue[1] = 255;
	iValue[0] = iValue[2] = 0;
	for (i = 0; i < 3; i++) {
		GetVars()->GetInt(L_GROUP, L_CON_CONSOLE, L_CON_FCOLOR, i, &iValue[i]);
	}
	wsprintf(szBuf, "%s=%i,%i,%i\n", L_FONTCOLOR, iValue[0], iValue[1], iValue[2]);
	fputs(szBuf, fout);

	wsprintf(szBuf, "%s\n", P_END);
	fputs(szBuf, fout);

	fclose(fout);

	// console scene

	_makepath(fp, NULL, m_szRoot, L_ENGINE_CONSOLE, NULL);

	fout = fopen(fp, "w");
	if (!fout)
		return FALSE;

	/*
	BEGIN=SCENE,console
		zpos=10
		viewrect=10,10,400,90
		cameraxy=0,0
		wwidth=1000
		wheight=1000
	END
	*/

	wsprintf(szBuf, "%s=%s,%s\n", P_BEGIN, L_SCENE, L_ENGINE_CON_NAME);
	fputs(szBuf, fout);

	wsprintf(szBuf, "%s=%i\n", L_ZPOS, 10);
	fputs(szBuf, fout);

	wsprintf(szBuf, "%s=%i,%i,%i,%i\n", L_VIEWRECT, 10, 10, 400, 120);
	fputs(szBuf, fout);

	wsprintf(szBuf, "%s=%i,%i\n", L_CAMERAXY, 0, 0);
	fputs(szBuf, fout);

	wsprintf(szBuf, "%s=%i\n", L_WWIDTH, 1000);
	fputs(szBuf, fout);

	wsprintf(szBuf, "%s=%i\n", L_WHEIGHT, 1000);
	fputs(szBuf, fout);

	wsprintf(szBuf, "%s\n", P_END);
	fputs(szBuf, fout);

	fclose(fout);

	return TRUE;
}


//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
void CEngine::e_DestroyConsoleScripts()
{
	char fp[MAX_PATH];

	_makepath(fp, NULL, m_szRoot, L_ENGINE_FONT, NULL);
	_unlink(fp);

	_makepath(fp, NULL, m_szRoot, L_ENGINE_CONSOLE, NULL);
	_unlink(fp);
}


//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
BOOL CEngine::e_CreateMouseScript()
{
	char fp[MAX_PATH];
	_makepath(fp, NULL, m_szRoot, L_ENGINE_CURSOR, NULL);

	FILE* fout = fopen(fp, "w");
	if (!fout)
		return FALSE;

	char szBuf[MAX_BUFFER];
	char pBuf[MAX_BUFFER];
	int i, iValue[3];

	// mouse scene

	/*
	BEGIN=SCENE,cursor
		zpos=11
		viewrect=0,0,width,height
		cameraxy=0,0
		wwidth=width + 10
		wheight=height + 10
	END
	*/

	wsprintf(szBuf, "%s=%s,%s\n", P_BEGIN, L_SCENE, L_ENGINE_CUR_NAME);
	fputs(szBuf, fout);

	wsprintf(szBuf, "%s=%i\n", L_ZPOS, 11);
	fputs(szBuf, fout);

	int iWidth = GetVideo()->GetScreenWidth();
	int iHeight = GetVideo()->GetScreenHeight();
	
	wsprintf(szBuf, "%s=%i,%i,%i,%i\n", L_VIEWRECT, 0, 0, iWidth, iHeight);
	fputs(szBuf, fout);

	wsprintf(szBuf, "%s=%i,%i\n", L_CAMERAXY, 0, 0);
	fputs(szBuf, fout);

	wsprintf(szBuf, "%s=%i\n", L_WWIDTH, iWidth + 10);
	fputs(szBuf, fout);

	wsprintf(szBuf, "%s=%i\n", L_WHEIGHT, iHeight + 10);
	fputs(szBuf, fout);

	wsprintf(szBuf, "%s\n", P_END);
	fputs(szBuf, fout);

	// cursor sprite

    /*
    BEGIN=SPRITE,cursor
       data=xxxx
	   blenddata=xxxxx
	   blendcolor=0,0,0
       wposxy=width/2,height/2
       hotspot=2,48
       frames=7
	   rows=1
       zpos=1
	   speed=0
    END
	*/

	wsprintf(szBuf, "\n%s=%s,%s\n", P_BEGIN, L_SPRITE, L_ENGINE_CUR_NAME);
	fputs(szBuf, fout);

	GetVars()->GetString(L_GROUP, L_MS_MOUSE, L_MS_DATA, pBuf);
	wsprintf(szBuf, "%s=%s\n", L_RESDATA, pBuf);
	fputs(szBuf, fout);

	if (GetVars()->GetString(L_GROUP, L_MS_MOUSE, L_MS_BLENDDATA, pBuf)) {
		wsprintf(szBuf, "%s=%s\n", L_BLENDDATA, pBuf);
		fputs(szBuf, fout);

		if (GetVars()->GetInt(L_GROUP, L_MS_MOUSE, L_MS_BLENDCOLOR, 0, &i)) {
			iValue[0] = iValue[1] = iValue[2] = 0;
			for (i = 0; i < 3; i++)
				GetVars()->GetInt(L_GROUP, L_MS_MOUSE, L_MS_BLENDCOLOR, i, &iValue[i]);
			wsprintf(szBuf, "%s=%i,%i,%i\n", L_BLENDCOLOR, iValue[0], iValue[1], iValue[2]);
			fputs(szBuf, fout);
		}
	}

	wsprintf(szBuf, "%s=%i,%i\n", L_WPOSXY, iWidth/2, iHeight/2);
	fputs(szBuf, fout);

	GetVars()->GetInt(L_GROUP, L_MS_MOUSE, L_MS_HOTSPOT, 0, &iValue[0]);
	GetVars()->GetInt(L_GROUP, L_MS_MOUSE, L_MS_HOTSPOT, 1, &iValue[1]);
	wsprintf(szBuf, "%s=%i,%i\n", L_HOTSPOT, iValue[0], iValue[1]);
	fputs(szBuf, fout);

	if (GetVars()->GetInt(L_GROUP, L_MS_MOUSE, L_MS_FRAMES, 0, &i)) {
		wsprintf(szBuf, "%s=%i\n", L_FRAMES, i);
		fputs(szBuf, fout);
	}

	if (GetVars()->GetInt(L_GROUP, L_MS_MOUSE, L_MS_ROWS, 0, &i)) {
		wsprintf(szBuf, "%s=%i\n", L_ROWS, i);
		fputs(szBuf, fout);
	}

	wsprintf(szBuf, "%s=%i\n", L_ZPOS, 1);
	fputs(szBuf, fout);

	if (GetVars()->GetInt(L_GROUP, L_MS_MOUSE, L_MS_ANIMSPEED, 0, &i)) {
		wsprintf(szBuf, "%s=%i\n", L_SPEED, i);
		fputs(szBuf, fout);
	}

	wsprintf(szBuf, "%s\n", P_END);
	fputs(szBuf, fout);

	fclose(fout);

	return TRUE;
}


//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
void CEngine::e_DestroyMouseScript()
{
	char fp[MAX_PATH];

	_makepath(fp, NULL, m_szRoot, L_ENGINE_CURSOR, NULL);
	_unlink(fp);
}


//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
void CEngine::ProcessConsole(const char* pcszConCmd)
{
	// custom code here
}


//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
void CEngine::SetConsoleStatus(const char* pcszConStatus)
{
	if (m_pConList && m_bIsConsole) {
		m_pConList->SetStatus(pcszConStatus);
	}
}


//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
BOOL CEngine::GetWindowed()
{
	return m_bWindowed;
}


//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
DWORD CEngine::GetLastTick()
{
	return m_dwLastTick;
}


//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
char* CEngine::GetRoot()
{
	return m_szRoot;
}


//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
CVideoManager* CEngine::GetVideo()
{
	return m_pVideo;
}


//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
CSoundManager* CEngine::GetAudio()
{
	return m_pSnd;
}


//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
CVars* CEngine::GetVars()
{
	return m_pINIVars;
}


//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
HWND CEngine::GetHWnd()
{
	return m_hWnd;
}


//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
CResourceManager* CEngine::GetResource(const char* pszName)
{
	return e_FindResource(pszName);
}


//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
CSceneManager* CEngine::GetScene(const char* pszName)
{
	return e_FindScene(pszName);
}


//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
BYTE CEngine::GetMouseSpeed()
{
	return m_nMouseSpeed;
}


//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
BOOL CEngine::GetActive()
{
	return m_bIsActive;
}


//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
RECT CEngine::GetClientRect()
{
	return m_rcClientRect;
}


//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
BOOL CEngine::GetSafeMode()
{
	return m_bIsSafeMode;
}


//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
void CEngine::SetWindowed(BOOL bWindowed)
{
	m_bWindowed = bWindowed;
}


//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
void CEngine::SetLastTick(DWORD dwLastTick)
{
	m_dwLastTick = dwLastTick;
}


//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
void CEngine::SetMouseSpeed(BYTE nMouseSpeed)
{
	m_nMouseSpeed = nMouseSpeed;
}


//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
void CEngine::SetSafeMode(BOOL bIsSafeMode)
{
	m_bIsSafeMode = bIsSafeMode;
}


//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
void CEngine::SetCurrTick(DWORD dwCurrTick)
{
	m_dwCurrTick = dwCurrTick;
}


//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
DWORD CEngine::GetCurrTick()
{
	return m_dwCurrTick;
}


//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
BOOL CEngine::CreateSession(const char* pszSessionName, int nMaxPlayers)
{
	return Network_CreateSession(pszSessionName, nMaxPlayers);
}


//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
BOOL CEngine::ConnectToSession(const char* pszSessionName, const char* pcszServerIP)
{
	return Network_ConnectToSession(pszSessionName, pcszServerIP);
}


//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
BOOL CEngine::GetSessionsStats(NM_SESSIONS_STATS* pStats)
{
	return Network_GetSessionsStats(pStats);
}


//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
BOOL CEngine::CreateLocalPlayer(const char* pszPlayerName)
{
	return Network_CreatePlayer(pszPlayerName);
}


//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
DPID CEngine::GetLocalPlayerId()
{
	return Network_GetPlayerId();
}


//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
E_NMSG* CEngine::GetPlayerName(DPID idPlayer)
{
	return Network_GetPlayerName(idPlayer);
}


//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
BOOL CEngine::SetLocalPlayerData(void* pData, DWORD dwSize)
{
	return Network_SetPlayerData(pData, dwSize);
}


//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
E_NMSG* CEngine::GetPlayerData(DPID idPlayer)
{
	return Network_GetPlayerData(idPlayer);
}


//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
BOOL CEngine::SendData(void* pData, DWORD dwSize)
{
	return Network_Send(pData, dwSize);
}


//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
BOOL CEngine::CloseSession()
{
	return Network_CloseSession();
}

//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
BOOL CEngine::DestroyPlayer()
{
	return Network_DestroyPlayer();
}


//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
CSoundCore* CEngine::CreateSoundCore()
{
	// by default we use FMOD sound system
	CSoundCore* pCore = new CFMODCore();
	
	return pCore;	
}


//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
void CEngine::DeleteSoundCore(CSoundCore* pSndCore)
{
	if (pSndCore) {
		delete pSndCore;
	}
}
