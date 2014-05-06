//-----------------------------------------------------------------------------
// File: lang.h
//
// Desc: Language definition
//
// Copyright (c) 2002 PlainX Group
// 
//-----------------------------------------------------------------------------

#ifndef __LANG_H__
#define __LANG_H__

#pragma once


// sprite
#define L_SPRITE        "SPRITE"
#define L_RESDATA       "data"
#define L_WPOSXY        "wposxy"
#define L_VISIBLE       "visible"
#define L_SPEED         "speed"
#define L_FRAMES        "frames"
#define L_ROWS          "rows"
#define L_HOTSPOT       "hotspot"
#define L_ALPHA         "alpha"
#define L_BLENDDATA     "blenddata"
#define L_BLENDCOLOR    "blendcolor"
#define L_FRAMEWIDTH    "framewidth"
#define L_ROTATION      "rotation"
#define L_BBOX          "bbox" // left,top,right,bottom
#define L_BCIRCLE       "bcircle" // x,y,radius

// resman
#define L_DATA          "DATA"
#define L_GROUP         "GROUP"
#define L_CKEY          "ckey"
#define L_MEMORY        "memory"
#define L_VIDEOMEM      "video"
#define L_FNAME         "fname"
#define L_ARCHIVE       "archive"
#define L_WIDTH         "width"
#define L_HEIGHT        "height"

// sprman
#define L_SCENE         "SCENE"
#define L_VIEWRECT      "viewrect" // left,top,right,bottom
#define L_CAMERAXY      "cameraxy"
#define L_WWIDTH        "wwidth"
#define L_WHEIGHT       "wheight"
#define L_CLEARCOLOR    "clearcolor"
#define L_ZPOS          "zpos"
#define L_DESC          "description"
#define L_BORDER        "border"
#define L_CELLSIZE      "cellsize"

// fonts
#define L_FONT              "FONT"
#define L_FONTSPRITE        "fontsprite"
#define L_FONTSYSTEM        "fontsystem"
#define L_FONTHEIGHT        "height"
#define L_FONTWEIGHT        "weight"
#define L_FONTTYPES         "types"
#define L_FONTQUALITY       "quality"
#define L_FONTBACKCOLOR     "backcolor"
#define L_FONTCOLOR         "color"
#define L_FONTCHARSET       "charset"
#define L_FONTOPAQUE        "opaque"

// sound
#define L_SFX           "SFX"
#define L_SFXTYPE       "sfxtype"
#define L_SAMPLERATE    "samplerate"
#define L_MAXCHANNELS   "maxchannels"
#define L_INITFLAGS     "initflags"
#define L_SAMPLE        "sample"
#define L_MUSIC         "music"

// effects
#define L_EFFECT        "EFFECT"
#define L_EFS_SPRITE    "sprite"
#define L_EFS_CYCLES    "cycles"
#define L_EFS_SPEED     "speed"
#define L_EFS_SAMPLE    "sample"

// string table section
#define L_STRTABLE "TABLE"

// parser
#define P_DELIMETER     "="
#define P_COMMENT       "#"
#define P_END           "END"
#define P_COMMA         ","
#define P_BEGIN         "BEGIN"

// run-time vars
#define L_RT_USE_VID_MEM "plainx_usevidmem"

// video settings
#define L_VID_VIDEO         "video"
#define L_VID_WIDTH         "width"
#define L_VID_HEIGHT        "height"
#define L_VID_BPP           "bpp"
#define L_VID_SCREENPOS     "screenpos"
#define L_VID_FULLSCREEN    "fullscreen"

// mouse settings
#define L_MS_MOUSE          "mouse"
#define L_MS_HOTSPOT        "hotspot"
#define L_MS_DATA           "data"
#define L_MS_BLENDDATA      "blenddata"
#define L_MS_BLENDCOLOR     "blendcolor"
#define L_MS_FRAMES         "frames"
#define L_MS_ROWS           "rows"
#define L_MS_ANIMSPEED      "animspeed"
#define L_MS_MOVESPEED      "movespeed"

// ddraw settings
#define L_DD_DDRAW          "ddraw"
#define L_DD_USE_VIDEOMEM   "usevideomem"
#define L_DD_USE_HARDBLT    "usehardblt"
#define L_DD_MIN_VIDEOMEM   "minvideomem"

// console settings
#define L_CON_CONSOLE   "console"
#define L_CON_OPAQUE    "opaque"
#define L_CON_BCOLOR    "bcolor"
#define L_CON_FCOLOR    "fcolor"
#define L_CON_FONT      "font"

#endif //__LANG_H__