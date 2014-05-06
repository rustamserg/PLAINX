#include "plainx.h"
#include "sprman.h"

#define PX_EFFECTS "px_effects"
#define L_CLONE "clone"


CSceneManager::CSceneManager()
{
  m_nWHeight = 0;
  m_nWWidth = 0;
  m_pCamera.x = 0;
  m_pCamera.y = 0;
  m_pVars = new CVars();
  m_pRM = NULL;
  m_pSysFontRM = NULL;
  m_pStrM = NULL;
  m_pEngine = NULL;
  m_pMap = NULL;
  m_nBorder = DEF_BORDER;
  m_nCellSizeH = DEF_CELLSIZE;
  m_nCellSizeW = DEF_CELLSIZE;

  m_nCloneCount = 0;

  m_bFirst = TRUE;
  m_bIsDrawGrid = FALSE;

  strcpy(m_szName, "");
  strcpy(m_szDescription, "");

  m_rcView.top = 0;
  m_rcView.left = 0;
  m_rcView.bottom = 0;
  m_rcView.right = 0;
  
  m_nZPos = 0;
  m_nClearColor = 0;

  m_pEffects = 0;
}


CSceneManager::~CSceneManager()
{
  Free();
  
  if (m_pVars) 
    delete m_pVars;
}


//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
BOOL CSceneManager::Init(const char* pszConfig, 
                         CResourceManager* pResMan,
                         CEngine* pEngine, 
                         const char* pszName)
{
  if (pszConfig == NULL || m_pVars == NULL ||
        pResMan == NULL || pEngine == NULL || pszName == NULL)
    return FALSE;

  if (!m_pVars->Init(pszConfig))
    return FALSE;

  m_pRM = pResMan;
  m_pEngine = pEngine;

  //string support
  m_pStrM = new CStringManager();
  m_pStrM->Init(this);

  m_pMap = new CMapManager();

  m_nWHeight = 0;
  m_nWWidth = 0;

  m_rcView.top = 0;
  m_rcView.left = 0;
  m_rcView.bottom = 0;
  m_rcView.right = 0;

  m_pCamera.x = 0;
  m_pCamera.y = 0;

  m_nZPos = 0;
  strcpy(m_szName, pszName);

  InitWorldDirtRects();

  //try to load
  return sm_LoadScene( pszName );
}


//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
void CSceneManager::Free()
{
  m_pRM = NULL;

  if (m_pSysFontRM != NULL) {
      m_pSysFontRM->Free();
      delete m_pSysFontRM;
      m_pSysFontRM = NULL;
  }

  delete m_pStrM;
  m_pStrM = NULL;

  ClearMap();

  delete m_pMap;
  m_pMap = NULL;

  ClearListeners();
  InitWorldDirtRects();

  m_pEffects = NULL;
}


//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
void CSceneManager::SetDataResourceManager(const char* pcszName)
{
    m_pRM = m_pEngine->GetResource(pcszName);
    ReloadSprites();
}


//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
CSprite* CSceneManager::GetSprite(const char* pszName)
{
  if (pszName == NULL)
    return NULL;

  return m_pMap->GetSprite(pszName);
}


//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
int CSceneManager::GetSpritesFromPoint(int nXSPos, 
                                       int nYSPos, //point in screen coords
                                       CSprite** pList, //array for return values
                                       int nSize, //size of array if 0 we return required size
                                       int nZPos) //z order of sprite
{
  BOOL bIsEnd = FALSE;
  int nRet = 0;
  BOOL bListReady = (BOOL)nSize;

  //if we specify zpos
  if (nZPos > -1 && nZPos < L_DEPTH) {
    CSprite* ps = m_pMap->GetFirstSprite();
    while (!bIsEnd) {
      if (ps && ps->IsInSprite(nXSPos, nYSPos) && ps->GetZPos() == nZPos) {
        if (bListReady) {
          if (nRet < nSize) pList[nRet] = ps;
        }
        nRet++;
      }
      ps = m_pMap->GetNextSprite(&bIsEnd);
    }
  }
  return nRet;
}


