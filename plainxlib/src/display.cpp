#include "plainx.h"
#include "display.h"


CDisplay::CDisplay()
{
  m_pDD                = NULL;
  m_pddsFrontBuffer    = NULL;
  m_pddsBackBuffer     = NULL;
  m_pddsEffectsBuffer  = NULL;
  
  m_bIsCalcMath = FALSE;
  m_bIsDrawLogo = FALSE;
  m_bUseVideoMemory = FALSE;
  m_bUseHardwareBlt = FALSE;
}


CDisplay::~CDisplay()
{
  DestroyObjects();

  SAFE_RELEASE(m_pDD);
}


//-----------------------------------------------------------------------------
// Name:
// Desc:
//-----------------------------------------------------------------------------
void CDisplay::CalcMath()
{
  if (m_bIsCalcMath)
    return;

  for (int nA=0; nA<256; nA++) {
    for (int nD=0; nD<256; nD++) {
      for (int nS=0; nS<256; nS++) {
        m_nAlpha[nD][nS][nA] = (BYTE)((nA*nS + (255-nA)*nD)/255);
      }
    }
  }
  for (int i=0; i<360; i++) {
    m_nSin[i] = sin(i*pi/180);
    m_nCos[i] = cos(i*pi/180);
  }
  m_bIsCalcMath = TRUE;
}


//-----------------------------------------------------------------------------
// Name:
// Desc:
//-----------------------------------------------------------------------------
void CDisplay::SetUseVideoMemory(BOOL bv)
{
  m_bUseVideoMemory = bv;

  // need reload
}

                                 
//-----------------------------------------------------------------------------
// Name:
// Desc:
//-----------------------------------------------------------------------------
void CDisplay::SetUseHardwareBlt(BOOL bv)
{
  m_bUseHardwareBlt = bv;
}


//-----------------------------------------------------------------------------
// Name: DestroyObjects()
// Desc:
//-----------------------------------------------------------------------------
HRESULT CDisplay::DestroyObjects()
{
  SAFE_RELEASE(m_pddsBackBuffer);
  SAFE_RELEASE(m_pddsEffectsBuffer);
  SAFE_RELEASE(m_pddsFrontBuffer);
  
  if (m_pDD) {
    m_pDD->SetCooperativeLevel(m_hWnd, DDSCL_NORMAL);
  }

  return S_OK;
}


//-----------------------------------------------------------------------------
// Name: DestroyObjects()
// Desc:
//-----------------------------------------------------------------------------
HRESULT CDisplay::CreateDirectDraw() 
{
  // DDraw object create
  SAFE_RELEASE(m_pDD);

  return DirectDrawCreateEx(NULL, (VOID**)&m_pDD, IID_IDirectDraw7, NULL);
}


//-----------------------------------------------------------------------------
// Name: CreateFullScreenDisplay()
// Desc:
//-----------------------------------------------------------------------------
HRESULT CDisplay::CreateFullScreenDisplay(HWND hWnd, 
                                          DWORD dwWidth,
                                          DWORD dwHeight, 
                                          DWORD dwBPP)
{
  HRESULT hr;

  // Cleanup anything from a previous call
  DestroyObjects();

  // Set cooperative level
  hr = m_pDD->SetCooperativeLevel(hWnd, DDSCL_EXCLUSIVE | DDSCL_FULLSCREEN);
  if (FAILED(hr))
    return E_FAIL;

  // Set the display mode
  if (FAILED(m_pDD->SetDisplayMode( dwWidth, dwHeight, dwBPP, 0, 0)))
    return E_FAIL;

  // Create primary surface (with backbuffer attached)
  DDSURFACEDESC2 ddsd;
  memset(&ddsd, 0, sizeof(DDSURFACEDESC2));
  ddsd.dwSize            = sizeof(DDSURFACEDESC2);
  ddsd.dwFlags           = DDSD_CAPS;// | DDSD_BACKBUFFERCOUNT;
  ddsd.ddsCaps.dwCaps    = DDSCAPS_PRIMARYSURFACE;// | DDSCAPS_FLIP | DDSCAPS_COMPLEX;
  //ddsd.dwBackBufferCount = 1;

  if (FAILED(hr = m_pDD->CreateSurface(&ddsd, &m_pddsFrontBuffer, NULL)))
    return E_FAIL;

  //create backbuffer (in the video memory !) copy (for fullscreen)
  //we are going blt all to backbuffer copy then bltfast to backbuffer then flip
  memset(&ddsd, 0, sizeof(DDSURFACEDESC2));
  ddsd.dwSize         = sizeof(DDSURFACEDESC2);
  ddsd.dwFlags        = DDSD_CAPS | DDSD_WIDTH | DDSD_HEIGHT; 

  // try create in video but if not enough :( in the system 
  ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN;// | DDSCAPS_VIDEOMEMORY;
  ddsd.dwWidth        = dwWidth;
  ddsd.dwHeight       = dwHeight;
	
  if (FAILED(hr = m_pDD->CreateSurface(&ddsd, &m_pddsBackBuffer, NULL)))
    return E_FAIL;

  //create effects buffer (in the system memory because we are going to read from buffer
  // so video will be very slow)
  memset(&ddsd, 0, sizeof(DDSURFACEDESC2));
  ddsd.dwSize         = sizeof(DDSURFACEDESC2);
  ddsd.dwFlags        = DDSD_CAPS | DDSD_WIDTH | DDSD_HEIGHT; 
  ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN | DDSCAPS_SYSTEMMEMORY;
  ddsd.dwWidth        = dwWidth;
  ddsd.dwHeight       = dwHeight;
	
  if (FAILED(hr = m_pDD->CreateSurface(&ddsd, &m_pddsEffectsBuffer, NULL)))
    return E_FAIL;

  //remember screen
  m_rcScreen.left = 0;
  m_rcScreen.top = 0;
  m_rcScreen.bottom = dwHeight;
  m_rcScreen.right = dwWidth;

  m_hWnd      = hWnd;
  m_bWindowed = FALSE;
  UpdateBounds();

  //remember bitmask and shifts
  memset(&ddsd, 0, sizeof(ddsd));
  ddsd.dwSize = sizeof(DDSURFACEDESC2);

  //lock surface
  if (FAILED(hr = m_pddsEffectsBuffer->Lock(NULL, &ddsd, DDLOCK_WAIT, NULL)))
    return hr;
  	
  //get surface bit masks
  m_dwBitMask[0] = ddsd.ddpfPixelFormat.dwRBitMask;
  m_dwBitMask[1] = ddsd.ddpfPixelFormat.dwGBitMask;
  m_dwBitMask[2] = ddsd.ddpfPixelFormat.dwBBitMask;
  
  for (int i=0; i<3; i++) {
    DWORD dwTemp;
    CSurface::GetBitMaskInfo(m_dwBitMask[i], &m_dwShift[i], &dwTemp);
  }
  m_pddsEffectsBuffer->Unlock(NULL);

  DrawLogo();
  CalcMath();

  return S_OK;
}
    

