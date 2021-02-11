// Copyright 2008 Drew Olbrich

#ifndef MESHRFM__RFM_FILE_WRITER__INCLUDED
#define MESHRFM__RFM_FILE_WRITER__INCLUDED

#include <string>
#include <set>

#include <mesh/Mesh.h>

#include "RfmFileHeader.h"
#include "Zlib.h"

namespace meshrfm {

// RfmFileWriter
//
// This class provides a context for writing a .rfm file.

class RfmFileWriter
{
public:
    RfmFileWriter();
    ~RfmFileWriter();

    void writeRfmFile(const mesh::Mesh &mesh, const std::string &filename);
    
private:
    // This AttributeKey is temporarily assigned to all mesh elements
    // so that the elements' indices can be referenced by other
    // elements when they are written out to the file.
    static const std::string INDEX_KEY_NAME;

    // Reset the RfmFileWriter and free up allocated memory.
    void reset();
    
    // Functions that write the components of the mesh file.
    void writeMagicNumber();
    void writeHeader();
    void writeAttributeKeyMap();
    void writeAttributeKey(const std::string &name, const mesh::AttributeKey &key);
    void writeVertices();
    void writeVertex(const mesh::Vertex &vertex);
    void writeEdges();
    void writeEdge(const mesh::Edge &edge);
    void writeFaces();
    void writeFace(const mesh::Face &face);
    void writeAttributes(const mesh::AttributePossessor &attributePossessor);
    void writeAttributeData(const mesh::AttributeData &attributeData);

    // Writes a string to the file.
    int write(const std::string &string) const;

    // Function template for writing out any kind of type without
    // the possibility of specifying the buffer size incorrectly,
    // as would be the case with a direct call to writeBytes, below.
    template<typename TYPE> int write(const TYPE &buffer) const;

    // Wrapper around gzwrite that throws an exception when an error occurs.
    int writeBytes(const void *buffer, unsigned length) const;

    // Temporarily sets and clears the index attributes which the
    // elements use to refer to each other on disk.
    void setMeshIndices();
    void eraseMeshIndices();

    // Return the count of all non-temporary attribute keys.
    unsigned getNonTemporaryAttributeKeyCount() const;

    // Build a set of all temporary attribute key handles,
    // for fast lookup.
    typedef std::set<mesh::AttributeKey::Handle> AttributeKeyHandleSet;
    void buildTemporaryAttributeKeyHandleSet();
    
    mesh::Mesh *mMesh;
    int mFileDescriptor;
    gzFile mFile;
    std::string mFilename;
    RfmFileHeader mRfmFileHeader;
    mesh::AttributeKey mIndexKey;
    mutable unsigned mBytesWritten;
    AttributeKeyHandleSet mTemporaryAttributeKeyHandleSet;
};

template<typename TYPE> 
int 
RfmFileWriter::write(const TYPE &buffer) const
{
    return writeBytes(&buffer, sizeof(TYPE));
}

} // namespace meshrfm

#endif // MESHRFM__RFM_FILE_WRITER__INCLUDED
