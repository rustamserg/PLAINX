#include "plainx.h"
#include "fmodcore.h"


typedef signed char    (F_API * ptrFSOUND_Init)(int mixrate, int maxsoftwarechannels, unsigned int flags);
typedef void           (WINAPI * ptrFSOUND_Close)();
typedef FSOUND_SAMPLE* (WINAPI * ptrFSOUND_Sample_Load)(int index, const char *name_or_data, unsigned int mode, int memlength);
typedef FMUSIC_MODULE* (WINAPI * ptrFMUSIC_LoadSongMemory)(void *data, int length);
typedef void           (WINAPI * ptrFSOUND_Sample_Free)(FSOUND_SAMPLE *sptr);
typedef signed char    (WINAPI * ptrFMUSIC_IsPlaying)(FMUSIC_MODULE *mod);
typedef signed char    (WINAPI * ptrFMUSIC_StopSong)(FMUSIC_MODULE *mod);
typedef signed char    (WINAPI * ptrFMUSIC_FreeSong)(FMUSIC_MODULE *mod);
typedef signed char    (WINAPI * ptrFMUSIC_PlaySong)(FMUSIC_MODULE *mod);
typedef signed char    (WINAPI * ptrFSOUND_GetMute)(int channel);
typedef signed char    (WINAPI * ptrFSOUND_SetMute)(int channel, signed char mute);
typedef int            (WINAPI * ptrFSOUND_PlaySound)(int channel, FSOUND_SAMPLE *sptr);
typedef void           (WINAPI * ptrFSOUND_SetSFXMasterVolume)(int volume);
typedef int            (WINAPI * ptrFSOUND_GetSFXMasterVolume)();
typedef signed char    (WINAPI * ptrFMUSIC_SetMasterVolume)(FMUSIC_MODULE *mod, int volume);
typedef int            (WINAPI * ptrFMUSIC_GetMasterVolume)(FMUSIC_MODULE *mod);
typedef signed char    (WINAPI * ptrFMUSIC_IsFinished)(FMUSIC_MODULE *mod);

static HMODULE g_hFMOD = 0;
static int g_iFMODCounter = 0;

static ptrFSOUND_Init g_pfnFSOUND_Init = 0;
static ptrFSOUND_Close g_pfnFSOUND_Close = 0;
static ptrFSOUND_Sample_Load g_pfnFSOUND_Sample_Load = 0;
static ptrFMUSIC_LoadSongMemory g_pfnFMUSIC_LoadSongMemory = 0;
static ptrFSOUND_Sample_Free g_pfnFSOUND_Sample_Free = 0;
static ptrFMUSIC_IsPlaying g_pfnFMUSIC_IsPlaying = 0;
static ptrFMUSIC_StopSong g_pfnFMUSIC_StopSong = 0;
static ptrFMUSIC_FreeSong g_pfnFMUSIC_FreeSong = 0;
static ptrFMUSIC_PlaySong g_pfnFMUSIC_PlaySong = 0;
static ptrFSOUND_GetMute g_pfnFSOUND_GetMute = 0;
static ptrFSOUND_SetMute g_pfnFSOUND_SetMute = 0;
static ptrFSOUND_PlaySound g_pfnFSOUND_PlaySound = 0;
static ptrFSOUND_SetSFXMasterVolume g_pfnFSOUND_SetSFXMasterVolume = 0;
static ptrFSOUND_GetSFXMasterVolume g_pfnFSOUND_GetSFXMasterVolume = 0;
static ptrFMUSIC_SetMasterVolume g_pfnFMUSIC_SetMasterVolume = 0;
static ptrFMUSIC_GetMasterVolume g_pfnFMUSIC_GetMasterVolume = 0;
static ptrFMUSIC_IsFinished g_pfnFMUSIC_IsFinished = 0;



CFMODCore::CFMODCore()
{
	m_nCounter = 0;
}


CFMODCore::~CFMODCore()
{
}


