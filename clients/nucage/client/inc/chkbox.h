// chkbox.h: interface for the user interface classes.
//
//////////////////////////////////////////////////////////////////////

#ifndef CHKBOX_H
#define CHKBOX_H

#include "listener.h"

/////////////////////////////////////////////////////////////////////
//
// Class for check box support
//
/////////////////////////////////////////////////////////////////////
class CCheckBox : public CListener
{
public:
    CCheckBox();
    virtual ~CCheckBox();

    BOOL ProcessMessage(E_MESSAGE eMessage, DWORD dwCurrTick);

    // init check box
    void InitCheckBox(const char* pszSpriteName,
					  const char* pcszResName,
					  const char* pcszFontName);

    void DrawCheckBox(int wx, int wy, int wz);
	void SetState(BOOL fChecked);
    void HideCheckBox();
    void UnHideCheckBox();

protected:
    virtual void OnClick(BOOL fChecked);

protected:
    BOOL m_fChecked;
    CSprite* m_pCheckBox;
    int m_iLabelId;
};


/////////////////////////////////////////////////////////////////////
//
// Classes for different check boxes
//
/////////////////////////////////////////////////////////////////////

class CFullScreen: public CCheckBox
{
public:
    CFullScreen();

protected:
    void OnClick(BOOL fChecked);
};

class CMuteAllSnd : public CCheckBox
{
public:
    CMuteAllSnd();

protected:
    void OnClick(BOOL fChecked);
};

class CRusLng : public CCheckBox
{
public:
    CRusLng();

protected:
    void OnClick(BOOL fChecked);
};

class CEngLng : public CCheckBox
{
public:
    CEngLng();

protected:
    void OnClick(BOOL fChecked);
};

#endif // CHKBOX_H
