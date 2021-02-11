// Copyright 2008 Drew Olbrich

#ifndef MESHOBJ__OBJ_FILE_READER__INCLUDED
#define MESHOBJ__OBJ_FILE_READER__INCLUDED

#include <string>
#include <fstream>

#include <mesh/Mesh.h>
#include <mesh/Types.h>
#include <cgmath/Vector2f.h>
#include <cgmath/Vector3f.h>

#include "ObjMaterialMap.h"

namespace meshobj {

// ObjFileReader
//
// This class provides a context for reading a Wavefront OBJ file.

class ObjFileReader
{
public:
    ObjFileReader();
    ~ObjFileReader();

    // Load a Mesh object from a Wavefront OBJ file. If the OBJ file
    // cannot be opened, an OpenFileException is thrown. If the file
    // contains errors, a FailedOperationException is thrown.
    void readObjFile(mesh::Mesh *mesh, const std::string &objFilename);

private:
    // A map of material names to mesh material indices.
    typedef std::map<const std::string, int> MeshMaterialIndexMap;

    // A map of texture names to mesh texture indices.
    typedef std::map<const std::string, int> MeshTextureIndexMap;

    // A table of pointers to the vertices read in from the file.
    // This allows us to quickly reference them by index.
    typedef std::vector<mesh::VertexPtr> ObjVertexTable;

    // A table of normal vertices.
    typedef std::vector<cgmath::Vector3f> ObjNormalVertexTable;

    // A table of texture vertices.
    typedef std::vector<cgmath::Vector2f> ObjTextureVertexTable;

    // Free up all of the memory used by the class.
    void reset();
    
    // Read a line that starts with an "f" keyword.
    void readFace(std::istringstream &istr);

    // Read a face vertex token.
    bool readFaceVertex(std::istringstream &istr,
        int &geometricVertexIndex, int &textureVertexIndex,
        int &normalVertexIndex);

    // Obtain a material index, given an OBJ material name. If no
    // matching material exists, create a new one. When this function
    // creates new material indices, it also adds their definitions to
    // the specified Mesh as Mesh attributes.
    int findOrCreateMaterialIndex(const std::string &objMaterialName);

    // Obtain a texture index, given an OBJ texture file name. If no
    // matching texture exists, create a new one, and add it to the
    // Mesh's attribute-based texture table.
    int findOrCreateTextureIndex(const std::string &objTextureName);

    // Read a line from an OBJ file that starts with a "mtllib" keyword,
    // and the MTL file that it references.
    void readMaterialLibrary(std::string mtlFilename);
    
    // Read a line that starts with the "usemtl" keyword.
    void readMaterial(std::istringstream &istr, 
        std::string &objMaterialName);
    
    // If materialFound is not true, throw an exception 
    // with an error message.
    void verifyMaterialFound(bool materialFound, 
        const std::string &mtlFilename, unsigned int errorLineNumber);

    // Read the next line from the file. Lines that end in backslashes
    // are concatenated with the lines that follow them.
    std::string readLine(std::ifstream &file, 
        unsigned int &currentLineNumber);

    // Read a string following a keyword.
    void readString(std::istringstream &istr, 
        const std::string &filename, unsigned int errorLineNumber,
        std::string &string);

    // Read a scalar following a keyword.
    void readFloat(std::istringstream &istr, 
        const std::string &filename, unsigned int errorLineNumber,
        float &scalar);

    // Read a vector following a keyword.
    void readVector3f(std::istringstream &istr, 
        const std::string &filename, unsigned int errorLineNumber,
        cgmath::Vector3f &vector3f);

    // Read a line that starts with a "v" keyword.
    void readVertex(std::istringstream &istr);
    
    // Read a line that starts with a "vt" keyword.
    void readTextureVertex(std::istringstream &istr);
    
    // Read a line that starts with a "vn" keyword.
    void readNormalVertex(std::istringstream &istr);
    
    // Get the VertexPtr corresponding to a geometric vertex index.
    mesh::VertexPtr getVertexPtrFromGeometricVertexIndex(
        int geometricVertexIndex);

    // Get the texture coordinate corresponding to a texture vertex index.
    cgmath::Vector2f getVector2fFromTextureVertexIndex(
        int textureVertexIndex);

    // Get the normal vector corresponding to a normal vertex index.
    cgmath::Vector3f getVector3fFromNormalVertexIndex(
        int normalVertexIndex);
    
    std::ifstream *mFile;
    mesh::Mesh mMesh;
    std::string mObjFilename;
    unsigned int mErrorLineNumber;
    ObjVertexTable mObjVertexTable;
    ObjNormalVertexTable mObjNormalVertexTable;
    ObjTextureVertexTable mObjTextureVertexTable;
    ObjMaterialMap mObjMaterialMap;
    MeshMaterialIndexMap mMeshMaterialIndexMap;
    MeshTextureIndexMap mMeshTextureIndexMap;
    int mMeshMaterialIndex;
    int mMeshTextureIndex;
    int mMaterialCount;
    int mTextureCount;
};

} // namespace meshobj

#endif // MESHOBJ__OBJ_FILE_READER__INCLUDED