//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
BOOL CFMODCore::LoadCore(void** context)
{
	g_iFMODCounter++;

	*context = NULL;

	if (g_hFMOD != 0)
		return TRUE;

	g_hFMOD = LoadLibrary("fmod.dll");
	if (g_hFMOD == 0)
		return FALSE;

	g_pfnFSOUND_Init = (ptrFSOUND_Init)GetProcAddress(g_hFMOD, "_FSOUND_Init@12");
	g_pfnFSOUND_Close = (ptrFSOUND_Close)GetProcAddress(g_hFMOD, "_FSOUND_Close@0");
	g_pfnFSOUND_Sample_Load = (ptrFSOUND_Sample_Load)GetProcAddress(g_hFMOD, "_FSOUND_Sample_Load@16");
	g_pfnFMUSIC_LoadSongMemory = (ptrFMUSIC_LoadSongMemory)GetProcAddress(g_hFMOD, "_FMUSIC_LoadSongMemory@8");
	g_pfnFSOUND_Sample_Free = (ptrFSOUND_Sample_Free)GetProcAddress(g_hFMOD, "_FSOUND_Sample_Free@4");
	g_pfnFMUSIC_IsPlaying = (ptrFMUSIC_IsPlaying)GetProcAddress(g_hFMOD, "_FMUSIC_IsPlaying@4");
	g_pfnFMUSIC_StopSong = (ptrFMUSIC_StopSong)GetProcAddress(g_hFMOD, "_FMUSIC_StopSong@4");
	g_pfnFMUSIC_FreeSong = (ptrFMUSIC_FreeSong)GetProcAddress(g_hFMOD, "_FMUSIC_FreeSong@4");
	g_pfnFMUSIC_PlaySong = (ptrFMUSIC_PlaySong)GetProcAddress(g_hFMOD, "_FMUSIC_PlaySong@4");
	g_pfnFSOUND_GetMute = (ptrFSOUND_GetMute)GetProcAddress(g_hFMOD, "_FSOUND_GetMute@4");
	g_pfnFSOUND_SetMute = (ptrFSOUND_SetMute)GetProcAddress(g_hFMOD, "_FSOUND_SetMute@8");
	g_pfnFSOUND_PlaySound = (ptrFSOUND_PlaySound)GetProcAddress(g_hFMOD, "_FSOUND_PlaySound@8");
	g_pfnFSOUND_SetSFXMasterVolume = (ptrFSOUND_SetSFXMasterVolume)GetProcAddress(g_hFMOD, "_FSOUND_SetSFXMasterVolume@4");
	g_pfnFSOUND_GetSFXMasterVolume = (ptrFSOUND_GetSFXMasterVolume)GetProcAddress(g_hFMOD, "_FSOUND_GetSFXMasterVolume@0");
	g_pfnFMUSIC_SetMasterVolume = (ptrFMUSIC_SetMasterVolume)GetProcAddress(g_hFMOD, "_FMUSIC_SetMasterVolume@8");
	g_pfnFMUSIC_GetMasterVolume = (ptrFMUSIC_GetMasterVolume)GetProcAddress(g_hFMOD, "_FMUSIC_GetMasterVolume@4");
	g_pfnFMUSIC_IsFinished = (ptrFMUSIC_IsFinished)GetProcAddress(g_hFMOD, "_FMUSIC_IsFinished@4");

	if (g_pfnFSOUND_Init == 0 ||
		g_pfnFSOUND_Close == 0 ||
		g_pfnFSOUND_Sample_Load == 0 ||
		g_pfnFMUSIC_LoadSongMemory == 0 ||
		g_pfnFSOUND_Sample_Free == 0 ||
		g_pfnFMUSIC_IsPlaying == 0 ||
		g_pfnFMUSIC_StopSong == 0 ||
		g_pfnFMUSIC_FreeSong == 0 ||
		g_pfnFMUSIC_PlaySong == 0 ||
		g_pfnFSOUND_GetMute == 0 ||
		g_pfnFSOUND_SetMute == 0 ||
		g_pfnFSOUND_PlaySound == 0 ||
		g_pfnFSOUND_SetSFXMasterVolume == 0 ||
		g_pfnFSOUND_GetSFXMasterVolume == 0 ||
		g_pfnFMUSIC_SetMasterVolume == 0 ||
		g_pfnFMUSIC_GetMasterVolume == 0 ||
		g_pfnFMUSIC_IsFinished == 0)
	{
		return FALSE;
	}

	// all ok
	return TRUE;
}


