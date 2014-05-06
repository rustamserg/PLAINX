//-----------------------------------------------------------------------------
// File: archive.h
//
// Desc: Archive definitions
//
// Copyright (c) 2002 PlainX Group
// 
//-----------------------------------------------------------------------------

#ifndef __ARCHIVE_H__
#define __ARCHIVE_H__

#pragma once

typedef struct Header {
    DWORD id;           // Signature of the archive file. Must be set to ID macro.
    DWORD version;      // Version of the archive file format. See VER, VER2 and VER3 macros.
    DWORD flags;        // Flags that defines format for file format >=2.0.0. In other cases must be GAF_NONE.
    DWORD reserved2;    // Reserved for future use.
} *LPHeader;

#pragma pack(1)
typedef struct Entry {
    char name[13];    // Name of the file or dir.
    DWORD data;       // For GAF_ARCHIVED flag present contains the real file size.
    DWORD reserved2;  // Reserved for future use.
    fpos_t offset;    // Offset in archive file where the file data begins.
    DWORD size;       // Contains size of file in archive.
} *LPEntry;

#define ID      0x66616700
#define VER     0x00000001
#define VER2    0x00000501
#define VER3    0x00000002

// Flag defines for flags field of Header structure
#define GAF_NONE        0
#define GAF_ARCHIVED    1
#define GAF_ENCRYPTED   2
#define GAF_4           4
#define GAF_8           8
#define GAF_16          16
#define GAF_32          32
#define GAF_64          64
#define GAF_128         128
#define GAF_256         256
#define GAF_512         512
#define GAF_1024        1024
#define GAF_2048        2048
#define GAF_4096        4096
#define GAF_8192        8192

#endif // __ARCHIVE_H__
