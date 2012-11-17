// Copyright 2010 Retroactive Fiasco.

#ifndef MESH__SPLIT_EDGE_TRIANGULATOR__INCLUDED
#define MESH__SPLIT_EDGE_TRIANGULATOR__INCLUDED

#include "Types.h"
#include "AttributeKey.h"

namespace mesh {

class Mesh;
class DeletedElementTracker;

// SplitEdgeTriangulator
//
// This class triangulates triangular faces of a mesh whose edges
// may or may not have been split. Vertices created as a result of edge splitting
// must be marked with the boolean attribute returned by 
// the function SplitEdgeTriangulator::splitEdgeVertexAttributeKey.
//
// Unlike Triangulator, this class can handle degenerate faces.

class SplitEdgeTriangulator
{
public:
    SplitEdgeTriangulator();
    ~SplitEdgeTriangulator();

    // The mesh to triangulate.
    void setMesh(Mesh *mesh);
    Mesh *mesh() const;

    // Optional DeletedElementTracker, to track newly created mesh elements
    // and the deleted faces that they replace.
    void setDeletedElementTracker(DeletedElementTracker *deletedElementTracker);
    DeletedElementTracker *deletedElementTracker() const;

    // Must be called after setting the mesh.
    void initialize();

    // The attribute key that should be assigned to vertices that
    // result from edge splitting. This function must only be called
    // after 'initialize' is called.
    const AttributeKey &splitEdgeVertexAttributeKey() const;

    // Triangulate the mesh.
    void triangulate();

private:
    void triangulateFace(FacePtr facePtr);

    Mesh *mMesh;
    DeletedElementTracker *mElementTracker;
    AttributeKey mSplitEdgeVertexAttributeKey;
};

} // namespace mesh

#endif // MESH__SPLIT_EDGE_TRIANGULATOR__INCLUDED
