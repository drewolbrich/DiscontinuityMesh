// Copyright 2008 Retroactive Fiasco.

#include <cstdio>
#include <cstring>
#include <cassert>
#include <string>
#include <map>
#include <memory>
#include <algorithm>

#include <zlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdint.h>

#include <except/FailedOperationException.h>
#include <except/OpenFileException.h>
#include <os/Error.h>
#include <str/FilenameOperations.h>
#include <mesh/Mesh.h>
#include <mesh/IsConsistent.h>

#include "RfmFileReader.h"
#include "RfmFileHeader.h"
#include "Zlib.h"

using except::FailedOperationException;
using except::OpenFileException;
using os::Error;
using cgmath::Vector2f;
using cgmath::Vector3f;
using cgmath::Vector4f;
using cgmath::Matrix3f;
using cgmath::Matrix4f;
using cgmath::BoundingBox2f;
using cgmath::BoundingBox3f;

namespace meshrfm {

RfmFileReader::RfmFileReader()
    : mMesh(),
      mFileDescriptor(-1),
      mFile(NULL),
      mFilename(),
      mRfmFileHeader(),
      mAttributeKeyHandleMap(),
      mVertexPtrVector(),
      mEdgePtrVector(),
      mFacePtrVector(),
      mBytesRead(0)
{
}

RfmFileReader::~RfmFileReader()
{
}

void 
RfmFileReader::readRfmFile(mesh::Mesh *mesh, RfmFileHeader *header,
    const std::string &filename, bool readElements)
{
    reset();

    mFilename = filename;
    mMesh.setFilename(filename);

    mFileDescriptor = open(mFilename.c_str(), O_RDONLY);
    if (mFileDescriptor < 0) {
        throw OpenFileException(SOURCE_LINE, Error::fromSystemError())
            << "Could not open mesh file \"" << filename << "\" for reading.";
    }

    try {

        readMagicNumber();

        // The rest of the file is compressed with zlib.
        mFile = gzdopen(mFileDescriptor, "rb");
        if (mFile == NULL) {
            throw FailedOperationException(SOURCE_LINE)
                << "Could not open zlib stream for mesh file \"" << mFilename << "\".";
        }
        
        readHeader();
        readAttributeKeyMap();
        readAttributes(&mMesh);

        if (readElements) {
            // Since we know in advance how many mesh elements
            // we'll be reading in, reserve space for them
            // in the STL tables.
            mVertexPtrVector.reserve(mRfmFileHeader.mVertexCount);
            mEdgePtrVector.reserve(mRfmFileHeader.mEdgeCount);
            mFacePtrVector.reserve(mRfmFileHeader.mFaceCount);
            
            // Create all the elements.
            for (uint32_t i = 0; i < mRfmFileHeader.mVertexCount; ++i) {
                mVertexPtrVector.push_back(mMesh.createVertex());
            }
            for (uint32_t i = 0; i < mRfmFileHeader.mEdgeCount; ++i) {
                mEdgePtrVector.push_back(mMesh.createEdge());
            }
            for (uint32_t i = 0; i < mRfmFileHeader.mFaceCount; ++i) {
                mFacePtrVector.push_back(mMesh.createFace());
            }
            
            // The number of elements now in the mesh must match
            // up with the sizes of the vectors.
            if (mMesh.vertexCount() != mVertexPtrVector.size()
                || mMesh.edgeCount() != mEdgePtrVector.size()
                || mMesh.faceCount() != mFacePtrVector.size()) {
                throw FailedOperationException(SOURCE_LINE)
                    << "Out of memory while loading mesh file \"" << mFilename << "\".";
            }
            
            // Read the element definitions from the file.
            readVertices();
            readEdges();
            readFaces();
        }
        
    } catch (...) {

        if (mFile != NULL) {
            gzclose(mFile);
        } else {
            close(mFileDescriptor);
        }

        reset();

        throw;
    }

    gzclose(mFile);
    
    *header = mRfmFileHeader;

    assert(mesh::IsConsistent(mMesh));
    
    // Swap the contents of the Mesh we just read in into the Mesh that is the return
    // value. By doing this at the very end, and not operating on the passed mesh argument
    // directly, we avoid returning a partial, invalid mesh if an exception is thrown.
    mesh->swap(mMesh);

    reset();
}

void 
RfmFileReader::reset()
{
    mMesh.clear();

    mFileDescriptor = -1;
    mFile = NULL;
    mFilename = "";

    mAttributeKeyHandleMap.clear();
    mVertexPtrVector.clear();
    mEdgePtrVector.clear();
    mFacePtrVector.clear();
    mBytesRead = 0;
}

void 
RfmFileReader::readMagicNumber()
{
    // The 32 bit magic number is read as a sequence of four bytes,
    // rather than as a 32 bit integer, so that it'll never be
    // byte-swapped.
    uint8_t magicNumber[sizeof(RFM_MAGIC_NUMBER)];
    if (::read(mFileDescriptor, &magicNumber, sizeof(magicNumber)) < 0) {
        throw FailedOperationException(SOURCE_LINE, Error::fromSystemError())
            << "Could not read magic number from mesh file \"" << mFilename << "\".";
    }

    // Check to make sure that the magic number in the file
    // is correct.
    if (memcmp(magicNumber, RFM_MAGIC_NUMBER, sizeof(magicNumber)) != 0) {
        throw FailedOperationException(SOURCE_LINE)
            << "Could not read mesh file \"" << mFilename << "\" because it does not have the "
                << "expected magic number.";
    }
}

void 
RfmFileReader::readHeader()
{
#ifdef DEBUG
    unsigned initialBytesRead = mBytesRead;
#endif

    RfmFileHeader header;

    // Fill the header with zeros, because we might not
    // read the whole thing from the file if there's
    // a version mismatch.
    memset(&header, 0, sizeof(header));

    read(&header.mByteOrder);

    // Check if the byte order of the file is our machine's native
    // byte order. There is currently no support for reading
    // non-native byte orders. If this functionality is added later,
    // this error message would be removed, and the rest of the code
    // in this file would have to be updated to do byte-swapping as
    // necessary.
    if (header.mByteOrder != RfmFileHeader::NATIVE_BYTE_ORDER) {
        throw FailedOperationException(SOURCE_LINE)
            << "Could not read mesh file \"" 
                << mFilename << "\" because it was created on a computer "
                << "with a different byte order.";
    }

    read(&header.mHeaderSize);
    read(&header.mMajorVersion);
    read(&header.mMinorVersion);
    read(&header.mAttributeKeyCount);
    read(&header.mVertexCount);
    read(&header.mEdgeCount);
    read(&header.mFaceCount);
    read(&header.mBoundingMin);
    read(&header.mBoundingMax);

    // The number of bytes read in must exactly match the
    // size of the header we are expecting to read.
    assert(mBytesRead - initialBytesRead == RfmFileHeader::HEADER_SIZE);

    // We must not have read more bytes than were specified
    // in the file as the size of the header. If this fails, the code
    // should be modified to only read as much of the header
    // as exists on disk.
    assert(mBytesRead - initialBytesRead <= header.mHeaderSize);
    
    // If the header in the file is larger than our own concept of how
    // large the header should be, skip to the end of the header in
    // the file. This is how the code can be forward compatible in the
    // case when only the minor version number changes.
    if (header.mHeaderSize > RfmFileHeader::HEADER_SIZE) {
        if (gzseek(mFile, 
                header.mHeaderSize - RfmFileHeader::HEADER_SIZE, 
                SEEK_CUR) < 0) {
            throw FailedOperationException(SOURCE_LINE, Error::fromSystemError())
                << "Could not seek to end of header in mesh file \"" 
                    << mFilename << "\".";
        }
    }

    // Make sure that the major version number is supported. When new
    // versions are created, code will have to be added to recognize
    // older file versions, and interpret them correctly.
    if (header.mMajorVersion > RfmFileHeader::MAJOR_VERSION) {
        throw FailedOperationException(SOURCE_LINE)
            << "Could not read mesh file \"" 
                << mFilename << "\" because it has a major version number ("
                << header.mMajorVersion << ") that "
                << "is newer than this program supports ("
                << RfmFileHeader::MAJOR_VERSION << ").";
    }

    mRfmFileHeader = header;
}

void 
RfmFileReader::readAttributeKeyMap()
{
    uint32_t count = mRfmFileHeader.mAttributeKeyCount;
    for (unsigned int i = 0; i < count; ++i) {
        readAttributeKey();
    }
}

void 
RfmFileReader::readAttributeKey()
{
    std::string name;
    read(&name);

    uint16_t handle;
    read(&handle);

    uint16_t type;
    read(&type);

    uint32_t flags = 0;
    read(&flags);
    
    // Create the corresponding AttributeKey.
    mesh::AttributeKey attributeKey = mMesh.getAttributeKey(name, 
        static_cast<mesh::AttributeKey::Type>(type), flags);

    // Insert the AttributeKey into the AttributeKeyHandleMap,
    // so we can look it up by handle when attributes are
    // being read from the file.
    mAttributeKeyHandleMap.insert(std::make_pair(handle, attributeKey));
}

void 
RfmFileReader::readVertices()
{
    for (uint32_t i = 0; i < mVertexPtrVector.size(); ++i) {
        readVertex(mVertexPtrVector[i]);
    }
}

void
RfmFileReader::readVertex(mesh::VertexPtr vertexPtr)
{
    // Read the vertex's attributes.
    readAttributes(&*vertexPtr);

    // Read the vertex's position.
    float position[3];
    read(&position);
    vertexPtr->setPosition(Vector3f(position));
}

void 
RfmFileReader::readEdges()
{
    for (uint32_t i = 0; i < mEdgePtrVector.size(); ++i) {
        readEdge(mEdgePtrVector[i]);
    }
}

void
RfmFileReader::readEdge(mesh::EdgePtr edgePtr)
{
    // Read the edge's attributes.
    readAttributes(&*edgePtr);

    // Read the count of adjacent vertices.
    uint16_t adjacentVertexCount;
    read(&adjacentVertexCount);

    // Read the adjacent vertices.
    for (uint16_t i = 0; i < adjacentVertexCount; ++i) {
        uint32_t index;
        read(&index);
        if (index >= mVertexPtrVector.size()) {
            throw FailedOperationException(SOURCE_LINE) << "Illegal vertex index " << index
                    << " for edge in mesh file \"" << mFilename << "\".";
        }
        edgePtr->addAdjacentVertex(mVertexPtrVector[index]);
        mVertexPtrVector[index]->addAdjacentEdge(edgePtr);
    }
}

void 
RfmFileReader::readFaces()
{
    for (uint32_t i = 0; i < mFacePtrVector.size(); ++i) {
        readFace(mFacePtrVector[i]);
    }
}

void
RfmFileReader::readFace(mesh::FacePtr facePtr)
{
    // Read the face's attributes.
    readAttributes(&*facePtr);

    // Read the count of adjacent vertices.
    uint16_t adjacentVertexCount;
    read(&adjacentVertexCount);

    // Read the adjacent vertices.
    for (uint16_t i = 0; i < adjacentVertexCount; ++i) {
        uint32_t index;
        read(&index);
        if (index >= mVertexPtrVector.size()) {
            throw FailedOperationException(SOURCE_LINE) << "Illegal vertex index " << index
                << " for face in mesh file \"" << mFilename << "\".";
        }
        facePtr->addAdjacentVertex(mVertexPtrVector[index]);
        mVertexPtrVector[index]->addAdjacentFace(facePtr);
    }

    // Read the count of adjacent edges.
    uint16_t adjacentEdgeCount;
    read(&adjacentEdgeCount);

    // Read the adjacent edges.
    for (uint16_t i = 0; i < adjacentEdgeCount; ++i) {
        uint32_t index;
        read(&index);
        if (index >= mEdgePtrVector.size()) {
            throw FailedOperationException(SOURCE_LINE) << "Illegal vertex index " << index
                << " for edge in mesh file \"" << mFilename << "\".";
        }
        facePtr->addAdjacentEdge(mEdgePtrVector[index]);
        mEdgePtrVector[index]->addAdjacentFace(facePtr);
    }

    // Read the count of face vertices.
    uint16_t faceVertexCount;
    read(&faceVertexCount);

    // Read the face vertex attributes for each face vertex.
    for (uint16_t i = 0; i < faceVertexCount; ++i) {
        uint32_t index;
        read(&index);
        if (index >= mVertexPtrVector.size()) {
            throw FailedOperationException(SOURCE_LINE) << "Illegal vertex index " << index
                << " for face vertex in mesh file \"" << mFilename << "\".";
        }
        mesh::FaceVertex *faceVertex 
            = facePtr->getFaceVertex(mVertexPtrVector[index]);
        readAttributes(&*faceVertex);
    }
}

void
RfmFileReader::readAttributes(mesh::AttributePossessor *attributePossessor)
{
    uint16_t count;
    read(&count);

    for (uint16_t i = 0; i < count; ++i) {
        readAttribute(attributePossessor);
    }
}

void
RfmFileReader::readAttribute(mesh::AttributePossessor *attributePossessor)
{
    uint16_t handle;
    read(&handle);

    AttributeKeyHandleMap::const_iterator iterator
        = mAttributeKeyHandleMap.find(handle);

    // The handle must exist in the mAttributeKeyHandleMap.
    if (iterator == mAttributeKeyHandleMap.end()) {
        throw FailedOperationException(SOURCE_LINE) << "Illegal attribute handle " << handle
            << " in mesh file \"" << mFilename << "\".";
    }

    // Get the AttributeKey that corresponds to the handle.
    mesh::AttributeKey attributeKey((*iterator).second);

    bool boolValue;
    int32_t intValue;
    float floatValue;
    float vector2fValue[2];
    float vector3fValue[3];
    float vector4fValue[4];
    float matrix3fValue[3][3];
    float matrix4fValue[4][4];
    float boundingBox2fValue[4];
    float boundingBox3fValue[6];
    std::string stringValue;
    
    switch (attributeKey.type()) {
    case mesh::AttributeKey::UNDEFINED:
        // Cannot write attribute data of type UNDEFINED.
        abort();
    case mesh::AttributeKey::BOOL:
        read(&boolValue);
        attributePossessor->setBool(attributeKey, boolValue);
        break;
    case mesh::AttributeKey::INT:
        read(&intValue);
        attributePossessor->setInt(attributeKey, intValue);
        break;
    case mesh::AttributeKey::FLOAT:
        read(&floatValue);
        attributePossessor->setFloat(attributeKey, floatValue);
        break;
    case mesh::AttributeKey::VECTOR2F:
        read(&vector2fValue);
        attributePossessor->setVector2f(attributeKey, Vector2f(vector2fValue));
        break;
    case mesh::AttributeKey::VECTOR3F:
        read(&vector3fValue);
        attributePossessor->setVector3f(attributeKey, Vector3f(vector3fValue));
        break;
    case mesh::AttributeKey::VECTOR4F:
        read(&vector4fValue);
        attributePossessor->setVector4f(attributeKey, Vector4f(vector4fValue));
        break;
    case mesh::AttributeKey::MATRIX3F:
        read(&matrix3fValue);
        attributePossessor->setMatrix3f(attributeKey, Matrix3f(matrix3fValue));
        break;
    case mesh::AttributeKey::MATRIX4F:
        read(&matrix4fValue);
        attributePossessor->setMatrix4f(attributeKey, Matrix4f(matrix4fValue));
        break;
    case mesh::AttributeKey::STRING:
        read(&stringValue);
        attributePossessor->setString(attributeKey, stringValue);
        break;
    case mesh::AttributeKey::UNIT_VECTOR3F:
        read(&vector3fValue);
        attributePossessor->setUnitVector3f(attributeKey, Vector3f(vector3fValue));
        break;
    case mesh::AttributeKey::BOUNDINGBOX2F:
        read(&boundingBox2fValue);
        attributePossessor->setBoundingBox2f(attributeKey, 
            BoundingBox2f(
                Vector2f(boundingBox2fValue[0], boundingBox2fValue[1]),
                Vector2f(boundingBox2fValue[2], boundingBox2fValue[3])));
        break;
    case mesh::AttributeKey::BOUNDINGBOX3F:
        read(&boundingBox3fValue);
        attributePossessor->setBoundingBox3f(attributeKey, 
            BoundingBox3f(
                Vector3f(boundingBox3fValue[0], boundingBox3fValue[1], boundingBox3fValue[2]),
                Vector3f(boundingBox3fValue[3], boundingBox3fValue[4], boundingBox3fValue[5])));
        break;
    default:
        // The code to write out this attribute's type has not
        // yet been implemented.
        abort();
    }
}

int 
RfmFileReader::readBytes(void *buffer, unsigned length) const
{
    int result = gzread(mFile, buffer, length);

    if (result < 0) {
        std::ostringstream ostr;
        ostr << "Unexpected end of file while reading mesh file \"" 
            << mFilename << "\".";
        ThrowZlibErrorException(mFile, SOURCE_LINE, ostr.str());
    }

    if ((unsigned) result < length) {
        throw FailedOperationException(SOURCE_LINE) 
            << "Unexpected end of file while reading mesh file \"" 
                << mFilename << "\".";
    }

    mBytesRead += length;

    return result;
}

int
RfmFileReader::read(std::string *string) const
{
    uint32_t stringSize;
    read(&stringSize);

    // Use auto_ptr to keep track of the array of characters
    // that are allocated, because it's possible that readBytes
    // may throw an exception. Otherwise, memory could be leaked.
    std::auto_ptr<char> cstr(
        static_cast<char *>(operator new(sizeof(char)*stringSize)));

    int result = readBytes(cstr.get(), stringSize);

    *string = std::string(cstr.get(), stringSize);

    return result;
}

} // namespace mesh
