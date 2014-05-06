// buttons.h: interface for the user interface classes.
//
//////////////////////////////////////////////////////////////////////

#ifndef BUTTONS_H
#define BUTTONS_H

#include "listener.h"
#include "sprite.h"

/////////////////////////////////////////////////////////////////////
//
// Class for button support
//
/////////////////////////////////////////////////////////////////////
class CButton : public CListener
{
public:
	CButton();
	virtual ~CButton();

	BOOL ProcessMessage(E_MESSAGE eMessage, DWORD dwCurrTick);
	
	//accessors
	CSprite* GetButton(){return m_pButton;};
	
	//mutators
	void SetButton(const char* pszName);

protected:
	virtual void OnClick(int x, int y);
	
protected:
	BOOL     m_bSetLight;
	CSprite* m_pButton;
};


/////////////////////////////////////////////////////////////////////
//
// Classes for different buttons
//
/////////////////////////////////////////////////////////////////////
class COptBut: public CButton
{
public:
	COptBut();
	BOOL ProcessMessage(E_MESSAGE eMesage, DWORD dwCurrTick);

protected:
	void OnClick(int x, int y);

private:
	BOOL m_bIsOpen;
};

/////////////////////////////////////////////////////////////////////
class CCheck: public CButton
{
public:
	CCheck();

protected:
	void OnClick(int x, int y);
};

/////////////////////////////////////////////////////////////////////
class CNewBtn: public CButton
{
public:
	CNewBtn();
protected:
	void OnClick(int x, int y);
};

/////////////////////////////////////////////////////////////////////
class CCloseBut: public CButton
{
public:
	CCloseBut();
protected:
	void OnClick(int x, int y);
};

/////////////////////////////////////////////////////////////////////
class CMinBut: public CButton
{
public:
	CMinBut();
protected:
	void OnClick(int x, int y);
};

#endif //BUTTONS_H