//-----------------------------------------------------------------------------
// Name: CreateWindowedDisplay()
// Desc:
//-----------------------------------------------------------------------------
HRESULT CDisplay::CreateWindowedDisplay(HWND hWnd, DWORD dwWidth, DWORD dwHeight)
{
  HRESULT hr;

  // Cleanup anything from a previous call
  DestroyObjects();

  // Set cooperative level
  hr = m_pDD->SetCooperativeLevel(hWnd, DDSCL_NORMAL);
  if (FAILED(hr))
    return E_FAIL;

  RECT  rcWork;
  RECT  rc;
  DWORD dwStyle;

  // If we are still a WS_POPUP window we should convert to a normal app
  // window so we look like a windows app.
  dwStyle  = GetWindowStyle(hWnd);
  dwStyle &= ~WS_POPUP;
  dwStyle |= WS_OVERLAPPED | WS_CAPTION | WS_THICKFRAME | WS_MINIMIZEBOX;
  SetWindowLong(hWnd, GWL_STYLE, dwStyle);

  // Aet window size
  SetRect(&rc, 0, 0, dwWidth, dwHeight);

  AdjustWindowRectEx(&rc, 
                     GetWindowStyle(hWnd), 
                     GetMenu(hWnd) != NULL,
                     GetWindowExStyle(hWnd));

  SetWindowPos(hWnd, NULL, 0, 0, rc.right-rc.left, rc.bottom-rc.top,
               SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE);

  SetWindowPos(hWnd, HWND_NOTOPMOST, 0, 0, 0, 0,
               SWP_NOSIZE | SWP_NOMOVE | SWP_NOACTIVATE);

  //  Make sure our window does not hang outside of the work area
  SystemParametersInfo(SPI_GETWORKAREA, 0, &rcWork, 0);
  GetWindowRect(hWnd, &rc);
  if (rc.left < rcWork.left) rc.left = rcWork.left;
  if (rc.top  < rcWork.top)  rc.top  = rcWork.top;
  SetWindowPos(hWnd, NULL, rc.left, rc.top, 0, 0,
               SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);

  LPDIRECTDRAWCLIPPER pcClipper;
    
  // Create the primary surface
  DDSURFACEDESC2 ddsd;
  memset(&ddsd, 0, sizeof(DDSURFACEDESC2));
  ddsd.dwSize         = sizeof(DDSURFACEDESC2);
  ddsd.dwFlags        = DDSD_CAPS;
  ddsd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE;

  if (FAILED(m_pDD->CreateSurface( &ddsd, &m_pddsFrontBuffer, NULL)))
    return E_FAIL;

  if (FAILED(hr = m_pDD->CreateClipper(0, &pcClipper, NULL)))
    return E_FAIL;

  if (FAILED(hr = pcClipper->SetHWnd(0, hWnd))) {
    pcClipper->Release();
    return E_FAIL;
  }

  if (FAILED(hr = m_pddsFrontBuffer->SetClipper(pcClipper))) {
    pcClipper->Release();
    return E_FAIL;
  }

  // Done with clipper
  pcClipper->Release();

  //create backbuffer (in the video memory !) copy (for fullscreen)
  //we are going blt all to backbuffer copy then bltfast to backbuffer then flip
  memset(&ddsd, 0, sizeof(DDSURFACEDESC2));
  ddsd.dwSize         = sizeof(DDSURFACEDESC2);
  ddsd.dwFlags        = DDSD_CAPS | DDSD_WIDTH | DDSD_HEIGHT;
  
  // try create in video but if not enough :( in the system 
  ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN;// | DDSCAPS_VIDEOMEMORY;
  ddsd.dwWidth        = dwWidth;
  ddsd.dwHeight       = dwHeight;
	
  if (FAILED(hr = m_pDD->CreateSurface(&ddsd, &m_pddsBackBuffer, NULL)))
    return E_FAIL;
  
  memset(&ddsd, 0, sizeof(DDSURFACEDESC2));
  ddsd.dwSize         = sizeof(DDSURFACEDESC2);
  ddsd.dwFlags        = DDSD_CAPS | DDSD_WIDTH | DDSD_HEIGHT; 
  ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN | DDSCAPS_SYSTEMMEMORY;
  ddsd.dwWidth        = dwWidth;
  ddsd.dwHeight       = dwHeight;
	
  if (FAILED(hr = m_pDD->CreateSurface(&ddsd, &m_pddsEffectsBuffer, NULL)))
    return E_FAIL;

  //remember screen
  m_rcScreen.left = 0;
  m_rcScreen.top = 0;
  m_rcScreen.bottom = dwHeight;
  m_rcScreen.right = dwWidth;
	
  m_hWnd      = hWnd;
  m_bWindowed = TRUE;
  UpdateBounds();

  //remember bitmask and shifts
  memset(&ddsd, 0, sizeof(ddsd));
  ddsd.dwSize = sizeof(DDSURFACEDESC2);

  //lock surface
  if (FAILED(hr = m_pddsEffectsBuffer->Lock(NULL, &ddsd, DDLOCK_WAIT, NULL)))
    return hr;
  	
  //get surface bit masks
  m_dwBitMask[0] = ddsd.ddpfPixelFormat.dwRBitMask;
  m_dwBitMask[1] = ddsd.ddpfPixelFormat.dwGBitMask;
  m_dwBitMask[2] = ddsd.ddpfPixelFormat.dwBBitMask;
  
  for (int i=0; i<3; i++) {
    DWORD dwTemp;
    CSurface::GetBitMaskInfo(m_dwBitMask[i], &m_dwShift[i], &dwTemp);
  }
  m_pddsEffectsBuffer->Unlock(NULL);
  
  DrawLogo();
  CalcMath();

  return S_OK;
}


//-----------------------------------------------------------------------------
// Name:
// Desc:
//-----------------------------------------------------------------------------
void CDisplay::DrawLogo()
{
  HDC hDC;
  HFONT hFontBold, hFontLight;

  if (m_bIsDrawLogo)
    return;
      
  Clear();

  GetBackBuffer()->GetDC(&hDC);
  
  hFontBold = CreateFont(40, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, ANSI_CHARSET, 
    OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY, DEFAULT_PITCH, "Arial");
  hFontLight = CreateFont(30, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, ANSI_CHARSET, 
    OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY, DEFAULT_PITCH, "Arial");
  
  SetTextColor(hDC, RGB(255, 255, 255));
  SetBkColor(hDC, RGB(0, 0, 0));
  
  HFONT hOldFont = (HFONT)SelectObject(hDC, hFontBold);
  TextOut(hDC, 100, 100, PX_LOGO, strlen(PX_LOGO));

  SelectObject(hDC, hFontLight);
  TextOut(hDC, 100, 140, PX_GetVersion(), strlen(PX_GetVersion()));

  SelectObject(hDC, hOldFont);

  GetBackBuffer()->ReleaseDC(hDC);

  DeleteObject(hFontBold);
  DeleteObject(hFontLight);
  
  Present();

  m_bIsDrawLogo = TRUE;
}


