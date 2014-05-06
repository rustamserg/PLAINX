//-----------------------------------------------------------------------------
// File: sndcore.h
//
// Desc: Sound core interface
//
// Copyright (c) 2002 PlainX Group
// 
//-----------------------------------------------------------------------------

#ifndef __SNDCORE_H__
#define __SNDCORE_H__

#pragma once


class CSoundCore 
{
public:
    CSoundCore();
    virtual ~CSoundCore();

    virtual BOOL LoadCore(void** context) = 0;
    virtual int UnloadCore(void* context) = 0;

    virtual BOOL OpenCore(int samplerate, int channels, unsigned int flags, void* context) = 0;
    virtual BOOL ReopenCore(int samplerate, int channels, unsigned int flags, void* context);
    virtual void CloseCore(void* context) = 0;

    virtual void* LoadSample(char* buffer, int size, void* context) = 0;
    virtual void* LoadMusic(char* buffer, int size, void* context) = 0;

    virtual void FreeSample(void* handle, void* context) = 0;
    virtual void FreeMusic(void* handle, void* context) = 0;

    virtual void StopMusic(void* handle, void* context) = 0;
    virtual void PlayMusic(void* handle, void* context) = 0;

    virtual void StopSample(void* handle, void* context) = 0;
    virtual void PlaySample(void* handle, void* context) = 0;

    virtual BOOL IsMusicPlaying(void* handle, void* context) = 0;
    virtual BOOL IsMusicFinished(void* handle, void* context) = 0;

    virtual BOOL GetMute(void* context) = 0;
    virtual void SetMute(BOOL mute, void* context) = 0;

    virtual int GetSampleVolume(void* context) = 0;
    virtual void SetSampleVolume(int vol, void* context) = 0;

    virtual int GetMusicVolume(void* handle, void* context) = 0;
    virtual void SetMusicVolume(void* handle, int vol, void* context) = 0;
};

#endif // __SNDCORE_H__





 