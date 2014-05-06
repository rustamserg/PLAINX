// ui.h: interface for the user interface classes.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_UI_H__8FEFB03D_F604_41EC_A310_E8DBB8A62828__INCLUDED_)
#define AFX_UI_H__8FEFB03D_F604_41EC_A310_E8DBB8A62828__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "listener.h"
#include "sprite.h"



class CFCell : public CListener
{
public:
	CFCell();
	void Init();
	BOOL ProcessMessage(E_MESSAGE eMessage, DWORD dwCurrTick);

protected:
	void OnClick(int x, int y);

private:
	int      m_cFrame;
	CSprite* m_pFCell[6];
};

/////////////////////////////////////////////////////////////////////
class CTCell : public CListener
{
public:
	CTCell();
	void Init();
	BOOL ProcessMessage(E_MESSAGE eMessage, DWORD dwCurrTick);

protected:
	void OnClick(int x, int y);

private:
	int      m_cPos;
	CSprite* m_pTCell[4][9];
};

/////////////////////////////////////////////////////////////////////
class COptions: public CListener
{
public:
	COptions();

	BOOL ProcessMessage(E_MESSAGE eMessage, DWORD dwCurrTick);
	BOOL ProcessState(E_STATE eState, DWORD dwCurrTick);

private:
	int  m_nX;
	BOOL m_bGrabSoundVol;
	BOOL m_bGrab;
	BOOL m_bDirty;

private:
	BOOL IsUnnamed();
};

#endif // !defined(AFX_UI_H__8FEFB03D_F604_41EC_A310_E8DBB8A62828__INCLUDED_)