//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
int CFMODCore::UnloadCore(void* context)
{
	g_iFMODCounter--;

	if (g_iFMODCounter > 0)
		return g_iFMODCounter;
  
	if (g_hFMOD != 0) {
	    FreeLibrary(g_hFMOD);
		g_hFMOD = 0;
	}
  
	g_pfnFSOUND_Init = 0;
	g_pfnFSOUND_Close = 0;
	g_pfnFSOUND_Sample_Load = 0;
	g_pfnFMUSIC_LoadSongMemory = 0;
	g_pfnFSOUND_Sample_Free = 0;
	g_pfnFMUSIC_IsPlaying = 0;
	g_pfnFMUSIC_StopSong = 0;
	g_pfnFMUSIC_FreeSong = 0;
	g_pfnFMUSIC_PlaySong = 0;
	g_pfnFSOUND_GetMute = 0;
	g_pfnFSOUND_SetMute = 0;
	g_pfnFSOUND_PlaySound = 0;
	g_pfnFSOUND_SetSFXMasterVolume = 0;
	g_pfnFSOUND_GetSFXMasterVolume = 0;
	g_pfnFMUSIC_SetMasterVolume = 0;
	g_pfnFMUSIC_GetMasterVolume = 0;
	g_pfnFMUSIC_IsFinished = 0;

	g_iFMODCounter = 0;

	return g_iFMODCounter;
}


//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
BOOL CFMODCore::OpenCore(int samplerate, int channels, unsigned int flags, void* context)
{
	if (!g_pfnFSOUND_Init(samplerate, channels, flags))
		return FALSE;

	return TRUE;
}


//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
void CFMODCore::CloseCore(void* context)
{
	g_pfnFSOUND_Close();
}


//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
void* CFMODCore::LoadSample(char* buffer, int size, void* context)
{
	return (void*)g_pfnFSOUND_Sample_Load(m_nCounter++, buffer, 
											FSOUND_16BITS | FSOUND_STEREO | FSOUND_LOADMEMORY,
											size);
}


//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
void* CFMODCore::LoadMusic(char* buffer, int size, void* context)
{
	return (void*)g_pfnFMUSIC_LoadSongMemory(buffer, size);
}


//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
void CFMODCore::FreeSample(void* handle, void* context)
{      
	g_pfnFSOUND_Sample_Free((FSOUND_SAMPLE*)handle);
}


//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
void CFMODCore::FreeMusic(void* handle, void* context)
{
	g_pfnFMUSIC_FreeSong((FMUSIC_MODULE*)handle);
}


//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
void CFMODCore::StopMusic(void* handle, void* context)
{
	g_pfnFMUSIC_StopSong((FMUSIC_MODULE*)handle);
}


//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
void CFMODCore::PlayMusic(void* handle, void* context)
{
	g_pfnFMUSIC_PlaySong((FMUSIC_MODULE*)handle);
}


//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
void CFMODCore::StopSample(void* handle, void* context)
{
	// not implemented
}


//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
void CFMODCore::PlaySample(void* handle, void* context)
{
	g_pfnFSOUND_PlaySound(FSOUND_FREE, (FSOUND_SAMPLE*)handle);
}


//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
BOOL CFMODCore::IsMusicPlaying(void* handle, void* context)
{
	return g_pfnFMUSIC_IsPlaying((FMUSIC_MODULE*)handle);
}


//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
BOOL CFMODCore::IsMusicFinished(void* handle, void* context)
{
	return g_pfnFMUSIC_IsFinished((FMUSIC_MODULE*)handle);
}


//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
BOOL CFMODCore::GetMute(void* context)
{
	return g_pfnFSOUND_GetMute(0);
}


//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
void CFMODCore::SetMute(BOOL mute, void* context)
{
	g_pfnFSOUND_SetMute(FSOUND_ALL, mute);
}


//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
int CFMODCore::GetSampleVolume(void* context)
{
	return g_pfnFSOUND_GetSFXMasterVolume();
}


//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
void CFMODCore::SetSampleVolume(int vol, void* context)
{
	g_pfnFSOUND_SetSFXMasterVolume(vol);
}


//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
int CFMODCore::GetMusicVolume(void* handle, void* context)
{
	return g_pfnFMUSIC_GetMasterVolume((FMUSIC_MODULE*)handle);
}


//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
void CFMODCore::SetMusicVolume(void* handle, int vol, void* context)
{
	g_pfnFMUSIC_SetMasterVolume((FMUSIC_MODULE*)handle, vol);
}