//-----------------------------------------------------------------------------
// Name: CDisplay::CreateSurface()
// Desc: Create a DirectDrawSurface
//-----------------------------------------------------------------------------
HRESULT CDisplay::CreateSurface(CSurface** ppSurface, int nWidth, int nHeight, BOOL bIsVideoMemory)
{
  HRESULT        hr;
  DDSURFACEDESC2 ddsd;
	
  if (m_pDD == NULL || ppSurface == NULL) 
    return E_INVALIDARG;

  *ppSurface = NULL;

  // Create a DirectDrawSurface for this bitmap
  memset(&ddsd, 0, sizeof(DDSURFACEDESC2));
  ddsd.dwSize         = sizeof(DDSURFACEDESC2);
  ddsd.dwFlags        = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH;
  ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN;
  ddsd.dwWidth = nWidth;
  ddsd.dwHeight = nHeight;
	
  if (bIsVideoMemory && m_bUseVideoMemory) {
    ddsd.ddsCaps.dwCaps |= DDSCAPS_VIDEOMEMORY;
  } else {
    ddsd.ddsCaps.dwCaps |= DDSCAPS_SYSTEMMEMORY;
  }
  
  (*ppSurface) = new CSurface();

  hr = (*ppSurface)->Create(m_pDD, &ddsd);
  if (FAILED(hr)) {
    delete (*ppSurface);
    *ppSurface = NULL;
  } else {
    hr = (*ppSurface)->Clear(0);
  }
  return hr;
}


//-----------------------------------------------------------------------------
// Name: CDisplay::CreateSurfaceFromAny()
// Desc: Create a DirectDrawSurface from a bitmap resource or bitmap file.
//-----------------------------------------------------------------------------
HRESULT CDisplay::CreateSurfaceFromAny(CSurface** ppSurface,
                                       BYTE* pImage, 
                                       DWORD dwSize,
                                       int* pnWidth, 
                                       int* pnHeight,
                                       BOOL bIsVideoMemory)
{
  HRESULT           hr;
  DDSURFACEDESC2    ddsd;
  PLMemSource       pMemSrc;
  PLAnyPicDecoder   pDecoder;
  PLWinBmp          pBmp;
	
  if (m_pDD == NULL || ppSurface == NULL || pImage == NULL) 
    return E_INVALIDARG;

  *ppSurface = NULL;

  pMemSrc.Open(pImage, dwSize);
  pDecoder.MakeBmp(&pMemSrc, &pBmp);

  // Create a DirectDrawSurface for this bitmap
  memset(&ddsd, 0, sizeof(DDSURFACEDESC2));
  ddsd.dwSize         = sizeof(DDSURFACEDESC2);
  ddsd.dwFlags        = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH;
  ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN;
  	
  if (bIsVideoMemory && m_bUseVideoMemory) {
    ddsd.ddsCaps.dwCaps |= DDSCAPS_VIDEOMEMORY;
  } else {
    ddsd.ddsCaps.dwCaps |= DDSCAPS_SYSTEMMEMORY;
  }

  if (*pnWidth == -1) {
    ddsd.dwWidth = pBmp.GetWidth();
    *pnWidth = ddsd.dwWidth;
  } else {
    ddsd.dwWidth = *pnWidth;
  }
	
  if (*pnHeight == -1) {
    ddsd.dwHeight = pBmp.GetHeight();
    *pnHeight = ddsd.dwHeight;
  } else {
    ddsd.dwHeight = *pnHeight;
  }
	
  (*ppSurface) = new CSurface();
  if (FAILED(hr = (*ppSurface)->Create(m_pDD, &ddsd))) {
    delete (*ppSurface);
    return hr;
  }
  (*ppSurface)->Clear(0);
	
  // Draw the bitmap on this surface
  if (FAILED(hr = (*ppSurface)->DrawBitmap( &pBmp, *pnWidth, *pnHeight))) 
    return hr;
    
  return S_OK;
}


//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
HRESULT CDisplay::Present()
{
  HRESULT hr;

  while (TRUE) {
    if (m_bWindowed) {
      hr = m_pddsFrontBuffer->Blt(&m_rcWindow, m_pddsBackBuffer, NULL, DDBLT_WAIT, NULL);
    } else {
      hr = m_pddsFrontBuffer->BltFast(0, 0, m_pddsBackBuffer, &m_rcScreen, DDBLTFAST_NOCOLORKEY | DDBLTFAST_WAIT);
    }
    if (hr == DD_OK)
      break;

    if (hr == DDERR_SURFACELOST) {
      m_pddsFrontBuffer->Restore();
      m_pddsBackBuffer->Restore();
      break;
    }
    if (hr != DDERR_WASSTILLDRAWING)
      break;
  }  
  return hr;
}


//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
HRESULT CDisplay::BltAlphaBlend( DWORD x, DWORD y, CSurface* pSurface, COLORREF dwBlendColor, RECT* prc)
{
    if (GetScreenDepth() == 16)
      return BltAlphaBlend16(x, y, pSurface, dwBlendColor, prc);
    else
      return BltAlphaBlend32(x, y, pSurface, dwBlendColor, prc);
}


