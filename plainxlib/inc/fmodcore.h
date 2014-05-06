//-----------------------------------------------------------------------------
// File: fmodcore.h
//
// Desc: FMOD support
//
// Copyright (c) 2002 PlainX Group
// 
//-----------------------------------------------------------------------------

#ifndef __FMODCORE_H__
#define __FMODCORE_H__

#pragma once

#include "sndcore.h"


class CFMODCore : public CSoundCore
{
public:
    CFMODCore();
    virtual ~CFMODCore();

    BOOL LoadCore(void** context);
    int UnloadCore(void* context);

    BOOL OpenCore(int samplerate, int channels, unsigned int flags, void* context);
    void CloseCore(void* context);

    void* LoadSample(char* buffer, int size, void* context);
    void* LoadMusic(char* buffer, int size, void* context);

    void FreeSample(void* handle, void* context);
    void FreeMusic(void* handle, void* context);

    void StopMusic(void* handle, void* context);
    void PlayMusic(void* handle, void* context);

    void StopSample(void* handle, void* context);
    void PlaySample(void* handle, void* context);

    BOOL IsMusicPlaying(void* handle, void* context);
    BOOL IsMusicFinished(void* handle, void* context);

    BOOL GetMute(void* context);
    void SetMute(BOOL mute, void* context);

    int GetSampleVolume(void* context);
    void SetSampleVolume(int vol, void* context);

    int GetMusicVolume(void* handle, void* context);
    void SetMusicVolume(void* handle, int vol, void* context);

private:
    int m_nCounter;
};

#endif // __SNDCORE_H__





 