//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
BOOL CSceneManager::sm_LoadScene(const char* pszScene)
{
  if (pszScene == NULL || m_pVars == NULL)
    return FALSE;

  int value;
  char buffer[MAX_BUFFER];

  //read left
  if (!m_pVars->GetInt(L_SCENE, pszScene, L_VIEWRECT, 0, &value))
    return FALSE;
  m_rcView.left = value;

  //read top
  if (!m_pVars->GetInt(L_SCENE, pszScene, L_VIEWRECT, 1, &value))
    return FALSE;
  m_rcView.top = value;

  //read right
  if (!m_pVars->GetInt(L_SCENE, pszScene, L_VIEWRECT, 2, &value))
    return FALSE;
  m_rcView.right = value;

  //read bottom
  if (!m_pVars->GetInt(L_SCENE, pszScene, L_VIEWRECT, 3, &value))
    return FALSE;
  m_rcView.bottom = value;

  //read border [optional]
  if (m_pVars->GetInt(L_SCENE, pszScene, L_BORDER, 0, &value))
    m_nBorder = value;

  //read clearcolor [optional]
  if (m_pVars->GetInt(L_SCENE, pszScene, L_CLEARCOLOR, 0, &value))
    m_nClearColor = value;
    
  //calculate view dimension
  m_nViewWWidth = m_rcView.right - m_rcView.left;
  m_nViewWHeight = m_rcView.bottom - m_rcView.top;

  //read cellsize [optional]
  if (m_pVars->GetInt(L_SCENE, pszScene, L_CELLSIZE, 0, &value)) {
    m_nCellSizeW = value;
  } else {
    m_nCellSizeW = m_nViewWWidth;
  }
  if (m_pVars->GetInt(L_SCENE, pszScene, L_CELLSIZE, 1, &value)) {
    m_nCellSizeH = value;
  } else {
    m_nCellSizeH = m_nViewWHeight;
  }

  //read desc [optional]
  if (m_pVars->GetString(L_SCENE, pszScene, L_DESC, buffer))
    strcpy(m_szDescription, buffer);

  //read wwidth
  if (!m_pVars->GetInt(L_SCENE, pszScene, L_WWIDTH, 0, &value))
    return FALSE;
  m_nWWidth = value;
  	
  //read wheight
  if (!m_pVars->GetInt(L_SCENE, pszScene, L_WHEIGHT, 0, &value))
    return FALSE;
  m_nWHeight = value;

  //aligned world dimension
  m_nWWidth = (m_nWWidth / m_nCellSizeW) * m_nCellSizeW;
  m_nWHeight = (m_nWHeight / m_nCellSizeH) * m_nCellSizeH;

  //read zpos
  if (!m_pVars->GetInt(L_SCENE, pszScene, L_ZPOS, 0, &value))
    return FALSE;
  m_nZPos = value;

  //read camerax [optional]
  if (m_pVars->GetInt(L_SCENE, pszScene, L_CAMERAXY, 0, &value))
    m_pCamera.x = value;

  //read cameray [optional]
  if (m_pVars->GetInt(L_SCENE, pszScene, L_CAMERAXY, 1, &value))
    m_pCamera.y = value;

  //remember screen size
  m_pEngine->GetVars()->GetInt(L_GROUP, L_VID_VIDEO, L_VID_WIDTH, 0, &m_nSWidth);
  m_pEngine->GetVars()->GetInt(L_GROUP, L_VID_VIDEO, L_VID_HEIGHT, 0, &m_nSHeight);

  //init map
  m_pMap->Init(this);

  //read first sprite
  if (m_pVars->GetNextGroup(L_SPRITE, buffer, TRUE)) {
    if (sm_CreateSprite(m_pVars, buffer) == NULL) {
      return FALSE;
    }
    while (m_pVars->GetNextGroup(L_SPRITE, buffer)) {
      if (sm_CreateSprite(m_pVars, buffer) == NULL) {
		return FALSE;
      }
    }
  }
  return TRUE;
}


