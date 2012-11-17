// Copyright 2008 Retroactive Fiasco.

#ifndef MESHRFM__RFM_FILE_HEADER__INCLUDED
#define MESHRFM__RFM_FILE_HEADER__INCLUDED

#include <stdint.h>

namespace meshrfm {

// The file format's magic number, encoded as a character array so
// that it's never byte-swapped. The magic number appears first in
// the file, followed by the rfm file header, followed by the
// mesh data.
extern const uint8_t RFM_MAGIC_NUMBER[4];

// The header for .rfm files
struct RfmFileHeader {

    // The size of the header, in bytes, if correctly written out to disk.
    static const unsigned HEADER_SIZE = 48;
    
    // The byte order of the contents of the file, either
    // NATIVE_BYTE_ORDER or FOREIGN_BYTE_ORDER. If the byte order
    // being read is foreign, all integers will have to be
    // byte-swapped. Because this value is stored natively as a 16 bit
    // integer, when read on non-native platform, the two bytes will
    // appear to be swapped. What is written as NATIVE_BYTE_ORDER on a
    // little-endian machine will be read as FOREIGN_BYTE_ORDER on a
    // big-endian machine, and vice versa.
    static const uint16_t NATIVE_BYTE_ORDER = 0x0f;
    static const uint16_t FOREIGN_BYTE_ORDER = 0xf0;
    uint16_t mByteOrder;

    // The size of the header, including the byte order value, and the
    // header size value itself. This can change between file
    // versions. This comes after the byte order, because its encoding
    // is determined by the file's byte order.
    uint16_t mHeaderSize;

    // The version number of the file. Older versions of the code
    // cannot read future file format versions with larger major
    // version numbers. However, older versions of the code are
    // assumed to be forward-compatible with files that have larger
    // minor version numbers. This could be the case if the only
    // change that is made is the addition of fields to the header,
    // since the header-reading code is able to ignore future parts of
    // the header it doesn't yet understand.
    //
    // 1.0 09/04/2008 - Initial version
    // 2.0 06/14/2009 - Added support for BoundingBox2f and BoundingBox3f attributes.
    //
    // When updating the file format, consider creating unit tests
    // to test for backward compatibility with the old file formats.
    static const uint16_t MAJOR_VERSION = 2;
    static const uint16_t MINOR_VERSION = 0;
    uint16_t mMajorVersion;
    uint16_t mMinorVersion;

    // The count of each kind of element in the file.
    uint32_t mAttributeKeyCount;
    uint32_t mVertexCount;
    uint32_t mEdgeCount;
    uint32_t mFaceCount;

    // The bounding box of the vertices in the file.
    float mBoundingMin[3];
    float mBoundingMax[3];
};

} // namespace mesh

#endif // MESHRFM__RFM_FILE_HEADER__INCLUDED
