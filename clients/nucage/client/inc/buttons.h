// buttons.h: interface for the user interface classes.
//
//////////////////////////////////////////////////////////////////////

#ifndef BUTTONS_H
#define BUTTONS_H

#include "listener.h"

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

    // init button
    void InitButton(const char* pszSpriteName,
                    const char* pcszResName,
                    const char* pcszFontName);

    void DrawButton(int wx, int wy, int wz);
    void HideButton();
    void UnHideButton();

protected:
    virtual void OnClick();

protected:
    BOOL m_bSetLight;
    CSprite* m_pButton;
    int m_iCaptionId;
};


/////////////////////////////////////////////////////////////////////
//
// Classes for different buttons
//
/////////////////////////////////////////////////////////////////////

class CPlayLevel: public CButton
{
public:
    CPlayLevel();

protected:
    void OnClick();
};

class CRetryLevel : public CButton
{
public:
    CRetryLevel();

protected:
    void OnClick();
};

class CNextBut : public CButton
{
public:
    CNextBut();

protected:
    void OnClick();
};

class CBackToMenu : public CButton
{
public:
    CBackToMenu();

protected:
    void OnClick();
};

class CQuitMenu : public CButton
{
public:
    CQuitMenu();

protected:
    void OnClick();
};

class CNextMiniLevel : public CButton
{
public:
    CNextMiniLevel();

protected:
    void OnClick();
};

class CPrevMiniLevel : public CButton
{
public:
    CPrevMiniLevel();

protected:
    void OnClick();
};

class CNextWorld : public CButton
{
public:
    CNextWorld();

protected:
    void OnClick();
};

class CPrevWorld : public CButton
{
public:
    CPrevWorld();

protected:
    void OnClick();
};

class COptionsBut : public CButton
{
public:
	COptionsBut();

protected:
	void OnClick();
};

class CHelpBut : public CButton
{
public:
	CHelpBut();

protected:
	void OnClick();
};

class COKBut : public CButton
{
public:
	COKBut();

protected:
	void OnClick();
};

class CMusicUpBut : public CButton
{
public:
	CMusicUpBut();

protected:
	void OnClick();
};

class CMusicDownBut : public CButton
{
public:
	CMusicDownBut();

protected:
	void OnClick();
};

class CSoundUpBut : public CButton
{
public:
	CSoundUpBut();

protected:
	void OnClick();
};

class CSoundDownBut : public CButton
{
public:
	CSoundDownBut();

protected:
	void OnClick();
};

#endif //BUTTONS_H
