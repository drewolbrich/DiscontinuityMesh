// Copyright 2010 Retroactive Fiasco.

#include "DegenerateFaceCollapser.h"

#include <cstdlib>
#include <cassert>
#include <vector>
#include <algorithm>

#include <except/FailedOperationException.h>
#include <cgmath/Tolerance.h>
#include <cgmath/LineOperations.h>

#include "Mesh.h"
#include "MeshOperations.h"
#include "VertexOperations.h"
#include "EdgeOperations.h"
#include "FaceOperations.h"
#include "DeletedElementTracker.h"
#include "IsConsistent.h"

namespace mesh {

DegenerateFaceCollapser::DegenerateFaceCollapser()
    : mMesh(NULL),
      mAbsoluteTolerance(cgmath::TOLERANCE),
      mRelativeTolerance(cgmath::TOLERANCE),
      mCollapsedFaceCount(0)
{
}

DegenerateFaceCollapser::~DegenerateFaceCollapser()
{
}

void
DegenerateFaceCollapser::setMesh(Mesh *mesh)
{
    mMesh = mesh;
}

Mesh *
DegenerateFaceCollapser::mesh() const
{
    return mMesh;
}

void
DegenerateFaceCollapser::setAbsoluteTolerance(float absoluteTolerance)
{
    mAbsoluteTolerance = absoluteTolerance;
}

float
DegenerateFaceCollapser::absoluteTolerance() const
{
    return mAbsoluteTolerance;
}

void
DegenerateFaceCollapser::setRelativeTolerance(float relativeTolerance)
{
    mRelativeTolerance = relativeTolerance;
}

float
DegenerateFaceCollapser::relativeTolerance() const
{
    return mRelativeTolerance;
}

void
DegenerateFaceCollapser::collapseDegenerateFaces()
{
    int count = 0;

    int initialFaceCount = mMesh->faceCount();
    mCollapsedFaceCount = 0;

    while (removeDegenerateFaces()) {
        // Keep looping as long as there are degenerate faces to remove.

        mCollapsedFaceCount = initialFaceCount - mMesh->faceCount();

        ++count;
        if (count > 100) {
            // If this happens, we've encountered an unexpected geometric
            // configuration that cannot be resolved.
            // This should probably never happen.
            throw except::FailedOperationException(SOURCE_LINE) << "Exceeded loop limit "
                << "in degenerate face collapser.";
        }
    }

    assert(IsConsistent(*mMesh));
}

unsigned
DegenerateFaceCollapser::collapsedFaceCount() const
{
    return mCollapsedFaceCount;
}

bool
DegenerateFaceCollapser::removeDegenerateFaces()
{
    // The current implementation of CollapseEdge can't handle nontriangular faces.
    assert(AllFacesAreTriangles(*mMesh));

    // Put all the degenerate faces in a vector so we won't have to deal with
    // invalidated iterators as we delete them.
    std::vector<FacePtr> facePtrVector;
    for (FacePtr facePtr = mMesh->faceBegin(); facePtr != mMesh->faceEnd(); ++facePtr) {
        float epsilon = GetEpsilonFromFace(facePtr, mAbsoluteTolerance, mRelativeTolerance);
        if (FaceIsDegenerate(facePtr, epsilon)) {
            facePtrVector.push_back(facePtr);
        }
    }

    if (facePtrVector.empty()) {
        // There are no degenerate faces.
        return false;
    }

    DeletedElementTracker deletedElementTracker;
    deletedElementTracker.setElementMask(DeletedElementTracker::EDGES 
        | DeletedElementTracker::FACES);

    for (size_t index = 0; index < facePtrVector.size(); ++index) {
        FacePtr facePtr = facePtrVector[index];
        if (deletedElementTracker.hasFace(facePtr)) {
            continue;
        }

        float epsilon = GetEpsilonFromFace(facePtr, mAbsoluteTolerance, mRelativeTolerance);

        if (!FaceIsDegenerate(facePtr, epsilon)) {
            // Manipulation of the mesh up to this point has caused
            // this face to lose its degenerate face status.
            continue;
        }

        // If the three vertices are nearly coincident,
        // collapse the face to a single point.
        if (AllFaceVerticesAreCoincident(facePtr, epsilon)) {
            CollapseFace(mMesh, facePtr, &deletedElementTracker);
            continue;
        }

        // If any edges are degenerate, collapse them.
        // We copy the face's adjacent edges into a vector so we can
        // iterate over them without worrying about if the face and its
        // adjacent edges get deleted as a result of collapsing an edge.
        std::vector<EdgePtr> edgePtrVector;
        std::copy(facePtr->adjacentEdgeBegin(), facePtr->adjacentEdgeEnd(), 
            std::back_inserter(edgePtrVector));
        bool collapsedEdge = false;
        for (size_t index = 0; index < edgePtrVector.size(); ++index) {
            EdgePtr edgePtr = edgePtrVector[index];
            if (!deletedElementTracker.hasEdge(edgePtr)) {
                if (GetEdgeLength(edgePtr) <= epsilon) {
                    // We use MergeVertexIntoVertex instead of CollapseEdge,
                    // so that it can handle the case when the edge's two vertices
                    // are shared by other edges.
                    VertexPtr v0;
                    VertexPtr v1;
                    GetEdgeAdjacentVertices(edgePtr, &v0, &v1);
                    MergeVertexIntoVertex(mMesh, v0, v1, &deletedElementTracker);
                    collapsedEdge = true;
                }
            }
        }
        if (collapsedEdge) {
            continue;
        }

        // If any vertices are nearly coincident with the opposing edge
        // of the triangle, correct the mesh to remove the degenerate face.
        bool removedDegenerateFace = false;
        for (AdjacentVertexIterator iterator = facePtr->adjacentVertexBegin();
             iterator != facePtr->adjacentVertexEnd(); ++iterator) {
            VertexPtr vertexPtr = *iterator;

            EdgePtr edgePtr;
            GetTriangularFaceEdgeOpposingVertex(facePtr, vertexPtr, &edgePtr);

            // Along the opposing edge, find the point nearest to the vertex.
            cgmath::Vector3f p0;
            cgmath::Vector3f p1;
            GetEdgeVertexPositions(edgePtr, &p0, &p1);
            cgmath::Vector3f edgePointNearestToVertex;
            cgmath::GetClosestPointOnLineSegment3f(p0, p1, vertexPtr->position(), 
                &edgePointNearestToVertex);

            if ((edgePointNearestToVertex - vertexPtr->position()).length() < epsilon) {
                VertexPtr newVertexPtr;
                SplitEdgeAndAdjacentTriangularFaces(mMesh, edgePtr,
                    edgePointNearestToVertex, &newVertexPtr, &deletedElementTracker);
                MergeVertexIntoVertex(mMesh, vertexPtr, newVertexPtr, 
                    &deletedElementTracker);
                removedDegenerateFace = true;
                break;
            }
        }
        if (removedDegenerateFace) {
            continue;
        }

        // We detected that the face was degenerate earlier
        // by calling FaceIsDegenerate, but then none of the tests
        // above passed. This should never happen.
        throw except::FailedOperationException(SOURCE_LINE) << "Degenerate face "
            << "failed specific case tests.";
    }

    return true;
}

} // namespace mesh
