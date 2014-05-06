// Copyright (c) 1996-1998 Ulrich von Zadow

#ifndef INCL_DATASRC
#define INCL_DATASRC

#include "except.h"

//! This is a base class for a source of picture data.
//! It defines methods to open, close, and read from data sources.
//! Does byte-order-conversions in the ReadByte, ReadWord, and
//! ReadLong routines.
//!
//! Restriction: All picture data is placed into the internal
//! buffer on open so the ReadXxx-routines can be inline'd in this
//! class. The effect is that anything other than memory-mapped
//! files or files already in memory need to be read into memory
//! before decoding can start. Now if someone would implement a smart
//! buffering scheme...
class CDataSource : public CObject
{
  DECLARE_DYNAMIC (CDataSource);

public:
  //!
  CDataSource
    ();

  //!
  virtual ~CDataSource
    ();

  //!
  virtual void Open
    ( const char * pszName,
      BYTE * pData,
      int    FileSize
    );

  //!
  virtual void Close
    ();

  //!
  char * GetName
    ();

  //!
  BYTE * GetBufferPtr
    ( int MinBytesInBuffer
    );

  //!
  int GetFileSize
    ();

  //!
  BYTE * ReadEverything
    ();

  //!
  BYTE * ReadNBytes
    ( int n
    );

  //!
  BYTE * Read1Byte
    ();

  //!
  BYTE * Read2Bytes
    ();

  //!
  BYTE * Read4Bytes
    ();

  //!
  void AlignToWord
    ();

  //!
  void Skip
    ( int n
    );

  //! Test to see if we didn't go past the end of the file
  void CheckEOF
    ();

protected:
  BYTE * m_pData;
  BYTE * m_pStartData;

private:
  char * m_pszName;        // Name of the data source for diagnostic
                           // purposes.
  int    m_FileSize;
  BOOL   m_bSrcLSBFirst;   // Source byte order: TRUE for intel order,
                           // FALSE for Motorola et al. (MSB first).
};


inline BYTE * CDataSource::GetBufferPtr
    ( int MinBytesInBuffer
    )
{
  return m_pData;
}

inline int CDataSource::GetFileSize
    ()
{
  return m_FileSize;
}

inline BYTE * CDataSource::ReadEverything
    ()
{
  return m_pData;    // ;-)
}

inline BYTE * CDataSource::ReadNBytes
    ( int n
    )
{
  m_pData += n;
  CheckEOF ();
  
  return m_pData-n;
}


inline BYTE * CDataSource::Read1Byte
    ()
{
  m_pData++;
  CheckEOF ();

  return m_pData-1;
}


inline BYTE * CDataSource::Read2Bytes
    ()
{
  m_pData += 2;
  CheckEOF ();

  return m_pData-2;
}


inline BYTE * CDataSource::Read4Bytes
    ()
{
  m_pData += 4;
  CheckEOF ();

  return m_pData-4;
}

inline void CDataSource::AlignToWord
    ()
{
  if ((LONG)(m_pData) & 1)
    Read1Byte();
}


inline void CDataSource::Skip
    ( int n
    )
{
  m_pData += n;
  CheckEOF ();
}

inline void CDataSource::CheckEOF
    ()
{
  if (m_pData - m_pStartData > m_FileSize)
  {
    throw CTextException (ERR_END_OF_FILE, 
          "End of file reached while decoding.\n");
  }
}

#endif
