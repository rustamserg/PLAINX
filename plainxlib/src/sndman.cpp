#include "plainx.h"
#include "sndman.h"

// default values for FMOD
#define SND_SAMPLERATE 48000
#define SND_MAXCHANNELS 32
#define SND_INITFLAGS 0


CSoundManager::CSoundManager()
{
  m_pVars = NULL;
  m_pMedia = NULL;

  strcpy(m_szGroup, "");
  strcpy(m_szConfig, "");

  m_pParent = NULL;
  m_pSndCore = NULL;
  m_pContext = NULL;

  m_bCoreLoaded = FALSE;
}


CSoundManager::~CSoundManager()
{
}


//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
int CSoundManager::Init(CEngine* pEngine, CSoundCore* pSndCore)
{
  m_pParent = pEngine;
  m_pSndCore = pSndCore;
  
  if (!m_pSndCore->LoadCore(&m_pContext))
	  return SM_ERR_FAILED_LOAD_CORE;

  m_bCoreLoaded = TRUE;

  if (!m_pSndCore->OpenCore(SND_SAMPLERATE, SND_MAXCHANNELS, SND_INITFLAGS, m_pContext))
    return SM_ERR_FAILED;

  return SM_ERR_OK;
}


//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
void CSoundManager::Free()
{
  if (!m_bCoreLoaded)
    return;

  FreeGroup();
  m_pSndCore->CloseCore(m_pContext);
    
  m_pParent = NULL;
  if (m_pMedia) 
  {
    delete m_pMedia;
    m_pMedia = NULL;
  }
  if (m_pVars) 
  {
    delete m_pVars;
    m_pVars = NULL;
  }
  strcpy(m_szGroup, "");
  strcpy(m_szConfig, "");

  if (m_pSndCore->UnloadCore(m_pContext) == 0)
	  m_bCoreLoaded = FALSE;
}


