// Copyright 2008 Drew Olbrich

#include <cstdio>
#include <cstring>
#include <string>
#include <cassert>

#include <zlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdint.h>

#include <except/FailedOperationException.h>
#include <except/OpenFileException.h>
#include <os/Error.h>
#include <cgmath/BoundingBox3f.h>
#include <mesh/Mesh.h>
#include <mesh/MeshOperations.h>
#include <mesh/IsConsistent.h>

#include "RfmFileWriter.h"
#include "RfmFileHeader.h"
#include "Zlib.h"

using except::FailedOperationException;
using except::OpenFileException;
using os::Error;

namespace meshrfm {

const std::string RfmFileWriter::INDEX_KEY_NAME = 
    "__WriteRfmFileContext_index";

RfmFileWriter::RfmFileWriter()
    : mMesh(NULL),
      mFileDescriptor(-1),
      mFile(NULL),
      mFilename(),
      mRfmFileHeader(),
      mIndexKey(),
      mBytesWritten(0),
      mTemporaryAttributeKeyHandleSet()
{
}

RfmFileWriter::~RfmFileWriter()
{
}

void 
RfmFileWriter::writeRfmFile(const mesh::Mesh &mesh, 
    const std::string &filename)
{
    reset();

    // The const_cast is OK because we'll be undoing the changes
    // we make to the mesh before returning.
    mMesh = const_cast<mesh::Mesh *>(&mesh);

    // In debug builds, always verify the consistency of the
    // mesh before writing it out.
    assert(mesh::IsConsistent(*mMesh));
    
    mFilename = filename;
    
    // Create a new file for writing, and allow everyone to
    // have access to it.
    mFileDescriptor = open(filename.c_str(), 
        O_CREAT | O_WRONLY | O_TRUNC,
        S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH);
    if (mFileDescriptor < 0) {
        throw OpenFileException(SOURCE_LINE, Error::fromSystemError())
            << "Could not open mesh file \"" << filename << "\" for writing.";
    }

    // Write the temporary mesh indices to the mesh elements.
    setMeshIndices();

    // Create an STL set of all the handles of all temporary
    // attribute keys, for fast lookup.
    buildTemporaryAttributeKeyHandleSet();
    
    try {

        // The magic number is written out before zlib is used
        // for compression.
        writeMagicNumber();

        // The rest of the file is compressed with zlib.
        mFile = gzdopen(mFileDescriptor, "wb");
        
        writeHeader();
        writeAttributeKeyMap();
        writeAttributes(*mMesh);
        writeVertices();
        writeEdges();
        writeFaces();

    } catch (...) {

        eraseMeshIndices();

        if (mFile != NULL) {
            gzclose(mFile);
        } else {
            close(mFileDescriptor);
        }

        // Delete the file because it has only partially been
        // written out, and is therefore corrupt.
        unlink(mFilename.c_str());

        reset();

        throw;
    }

    eraseMeshIndices();

    gzclose(mFile);

    reset();
}

void
RfmFileWriter::reset()
{
    mMesh = NULL;
    mFileDescriptor = -1;
    mFile = NULL;
    mFilename = "";
    mIndexKey = mesh::AttributeKey();
    mBytesWritten = 0;
    mTemporaryAttributeKeyHandleSet.clear();
}

void 
RfmFileWriter::writeMagicNumber()
{
    if (::write(mFileDescriptor, &RFM_MAGIC_NUMBER, 
            sizeof(RFM_MAGIC_NUMBER)) < 0) {
        throw FailedOperationException(SOURCE_LINE, Error::fromSystemError())
            << "Could not write magic number to mesh file \"" 
            << mFilename << "\".";
    }
}

void 
RfmFileWriter::writeHeader()
{
#ifdef DEBUG
    unsigned initialBytesWritten = mBytesWritten;
#endif

    // This might help avoid warnings in Purify/Valgrind.
    memset(&mRfmFileHeader, 0, sizeof(mRfmFileHeader));
    
    // We always write out data in the native byte order.
    // It's the file reader's responsibility to swap the
    // data read in, if necessary.
    mRfmFileHeader.mByteOrder = RfmFileHeader::NATIVE_BYTE_ORDER;

    mRfmFileHeader.mHeaderSize = RfmFileHeader::HEADER_SIZE;

    mRfmFileHeader.mMajorVersion = RfmFileHeader::MAJOR_VERSION;
    mRfmFileHeader.mMinorVersion = RfmFileHeader::MINOR_VERSION;

    mRfmFileHeader.mAttributeKeyCount = getNonTemporaryAttributeKeyCount();

    mRfmFileHeader.mVertexCount = mMesh->vertexCount();
    mRfmFileHeader.mEdgeCount = mMesh->edgeCount();
    mRfmFileHeader.mFaceCount = mMesh->faceCount();

    // Compute and write out the Mesh's bounding box.
    cgmath::BoundingBox3f boundingBox = mesh::ComputeBoundingBox(*mMesh);
    mRfmFileHeader.mBoundingMin[0] = boundingBox.minX();
    mRfmFileHeader.mBoundingMin[1] = boundingBox.minY();
    mRfmFileHeader.mBoundingMin[2] = boundingBox.minZ();
    mRfmFileHeader.mBoundingMax[0] = boundingBox.maxX();
    mRfmFileHeader.mBoundingMax[1] = boundingBox.maxY();
    mRfmFileHeader.mBoundingMax[2] = boundingBox.maxZ();

    // The data members of the header are written out individually,
    // as opposed to writing out the whole header at once.
    // Writing structs to disk isn't portable, as they may be 
    // laid out differently on other compilers or architectures.

    write(mRfmFileHeader.mByteOrder);
    write(mRfmFileHeader.mHeaderSize);
    write(mRfmFileHeader.mMajorVersion);
    write(mRfmFileHeader.mMinorVersion);
    write(mRfmFileHeader.mAttributeKeyCount);
    write(mRfmFileHeader.mVertexCount);
    write(mRfmFileHeader.mEdgeCount);
    write(mRfmFileHeader.mFaceCount);
    write(mRfmFileHeader.mBoundingMin);
    write(mRfmFileHeader.mBoundingMax);
    
    // The number of bytes written out must exactly match the
    // header size that was written into the file earlier.
    assert(mBytesWritten - initialBytesWritten == RfmFileHeader::HEADER_SIZE);
}

void 
RfmFileWriter::writeAttributeKeyMap()
{
    unsigned count = 0;
    
    for (mesh::AttributeKeyMap::const_iterator iterator = mMesh->attributeKeyMapBegin();
         iterator != mMesh->attributeKeyMapEnd(); ++iterator) {
        const std::string &name((*iterator).first);
        const mesh::AttributeKey &key((*iterator).second);
        if (!key.isTemporary()) {
            writeAttributeKey(name, key);
            count++;
        }
    }

    // Make sure we really wrote out the number of attribute keys
    // we were expecting.
    assert(count == mRfmFileHeader.mAttributeKeyCount);
}

void 
RfmFileWriter::writeAttributeKey(const std::string &name, 
    const mesh::AttributeKey &key)
{
    write(name);

    uint16_t handle = key.handle();
    write(handle);

    uint16_t type = key.type();
    write(type);

    uint32_t flags = key.flags();
    write(flags);
}

void 
RfmFileWriter::writeVertices()
{
    // Write out each vertex.
    for (mesh::ConstVertexPtr vertexPtr = mMesh->vertexBegin();
         vertexPtr != mMesh->vertexEnd(); ++vertexPtr) {
        writeVertex(*vertexPtr);
    }
}

void 
RfmFileWriter::writeVertex(const mesh::Vertex &vertex)
{
    // Write out the vertex's attributes.
    writeAttributes(vertex);

    // Write out the vertex's position.
    writeBytes(vertex.position().asFloatPtr(), 3*sizeof(float));
}

void 
RfmFileWriter::writeEdges()
{
    // Write out each edge.
    for (mesh::ConstEdgePtr edgePtr = mMesh->edgeBegin();
         edgePtr != mMesh->edgeEnd(); ++edgePtr) {
        writeEdge(*edgePtr);
    }
}

void 
RfmFileWriter::writeEdge(const mesh::Edge &edge)
{
    // Write out the edge's attributes.
    writeAttributes(edge);

    // The adjacent vertex count is written out as a 16 bit number,
    // so at most there can be 65535 adjacent vertices.
    assert(edge.adjacentVertexCount() < 65536);

    // Write out the number of neighboring vertices.
    uint16_t adjacentVertexCount = edge.adjacentVertexCount();
    write(adjacentVertexCount);
    
    // Write out the indices of the edge's neighboring vertices.
    for (mesh::AdjacentVertexConstIterator iterator = edge.adjacentVertexBegin(); 
         iterator != edge.adjacentVertexEnd(); ++iterator) {
        uint32_t index = (*iterator)->getInt(mIndexKey);
        write(index);
    }
}

void 
RfmFileWriter::writeFaces()
{
    // Write out each face.
    for (mesh::ConstFacePtr facePtr = mMesh->faceBegin();
         facePtr != mMesh->faceEnd(); ++facePtr) {
        writeFace(*facePtr);
    }
}

void 
RfmFileWriter::writeFace(const mesh::Face &face)
{
    // Write out the face's attributes.
    writeAttributes(face);

    // The adjacent vertex count is written out as a 16 bit number,
    // so at most there can be 65535 adjacent vertices.
    assert(face.adjacentVertexCount() < 65536);

    // Write out the number of neighboring vertices.
    uint16_t adjacentVertexCount = face.adjacentVertexCount();
    write(adjacentVertexCount);

    // Write out the indices of the face's neighboring vertices.
    for (mesh::AdjacentVertexConstIterator iterator 
             = face.adjacentVertexBegin(); 
         iterator != face.adjacentVertexEnd(); ++iterator) {
        uint32_t index = (*iterator)->getInt(mIndexKey);
        write(index);
    }

    // The adjacent edge count is written out as a 16 bit number,
    // so at most there can be 65535 adjacent edges.
    assert(face.adjacentEdgeCount() < 65536);
    
    // Write out the number of neighboring edges.
    uint16_t adjacentEdgeCount = face.adjacentEdgeCount();
    write(adjacentEdgeCount);

    // Write out the indices of the face's neighboring edges.
    for (mesh::AdjacentEdgeConstIterator iterator = face.adjacentEdgeBegin(); 
         iterator != face.adjacentEdgeEnd(); ++iterator) {
        uint32_t index = (*iterator)->getInt(mIndexKey);
        write(index);
    }

    // The face vertex count is written out as a 16 bit number,
    // so at most there can be 65535 adjacent edges.
    assert(face.faceVertexVectorSize() < 65536);

    // Write out the number of FaceVertex objects, used to
    // hold per-vertex face attribute data.
    uint16_t faceVertexCount = face.faceVertexVectorSize();
    write(faceVertexCount);

    // Write out the FaceVertex attributes.
    for (mesh::Face::FaceVertexVectorConstIterator iterator = face.faceVertexVectorBegin();
         iterator != face.faceVertexVectorEnd(); ++iterator) {
        const mesh::FaceVertex *faceVertex = *iterator;
        uint32_t index = faceVertex->vertexPtr()->getInt(mIndexKey);
        write(index);
        writeAttributes(*faceVertex);
    }
}

void 
RfmFileWriter::writeAttributes(const mesh::AttributePossessor &attributePossessor)
{
    // Write the number of attributes. Rather than call
    // AttributePossessor::attributeDataCount, we count the
    // attributes manually, skipping the temporary
    // index key attribute.
    uint16_t count = 0;
    for (mesh::AttributePossessor::const_iterator iterator
             = attributePossessor.attributeDataBegin();
         iterator != attributePossessor.attributeDataEnd(); ++iterator) {
        // Don't count the attributes flagged as temporary.
        if (mTemporaryAttributeKeyHandleSet.find((*iterator).handle())
            == mTemporaryAttributeKeyHandleSet.end()) {
            ++count;
        }
    }
    write(count);

    // Write out the attribute values.
    for (mesh::AttributePossessor::const_iterator iterator
             = attributePossessor.attributeDataBegin();
         iterator != attributePossessor.attributeDataEnd(); ++iterator) {
        // Don't write out the attributes flagged as temporary.
        if (mTemporaryAttributeKeyHandleSet.find((*iterator).handle())
            == mTemporaryAttributeKeyHandleSet.end()) {
            writeAttributeData(*iterator);
        }
    }
}

void 
RfmFileWriter::writeAttributeData(const mesh::AttributeData &attributeData)
{
    uint16_t handle = attributeData.handle();
    write(handle);

    const std::string *string = NULL;

    switch (attributeData.type()) {
    case mesh::AttributeKey::UNDEFINED:
        // Cannot read attribute data of type UNDEFINED.
        abort();
    case mesh::AttributeKey::BOOL:
        writeBytes(attributeData.data(), sizeof(bool));
        break;
    case mesh::AttributeKey::INT:
        writeBytes(attributeData.data(), sizeof(int32_t));
        break;
    case mesh::AttributeKey::FLOAT:
        writeBytes(attributeData.data(), sizeof(float));
        break;
    case mesh::AttributeKey::VECTOR2F:
        writeBytes(attributeData.data(), 2*sizeof(float));
        break;
    case mesh::AttributeKey::VECTOR3F:
        writeBytes(attributeData.data(), 3*sizeof(float));
        break;
    case mesh::AttributeKey::VECTOR4F:
        writeBytes(attributeData.data(), 4*sizeof(float));
        break;
    case mesh::AttributeKey::MATRIX3F:
        writeBytes(attributeData.data(), 9*sizeof(float));
        break;
    case mesh::AttributeKey::MATRIX4F:
        writeBytes(attributeData.data(), 16*sizeof(float));
        break;
    case mesh::AttributeKey::STRING:
        string = static_cast<const std::string *>(attributeData.data());
        write(*string);
        break;
    case mesh::AttributeKey::UNIT_VECTOR3F:
        writeBytes(attributeData.data(), 3*sizeof(float));
        break;
    case mesh::AttributeKey::BOUNDINGBOX2F:
        writeBytes(attributeData.data(), 4*sizeof(float));
        break;
    case mesh::AttributeKey::BOUNDINGBOX3F:
        writeBytes(attributeData.data(), 6*sizeof(float));
        break;
    default:
        // The code to read the attribute's type has not
        // yet been implemented.
        abort();
    }
}

int 
RfmFileWriter::writeBytes(const void *buffer, unsigned length) const
{
    int result = gzwrite(mFile, buffer, length);

    if (result == 0) {
        std::ostringstream ostr;
        ostr << "Could not write to mesh file \"" << mFilename << "\".";
        ThrowZlibErrorException(mFile, SOURCE_LINE, ostr.str());
    }

    mBytesWritten += length;
    
    return result;
}

void
RfmFileWriter::setMeshIndices()
{
    // This AttributeKey is temporarily assigned to all mesh elements
    // so that the elements' indices can be referenced by other
    // elements when they are written out to the file.
    mIndexKey = mMesh->getAttributeKey(INDEX_KEY_NAME, 
        mesh::AttributeKey::INT, mesh::AttributeKey::TEMPORARY);

    // Loop over all vertices, edges, and faces, assigning
    // each a unique index, starting with zero for each element type.

    int index = 0;
    for (mesh::VertexPtr vertexPtr = mMesh->vertexBegin();
         vertexPtr != mMesh->vertexEnd(); ++vertexPtr) {
        vertexPtr->setInt(mIndexKey, index);
        ++index;
    }

    index = 0;
    for (mesh::EdgePtr edgePtr = mMesh->edgeBegin();
         edgePtr != mMesh->edgeEnd(); ++edgePtr) {
        edgePtr->setInt(mIndexKey, index);
        ++index;
    }

    index = 0;
    for (mesh::FacePtr facePtr = mMesh->faceBegin();
         facePtr != mMesh->faceEnd(); ++facePtr) {
        facePtr->setInt(mIndexKey, index);
        ++index;
    }
}

void
RfmFileWriter::eraseMeshIndices()
{
    // Remove all the indices added in the previous function.
    // The index key has been flagged as temporary, so there's
    // no danger of it being written to disk, but we want to
    // restore the mesh to the exact condition we found it in.

    for (mesh::VertexPtr vertexPtr = mMesh->vertexBegin();
         vertexPtr != mMesh->vertexEnd(); ++vertexPtr) {
        vertexPtr->eraseAttribute(mIndexKey);
    }

    for (mesh::EdgePtr edgePtr = mMesh->edgeBegin();
         edgePtr != mMesh->edgeEnd(); ++edgePtr) {
        edgePtr->eraseAttribute(mIndexKey);
    }

    for (mesh::FacePtr facePtr = mMesh->faceBegin();
         facePtr != mMesh->faceEnd(); ++facePtr) {
        facePtr->eraseAttribute(mIndexKey);
    }

    mMesh->eraseAttributeKey(INDEX_KEY_NAME);
}

unsigned 
RfmFileWriter::getNonTemporaryAttributeKeyCount() const
{
    unsigned result = 0;

    for (mesh::AttributeKeyMap::const_iterator iterator
             = mMesh->attributeKeyMapBegin();
         iterator != mMesh->attributeKeyMapEnd(); ++iterator) {
        const mesh::AttributeKey &attributeKey((*iterator).second);
        if (!attributeKey.isTemporary()) {
            ++result;
        }
    }

    return result;
}

void 
RfmFileWriter::buildTemporaryAttributeKeyHandleSet()
{
    mTemporaryAttributeKeyHandleSet.clear();

    for (mesh::AttributeKeyMap::const_iterator iterator
             = mMesh->attributeKeyMapBegin();
         iterator != mMesh->attributeKeyMapEnd(); ++iterator) {
        const mesh::AttributeKey &attributeKey((*iterator).second);
        if (attributeKey.isTemporary()) {
            mTemporaryAttributeKeyHandleSet.insert(attributeKey.handle());
        }
    }
}

int
RfmFileWriter::write(const std::string &string) const
{
    uint32_t size = string.size();
    write(size);
    return writeBytes(string.data(), (unsigned) size);
}

} // namespace mesh
