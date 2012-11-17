// Copyright 2008 Retroactive Fiasco.

#ifndef RFM2WRL__WRL_FILE_WRITER__INCLUDED
#define RFM2WRL__WRL_FILE_WRITER__INCLUDED

#include <fstream>
#include <string>
#include <map>

#include <mesh/Mesh.h>
#include <mesh/Types.h>
#include <cgmath/Vector3f.h>

// WrlFileWriter
//
// This class provides a context for writing a VRML 2.0 WRL file.

class WrlFileWriter
{
public:
    WrlFileWriter();
    ~WrlFileWriter();

    // Set the sky color (the VRML background color).
    void setSkyColor(const cgmath::Vector3f &skyColor);

    void writeWrlFile(const mesh::Mesh &mesh, const std::string &wrlFilename);

private:
    void writeHeader();
    void writeCoordinates();
    void writeColors();
    void writeCoordinateIndices();
    void writeColorIndices();
    void writeFooter();

    cgmath::Vector3f mSkyColor;

    std::ofstream *mFile;
    const mesh::Mesh *mMesh;
    std::string mWrlFilename;
    cgmath::Vector3f mOffset;

    // In the definition of VertexPtrIndexMap, below, the following
    // function object is needed to provide a comparison operator for
    // VertexPtr, which is not provided by the system, because VertexPtr
    // is really an std::list iterator. We don't care about their ordering
    // within the list, so it's OK to cast them to void pointers for
    // purposes of comparison.
    struct VertexPtrCompare 
    {
        bool operator()(mesh::ConstVertexPtr p1, 
            mesh::ConstVertexPtr p2) const {
            return &*p1 < &*p2;
        }
    };

    // This container maps vertex pointers to OBJ file vertex indices.
    typedef std::map<const mesh::ConstVertexPtr, const uint32_t, 
                     VertexPtrCompare> VertexPtrIndexMap;
    VertexPtrIndexMap mVertexPtrIndexMap;
};

#endif // RFM2WRL__WRL_FILE_WRITER__INCLUDED
