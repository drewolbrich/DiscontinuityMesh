// Copyright 2008 Retroactive Fiasco.

#ifndef MESHOBJ__OBJ_FILE_WRITER__INCLUDED
#define MESHOBJ__OBJ_FILE_WRITER__INCLUDED

#include <fstream>
#include <string>
#include <set>

#include <stdint.h>

#include <mesh/Mesh.h>
#include <mesh/Types.h>

namespace meshobj {

// ObjFileWriter
//
// This class provides a context for writing a Wavefront OBJ file.

class ObjFileWriter
{
public:
    ObjFileWriter();
    ~ObjFileWriter();

    // Write a mesh into a Wavefront OBJ file. If an error occurs
    // while writing the file, a FailedOperationException is thrown. 
    // In addition to the OBJ file, a MTL file with the same root name
    // will be written out if the mesh contains material definitions.
    void writeObjFile(const mesh::Mesh &mesh, 
        const std::string &objFilename);

private:

    // The set of material names that are explicitly defined in the
    // mesh file. This is used to ensure that our procedurally defined
    // default material names don't conflict with the ones that are
    // explicitly defined.
    typedef std::set<std::string> MaterialNameSet;

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

    // This container maps vertex normals to OBJ normal vertex indices.
    typedef std::map<const cgmath::Vector3f, const uint32_t> NormalIndexMap;

    // This container maps texture coordinates to OBJ texture vertex indices.
    typedef std::map<const cgmath::Vector2f, const uint32_t> TexCoordIndexMap;

    // Free up all of the memory used by the class.
    void reset();
    
    // Write the name of the object into the OBJ file. The object name
    // is just the base part of the filename, with the path and
    // extension removed.
    void writeObjectName();

    // Write all the mesh faces into the OBJ file.
    void writeFaces();

    // If the mesh has any material definitions, write them into
    // a MTL file in the same directory as the OBJ file.
    void writeMaterials();

    // Write the material definition with the given index into
    // the MTL file.
    void writeMaterial(std::ofstream &file, int materialIndex);
    
    // Procedurally generate a material name by concatenating the
    // object name with the material index. (e.g. "foo.1"). However,
    // if this name conflicts with a material name that's explicitly
    // defined in the Mesh's attribute material table, add an
    // additional numeric suffix ("foo.1.0"), and keep incrementing it
    // until a name is found that has no conflicts.
    std::string getMaterialName(int materialIndex);

    // Write the vertex positions ("v") into the OBJ file.
    void writeVertexPositions();

    // Write the normal vertices ("vn") into the OBJ file.
    void writeNormalVertices();

    // Write the texture vertices ("vt") into the OBJ file.
    void writeTextureVertices();
    
    std::ofstream *mFile;
    const mesh::Mesh *mMesh;
    std::string mObjFilename;
    std::string mMtlFilename;
    std::string mObjectName;
    MaterialNameSet mMaterialNameSet;
    VertexPtrIndexMap mVertexPtrIndexMap;
    NormalIndexMap mNormalIndexMap;
    TexCoordIndexMap mTexCoordIndexMap;
};

} // namespace meshobj

#endif // MESHOBJ__OBJ_FILE_WRITER__INCLUDED