//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
BOOL CSoundManager::LoadGroup(const char* pcszConfig, const char* pcszMediaGroup)
{
  if (!m_bCoreLoaded)
    return TRUE;

  int samplerate, maxch, flags;

  FreeGroup();
  
  strcpy(m_szConfig, pcszConfig);
  strcpy(m_szGroup, pcszMediaGroup);

  if (m_pMedia == NULL) {
    m_pMedia = new CFileManager();
  }
  if (m_pVars == NULL) {
    m_pVars = new CVars();
  }
  if (m_pMedia == NULL || m_pVars == NULL)
    return FALSE;

  if (!m_pVars->Init(m_szConfig))
    return FALSE;

  //prepare buffer
  char buffer[MAX_BUFFER];

  //read archive
  if (!m_pVars->GetString(L_GROUP, m_szGroup, L_ARCHIVE, buffer)) {
    if (!m_pMedia->Init(m_pParent->GetRoot()))
      return FALSE;
  } else {
    if (!m_pMedia->Init(m_pParent->GetRoot(), buffer))
      return FALSE;
  }
  
  // read audio settings
  if (!m_pVars->GetInt(L_GROUP, m_szGroup, L_SAMPLERATE, 0, &samplerate)) {
    samplerate = SND_SAMPLERATE;
  }
  if (!m_pVars->GetInt(L_GROUP, m_szGroup, L_MAXCHANNELS, 0, &maxch)) {
    maxch = SND_MAXCHANNELS;
  }
  if (!m_pVars->GetInt(L_GROUP, m_szGroup, L_INITFLAGS, 0, &flags)) {
    flags = SND_INITFLAGS;
  }
  
  //reopen fsound
  if (!m_pSndCore->ReopenCore(samplerate, maxch, flags, m_pContext)) {
    return FALSE;
  }

  //load samples and music
  
  //read first data
  if (m_pVars->GetNextGroup(L_SFX, buffer, TRUE)) {
    if (!sndm_CreateData(buffer)) {
      return FALSE;
    }
    while (m_pVars->GetNextGroup(L_SFX, buffer)) {
      if (!sndm_CreateData(buffer)) {
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
BOOL CSoundManager::sndm_CreateData(const char* pszName)
{
  //create data struct
  SND_DATA* snd = (SND_DATA*)malloc(sizeof(SND_DATA));
  memset(snd, 0, sizeof(SND_DATA));
	
  strcpy(snd->szName, pszName);

  //prepare buffer
  char buffer[MAX_BUFFER];

  //read filename
  if (!m_pVars->GetString(L_SFX, pszName, L_FNAME, buffer)) {
    free(snd);
    return FALSE;
  }
  strcpy(snd->szPath, buffer);
  
  //read type
  if (m_pVars->GetString(L_SFX, pszName, L_SFXTYPE, buffer)) {
    strcpy(snd->szType, buffer);
  } else {
    strcpy(snd->szType, L_SAMPLE);
  }
  
  //map media
  DWORD dwSize;
  char* pMedia = (char*)m_pMedia->LoadFile(snd->szPath, &dwSize);
  if (pMedia == NULL) {
    free(snd);
    return FALSE;
  }

  //load media
  if (strcmp(snd->szType, L_SAMPLE) == 0) {
    snd->ph = m_pSndCore->LoadSample(pMedia, dwSize, m_pContext);
  } else {
    snd->ph = m_pSndCore->LoadMusic(pMedia, dwSize, m_pContext);
  }
  
  //check if load
  if (!snd->ph) {
    free(snd);
    return FALSE;
  }

  //add to hash
  if (strcmp(snd->szType, L_SAMPLE) == 0) {
    m_hmSamples[snd->szName] = snd;
  } else {
    m_hmMusics[snd->szName] = snd;
  }
  return TRUE;
}


//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
void CSoundManager::FreeGroup()
{
  if (!m_bCoreLoaded)
    return;

  if (strcmp(m_szGroup, "") == 0)
    return;

  SAMPLES_HASH_MAP_ITER iter;
  
  for (iter = m_hmSamples.begin(); iter != m_hmSamples.end(); iter++) {
    SND_DATA* snd = (SND_DATA*)(*iter).second;
    if (snd) {
		m_pSndCore->FreeSample(snd->ph, m_pContext);
      free(snd);
    }
  }
  m_hmSamples.clear();

  for (iter = m_hmMusics.begin(); iter != m_hmMusics.end(); iter++) {
    SND_DATA* snd = (SND_DATA*)(*iter).second;
    if (snd) {
      if (m_pSndCore->IsMusicPlaying(snd->ph, m_pContext)) {
        m_pSndCore->StopMusic(snd->ph, m_pContext);
      }
      m_pSndCore->FreeMusic(snd->ph, m_pContext);
      free(snd);
    }
  }
  m_hmMusics.clear();
  
  strcpy(m_szGroup, "");
}


//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
void CSoundManager::PlaySample(const char* pcszSampleName)
{
  if (!m_bCoreLoaded)
    return;

  SND_DATA* pData;

  if (!m_pSndCore->GetMute(m_pContext)) {
    if (sndm_GetData(pcszSampleName, L_SAMPLE, &pData)) {
      m_pSndCore->PlaySample(pData->ph, m_pContext);
    }
  }
}


//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
void CSoundManager::PlayMusic(const char* pcszMusicName)
{
  if (!m_bCoreLoaded)
    return;

  SND_DATA* pData;

  if (!m_pSndCore->GetMute(m_pContext)) {
    if (sndm_GetData(pcszMusicName, L_MUSIC, &pData)) {
      m_pSndCore->PlayMusic(pData->ph, m_pContext);
    }
  }
}


//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
void CSoundManager::StopMusic(const char* pcszMusicName)
{
  if (!m_bCoreLoaded)
    return;

  SND_DATA* pData;

  if (!m_pSndCore->GetMute(m_pContext)) {
    if (sndm_GetData(pcszMusicName, L_MUSIC, &pData)) {
      m_pSndCore->StopMusic(pData->ph, m_pContext);
    }
  }
}


//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
void CSoundManager::SetSFXVolume(int nVolume)
{
  if (!m_bCoreLoaded)
    return;

  m_pSndCore->SetSampleVolume(nVolume, m_pContext);
}


//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
int CSoundManager::GetSFXVolume()
{
  if (!m_bCoreLoaded)
    return 0;

  return m_pSndCore->GetSampleVolume(m_pContext);
}


//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
void CSoundManager::SetMusicVolume(const char* pcszMusicName, int nVolume)
{
  if (!m_bCoreLoaded)
    return;

  SND_DATA* pData;

  if (!m_pSndCore->GetMute(m_pContext)) {
    if (sndm_GetData(pcszMusicName, L_MUSIC, &pData)) {
      m_pSndCore->SetMusicVolume(pData->ph, nVolume, m_pContext);
    }
  }
}


//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
int CSoundManager::GetMusicVolume(const char* pcszMusicName)
{
  if (!m_bCoreLoaded)
    return 0;

  SND_DATA* pData;
  
  if (!m_pSndCore->GetMute(m_pContext)) {
    if (sndm_GetData(pcszMusicName, L_MUSIC, &pData)) {
      return m_pSndCore->GetMusicVolume(pData->ph, m_pContext);
    }
  }
  return 0;
}


//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
void CSoundManager::SetMute(BOOL bIsMute)
{
  if (!m_bCoreLoaded)
    return;

  m_pSndCore->SetMute(bIsMute, m_pContext);
}


//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
BOOL CSoundManager::GetMute()
{
  if (!m_bCoreLoaded)
    return TRUE;

  return m_pSndCore->GetMute(m_pContext);
}


//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
BOOL CSoundManager::IsMusicPlay(const char* pcszMusicName)
{
  if (!m_bCoreLoaded)
    return FALSE;

  SND_DATA* pData;

  if (!m_pSndCore->GetMute(m_pContext)) {
    if (sndm_GetData(pcszMusicName, L_MUSIC, &pData)) {
      return !(m_pSndCore->IsMusicFinished(pData->ph, m_pContext));
    }
  }
  return FALSE;
}


//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
BOOL CSoundManager::sndm_GetData(const char* pszName, const char* pszType, SND_DATA** ppData)
{
  SND_DATA* pData;

  *ppData = NULL;
  if (strcmp(pszType, L_SAMPLE) == 0) {
    pData = m_hmSamples[pszName];
  } else {
    pData = m_hmMusics[pszName];
  }
  if (!pData) {
    return FALSE;
  }
  *ppData = pData;
  return TRUE;
}


