#include "plainx.h"
#include "strman.h"


#define SYMBOL_FORMAT "symbol_%i"


CStringManager::CStringManager()
{
  m_nCounter = 0;
  m_pParent = NULL;
}


CStringManager::~CStringManager()
{
  Free();
}


//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
void CStringManager::Init(CSceneManager* pParent)
{
  m_pParent = pParent;
  Free();
}


//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
int CStringManager::Create(const char* pszString,
                           const char* pszFont,
                           int nWXPos,
                           int nWYPos,
                           int nWZPos)
{
  PXString* pStr;
  PXFont* pFont;

  if (!GetPXFont(pszFont, &pFont))
    return -1;

  pStr = (PXString*)malloc(sizeof(PXString));
  if (!pStr)
    return -1;

  strcpy(pStr->buf, pszString);
  strcpy(pStr->old, pszString);
  strcpy(pStr->font, pszFont);
  pStr->id = m_nCounter++;
  pStr->x = nWXPos;
  pStr->y = nWYPos;
  pStr->z = nWZPos;
  pStr->vis = TRUE;
  pStr->fFromTable = FALSE;
  pStr->szVarName[0] = 0;

  m_hmStrings[pStr->id] = pStr;
  Draw(pStr);
  return pStr->id;
}


//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
int CStringManager::CreateFromTable(const char* pcszVarName,
									const char* pcszFont,
									int nWXPos,
									int nWYPos,
									int nWZPos)
{
  PXString* pStr;
  PXFont* pFont;
  const char* pcszString;

  if (!GetPXFont(pcszFont, &pFont))
    return -1;

  pStr = (PXString*)malloc(sizeof(PXString));
  if (!pStr)
    return -1;

  pcszString = m_pParent->GetEngine()->GetStringTable()->GetString(pcszVarName);
  strcpy(pStr->buf, pcszString);
  strcpy(pStr->old, pcszString);
  strcpy(pStr->font, pcszFont);
  strcpy(pStr->szVarName, pcszVarName);
  pStr->id = m_nCounter++;
  pStr->x = nWXPos;
  pStr->y = nWYPos;
  pStr->z = nWZPos;
  pStr->vis = TRUE;
  pStr->fFromTable = TRUE;

  m_hmStrings[pStr->id] = pStr;
  Draw(pStr);
  return pStr->id;
}


//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
void CStringManager::OnLocaleChanged()
{
	for (STRM_HASH_MAP_ITER i = m_hmStrings.begin(); i != m_hmStrings.end(); i++) {
		PXString* pStr = (PXString*)(*i).second;
		if (pStr && pStr->fFromTable) {
			const char* pcszString = m_pParent->GetEngine()->GetStringTable()->GetString(pStr->szVarName);
			Change(pcszString, pStr->id);
		}
	}
}


//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
BOOL CStringManager::CreateFont(const char* pcszConfig, const char* pcszFont)
{
  CVars vars;
  PXFont* pFont;
  
  if (!vars.Init(pcszConfig))
    return FALSE;

  char buffer[MAX_BUFFER];

  pFont = (PXFont*)malloc(sizeof(PXFont));
  if (!pFont)
    return FALSE;

  memset(pFont, 0, sizeof(PXFont));

  strcpy(pFont->font, pcszFont);
  
  //get sprite
  if (!vars.GetString(L_FONT, pcszFont, L_FONTSPRITE, buffer)) {
    if (!vars.GetString(L_FONT, pcszFont, L_FONTSYSTEM, buffer)) {
      free(pFont);
      return FALSE;
    } else {
      if (!CreateSystemFont(&vars, pFont, pcszFont, buffer)) {
        free(pFont);
        return FALSE;
      }
    }
  } else {
    strcpy(pFont->sprite, buffer);

    //get symbols
    int k = 0;
    for (int i = 32; i < 256; i++) {
      char szSymbolName[MAX_BUFFER];
      int nFrame;

      wsprintf(szSymbolName, SYMBOL_FORMAT, i);
      if (vars.GetInt(L_FONT, pcszFont, szSymbolName, 0, &nFrame)) {
        pFont->symbols[k].symbol = i;
        pFont->symbols[k].frame = nFrame;
        k++;
        if (k == (MAX_SYMBOLS - 1))
          break;
      }
    }
    pFont->symbols[k].symbol = 0;
  }

  //add to fonts
  m_hmFonts[pFont->font] = pFont;

  return TRUE;
}


