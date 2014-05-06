#include "plainx.h"
#include "archive.h"
#include "fileman.h"


CFileManager::CFileManager()
{
  m_szRootPath = NULL;
  m_szArName = NULL;
}


CFileManager::~CFileManager()
{
  if (m_szRootPath != NULL) free(m_szRootPath);
  if (m_szArName != NULL) free(m_szArName);
  CloseFiles();
}


//-------------------------------------------------------------------
//
//
//-------------------------------------------------------------------
void* CFileManager::LoadFile(char* path, LPDWORD pdwSize)
{
  // check if we already load this file
  FM_VIEW* fmView = NULL;

  fmView = m_viewMap[path];
  if (fmView != NULL) {
    *pdwSize = fmView->dwSize;
    return (void*)(fmView->vppViewList);
  }

  char szTemp[1024];
  if (m_szRootPath != NULL)
    wsprintf(szTemp, "%s\\%s", m_szRootPath, path);
  else
    wsprintf(szTemp, "%s", path);

  HANDLE hFile = CreateFile(szTemp,
                            GENERIC_READ,
                            FILE_SHARE_READ,
                            NULL,
                            OPEN_EXISTING,
		            FILE_ATTRIBUTE_NORMAL,
                            NULL);
	
  if (hFile != INVALID_HANDLE_VALUE) {
    DWORD dwFileSizeHigh, dwFileSizeLow;
    dwFileSizeLow = GetFileSize(hFile, &dwFileSizeHigh);

    if (dwFileSizeLow == INVALID_FILE_SIZE)
      return NULL;

    if (pdwSize != NULL)
      *pdwSize = dwFileSizeLow;

    HANDLE hMap = CreateFileMapping(hFile,
                                    NULL,
                                    PAGE_READONLY,
                                    0,
                                    0,
                                    NULL);

    if (hMap == INVALID_HANDLE_VALUE)
      return NULL;

    void* vpTemp = MapViewOfFile(hMap, FILE_MAP_READ, 0, 0, 0);

    CloseHandle(hMap);
    CloseHandle(hFile);

    fmView = (FM_VIEW*)malloc(sizeof(FM_VIEW));
    fmView->vppViewList = (DWORD)vpTemp;
    fmView->dwSize = *pdwSize;
    strcpy(fmView->name, path);
    m_viewMap[fmView->name] = fmView;
		
    return vpTemp;
  }

  // Here go code for load file from archive, if it not exists.

  DWORD dwSize;
  void* retval = LoadArchiveFile(path, &dwSize);
  if (pdwSize != NULL)
    *pdwSize = dwSize;

  fmView = (FM_VIEW*)malloc(sizeof(FM_VIEW));
  fmView->vppViewList = (DWORD)retval;
  fmView->dwSize = *pdwSize;
  strcpy(fmView->name, path);
  m_viewMap[fmView->name] = fmView;
	
  return retval;
}


//-------------------------------------------------------------------
//
//
//-------------------------------------------------------------------
BOOL CFileManager::CloseFiles()
{
  BOOL bResult = TRUE;

  for (FM_VIEW_HASH_MAP_ITERATOR i = m_viewMap.begin(); i != m_viewMap.end(); i++) {
    FM_VIEW* pfmView = (FM_VIEW*)(*i).second;
    if (pfmView) {
	  void*	lpBaseAddress = (void*)(pfmView->vppViewList);
	  if (lpBaseAddress)
		bResult &= UnmapViewOfFile(lpBaseAddress);
      free(pfmView);
    }
  }

  m_viewMap.clear();
	
  return bResult;
}


//-------------------------------------------------------------------
//
//
//-------------------------------------------------------------------
BOOL CFileManager::Init(const char* path, const char* ArName)
{
  if (m_szRootPath != NULL) free(m_szRootPath);
  if (m_szArName != NULL) free(m_szArName);

  if (path != NULL) {
    if (path[strlen(path) - 1] == '\\') {
      m_szRootPath = (char*)malloc(sizeof(char) * (strlen(path)));
      strncpy(m_szRootPath, path, strlen(path) - 1);
      m_szRootPath[strlen(path) - 1] = '\0';
    } else {
      m_szRootPath = (char*)malloc(sizeof(char) * (strlen(path) + 1));
      strcpy(m_szRootPath, path);
    }
  } else {
    return FALSE;
  }

  if (ArName != NULL) {
    m_szArName = (char*)malloc(sizeof(char) * (strlen(ArName) + 1));
    strcpy(m_szArName, ArName);
    AddPatchFile(ArName);
  }

  return TRUE;
}


