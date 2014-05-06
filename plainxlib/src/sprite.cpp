#include "plainx.h"
#include "sprite.h"


#define TS_NONE    0x00000001
#define TS_CREATED 0x00000002
#define TS_STRETCH 0x00000004
#define TS_ROTATE  0x00000008


CSprite::CSprite()
{
  strcpy(m_szName, "");
  strcpy(m_szData, "");
  strcpy(m_szBlendData, "");
  m_iColorKey = 0;
  m_bIsUseSysFontRM = FALSE;
  m_ptWPos.x = 0;
  m_ptWPos.y = 0;
  m_bVisible = TRUE;
  m_nSpeed = 0;
  m_nAngle = 0;
  m_nCurFrame = 0;
  m_nCurRow = 0;
  m_nFrames = 1;
  m_nRows = 1;
  m_nRowHeight = 0;
  for (int i=0; i<MAX_FRAMES; i++) {
    m_nFrameWidth[i] = 0;
    m_nTransFrameWidth[i] = 0;
  }
  m_ptHotSpot.x = 0;
  m_ptHotSpot.y = 0;
  m_ptTransHotSpot.x = 0;
  m_ptTransHotSpot.y = 0;
  m_nZPos = 0;
  m_pParent = NULL;
  m_pSurface = NULL;
  m_pBlendSurface = NULL;
  m_nAnimCycles = 0;
  m_dwBlendColor = 0;
  m_dwAnimState = SS_NONE;
  m_bUpFrames = TRUE;
  m_nAlpha = 0;
  m_bIsDirty = TRUE;
  m_pTransSurf = NULL;
  m_pBlendTransSurf = NULL;
  m_nRotAngle = 0;
  m_dwTransState = TS_NONE;
  m_nTransRowHeight = 0;
  m_nMaxFrameWidth = 0;
  m_nMaxFrameHeight = 0;
  m_dwPrevTick = 0;
  m_dwDiffTick = 1;
  m_pBox = NULL;
}


CSprite::~CSprite()
{
}


//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
BOOL CSprite::CreateTransformSurface(int nMaxFrameWidth, int nMaxFrameHeight)
{
  DeleteTransformSurface();

  CDisplay* pDisp = GetScene()->GetEngine()->GetVideo()->GetDisplay();
  if (!pDisp)
    return FALSE;

  m_nMaxFrameWidth = nMaxFrameWidth;
  m_nMaxFrameHeight = nMaxFrameHeight;

  int nTransSurfWidth = nMaxFrameWidth * m_nFrames;
  int nTransSurfHeight = nMaxFrameHeight * m_nRows;

  HRESULT hr = pDisp->CreateSurface(&m_pTransSurf, nTransSurfWidth, nTransSurfHeight);
  if (FAILED(hr)) {
    hr = pDisp->CreateSurface(&m_pTransSurf, nTransSurfWidth, nTransSurfHeight, FALSE);
    if (FAILED(hr))
      return FALSE;
  }
  if (m_pBlendSurface) {
    hr = pDisp->CreateSurface(&m_pBlendTransSurf, nTransSurfWidth, nTransSurfHeight, FALSE);
    if (FAILED(hr))
      return FALSE;
  }
  m_dwTransState |= TS_CREATED;
  return TRUE;
}


//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
void CSprite::DeleteTransformSurface()
{
  if (m_pTransSurf)
    delete m_pTransSurf;

  if (m_pBlendTransSurf)
    delete m_pBlendTransSurf;

  m_pBlendTransSurf = NULL;
  m_pTransSurf = NULL;
  
  m_dwTransState = TS_NONE;
}