//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
CSprite* CSceneManager::sm_CreateSprite(CVars* pVars, const char* pcszName)
{
  char szName[MAX_BUFFER];

  // create name
  if (!pcszName) {
    wsprintf(szName, "%s_%i", L_CLONE, m_nCloneCount);
    m_nCloneCount++;
  } else {
    strcpy(szName, pcszName);
  }
  if (GetSprite(szName) != NULL)
    return NULL;
 
  // create sprite
  CSprite* ps = new CSprite();
  if (ps == NULL) return NULL;

  // init sprite
  if (!ps->Init(pVars, this, &szName[0])) {
    delete ps;
    return NULL;
  }

  // add to map
  m_pMap->AddSprite(ps);

  // return it
  return ps;
}


//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
CSprite* CSceneManager::CreateSprite(SPRITE_DATA* pData, const char* pcszName)
{
  char szName[MAX_BUFFER];

  // create name
  if (!pcszName) {
    wsprintf(szName, "%s_%i", L_CLONE, m_nCloneCount);
    m_nCloneCount++;
  } else {
    strcpy(szName, pcszName);
  }
  if (GetSprite(szName) != NULL)
    return NULL;

  // create sprite
  CSprite* ps = new CSprite();
  if (ps == NULL) return NULL;

  // init sprite
  if (!ps->Init(pData, this, &szName[0])) {
    delete ps;
    return NULL;
  }

  // add to map
  m_pMap->AddSprite(ps);

  // return it
  return ps;
}


//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
BOOL CSceneManager::SetCameraX(int xPos)
{
  if (xPos < 0 || xPos > (m_nWWidth - (m_rcView.right-m_rcView.left)))
    return FALSE;
	
  //remember new position
  m_pCamera.x = xPos;

  //mark new camera view as dirt
  DirtCamera();

  return TRUE;
}

//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
BOOL CSceneManager::SetCameraY(int yPos)
{
  if (yPos < 0 || yPos > (m_nWHeight - (m_rcView.bottom-m_rcView.top)))
    return FALSE; 

  //remember new position
  m_pCamera.y = yPos;
	
  //mark new camera view as dirt
  DirtCamera();

  return TRUE;
}

//-----------------------------------------------------------------------------
// Name: 
// Desc: return down-left point (in screen coord) of cell size (in world coord)
//-----------------------------------------------------------------------------
BOOL CSceneManager::WorldToScreen(int nWX, int nWY, POINT* ptScreen)
{
  ptScreen->x = m_rcView.left + (nWX - m_pCamera.x);
  ptScreen->y = m_rcView.bottom - (nWY - m_pCamera.y);
	
  return TRUE;
}

//-----------------------------------------------------------------------------
// Name: 
// Desc: return down-left point (in screen coord) of cell size (in world coord)
//-----------------------------------------------------------------------------
BOOL CSceneManager::ScreenToWorld(int nSX, int nSY, POINT* ptWorld)
{
  ptWorld->x = m_pCamera.x + (nSX - m_rcView.left);
  ptWorld->y = m_pCamera.y + (m_rcView.bottom - nSY);

  return TRUE;
}

//-----------------------------------------------------------------------------
// Name: 
// Desc: call in update video
//-----------------------------------------------------------------------------
void CSceneManager::UpdateDirtRects()
{
  //is first run
  if (m_bFirst) {
    ClearCamera();
    DirtCamera();
    m_bFirst = FALSE;
    InitWorldDirtRects();
    return;
  }
	
  //main draw cycle
  BOOL bIsEnd = FALSE;
	
  //redraw dirty sprites pieces
  CSprite* ps = m_pMap->GetFirstSprite();
  while (!bIsEnd) {
     if (ps && ps->GetVisible() && ps->GetZPos() > 0) 
       sm_UpdateDirtySpriteRects(ps);
     ps = m_pMap->GetNextSprite(&bIsEnd);
  }

  //reset world dirt rect list
  InitWorldDirtRects();

   //draw grid
  if (m_bIsDrawGrid) {
    sm_DrawGrid();
  }
}

