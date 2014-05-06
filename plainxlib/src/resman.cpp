#include "plainx.h"
#include "resman.h"

#define BMPDUMMYSIZE 100


CResourceManager::CResourceManager()
{
    m_pParent = NULL;
    m_pFM = new CFileManager();
}


CResourceManager::CResourceManager(CEngine* pEngine)
{
    m_pParent = pEngine;
    m_pFM = new CFileManager();
}


CResourceManager::~CResourceManager()
{
    Free();

    if (m_pFM) {
        m_pFM->CloseFiles();
        delete m_pFM;
    }
}


//-----------------------------------------------------------------------------
// Name: InitResourceManager
// Desc: Load config file
//-----------------------------------------------------------------------------
BOOL CResourceManager::Init(const char* pszConfig, CEngine* pEngine, const char* pszName)
{
    CVars vars;

    if (pszConfig == NULL || m_pFM == NULL || pEngine == NULL || pszName == NULL)
        return FALSE;

    m_pParent = pEngine;


    if (!vars.Init(pszConfig))
        return FALSE;

    strcpy(m_szConfig, pszConfig);
    Free();
    return rm_LoadGroup(pszName, &vars);
}


//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
void CResourceManager::Free()
{
  for (RM_DATA_HASH_MAP_ITERATOR i = m_pData.begin(); i != m_pData.end(); i++) {
    RM_DATA* prm_d = (RM_DATA*)(*i).second;
    if (prm_d != NULL) {
      if (prm_d->pSurface != NULL)
        delete prm_d->pSurface;
      free(prm_d);
    }
  }
  m_pData.clear();
}


//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
BOOL CResourceManager::RecreateAll()
{
  RM_DATA* rmd;
  int iSize, k;

  iSize = m_pData.size();
  
  if (iSize == 0)
    return TRUE;

  rmd = (RM_DATA*)malloc(iSize * sizeof(RM_DATA));
  if (rmd == NULL)
    return FALSE;

  memset(rmd, 0, iSize * sizeof(RM_DATA));

  k = 0;
  
  for (RM_DATA_HASH_MAP_ITERATOR i = m_pData.begin(); i != m_pData.end(); i++) {
    RM_DATA* prm_d = (RM_DATA*)(*i).second;
    if (prm_d != NULL) {
      delete prm_d->pSurface;
      prm_d->pSurface = NULL;
      memcpy(&rmd[k++], prm_d, sizeof(RM_DATA));
      free(prm_d);
    }
  }
  
  m_pData.clear();
  BOOL bRes = TRUE;
  
  for (int j = 0; j < k && bRes; j++) 
  {
    RM_DATA* pt = &rmd[j];
    bRes = CreateSurface(pt->szName, pt->nWidth, pt->nHeight,
                         pt->szPath, pt->eMemoryType, pt->nCKey, 
                         pt->crBack);
  }
  free(rmd);
  return bRes;
}


//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
BOOL CResourceManager::ReloadAll()
{
  HRESULT hr;
  DWORD dwSize;
  BOOL bIsDummy;
  BYTE* pBMP;


  for (RM_DATA_HASH_MAP_ITERATOR i = m_pData.begin(); i != m_pData.end(); i++) 
  {
    RM_DATA* prm_d = (RM_DATA*)(*i).second;
    if (prm_d != NULL) 
    {
      if (prm_d->pSurface != NULL) 
      {
        //map bitmap
        bIsDummy = FALSE;
        pBMP = (BYTE*)m_pFM->LoadFile(prm_d->szPath, &dwSize);
        if (pBMP == NULL) 
        {
          pBMP = rm_CreateDummyBitmap(prm_d->nWidth, 
                                      prm_d->nHeight,
                                      prm_d->crBack,
                                      &dwSize);
          bIsDummy = TRUE;
          if (pBMP == NULL)
            return FALSE;
        }

        // restore surface
        hr = prm_d->pSurface->GetDDrawSurface()->Restore();
        if (FAILED(hr))
          return FALSE;

        // reload existing surface
        hr = prm_d->pSurface->DrawAny(pBMP, dwSize, prm_d->nWidth, prm_d->nHeight);
        if (FAILED(hr))
          return FALSE;

        // set color key
        if (prm_d->nCKey) 
        {
          hr = prm_d->pSurface->SetColorKey(CLR_INVALID);
          if (FAILED(hr))
            return FALSE;
        }
      }
    }
  }
  return TRUE;
}