//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
HRESULT CDisplay::BltAlphaBlend16( DWORD x, DWORD y, CSurface* pSurface, COLORREF dwBlendColor, RECT* prc)
{
  //define rect dimension
  DWORD dwWidth = prc->right - prc->left;
  DWORD dwHeight= prc->bottom - prc->top;

  DWORD dwOffset = 0;
  DWORD dwOffset_bb = 0;

  //at first we copy rect from back buffer to effects buffer
  RECT rcEB;
  rcEB.left = x;
  rcEB.top = y;
  rcEB.right = rcEB.left + dwWidth;
  rcEB.bottom = rcEB.top + dwHeight;
  m_pddsEffectsBuffer->BltFast(x, y, m_pddsBackBuffer, &rcEB, DDBLTFAST_NOCOLORKEY);

  //now work with effects buffer
  DDSURFACEDESC2 ddsd;
  DDSURFACEDESC2 ddsd_bb;
  LPDIRECTDRAWSURFACE7 pdds = pSurface->GetDDrawSurface();
  HRESULT hr;
	
  memset(&ddsd, 0, sizeof(ddsd));
  ddsd.dwSize = sizeof(DDSURFACEDESC2);

  memset(&ddsd_bb, 0, sizeof(DDSURFACEDESC2));
  ddsd_bb.dwSize = sizeof(DDSURFACEDESC2);
	
  //we need call ConvertColor before lock
  DWORD dwTempBC;

  //convert blend color to surface color
  dwTempBC = pSurface->ConvertGDIColor(dwBlendColor);

  //lock surface
  if (FAILED(hr = pdds->Lock(NULL, &ddsd, DDLOCK_WAIT, NULL)))
    return hr;

  if (FAILED(hr = m_pddsEffectsBuffer->Lock(NULL, &ddsd_bb, DDLOCK_WAIT, NULL)))
    return hr;
			
  dwOffset_bb = y*ddsd_bb.lPitch;
  dwOffset = prc->top*ddsd.lPitch;
  dwOffset_bb += x*2;
  dwOffset += prc->left*2;
	
  WORD *pwDest = (WORD*)((BYTE*)ddsd_bb.lpSurface + dwOffset_bb);
  WORD *pwSource = (WORD*)((BYTE*)ddsd.lpSurface + dwOffset);
	
  DWORD dwDiff_bb = (ddsd_bb.lPitch -  dwWidth*2)/2;
  DWORD dwDiff = (ddsd.lPitch - dwWidth*2)/2;
	
  DWORD dwD[3];
  DWORD dwS[3];
  DWORD dwBC[3];
  DWORD dwMaxBC[3];
		
  //get Red Green And Blue of Blend color
  for (int i=0; i<3; i++) {
    dwBC[i] = (DWORD)(dwTempBC & m_dwBitMask[i]) >> m_dwShift[i];
    dwMaxBC[i] = m_dwBitMask[i] >> m_dwShift[i];
  }
	
  //blend
  for (DWORD iY=0; iY<dwHeight; iY++) {
    for (DWORD iX=0; iX<dwWidth; iX++) {
      for (DWORD i=0; i<3; i++) {
	dwS[i] = (DWORD)(m_dwBitMask[i] & *pwSource) >> m_dwShift[i];
	dwD[i] = (DWORD)(m_dwBitMask[i] & *pwDest) >> m_dwShift[i];
	dwD[i] = (DWORD)(((dwMaxBC[i] - dwS[i])*dwD[i] + dwS[i]*dwBC[i])/dwMaxBC[i]);
	dwD[i] <<= m_dwShift[i];
      }				
      *pwDest = (WORD)(dwD[0] | dwD[1] | dwD[2]);
      pwDest++;
      pwSource++;
    }
    pwDest += dwDiff_bb;
    pwSource += dwDiff;
  }

  //unlock surfaces
  m_pddsEffectsBuffer->Unlock(NULL);
  pdds->Unlock(NULL);
	
  //copy effects to back
  m_pddsBackBuffer->BltFast(x, y, m_pddsEffectsBuffer, &rcEB,  DDBLTFAST_NOCOLORKEY);
  return S_OK;
}


//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
HRESULT CDisplay::BltAlphaBlend32( DWORD x, DWORD y, CSurface* pSurface, COLORREF dwBlendColor, RECT* prc)
{
  //define rect dimension
  DWORD dwWidth = prc->right - prc->left;
  DWORD dwHeight= prc->bottom - prc->top;

  DWORD dwOffset = 0;
  DWORD dwOffset_bb = 0;

  //at first we copy rect from back buffer to effects buffer
  RECT rcEB;
  rcEB.left = x;
  rcEB.top = y;
  rcEB.right = rcEB.left + dwWidth;
  rcEB.bottom = rcEB.top + dwHeight;
  m_pddsEffectsBuffer->BltFast(x, y, m_pddsBackBuffer, &rcEB, DDBLTFAST_NOCOLORKEY);

  //now work with effects buffer
  DDSURFACEDESC2 ddsd;
  DDSURFACEDESC2 ddsd_bb;
  LPDIRECTDRAWSURFACE7 pdds = pSurface->GetDDrawSurface();
  HRESULT hr;
	
  memset(&ddsd, 0, sizeof(ddsd));
  ddsd.dwSize = sizeof(DDSURFACEDESC2);

  memset(&ddsd_bb, 0, sizeof(DDSURFACEDESC2));
  ddsd_bb.dwSize = sizeof(DDSURFACEDESC2);
	
  //we need call ConvertColor before lock
  DWORD dwTempBC;

  //convert blend color to surface color
  dwTempBC = pSurface->ConvertGDIColor(dwBlendColor);

  //lock surface
  if (FAILED(hr = pdds->Lock(NULL, &ddsd, DDLOCK_WAIT, NULL)))
    return hr;

  if (FAILED(hr = m_pddsEffectsBuffer->Lock(NULL, &ddsd_bb, DDLOCK_WAIT, NULL)))
    return hr;
			
  dwOffset_bb = y*ddsd_bb.lPitch;
  dwOffset = prc->top*ddsd.lPitch;
  dwOffset_bb += x*4;
  dwOffset += prc->left*4;
	
  DWORD *pdwDest = (DWORD*)((BYTE*)ddsd_bb.lpSurface + dwOffset_bb);
  DWORD *pdwSource = (DWORD*)((BYTE*)ddsd.lpSurface + dwOffset);
	
  DWORD dwDiff_bb = (ddsd_bb.lPitch -  dwWidth*4)/4;
  DWORD dwDiff = (ddsd.lPitch - dwWidth*4)/4;
	
  DWORD dwD[3];
  DWORD dwS[3];
  DWORD dwBC[3];
  DWORD dwMaxBC[3];
  		
  //get Red Green And Blue of Blend color
  for (int i=0; i<3; i++) {
    dwBC[i] = (DWORD)(dwTempBC & m_dwBitMask[i]) >> m_dwShift[i];
    dwMaxBC[i] = m_dwBitMask[i] >> m_dwShift[i];
  }
	
  //blend
  for (DWORD iY=0; iY<dwHeight; iY++) {
    for (DWORD iX=0; iX<dwWidth; iX++) {
      for (DWORD i=0; i<3; i++) {
	dwS[i] = (DWORD)(m_dwBitMask[i] & *pdwSource) >> m_dwShift[i];
	dwD[i] = (DWORD)(m_dwBitMask[i] & *pdwDest) >> m_dwShift[i];
	dwD[i] = (DWORD)(((dwMaxBC[i] - dwS[i])*dwD[i] + dwS[i]*dwBC[i])/dwMaxBC[i]);
	dwD[i] <<= m_dwShift[i];
      }				
      *pdwDest = (DWORD)(dwD[0] | dwD[1] | dwD[2]);
      pdwDest++;
      pdwSource++;
    }
    pdwDest += dwDiff_bb;
    pdwSource += dwDiff;
  }

  //unlock surfaces
  m_pddsEffectsBuffer->Unlock(NULL);
  pdds->Unlock(NULL);
	
  //copy effects to back
  m_pddsBackBuffer->BltFast(x, y, m_pddsEffectsBuffer, &rcEB,  DDBLTFAST_NOCOLORKEY);
  return S_OK;
}


//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
HRESULT CDisplay::BltAlpha(DWORD x, DWORD y, CSurface* pSurface, BYTE nAlpha, RECT* prc)
{
  if (GetScreenDepth() == 16)
    return BltAlpha16(x, y, pSurface, nAlpha, prc);
  else
    return BltAlpha32(x, y, pSurface, nAlpha, prc);
}


