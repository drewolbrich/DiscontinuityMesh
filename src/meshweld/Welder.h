// Copyright 2009 Retroactive Fiasco.

#ifndef MESHWELD__WELDER__INCLUDED
#define MESHWELD__WELDER__INCLUDED

#include <vector>

#include <mesh/Mesh.h>

#include "EdgeAabbTree.h"

namespace meshweld {

// Welder
//
// Locates perimeter edges of the mesh (edges with only one adjacent face)
// and merges them with other perimeter edges within a small distance.
// Vertices on the perimeter are also merged.

class Welder
{
public:
    Welder();
    ~Welder();

    // The mesh that's going to be welded.
    void setMesh(mesh::Mesh *mesh);
    mesh::Mesh *mesh() const;

    // Absolute tolerance to use when testing for coincident vertices.
    void setAbsoluteTolerance(float absoluteTolerance);
    float absoluteTolerance() const;

    // Relative tolerance. When large values are compared, the relative tolerance
    // supercedes the absolute tolerance.
    void setRelativeTolerance(float relativeTolerance);
    float relativeTolerance() const;

    // If this bool attribute key is defined, only edges with this attribute
    // set to true will be considered for merging. Otherwise, only
    // perimeter edges of the mesh will be considered.
    void setCandidateEdgeAttributeKey(const mesh::AttributeKey &candidateEdgeAttributeKey);
    const mesh::AttributeKey &candidateEdgeAttributeKey() const;

    // If true, all edges are candidates for merging.
    void setMergeAny(bool mergeAny);
    bool mergeAny() const;

    // If true, mark faces with colors for debugging.
    // Faces of each region will be flagged with a different color.
    void setMarkFacesWithDebugColors(bool markFacesWithDebugColors);
    bool markFacesWithDebugColors() const;

    // Welds perimeter of the mesh together.
    void weld();

    // Return the number of edges that were removed via merging into other edges.
    unsigned mergedEdges() const;

private:
    void floodFillRegion(mesh::EdgePtr edgePtr, int regionId);

    cgmath::Vector3f convertRegionIdToColor(int regionId) const;

    void buildEdgeAabbTree();

    void weldEdges(float absoluteTolerance, float relativeTolerance);

    void swapEdgeVertices(mesh::EdgePtr edgePtr);

    // Returns true if the edge is allowed to be merged.
    bool edgeCanBeMerged(mesh::EdgePtr edgePtr);

    mesh::Mesh *mMesh;

    float mAbsoluteTolerance;
    float mRelativeTolerance;

    mesh::AttributeKey mCandidateEdgeAttributeKey;

    bool mMergeAny;

    bool mMarkFacesWithDebugColors;

    mesh::AttributeKey mRegionIdAttributeKey;
    mesh::AttributeKey mColor3fAttributeKey;

    EdgeAabbTree mEdgeAabbTree;

    unsigned mMergedEdges;
};

} // namespace meshweld

#endif // MESHWELD__WELDER__INCLUDED
