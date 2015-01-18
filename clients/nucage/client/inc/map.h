#ifndef MAP_H
#define MAP_H

#include "gobj.h"


class CMap
{
public:
    CMap();
    virtual ~CMap();

    BOOL LoadMap(const char* pcszName);
    void ClearMap();

    int GetTime() { return m_Level.iTime; }
    const char* GetName() { return m_Level.szName; }
    const char* GetSkin() { return m_Level.szSkin; }
    CGObj* GetElement(int x, int y);
    void SetElement(int x, int y, CGObj* pObj);

private:
    MapID GetEncryptCode(BYTE db);

private:
    struct Map {
        char szName[101];
        char szSkin[9];
        int iTime;
        BYTE pData[MAP_WIDTH * MAP_HEIGHT];
    };

private:
    BYTE m_Key[256];
    Map m_Level;
    CGObj* m_Map[MAP_WIDTH][MAP_HEIGHT];
    CGObj* m_pBorder;
};

#endif // MAP_H 