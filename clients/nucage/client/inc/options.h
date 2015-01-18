#ifndef OPTDLG_H
#define OPTDLG_H

#include "listener.h"
#include "buttons.h"
#include "chkbox.h"

class COptionsDlg: public CListener
{
public:
    COptionsDlg();
    virtual ~COptionsDlg();

    BOOL ProcessState(E_STATE sState, DWORD dwCurrTick);

public:
	void DrawOptions();
	void MusicUp();
	void MusicDown();
	void SoundUp();
	void SoundDown();
	void SetRusLng();
	void SetEngLng();

private:
	CButton* m_pOKBut;
	CButton* m_pMusicUp;
	CButton* m_pMusicDown;
	CButton* m_pSoundUp;
	CButton* m_pSoundDown;
	CCheckBox* m_pFullScr;
	CCheckBox* m_pMuteAll;
	CCheckBox* m_pRusLng;
	CCheckBox* m_pEngLng;
	int m_iOptionsId;
	int m_iMusicVolId;
	int m_iSoundVolId;
	int m_iMusicVolValId;
	int m_iSoundVolValId;
};

#endif // OPTDLG_H
