#include "plainx.h"
#include "sndcore.h"


CSoundCore::CSoundCore()
{
}


CSoundCore::~CSoundCore()
{
}


//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
BOOL CSoundCore::ReopenCore(int samplerate, int channels, unsigned int flags, void* context)
{
	CloseCore(context);
	return OpenCore(samplerate, channels, flags, context);
}