//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
BOOL CSprite::ScaleFrame(float fScale)
{
  CDisplay* pDisp = GetScene()->GetEngine()->GetVideo()->GetDisplay();
  if (!pDisp)
    return FALSE;
  
  if (!(m_dwTransState & TS_CREATED)) {
    if (!CreateTransformSurface(GetFrameWidth(), GetRowHeight()))
      return FALSE;
  }

  if (m_dwTransState & TS_ROTATE)
    return FALSE;

  DirtSprite();

  s_UpdateDimension(fScale);

  RECT rcSource, rcDest;
  int nWidth = 0, nTransWidth = 0;

  for (int i=0; i<m_nFrames; i++) {
    nWidth += m_nFrameWidth[i];
    nTransWidth += m_nTransFrameWidth[i];
  }

  rcSource.left = 0;
  rcSource.top = 0;
  rcSource.right = nWidth;
  rcSource.bottom = m_nRowHeight * m_nRows;

  rcDest.left = 0;
  rcDest.top = 0;
  rcDest.right = nTransWidth;
  rcDest.bottom = m_nTransRowHeight * m_nRows;
  
  HRESULT hr = pDisp->BltStretch(&rcDest, m_pTransSurf, &rcSource, m_pSurface);
  if (FAILED(hr)) {
    return FALSE;
  }
  if (m_pBlendTransSurf) {
    hr = pDisp->BltStretch(&rcDest, m_pBlendTransSurf, &rcSource, m_pBlendSurface);
    if (FAILED(hr)) {
      return FALSE;
    }
  }
  if (m_pSurface->IsColorKeyed()) {
    m_pTransSurf->SetColorKey(CLR_INVALID);
  }
  m_dwTransState |= TS_STRETCH;
  m_bIsDirty = TRUE;
  return TRUE;
}


//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
BOOL CSprite::RotateFrame(int nAngle)
{
  CDisplay* pDisp = GetScene()->GetEngine()->GetVideo()->GetDisplay();
  if (!pDisp)
    return FALSE;

  if (!(m_dwTransState & TS_CREATED)) {
    if (!CreateTransformSurface(GetFrameWidth(), GetRowHeight()))
      return FALSE;
  }

  if (m_dwTransState & TS_STRETCH)
    return FALSE;
  
  RECT    rcFrame;
  HRESULT hr;

  m_nAngle += nAngle;
  if (m_nAngle > 360) {
    m_nAngle -= 360;
  } else if (m_nAngle < -360) {
    m_nAngle += 360;
  }
  
  POINT ptO;
  ptO.x = m_ptHotSpot.x;
  ptO.y = m_nRowHeight - m_ptHotSpot.y;

  rcFrame.left = 0;
  for (int i=0; i<m_nCurFrame; i++)
    rcFrame.left += m_nFrameWidth[i];
  rcFrame.right = rcFrame.left + m_nFrameWidth[m_nCurFrame];
  rcFrame.top = m_nCurRow*m_nRowHeight;
  rcFrame.bottom = rcFrame.top + m_nRowHeight;
      
  hr = pDisp->BltRotate(&rcFrame, m_pTransSurf, m_pSurface, ptO, m_nAngle);
  if (FAILED(hr))
    return FALSE;
  
  if (m_pBlendTransSurf) {
    hr = pDisp->BltRotate(&rcFrame, m_pBlendTransSurf, m_pBlendSurface, ptO, m_nAngle);
    if (FAILED(hr))
      return FALSE;
  }
  if (m_pSurface->IsColorKeyed()) {
    m_pTransSurf->SetColorKey(CLR_INVALID);
  }
  m_bIsDirty = TRUE;
  m_dwTransState |= TS_ROTATE;
  return TRUE;
}


//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
BOOL CSprite::SetRotation(int nRotAngle)
{
  if (nRotAngle == m_nRotAngle)
    return TRUE;

  m_nRotAngle = nRotAngle;
  
  if (m_nRotAngle != 0) {
    m_dwAnimState |= SS_ROTATE;
  } else {
    m_dwAnimState &= ~SS_ROTATE;
  }
  m_bIsDirty = TRUE;
  return TRUE;
}


//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
void CSprite::s_UpdateDimension(float fScale)
{
  if (m_dwTransState == (TS_CREATED | TS_NONE)) {
    m_nAngle = 0;
    m_nTransRowHeight = m_nRowHeight;
    for (int i=0; i<m_nFrames; i++) {
      m_nTransFrameWidth[i] = m_nFrameWidth[i];
    }  
    m_ptTransHotSpot.x = m_ptHotSpot.x;
    m_ptTransHotSpot.y = m_ptHotSpot.y;
  }

  m_nTransRowHeight = (int)((float)m_nTransRowHeight * fScale);
  for (int i = 0; i < m_nFrames; i++) {
    m_nTransFrameWidth[i] = (int)((float)m_nTransFrameWidth[i] * fScale);
  }
  
  m_ptTransHotSpot.x = (int)((float)m_ptTransHotSpot.x * fScale);
  m_ptTransHotSpot.y = (int)((float)m_ptTransHotSpot.y * fScale);
}


