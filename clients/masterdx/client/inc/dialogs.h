// dialogs.h: interface for the user interface classes.
//
//////////////////////////////////////////////////////////////////////

#ifndef DIALOGS_H
#define DIALOGS_H

#include "listener.h"
#include "sprite.h"


/////////////////////////////////////////////////////////////////////
//
// Class for dialog support
//
/////////////////////////////////////////////////////////////////////
class CDialog : public CListener
{
public:
	CDialog();
	BOOL ProcessMessage(E_MESSAGE eMessage, DWORD dwCurrTick);
	void SetOk(const char* pszName);
	void SetCancel(const char* pszName);
	void SetDialog(const char* pszName);

protected:
	virtual void OnOk();
	virtual void OnCancel();

protected:
	CSprite* m_pOk;
	CSprite* m_pCancel;
	CSprite* m_pDialog;

private:
	BOOL m_bGrab;
	int  m_nX;
	int  m_nY;
};

/////////////////////////////////////////////////////////////////////
class CDialogLW: public CDialog
{
public:
	CDialogLW();

protected:
	void OnOk(); 
};

/////////////////////////////////////////////////////////////////////
class CDialogNagOpt: public CDialog
{
public:
	CDialogNagOpt();

protected:
	void OnOk(); 
};

/////////////////////////////////////////////////////////////////////
class CDialogRegRes: public CDialog
{
public:
	CDialogRegRes();

protected:
	void OnOk();
};

/////////////////////////////////////////////////////////////////////
class CDialogNag: public CDialog
{
public:
	CDialogNag();
	BOOL ProcessState(E_STATE sState, DWORD dwCurrTick);

protected:
	void OnOk();
};

#endif //DIALOGS_H
