// Copyright (c) 1996-1998 Ulrich von Zadow

#ifndef INCL_JPEGDEC
#define INCL_JPEGDEC

#ifndef INCL_PICDEC
#include "picdec.h"
#endif

extern "C"
{
#include "jpeglib.h"
}

//! JPEG file decoder. Uses the independent JPEG group's library
//! to do the actual conversion.
class CJPEGDecoder : public CPicDecoder
{

DECLARE_DYNAMIC (CJPEGDecoder);

public:
  //! Creates a decoder
  CJPEGDecoder
    ();

  //! Destroys a decoder
  ~CJPEGDecoder
    ();

/* I used this function to test the IJPEG rouines without a custom
   data source. If something goes wrong, it may still be useful to
   isolate bugs.
  CBmp * MakeBmpFromFile
    ( char * pszFName
    );
*/

  //! TRUE (the default) selects fast but sloppy decoding.
  void SetFast
    ( BOOL bFast
    );

protected:
  //! Sets up the jpeg library data source and error handler and
  //! calls the jpeg decoder.
  void DoDecode
    ( CBmp * pBmp,
      RGBAPIXEL ** ppPal,
      int * pDestBPP,
      CDataSource * pDataSrc
    );

private:
  // Assumes IJPEG decoder is already set up.
  void decodeGray
    ( CBmp * pBmp,
      int w,
      int h,
      int * pDestBPP
    );

  // Assumes IJPEG decoder is already set up.
  void decodeRGB
    ( CBmp * pBmp,
      int w,
      int h,
      int * pDestBPP
    );

  jpeg_decompress_struct cinfo;  // IJPEG decoder state.
  jpeg_error_mgr         jerr;   // Custom error manager.

  BOOL  m_bFast;  // TRUE selects fast but sloppy decoding.
};

#endif