//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
POINT CSprite::GetHotSpot() 
{
  if (m_dwTransState & TS_STRETCH)
    return m_ptTransHotSpot;
  else
    return m_ptHotSpot;
}


//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
int CSprite::GetRowHeight() 
{
  if (m_dwTransState & TS_STRETCH)
    return m_nTransRowHeight;
  else
    return m_nRowHeight;
}


//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
int CSprite::GetFrameWidth() 
{
  if (m_dwTransState & TS_STRETCH)
    return m_nTransFrameWidth[m_nCurFrame];
  else
    return m_nFrameWidth[m_nCurFrame];
}


//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
CSurface* CSprite::GetSurface()
{
  if ((m_dwTransState & TS_STRETCH) || (m_dwTransState & TS_ROTATE))
    return m_pTransSurf;
  else
    return m_pSurface;
}


//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
CSurface* CSprite::GetBlendSurface()
{
  if ((m_dwTransState & TS_STRETCH) || (m_dwTransState & TS_ROTATE))
    return m_pBlendTransSurf;
  else
    return m_pBlendSurface;
}


//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
BOOL CSprite::Init(CVars* pVars, CSceneManager* pParent, const char* pszName)
{
  if (pVars == NULL || pszName == NULL || pParent == NULL)
    return FALSE;

  strcpy(m_szName, pszName);
  m_pParent = pParent;

  char buffer[MAX_BUFFER];
  int value = 0;
  int framewdef = 0;

  //read data
  if (!pVars->GetString(L_SPRITE, pszName, L_RESDATA, buffer)) {
    return FALSE;
  }
  strcpy(m_szData, buffer);

  // check if we will use sysfont res man or data res man
  m_pSurface = m_pParent->GetResourceManager(m_bIsUseSysFontRM)->GetSurface(m_szData);
  if (m_pSurface == NULL) {
      m_bIsUseSysFontRM = !m_bIsUseSysFontRM;
      m_pSurface = m_pParent->GetResourceManager(m_bIsUseSysFontRM)->GetSurface(m_szData);
      if (m_pSurface == NULL) {
          return FALSE;
      }
  }

  //read xpos
  if (!pVars->GetInt(L_SPRITE, pszName, L_WPOSXY, 0, &value)) {
    return FALSE;
  }
  m_ptWPos.x = value;

  //read ypos
  if (!pVars->GetInt(L_SPRITE, pszName, L_WPOSXY, 1, &value)) {
    return FALSE;
  }
  m_ptWPos.y = value;

  //read hotspot [optional]
  if (pVars->GetInt(L_SPRITE, pszName, L_HOTSPOT, 0, &value))
    m_ptHotSpot.x = value;
  if (pVars->GetInt(L_SPRITE, pszName, L_HOTSPOT, 1, &value))
    m_ptHotSpot.y = value;

  //read visible [optional]
  if (pVars->GetInt(L_SPRITE, pszName, L_VISIBLE, 0, &value))
    m_bVisible = (BOOL)value;

  //read speed [optional]
  if (pVars->GetInt(L_SPRITE, pszName, L_SPEED, 0, &value))
    SetSpeed(value);
  
  //read frames [optional]
  if (pVars->GetInt(L_SPRITE, pszName, L_FRAMES, 0, &value))
    m_nFrames = value;

  //read rows [optional]
  if (pVars->GetInt(L_SPRITE, pszName, L_ROWS, 0, &value))
    m_nRows = value;

  //read zpos [optional]
  if (pVars->GetInt(L_SPRITE, pszName, L_ZPOS, 0, &value))
    m_nZPos = value;

  //read alpha [optional]
  if (pVars->GetInt(L_SPRITE, pszName, L_ALPHA, 0, &value))
    m_nAlpha = value;

  //read blenddata [optional]
  if (pVars->GetString(L_SPRITE, pszName, L_BLENDDATA, buffer)) {
    strcpy(m_szBlendData, buffer);
    m_pBlendSurface = m_pParent->GetResourceManager(m_bIsUseSysFontRM)->GetSurface(m_szBlendData);
  }

  //read framewidth [optional]
  if (m_nFrames > 1) {
    framewdef = pVars->GetInt(L_SPRITE, pszName, L_FRAMEWIDTH, 0, &value);
    if (framewdef) {
      int i = 0, res;
      res = pVars->GetInt(L_SPRITE, pszName, L_FRAMEWIDTH, i, &value);
      while (res) {
        m_nFrameWidth[i++] = value;
        res = pVars->GetInt(L_SPRITE, pszName, L_FRAMEWIDTH, i, &value);
      }
    }
  }

  //read blendcolor [optional]
  BYTE nRGB[3];
  for (int i=0; i<3; i++) {
    nRGB[i] = 0;
    if (pVars->GetInt(L_SPRITE, pszName, L_BLENDCOLOR, i, &value))
      nRGB[i] = value;
  }
  m_dwBlendColor = RGB(nRGB[0], nRGB[1], nRGB[2]);

  //read rotation angle [optional]
  if (pVars->GetInt(L_SPRITE, pszName, L_ROTATION, 0, &value))
    SetRotation(value);

  // read bounding box [optional]
  if (pVars->GetInt(L_SPRITE, pszName, L_BBOX, 0, &value)) {
    CPXBBox* pBox = new CPXBBox();
    if (pBox == NULL) {
        return FALSE;
    }
    RECT rc;
    rc.left = value;
    if (!pVars->GetInt(L_SPRITE, pszName, L_BBOX, 1, &value)) {
        delete pBox;
        return FALSE;
    }
    rc.top = value;
    if (!pVars->GetInt(L_SPRITE, pszName, L_BBOX, 2, &value)) {
        delete pBox;
        return FALSE;
    }
    rc.right = value;
    if (!pVars->GetInt(L_SPRITE, pszName, L_BBOX, 3, &value)) {
        delete pBox;
        return FALSE;
    }
    rc.bottom = value;
    pBox->Set(rc.left, rc.top, rc.right, rc.bottom);
    SetBox(pBox);
  } else if (pVars->GetInt(L_SPRITE, pszName, L_BCIRCLE, 0, &value)) {
    CPXBBox* pBox = new CPXBBox();
    if (pBox == NULL) {
        return FALSE;
    }
    POINT pt;
    pt.x = value;
    if (!pVars->GetInt(L_SPRITE, pszName, L_BCIRCLE, 1, &value)) {
        delete pBox;
        return FALSE;
    }
    pt.y = value;
    if (!pVars->GetInt(L_SPRITE, pszName, L_BCIRCLE, 2, &value)) {
        delete pBox;
        return FALSE;
    }
    pBox->Set(pt.x, pt.y, value);
    SetBox(pBox);
  }

  //init frame
  int nValue = m_pParent->GetResourceManager(m_bIsUseSysFontRM)->GetHeight(m_szData);
  if (nValue == -1) 
    return FALSE;

  m_nRowHeight = (int)(nValue/m_nRows);

  if (!framewdef) {
    int nFW;

    nValue = m_pParent->GetResourceManager(m_bIsUseSysFontRM)->GetWidth(m_szData);
    nFW = (int)(nValue/m_nFrames);
    for (int i=0; i<m_nFrames; i++) {
      m_nFrameWidth[i] = nFW;
    }
  }

  // init color key
  m_iColorKey = m_pParent->GetResourceManager(m_bIsUseSysFontRM)->GetColorKey(m_szData);

  //set rect as dirty
  m_bIsDirty = TRUE;

  //all was ok
  return TRUE;
}


