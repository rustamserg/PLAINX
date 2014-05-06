// Copyright (c) 1996-1998 Ulrich von Zadow

#ifndef INCL_ANYBMP
#define INCL_ANYBMP

#ifndef INCL_BITMAP
#include "bitmap.h"
#endif

//! Manipulates uncompressed device- and platform-independent bitmaps.
//! Supported data formats are 8 and 32 bpp. The data is stored
//! sequentially without padding in the bitmap. The class implements exactly
//! the interface defined by CBmp without additions.
class CAnyBmp : public CBmp
{

DECLARE_DYNAMIC (CAnyBmp);

public:
  //! Creates an empty bitmap.
  CAnyBmp
    ();

  //! Destroys the bitmap.
  virtual ~CAnyBmp
    ();


  // CAnyBmp information

  //! Returns the amount of memory used by the object.
  long GetMemUsed
    ();

  //! Returns number of bytes used per line.
  long GetBytesPerLine
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


private:
  // Local functions

  // Create a new bitmap with uninitialized bits. Assumes no memory
  // is allocated yet.
  virtual void internalCreate
    ( LONG Width,
      LONG Height,
      WORD BitsPerPixel,
      BOOL bAlphaChannel
    );

  // Deletes memory allocated by member variables.
  virtual void freeMembers
    ();

  // Initializes internal table of line addresses.
  virtual void initLineArray
    ();
};

#endif