//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
HRESULT CDisplay::BltAlpha16(DWORD x, DWORD y, CSurface* pSurface, BYTE nAlpha, RECT* prc)
{
  //define rect dimension
  DWORD dwWidth = prc->right - prc->left;
  DWORD dwHeight= prc->bottom - prc->top;
  
  DWORD dwOffset = 0;
  DWORD dwOffset_bb = 0;

  //at first we copy rect from back buffer to effects buffer
  RECT rcEB;
  rcEB.left = x;
  rcEB.top = y;
  rcEB.right = rcEB.left + dwWidth;
  rcEB.bottom = rcEB.top + dwHeight;
  m_pddsEffectsBuffer->BltFast(x, y, m_pddsBackBuffer, &rcEB, DDBLTFAST_NOCOLORKEY);

  //now work with effects buffer
  DDSURFACEDESC2 ddsd;
  DDSURFACEDESC2 ddsd_bb;
  LPDIRECTDRAWSURFACE7 pdds = pSurface->GetDDrawSurface();
  HRESULT hr;
	
  memset(&ddsd, 0, sizeof(ddsd));
  ddsd.dwSize = sizeof(DDSURFACEDESC2);

  memset(&ddsd_bb, 0, sizeof(DDSURFACEDESC2));
  ddsd_bb.dwSize = sizeof(DDSURFACEDESC2);
	
  //lock surface
  if (FAILED(hr = pdds->Lock(NULL, &ddsd, DDLOCK_WAIT, NULL)))
    return hr;

  if (FAILED(hr = m_pddsEffectsBuffer->Lock(NULL, &ddsd_bb, DDLOCK_WAIT, NULL)))
    return hr;
			
  dwOffset_bb = y*ddsd_bb.lPitch;
  dwOffset = prc->top*ddsd.lPitch;
  dwOffset_bb += x*2;
  dwOffset += prc->left*2;
	
  WORD *pwDest = (WORD*)((BYTE*)ddsd_bb.lpSurface + dwOffset_bb);
  WORD *pwSource = (WORD*)((BYTE*)ddsd.lpSurface + dwOffset);
	
  DWORD dwDiff_bb = (ddsd_bb.lPitch -  dwWidth*2)/2;
  DWORD dwDiff = (ddsd.lPitch - dwWidth*2)/2;
	
  DWORD dwD[3];
  DWORD dwS[3];
  
  //check if surface has transparent areas
  if (pSurface->IsColorKeyed()) {
    //get transparent value
    WORD *pwTrr = (WORD*)((BYTE*)ddsd.lpSurface);

    //draw with transparent areas
    for (DWORD iY=0; iY<dwHeight; iY++) {
      for (DWORD iX=0; iX<dwWidth; iX++) {
        //raster only if no transparent pixel
	if (*pwSource != *pwTrr) {
          for (int i=0; i<3; i++) {
	    dwS[i] = (DWORD)(m_dwBitMask[i] & *pwSource) >> m_dwShift[i];
	    dwD[i] = (DWORD)(m_dwBitMask[i] & *pwDest) >> m_dwShift[i];
	    dwD[i] = m_nAlpha[dwS[i]][dwD[i]][nAlpha];
	    dwD[i] <<= m_dwShift[i];
	  }				
	  *pwDest = (WORD)(dwD[0] | dwD[1] | dwD[2]);
        }
	//go next pixel
	pwDest++;
	pwSource++;
      }
      //aligment
      pwDest += dwDiff_bb;
      pwSource += dwDiff;
    }
  } else {
    //draw without transparent areas
    for (DWORD iY=0; iY<dwHeight; iY++) {
      for (DWORD iX=0; iX<dwWidth; iX++) {
	for (int i=0; i<3; i++) {
	  dwS[i] = (DWORD)(m_dwBitMask[i] & *pwSource) >> m_dwShift[i];
	  dwD[i] = (DWORD)(m_dwBitMask[i] & *pwDest) >> m_dwShift[i];
	  dwD[i] = m_nAlpha[dwS[i]][dwD[i]][nAlpha];
	  dwD[i] <<= m_dwShift[i];
	}				
	*pwDest = (WORD)(dwD[0] | dwD[1] | dwD[2]);
        pwDest++;
	pwSource++;
      }
      pwDest += dwDiff_bb;
      pwSource += dwDiff;
    }
  }
	
  //unlock surfaces
  m_pddsEffectsBuffer->Unlock(NULL);
  pdds->Unlock(NULL);
	
  //copy effects to back
  m_pddsBackBuffer->BltFast(x, y, m_pddsEffectsBuffer, &rcEB, DDBLTFAST_NOCOLORKEY);
  return S_OK;
}


//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
HRESULT CDisplay::BltAlpha32(DWORD x, DWORD y, CSurface* pSurface, BYTE nAlpha, RECT* prc)
{
  //define rect dimension
  DWORD dwWidth = prc->right - prc->left;
  DWORD dwHeight= prc->bottom - prc->top;

  DWORD dwOffset = 0;
  DWORD dwOffset_bb = 0;

  //at first we copy rect from back buffer to effects buffer
  RECT rcEB;
  rcEB.left = x;
  rcEB.top = y;
  rcEB.right = rcEB.left + dwWidth;
  rcEB.bottom = rcEB.top + dwHeight;
  m_pddsEffectsBuffer->BltFast(x, y, m_pddsBackBuffer, &rcEB, DDBLTFAST_NOCOLORKEY);

  //now work with effects buffer
  DDSURFACEDESC2 ddsd;
  DDSURFACEDESC2 ddsd_bb;
  LPDIRECTDRAWSURFACE7 pdds = pSurface->GetDDrawSurface();
  HRESULT hr;
	
  memset(&ddsd, 0, sizeof(ddsd));
  ddsd.dwSize = sizeof(DDSURFACEDESC2);

  memset(&ddsd_bb, 0, sizeof(DDSURFACEDESC2));
  ddsd_bb.dwSize = sizeof(DDSURFACEDESC2);
	
  //lock surface
  if (FAILED(hr = pdds->Lock(NULL, &ddsd, DDLOCK_WAIT, NULL)))
    return hr;

  if (FAILED(hr = m_pddsEffectsBuffer->Lock(NULL, &ddsd_bb, DDLOCK_WAIT, NULL)))
    return hr;
			
  dwOffset_bb = y*ddsd_bb.lPitch;
  dwOffset = prc->top*ddsd.lPitch;
  dwOffset_bb += x*4;
  dwOffset += prc->left*4;
	
  DWORD *pdwDest = (DWORD*)((BYTE*)ddsd_bb.lpSurface + dwOffset_bb);
  DWORD *pdwSource = (DWORD*)((BYTE*)ddsd.lpSurface + dwOffset);
	
  DWORD dwDiff_bb = (ddsd_bb.lPitch -  dwWidth*4)/4;
  DWORD dwDiff = (ddsd.lPitch - dwWidth*4)/4;
	
  DWORD dwD[3];
  DWORD dwS[3];
  
  //check if surface has transparent areas
  if (pSurface->IsColorKeyed()) {
    //get transparent value
    DWORD *pdwTrr = (DWORD*)((BYTE*)ddsd.lpSurface);

    //draw with transparent areas
    for (DWORD iY=0; iY<dwHeight; iY++) {
      for (DWORD iX=0; iX<dwWidth; iX++) {
        //raster only if no transparent pixel
	if (*pdwSource != *pdwTrr) {
	  for (int i=0; i<3; i++) {
	    dwS[i] = (DWORD)(m_dwBitMask[i] & *pdwSource) >> m_dwShift[i];
	    dwD[i] = (DWORD)(m_dwBitMask[i] & *pdwDest) >> m_dwShift[i];
	    dwD[i] = m_nAlpha[dwS[i]][dwD[i]][nAlpha];
	    dwD[i] <<= m_dwShift[i];
	  }				
	  *pdwDest = (DWORD)(dwD[0] | dwD[1] | dwD[2]);
	}
	//go next pixel
	pdwDest++;
	pdwSource++;
      }
      //aligment
      pdwDest += dwDiff_bb;
      pdwSource += dwDiff;
    }
  } else {
    //draw without transparent areas
    for (DWORD iY=0; iY<dwHeight; iY++) {
      for (DWORD iX=0; iX<dwWidth; iX++) {
	for (DWORD i=0; i<3; i++) {
	  dwS[i] = (DWORD)(m_dwBitMask[i] & *pdwSource) >> m_dwShift[i];
	  dwD[i] = (DWORD)(m_dwBitMask[i] & *pdwDest) >> m_dwShift[i];
	  dwD[i] = m_nAlpha[dwS[i]][dwD[i]][nAlpha];
	  dwD[i] <<= m_dwShift[i];
	}				
	*pdwDest = (DWORD)(dwD[0] | dwD[1] | dwD[2]);
	pdwDest++;
	pdwSource++;
      }
      pdwDest += dwDiff_bb;
      pdwSource += dwDiff;
    }
  }
	
  //unlock surfaces
  m_pddsEffectsBuffer->Unlock(NULL);
  pdds->Unlock(NULL);
	
  //copy effects to back
  m_pddsBackBuffer->BltFast(x, y, m_pddsEffectsBuffer, &rcEB, DDBLTFAST_NOCOLORKEY);
  return S_OK;
}


