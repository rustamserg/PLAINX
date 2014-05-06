#include "plainx.h"
#include "listener.h"


CListener::CListener()
{
  m_bEnable = TRUE;
  m_pParent = NULL;
  m_dwLastTick = 0;
}


CListener::~CListener()
{
}


//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
void CListener::Init(CSceneManager* pParent)
{
  m_pParent = pParent;
}


//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
void CListener::PostState(DWORD dwStateID, LPARAM lParam, WPARAM wParam)
{
  E_STATE eState = {dwStateID, lParam, wParam};
  m_qState.push( eState );
}


//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
BOOL CListener::GetState(E_STATE* peState)
{
  if (m_qState.empty())
    return FALSE;

  *peState = m_qState.front();
  
  return TRUE;
}


//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
void CListener::DelState() 
{
  if (!m_qState.empty())
    m_qState.pop();
}


//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
E_STATE CListener::GetState()
{
  E_STATE eState = {ES_IDLE, 0, 0};

  if (m_qState.empty())
    return eState;

  eState = m_qState.front();

  return eState;
}


//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
void CListener::ClearStates()
{
  while (!m_qState.empty())
    m_qState.pop();
}


//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
BOOL CListener::ProcessState(E_STATE eState, DWORD dwCurrTick)
{
  // custom code here
  
  return TRUE;
}


//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
BOOL CListener::ProcessMessage(E_MESSAGE eMessage, DWORD dwCurrTick)
{
  // custom code here

  return FALSE;
}


//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
DWORD CListener::GetMSecDiff(DWORD dwCurrTick)
{
	if (m_dwLastTick == 0) {
		SetLastTick(dwCurrTick);
		return 0;
	}
	return (dwCurrTick - m_dwLastTick);
}