//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
BOOL CSprite::Init(SPRITE_DATA* pData, CSceneManager* pParent, const char* pszName)
{
  if (pData == NULL || pParent == 0 || pszName == 0)
    return FALSE;

  strcpy(m_szName, pszName);
  m_pParent = pParent;

  // set resource data
  strcpy(m_szData, pData->szData);

  // check if we will use sysfont res man or data res man
  m_pSurface = m_pParent->GetResourceManager(m_bIsUseSysFontRM)->GetSurface(m_szData);
  if (m_pSurface == NULL) {
      m_bIsUseSysFontRM = !m_bIsUseSysFontRM;
      m_pSurface = m_pParent->GetResourceManager(m_bIsUseSysFontRM)->GetSurface(m_szData);
      if (m_pSurface == NULL)
          return FALSE;
  }

  // set wpos
  m_ptWPos = pData->ptWPos;

  // set hotspot
  m_ptHotSpot = pData->ptHotSpot;

  // set visible
  m_bVisible = pData->bVisible;

  // set frames
  m_nFrames = pData->nFrames;

  // set rows
  m_nRows = pData->nRows;

  // set zpos
  m_nZPos = pData->nZPos;

  // set framewidth
  int framewdef = 0;

  if (m_nFrames > 1) {
    int i = 0, res;
    framewdef = pData->nFrameWidth[i];
    res = framewdef;
    while (res > 0) {
      m_nFrameWidth[i++] = res;
      res = pData->nFrameWidth[i];
    }
  }

  // init frame
  int nValue = m_pParent->GetResourceManager(m_bIsUseSysFontRM)->GetHeight(m_szData);
  if (nValue == -1) 
    return FALSE;

  m_nRowHeight = (int)(nValue/m_nRows);

  if (framewdef == 0) {
    int nValue = m_pParent->GetResourceManager(m_bIsUseSysFontRM)->GetWidth(m_szData);
    framewdef = (int)(nValue / m_nFrames);
    for (int i = 0; i < m_nFrames; i++) {
      m_nFrameWidth[i] = framewdef;
    }
  }

  // init color key
  m_iColorKey = m_pParent->GetResourceManager(m_bIsUseSysFontRM)->GetColorKey(m_szData);

  //set rect as dirty
  m_bIsDirty = TRUE;

  //all was ok
  return TRUE;
}


