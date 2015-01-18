#ifndef GOBJ_H
#define GOBJ_H

#include "player.h"
#include "gglob.h"


class CGObj 
{
public:
    virtual BOOL IsCanEnter() = 0;
    virtual BOOL IsCanMove(MoveID eDir) { return FALSE; }

    // return FALSE if you want called again
    // TRUE means that event passed
    virtual void OnEnter() {}
    virtual void OnExit() {}
    virtual BOOL OnMoving(MoveID eDir) { return TRUE; }


public:
    MapID m_eID;
    CSprite* m_pSp;
    char* m_pszName;
    int m_iX;
    int m_iY;
};

#endif // GOBJ_H