//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
void CStringManager::Draw(PXString* pStr)
{
  char s;
  char szSprite[MAX_BUFFER];
  int iFrame, xpos, lo, ln, i;
  CSprite* pS;
  PXFont* pFont;

  if (!GetPXFont(pStr->font, &pFont))
    return;
  
  xpos = pStr->x;
  ln = strlen(pStr->buf);
  lo = strlen(pStr->old);

  for (i=0; i<ln; i++) {
    s = pStr->buf[i];
    if (Symbol2Frame(pFont, s, &iFrame)) {
      wsprintf(szSprite, STRING_NAME_FORMAT, pStr->id, i);
      pS = m_pParent->GetSprite(szSprite);
      if (pS == NULL) {
        if (m_pParent->CloneSprite(pFont->sprite, szSprite)) {
          pS = m_pParent->GetSprite(szSprite);
        } else {
          break;
        }
      }
      if (pS != NULL) {
        pS->SetFrame(iFrame);
        pS->SetVisible(pStr->vis, TRUE);
        pS->SetWPos(xpos, pStr->y, pStr->z);
        xpos += pS->GetFrameWidth();
      } else {
        break;
      }
    }
  }
  if ((lo - ln) > 0) {
    for (i = ln; i<lo; i++) {
      wsprintf(szSprite, STRING_NAME_FORMAT, pStr->id, i);
      m_pParent->DelSprite(szSprite);
    }
  }
}


//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
void CStringManager::Delete(int nStringID)
{
  PXString* pStr;

  if (GetPXString(nStringID, &pStr)) {
    Change("", nStringID);
    free(pStr);
    m_hmStrings.erase(nStringID);
  }
}


//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
void CStringManager::DeleteAll()
{
  for (STRM_HASH_MAP_ITER i = m_hmStrings.begin(); i != m_hmStrings.end(); i++) {
    PXString* pStr = (PXString*)(*i).second;
    if (pStr) Change("", pStr->id);
  }
  Free();
}


//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
void CStringManager::Change(const char* pszString, int nStringID)
{
  PXString* pStr;

  if (GetPXString(nStringID, &pStr)) {
    strcpy(pStr->old, pStr->buf);
    strcpy(pStr->buf, pszString);
    Draw(pStr);
  }
}


//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
void CStringManager::ChangeFromTable(const char* pcszVarName, int nStringID)
{
	PXString* pStr;

	if (GetPXString(nStringID, &pStr)) {
		if (pStr->fFromTable) {
			const char* pcszVarValue = m_pParent->GetEngine()->GetStringTable()->GetString(pcszVarName);
			strcpy(pStr->szVarName, pcszVarName);
			Change(pcszVarValue, nStringID);
		}
	}
}

//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
void CStringManager::GetPos(int nStringID, int* pnWXPos, int *pnWYPos, int* pnWZPos)
{
  PXString* pStr;

  if (GetPXString(nStringID, &pStr)) {
    *pnWXPos = pStr->x;
    *pnWYPos = pStr->y;
    *pnWZPos = pStr->z;
  }
}


//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
void CStringManager::SetPos(int nStringID, int nWXPos, int nWYPos, int nWZPos)
{
  PXString* pStr;

  if (GetPXString(nStringID, &pStr)) {
    pStr->x = nWXPos;
    pStr->y = nWYPos;
    pStr->z = nWZPos;
    Draw(pStr);
  }
}


//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
BOOL CStringManager::GetVisible(int nStringID)
{
  PXString* pStr;

  if (GetPXString(nStringID, &pStr)) {
    return pStr->vis;
  }
  return FALSE;
}


