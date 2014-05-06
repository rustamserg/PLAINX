// Copyright (c) 1996-1998 Ulrich von Zadow

#ifndef INCL_PNGDEC
#define INCL_PNGDEC

#ifndef INCL_PICDEC
#include "picdec.h"
#endif

#ifndef INCL_PNG
#include "PNG.h"
#endif



//! PNG file decoder. Uses LibPng to do the actual decoding.
//! PNG supports many pixel formats not supported by paintlib. These
//! pixel formats are converted to the nearest paintlib equivalent.
//! Images with less or more than 8 bits per channel are converted to 
//! 8 bits per channel. Images with 16-bit palettes or grayscale 
//! images with an alpha channel are returned as full 32-bit RGBA 
//! bitmaps.
class CPNGDecoder : public CPicDecoder
{

DECLARE_DYNAMIC (CPNGDecoder);

public:
  //! Creates a decoder
  CPNGDecoder
    ();

  //! Destroys a decoder
  virtual ~CPNGDecoder
    ();

protected:
  //! Main decoder routine. Reads the header, creates the bitmap,
  //! reads the palette, and reads the image data.
  virtual void DoDecode
    ( CBmp * pBmp,
      RGBAPIXEL ** ppPal,
      int * pDestBPP,
      CDataSource * pDataSrc
    );

private:

  static void CPNGDecoder::user_error_fn
    ( png_structp png_ptr,
      png_const_charp error_msg
    );

  static void CPNGDecoder::user_warning_fn
    ( png_structp png_ptr,
      png_const_charp warning_msg
    );
};

#endif
