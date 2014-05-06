//! Defines a custom data source for LIBJPEG

GLOBAL(void)
jpeg_mem_src (j_decompress_ptr cinfo, JOCTET * pData, int FileSize);