//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
void CStringManager::SetVisible(BOOL bVisible, int nStringID)
{
  PXString* pStr;
  
  if (nStringID != -1) {
    if (GetPXString(nStringID, &pStr)) {
      pStr->vis = bVisible;
      Draw(pStr);
    }
  } else {
    for (STRM_HASH_MAP_ITER i = m_hmStrings.begin(); i != m_hmStrings.end(); i++) {
      PXString* pStr = (PXString*)(*i).second;
      if (pStr) {
        pStr->vis = bVisible;
        Draw(pStr);
      }
    }
  }
}


//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
int CStringManager::GetPXString(int id, PXString** ppStr)
{
  int res;
  
  res = (m_hmStrings.find(id) == m_hmStrings.end()) ? 0 : 1;
  if (res) {
    *ppStr = m_hmStrings[id];
  }
  return res;
}


//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
int CStringManager::GetPXFont(const char* pcszFont, PXFont** ppFont)
{
  int res;
  
  res = (m_hmFonts.find(pcszFont) == m_hmFonts.end()) ? 0 : 1;
  if (res) {
    *ppFont = m_hmFonts[pcszFont];
  }
  return res;
}


//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
BOOL CStringManager::Symbol2Frame(PXFont* pFont, char symbol, int* pnFrame)
{
  BOOL bRes = FALSE;
  PXSymbol ps;
  int i = 0;

  *pnFrame = 0;
  ps = pFont->symbols[i++];
  while (ps.symbol != 0 && !bRes) {
    if (ps.symbol == symbol) {
      *pnFrame = ps.frame;
      bRes = TRUE;
    }
    ps = pFont->symbols[i++];
  }
  return bRes;
}


//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
void CStringManager::Free()
{
  for (STRM_HASH_MAP_ITER i = m_hmStrings.begin(); i != m_hmStrings.end(); i++) {
    PXString* pStr = (PXString*)(*i).second;
    if (pStr) free(pStr);
  }
  m_hmStrings.clear();
  
  for (FONTS_HASH_MAP_ITER j = m_hmFonts.begin(); j != m_hmFonts.end(); j++) {
    PXFont* pFont = (PXFont*)(*j).second;
    if (pFont) {
      if (pFont->hFont) 
        DeleteObject(pFont->hFont);
      free(pFont);
    }
  }
  m_hmFonts.clear();

  m_nCounter = 0;
}


