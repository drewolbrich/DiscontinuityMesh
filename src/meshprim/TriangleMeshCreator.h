// Copyright 2009 Retroactive Fiasco.

#ifndef MESHPRIM__TRIANGLE_MESH_CREATOR__INCLUDED
#define MESHPRIM__TRIANGLE_MESH_CREATOR__INCLUDED

#include <vector>

#include <cgmath/Vector3f.h>
#include <mesh/AttributePossessor.h>
#include <mesh/Types.h>

namespace cgmath {
class Matrix4f;
}

namespace mesh {
class Mesh;
}

namespace meshprim {

// TriangleMeshCreator
//
// Adds a mesh of triangles to a mesh, as defined by a vector of vertex positions
// a vector of faces, specified by vertex index triplets.

class TriangleMeshCreator
{
public:
    TriangleMeshCreator();
    ~TriangleMeshCreator();

    // The mesh to add the cylinder to.
    void setMesh(mesh::Mesh *mesh);
    mesh::Mesh *mesh() const;

    // The transformation matrix to apply to the cylinder's vertices
    // before adding them to the mesh.
    void setTransformationMatrix(const cgmath::Matrix4f &transformationMatrix);
    const cgmath::Matrix4f &transformationMatrix() const;

    // The positions of the vertices in the mesh.
    typedef std::vector<cgmath::Vector3f> VertexPositionVector;
    void setVertexPositionVector(const VertexPositionVector *vertexPositionVector);
    const VertexPositionVector *vertexPositionVector() const;

    // The triangles, defined as a vector of vertex index triplets,
    // referencing the indices of the vertices in the vertex position vector.
    struct Triangle {
        Triangle(size_t v0, size_t v1, size_t v2) {
            mVertexIndexArray[0] = v0;
            mVertexIndexArray[1] = v1;
            mVertexIndexArray[2] = v2;
        }
        size_t mVertexIndexArray[3];
    };
    typedef std::vector<Triangle> TriangleVector;
    void setTriangleVector(const TriangleVector *triangleVector);
    const TriangleVector *triangleVector() const;

    // Optional attributes to apply to each face.
    void setFaceAttributes(const mesh::AttributePossessor &faceAttributes);
    const mesh::AttributePossessor &faceAttributes() const;

    // If true, faceted normals will be assigned.
    void setShouldAssignNormals(bool shouldAssignNormals);
    bool shouldAssignNormals() const;

    // Create the vertex, edges, and faces and add them to the mesh.
    void create();

    // The array of created vertices. The order of these vertices correspond
    // to order of the input vector of vertex positions.
    typedef std::vector<mesh::VertexPtr> VertexPtrVector;
    const VertexPtrVector &vertexPtrVector() const;

    // The array of created edges.
    typedef std::vector<mesh::EdgePtr> EdgePtrVector;
    const EdgePtrVector &edgePtrVector() const;

    // The array of created faces.
    typedef std::vector<mesh::FacePtr> FacePtrVector;
    const FacePtrVector &facePtrVector() const;

private:
    mesh::Mesh *mMesh;
    cgmath::Matrix4f mTransformationMatrix;
    const VertexPositionVector *mVertexPositionVector;
    const TriangleVector *mTriangleVector;
    mesh::AttributePossessor mFaceAttributes;
    bool mShouldAssignNormals;

    VertexPtrVector mVertexPtrVector;
    EdgePtrVector mEdgePtrVector;
    FacePtrVector mFacePtrVector;
};

} // namespace meshprim

#endif // MESHPRIM__TRIANGLE_MESH_CREATOR__INCLUDED
