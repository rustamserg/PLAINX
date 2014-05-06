#include "plainx.h"
#include "video.h"


CVideoManager::CVideoManager()
{
  m_pDisplay = NULL;
  m_pParent = NULL;

  m_nSWidth = 640;
  m_nSHeight = 480;
  m_nSBPP = 16;

  m_bNeedReload = FALSE;
  m_bNeedRecreate = FALSE;
}


CVideoManager::~CVideoManager()
{
}


//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
BOOL CVideoManager::Init(CEngine* pEngine)
{
  m_pDisplay = new CDisplay();
  HRESULT hr = m_pDisplay->CreateDirectDraw();
  if (FAILED(hr))
    return FALSE;

  m_pParent = pEngine;

  //get use video memory
  if (m_pParent) {
    if (m_pParent->GetSafeMode()) {
      SetUseVideoMemory(FALSE);
      SetUseHardwareBlt(FALSE);
    } else {
      int nV = 0;
	  m_pParent->GetVars()->GetInt(L_GROUP, L_DD_DDRAW, L_DD_USE_HARDBLT, 0, &nV);
      SetUseHardwareBlt((BOOL)nV);
	  if (nV) {
		  m_pParent->GetVars()->GetInt(L_GROUP, L_DD_DDRAW, L_DD_USE_VIDEOMEM, 0, &nV);
	  }
      SetUseVideoMemory((BOOL)nV);
    }
  }
  return TRUE;
}

//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
void CVideoManager::SetUseVideoMemory(BOOL bv)
{
  if (m_pDisplay) {
    BOOL bold = m_pDisplay->GetUseVideoMemory();
    m_pDisplay->SetUseVideoMemory(bv);
    if (m_pParent && bold != bv) {
      if (!m_pParent->GetSafeMode()) {
        m_pParent->GetVars()->SetInt(L_GROUP, L_DD_DDRAW, L_DD_USE_VIDEOMEM, 0, (int)bv);
      }
      m_pParent->RecreateAll();
    }
  }
}

//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
void CVideoManager::SetUseHardwareBlt(BOOL bv)
{
  if (m_pDisplay) {
    m_pDisplay->SetUseHardwareBlt(bv);
    if (!bv) SetUseVideoMemory(FALSE);
    if (m_pParent) {
      if (!m_pParent->GetSafeMode()) {
        m_pParent->GetVars()->SetInt(L_GROUP, L_DD_DDRAW, L_DD_USE_HARDBLT, 0, (int)bv);
      }
    }
  }
}

//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
BOOL CVideoManager::GetUseVideoMemory()
{
  if (m_pDisplay) {
    return m_pDisplay->GetUseVideoMemory();
  }
  return FALSE;
}

//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
BOOL CVideoManager::GetUseHardwareBlt()
{
  if (m_pDisplay) {
    m_pDisplay->GetUseHardwareBlt();
  }
  return FALSE;
}


//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
BOOL CVideoManager::IsCanBeWindowed(int nSBPP)
{
  //prevent use windows mode for 16-bit application when desktop is not 16 bit
  if ((nSBPP == 16) && 
      (m_pDisplay->GetScreenDepth() != 16) && 
      (m_pDisplay->GetScreenDepth() != 32))
    return FALSE;

  return TRUE;
}


//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
int CVideoManager::GetScreenDepth()
{ 
    return m_pDisplay->GetScreenDepth(); 
}


//-----------------------------------------------------------------------------
// Name: vid_InitDirectDrawMode()
// Desc: Called when the user wants to toggle between full-screen and windowed 
//       to create all the needed DDraw surfaces and set the coop level
//-----------------------------------------------------------------------------
BOOL CVideoManager::SetMode(HWND hWnd, 
                            int* pnSWidth, 
                            int* pnSHeight,
                            int* pnSBPP, 
                            BOOL bWindowed)
{
  if (pnSWidth)
    m_nSWidth = *pnSWidth;
  if (pnSHeight)
    m_nSHeight = *pnSHeight;
  if (pnSBPP)
    m_nSBPP = *pnSBPP;   
	
  //check for screen depth
  if ((m_nSBPP != 16) && (m_nSBPP != 32))
    return FALSE;
	
  //go on!
  if (bWindowed)  {
    if (FAILED(m_pDisplay->CreateWindowedDisplay(hWnd, m_nSWidth, m_nSHeight)))
      return FALSE;

    // Add the system menu to the window's style
    DWORD dwStyle = GetWindowLong(hWnd, GWL_STYLE);
    dwStyle |= WS_SYSMENU;
    SetWindowLong(hWnd, GWL_STYLE, dwStyle);
  } else {
    if (FAILED(m_pDisplay->CreateFullScreenDisplay(hWnd, m_nSWidth, m_nSHeight, m_nSBPP))) {
      MessageBox(NULL, 
                 TEXT("This display card does not support current mode."),
                 TEXT("Client"), 
                 MB_ICONERROR | MB_OK);
      return FALSE;
    }

    // Remove the system menu from the window's style
    DWORD dwStyle = GetWindowLong(hWnd, GWL_STYLE);
    dwStyle &= ~WS_SYSMENU;
    SetWindowLong(hWnd, GWL_STYLE, dwStyle);       
  }
  return TRUE;
}




