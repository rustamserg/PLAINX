// Copyright (c) 1996-1998 Ulrich von Zadow

#ifndef INCL_BMPDEC
#define INCL_BMPDEC

#ifndef INCL_PICDEC
#include "picdec.h"
#endif

#ifndef INCL_WINDEFS
#include "windefs.h"
#endif

//! Windows bitmap file decoder. Decodes 1, 4, 8, and 24 bpp
//! bitmap files (compressed and uncompressed). Returns an 8
//! or 32 bpp bitmap.
class CBmpDecoder : public CPicDecoder
{

DECLARE_DYNAMIC (CBmpDecoder);

public:
  //! Creates a decoder
  CBmpDecoder
    ();

  //! Destroys a decoder
  virtual ~CBmpDecoder
    ();

protected:
  //! Does the actual decoding of bitmap data.
  virtual void DoDecode
    ( CBmp * pBmp,
      RGBAPIXEL ** ppPal,
      int * pDestBPP,
      CDataSource * pDataSrc
    );

private:

  // Decodes the bitmap file & info headers
  WINBITMAPINFOHEADER * getInfoHeader
    ( BYTE ** ppBits,     // Used to return location of bitmap bits.
      CDataSource * pDataSrc,
      RGBAPIXEL ** ppPal
    );

  // Decodes a 2-color bitmap. Ignores the palette & just uses
  // black & white as 'colors'
  void decode1bpp
    ( BYTE * pBits,
      CBmp * pBmp,
      int DestBPP
    );

  // Decodes an uncompressed 16-color-bitmap.
  void decode4bpp
    ( BYTE * pBits,
      CBmp * pBmp,
      RGBAPIXEL * pPal,
      int DestBPP
    );

  // Decodes an uncompressed 256-color-bitmap.
  void decode8bpp
    ( BYTE * pBits,
      CBmp * pBmp,
      RGBAPIXEL * pPal,
      int DestBPP
    );

  // Decodes a compressed 16-color-bitmap.
  void decodeRLE4
    ( BYTE * pBits,
      CBmp * pBmp,
      RGBAPIXEL * pPal,
      int DestBPP
    );

  // Decodes a compressed 256-color-bitmap.
  void decodeRLE8
    ( BYTE * pBits,
      CBmp * pBmp,
      RGBAPIXEL * pPal,
      int DestBPP
    );

  // Decodes true-color bitmap
  void decode24bpp
    ( BYTE * pBits,
      CBmp * pBmp
    );

  // Decodes true-color bitmap
  void CBmpDecoder::decode32bpp
    ( BYTE * pBits,
      CBmp * pBmp
    );

  // Decodes two 4-bit pixels using a palette & puts them in pDest
  // and pDest+4.
  void decode2Nibbles
    ( BYTE * pDest,
      BYTE SrcByte,
      RGBAPIXEL * pPal,
      int DestBPP
    );

  // Makes a private copy of the palette in the file and sets the
  // alpha channel. Returns the size of the palette in bytes.
  int readPalette
    ( WINBITMAPINFOHEADER * pBMI,     // Pointer to bitmapinfoheader in file.
      CDataSource * pDataSrc,
      RGBAPIXEL ** ppPal
    );

};

#endif
