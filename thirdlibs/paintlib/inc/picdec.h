// Copyright (c) 1996-1998 Ulrich von Zadow

#ifndef INCL_PICDEC
#define INCL_PICDEC

#ifndef INCL_BITMAP
#include "bitmap.h"
#endif

#ifndef INCL_DATASRC
#include "datasrc.h"
#endif

//! CPicDecoder is an abstract base class. It defines common routines
//! for all decoders. Decoders for specific file formats can be
//! derived from this class. Objects of this class interact with a
//! CDataSource to decode bitmaps.
class CPicDecoder : public CObject
{

DECLARE_DYNAMIC (CPicDecoder);

public:
  //! Empty constructor. The actual initialization takes place in a
  //! derived class.
  CPicDecoder
    ();

  //! Destructor. Frees memory allocated.
  virtual ~CPicDecoder
    ();

  //! Decodes a picture in a file and stores the results in pBmp.
  //! BPPWanted is the number of bits per pixel in the bitmap
  //! returned. Valid values for BPPWanted are 8, 24, and 0.
  //! 0 means 'keep the input format'. Decoding a true-color image
  //! to 8 bpp is not supported. As an exception, BPPWanted can be
  //! 1 for b/w tiff files.
  virtual void MakeBmpFromFile
    ( const char * pszFName,
      CBmp * pBmp,
      int BPPWanted = 0
    );

#ifdef _WINDOWS
  //! Decodes a picture in a resource by creating a resource data
  //! source and calling MakeBmp with this data source.
  //! BPPWanted is the number of bits per pixel in the bitmap
  //! returned. Valid values for BPPWanted are 8, 24, and 0.
  //! 0 means 'keep the input format'. Decoding a true-color image
  //! to 8 bpp is not supported. As an exception, BPPWanted can be
  //! 1 for b/w tiff files.
  void MakeBmpFromResource
    ( int ResourceID,
      CBmp * pBmp,
      int BPPWanted = 0
    );
#endif

  //! Decodes a picture by getting the encoded data from pDataSrc.
  //! Stores the results in pBmp. BPPWanted is the number of bits
  //! per pixel in the bitmap returned. Valid values for BPPWanted
  //! are 8, 24, and 0. 0 means 'keep the input format'. Decoding a
  //! true-color image to 8 bpp is not supported.
  virtual void MakeBmp
    ( CDataSource * pDataSrc,
      CBmp * pBmp,
      int BPPWanted = 0
    );

  //! Sets the amount and destination of debug traces output by the
  //! debug version. pszFName contains either a valid file name or is
  //! NULL. If it contains a file name, this file is used to store
  //! debug information. If pszFName is NULL, the destination is
  //! either the MSVC debug console or stderr depending on the
  //! version of the library. Valid values for Level are: <BR>
  //! <BR>
  //! 0: Trace only errors.<BR>
  //! 1: Trace top-level calls.<BR>
  //! 2: Trace picture format information<BR>
  //! 3: Trace all miscellaneous info.<BR>
  //! <BR>
  //! The trace configuration is global to all decoders.
  static void SetTraceConfig
    ( int Level,
      char * pszFName
    );

  // This function is needed by callbacks outside of any object,
  // so it's public and static. It should not be called from
  // outside of the library.
  static void raiseError
    ( int Code,
      char * pszErr
    );

  //! Called to output status messages to the current debug console
  static void Trace
    ( int TraceLevel,
      char * pszMessage
    );

protected:
  //! Implements the actual decoding process. Uses variables local to
  //! the object to retrieve and store the data. Implemented in
  //! derived classes.
  //!
  //! This routine should never be called. It's here so derived classes
  //! can override MakeDIB directly if they want to. (CAnyDecoder does
  //! this).
  virtual void DoDecode
    ( CBmp * pBmp,
      RGBAPIXEL ** ppPal,
      int * pDestBPP,
      CDataSource * pDataSrc
    );

// These routines expand pixel data of various bit depths to 32 bpp.
// The original intent was for several derived classes to use them.
// As it is, they are too slow & therefore almost unused.

  void Expand1bpp
    ( BYTE * pDest,
      BYTE * pSrc,
      int Width,      // Width in pixels
      RGBAPIXEL * pPal
    );

  void Expand2bpp
    ( BYTE * pDest,
      BYTE * pSrc,
      int Width,      // Width in pixels
      RGBAPIXEL * pPal
    );

  void Expand4bpp
    ( BYTE * pDest,
      BYTE * pSrc,
      int Width,      // Width in pixels
      RGBAPIXEL * pPal
    );

  void Expand8bpp
    ( BYTE * pDest,
      BYTE * pSrc,
      int Width,      // Width in pixels
      RGBAPIXEL * pPal
    );

  BYTE ReadByte
    ( CDataSource * pDataSrc
    );

  // Machine-independent routines for byte-order conversion.

  WORD ReadIWord
    ( CDataSource * pDataSrc
    );

  WORD ReadMWord
    ( CDataSource * pDataSrc
    );

  LONG ReadILong
    ( CDataSource * pDataSrc
    );

  LONG ReadMLong
    ( CDataSource * pDataSrc
    );

  void CalcDestBPP
    ( int SrcBPP,
      int* DestBPP
    );

private:
  ///////////////////////////////////////////////////////////////////
  // Member variables.

  static int    m_TraceLevel;    // 0: Trace only errors.
                                 // 1: Trace top-level calls.
                                 // 2: Trace picture format info.
                                 // 3: Trace misc. info.
  static char * m_pszTraceFName; // Name of trace file. NULL if trace to
                                 // MSVC debug console.
};


inline BYTE CPicDecoder::ReadByte
    ( CDataSource * pDataSrc
    )
{

  return *(pDataSrc->ReadNBytes (1));
}


inline WORD CPicDecoder::ReadIWord
    ( CDataSource * pDataSrc
    )
{
  BYTE * pData = pDataSrc->Read2Bytes ();
  // This should work regardless of the destination byte order ;-)
  return pData[0] + (pData[1]<<8);
}


inline WORD CPicDecoder::ReadMWord
    ( CDataSource * pDataSrc
    )
{
  BYTE * pData = pDataSrc->Read2Bytes ();
  return pData[1] + (pData[0]<<8);
}


inline LONG CPicDecoder::ReadILong
    ( CDataSource * pDataSrc
    )
{
  BYTE * pData = pDataSrc->Read4Bytes ();
  return pData[0] + (pData[1]<<8) + (pData[2]<<16) + (pData[3]<<24);
}

inline LONG CPicDecoder::ReadMLong
    ( CDataSource * pDataSrc
    )
{
  BYTE * pData = pDataSrc->Read4Bytes ();
  return pData[3] + (pData[2]<<8) + (pData[1]<<16) + (pData[0]<<24);
}

#endif