//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
HRESULT CDisplay::BltSoft16(DWORD x, DWORD y, LPDIRECTDRAWSURFACE7 pdds, RECT* prc, DWORD dwFlags)
{
  DDSURFACEDESC2 ddsd;
  DDSURFACEDESC2 ddsd_bb;
  HRESULT hr;
	
  memset(&ddsd, 0, sizeof(ddsd));
  ddsd.dwSize = sizeof(DDSURFACEDESC2);

  memset(&ddsd_bb, 0, sizeof(DDSURFACEDESC2));
  ddsd_bb.dwSize = sizeof(DDSURFACEDESC2);
	
  //define rect dimension
  DWORD dwWidth = prc->right - prc->left;
  DWORD dwHeight= prc->bottom - prc->top;

  DWORD dwOffset = 0;
  DWORD dwOffset_bb = 0;

  //lock surface
  if (FAILED(hr = pdds->Lock(NULL, &ddsd, DDLOCK_WAIT, NULL)))
    return hr;

  if (FAILED(hr = m_pddsBackBuffer->Lock(NULL, &ddsd_bb, DDLOCK_WAIT, NULL)))
    return hr;
			
  dwOffset_bb = y*ddsd_bb.lPitch;
  dwOffset = prc->top*ddsd.lPitch;
  dwOffset_bb += x*2;
  dwOffset += prc->left*2;
	
  WORD *psDest = (WORD*)((BYTE*)ddsd_bb.lpSurface + dwOffset_bb);
  WORD *psSource = (WORD*)((BYTE*)ddsd.lpSurface + dwOffset);

  WORD *psTrr = (WORD*)((BYTE*)ddsd.lpSurface);
  
  DWORD dwDiff_bb = ddsd_bb.lPitch -  dwWidth*2;
  DWORD dwDiff = ddsd.lPitch - dwWidth*2;
	
  if (dwFlags == 0) {
    __asm {
	mov ecx, dwHeight
	mov edi, dword ptr psDest
	mov esi, dword ptr psSource
	loop_height: push ecx
	mov ecx, dwWidth
	rep movsw
	pop ecx
	add edi, dwDiff_bb
	add esi, dwDiff
	loop loop_height
    }
  } else {
    __asm {
        mov ecx, dwHeight
	mov edi, dword ptr psDest
	mov esi, dword ptr psSource
	loop_height_tr:	push ecx
	mov ecx, dwWidth
	loop_width_tr:	push edi
	push esi
	mov edi, dword ptr psTrr
	cmpsw
	pop esi
	pop edi
	jz skip_pixel
	movsw
	jmp next_pixel
	skip_pixel: add esi,2
	add edi,2
	next_pixel: loop loop_width_tr
	pop ecx
	add edi, dwDiff_bb
	add esi, dwDiff
	loop loop_height_tr
    }
  }
  
  //unlock surface
  m_pddsBackBuffer->Unlock(NULL);
  pdds->Unlock(NULL);

  return S_OK;
}


//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
HRESULT CDisplay::BltSoft32(DWORD x, DWORD y, LPDIRECTDRAWSURFACE7 pdds, RECT* prc, DWORD dwFlags)
{
  DDSURFACEDESC2 ddsd;
  DDSURFACEDESC2 ddsd_bb;
  HRESULT hr;
	
  memset(&ddsd, 0, sizeof(ddsd));
  ddsd.dwSize = sizeof(DDSURFACEDESC2);

  memset(&ddsd_bb, 0, sizeof(DDSURFACEDESC2));
  ddsd_bb.dwSize = sizeof(DDSURFACEDESC2);
	
  //define rect dimension
  DWORD dwWidth = prc->right - prc->left;
  DWORD dwHeight= prc->bottom - prc->top;

  DWORD dwOffset = 0;
  DWORD dwOffset_bb = 0;

  //lock surface
  if (FAILED(hr = pdds->Lock(NULL, &ddsd, DDLOCK_WAIT, NULL)))
    return hr;

  if (FAILED(hr = m_pddsBackBuffer->Lock(NULL, &ddsd_bb, DDLOCK_WAIT, NULL)))
    return hr;
			
  dwOffset_bb = y*ddsd_bb.lPitch;
  dwOffset = prc->top*ddsd.lPitch;
  dwOffset_bb += x*4;
  dwOffset += prc->left*4;
	
  DWORD *psDest = (DWORD*)((BYTE*)ddsd_bb.lpSurface + dwOffset_bb);
  DWORD *psSource = (DWORD*)((BYTE*)ddsd.lpSurface + dwOffset);

  DWORD *psTrr = (DWORD*)((BYTE*)ddsd.lpSurface);
  
  DWORD dwDiff_bb = ddsd_bb.lPitch -  dwWidth*4;
  DWORD dwDiff = ddsd.lPitch - dwWidth*4;
	
  if (dwFlags == 0) {
    __asm {
	mov ecx, dwHeight
	mov edi, dword ptr psDest
	mov esi, dword ptr psSource
	loop_height: push ecx
	mov ecx, dwWidth
	rep movsd
	pop ecx
	add edi, dwDiff_bb
	add esi, dwDiff
	loop loop_height
    }
  } else {
    __asm {
        mov ecx, dwHeight
	mov edi, dword ptr psDest
	mov esi, dword ptr psSource
	loop_height_tr:	push ecx
	mov ecx, dwWidth
	loop_width_tr:	push edi
	push esi
	mov edi, dword ptr psTrr
	cmpsd
	pop esi
	pop edi
	jz skip_pixel
	movsd
	jmp next_pixel
	skip_pixel: add esi, 4
	add edi, 4
	next_pixel: loop loop_width_tr
	pop ecx
	add edi, dwDiff_bb
	add esi, dwDiff
	loop loop_height_tr
    }
  }
  
  //unlock surface
  m_pddsBackBuffer->Unlock(NULL);
  pdds->Unlock(NULL);

  return S_OK;
}


