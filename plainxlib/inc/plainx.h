//-----------------------------------------------------------------------------
// File: plainx.h
//
// Desc: Main include file. Should be included in all plainx files.
//
// Copyright (c) 2002 PlainX Group
// 
//-----------------------------------------------------------------------------

#ifndef __PLAINX_H__
#define __PLAINX_H__

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
#define _CRT_SECURE_NO_WARNINGS

//system headers
#include <windows.h>
#include <windowsx.h>
#include <tchar.h>
#include <stdio.h>
#include <math.h>
#include <mmsystem.h>
#include <direct.h>
#include <time.h>

//dxsdk headers
#define  DIRECTINPUT_VERSION 0x0700
#include "dinput.h"
#include "ddraw.h"
#include "dplay.h"
#include "dplobby.h"

//fmod headers
#include "fmod.h"
#include "fmod_errors.h"

//stl headers
#include <map>
#include <stack>
#include <string>
#include <queue>
#include <list>

// zlib headers
#include "zlib.h"

// paintlib headers
#include "planydec.h"
#include "plwinbmp.h"
#include "plmemsrc.h"

//libs
#ifdef _DEBUG
#pragma comment (lib, "plainxd.lib")
#else
#pragma comment (lib, "plainx.lib")
#endif
#pragma comment (lib, "zlib.lib")
#pragma comment (lib, "ddraw.lib")
#pragma comment (lib, "winmm.lib")
#pragma comment (lib, "dinput.lib")
#pragma comment (lib, "dplayx.lib")
#pragma comment (lib, "dxguid.lib")
#pragma comment (lib, "paintlib.lib")

//global defines
#define MAX_BUFFER      256         // size of various buffers allocated inside engine
#define MAX_FRAMES      256         // maximum numbers of frames for one sprite row
#define L_DEPTH         10          // maximum levels of z-order's scenes
#define DEF_BORDER      2           // number of cells around one world
#define DEF_CELLSIZE    100         // default size of one cell in pixels
#define pi              3.14159387  // quess who ?
#define MAX_FPS         100         // fps limit, just for case

// helpfull COM macros
#define SAFE_DELETE(p)          { if(p) { delete (p);     (p)=NULL; } }
#define SAFE_RELEASE(p)         { if(p) { (p)->Release(); (p)=NULL; } }
#define SAFE_DELETE_ARRAY(p)    { if(p) { delete[] (p);   (p)=NULL; } }

// send ASCII_NULL_CODE in lParam if ASCII code for DIK code doesn't exist
#define ASCII_NULL_CODE 0

// PlainX engine version x.y
#define PX_LOGO "PlainX"
#define PX_MAJOR_VERSION 1
#define PX_MINOR_VERSION 0

// forward declarations
class CVars;
class CEngine;
class CConsole;
class CSprite;
class CParser;
class CEffects;
class CSoundCore;
class CCursor;
class CDisplay;
class CSurface;
class CListener;
class CSceneManager;
class CResourceManager;
class CVideoManager;
class CSoundManager;
class CInputManager;
class CFileManager;
class CStringManager;
class CMapManager;
class CPXBBox;
class CStringTable;

// helpfull macros - use inside listeners only
#define GET_AUDIO()                 GetScene()->GetEngine()->GetAudio()
#define GET_ENGINE()                GetScene()->GetEngine()
#define GET_STRING()                GetScene()->GetStringManager()
#define GET_SPRITE(x)               GetScene()->GetSprite(x)
#define GET_SPRITE2(x, y)           GetScene()->GetEngine()->GetScene(x)->GetSprite(y)
#define GET_LISTENER(x)             GetScene()->GetListener(x)
#define GET_LISTENER2(x, y)         GetScene()->GetEngine()->GetScene(x)->GetListener(y)
#define GET_SCENE(x)                GetScene()->GetEngine()->GetScene(x)
#define ENABLE_LISTENER(x)          GetScene()->GetListener(x)->Enable()
#define ENABLE_LISTENER2(x, y)      GetScene()->GetEngine()->GetScene(x)->GetListener(y)->Enable()
#define DISABLE_LISTENER(x)         GetScene()->GetListener(x)->Disable()
#define DISABLE_LISTENER2(x, y)     GetScene()->GetEngine()->GetScene(x)->GetListener(y)->Disable()
#define GET_TABLE()					GetScene()->GetEngine()->GetStringTable()

//////////////////////////////////////////////////////////////////

