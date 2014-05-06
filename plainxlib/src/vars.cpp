#include "plainx.h"
#include "vars.h"


CVars::CVars()
{
  m_pParser = NULL;
}


CVars::~CVars()
{
  if (m_pParser) {
    m_pParser->Close();
    delete m_pParser;
    m_pParser = NULL;
  }
}


//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
BOOL CVars::Init(const char* pszPath)
{
  if (pszPath == NULL)
    return FALSE;

  if (m_pParser == NULL) {
    m_pParser = new CParser();
  }
  if (m_pParser == NULL)
    return FALSE;

  if (!m_pParser->Open(pszPath))
    return FALSE;

  return TRUE;
}

//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
BOOL CVars::GetInt(const char* pszGroupLabel,
                   const char* pszGroupName,
                   const char* pszVarName,
                   int nPos,
                   int* pnVarValue)
{
  char pszValue[MAX_BUFFER];

  if (m_pParser->GetVar(pszGroupLabel, pszGroupName, pszVarName, pszValue)) {
    char* pv = pszValue;
    for (int i=0; i<nPos && pv; i++) {
      pv = strstr(pv, P_COMMA);
      if (pv) pv++;
    }
    if (pv) {
      char* pvend = strstr(pv, P_COMMA);
      if (pvend) pvend[0] = 0;
      *pnVarValue = atoi(pv);
      return TRUE;
    }
  }
  return FALSE;
}

//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
BOOL CVars::GetString(const char* pszGroupLabel,
                      const char* pszGroupName,
                      const char* pszVarName,
                      char* pszVarValue)
{
  return m_pParser->GetVar(pszGroupLabel,
						   pszGroupName,
						   pszVarName,
						   pszVarValue);
}


//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
BOOL CVars::SetInt(const char* pszGroupLabel, 
                   const char* pszGroupName, 
                   const char* pszVarName, 
                   int nPos, 
                   int nVarValue)
{
  char pszValue[MAX_BUFFER];
  char szNewValue[MAX_BUFFER];
  char szBuf[MAX_BUFFER];
  
  if (m_pParser->GetVar(pszGroupLabel, pszGroupName, pszVarName, pszValue)) {
    char* pv = pszValue;
    for (int i=0; i<nPos && pv; i++) {
      pv = strstr(pv, P_COMMA);
      pv++;
    }
    if (pv) {
      char* pvend = strstr(pv, P_COMMA);
      pv[0] = 0;
      strcpy(szNewValue, pszValue);
      _itoa(nVarValue, szBuf, 10);
      strcat(szNewValue, szBuf);
      if (pvend) strcat(szNewValue, pvend);

      return m_pParser->SetVar(pszGroupLabel, 
                               pszGroupName, 
                               pszVarName, 
                               szNewValue);
    }
  }
  return FALSE;
}

//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
BOOL CVars::SetString(const char* pszGroupLabel,
                      const char* pszGroupName,
                      const char* pszVarName,
                      char* pszVarValue)
{
	return m_pParser->SetVar(pszGroupLabel,
							 pszGroupName,
							 pszVarName,
							 pszVarValue);
}

//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
BOOL CVars::GetNextGroup(const char* pszGroupLabel,
                         char* pszGroupName,
                         BOOL bIsFirst)
{
  return m_pParser->GetNextGroup(pszGroupLabel,
                                 pszGroupName,
                                 bIsFirst);
}

