#include "plainx.h"
#include "surface.h"


CSurface::CSurface()
{
  m_pdds = NULL;
  m_bColorKeyed = NULL;
}


CSurface::~CSurface()
{
  SAFE_RELEASE(m_pdds);
}


//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
HRESULT CSurface::Create(LPDIRECTDRAWSURFACE7 pdds)
{
  m_pdds = pdds;

  if (m_pdds) {
    m_pdds->AddRef();

    // Get the DDSURFACEDESC structure for this surface
    m_ddsd.dwSize = sizeof(m_ddsd);
    m_pdds->GetSurfaceDesc( &m_ddsd );
  }
  return S_OK;
}


//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
HRESULT CSurface::Create(LPDIRECTDRAW7 pDD, DDSURFACEDESC2* pddsd)
{
  HRESULT hr;

  // Create the DDraw surface
  if (FAILED(hr = pDD->CreateSurface(pddsd, &m_pdds, NULL)))
    return hr;

  // Prepare the DDSURFACEDESC structure
  m_ddsd.dwSize = sizeof(m_ddsd);

  // Get the DDSURFACEDESC structure for this surface
  m_pdds->GetSurfaceDesc(&m_ddsd);

  return S_OK;
}


//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
HRESULT CSurface::Destroy()
{
  SAFE_RELEASE(m_pdds);
  return S_OK;
}


//-----------------------------------------------------------------------------
// Name: CSurface::DrawBitmap()
// Desc: Draws a bitmap over an entire DirectDrawSurface, stretching the 
//       bitmap if nessasary
//-----------------------------------------------------------------------------
HRESULT CSurface::DrawBitmap(HBITMAP hBMP, 
                             DWORD dwBMPOriginX, DWORD dwBMPOriginY, 
                             DWORD dwBMPWidth, DWORD dwBMPHeight)
{
  HDC            hDCImage;
  HDC            hDC;
  BITMAP         bmp;
  DDSURFACEDESC2 ddsd;
  HRESULT        hr;

  if (hBMP == NULL || m_pdds == NULL)
    return E_INVALIDARG;

  // Make sure this surface is restored.
  if (FAILED(hr = m_pdds->Restore()))
    return hr;

  // Get the surface.description
  ddsd.dwSize  = sizeof(ddsd);
  m_pdds->GetSurfaceDesc(&ddsd);

  if (ddsd.ddpfPixelFormat.dwFlags == DDPF_FOURCC)
    return E_NOTIMPL;

  // Select bitmap into a memoryDC so we can use it.
  hDCImage = CreateCompatibleDC(NULL);
  if (hDCImage == NULL)
    return E_FAIL;

  SelectObject(hDCImage, hBMP);

  // Get size of the bitmap
  GetObject(hBMP, sizeof(bmp), &bmp);

  // Use the passed size, unless zero
  dwBMPWidth  = (dwBMPWidth  == 0) ? bmp.bmWidth  : dwBMPWidth;     
  dwBMPHeight = (dwBMPHeight == 0) ? bmp.bmHeight : dwBMPHeight;

  // Stretch the bitmap to cover this surface
  if (FAILED(hr = m_pdds->GetDC(&hDC)))
    return hr;

  StretchBlt(hDC, 0, 0, 
             ddsd.dwWidth, ddsd.dwHeight, 
             hDCImage, dwBMPOriginX, dwBMPOriginY,
             dwBMPWidth, dwBMPHeight, SRCCOPY);

  if (FAILED(hr = m_pdds->ReleaseDC(hDC)))
    return hr;

  DeleteDC(hDCImage);

  return S_OK;
}


//-----------------------------------------------------------------------------
// Name: CSurface::DrawText()
// Desc: Draws a text string on a DirectDraw surface using hFont or the default
//       GDI font if hFont is NULL.  
//-----------------------------------------------------------------------------
HRESULT CSurface::DrawText(HFONT hFont, TCHAR* strText, 
                           DWORD dwOriginX, DWORD dwOriginY,
                           COLORREF crColor)
{
  HDC     hDC = NULL;
  HRESULT hr;

  if (m_pdds == NULL || strText == NULL)
    return E_INVALIDARG;

  // Make sure this surface is restored.
  if (FAILED(hr = m_pdds->Restore()))
    return hr;

  if (FAILED(hr = m_pdds->GetDC(&hDC)))
    return hr;

  // Set the foreground color
  SetTextColor(hDC, crColor);

  // set background mode
  SetBkMode(hDC, TRANSPARENT);

  if (hFont)
    SelectObject(hDC, hFont);

  // Use GDI to draw the text on the surface
  TextOut(hDC, dwOriginX, dwOriginY, strText, _tcslen(strText));

  if (FAILED(hr = m_pdds->ReleaseDC(hDC)))
    return hr;

  return S_OK;
}


