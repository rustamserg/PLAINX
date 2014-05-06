// Copyright (c) 1996-1998 Ulrich von Zadow

#ifndef INCL_TIFFDEC
#define INCL_TIFFDEC

#ifndef INCL_PICDEC
#include "picdec.h"
#endif

typedef	struct tiff TIFF;

//! TIFF file decoder. Uses LIBTIFF to do the actual conversion.
//! Supports 1, 8 and 24 bits per pixel as output formats.
class CTIFFDecoder : public CPicDecoder
{

DECLARE_DYNAMIC (CTIFFDecoder);

public:
  //! Creates a decoder
  CTIFFDecoder
    ();

  //! Destroys a decoder
  virtual ~CTIFFDecoder
    ();
/*
  CBmp * MakeBmpFromFile
    ( char * pszFName
    );
*/
  // Callback from the TIFF library.
  // Should be accessed only by the TIFF decoder and libtiff.
  static void Win32WarningHandler
    ( const char* module,
      const char* fmt,
      va_list ap
    );

  // Callback from the TIFF library.
  // Should be accessed only by the TIFF decoder and libtiff.
  static void Win32ErrorHandler
    ( const char* module,
      const char* fmt,
      va_list ap
    );

protected:
  //! Sets up LIBTIFF environment and calls LIBTIFF to decode an
  //! image.
  virtual void DoDecode
    ( CBmp * pBmp,
      RGBAPIXEL ** ppPal,
      int * pDestBPP,
      CDataSource * pDataSrc
    );

private:
	// these 2 helpers dedicated to hi/low bit-depths respectively
	void doHiColor
    ( TIFF * tif,
      CBmp * pBmp,
      int * pDestBPP
    );
	void doLoColor
    ( TIFF * tif,
      CBmp * pBmp,
      int * pDestBPP,
      RGBAPIXEL ** ppPal
    );

private:
  // Temporary storage for an error message. Used in the callbacks,
  // so it's static.
  static char m_szLastErr[256];
};

#endif
