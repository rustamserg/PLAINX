//! This file defines the data structures used in pict files. They
//! correspond to the appropriate MAC QuickDraw structs. See
//! QuickDraw docs for an explanation of these structs.

#ifndef INCL_QDRAW
#define INCL_QDRAW

struct MacRect
{
	WORD top;
	WORD left;
	WORD bottom;
	WORD right;
};

struct MacpixMap
{
	// Ptr baseAddr              // Not used in file.
  // short rowBytes            // read in seperatly.
  struct MacRect Bounds;
	WORD version;
	WORD packType;
	LONG packSize;
	LONG hRes;
	LONG vRes;
	WORD pixelType;
	WORD pixelSize;
	WORD cmpCount;
	WORD cmpSize;
	LONG planeBytes;
	LONG pmTable;
	LONG pmReserved;
};

struct MacRGBColour
{
	WORD red;
	WORD green;
	WORD blue;
};

struct MacPoint
{
	WORD x;
	WORD y;
};

struct MacPattern // Klaube
{
	BYTE pix[64];
};

#endif