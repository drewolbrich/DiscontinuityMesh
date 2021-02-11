// Copyright 2009 Drew Olbrich

#include "DegenerateEdgeCollapser.h"

#include <cstdlib>
#include <cassert>
#include <vector>

#include <except/FailedOperationException.h>
#include <cgmath/Tolerance.h>

#include "Mesh.h"
#include "MeshOperations.h"
#include "EdgeOperations.h"
#include "DeletedElementTracker.h"

namespace mesh {

DegenerateEdgeCollapser::DegenerateEdgeCollapser()
    : mMesh(NULL),
      mAbsoluteTolerance(cgmath::TOLERANCE),
      mRelativeTolerance(cgmath::TOLERANCE),
      mCollapsedEdgeCount(0)
{
}

DegenerateEdgeCollapser::~DegenerateEdgeCollapser()
{
}

void
DegenerateEdgeCollapser::setMesh(Mesh *mesh)
{
    mMesh = mesh;
}

Mesh *
DegenerateEdgeCollapser::mesh() const
{
    return mMesh;
}

void
DegenerateEdgeCollapser::setAbsoluteTolerance(float absoluteTolerance)
{
    mAbsoluteTolerance = absoluteTolerance;
}

float
DegenerateEdgeCollapser::absoluteTolerance() const
{
    return mAbsoluteTolerance;
}

void
DegenerateEdgeCollapser::setRelativeTolerance(float relativeTolerance)
{
    mRelativeTolerance = relativeTolerance;
}

float
DegenerateEdgeCollapser::relativeTolerance() const
{
    return mRelativeTolerance;
}

void
DegenerateEdgeCollapser::collapseDegenerateEdges()
{
    int count = 0;

    mCollapsedEdgeCount = 0;
    while (removeDegenerateEdges()) {
        // Keep looping as long as there are degenerate edges to remove.

        ++count;
        if (count > 100) {
            // If this happens, we've encountered an unexpected geometric
            // configuration that cannot be resolved.
            // This should probably never happen.
            throw except::FailedOperationException(SOURCE_LINE) << "Exceeded loop limit "
                << "in degenerate edge collapser.";
        }
    }
}

unsigned
DegenerateEdgeCollapser::collapsedEdgeCount() const
{
    return mCollapsedEdgeCount;
}

bool
DegenerateEdgeCollapser::removeDegenerateEdges()
{
    // The current implementation of CollapseEdge can't handle nontriangular faces.
    assert(AllFacesAreTriangles(*mMesh));

    // Put all the degenerate edges in a vector so we won't have to deal with
    // invalidated iterators as we delete them.
    std::vector<EdgePtr> edgePtrVector;
    for (EdgePtr edgePtr = mMesh->edgeBegin(); edgePtr != mMesh->edgeEnd(); ++edgePtr) {
        cgmath::Vector3f p0;
        cgmath::Vector3f p1;
        GetEdgeVertexPositions(edgePtr, &p0, &p1);
        if ((p1 - p0).length() <= GetEpsilonFromEdge(edgePtr, 
                mAbsoluteTolerance, mRelativeTolerance)) {
            edgePtrVector.push_back(edgePtr);
        }
    }

    if (edgePtrVector.empty()) {
        // There are no degenerate edges.
        return false;
    }

    // Collapse the edges, but skip over edges that are removed as a result
    // of edge collapses.
    DeletedElementTracker deletedElementTracker;
    for (size_t index = 0; index < edgePtrVector.size(); ++index) {
        EdgePtr edgePtr = edgePtrVector[index];
        if (!deletedElementTracker.hasEdge(edgePtr)) {

            VertexPtr v0;
            VertexPtr v1;
            GetEdgeAdjacentVertices(edgePtr, &v0, &v1);
            cgmath::Vector3f midpoint = (v0->position() + v1->position())*0.5;

            CollapseEdge(mMesh, edgePtr, &deletedElementTracker);

            // Replace the collapsed edge with a vertex at the degenerate edge's midpoint.
            v1->setPosition(midpoint);

            ++mCollapsedEdgeCount;
        }
    }

    return true;
}

} // namespace mesh