//-----------------------------------------------------------------------------
// Name: CSurface::ReDrawBitmapOnSurface()
// Desc: Load a bitmap from a file or resource into a DirectDraw surface.
//       normaly used to re-load a surface after a restore.
//-----------------------------------------------------------------------------
HRESULT CSurface::DrawBitmap(TCHAR* strBMP, 
                             DWORD dwDesiredWidth, DWORD dwDesiredHeight)
{
  HBITMAP hBMP;
  HRESULT hr;

  if (m_pdds == NULL || strBMP == NULL)
    return E_INVALIDARG;

  //  Try to load the bitmap as a resource, if that fails, try it as a file
  hBMP = (HBITMAP) LoadImage(GetModuleHandle(NULL), strBMP, 
                             IMAGE_BITMAP, dwDesiredWidth, dwDesiredHeight, 
                             LR_CREATEDIBSECTION);
  if(hBMP == NULL) {
    hBMP = (HBITMAP) LoadImage(NULL, strBMP, IMAGE_BITMAP, 
                               dwDesiredWidth, dwDesiredHeight, 
                               LR_LOADFROMFILE | LR_CREATEDIBSECTION);
    if (hBMP == NULL)
      return E_FAIL;
  }

  // Draw the bitmap on this surface
  if (FAILED(hr = DrawBitmap(hBMP, 0, 0, 0, 0))) {
    DeleteObject(hBMP);
    return hr;
  }

  DeleteObject(hBMP);

  return S_OK;
}


//-----------------------------------------------------------------------------
// Name: CSurface::DrawBitmap()
// Desc: Draws a bitmap over an entire DirectDrawSurface, stretching the 
//       bitmap if nessasary
//-----------------------------------------------------------------------------
HRESULT CSurface::DrawBitmap(PLWinBmp* pBmp, DWORD dwBMPWidth, DWORD dwBMPHeight)
{
  if (m_pdds == NULL || pBmp == NULL)
    return E_INVALIDARG;

  HRESULT        hr;
  HDC            hDC;
  DDSURFACEDESC2 ddsd;

  // Make sure this surface is restored.
  if (FAILED(hr = m_pdds->Restore()))
    return hr;

  // Get the surface.description
  memset(&ddsd, 0, sizeof(ddsd));
  
  ddsd.dwSize  = sizeof(ddsd);
  m_pdds->GetSurfaceDesc(&ddsd);

  if (ddsd.ddpfPixelFormat.dwFlags == DDPF_FOURCC)
    return E_NOTIMPL;

  // Stretch the bitmap to cover this surface
  if (FAILED(hr = m_pdds->GetDC(&hDC)))
    return hr;
	
  // copy picture to device context
  pBmp->StretchDraw(hDC, 0, 0, (int)dwBMPWidth, (int)dwBMPHeight);
	
  if (FAILED(hr = m_pdds->ReleaseDC(hDC)))
    return hr;

  return S_OK;
}


//-----------------------------------------------------------------------------
// Name: CSurface::DrawAny()
// Desc: Load a bitmap from a file or resource into a DirectDraw surface.
//       normaly used to re-load a surface after a restore.
//-----------------------------------------------------------------------------
HRESULT CSurface::DrawAny(BYTE* pImage, DWORD dwSize, DWORD dwBMPWidth, DWORD dwBMPHeight)
{
  if (pImage == NULL || m_pdds == NULL)
    return E_INVALIDARG;

  HRESULT           hr;
  PLMemSource       memSrc;
  PLAnyPicDecoder   Decoder;
  PLWinBmp          Bmp;
	
  memSrc.Open(pImage, dwSize);
  Decoder.MakeBmp(&memSrc, &Bmp);
	
  if (FAILED(hr = DrawBitmap(&Bmp, dwBMPWidth, dwBMPHeight)))
    return hr;

  return S_OK;
}


