#include "plainx.h"
#include "strtable.h"


CStringTable::CStringTable()
{
	m_pVars = NULL;
	m_szLocale[0] = 0;
	m_szBuffer[0] = 0;
	m_fInited = FALSE;
}


CStringTable::~CStringTable()
{
	if (m_pVars != NULL) {
		delete m_pVars;
		m_pVars = NULL;
	}
}


//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
BOOL CStringTable::Init(const char* pcszConfig)
{
	BOOL fRes;

	m_pVars = new CVars();
	if (m_pVars == NULL)
		return FALSE;

	fRes = m_pVars->Init(pcszConfig);
	if (!fRes)
		return FALSE;

	fRes = m_pVars->GetNextGroup(L_STRTABLE, m_szLocale, TRUE);
	if (!fRes)
		return FALSE;

	m_fInited = TRUE;
	return TRUE;
}


//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
BOOL CStringTable::SetLocale(const char* pcszLocale)
{
	BOOL fRes;

	if (!m_fInited)
		return FALSE;

	m_fInited = FALSE;

	fRes = m_pVars->GetNextGroup(L_STRTABLE, m_szLocale, TRUE);
	while (fRes) {
		if (strcmp(pcszLocale, m_szLocale) == 0) {
			m_fInited = TRUE;
			break;
		}
		fRes = m_pVars->GetNextGroup(L_STRTABLE, m_szLocale);
	}
	return m_fInited;
}


//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
const char* CStringTable::GetLocale()
{
	return m_szLocale;
}


//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
const char* CStringTable::GetString(const char* pcszVarName)
{
	BOOL fRes;

	if (!m_fInited)
		return m_szBuffer;

	fRes = m_pVars->GetString(L_STRTABLE, m_szLocale, pcszVarName, m_szBuffer);
	if (!fRes) {
		m_szBuffer[0] = 0;
	}
	return m_szBuffer;
}

