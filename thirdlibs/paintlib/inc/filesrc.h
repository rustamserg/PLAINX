// Copyright (c) 1996-1998 Ulrich von Zadow

#ifndef INCL_FILESRC
#define INCL_FILESRC

#ifndef INCL_DATASRC
#include "datasrc.h"
#endif

//! This is a class which takes a file as a source of picture data.
//!
//! Restriction: If memory-mapped files are not available, this
//! class reads all data into memory _before_ decoding can start.
class CFileSource : public CDataSource
{
  DECLARE_DYNAMIC (CFileSource);

public:
  //!
  CFileSource
    ();

  //!
  virtual ~CFileSource
    ();

  //!
  virtual int Open
    ( const char * pszFName
    );

  //!
  virtual void Close
    ();

private:
#ifdef _WINDOWS
  HANDLE m_hf;    // File handle.
  HANDLE m_hm;    // Handle to file-mapping object.
#else
  FILE * m_pFile;
  BYTE * m_pBuffer;
#endif
};

#endif