//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
int  CSprite::GetFrameWidth(int nFrame)
{
  if (m_dwTransState & TS_STRETCH)
    return (nFrame >= 0 && nFrame < m_nFrames) ? m_nTransFrameWidth[nFrame] : 0;
  else
    return (nFrame >= 0 && nFrame < m_nFrames) ? m_nFrameWidth[nFrame] : 0;
}


//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
void CSprite::SetHotSpot(POINT ptHotSpot)
{
  DirtSprite();
    
  if (m_dwTransState & TS_STRETCH) {
    m_ptTransHotSpot.x = ptHotSpot.x;
    m_ptTransHotSpot.y = ptHotSpot.y;
  } else {
    m_ptHotSpot.x = ptHotSpot.x;
    m_ptHotSpot.y = ptHotSpot.y;
  }

  m_bIsDirty = TRUE;
}


//-----------------------------------------------------------------------------
// Name: GetUpdateRect([in] prcWorld, [out] prcSurface)
// Desc: input world rect to update, output corresponding surface rect
//-----------------------------------------------------------------------------
BOOL CSprite::GetUpdateRect(RECT* prcWorld, RECT* prcSurface)
{
  //parallel move
  RECT rcWO;
	
  rcWO.left = prcWorld->left - GetDLWXPos();
  rcWO.bottom = prcWorld->bottom - GetDLWYPos();
  rcWO.right = prcWorld->right - GetDLWXPos();
  rcWO.top = prcWorld->top - GetDLWYPos();

  //check if we in sprite
  if (rcWO.top < 0 || rcWO.bottom > GetRowHeight())
    return FALSE;

  if (rcWO.left > GetFrameWidth() || rcWO.right < 0)
    return FALSE;

  //clip top
  if (rcWO.top >= GetRowHeight())
    prcSurface->top = 0;
  else
    prcSurface->top = GetRowHeight() - rcWO.top;

  //clip bottom
  if (rcWO.bottom <= 0)
    prcSurface->bottom = GetRowHeight();
  else
    prcSurface->bottom = GetRowHeight() - rcWO.bottom;

  //clip left
  if (rcWO.left <= 0)
    prcSurface->left = 0;
  else
    prcSurface->left = rcWO.left;

  //clip right
  if (rcWO.right >= GetFrameWidth())
    prcSurface->right = GetFrameWidth();
  else
    prcSurface->right = rcWO.right;

  //update frame and row
  int nSW = prcSurface->right - prcSurface->left;
  int nSH = prcSurface->bottom - prcSurface->top;
  
  //update world position
  prcWorld->left = GetDLWXPos() + prcSurface->left;
  prcWorld->bottom = GetDLWYPos() + (GetRowHeight() - prcSurface->bottom);
  prcWorld->right = prcWorld->left + nSW;
  prcWorld->top = prcWorld->bottom + nSH;

  //frame
  prcSurface->left += s_GetFrameOffset(m_nCurFrame);
  prcSurface->right = (prcSurface->left + nSW);

  //row
  prcSurface->top += m_nCurRow*GetRowHeight();
  prcSurface->bottom = (prcSurface->top + nSH);

  //all was ok
  return TRUE;
}


