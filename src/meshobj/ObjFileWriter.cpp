// Copyright 2008 Retroactive Fiasco.

#include "ObjFileWriter.h"

#include <cassert>
#include <iostream>
#include <sstream>

#include <stdint.h>

#include <mesh/Mesh.h>
#include <mesh/Vertex.h>
#include <mesh/StandardAttributes.h>
#include <mesh/IsConsistent.h>
#include <cgmath/Vector2f.h>
#include <cgmath/Vector3f.h>
#include <cgmath/Vector4f.h>
#include <except/FailedOperationException.h>
#include <except/OpenFileException.h>
#include <os/Error.h>
#include <str/FilenameOperations.h>

using mesh::VertexPtr;
using mesh::ConstVertexPtr;
using mesh::ConstFacePtr;
using mesh::AdjacentVertexConstIterator;
using mesh::AttributeKey;
using cgmath::Vector2f;
using cgmath::Vector3f;
using cgmath::Vector4f;
using except::FailedOperationException;
using except::OpenFileException;
using os::Error;

namespace meshobj {

ObjFileWriter::ObjFileWriter()
    : mFile(),
      mMesh(NULL),
      mObjFilename(),
      mObjectName(),
      mMaterialNameSet(),
      mVertexPtrIndexMap(),
      mNormalIndexMap(),
      mTexCoordIndexMap()
{
}

ObjFileWriter::~ObjFileWriter()
{
}

void 
ObjFileWriter::writeObjFile(const mesh::Mesh &mesh, 
    const std::string &objFilename)
{
    reset();

    mMesh = &mesh;
    
    // Make sure the references between the mesh components
    // are consistent. (Only has an effect in debug builds.)
    assert(mesh::IsConsistent(*mMesh));

    mObjFilename = objFilename;
    
    // Try to open the OBJ file. By declaring the std::ofstream in
    // local storage, and then referencing it throughout the class via
    // a pointer, we ensure that the file will be closed if an
    // exception is thrown.
    std::ofstream file(mObjFilename.c_str());
    mFile = &file;

    if (!*mFile) {
        throw OpenFileException(SOURCE_LINE, Error::fromSystemError())
            << "Could not open file \"" << mObjFilename << "\".";
    }

    writeObjectName();
    writeMaterials();
    writeVertexPositions();
    writeNormalVertices();
    writeTextureVertices();
    writeFaces();

    reset();
}

void 
ObjFileWriter::reset()
{
    mFile = NULL;
    mMesh = NULL;
    mObjFilename = "";
    mMtlFilename = "";
    mObjectName = "";
    mMaterialNameSet.clear();
    mVertexPtrIndexMap.clear();
    mNormalIndexMap.clear();
    mTexCoordIndexMap.clear();
}

void 
ObjFileWriter::writeObjectName()
{
    mObjectName = str::RemovePathFromFilename(
        str::RemoveFilenameExtension(mObjFilename));
    *mFile << "o " << mObjectName << std::endl;

    if (mFile->fail()) {
        throw FailedOperationException(SOURCE_LINE, Error::fromSystemError())
            << "Could not write object name to file \"" 
                << mObjFilename << "\".";
    }
}

void 
ObjFileWriter::writeFaces()
{
    AttributeKey materialIndexKey = mesh::GetMaterialIndexAttributeKey(*mMesh);
    AttributeKey textureIndexKey = mesh::GetTextureIndexAttributeKey(*mMesh);
    AttributeKey normal3fKey = mesh::GetNormal3fAttributeKey(*mMesh);
    AttributeKey texCoord2fKey = mesh::GetTexCoord2fAttributeKey(*mMesh);

    // Loop over all the Faces in the Mesh.
    static int lastMaterialIndex = -1;
    static int lastTextureIndex = -1;
    for (ConstFacePtr facePtr = mMesh->faceBegin();
         facePtr != mMesh->faceEnd(); ++facePtr) {

        int materialIndex = facePtr->getInt(materialIndexKey);
        if (facePtr->hasAttribute(materialIndexKey)
            && mesh::HasMaterialDefinition(*mMesh, materialIndex)) {

            // Only write out the material reference if it's
            // different than the one that the last face had.
            if (materialIndex != lastMaterialIndex) {

                AttributeKey materialNameKey 
                    = mesh::GetMaterialNameAttributeKey(*mMesh, materialIndex);
                const std::string &materialName = mMesh->getString(materialNameKey);
                if (mMesh->hasAttribute(materialNameKey)) {
                    *mFile << "usemtl " << materialName << std::endl;
                } else {
                    *mFile << "usemtl " 
                        << getMaterialName(materialIndex) << std::endl;
                }
                lastMaterialIndex = materialIndex;
            }

        } else {

            if (lastMaterialIndex != -1) {
                // This face has no known material assignment
                // defined, so request the default material.
                *mFile << "usemtl" << std::endl;
                lastMaterialIndex = -1;
            }
        }

        int textureIndex = facePtr->getInt(textureIndexKey);
        if (facePtr->hasAttribute(textureIndexKey)
            && mesh::HasTextureDefinition(*mMesh, textureIndex)) {

            // Only write out the texture reference if it's
            // different than the one that the last face had.
            if (textureIndex != lastTextureIndex) {

                AttributeKey textureFilenameKey = mesh::GetTextureFilenameAttributeKey(
                    *mMesh, textureIndex);
                if (mMesh->hasAttribute(textureFilenameKey)) {
                    std::string textureFilename = mMesh->getString(textureFilenameKey);

                    // Localize the texture filename's path relative
                    // to the OBJ filename's path. If the OBJ filename
                    // is "foo/baz.obj", and the texture filename's
                    // path is "foo/bar/baz.jpg", then the localized
                    // texture filename will be "bar/baz.jpg".
                    // This is required because the texture file is
                    // effectively included by the OBJ filename,
                    // and it must be evaluated relative to the OBJ
                    // file's local directory.
                    textureFilename = str::LocalizeFilename(textureFilename,
                        str::GetFilenamePath(mObjFilename));
                    
                    *mFile << "usemap " << textureFilename << std::endl;
                }
                lastTextureIndex = textureIndex;
            } 

        } else {

            if (lastTextureIndex != -1) {
                // This face has no known texture assignment
                // defined, so request the default texture.
                *mFile << "usemap off" << std::endl;
                lastTextureIndex = -1;
            }
        }
        
        // Only write out the face if it actually has vertices.
        if (facePtr->adjacentVertexCount() > 0) {
            *mFile << "f";

#ifdef DEBUG
            // According to the OBJ file documentation, it's an error
            // if some, but not all, of the face vertices have normals
            // or texture coordinates. The current version of the code
            // checks for this situation in debug mode, and aborts, as
            // if this was a programmer error in the creation of the
            // data. In the future, the code could be modified to
            // handle this case a little more gracefully, by inserting
            // placeholder normals or texture coordinates. However,
            // it really is an undefined situation.

            // Count the number of face vertices, and the number
            // of face vertices that have normals and texture
            // coordinates.
            int vertexCount = 0;
            int normalCount = 0;
            int texCoordCount = 0;
            for (AdjacentVertexConstIterator iterator = facePtr->adjacentVertexBegin();
                 iterator != facePtr->adjacentVertexEnd(); ++iterator) {
                const VertexPtr &vertexPtr = *iterator;
                ++vertexCount;
                if (facePtr->hasVertexAttribute(vertexPtr, texCoord2fKey)) {
                    ++texCoordCount;
                }
                if (facePtr->hasVertexAttribute(vertexPtr, normal3fKey)) {
                    ++normalCount;
                }
            }

            // Either all of the face vertices must have normals,
            // or none of them must have normals.
            assert(normalCount == vertexCount || normalCount == 0);

            // Either all of the face vertices must have texture
            // coordinates, or none of them must have texture
            // coordinate.
            assert(texCoordCount == vertexCount || texCoordCount == 0);
#endif
            
            // Loop over all the Face's adjacent vertices.
            for (AdjacentVertexConstIterator iterator = facePtr->adjacentVertexBegin();
                 iterator != facePtr->adjacentVertexEnd(); ++iterator) {
                const VertexPtr &vertexPtr = *iterator;

                // The VertexPtr must exist in the map.
                VertexPtrIndexMap::iterator vertexPtrIndexMapIterator
                    = mVertexPtrIndexMap.find(vertexPtr);
                assert(vertexPtrIndexMapIterator != mVertexPtrIndexMap.end());

                uint32_t vertexIndex = (*vertexPtrIndexMapIterator).second;
                *mFile << " " << vertexIndex;

                // Write out texture coordinates.
                if (facePtr->hasVertexAttribute(vertexPtr, texCoord2fKey)) {
                    const Vector2f &texCoord = facePtr->getVertexVector2f(
                        vertexPtr, texCoord2fKey);
                    TexCoordIndexMap::iterator texCoordIndexMapIterator
                        = mTexCoordIndexMap.find(texCoord);
                    assert(texCoordIndexMapIterator != mTexCoordIndexMap.end());

                    uint32_t texCoordIndex = (*texCoordIndexMapIterator).second;
                    *mFile << "/" << texCoordIndex;
                }

                // Write out the vertex normal.
                bool normalIsDefined = false;
                Vector3f normal;
                if (facePtr->hasVertexAttribute(vertexPtr, normal3fKey)) {
                    normalIsDefined = true;
                    normal = facePtr->getVertexUnitVector3f(vertexPtr, normal3fKey);
                } else if (facePtr->hasAttribute(normal3fKey)) {
                    normalIsDefined = true;
                    normal = facePtr->getUnitVector3f(normal3fKey);
                }
                if (normalIsDefined) {
                    NormalIndexMap::iterator normalIndexMapIterator = mNormalIndexMap.find(normal);
                    assert(normalIndexMapIterator != mNormalIndexMap.end());

                    // If we didn't yet write out a texture coordinate
                    // for this vertex, an extra slash must be
                    // output to account for it's absence.
                    if (!facePtr->hasVertexAttribute(vertexPtr, texCoord2fKey)) {
                        *mFile << "/";
                    }
                    
                    uint32_t normalIndex = (*normalIndexMapIterator).second;
                    *mFile << "/" << normalIndex;
                }
            }
            
            *mFile << std::endl;
            
            if (mFile->fail()) {
                throw FailedOperationException(SOURCE_LINE, Error::fromSystemError())
                    << "Could not write face to file \"" 
                        << mObjFilename << "\".";
            }
        }
    }
}

void 
ObjFileWriter::writeMaterials()
{
    AttributeKey materialIndexKey = mesh::GetMaterialIndexAttributeKey(*mMesh);

    // Loop over all the Faces in the Mesh.
    bool foundMaterialIndex = false;
    for (ConstFacePtr facePtr = mMesh->faceBegin();
         facePtr != mMesh->faceEnd(); ++facePtr) {
        if (facePtr->hasAttribute(materialIndexKey)) {
            int materialIndex = facePtr->getInt(materialIndexKey);
            if (mesh::HasMaterialDefinition(*mMesh, materialIndex)) {
                foundMaterialIndex = true;
                break;
            }
        }
    }

    // If none of the faces have material assignments,
    // do nothing.
    if (!foundMaterialIndex) {
        return;
    }

    // Create a set of all of the unique material indices
    // that appear in the file.
    typedef std::set<int> MaterialIndexSet;
    MaterialIndexSet materialIndexSet;
    for (ConstFacePtr facePtr = mMesh->faceBegin();
         facePtr != mMesh->faceEnd(); ++facePtr) {
        if (facePtr->hasAttribute(materialIndexKey)) {
            int materialIndex = facePtr->getInt(materialIndexKey);
            if (mesh::HasMaterialDefinition(*mMesh, materialIndex)) {

                // Add this material index to the set of
                // unique material indices encountered.
                std::pair<MaterialIndexSet::iterator, bool> result
                    = materialIndexSet.insert(materialIndex);

                // If this is the first time the material index
                // was added to the set, check if the corresponding
                // material definition has a name defined.
                // If so, add it to the set of material names
                // that are explicitly defined. We'll use this set
                // to ensure that our procedurally generated
                // default material names don't conflict with the
                // explicitly defined names.
                if (result.second) {
                    AttributeKey materialNameKey = mesh::GetMaterialNameAttributeKey(*mMesh,
                        materialIndex);
                    if (mMesh->hasAttribute(materialNameKey)) {
                        const std::string &materialName(mMesh->getString(materialNameKey));
                        mMaterialNameSet.insert(materialName);
                    }
                }
            }
        }
    }
    
    // Try to open the MTL file.
    mMtlFilename = str::RemoveFilenameExtension(mObjFilename) + ".mtl";
    std::ofstream mtlFile(mMtlFilename.c_str());
    if (!mtlFile) {
        throw OpenFileException(SOURCE_LINE, Error::fromSystemError())
            << "Could not open file \"" << mMtlFilename << "\".";
    }

    for (MaterialIndexSet::const_iterator iterator = materialIndexSet.begin();
         iterator != materialIndexSet.end(); ++iterator) {
        int materialIndex = *iterator;
        writeMaterial(mtlFile, materialIndex);
    }

    mtlFile.close();

    // Write the material library file reference into the OBJ file,
    // but remove the path from the MTL filename, because the MTL
    // filename as it appears in the OBJ file should be relative to
    // the OBJ file's directory, and not the directory that the
    // application is running in (which is the case for the original
    // OBJ filename).
    *mFile << "mtllib " << str::RemovePathFromFilename(mMtlFilename) << std::endl;

    if (mFile->fail()) {
        throw FailedOperationException(SOURCE_LINE, Error::fromSystemError())
            << "Could not write material library reference to file \"" 
                << mObjFilename << "\".";
    }
}

void 
ObjFileWriter::writeMaterial(std::ofstream &file, int materialIndex)
{
    AttributeKey materialNameKey = mesh::GetMaterialNameAttributeKey(*mMesh, materialIndex);

    const std::string &materialName = mMesh->getString(materialNameKey);
    if (mMesh->hasAttribute(materialNameKey)) {
        file << "newmtl " << materialName << std::endl;
    } else {
        file << "newmtl " << getMaterialName(materialIndex) << std::endl;
    }

    AttributeKey ambient4fKey = mesh::GetMaterialAmbient4fAttributeKey(*mMesh, materialIndex);
    AttributeKey diffuse4fKey  = mesh::GetMaterialDiffuse4fAttributeKey(*mMesh, materialIndex);
    AttributeKey specular4fKey = mesh::GetMaterialSpecular4fAttributeKey(*mMesh, materialIndex);
    AttributeKey shininessKey = mesh::GetMaterialShininessAttributeKey(*mMesh, materialIndex);

    // Shininess.
    if (mMesh->hasAttribute(shininessKey)) {
        float shininess = mMesh->getFloat(shininessKey);
        file << "Ns " << shininess << std::endl;
    }

    // Ambient.
    if (mMesh->hasAttribute(ambient4fKey)) {
        const Vector4f &ambient = mMesh->getVector4f(ambient4fKey);
        file << "Ka " << ambient[0] << " " << ambient[1]
            << " " << ambient[2] << std::endl;
    }

    // Diffuse.
    if (mMesh->hasAttribute(diffuse4fKey)) {
        const Vector4f &diffuse = mMesh->getVector4f(diffuse4fKey);
        file << "Kd " << diffuse[0] << " " << diffuse[1]
            << " " << diffuse[2] << std::endl;
    }

    // Specular.
    if (mMesh->hasAttribute(specular4fKey)) {
        const Vector4f &specular = mMesh->getVector4f(specular4fKey);
        file << "Ks " << specular[0] << " " << specular[1]
            << " " << specular[2] << std::endl;
    }

    // Transparency.
    if (mMesh->hasAttribute(diffuse4fKey)) {
        const Vector4f &diffuse = mMesh->getVector4f(diffuse4fKey);
        file << "d " << diffuse[3] << std::endl;
    } else if (mMesh->hasAttribute(ambient4fKey)) {
        const Vector4f &ambient = mMesh->getVector4f(ambient4fKey);
        file << "d " << ambient[3] << std::endl;
    }
    
    // Illumination model.
    if (mMesh->hasAttribute(shininessKey)) {
        file << "illum 2 " << std::endl;
    } else {
        file << "illum 1 " << std::endl;
    }

    if (file.fail()) {
        throw FailedOperationException(SOURCE_LINE, Error::fromSystemError())
            << "Could not write material to file \"" 
                << mMtlFilename << "\".";
    }
}

std::string 
ObjFileWriter::getMaterialName(int materialIndex)
{
    std::string materialName;
    int suffix = 0;
    do {
        std::ostringstream ostr;
        ostr << mObjectName << "." << materialIndex;
        if (suffix > 0) {
            ostr << "." << suffix;
        }
        materialName = ostr.str();
        ++suffix;
    } while (mMaterialNameSet.find(materialName) != mMaterialNameSet.end());

    return materialName;
}

void
ObjFileWriter::writeVertexPositions()
{
    // According to the OBJ file documentation, the first 
    // vertex index is always 1.
    uint32_t index = 1;

    for (ConstVertexPtr vertexPtr = mMesh->vertexBegin();
         vertexPtr != mMesh->vertexEnd(); ++vertexPtr) {

        const Vector3f &position = vertexPtr->position();
        *mFile << "v " << position[0] << " " << position[1]
            << " " << position[2] << std::endl;

        if (mFile->fail()) {
            throw FailedOperationException(SOURCE_LINE, Error::fromSystemError())
                << "Could not write vertex to file \"" 
                    << mObjFilename << "\".";
        }
        
        mVertexPtrIndexMap.insert(std::make_pair(vertexPtr, index));
        ++index;
    }
}

void 
ObjFileWriter::writeNormalVertices()
{
    // According to the OBJ file documentation, the first 
    // vertex index is always 1.
    uint32_t index = 1;

    AttributeKey normal3fKey = mesh::GetNormal3fAttributeKey(*mMesh);

    // Loop over all the Faces in the Mesh.
    for (ConstFacePtr facePtr = mMesh->faceBegin();
         facePtr != mMesh->faceEnd(); ++facePtr) {

        // Loop over all the Face's adjacent vertices.
        for (AdjacentVertexConstIterator iterator = facePtr->adjacentVertexBegin();
             iterator != facePtr->adjacentVertexEnd(); ++iterator) {
            const VertexPtr &vertexPtr = *iterator;

            // Write out the face vertex normal, or face normal if no face vertex normal
            // is defined.
            bool normalIsDefined = false;
            Vector3f normal;
            if (facePtr->hasVertexAttribute(vertexPtr, normal3fKey)) {
                normal = facePtr->getVertexUnitVector3f(vertexPtr, normal3fKey);
                normalIsDefined = true;
            } else if (facePtr->hasAttribute(normal3fKey)) {
                normal = facePtr->getUnitVector3f(normal3fKey);
                normalIsDefined = true;
            }
            if (normalIsDefined) {
                // Only insert the normal into the map and write it
                // into the file if we haven't encountered a normal
                // with this value before.
                if (mNormalIndexMap.find(normal) == mNormalIndexMap.end()) {
                    *mFile << "vn " << normal[0] << " " << normal[1]
                        << " " << normal[2] << std::endl;

                    if (mFile->fail()) {
                        throw FailedOperationException(SOURCE_LINE, Error::fromSystemError())
                            << "Could not write normal vertex to file \"" 
                                << mObjFilename << "\".";
                    }

                    mNormalIndexMap.insert(std::make_pair(normal, index));
                    ++index;
                }
            }
        }
    }
}

void 
ObjFileWriter::writeTextureVertices()
{
    // According to the OBJ file documentation, the first 
    // vertex index is always 1.
    uint32_t index = 1;

    AttributeKey texCoord2fKey = mesh::GetTexCoord2fAttributeKey(*mMesh);

    // Loop over all the Faces in the Mesh.
    for (ConstFacePtr facePtr = mMesh->faceBegin(); 
         facePtr != mMesh->faceEnd(); ++facePtr) {

        // Loop over all the Face's adjacent vertices.
        for (AdjacentVertexConstIterator iterator = facePtr->adjacentVertexBegin();
             iterator != facePtr->adjacentVertexEnd(); ++iterator) {
            const VertexPtr &vertexPtr = *iterator;

            // Only attempt to write out the face's texture coordinate
            // vector if it has one defined.
            if (facePtr->hasVertexAttribute(vertexPtr, texCoord2fKey)) {
                const Vector2f &texCoord = facePtr->getVertexVector2f(
                    vertexPtr, texCoord2fKey);

                // Only insert the texture coordinate into the map and
                // write it into the file if we haven't encountered a
                // texture coordinate with this value before.
                if (mTexCoordIndexMap.find(texCoord) == mTexCoordIndexMap.end()) {
                    *mFile << "vt " << texCoord[0] << " " << texCoord[1]
                        << std::endl;

                    if (mFile->fail()) {
                        throw FailedOperationException(SOURCE_LINE, Error::fromSystemError())
                            << "Could not write texture vertex to file \"" 
                                << mObjFilename << "\".";
                    }

                    mTexCoordIndexMap.insert(std::make_pair(texCoord, index));
                    ++index;
                }
            }
        }
    }
}

} // namespace meshobj
