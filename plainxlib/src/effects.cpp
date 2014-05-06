#include "plainx.h"
#include "effects.h"

#define ES_EFFECTS_UPDATE 10


CEffects::CEffects()
: CListener()
{
	for (int i = 0; i < EFS_MAX_SPRITES; i++) {
		m_pSprites[i] = NULL;
	}
	m_iNextPos = 0;
}


CEffects::~CEffects()
{
	EFS_HASH_LIST_ITERATOR i;

	for (i = m_effects.begin(); i != m_effects.end(); i++) {
		EFS* pt = (EFS*)(*i).second;
		
		if (pt)
			free(pt);
	}
	m_effects.clear();
}


BOOL CEffects::ProcessState(E_STATE sState, DWORD dwCurrTick)
{
	switch (sState.dwStateID) {
	case ES_LISTENERINIT:
		PostState(ES_EFFECTS_UPDATE, 0, 0);
		break;
	case ES_EFFECTS_UPDATE:
		for (int i = 0; i < EFS_MAX_SPRITES; i++) {
			CSprite* ps = m_pSprites[i];
			if (ps) {
				if (!(ps->GetAnimState() & SS_ANIM)) {
					GetScene()->DelSprite(ps->GetName());
					m_pSprites[i] = NULL;
				}
			}
		}
		return FALSE; // infinite loop
	}

	// return TRUE if we process state and want to process next state
	return TRUE;
}


BOOL CEffects::Add(const char* pcszConfig, const char* pcszEffectName)
{
	CVars var;

	if (!var.Init(pcszConfig))
		return FALSE;

	EFS* pt = (EFS*)malloc(sizeof(EFS));
	if (!pt)
		return FALSE;

	pt->szName[0] = 0;
	pt->szSample[0] = 0;
	pt->szSprite[0] = 0;
	pt->iCycles = 1;
	pt->iSpeed = 10;

	int iValue;
	char buf[MAX_BUFFER];

	// set name
	strcpy(pt->szName, pcszEffectName);

	// get sprite
	if (!var.GetString(L_EFFECT, pcszEffectName, L_EFS_SPRITE, buf)) {
		free(pt);
		return FALSE;
	}
	strcpy(pt->szSprite, buf);

	// get sample [optional]
	if (var.GetString(L_EFFECT, pcszEffectName, L_EFS_SAMPLE, buf)) {
		strcpy(pt->szSample, buf);
	}

	// get cycles [optional] set 0 for infinite loop
	if (var.GetInt(L_EFFECT, pcszEffectName, L_EFS_CYCLES, 0, &iValue)) {
		pt->iCycles = iValue;
	}

	// get animation speed [optional] (fps) default value is 10 fps
	if (var.GetInt(L_EFFECT, pcszEffectName, L_EFS_SPEED, 0, &iValue)) {
		pt->iSpeed = iValue;
	}

	// add to map
	m_effects[pt->szName] = pt;

	return TRUE;
}


void CEffects::Del(const char* pcszEffectName)
{
	EFS* pt = m_effects[pcszEffectName];

	if (pt)
		free(pt);
	
	m_effects.erase(pcszEffectName);
}


void CEffects::Free()
{
	EFS_HASH_LIST_ITERATOR i;

	for (i = m_effects.begin(); i != m_effects.end(); i++) {
		EFS* pt = (EFS*)(*i).second;
		
		if (pt)
			free(pt);
	}
	m_effects.clear();

	for (int j = 0; j < EFS_MAX_SPRITES; j++) {
		CSprite* ps = m_pSprites[j];
		if (ps) {
			GetScene()->DelSprite(ps->GetName());
		}
		m_pSprites[j] = NULL;
	}
	m_iNextPos = 0;
}


void CEffects::Draw(const char* pcszEffectName, int iWX, int iWY, int iWZ)
{
	EFS* pt = m_effects[pcszEffectName];
	if (!pt)
		return;

	// find free cell for effect sprite
	int iPos = -1;

	while (TRUE) {
		for (int i = m_iNextPos; i < EFS_MAX_SPRITES; i++) {
			if (m_pSprites[i] == NULL) {
				iPos = i;
				m_iNextPos = i + 1;
				break;
			}
		}
		if (m_iNextPos == EFS_MAX_SPRITES) {
			m_iNextPos = 0;
			if (iPos == -1) {
				iPos = -2;
				continue;
			}
		}
		break;
	}

	if (iPos < 0)
		return;

	// create sprite
	CSprite* ps = GetScene()->CloneSprite(pt->szSprite);
	if (!ps)
		return;

	// setup sprite
	ps->SetVisible(TRUE);
	ps->SetAnimCycles(pt->iCycles);
	ps->SetFrame(0);
	ps->SetSpeed(pt->iSpeed);
	ps->SetWPos(iWX, iWY, iWZ);

	// add to array
	m_pSprites[iPos] = ps;

	// play sample sound
	if (pt->szSample[0] != 0)
		GET_AUDIO()->PlaySample(pt->szSample);
}