//-----------------------------------------------------------------------------
// Name: 
// Desc: call after update all logics
//-----------------------------------------------------------------------------
void CSceneManager::UpdateSprites(DWORD dwCurrTick)
{
  BOOL bIsEnd = FALSE;
	
  //update sprites in view rect
  CSprite* ps = m_pMap->GetFirstSprite();
  while (!bIsEnd) {
    if (ps && ps->GetVisible() && ps->GetZPos() > 0) 
      ps->Update(dwCurrTick);
    ps = m_pMap->GetNextSprite(&bIsEnd);
  }
}

//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
inline void CSceneManager::sm_UpdateDirtySpriteRects(CSprite* pSprite)
{
  if (pSprite == NULL)
    return;

  for (unsigned int i = 0; i<m_rcDirtRects.size(); i++) {
    //dirt rect
    RECT rcW;

    //get dirt rect
    CopyRect(&rcW, &m_rcDirtRects[i]);

    //get rect of surface for updated piece
    RECT  rcSurf;
    if (pSprite->GetUpdateRect(&rcW, &rcSurf)) {
      //get rect in screen coordinates
      RECT rcS;
      if (sm_ClipRect(&rcW, &rcS)) {
        //adjust surface rect for screen
	int nWSurf = rcSurf.right-rcSurf.left;
	int nHSurf = rcSurf.bottom-rcSurf.top;
	int nWScr = rcS.right-rcS.left;
	int nHScr = rcS.bottom-rcS.top;
	if (nWSurf > nWScr) {
	  if (rcW.left > m_pCamera.x)
	    rcSurf.right -= (nWSurf-nWScr);
	  else
	    rcSurf.left += (nWSurf-nWScr);
	}
	if (nHSurf > nHScr) {
          if (rcW.top > (m_pCamera.y + (m_rcView.bottom - m_rcView.top)))
	    rcSurf.top += (nHSurf-nHScr);
	  else
	    rcSurf.bottom -= (nHSurf-nHScr);
	}
	
        //draw it !
	nWSurf = rcSurf.right-rcSurf.left;
	nHSurf = rcSurf.bottom-rcSurf.top;
	if ((nWSurf > 0) && (nHSurf > 0)) {
	  //check if have blenddata
	  if (pSprite->GetBlendSurface() != NULL) {
	    m_pEngine->GetVideo()->GetDisplay()->BltAlphaBlend(rcS.left, rcS.top, pSprite->GetBlendSurface(), pSprite->GetBlendColor(), &rcSurf);
          } 
	  //check if have alpha
	  if (pSprite->GetAlpha() > 0) {
             m_pEngine->GetVideo()->GetDisplay()->BltAlpha(rcS.left, rcS.top, pSprite->GetSurface(), pSprite->GetAlpha(), &rcSurf);
          } else {
            m_pEngine->GetVideo()->GetDisplay()->Blt(rcS.left, rcS.top, pSprite->GetSurface(), &rcSurf);
	  }
	} //end draw
      }
    }
  }
}