//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
HRESULT CDisplay::BltStretch(RECT* prcDest, CSurface* pDestSurface, RECT* prcSourceSurface, CSurface* pSourceSurface)
{
  if (pDestSurface == NULL || pSourceSurface == NULL)
    return E_INVALIDARG;

  if (pSourceSurface->IsColorKeyed()) {
     return pDestSurface->GetDDrawSurface()->Blt(prcDest, pSourceSurface->GetDDrawSurface(), prcSourceSurface, DDBLT_KEYSRC, NULL);
  } else {
    return pDestSurface->GetDDrawSurface()->Blt(prcDest, pSourceSurface->GetDDrawSurface(), prcSourceSurface, 0, NULL);
  }
}


//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
HRESULT CDisplay::BltRotate(RECT* prcDest, CSurface* pDestSurface, CSurface* pSourceSurface, POINT ptO, int nAngle)
{
  if (GetScreenDepth() == 16)
    return BltRotate16(prcDest, pDestSurface, pSourceSurface, ptO, nAngle);
  else
    return BltRotate32(prcDest, pDestSurface, pSourceSurface, ptO, nAngle);
}


//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
HRESULT CDisplay::BltRotate16(RECT* prcDest, CSurface* pDestSurface, CSurface* pSourceSurface, POINT ptO, int nAngle)
{
  if (pDestSurface == NULL || pSourceSurface == NULL)
    return E_INVALIDARG;

  // define rect dimension
  DWORD dwWidth = prcDest->right - prcDest->left;
  DWORD dwHeight= prcDest->bottom - prcDest->top;

  DWORD dwOffset_s = 0;
  DWORD dwOffset_d = 0;

  // now work with buffer
  DDSURFACEDESC2 ddsd_s;
  DDSURFACEDESC2 ddsd_d;
  LPDIRECTDRAWSURFACE7 pdds_d = pDestSurface->GetDDrawSurface();
  LPDIRECTDRAWSURFACE7 pdds_s = pSourceSurface->GetDDrawSurface();

  HRESULT hr;
	
  memset(&ddsd_s, 0, sizeof(DDSURFACEDESC2));
  ddsd_s.dwSize = sizeof(DDSURFACEDESC2);

  memset(&ddsd_d, 0, sizeof(DDSURFACEDESC2));
  ddsd_d.dwSize = sizeof(DDSURFACEDESC2);
	
  // lock surfaces
  if (FAILED(hr = pdds_d->Lock(NULL, &ddsd_d, DDLOCK_WAIT, NULL)))
    return hr;

  if (FAILED(hr = pdds_s->Lock(NULL, &ddsd_s, DDLOCK_WAIT, NULL)))
    return hr;
			
  dwOffset_s = prcDest->top*ddsd_s.lPitch;
  dwOffset_s += prcDest->left*2;
  dwOffset_d = prcDest->top*ddsd_d.lPitch;
  dwOffset_d += prcDest->left*2;

  WORD dwNullValue = 0;
  if (pSourceSurface->IsColorKeyed()) {
    dwNullValue = *((WORD*)ddsd_s.lpSurface);
  }
	
  WORD* pwDest = (WORD*)((BYTE*)ddsd_d.lpSurface + dwOffset_d);
  WORD* pwSource = (WORD*)((BYTE*)ddsd_s.lpSurface + dwOffset_s);
	
  DWORD dwDiff_d = (ddsd_d.lPitch -  dwWidth*2)/2;
  DWORD dwDiff_s = (ddsd_s.lPitch - dwWidth*2)/2;

  double sinAngle = m_nSin[nAngle%360];
  double cosAngle = m_nCos[nAngle%360];

  for (DWORD iY = 0; iY < dwHeight; iY++) {
    DWORD iYPrime = iY - ptO.y;
    for (DWORD iX = 0; iX < dwWidth; iX++) {
      DWORD iXPrime = ptO.x - iX;
      DWORD iXOriginal = ptO.x + (iXPrime*cosAngle + iYPrime*sinAngle);
      DWORD iYOriginal = ptO.y + (iXPrime*sinAngle - iYPrime*cosAngle);
      if (iXOriginal >=0 && iXOriginal < dwWidth &&
          iYOriginal >=0 && iYOriginal < dwHeight) {
        *pwDest = *(pwSource + iYOriginal*(dwWidth + dwDiff_s) + iXOriginal);
      } else {
        *pwDest = dwNullValue;
      }
      pwDest++;
    }
    pwDest += dwDiff_d;
  }
  if (pSourceSurface->IsColorKeyed()) {
    *((WORD*)ddsd_d.lpSurface) = dwNullValue;
  }
  
  // unlock surfaces
  pdds_d->Unlock(NULL);
  pdds_s->Unlock(NULL);

  return S_OK;
}