// variables support data
struct VAR {
    char szVarName[MAX_BUFFER];
    char szVarValue[MAX_BUFFER];
};

// vars from one group
typedef std::map<std::string, VAR*> VAR_LIST;
typedef VAR_LIST::iterator VAR_LIST_ITERATOR;

// list of groups
struct GROUP_VAR {
    char szGroupName[MAX_BUFFER];
    char szGroupLabel[MAX_BUFFER];
    VAR_LIST vList;
};

// list of groups
typedef std::map<std::string, GROUP_VAR*> GROUP_VAR_LIST;
typedef GROUP_VAR_LIST::iterator GROUP_VAR_LIST_ITERATOR;

/////////////////////////////////////////////////////////////////

// network session info
struct NM_SESSION_INFO {
    char szSessionName[MAX_BUFFER];
    int iMaxPlayers;
    int iCurrentPlayers;
};

// network session statistics
struct NM_SESSIONS_STATS {
    NM_SESSION_INFO* pInfos;
    int iSize;
    int iRealSize;
};

// network message
struct E_NMSG {
    void* pData;
    DWORD dwSize;
};

/////////////////////////////////////////////////////////////////

// resource media type
enum RM_TYPE {
    RMT_VIDEO,
    RMT_SYSTEM,
    RMT_BOTH,
};

// sprite description
struct SPRITE_DATA {
    char    szData[MAX_BUFFER];         // resource data name
    POINT   ptHotSpot;                  // sprite hotspot
    POINT   ptWPos;                     // sprite position if world coords
    int     nZPos;                      // z position
    BOOL    bVisible;                   // is visible
    int     nFrames;                    // number of frames
    int     nRows;                      // number of rows
    int     nFrameWidth[MAX_FRAMES];    // width of frame in pixels
};

/////////////////////////////////////////////////////////////////

// sprite animation state
#define SS_NONE     0x00000001
#define SS_ANIM     0x00000002
#define SS_ROTATE   0x00000004

// pixel in 16/32 bits modes
struct SPIXEL16 {
    BYTE b1;
    BYTE b2;
};

struct SPIXEL32 {
    BYTE b1;
    BYTE b2;
    BYTE b3;
    BYTE b4;
};


/////////////////////////////////////////////////////////////////////////

// state and message structs
struct E_MESSAGE {
    DWORD  dwMessageID;
    LPARAM lParam;
    WPARAM wParam;
};

struct E_STATE {
    DWORD  dwStateID;
    LPARAM lParam;
    WPARAM wParam;
};

// message and state queue
typedef std::queue<E_MESSAGE> E_QMESSAGE;
typedef std::queue<E_STATE> E_QSTATE;

// states id's
#define ES_STARTENGINE      0
#define ES_IDLE             1
#define ES_LISTENERINIT     2
#define ES_USERSTATE        100

//messages id's
#define EM_MOUSEMOVE        0
#define EM_LMOUSECLICK      1
#define EM_RMOUSECLICK      2
#define EM_RMOUSEUP         3
#define EM_RMOUSEDOWN       4
#define EM_LMOUSEUP         5
#define EM_LMOUSEDOWN       6
#define EM_KEYDOWN          7   // lParam - ASCII code if exist, wParam - DIK code
#define EM_KEYPRESSED       8

// messages for network support
#define EM_NMSG_MIN                 10
#define EM_NMSG_DATARECEIVED        11  // receive after message received from other player
#define EM_NMSG_SETPLAYERDATA       12  // message received when set player data was called for player with playerId
#define EM_NMSG_CREATEREMOTEPLAYER  13  // receive when remote player created you receive remote playerId
#define EM_NMSG_DESTROYPLAYER       14  // receive when player destroyed
#define EM_NMSG_CHAT                15  // receive when chat message arived
#define EM_NMSG_MAX                 20

// user messages
#define EM_USERMESSAGE 100

// plainx headers
#include "engine.h"
#include "video.h"
#include "sndman.h"
#include "vars.h"
#include "sndcore.h"
#include "input.h"
#include "console.h"
#include "cursor.h"
#include "display.h"
#include "fileman.h"
#include "parser.h"
#include "listener.h"
#include "surface.h"
#include "sprman.h"
#include "strman.h"
#include "mapman.h"
#include "resman.h"
#include "effects.h"
#include "sprite.h"
#include "lang.h"
#include "netman.h"
#include "log.h"
#include "pxbbox.h"
#include "strtable.h"

#endif // __PLAINX_H__
