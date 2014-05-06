//-----------------------------------------------------------------------------
// File: sndman.h
//
// Desc: Sound manager interface
//
// Copyright (c) 2002 PlainX Group
// 
//-----------------------------------------------------------------------------

#ifndef __SNDMAN_H__
#define __SNDMAN_H__

#pragma once

#define SM_ERR_OK 0
#define SM_ERR_FAILED_LOAD_CORE 1
#define SM_ERR_FAILED 2


class CSoundManager
{
public:
    CSoundManager();
    virtual ~CSoundManager();

    // by default PlainX uses FMOD sound core
    int Init(CEngine* pEngine, CSoundCore* pSndCore);
    void Free();

    BOOL LoadGroup(const char* pcszConfig, const char* pcszMediaGroup);
    void FreeGroup();

    void PlaySample(const char* pcszSampleName);
    void PlayMusic(const char* pcszMusicName);
    void StopMusic(const char* pcszMusicName);

    void SetSFXVolume(int nVolume);
    int GetSFXVolume();

    void SetMusicVolume(const char* pcszMusicName, int nVolume);
    int GetMusicVolume(const char* pcszMusicName);

    void SetMute(BOOL bIsMute);
    BOOL GetMute();
    BOOL IsMusicPlay(const char* pcszMusicName);

private:
    typedef struct {
        void* ph;
        char szName[MAX_BUFFER];
        char szPath[MAX_BUFFER];
        char szType[MAX_BUFFER];
    } SND_DATA;

    typedef std::map<std::string, SND_DATA*> SAMPLES_HASH_MAP;
    typedef SAMPLES_HASH_MAP::iterator SAMPLES_HASH_MAP_ITER;

private:
    CFileManager*       m_pMedia;
    SAMPLES_HASH_MAP    m_hmSamples;
    SAMPLES_HASH_MAP    m_hmMusics;
    CVars*              m_pVars;
    char                m_szGroup[MAX_BUFFER];
    char                m_szConfig[MAX_BUFFER];
    CEngine*            m_pParent;
    CSoundCore*         m_pSndCore;
    BOOL                m_bCoreLoaded;
    void*               m_pContext;

private:
    BOOL sndm_CreateData(const char* pszName);
    BOOL sndm_GetData(const char* pszName, const char* pszType, SND_DATA** ppData);
};

#endif //__SNDMAN_H__