//-------------------------------------------------------------------
//
//
//-------------------------------------------------------------------
void* CFileManager::fm_LoadDir(char* path, 
                               void* vpStartPos, 
                               void* vpEndPos, 
                               DWORD* pdwSize)
{
  LPEntry pEntry;
  char* path2;
  char* name;
  char* tmp;
  void* vpTemp=vpStartPos;
  void* vpRetval;
	
  pEntry = (LPEntry)vpStartPos;
  tmp = strchr(path, '\\');

  if (tmp != NULL) {
    tmp += 1;
    path2 = (char*)malloc(strlen(tmp) + 1);
    strcpy(path2, tmp);

    int len = strlen(path) - strlen(tmp);
    name = (char*)malloc(len + 1);
    strncpy(name, path, len);
    name[len]= '\0';

    _strlwr(name);
    while ((strcmp(name, pEntry->name) != 0) && (vpTemp < vpEndPos)) {
      vpTemp = (void*)((DWORD)vpTemp + pEntry->size + sizeof(Entry));
      pEntry = (LPEntry)vpTemp;
    }

    if (strcmp(name, pEntry->name) != 0) {
      free(path2);
      free(name);
      pdwSize = NULL;
      return NULL;
    }

    vpRetval = fm_LoadDir(path2, 
                          (void*)((DWORD)vpTemp + sizeof(Entry)),
                          (void*)((DWORD)vpTemp + sizeof(Entry) + pEntry->size),
                          pdwSize);

    free(path2);
    free(name);

    return vpRetval;
  } else {
    vpRetval = fm_LoadFile(path, vpStartPos, vpEndPos, pdwSize);
    return vpRetval;
  }

  return NULL;
}

//-------------------------------------------------------------------
//
//
//-------------------------------------------------------------------
void* CFileManager::fm_LoadFile(char* path, 
                                void* vpStartPos, 
                                void* vpEndPos, 
                                DWORD* pdwSize)
{
  LPEntry pEntry;
  void* vpTemp=vpStartPos;
  char* name;
  void* vpRetval;

  pEntry = (LPEntry)vpTemp;
  name = (char*)malloc(strlen(path) + 1);
  strcpy(name, path);
  _strlwr(name);

  while ((strcmp(name, pEntry->name) != 0) && (vpTemp < vpEndPos)) {
    vpTemp = (void*)((DWORD)vpTemp + pEntry->size + sizeof(Entry));
    pEntry = (LPEntry)vpTemp;
  }

  if (strcmp(name, pEntry->name) != 0) {
    free(name);
    pdwSize = NULL;
    return NULL;
  }

  vpRetval = (void*)((DWORD)vpTemp + sizeof(Entry));
  *pdwSize = pEntry->size;
  free(name);

  return vpRetval;
}

