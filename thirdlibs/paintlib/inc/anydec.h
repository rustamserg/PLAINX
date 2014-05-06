// Copyright (c) 1996-1998 Ulrich von Zadow

#ifndef INCL_ANYDEC
#define INCL_ANYDEC

#ifndef INCL_PICDEC
#include "picdec.h"
#endif

#ifndef INCL_CONFIG
#include "config.h"
#endif

class CBitmap;

#ifdef SUPPORT_BMP
class CBmpDecoder;
#endif

#ifdef SUPPORT_PICT
class CPictDecoder;
#endif

#ifdef SUPPORT_TGA
class CTGADecoder;
#endif

#ifdef SUPPORT_TIFF
class CTIFFDecoder;
#endif

#ifdef SUPPORT_JPEG
class CJPEGDecoder;
#endif

#ifdef SUPPORT_PNG
class CPNGDecoder;
#endif

//! Class which decodes pictures with any known format. It
//! auto-detects the format to use and delegates the work to one of
//! the other decoder classes.
//!
//! The interface to CAnyPicDecoder is the same as CPicDecoder.
class CAnyPicDecoder : public CPicDecoder
{

DECLARE_DYNAMIC (CAnyPicDecoder);

public:
  //! Creates a decoder.
  CAnyPicDecoder
    ();

  //! Destroys the decoder.
  ~CAnyPicDecoder
    ();

  //! Decodes a picture in a block of memory.
  void MakeBmp
    ( CDataSource * pDataSrc,
      CBmp * pBmp,
      int BPPWanted
    );

private:
  // Check for file-format-specific data & return the file type if
  // something fits.
  int getFileType
    ( BYTE * pData,
      int DataLen
    );

  long epsLongVal 
    ( unsigned char *p
    );

  ///////////////////////////////////////////////////////////////////
  // Member variables.

#ifdef SUPPORT_BMP
  CBmpDecoder  * m_pBmpDec;
#endif
#ifdef SUPPORT_PICT
  CPictDecoder * m_pPictDec;
#endif
#ifdef SUPPORT_TGA
  CTGADecoder  * m_pTGADec;
#endif
#ifdef SUPPORT_TIFF
  CTIFFDecoder * m_pTIFFDec;
#endif
#ifdef SUPPORT_JPEG
  CJPEGDecoder * m_pJPEGDec;
#endif
#ifdef SUPPORT_PNG
  CPNGDecoder * m_pPNGDec;
#endif

};

#endif