//-----------------------------------------------------------------------------
// Name: vid_FreeDirectDraw()
// Desc: Release all the DirectDraw objects
//-----------------------------------------------------------------------------
void CVideoManager::Free()
{
  SAFE_DELETE(m_pDisplay);
}


//------------------------------------------------------------------------------
// Name: vid_ToggleSreenMode
// Desc: Toggle the fullscreen/window mode
//------------------------------------------------------------------------------
void CVideoManager::ToggleScreenMode(HWND hWnd, BOOL bWindowed, RECT* prcWindow)
{
  if (bWindowed)
    GetWindowRect(hWnd, prcWindow);

  if (FAILED(SetMode(hWnd, NULL, NULL, NULL, !bWindowed)))
    SAFE_DELETE(m_pDisplay);

  if (!bWindowed) 
    ::SetWindowPos(hWnd, 
                   HWND_TOP, 
                   prcWindow->left, 
                   prcWindow->top,
                   prcWindow->right - prcWindow->left, 
                   prcWindow->bottom - prcWindow->top,
                   SWP_SHOWWINDOW);
}


//------------------------------------------------------------------------------
// Name: vid_RetrieveWindowPosition
// Desc: Update window position 
//------------------------------------------------------------------------------
HRESULT CVideoManager::UpdateScreen()
{
  HRESULT hr;

  // Flip or blt the back buffer onto the primary buffer
  hr = m_pDisplay->Present();
  if (FAILED(hr)) {
    if (hr != DDERR_SURFACELOST) 
      return hr;

    // The surfaces were lost so restore them 
    m_pParent->ReloadAll();
  }
  return S_OK;
}


//------------------------------------------------------------------------------
// Name: vid_RetrieveWindowPosition
// Desc: Update window position 
//------------------------------------------------------------------------------
HRESULT CVideoManager::TestCooperativeLevel()
{
  HRESULT hr;
  
  hr = m_pDisplay->GetDirectDraw()->TestCooperativeLevel();
  if (FAILED(hr))
  {
    switch (hr)
    {
      case DDERR_EXCLUSIVEMODEALREADYSET:
      case DDERR_NOEXCLUSIVEMODE:
        // Do nothing because some other app has exclusive mode
        Sleep(10);
        m_bNeedReload = TRUE;
        break;

      case DDERR_WRONGMODE:
        // The display mode changed on us. Update the
        // DirectDraw surfaces accordingly
        if (FAILED(hr = m_pDisplay->CreateDirectDraw()))
          return hr;

        SetMode(m_pParent->GetHWnd(), NULL, NULL, NULL, m_pParent->GetWindowed());
        m_bNeedRecreate = TRUE;
        break;
    }
    return hr;
  }

  // test coop was good but for last state we need reload
  if (m_bNeedReload) {
    m_pParent->ReloadAll();
    m_bNeedReload = FALSE;
  }

  // display mode was changed we need recreate
  if (m_bNeedRecreate) {
    m_pParent->RecreateAll();
    m_bNeedRecreate = FALSE;
  }
  return hr; 
}


//------------------------------------------------------------------------------
// Name: vid_RetrieveWindowPosition
// Desc: Update window position 
//------------------------------------------------------------------------------
void CVideoManager::RetrieveWindowPosition()
{
  if (m_pDisplay) 
    m_pDisplay->UpdateBounds();
}

//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
int CVideoManager::GetTotalVideoMemory()
{
  DDSCAPS2      ddsCaps2; 
  DWORD         dwTotal; 
  DWORD         dwFree;
  HRESULT       hr; 
 
  // Initialize the structure.
  memset(&ddsCaps2, 0, sizeof(ddsCaps2));
 
  ddsCaps2.dwCaps = DDSCAPS_OFFSCREENPLAIN; 
  hr = m_pDisplay->GetDirectDraw()->GetAvailableVidMem(&ddsCaps2, &dwTotal, &dwFree); 
  if (FAILED(hr))
    return 0;
  
  return (dwTotal/1024)/1024;
}

//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
int CVideoManager::GetFreeVideoMemory()
{
  DDSCAPS2      ddsCaps2; 
  DWORD         dwTotal; 
  DWORD         dwFree;
  HRESULT       hr; 
 
  // Initialize the structure.
  memset(&ddsCaps2, 0, sizeof(ddsCaps2));
 
  ddsCaps2.dwCaps = DDSCAPS_OFFSCREENPLAIN; 
  hr = m_pDisplay->GetDirectDraw()->GetAvailableVidMem(&ddsCaps2, &dwTotal, &dwFree); 
  if (FAILED(hr))
    return 0;

  return (dwFree/1024)/1024;
}
