// Copyright (c) 1997-1998 Ulrich von Zadow

#ifndef INCL_WINBMP
#define INCL_WINBMP

#ifndef INCL_BITMAP
#include "bitmap.h"
#endif

//! This is the windows version of CBmp. The internal storage format
//! is a windows DIB. It supports all color depths allowed by
//! windows: 1, 4, 8, 16, 24, and 32 bpp.
//!
//! The subset of the windows DIB format supported is as follows: The
//! DIB is stored so that header, palette, and bits are in one
//! buffer. The bottom line is stored first (biHeight must be > 0)
//! and the data is uncompressed (BI_RGB). Color tables for 16, 24,
//! and 32 bpp are not supported. biClrUsed is always 0. The palette
//! mode is DIB_RGB_COLORS. DIB_PAL_COLORS is not supported.
//!
//! Note that almost all real-life DIBs conform to this subset
//! anyway, so there shouldn't be any problems.
//!
//! <i>In the current version, some functions (notably CreateCopy) only
//! support 1, 8 and 32 bpp. Sorry!</i>
//!
//! 21.01.99, Martin Skinner - I added SaveAsBmp(CFile &) to allow saving the Bmp
//!            to any CFile-Derived i/o-object.
//!            SaveAsBmp(const char *) is still there to ensure back-comp.
//!            but it simply calls SaveAsBmp(CFile &).
class CWinBmp : public CBmp
{

DECLARE_DYNAMIC (CWinBmp);

public:
  // Creates an empty bitmap.
  CWinBmp
    ();

  //! Destroys the bitmap.
  virtual ~CWinBmp
    ();

#ifdef _DEBUG
  virtual void AssertValid
    () const;    // Tests internal object state
#endif

  // CWinBmp manipulation

  // Do a bitblt using the alpha channel of pSrcBmp. Restricted to
  // 32 bpp.
  // Legacy routine. Use the Blt classes instead.
  void AlphaBlt
    ( CWinBmp * pSrcBmp,
      int x,
      int y
    );

  // CWinBmp information

  //! Returns the amount of memory used by the object.
  virtual long GetMemUsed
    ();

  //! Returns number of bytes used per line.
  long GetBytesPerLine
    ();

  // Windows-specific interface

  //! Loads a bitmap from a windows resource (.rc or .res linked to
  //! the exe). Fails if the bitmap is compressed.
  virtual void CreateRes
    ( int ID
    );

  //! Returns the size of the bitmap in pixels
  CSize GetSize
    ();

  //! Access the windows bitmap structure. Using this structure, all
  //! standard DIB manipulations can be performed.
  BITMAPINFOHEADER * GetBMI
    ();

  //! Saves the DIB in windows bmp format.
  void SaveAsBmp
    ( const char * pszFName
    );

  //! Saves the DIB in windows bmp format. rFile must be opened for
  //! writing. rFile remains open after the call. A CFileException
  //! may be thrown by MFC.
  void CWinBmp::SaveAsBmp
    ( CFile & rFile
    );

  // CWinBmp output

  //! Draws the bitmap on the given device context using
  //! StretchDIBits.
  virtual void Draw
    ( CDC* pDC,
      int x,
      int y,
      DWORD rop = SRCCOPY
    );

  //! Draws the bitmap on the given device context using
  //! StretchDIBits. Scales the bitmap by Factor.
  virtual void StretchDraw
    ( CDC* pDC,
      int x,
      int y,
      double Factor,
      DWORD rop = SRCCOPY
    );

  //! Draws a portion of the bitmap on the given device context
  virtual BOOL DrawExtract
    ( CDC* pDC,
      CPoint pntDest,
      CRect rcSrc
    );

  //! Puts a copy of the DIB in the clipboard
  void ToClipBoard
    ();

  // Static functions

  //! Returns memory needed by a bitmap with the specified attributes.
  static long GetMemNeeded
    ( LONG width,
      LONG height,
      WORD BitsPerPixel
    );

  //! Returns memory needed by bitmap bits.
  static long GetBitsMemNeeded
    ( LONG width,
      LONG height,
      WORD BitsPerPixel
    );

protected:

  // Protected callbacks

  //! Create a new empty DIB. Bits are uninitialized.
  //! Assumes that no memory is allocated before the call.
  virtual void internalCreate
    ( LONG Width,
      LONG Height,
      WORD BitsPerPixel,
      BOOL bAlphaChannel
    );

  //! Creates a CWinBmp from an existing bitmap pointer.
  //! Assumes that no memory is allocated before the call.
  virtual void internalCreate
    ( BITMAPINFOHEADER* pBMI
    );

  //! Deletes memory allocated by member variables.
  virtual void freeMembers
    ();

  //! Initializes internal table of line addresses.
  virtual void initLineArray
    ();
  // Creates a copy of the current bitmap in a global memory block
  // and returns a handle to this block.
  virtual HANDLE createCopyHandle
    ();

  // Set color table pointer & pointer to bits based on m_pBMI.
  virtual void initPointers
    ();

  // Member variables.

  BITMAPINFOHEADER * m_pBMI;  // Pointer to picture format information.

private:
};

#endif
