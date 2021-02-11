// Copyright 2010 Drew Olbrich

#ifndef MESH__DEGENERATE_FACE_COLLAPSER__INCLUDED
#define MESH__DEGENERATE_FACE_COLLAPSER__INCLUDED

#include "Types.h"

namespace mesh {

class Mesh;

// DegenerateFaceCollapser
//
// Filters out degenerate faces from a mesh.

class DegenerateFaceCollapser
{
public:
    DegenerateFaceCollapser();
    ~DegenerateFaceCollapser();

    // The mesh to filter.
    void setMesh(Mesh *mesh);
    Mesh *mesh() const;

    // Absolute tolerance for detecting degenerate faces.
    void setAbsoluteTolerance(float absoluteTolerance);
    float absoluteTolerance() const;

    // Relative tolerance for detecting degenerate faces.
    void setRelativeTolerance(float relativeTolerance);
    float relativeTolerance() const;

    // Remove degenerate faces from the mesh.
    // This function may throw a FailedOperationException if an unexpected geometric
    // configuration causes it to get stuck in an infinite loop.
    void collapseDegenerateFaces();

    // Returns how many faces were collapsed.
    unsigned collapsedFaceCount() const;

private:
    bool removeDegenerateFaces();

    void testForMultipleEdgesSharingVertices();

    Mesh *mMesh;
    float mAbsoluteTolerance;
    float mRelativeTolerance;

    unsigned mCollapsedFaceCount;
};

} // namespace mesh

#endif // MESH__DEGENERATE_FACE_COLLAPSER__INCLUDED