//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
HRESULT CSurface::SetColorKey(DWORD dwColorKey)
{
  if (m_pdds == NULL)
    return E_POINTER;

  m_bColorKeyed = TRUE;

  DDCOLORKEY ddck;
  ddck.dwColorSpaceLowValue  = ConvertGDIColor(dwColorKey);
  ddck.dwColorSpaceHighValue = ConvertGDIColor(dwColorKey);
    
  return m_pdds->SetColorKey(DDCKEY_SRCBLT, &ddck);
}


//-----------------------------------------------------------------------------
// Name: CSurface::ConvertGDIColor()
// Desc: Converts a GDI color (0x00bbggrr) into the equivalent color on a 
//       DirectDrawSurface using its pixel format.  
//-----------------------------------------------------------------------------
DWORD CSurface::ConvertGDIColor(COLORREF dwGDIColor)
{
  if (m_pdds == NULL)
    return 0x00000000;

  COLORREF       rgbT;
  HDC            hdc;
  DWORD          dw = CLR_INVALID;
  DDSURFACEDESC2 ddsd;
  HRESULT        hr;

  //  Use GDI SetPixel to color match for us
  if (dwGDIColor != CLR_INVALID && m_pdds->GetDC(&hdc) == DD_OK) {
    rgbT = GetPixel(hdc, 0, 0);     // Save current pixel value
    SetPixel(hdc, 0, 0, dwGDIColor);       // Set our value
    m_pdds->ReleaseDC(hdc);
  }

  // Now lock the surface so we can read back the converted color
  ddsd.dwSize = sizeof(ddsd);
  hr = m_pdds->Lock(NULL, &ddsd, DDLOCK_WAIT, NULL);
  
  if (hr == DD_OK) 
  {
    dw = *(DWORD *) ddsd.lpSurface; 
    if (ddsd.ddpfPixelFormat.dwRGBBitCount < 32) // Mask it to bpp
      dw &= ( 1 << ddsd.ddpfPixelFormat.dwRGBBitCount ) - 1;  
    
    m_pdds->Unlock(NULL);
  }

  //  Now put the color that was there back.
  if (dwGDIColor != CLR_INVALID && m_pdds->GetDC(&hdc) == DD_OK)
  {
    SetPixel(hdc, 0, 0, rgbT);
    m_pdds->ReleaseDC(hdc);
  }
    
  return dw;    
}


//-----------------------------------------------------------------------------
// Name: CSurface::GetBitMaskInfo()
// Desc: Returns the number of bits and the shift in the bit mask
//-----------------------------------------------------------------------------
HRESULT CSurface::GetBitMaskInfo(DWORD dwBitMask, DWORD* pdwShift, DWORD* pdwBits)
{
  DWORD dwShift = 0;
  DWORD dwBits  = 0; 

  if (pdwShift == NULL || pdwBits == NULL)
    return E_INVALIDARG;

  if (dwBitMask)
  {
    while((dwBitMask & 1) == 0)
    {
      dwShift++;
      dwBitMask >>= 1;
    }
  }

  while ((dwBitMask & 1) != 0)
  {
    dwBits++;
    dwBitMask >>= 1;
  }

  *pdwShift = dwShift;
  *pdwBits  = dwBits;

  return S_OK;
}


//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
BOOL CSurface::GetSPixel16( DWORD dwHeight, DWORD dwWidth, SPIXEL16* psPixel) 
{
  DDSURFACEDESC2 ddsd;
	
  memset(&ddsd, 0, sizeof(ddsd));
  ddsd.dwSize = sizeof(DDSURFACEDESC2);
	
  // get surface description
  if (FAILED(m_pdds->GetSurfaceDesc(&ddsd)))
    return FALSE;
	
  // lock surface
  if (FAILED(m_pdds->Lock(NULL, &ddsd, 0, NULL)))
    return FALSE;
			
  // calculate offset in bytes
  DWORD dwOffset = 0;
		
  dwOffset += dwHeight * ddsd.lPitch; // skip dwHeight rows
  dwOffset += dwWidth * (sizeof(SPIXEL16)/sizeof(BYTE)); // skip dwWidth length
	
  // get destination pixel
  SPIXEL16 *psDest = (SPIXEL16*)((BYTE*)ddsd.lpSurface + dwOffset);

  // copy into result
  memcpy(psPixel, psDest, sizeof(SPIXEL16));
	
  // unlock surface
  m_pdds->Unlock(NULL);

  return TRUE;
}