//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
HRESULT CDisplay::BltRotate32(RECT* prcDest, CSurface* pDestSurface, CSurface* pSourceSurface, POINT ptO, int nAngle)
{
  if (pDestSurface == NULL || pSourceSurface == NULL)
    return E_INVALIDARG;

  // define rect dimension
  DWORD dwWidth = prcDest->right - prcDest->left;
  DWORD dwHeight= prcDest->bottom - prcDest->top;

  DWORD dwOffset_s = 0;
  DWORD dwOffset_d = 0;

  // now work with buffer
  DDSURFACEDESC2 ddsd_s;
  DDSURFACEDESC2 ddsd_d;
  LPDIRECTDRAWSURFACE7 pdds_d = pDestSurface->GetDDrawSurface();
  LPDIRECTDRAWSURFACE7 pdds_s = pSourceSurface->GetDDrawSurface();

  HRESULT hr;
	
  memset(&ddsd_s, 0, sizeof(DDSURFACEDESC2));
  ddsd_s.dwSize = sizeof(DDSURFACEDESC2);

  memset(&ddsd_d, 0, sizeof(DDSURFACEDESC2));
  ddsd_d.dwSize = sizeof(DDSURFACEDESC2);
	
  // lock surfaces
  if (FAILED(hr = pdds_d->Lock(NULL, &ddsd_d, DDLOCK_WAIT, NULL)))
    return hr;

  if (FAILED(hr = pdds_s->Lock(NULL, &ddsd_s, DDLOCK_WAIT, NULL)))
    return hr;
			
  dwOffset_s = prcDest->top*ddsd_s.lPitch;
  dwOffset_s += prcDest->left*4;
  dwOffset_d = prcDest->top*ddsd_d.lPitch;
  dwOffset_d += prcDest->left*4;

  DWORD dwNullValue = 0;
  if (pSourceSurface->IsColorKeyed()) {
    dwNullValue = *((DWORD*)ddsd_s.lpSurface);
  }
	
  DWORD* pwDest = (DWORD*)((BYTE*)ddsd_d.lpSurface + dwOffset_d);
  DWORD* pwSource = (DWORD*)((BYTE*)ddsd_s.lpSurface + dwOffset_s);
	
  DWORD dwDiff_d = (ddsd_d.lPitch -  dwWidth*4)/4;
  DWORD dwDiff_s = (ddsd_s.lPitch - dwWidth*4)/4;

  double sinAngle = m_nSin[nAngle%360];
  double cosAngle = m_nCos[nAngle%360];

  for (DWORD iY = 0; iY < dwHeight; iY++) {
    DWORD iYPrime = iY - ptO.y;
    for (DWORD iX = 0; iX < dwWidth; iX++) {
      DWORD iXPrime = ptO.x - iX;
      DWORD iXOriginal = ptO.x + (iXPrime*cosAngle + iYPrime*sinAngle);
      DWORD iYOriginal = ptO.y + (iXPrime*sinAngle - iYPrime*cosAngle);
      if (iXOriginal >=0 && iXOriginal < dwWidth &&
          iYOriginal >=0 && iYOriginal < dwHeight) {
        *pwDest = *(pwSource + iYOriginal*(dwWidth + dwDiff_s) + iXOriginal);
      } else {
        *pwDest = dwNullValue;
      }
      pwDest++;
    }
    pwDest += dwDiff_d;
  }
  if (pSourceSurface->IsColorKeyed()) {
    *((DWORD*)ddsd_d.lpSurface) = dwNullValue;
  }
  
  // unlock surfaces
  pdds_d->Unlock(NULL);
  pdds_s->Unlock(NULL);

  return S_OK;
}


//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
HRESULT CDisplay::BltHard(DWORD x, DWORD y, LPDIRECTDRAWSURFACE7 pdds, RECT* prc, DWORD dwFlags)
{
  RECT rcDest;
  SetRect(&rcDest, x, y, x + prc->right - prc->left, y + prc->bottom - prc->top);

  return m_pddsBackBuffer->Blt(&rcDest, pdds, prc, dwFlags, NULL);
}


//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
HRESULT CDisplay::Blt(DWORD x, DWORD y, CSurface* pSurface, RECT* prc)
{
  if (pSurface == NULL)
    return E_INVALIDARG;

  if (!m_bUseHardwareBlt) {
    if (GetScreenDepth() == 16) {
      if (pSurface->IsColorKeyed()) {
        return BltSoft16(x, y, pSurface->GetDDrawSurface(), prc, DDBLTFAST_SRCCOLORKEY);
      } else {
        return BltSoft16(x, y, pSurface->GetDDrawSurface(), prc, 0);
      }
    } else {
      if (pSurface->IsColorKeyed()) {
        return BltSoft32(x, y, pSurface->GetDDrawSurface(), prc, DDBLTFAST_SRCCOLORKEY);
      } else {
        return BltSoft32(x, y, pSurface->GetDDrawSurface(), prc, 0);
      }
    }
  } else {
    if (pSurface->IsColorKeyed()) {
      return BltHard(x, y, pSurface->GetDDrawSurface(), prc, DDBLT_KEYSRC);
    } else {
      return BltHard(x, y, pSurface->GetDDrawSurface(), prc, 0);
    }
  }
}


//-----------------------------------------------------------------------------
// Name: DestroyObjects()
// Desc:
//-----------------------------------------------------------------------------
DWORD CDisplay::GetScreenDepth() 
{
  if (m_pDD == NULL)
    return 0;

  DDSURFACEDESC2 ddsd;

  memset(&ddsd, 0, sizeof(DDSURFACEDESC2));
  ddsd.dwSize = sizeof(DDSURFACEDESC2);

  if (FAILED(m_pDD->GetDisplayMode(&ddsd)))
    return 0;

  return ddsd.ddpfPixelFormat.dwRGBBitCount;
}


//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
HRESULT CDisplay::Clear(DWORD dwColor)
{
    if(m_pddsBackBuffer == NULL)
        return E_POINTER;

    // Erase the background
    DDBLTFX ddbltfx;
    memset(&ddbltfx, 0, sizeof(DDBLTFX));
    ddbltfx.dwSize      = sizeof(DDBLTFX);
    ddbltfx.dwFillColor = dwColor;

    return m_pddsBackBuffer->Blt(NULL, NULL, NULL, DDBLT_COLORFILL, &ddbltfx);
}


//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
HRESULT CDisplay::ClearEx(DWORD dwColor, RECT* prc)
{
    if(m_pddsBackBuffer == NULL)
        return E_POINTER;

    // Erase the background
    DDBLTFX ddbltfx;
    memset(&ddbltfx, 0, sizeof(DDBLTFX));
    ddbltfx.dwSize      = sizeof(DDBLTFX);
    ddbltfx.dwFillColor = dwColor;

    return m_pddsBackBuffer->Blt(prc, NULL, NULL, DDBLT_COLORFILL, &ddbltfx);
}


//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
HRESULT CDisplay::UpdateBounds()
{
    if (m_bWindowed)
    {
        GetClientRect(m_hWnd, &m_rcWindow);
        ClientToScreen(m_hWnd, (POINT*)&m_rcWindow);
        ClientToScreen(m_hWnd, (POINT*)&m_rcWindow+1);
    }
    else
    {
        SetRect(&m_rcWindow, 0, 0, GetSystemMetrics(SM_CXSCREEN),
                 GetSystemMetrics(SM_CYSCREEN));
    }
    return S_OK;
}


//-----------------------------------------------------------------------------
// Name: CDisplay::InitClipper
// Desc: 
//-----------------------------------------------------------------------------
HRESULT CDisplay::InitClipper()
{
    LPDIRECTDRAWCLIPPER pClipper;
    HRESULT hr;

    // Create a clipper when using GDI to draw on the primary surface 
    if (FAILED(hr = m_pDD->CreateClipper(0, &pClipper, NULL)))
        return hr;

    pClipper->SetHWnd(0, m_hWnd);

    if (FAILED(hr = m_pddsFrontBuffer->SetClipper( pClipper)))
        return hr;

    // We can release the clipper now since g_pDDSPrimary 
    // now maintains a ref count on the clipper
    SAFE_RELEASE(pClipper);

    return S_OK;
}