//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
BOOL CResourceManager::rm_LoadGroup(const char* pszName, CVars* pVars)
{
  if (pszName == NULL || pVars == NULL)
    return FALSE;

  //remember group name
  strcpy(m_szGroup, pszName);

  //prepare buffer
  char buffer[MAX_BUFFER];
 
  //read archive
  if (!pVars->GetString(L_GROUP, m_szGroup, L_ARCHIVE, buffer)) {
    if (!m_pFM->Init(m_pParent->GetRoot()))
      return FALSE;
  } else {
    if (!m_pFM->Init(m_pParent->GetRoot(), buffer))
      return FALSE;
  }

  //read first data
  if (pVars->GetNextGroup(L_DATA, buffer, TRUE)) {
    if (!rm_CreateData(buffer, pVars)) {
      return FALSE;
    }
    while (pVars->GetNextGroup(L_DATA, buffer)) {
      if (!rm_CreateData(buffer, pVars)) {
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
BOOL CResourceManager::rm_CreateData(const char* pszName, CVars* pVars)
{
  char szFilename[1024];
  int value, iWidth, iHeight, iCKey;
  RM_TYPE eAllocType;
  char szBuffer[MAX_BUFFER];

  // set def values
  iCKey = 0;
  eAllocType = RMT_BOTH;
  iWidth = -1;
  iHeight = -1;
  COLORREF crBack = RGB(0, 0, 0);
  strcpy(szFilename, "");

  //read filename
  if (pVars->GetString(L_DATA, pszName, L_FNAME, szBuffer))
    strcpy(szFilename, szBuffer);

  //read ckey [optional]
  if (pVars->GetInt(L_DATA, pszName, L_CKEY, 0, &value))
    iCKey = value;

  //read width [optional]
  if (pVars->GetInt(L_DATA, pszName, L_WIDTH, 0, &value))
    iWidth = value;

  //read height [optional]
  if (pVars->GetInt(L_DATA, pszName, L_HEIGHT, 0, &value))
    iHeight = value;

  //read memory type [optional]
  if (pVars->GetString(L_DATA, pszName, L_MEMORY, szBuffer)) {
    if (strcmp(szBuffer, L_VIDEOMEM) == 0)
      eAllocType = RMT_VIDEO;
    else
      eAllocType = RMT_SYSTEM;
  }

  // create surface
  BOOL bRes = CreateSurface(pszName,
                            iWidth,
                            iHeight,
                            szFilename,
                            eAllocType,
                            iCKey,
                            crBack);
  return bRes;
}


//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
CSurface* CResourceManager::GetSurface(const char* pszName)
{
  RM_DATA* pData = m_pData[pszName];
  if (pData != NULL)
    return pData->pSurface;
  else
    return NULL;
}

//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
int CResourceManager::GetWidth(const char* pszName)
{
  RM_DATA* pData = m_pData[pszName];
  if (pData != NULL)
    return pData->nWidth;
  else
    return -1;
}

//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
int  CResourceManager::GetHeight(const char* pszName)
{
  RM_DATA* pData = m_pData[pszName];
  if (pData != NULL)
    return pData->nHeight;
  else
    return -1;
}

//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
int  CResourceManager::GetColorKey(const char* pszName)
{
  RM_DATA* pData = m_pData[pszName];
  if (pData != NULL)
    return pData->nCKey;
  else
    return -1;
}

//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
BYTE* CResourceManager::rm_CreateDummyBitmap(int nWidth, 
                                             int nHeight, 
                                             COLORREF crBack, 
                                             DWORD* pdwSize)
{
  #define BMPFH_SIZE 14
  #define BMPIH_SIZE 40
  #define MKTAG2(a,b) (a | (b << 8))

  BITMAPINFOHEADER bih;
  BITMAPFILEHEADER bfh;

  memset(&bfh, 0, BMPFH_SIZE);
  memset(&bih, 0, BMPIH_SIZE);

  /* fill bitmap file header */
  bfh.bfType = MKTAG2('B','M'); /* BM */
  bfh.bfSize = BMPFH_SIZE + BMPIH_SIZE + nHeight*(nWidth + nWidth % 4)*3;
  bfh.bfOffBits = BMPFH_SIZE + BMPIH_SIZE; /* 54 */
    
  /* fill bitmap info header */
  bih.biSize = BMPIH_SIZE;
  bih.biWidth = nWidth;
  bih.biHeight = nHeight;
  bih.biPlanes = 1;
  bih.biBitCount =  24;
  bih.biCompression = 0;
   
  *pdwSize = bfh.bfSize;
  BYTE* bmp = (BYTE*)malloc(bfh.bfSize);
  if (bmp == NULL) 
    return NULL;

  BYTE* p = bmp;

  memcpy(p, &bfh, BMPFH_SIZE);
  p += BMPFH_SIZE;
  memcpy(p, &bih, BMPIH_SIZE);
  p += BMPIH_SIZE;

  for (int y = 0; y < nHeight; y++) {
	  for (int x = 0; x < nWidth; x++) {
		*p++ = GetBValue(crBack);
		*p++ = GetGValue(crBack);
		*p++ = GetRValue(crBack);
	  }
	  p += nWidth % 4;
  }
  
  /*static int co = 0;
  char dsds[256];
  wsprintf(dsds, "dump_%i.bmp", co++);
  FILE* ftemp = fopen(dsds, "wb");
  fwrite(bmp, 1, bfh.bfSize, ftemp);
  fclose(ftemp);*/

  return bmp;
}


//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
BOOL CResourceManager::CreateSurface(const char* pcszName, 
                                     int iWidth, 
                                     int iHeight,
                                     const char* pcszFilename,
                                     RM_TYPE eAllocationType,
                                     int iCKey,
                                     COLORREF crBack)
{
  // check for existing
  if (GetSurface(pcszName) != NULL)
    return FALSE;
  
  // create data struct
  RM_DATA* prm_d = (RM_DATA*)malloc(sizeof(RM_DATA));
  memset(prm_d, 0, sizeof(RM_DATA));
	
  // fill data struct
  strcpy(prm_d->szName, pcszName);
  prm_d->pSurface = NULL;
  prm_d->nCKey = iCKey;
  prm_d->nHeight = iHeight;
  prm_d->nWidth = iWidth;
  prm_d->eMemoryType = eAllocationType; //default use both memory
  prm_d->crBack = crBack;
  if (pcszFilename == 0) {
    strcpy(prm_d->szPath, "");
  } else {
    strcpy(prm_d->szPath, pcszFilename);
  }

  //map bitmap
  DWORD dwSize;
  BOOL bIsDummy = FALSE;
  BYTE* pBMP = (BYTE*)m_pFM->LoadFile(prm_d->szPath, &dwSize);
  if (pBMP == NULL) {
    if (prm_d->nWidth == -1) 
      prm_d->nWidth = BMPDUMMYSIZE;
    if (prm_d->nHeight == -1) 
      prm_d->nHeight = BMPDUMMYSIZE;
    
    pBMP = rm_CreateDummyBitmap(prm_d->nWidth, 
                                prm_d->nHeight, 
                                crBack,
                                &dwSize);
    if (pBMP == NULL) {
      free(prm_d);
      return FALSE;
    }
    bIsDummy = TRUE;
  }
		
  // check if enough free video memory
  if (m_pParent->GetRTIntVar(L_RT_USE_VID_MEM) == 1) {
    if (prm_d->eMemoryType == RMT_BOTH)
      prm_d->eMemoryType = RMT_VIDEO;
  }
  BOOL bIsUseVideoMemory = (prm_d->eMemoryType == RMT_VIDEO);

  //create surface
  if (FAILED(m_pParent->GetVideo()->GetDisplay()->CreateSurfaceFromAny(&prm_d->pSurface, 
																		pBMP, 
																		dwSize,					
																		&prm_d->nWidth, 
																		&prm_d->nHeight, 
																		bIsUseVideoMemory))) 
  {
    //well maybe there is no any video memory
    if (bIsUseVideoMemory) {

      //try load to system memory
      prm_d->eMemoryType = RMT_SYSTEM;
      if (FAILED(m_pParent->GetVideo()->GetDisplay()->CreateSurfaceFromAny(&prm_d->pSurface, 
                                                                           pBMP, 
                                                                           dwSize,
                                                                           &prm_d->nWidth,
                                                                           &prm_d->nHeight,
                                                                           FALSE))) 
	  {
        //we failed load to system memory - too shit
        
        if (bIsDummy)
          free(pBMP);
        
        free(prm_d);
	
        return FALSE;
      }
    } else {
      //we failed load to system memory - too shit

      if (bIsDummy)
        free(pBMP);

      free(prm_d);

      return FALSE;
    }
  }
		
  // Set the color key
  if (prm_d->nCKey) {
    if (FAILED(prm_d->pSurface->SetColorKey(CLR_INVALID))) {
      
      if (bIsDummy)
        free(pBMP);

      free(prm_d);

      return FALSE;
    }
  }

  //add to map
  m_pData[prm_d->szName] = prm_d;

  if (bIsDummy)
    free(pBMP);
	
  //all was ok
  return TRUE;
}