//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
void CSprite::Free()
{
  m_bIsDirty = TRUE;
  DeleteTransformSurface();
  m_pSurface = NULL;
  m_pBlendSurface = NULL;
  if (m_pBox) {
      delete m_pBox;
      m_pBox = NULL;
  }
}


//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
void CSprite::ReloadSurface()
{
  m_pSurface = m_pParent->GetResourceManager(m_bIsUseSysFontRM)->GetSurface(m_szData);
  m_pBlendSurface = m_pParent->GetResourceManager(m_bIsUseSysFontRM)->GetSurface(m_szBlendData);
  
  DWORD dwPrevTransState = m_dwTransState;
  if (m_dwTransState & TS_CREATED) {
    CreateTransformSurface(m_nMaxFrameWidth, m_nMaxFrameHeight);
  }
  m_dwTransState = dwPrevTransState;
  if (m_dwTransState & TS_STRETCH) {
    ScaleFrame(1.0);
  }
  if (m_dwTransState & TS_ROTATE) {
    RotateFrame(0);
  }
  m_bIsDirty = TRUE;
}


//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
void CSprite::Update(DWORD dwCurrTick)
{
	// init ticks count
	if (m_dwPrevTick == 0) {
		m_dwPrevTick = dwCurrTick;
		return;
	}

	// get number of ticks since last update
	DWORD dwDiff = dwCurrTick - m_dwPrevTick;

	// calc frame number increment
	int iFrameInc = dwDiff / m_dwDiffTick;

	// frame animation update
	if (m_dwAnimState & SS_ANIM) {
		if (iFrameInc > 0) {
			m_dwPrevTick = dwCurrTick;
			if (m_bUpFrames) {
				m_nCurFrame += iFrameInc;
				if (m_nCurFrame >= m_nFrames) {
					if (m_nAnimCycles > 0) {
						m_nAnimCycles--;
						if (m_nAnimCycles <= 0) {
							m_dwAnimState &= ~SS_ANIM;
							m_nCurFrame = m_nFrames - 1;
						} else {
							m_nCurFrame = m_nCurFrame % m_nFrames;
						}
					} else {
						m_nCurFrame = m_nCurFrame % m_nFrames;
					}
				}
			} else { // frame decrement
				m_nCurFrame -= iFrameInc;
				if (m_nCurFrame < 0) {
					if (m_nAnimCycles > 0) {
						m_nAnimCycles--;
						if (m_nAnimCycles <= 0) {
							m_dwAnimState &= ~SS_ANIM;
							m_nCurFrame = 0;
						} else {
							m_nCurFrame = m_nFrames - 1 + m_nCurFrame % m_nFrames;
						}
					} else {
						m_nCurFrame = m_nFrames - 1 + m_nCurFrame % m_nFrames;
					}
				}
			}
			if (m_dwAnimState & SS_ROTATE) {
				RotateFrame(m_nRotAngle * iFrameInc);
			}
			m_bIsDirty = TRUE;
		}
	} else if (m_dwAnimState & SS_ROTATE) {
		if (iFrameInc > 0) {
			m_dwPrevTick = dwCurrTick;
			RotateFrame(m_nRotAngle * iFrameInc);
			m_bIsDirty = TRUE;
		}
    } else {
        m_dwPrevTick = dwCurrTick; // just update tick
    }

	// update
	if (m_bIsDirty) {
		DirtSprite();
		// m_pParent->OnChangedSpritePos(m_szName); // ????
		m_bIsDirty = FALSE;
	}
}


//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
void CSprite::SetAnimCycles(int nAnimCycles)
{
  if (nAnimCycles < 0)
    return;

  m_nAnimCycles = nAnimCycles;
  m_dwAnimState |= SS_ANIM;
}


