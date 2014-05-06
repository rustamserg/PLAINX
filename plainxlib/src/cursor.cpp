#include "plainx.h"
#include "cursor.h"



CCursor::CCursor() :
CListener()
{
	m_pCursor = NULL;
}


CCursor::~CCursor()
{
}


//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
void CCursor::SetCursor(CSprite* pSprite)
{
	m_pCursor = pSprite;
}


//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
CSprite* CCursor::GetCursor()
{
	return m_pCursor;
}


//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
BOOL CCursor::ProcessMessage(E_MESSAGE eMessage, DWORD dwCurrTick)
{
	POINT ptW;

	switch (eMessage.dwMessageID) 
	{
	case EM_MOUSEMOVE:
		if (m_pCursor) {
			GetScene()->ScreenToWorld(eMessage.lParam, eMessage.wParam, &ptW);
			m_pCursor->SetWXPos(ptW.x);
			m_pCursor->SetWYPos(ptW.y);
		}
		break;
	}
	return FALSE; // we return FALSE because wants that other listeners process this message too
				  // if we return TRUE the message will be deleted from message queue
}
