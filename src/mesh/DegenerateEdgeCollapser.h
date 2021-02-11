// Copyright 2009 Drew Olbrich

#ifndef MESH__DEGENERATE_EDGE_COLLAPSER__INCLUDED
#define MESH__DEGENERATE_EDGE_COLLAPSER__INCLUDED

namespace mesh {

class Mesh;

// DegenerateEdgeCollapser
//
// Filters out degenerate edges from a mesh.

class DegenerateEdgeCollapser
{
public:
    DegenerateEdgeCollapser();
    ~DegenerateEdgeCollapser();

    // The mesh to filter.
    void setMesh(Mesh *mesh);
    Mesh *mesh() const;

    // Absolute tolerance for detecting degenerate edges.
    void setAbsoluteTolerance(float absoluteTolerance);
    float absoluteTolerance() const;

    // Relative tolerance for detecting degenerate edges.
    void setRelativeTolerance(float relativeTolerance);
    float relativeTolerance() const;

    // Remove degenerate edges from the mesh.
    // This function may throw a FailedOperationException if an unexpected geometric
    // configuration causes it to get stuck in an infinite loop.
    void collapseDegenerateEdges();

    // Returns how many edges were collapsed.
    unsigned collapsedEdgeCount() const;

private:
    bool removeDegenerateEdges();

    Mesh *mMesh;
    float mAbsoluteTolerance;
    float mRelativeTolerance;

    unsigned mCollapsedEdgeCount;
};

} // namespace mesh

#endif // MESH__DEGENERATE_EDGE_COLLAPSER__INCLUDED