//-----------------------------------------------------------------------------
// Name: DirtSprite 
// Desc: Set sprite rect as dirt by sending it to scene dirt rect list
//-----------------------------------------------------------------------------
void CSprite::DirtSprite()
{
  RECT rcW;

  rcW.left = GetDLWXPos();
  rcW.right = rcW.left + GetFrameWidth();
  rcW.bottom = GetDLWYPos();
  rcW.top = rcW.bottom + GetRowHeight();

  GetScene()->DirtWorldRect(&rcW);
}


//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
BOOL CSprite::s_SetNewWPos(int nWX, int nWY)
{
  //check if we in world area
  if (nWX < 0 || nWX >= m_pParent->GetWorldWidth())
    return FALSE;

  if (nWY < 0 || nWY >= m_pParent->GetWorldHeight())
    return FALSE;

  //mark old position as dirt
  DirtSprite();

  // check if we have bouding box then move it
  if (m_pBox) {
    m_pBox->Move(nWX - m_ptWPos.x, nWY - m_ptWPos.y);
  }

  //all was cool
  return TRUE;
}


//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
int CSprite::s_GetFrameOffset(int nFrame)
{
  int res = 0;

  if (m_dwTransState & TS_STRETCH)
    for (int i=0; i<nFrame; i++) res += m_nTransFrameWidth[i];
  else
    for (int i=0; i<nFrame; i++) res += m_nFrameWidth[i];

  return res;
}


//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
BOOL CSprite::SetWXPos(int nXPos) 
{
  if (s_SetNewWPos(nXPos, m_ptWPos.y)) {
    m_ptWPos.x = nXPos;
    m_pParent->OnChangedSpritePos(m_szName);
    m_bIsDirty = TRUE;
    return TRUE;
  }
  return FALSE;
}


//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
BOOL  CSprite::SetWYPos(int nYPos) 
{
  if (s_SetNewWPos(m_ptWPos.x, nYPos)) {
    m_ptWPos.y = nYPos;
    m_pParent->OnChangedSpritePos(m_szName);
    m_bIsDirty = TRUE;
    return TRUE;
  }
  return FALSE;
}


//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
BOOL CSprite::SetZPos(int nZPos) 
{
  if (nZPos == m_nZPos)
    return TRUE;

  if (m_nZPos == 0) {
      m_dwPrevTick = 0;
  }

  m_nZPos = nZPos;

  m_pParent->OnChangedSpritePos(m_szName);
  if (m_nZPos > 0) {
    m_bIsDirty = TRUE;
  } else {
    DirtSprite();
  }
    
  return TRUE;
}


//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
BOOL CSprite::SetWPos(int nXPos, int nYPos, int nZPos)
{
  BOOL bRes = TRUE;

  if (nXPos != m_ptWPos.x || nYPos != m_ptWPos.y) {
    bRes = s_SetNewWPos(nXPos, nYPos);
    if (bRes) {
      m_ptWPos.x = nXPos;
      m_ptWPos.y = nYPos;
      m_nZPos = nZPos;

      m_pParent->OnChangedSpritePos(m_szName);
      m_bIsDirty = TRUE;
    }
  }
  return bRes;
}


//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
void  CSprite::SetAlpha(BYTE nAlpha)
{
  m_nAlpha = nAlpha;
  m_bIsDirty = TRUE;
}


//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
void CSprite::SetVisible(BOOL bVisible, BOOL bIsDirt) 
{
  if (m_bVisible != bVisible) {
    m_bVisible = bVisible;
    if (bIsDirt) DirtSprite();
    if (m_bVisible) {
        m_dwPrevTick = 0;
    }
  }
}


//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
BOOL CSprite::SetSpeed(int nSpeed) 
{
  // speed of animation in the frames per one second

    if (nSpeed < 0)
        return FALSE;

  if (m_nSpeed == nSpeed)
    return TRUE;

  m_nSpeed = nSpeed;

  // calculate number of ticks need of frame change
  if (m_nSpeed > 0) {
    m_dwAnimState |= SS_ANIM;
    m_dwDiffTick = 1000 / m_nSpeed;
  } else {
    m_dwAnimState &= ~SS_ANIM;
    m_dwDiffTick = 1;
  }
  m_bIsDirty = TRUE;
  return TRUE;
}