//-------------------------------------------------------------------
//
//
//-------------------------------------------------------------------
void* CFileManager::LoadArchiveFile(char* path, LPDWORD pdwSize)
{
  BOOL bEncrypt = FALSE;
  BOOL bZip = FALSE;
  BYTE* key;

  if (m_szArName == NULL) return NULL;

  // Making path to archive file
  char arcPath[1024];

  if (m_szRootPath != NULL)
    wsprintf(arcPath, "%s\\%s", m_szRootPath, m_szArName);
  else
    wsprintf(arcPath, "%s", m_szArName);

  // Mapping archive file
  HANDLE hFile = CreateFile(arcPath,
                            GENERIC_READ | GENERIC_WRITE,
                            FILE_SHARE_READ,
                            NULL,
                            OPEN_EXISTING,
                            FILE_ATTRIBUTE_NORMAL,
                            NULL);

  if (hFile == INVALID_HANDLE_VALUE)
    return NULL;

  HANDLE hMap = CreateFileMapping(hFile, NULL, PAGE_WRITECOPY, 0, 0, NULL);
  CloseHandle(hFile);

  if (hMap == INVALID_HANDLE_VALUE)
    return NULL;
	
  void* vpTemp=MapViewOfFile(hMap, FILE_MAP_COPY, 0, 0, 0);

  if (vpTemp == NULL) {
    CloseHandle(hMap);
    return NULL;
  }
	
  // Loading header of archive
  LPHeader pHead;
  pHead = (LPHeader)vpTemp;

  if ((pHead->id) != (ID)) {
    return NULL; //incorect archive file.
  }

  switch (pHead->version) {
    case VER:
      bEncrypt = FALSE;
      bZip = FALSE;
      break;
    case VER2:
      bEncrypt = TRUE;
      bZip = FALSE;
      break;
    case VER3:
      if (pHead->flags == GAF_NONE) {
        bEncrypt = FALSE;
        bZip = FALSE;
      } else {
	if (((pHead->flags) & (GAF_ARCHIVED)) == GAF_ARCHIVED) bZip = TRUE;
        if (((pHead->flags) & (GAF_ENCRYPTED)) == GAF_ENCRYPTED) bEncrypt = TRUE;
      }
      break;
    default:
      return NULL; //incorect archive file.
  }

  void* vpTemp2=(void*)((DWORD)vpTemp + sizeof(Header));
  if (bEncrypt) {
    key = (BYTE*)vpTemp2;
    vpTemp2 = (void*)((DWORD)vpTemp2 + 256);
  }
  
  LPEntry pEntry = (LPEntry)vpTemp2;

  void* vpFile = fm_LoadDir(path,
                            vpTemp2,
                            (void*)((DWORD)vpTemp2 + sizeof(Entry) + pEntry->size),
                            pdwSize);

  if (vpFile == NULL) return NULL;

  pEntry = (LPEntry)((DWORD)vpFile - sizeof(Entry));
  SYSTEM_INFO sysInfo;

  GetSystemInfo(&sysInfo);

  DWORD ost = pEntry->offset % sysInfo.dwAllocationGranularity;

  void* retval = MapViewOfFile(hMap,
                               FILE_MAP_COPY,
                               0,
                               pEntry->offset - ost,
                               pEntry->size + ost);

  CloseHandle(hMap);

  if (bEncrypt) {
    BYTE* file=(BYTE*)((DWORD)retval + ost);

    for (DWORD i = 0; i < *pdwSize; i++) {
      for(int j = 0; j < 256; j++) {
        if (file[i] == key[j]) {
          file[i] = j;
          break;
        }
      }
    }
  }
  
  HANDLE hZipMap;
  void* vpZipView;

  if (bZip) {
    hZipMap = CreateFileMapping(INVALID_HANDLE_VALUE,
                                NULL,
                                PAGE_READWRITE,
                                0,
                                pEntry->data,
                                NULL);
    if (hZipMap == NULL) return NULL;

    vpZipView = MapViewOfFile(hZipMap, FILE_MAP_WRITE, 0, 0, 0);
    if (vpZipView == NULL) return NULL;

    CloseHandle(hZipMap);
    int err = uncompress((Bytef*)vpZipView,
                         (uLongf*)&(pEntry->data),
                         (Bytef*)((DWORD)retval + ost),
                         pEntry->size);
    if (err != Z_OK) {
      UnmapViewOfFile(vpZipView);
      return NULL;
    }
    
    UnmapViewOfFile(retval);
    retval = vpZipView;
    ost = 0;
    *pdwSize = pEntry->data;
  }

  UnmapViewOfFile(vpTemp);

  return (void*)((DWORD)retval + ost);
}

//-------------------------------------------------------------------
//
//
//-------------------------------------------------------------------
void CFileManager::AddPatchFile(const char* filename)
{
  m_patchList.insert(m_patchList.begin(), filename);
}

//-------------------------------------------------------------------
//
//
//-------------------------------------------------------------------
void* CFileManager::LoadPatchedFile(char* path, LPDWORD pdwSize)
{
  FM_PATCH_LIST_ITERATOR i;
  void* res;

  for (i = m_patchList.begin(); i != m_patchList.end(); i++) {
    res = LoadFile(path, pdwSize);

    if (res != NULL) return res;
  }
  return 0;
}