//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
BOOL CSurface::SetSPixel16(DWORD dwHeight, DWORD dwWidth, SPIXEL16 sPixel) 
{
  DDSURFACEDESC2 ddsd;
	
  memset(&ddsd, 0, sizeof(ddsd));
  ddsd.dwSize = sizeof(DDSURFACEDESC2);
	
  // get surface description
  if (FAILED(m_pdds->GetSurfaceDesc(&ddsd)))
    return FALSE;
	
  // lock surface
  if (FAILED(m_pdds->Lock(NULL, &ddsd, 0, NULL)))
    return FALSE;
			
  // calculate offset in bytes
  DWORD dwOffset = 0;
		
  dwOffset += dwHeight * ddsd.lPitch; // skip dwHeight rows
  dwOffset += dwWidth * (sizeof(SPIXEL16)/sizeof(BYTE)); // skip dwWidth length
	
  // get destination pixel
  SPIXEL16 *psDest = (SPIXEL16*)((BYTE*)ddsd.lpSurface + dwOffset);

  // copy into result
  memcpy(psDest, &sPixel, sizeof(SPIXEL16));
	
  // unlock surface
  m_pdds->Unlock(NULL);

  return TRUE;
}


//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
BOOL CSurface::GetSPixel32(DWORD dwHeight, DWORD dwWidth, SPIXEL32* psPixel) 
{
  DDSURFACEDESC2 ddsd;
	
  memset(&ddsd, 0, sizeof(ddsd));
  ddsd.dwSize = sizeof(DDSURFACEDESC2);
	
  // get surface description
  if (FAILED(m_pdds->GetSurfaceDesc(&ddsd)))
    return FALSE;
	
  // lock surface
  if (FAILED(m_pdds->Lock(NULL, &ddsd, 0, NULL)))
    return FALSE;
			
  // calculate offset in bytes
  DWORD dwOffset = 0;
		
  dwOffset += dwHeight * ddsd.lPitch; // skip dwHeight rows
  dwOffset += dwWidth * (sizeof(SPIXEL32)/sizeof(BYTE)); // skip dwWidth length
	
  // get destination pixel
  SPIXEL32 *psDest = (SPIXEL32*)((BYTE*)ddsd.lpSurface + dwOffset);

  // copy into result
  memcpy(psPixel, psDest, sizeof(SPIXEL32));
	
  // unlock surface
  m_pdds->Unlock(NULL);

  return TRUE;
}


//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
BOOL CSurface::SetSPixel32(DWORD dwHeight, DWORD dwWidth, SPIXEL32 sPixel) 
{
  DDSURFACEDESC2 ddsd;
	
  memset(&ddsd, 0, sizeof(ddsd));
  ddsd.dwSize = sizeof(DDSURFACEDESC2);
	
  // get surface description
  if (FAILED(m_pdds->GetSurfaceDesc(&ddsd)))
    return FALSE;
	
  // lock surface
  if (FAILED(m_pdds->Lock(NULL, &ddsd, 0, NULL)))
    return FALSE;
			
  // calculate offset in bytes
  DWORD dwOffset = 0;
		
  dwOffset += dwHeight*ddsd.lPitch; // skip dwHeight rows
  dwOffset += dwWidth*(sizeof(SPIXEL32)/sizeof(BYTE)); // skip dwWidth length
	
  // get destination pixel
  SPIXEL32 *psDest = (SPIXEL32*)((BYTE*)ddsd.lpSurface + dwOffset);

  // copy into result
  memcpy(psDest, &sPixel, sizeof(SPIXEL32));
	
  // unlock surface
  m_pdds->Unlock(NULL);

  return TRUE;
}


//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
HRESULT CSurface::Clear(DWORD dwColor)
{
  DDBLTFX ddbltfx;

  memset(&ddbltfx, 0, sizeof(DDBLTFX));

  ddbltfx.dwSize      = sizeof(DDBLTFX);
  ddbltfx.dwFillColor = dwColor;

  return m_pdds->Blt(NULL, NULL, NULL, DDBLT_COLORFILL, &ddbltfx);
}