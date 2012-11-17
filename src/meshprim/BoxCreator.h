// Copyright 2009 Retroactive Fiasco.

#ifndef MESHPRIM__BOX_CREATOR__INCLUDED
#define MESHPRIM__BOX_CREATOR__INCLUDED

#include <mesh/AttributePossessor.h>

namespace cgmath {
class Vector3f;
class Matrix4f;
}

namespace mesh {
class Mesh;
}

namespace meshprim {

// BoxCreator
//
// Adds geometry in the form of a box to a mesh.

class BoxCreator
{
public:
    BoxCreator();
    ~BoxCreator();

    // The mesh to add the cylinder to.
    void setMesh(mesh::Mesh *mesh);
    mesh::Mesh *mesh() const;

    // The transformation matrix to apply to the cylinder's vertices
    // before adding them to the mesh.
    void setTransformationMatrix(const cgmath::Matrix4f &transformationMatrix);
    const cgmath::Matrix4f &transformationMatrix() const;

    // Size along X axis.
    void setWidth(float width);
    float width() const;

    // Size along Y axis.
    void setHeight(float height);
    float height() const;

    // Size along Z axis.
    void setDepth(float depth);
    float depth() const;

    // Optional attributes to apply to each face.
    void setFaceAttributes(const mesh::AttributePossessor &faceAttributes);
    const mesh::AttributePossessor &faceAttributes() const;

    // If true, normals will be assigned.
    void setShouldAssignNormals(bool shouldAssignNormals);
    bool shouldAssignNormals() const;

    // If true, texture coordinates will be assigned.
    void setShouldAssignTextureCoordinates(bool shouldAssignTextureCoordinates);
    bool shouldAssignTextureCoordinates() const;

    // Create the vertex, edges, and faces of the box and add them to the mesh.
    void create();

private:
    mesh::Mesh *mMesh;
    cgmath::Matrix4f mTransformationMatrix;
    float mWidth;
    float mHeight;
    float mDepth;
    mesh::AttributePossessor mFaceAttributes;
    bool mShouldAssignNormals;
    bool mShouldAssignTextureCoordinates;
};

} // namespace meshprim

#endif // MESHPRIM__BOX_CREATOR__INCLUDED
