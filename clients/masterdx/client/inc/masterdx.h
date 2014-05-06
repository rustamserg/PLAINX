// masterdx.h: interface for the CMasterDX class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MASTERDX_H__8FEFB03D_F604_41EC_A310_E8DBB8A62828__INCLUDED_)
#define AFX_MASTERDX_H__8FEFB03D_F604_41EC_A310_E8DBB8A62828__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "engine.h"
#include "sprite.h"

class CMasterDX : public CEngine  
{
public:
	BOOL ProcessState(E_STATE sState, DWORD dwCurrTick);
        void RestoreAudio();
};


#endif // !defined(AFX_MASTERDX_H__8FEFB03D_F604_41EC_A310_E8DBB8A62828__INCLUDED_)