//-----------------------------------------------------------------------------
// Name: ClipRect([in] prcSrc, [out] prcDest)
// Desc: prcSrc rect in world coordinates, prcDest rect in screen coords
//-----------------------------------------------------------------------------
BOOL CSceneManager::sm_ClipRect(RECT* prcSrc, RECT* prcDest)
{
  POINT pUL;
  POINT pDR;

  if (prcSrc == NULL || prcDest == NULL)
    return FALSE;

  //get upper-left & down-right corner in screen
  WorldToScreen(prcSrc->left, prcSrc->top, &pUL);
  WorldToScreen(prcSrc->right, prcSrc->bottom, &pDR);

  //creat destination rect
  prcDest->left = pUL.x;
  prcDest->top = pUL.y;
  prcDest->right = pDR.x;
  prcDest->bottom = pDR.y;
  	
  //check if we in the view area
  if (pDR.x < m_rcView.left || pUL.x > m_rcView.right)
    return FALSE; //we are not in visible area at all

  if (pDR.y < m_rcView.top || pUL.y > m_rcView.bottom)
    return FALSE; //we are not in visible area at all
	
  //check for clipping
  if (pDR.x > m_rcView.right)
    prcDest->right = prcDest->right - (pDR.x - m_rcView.right);

  if (pDR.y > m_rcView.bottom)
    prcDest->bottom = prcDest->bottom - (pDR.y - m_rcView.bottom);

  if (pUL.x < m_rcView.left)
    prcDest->left = prcDest->left + (m_rcView.left - pUL.x);
	
  if (pUL.y < m_rcView.top)
    prcDest->top = prcDest->top + (m_rcView.top - pUL.y);
		
  return TRUE;
}


//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
void CSceneManager::ClearMap()
{
  if (m_pMap == NULL)
    return;
  
  BOOL bIsEnd = FALSE;
  CSprite* ps = m_pMap->GetFirstSprite(FALSE);

  while (!bIsEnd) {
    if (ps) delete ps;
    ps = m_pMap->GetNextSprite(&bIsEnd);
  }
  m_pMap->Free();
}


//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
void CSceneManager::Scale(float fScale)
{
  int wx, wy;

  if (m_pMap == NULL)
    return;

  std::vector<CSprite*> vec;
  
  BOOL bIsEnd = FALSE;
  CSprite* ps = m_pMap->GetFirstSprite(FALSE);

  while (!bIsEnd) {
    if (ps) vec.push_back(ps);
    ps = m_pMap->GetNextSprite(&bIsEnd);
  }

  for (size_t i = 0; i < vec.size(); i++) {
    CSprite* ps = (CSprite*)vec[i];
    wx = ps->GetWXPos();
    wy = ps->GetWYPos();
    ps->ScaleFrame(fScale);
    ps->SetWXPos(wx * fScale);
    ps->SetWYPos(wy * fScale);
  }
  ClearCamera();
}


//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
void CSceneManager::ReloadSprites()
{
  BOOL bIsEnd = FALSE;
  CSprite* ps = m_pMap->GetFirstSprite(FALSE);

  while (!bIsEnd) {
    if (ps) ps->ReloadSurface();
    ps = m_pMap->GetNextSprite(&bIsEnd);
  }
  m_bFirst = TRUE;

  // reload all system fonts
  if (m_pSysFontRM != NULL) {
      m_pSysFontRM->ReloadAll();
  }
  if (m_pStrM != NULL) {
    m_pStrM->RedrawAll();
  }
}


//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
CResourceManager* CSceneManager::GetResourceManager(BOOL bIsSysFontRM)
{
    if (bIsSysFontRM == TRUE) {
        if (m_pSysFontRM == NULL) {
            m_pSysFontRM = new CResourceManager(m_pEngine);
        }
        return m_pSysFontRM;
    }
    return m_pRM;
}


//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
void CSceneManager::RecreateSysFonts()
{
    if (m_pSysFontRM != NULL) {
        m_pSysFontRM->RecreateAll();
    }
}


//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
void CSceneManager::DirtCamera()
{
  //define camera rect in world coord
  int xul = m_pCamera.x - m_nBorder * m_nCellSizeW; //x upper-left corner
  int yul = m_pCamera.y + m_nViewWHeight; //y upper-left corner
  int xdr = m_pCamera.x + m_nViewWWidth;  //x down-right corner
  int ydr = m_pCamera.y - m_nBorder * m_nCellSizeH; //y down-right corner

  //clip in world dimensions
  if (xul<0)
    xul = 0;
  if (xul > m_nWWidth)
    xul = m_nWWidth;
  if (yul<0)
    yul = 0;
  if (yul > m_nWHeight)
    yul = m_nWHeight;
  if (xdr<0)
    xdr = 0;
  if (xdr > m_nWWidth)
    xdr = m_nWWidth;
  if (ydr<0)
    ydr=0;
  if (ydr > m_nWHeight)
    ydr = m_nWHeight;
  
  //define camera rect
  RECT rcCameraW;
  rcCameraW.top = yul;
  rcCameraW.bottom = ydr;
  rcCameraW.left = xul;
  rcCameraW.right = xdr;

  //mark area as dirt
  DirtWorldRect(&rcCameraW);
}

