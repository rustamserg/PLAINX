//! Contains most system-specific includes and definitions. On windows
//! systems, it corresponds to stdafx.h. On other systems, the
//! appropriate data types and macros are declared here.
//!
//! This file defines the basic data types (BOOL, BYTE, WORD,...) and
//! some debug macros.

#ifdef _WINDOWS

#define VC_EXTRALEAN        // Exclude rarely-used stuff from Windows headers

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>         // MFC support for Windows 95 Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT

#else
// Definitions for non-windows version

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

typedef int                 BOOL;
typedef unsigned char       BYTE;
typedef unsigned short      WORD;         // This is 16 bit.
typedef long                LONG;         // This is 32 bit.
typedef unsigned long       ULONG;

#ifndef FALSE
#define FALSE   0
#endif

#ifndef TRUE
#define TRUE    1
#endif

#ifndef NULL
#define NULL    0
#endif

//! This class is the base class for all objects used in the library.
//! Its definition is empty for all but Microsoft VC++ compiles.
class CObject
{
};

void Trace(char * pszFormat, ...);

#define HIBYTE(w)   ((BYTE) (((WORD) (w) >> 8) & 0xFF))

#ifdef _DEBUG
#define TRACE   ::Trace
#define ASSERT(f)            \
    if (!(f))                \
      {                      \
        TRACE ("Assertion failed at %s, %i", __FILE__, __LINE__); \
        abort();             \
      }

#define ASSERT_VALID(pOb) \
   ((pOb)->AssertValid())

#else // _DEBUG

#define ASSERT(f) {}
#define ASSERT_VALID(pOb) {}

// This will be optimized away in release mode and still allow TRACE
// to take a variable amount of arguments :-).
inline void Trace (const char *, ...) { }
#define TRACE              1 ? (void)0 : ::Trace

#endif // _DEBUG

#endif
