// Copyright 2009 Retroactive Fiasco.

#ifndef MESHPRIM__CYLINDER_CREATOR__INCLUDED
#define MESHPRIM__CYLINDER_CREATOR__INCLUDED

#include <mesh/AttributePossessor.h>

namespace cgmath {
class Vector3f;
class Matrix4f;
}

namespace mesh {
class Mesh;
}

namespace meshprim {

// CylinderCreator
//
// Adds geometry in the form of a cylinder to a mesh.

class CylinderCreator
{
public:
    CylinderCreator();
    ~CylinderCreator();

    // The mesh to add the cylinder to.
    void setMesh(mesh::Mesh *mesh);
    mesh::Mesh *mesh() const;

    // The transformation matrix to apply to the cylinder's vertices
    // before adding them to the mesh.
    void setTransformationMatrix(const cgmath::Matrix4f &transformationMatrix);
    const cgmath::Matrix4f &transformationMatrix() const;

    // The height of the cylinder, along the object space Y axis.
    void setHeight(float height);
    float height() const;

    // The radius of the cylinder, in the object space XZ plane.
    void setRadius(float radius);
    float radius() const;

    // The number of polygonal divisions around the cylinder.
    void setDivisions(const unsigned &divisions);
    const unsigned &divisions() const;

    // Optional attributes to apply to each face.
    void setFaceAttributes(const mesh::AttributePossessor &faceAttributes);
    const mesh::AttributePossessor &faceAttributes() const;

    // If true, normals will be assigned.
    void setShouldAssignNormals(bool shouldAssignNormals);
    bool shouldAssignNormals() const;

    // If true, texture coordinates will be assigned.
    void setShouldAssignTextureCoordinates(bool shouldAssignTextureCoordinates);
    bool shouldAssignTextureCoordinates() const;

    // Create the vertex, edges, and faces of the cylinder and add them to the mesh.
    void create();

private:
    mesh::Mesh *mMesh;
    cgmath::Matrix4f mTransformationMatrix;
    float mHeight;
    float mRadius;
    unsigned mDivisions;
    mesh::AttributePossessor mFaceAttributes;
    bool mShouldAssignNormals;
    bool mShouldAssignTextureCoordinates;
};

} // namespace meshprim

#endif // MESHPRIM__CYLINDER_CREATOR__INCLUDED