//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
void CSceneManager::ClearCamera() 
{
  m_pEngine->GetVideo()->GetDisplay()->ClearEx(m_nClearColor, &m_rcView);
}

//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
void CSceneManager::DelSprite(const char* pszName)
{
  CSprite* ps = m_pMap->GetSprite(pszName);
  if (ps) {
    m_pMap->DelSprite(pszName);
    ps->DirtSprite();
    delete ps;
  }
}

//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
void CSceneManager::OnChangedSpritePos(const char* pcszName)
{
  m_pMap->OnChangedSpritePos(pcszName);
}

//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
CSprite* CSceneManager::CloneSprite(const char* pszParentName, const char* pszChildName)
{
  //get parent
  CSprite* ps = m_pMap->GetSprite(pszParentName);
  if (ps) {
    //make clone
    CSprite* pClone = ps->Clone();
    if (pClone == NULL)
        return NULL;

    //change clone name
    if (pszChildName) {
      pClone->SetName(pszChildName);
    } else {
      //make own name
      char szName[MAX_BUFFER];
      wsprintf(szName, "%s_%i", L_CLONE, m_nCloneCount);
      m_nCloneCount++;

      //set name
      pClone->SetName(szName);
    }

    //add clone to map
    if (m_pMap->AddSprite(pClone)) {
      return pClone;
    } else {
      delete pClone;
    }
  }
  return NULL;
}

//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
BOOL CSceneManager::SetViewRect(RECT* prcView)
{
  if (prcView == NULL)
    return FALSE;

  //check for screen valid
  if (prcView->left < 0 || prcView->left > m_nSWidth)
    return FALSE;
  if (prcView->right < 0 || prcView->right > m_nSWidth)
    return FALSE;
  if (prcView->top < 0 || prcView->top > m_nSHeight)
    return FALSE;
  if (prcView->bottom < 0 || prcView->bottom > m_nSHeight)
    return FALSE;

  CopyRect(&m_rcView, prcView);

  m_nViewWWidth = m_rcView.right - m_rcView.left;
  m_nViewWHeight = m_rcView.bottom - m_rcView.top;

  DirtCamera();

  return TRUE;
}


//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
void CSceneManager::SetZPos(int nZPos)
{
  m_nZPos = nZPos;
  m_pEngine->OnChangeScenePos(m_szName);
  DirtCamera();
}


//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
void CSceneManager::SetVisible(BOOL bVisible)
{
  BOOL bIsEnd = FALSE;
  CSprite* ps = m_pMap->GetFirstSprite(FALSE);
  
  while (!bIsEnd) {
    if (ps) ps->SetVisible(bVisible);
    ps = m_pMap->GetNextSprite(&bIsEnd);
  }

  CStringManager* pStrMan = GetStringManager();
  if (pStrMan) {
	  pStrMan->SetVisible(bVisible, ALL_STRINGS);
  }
  DirtCamera();
}

//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
void CSceneManager::DirtWorldRect(RECT* prcDirt)
{
  RECT rcScreen;
  
  if (sm_ClipRect(prcDirt, &rcScreen)) 
    m_pEngine->AddDirtScreenRect(&rcScreen);
}

