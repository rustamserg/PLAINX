#include "plainx.h"
#include "log.h"

FILE* flog = NULL;

char logn[MAX_BUFFER];

extern HWND g_hWnd;


//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
void PX_LogOpen(char* logname)
{
	flog = fopen(logname, "w");

	if (flog) {
		strcpy(logn, logname);
		PX_LogWriteString(PX_GetVersion());
	}
}


//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
void PX_LogClose()
{
	if (flog == NULL)
		return;

	fclose(flog);
}


//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
void PX_LogWriteString(const char* message)
{
	if (flog == NULL)
		return;

	fprintf(flog, "%s\n", message);
	fflush(flog);
}


//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
void PX_LogWriteInteger(const char* message, int value)
{
	if (flog == NULL)
		return;

	fprintf(flog, "%s = %i\n", message, value);
	fflush(flog);
}


//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
void PX_ErrorHandle(const char* pszError, BOOL bTerminate)
{
	char szErrorMessage[MAX_BUFFER];

	wsprintf(szErrorMessage, "Internal error.\n%s.", pszError);
	PX_LogWriteString(szErrorMessage);

	MessageBox(g_hWnd, szErrorMessage, PX_GetVersion(), MB_OK | MB_ICONWARNING);

	if (bTerminate) 
		PostMessage(g_hWnd, WM_CLOSE, 0, 0);
}


//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
const char* PX_GetVersion()
{
	static char szVersion[100];
	char szBuf[MAX_BUFFER];

	strcpy(szVersion, "Game engine v");
	strcat(szVersion, _itoa(PX_MAJOR_VERSION, szBuf, 10));
	strcat(szVersion, ".");
	strcat(szVersion, _itoa(PX_MINOR_VERSION, szBuf, 10));

	return szVersion;
}
