//-----------------------------------------------------------------------------
// File: pxbbox.h
//
// Desc: PlainX bounding box
//
// Copyright (c) 2002 PlainX Group
// 
//-----------------------------------------------------------------------------

#ifndef __PXBBOX_H__
#define __PXBBOX_H__

#pragma once

/*
    World bounding box

    nWLeft,nWTop
    *-----------------------*
    |                       |
    |                       |
    |                       |
    |                       |
    *-----------------------*
    0,0                 nWRight,nWBottom
*/

class CPXBBox
{
public:
    CPXBBox();
    virtual ~CPXBBox();

    void Set(int nWLeft, int nWTop, int nWRight, int nWBottom);
    void Set(int nWX, int nWY, int nRadius);
    
    void Get(int* pnWLeft, int* pnWTop, int* pnWRight, int* pnWBottom);
    void Get(int* pnWX, int* pnWY, int* pnRadius);

    void Move(int nWDX, int nWDY);

    void Clear() { m_bIsClean = TRUE; }
    BOOL IsClean() { return m_bIsClean; }

    BOOL TestPoint(int nWX, int nWY);
    BOOL TestBox(CPXBBox* pTest);

private:
    BOOL m_bIsClean;
    BOOL m_bIsCircle;
    POINT m_ptCenter;
    int m_nRadius;
    int m_nRadQuad;
    RECT m_rcBox;
};

#endif // __PXBBOX_H__