//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
BOOL CSprite::SetRow(int nRow) 
{
  if (nRow < 0 || nRow >= m_nRows)
    return FALSE;

  if (m_nCurRow == nRow)
   return TRUE;

  m_nCurRow = nRow;
  m_bIsDirty = TRUE;

  return TRUE;
}


//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
BOOL CSprite::SetFrame(int nFrame) 
{
  if (nFrame < 0 || nFrame >= m_nFrames)
    return FALSE;

  if (m_nCurFrame == nFrame)
    return TRUE;

  m_nCurFrame = nFrame;
  m_bIsDirty = TRUE;

  return TRUE;
}


//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
inline int CSprite::GetDLWXPos()
{
  if (m_dwTransState & TS_STRETCH)
    return m_ptWPos.x - (int)m_ptTransHotSpot.x;
  else
    return m_ptWPos.x - (int)m_ptHotSpot.x;
}


//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
inline int CSprite::GetDLWYPos()
{
  if (m_dwTransState & TS_STRETCH)
    return m_ptWPos.y - (int)m_ptTransHotSpot.y;
  else
    return m_ptWPos.y - (int)m_ptHotSpot.y;
}


//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
BOOL CSprite::IsInSprite(int nSX, int nSY)
{
  if (!m_bVisible)
    return FALSE;

  //get world coords
  POINT ptW;
  GetScene()->ScreenToWorld(nSX, nSY, &ptW);

  //check for simple case
  if (ptW.x <= GetDLWXPos() || ptW.x >= (GetDLWXPos() + GetFrameWidth()))
    return FALSE;
  if (ptW.y <= GetDLWYPos() || ptW.y >= (GetDLWYPos() + GetRowHeight()))
    return FALSE;

  //if we have color key
  if (m_iColorKey == 1) {
    //get upper-left pixel
    int nBPP = GetScene()->GetEngine()->GetVideo()->GetScreenDepth();
    if (nBPP == 16) {
      SPIXEL16  spul16;
      
      if (!GetSurface()->GetSPixel16(0, 0, &spul16))
        return FALSE;

      //get tested pixel
      SPIXEL16 ptest;

      DWORD dwWidth = 0;
      DWORD dwHeight = 0;

      dwHeight += GetRow()*GetRowHeight();
      dwHeight += GetDLWYPos()+GetRowHeight()-ptW.y;
      dwWidth += s_GetFrameOffset(m_nCurFrame) + ptW.x-GetDLWXPos();

      if (!GetSurface()->GetSPixel16(dwHeight, dwWidth, &ptest))
        return FALSE;

      //check for transparent
      if (ptest.b1 == spul16.b1 && ptest.b2 == spul16.b2)
        return FALSE;
    } else {
      SPIXEL32  spul32;

      if (!GetSurface()->GetSPixel32(0, 0, &spul32))
        return FALSE;

      //get tested pixel
      SPIXEL32 ptest;

      DWORD dwWidth = 0;
      DWORD dwHeight = 0;

      dwHeight += GetRow()*GetRowHeight();
      dwHeight += GetDLWYPos()+GetRowHeight()-ptW.y;
      dwWidth += s_GetFrameOffset(m_nCurFrame) + ptW.x-GetDLWXPos();

      if (!GetSurface()->GetSPixel32(dwHeight, dwWidth, &ptest))
        return FALSE;

      //check for transparent
      if (ptest.b1 == spul32.b1 && ptest.b2 == spul32.b2 && 
          ptest.b3 == spul32.b3 && ptest.b4 == spul32.b4)
        return FALSE;
    }

    //all ok
    return TRUE;
  }
  
  //all ok
  return TRUE;
}


//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
void CSprite::SetBox(CPXBBox* pBox)
{
    if (m_pBox != NULL) {
        delete m_pBox;
        m_pBox = NULL;
    }
    m_pBox = pBox;
}


//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
CSprite* CSprite::Clone()
{
    CSprite* pClone = new CSprite();
    if (pClone == NULL)
        return NULL;

    memcpy(pClone, this, sizeof(CSprite));

    if (m_pBox != NULL) {
        CPXBBox* pBox = new CPXBBox();
        if (pBox == NULL) {
            delete pClone;
            return NULL;
        }
        memcpy(pBox, m_pBox, sizeof(CPXBBox));
        pClone->m_pBox = pBox;
    }
    return pClone;
}