//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
BOOL CStringManager::CreateSystemFont(CVars* pVars,
                                      PXFont* pFont,
                                      const char* pcszFont, 
                                      const char* pcszSystemFontName)
{
  int nHeight, nWeight;
  int fT[3];
  int nQuality;
  int nOpaque;
  int nCKey;
  HFONT hFont;
  COLORREF crColor;
  COLORREF crBackColor;
  int dwRGB[3];
  char charSet[MAX_BUFFER + 1];

  memset(charSet, 0, MAX_BUFFER);
  if (!pVars->GetString(L_FONT, pcszFont, L_FONTCHARSET, charSet)) {
    for (int i = 32; i < 256; i++) {
      charSet[i - 32] = i;
    }
  }
  charSet[MAX_BUFFER] = 0;

  pVars->GetInt(L_FONT, pcszFont, L_FONTHEIGHT, 0, &nHeight);
  pVars->GetInt(L_FONT, pcszFont, L_FONTWEIGHT, 0, &nWeight);
  
  nOpaque = 0;
  pVars->GetInt(L_FONT, pcszFont, L_FONTOPAQUE, 0, &nOpaque);
  if (nOpaque == 1) {
    nCKey = 0;
  } else {
    nCKey = 1; // transparent background
  }
  
  for (int i = 0; i < 3; i++)
    pVars->GetInt(L_FONT, pcszFont, L_FONTTYPES, i, &fT[i]);

  pVars->GetInt(L_FONT, pcszFont, L_FONTQUALITY, 0, &nQuality);

  for (int i = 0; i < 3; i++)
    pVars->GetInt(L_FONT, pcszFont, L_FONTCOLOR, i, &dwRGB[i]);
  crColor = RGB(dwRGB[0], dwRGB[1], dwRGB[2]);

  for (int i = 0; i < 3; i++)
    pVars->GetInt(L_FONT, pcszFont, L_FONTBACKCOLOR, i, &dwRGB[i]);
  crBackColor = RGB(dwRGB[0], dwRGB[1], dwRGB[2]);

  hFont = ::CreateFont(nHeight, 0, 0, 0, nWeight,
                       (BOOL)fT[0], (BOOL)fT[1], (BOOL)fT[2],
                       RUSSIAN_CHARSET, OUT_DEFAULT_PRECIS,
                       CLIP_DEFAULT_PRECIS, nQuality,
                       DEFAULT_PITCH, pcszSystemFontName);
  
  if (hFont == NULL) {
    return FALSE;
  }

  // get scene system font resource manager
  CResourceManager* pResMan = m_pParent->GetResourceManager(TRUE);
  if (pResMan == NULL) {
    DeleteObject(hFont);
    return FALSE;
  }

  HWND hWnd = m_pParent->GetEngine()->GetHWnd();
  HDC hDC = GetDC(hWnd);
  if (hDC == NULL) {
    DeleteObject(hFont);
    return FALSE;
  }

  HFONT hOldFont = (HFONT)SelectObject(hDC, hFont);

  SIZE ps;
  GetTextExtentPoint32(hDC, charSet, strlen(charSet), &ps);

  BOOL bRes = pResMan->CreateSurface(pcszFont, ps.cx, ps.cy,
									 0, RMT_BOTH, nCKey, crBackColor);
  if (!bRes) {
    SelectObject(hDC, hOldFont);
    ReleaseDC(hWnd, hDC);
    DeleteObject(hFont);
    return FALSE;
  }

  // draw font stripe
  HRESULT hr = pResMan->GetSurface(pcszFont)->DrawText(hFont, charSet,
                                                       0, 0, crColor);

  if (FAILED(hr)) {
    SelectObject(hDC, hOldFont);
    ReleaseDC(hWnd, hDC);
    DeleteObject(hFont);
    return FALSE;
  }

  // create sprite for surface
  SPRITE_DATA sd;
  memset(&sd, 0, sizeof(SPRITE_DATA));
  
  // set resource data
  strcpy(sd.szData, pcszFont);
  
  // set frames
  sd.nFrames = strlen(charSet);
  sd.nRows = 1;

  // set framewidth
  int k = 0;

  while (charSet[k] != 0) {
	char szSym[2];

	szSym[0] = charSet[k];
	szSym[1] = 0;

	GetTextExtentPoint32(hDC, szSym, strlen(szSym), &ps);
	sd.nFrameWidth[k++] = ps.cx;
  }

  // create font sprite
  CSprite* pfs = m_pParent->CreateSprite(&sd);
  if (pfs == NULL) {
    SelectObject(hDC, hOldFont);
    ReleaseDC(hWnd, hDC);
    DeleteObject(hFont);
    return FALSE;
  }

  // fill font struct
  strcpy(pFont->sprite, pfs->GetName());
  pFont->hFont = hFont;
  pFont->crColor = crColor;

  // get symbols
  k = 0;
  while (charSet[k] != 0) {
    pFont->symbols[k].symbol = charSet[k];
    pFont->symbols[k].frame = k++;
  }
  pFont->symbols[k].symbol = 0;

  SelectObject(hDC, hOldFont);
  ReleaseDC(hWnd, hDC);

  return TRUE;
}


//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
void CStringManager::RedrawAll()
{
  if (m_pParent == NULL)
    return;

  // redraw all system fonts
  for (FONTS_HASH_MAP_ITER i = m_hmFonts.begin(); i != m_hmFonts.end(); i++) {
    PXFont* pFont = (PXFont*)(*i).second;
    if (pFont) {
      if (pFont->hFont != NULL) {
        CSprite* ps = m_pParent->GetSprite(pFont->sprite);
        if (ps != NULL) {
          CSurface* pSurf = ps->GetSurface();
          if (pSurf != NULL) 
          {
            // recreate char set
            char szCharSet[MAX_SYMBOLS];
            int k = 0;

            memset(szCharSet, 0, sizeof(szCharSet));

            while (pFont->symbols[k].symbol != 0) {
              szCharSet[k] = pFont->symbols[k++].symbol;
            }

            pSurf->DrawText(pFont->hFont, szCharSet, 0, 0, pFont->crColor);
          }
        }
      }
    }
  }
}