//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
void CSceneManager::DirtScreenRect(RECT* prcDirt)
{
  //get world rect
  POINT ptULW;
  POINT ptDRW;
  ScreenToWorld( prcDirt->left, prcDirt->top, &ptULW);
  ScreenToWorld( prcDirt->right, prcDirt->bottom, &ptDRW);
	
  //clip to camera view [ptCamera, m_nViewWWidth, m_nViewWHeight]
  if (ptULW.x > (m_pCamera.x + m_nViewWWidth))
    return;

  if (ptULW.y < m_pCamera.y)
    return;

  if (ptDRW.x < m_pCamera.x)
    return;

  if (ptDRW.y > (m_pCamera.y + m_nViewWHeight))
    return;

  //so we are in camera view go clip it
  if (ptULW.x < m_pCamera.x)
    ptULW.x = m_pCamera.x;

  if (ptDRW.x > (m_pCamera.x + m_nViewWWidth))
    ptDRW.x = m_pCamera.x + m_nViewWWidth;

  if (ptULW.y > (m_pCamera.y + m_nViewWHeight))
    ptULW.y = m_pCamera.y + m_nViewWHeight;

  if (ptDRW.y < m_pCamera.y)
    ptDRW.y = m_pCamera.y;


  //add to dirt world list new rect
  RECT rcWDirt;
  rcWDirt.left = ptULW.x;
  rcWDirt.right = ptDRW.x;
  rcWDirt.top = ptULW.y;
  rcWDirt.bottom = ptDRW.y;

  //add to dirt world list new rect
  m_rcDirtRects.push_back(rcWDirt);
}

//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
void CSceneManager::InitWorldDirtRects()
{
  //clear list
  m_rcDirtRects.clear();
}


//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
BOOL CSceneManager::AddListener(CListener* pListener, const char* pszName)
{
  if (!pListener)
    return FALSE;

  pListener->Init(this);

  m_spListenerMap[pszName] = pListener;
  pListener->PostState(ES_LISTENERINIT, 0, 0);
	
  return TRUE;
}

//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
void CSceneManager::DelListener(const char* pszName)
{
  if (!pszName) 
    return;

  CListener* pl = m_spListenerMap[pszName];
  if (pl) {
    delete pl;
    m_spListenerMap.erase(pszName);
  }
}

//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
void CSceneManager::ClearListeners()
{
  if (!m_spListenerMap.empty()) {
    SM_LISTENER_HASH_MAP_ITERATOR i;
    for (i = m_spListenerMap.begin(); i != m_spListenerMap.end(); i++) {
      CListener* pl = (CListener*)(*i).second;
      if (pl) 
        delete pl;
    }
    m_spListenerMap.clear();
  }
}

//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
CListener* CSceneManager::GetListener(const char* pszName)
{
  if (!pszName) 
    return NULL;

  return m_spListenerMap[pszName];
}

//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
BOOL CSceneManager::ProcessMessages(DWORD dwCurrTick)
{
  BOOL bIsMessageDone = FALSE;
  SM_LISTENER_HASH_MAP_ITERATOR i;

  for (i = m_spListenerMap.begin(); i != m_spListenerMap.end(); i++) {
    CListener* pl = (CListener*)(*i).second;
    if (!pl) 
      continue;

    if (!pl->GetEnable()) 
      continue;
		
    //get front message
    E_MESSAGE eMessage;
					
    //we proccess first message and skip all same messages
    if (m_pEngine->GetMessage(&eMessage)) 
      bIsMessageDone = pl->ProcessMessage(eMessage, dwCurrTick);

    //if listener process message we break cycle
    if (bIsMessageDone) 
      break;
  }

  //process states
  for (i = m_spListenerMap.begin(); i != m_spListenerMap.end(); i++) {
    CListener* pl = (CListener*)(*i).second;
    if (!pl) 
      continue;
  
    if (!pl->GetEnable()) 
      continue;

    //get current state
    E_STATE eState;
    if (pl->GetState(&eState)) {
      //if there is state in queue
      if (pl->ProcessState(eState, dwCurrTick)) //if we process state remove from queue
	pl->DelState();
    }
  }
  return bIsMessageDone;
}


