// Copyright 2008 Retroactive Fiasco.

#ifndef MESHRFM__RFM_FILE_READER__INCLUDED
#define MESHRFM__RFM_FILE_READER__INCLUDED

#include <string>
#include <map>
#include <vector>

#include <mesh/Mesh.h>
#include <mesh/AttributeKey.h>

#include "RfmFileHeader.h"
#include "Zlib.h"

namespace meshrfm {

// RfmFileReader
//
// This class provides a context for reading an RFM file.

class RfmFileReader
{
public:
    RfmFileReader();
    ~RfmFileReader();

    // Reads a mesh and it's header from a file. If "readElements"
    // is false, the vertices, edges, and faces in the mesh file
    // are not read in, and only the mesh's global attribute table
    // is read.
    void readRfmFile(mesh::Mesh *mesh, RfmFileHeader *header,
        const std::string &filename, bool readElements);
    
private:
    // A map of AttributeKey handles to AttributeKeys. This map is
    // used to match up key handles in the file with the newly created
    // attribute keys (which may have a different set of handles.)
    typedef std::map<mesh::AttributeKey::Handle, mesh::AttributeKey> AttributeKeyHandleMap;

    // Tables that are used to reference the Mesh elements by index
    // as the mesh is being read in.
    typedef std::vector<mesh::VertexPtr> VertexPtrVector;
    typedef std::vector<mesh::EdgePtr> EdgePtrVector;
    typedef std::vector<mesh::FacePtr> FacePtrVector;

    // Reset the RfmFileReader and free up allocated memory.
    void reset();

    // Functions that read the components of the mesh file.
    void readMagicNumber();
    void readHeader();
    void readAttributeKeyMap();
    void readAttributeKey();
    void readVertices();
    void readVertex(mesh::VertexPtr vertexPtr);
    void readEdges();
    void readEdge(mesh::EdgePtr edgePtr);
    void readFaces();
    void readFace(mesh::FacePtr facePtr);
    void readAttributes(mesh::AttributePossessor *attributePossessor);
    void readAttribute(mesh::AttributePossessor *attributePossessor);

    // Reads a string from the file.
    int read(std::string *string) const;

    // Function template for reading any kind of type without
    // the possibility of specifying the buffer size incorrectly,
    // as would be the case with a direct call to readBytes, below.
    template<typename TYPE> int read(TYPE *buffer) const;

    // Wrapper around gzread that throws an exception when an
    // error occurs.
    int readBytes(void *buffer, unsigned length) const;

    mesh::Mesh mMesh;
    int mFileDescriptor;
    gzFile mFile;
    std::string mFilename;
    RfmFileHeader mRfmFileHeader;
    AttributeKeyHandleMap mAttributeKeyHandleMap;
    VertexPtrVector mVertexPtrVector;
    EdgePtrVector mEdgePtrVector;
    FacePtrVector mFacePtrVector;
    mutable unsigned mBytesRead;
};

template<typename TYPE> int 
RfmFileReader::read(TYPE *buffer) const
{
    return readBytes(buffer, sizeof(TYPE));
}

} // namespace meshrfm

#endif // MESHRFM__RFM_FILE_READER__INCLUDED