//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
void CSceneManager::PostState(DWORD dwStateID, 
                              LPARAM lParam, 
                              WPARAM wParam, 
                              const char* pszSpriteName)
{
  if (pszSpriteName) {
    CListener* sp = m_spListenerMap[pszSpriteName];
    if (sp) {
      if (sp->GetEnable())
        sp->PostState(dwStateID, lParam, wParam);
    }
    return;
  }
  SM_LISTENER_HASH_MAP_ITERATOR i;
  for (i = m_spListenerMap.begin(); i != m_spListenerMap.end(); i++) {
    CListener* ps = (CListener*)(*i).second;
    if (ps) {
      if (ps->GetEnable())
        ps->PostState(dwStateID, lParam, wParam);
    }
  }
}

//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
void CSceneManager::sm_DrawGrid()
{
  //get screen coords
  POINT ptS;
  WorldToScreen(m_pCamera.x, m_pCamera.y , &ptS);

  //get grid cell
  POINT ptC;
  ptC.x = (int)(m_pCamera.x/m_nCellSizeW);
  ptC.y = (int)(m_pCamera.y/m_nCellSizeH);
  
  //get offsetx and offsety
  DWORD dwOffX = m_pCamera.x - ptC.x*m_nCellSizeW;
  DWORD dwOffY = m_pCamera.y - ptC.y*m_nCellSizeH;

  DWORD dwStartX = ptS.x + m_nCellSizeW - dwOffX;
  DWORD dwStartY = ptS.y - m_nCellSizeH + dwOffY;

  HDC hDC;
  m_pEngine->GetVideo()->GetDisplay()->GetBackBuffer()->GetDC(&hDC);

  SelectObject(hDC, GetStockObject(WHITE_PEN));

  //draw camera view
  int x = (int)m_rcView.left;
  int y = (int)m_rcView.top;
  MoveToEx(hDC, x, y, NULL);
  x = (int)m_rcView.right;
  LineTo(hDC, x, y);
  y = (int)m_rcView.bottom;
  LineTo(hDC, x, y);
  x = (int)m_rcView.left;
  LineTo(hDC, x, y);
  y = (int)m_rcView.top;
  LineTo(hDC, x, y);

  //draw grid
  for (x = (int)dwStartX; x<m_rcView.right; x+=m_nCellSizeW) {
    MoveToEx(hDC, x, m_rcView.bottom, NULL);
    LineTo(hDC, x, m_rcView.top);
  }

  for (y = (int)dwStartY; y>m_rcView.top; y-=m_nCellSizeH) {
    MoveToEx(hDC, m_rcView.left, y, NULL);
    LineTo(hDC, m_rcView.right, y);
  }

  //release surface
  m_pEngine->GetVideo()->GetDisplay()->GetBackBuffer()->ReleaseDC(hDC);
}


//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
BOOL CSceneManager::AddEffect(const char* pcszConfig, const char* pcszEffectName)
{
	if (m_pEffects == NULL) {
		m_pEffects = new CEffects();
		if (!m_pEffects)
			return FALSE;

		if (!AddListener(m_pEffects, PX_EFFECTS)) {
			delete m_pEffects;
			m_pEffects = NULL;
			return FALSE;
		}
	}

	BOOL bRes = m_pEffects->Add(pcszConfig, pcszEffectName);

	return bRes;
}


//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
void CSceneManager::DrawEffect(const char* pcszEffectName, int iWX, int iWY, int iWZ)
{
	if (!m_pEffects)
		return;

	m_pEffects->Draw(pcszEffectName, iWX, iWY, iWZ);
}


//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
void CSceneManager::DelEffect(const char* pcszEffectName)
{
	if (!m_pEffects)
		return;

	m_pEffects->Del(pcszEffectName);
}


//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
void CSceneManager::ClearAllEffects()
{
	if (!m_pEffects)
		return;

	m_pEffects->Free();

	DelListener(PX_EFFECTS);
	m_pEffects = NULL;
}

