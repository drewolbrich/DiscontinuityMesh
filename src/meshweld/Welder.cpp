// Copyright 2009 Drew Olbrich

#include "Welder.h"

#include <cassert>
#include <cstdlib>

#include "EdgeMatcher.h"

#include <cgmath/Tolerance.h>
#include <mesh/StandardAttributes.h>
#include <mesh/VertexOperations.h>
#include <mesh/EdgeOperations.h>
#include <mesh/FaceOperations.h>
#include <mesh/MeshOperations.h>
#include <mesh/IsConsistent.h>
#include <mesh/DeletedElementTracker.h>

namespace meshweld {

Welder::Welder()
    : mMesh(NULL),
      mAbsoluteTolerance(cgmath::TOLERANCE),
      mRelativeTolerance(cgmath::TOLERANCE),
      mCandidateEdgeAttributeKey(),
      mMergeAny(false),
      mMarkFacesWithDebugColors(false),
      mRegionIdAttributeKey(),
      mColor3fAttributeKey(),
      mEdgeAabbTree(),
      mMergedEdges(0)
{
}

Welder::~Welder()
{
}

void
Welder::setMesh(mesh::Mesh *mesh)
{
    mMesh = mesh;
}

mesh::Mesh *
Welder::mesh() const
{
    return mMesh;
}

void
Welder::setAbsoluteTolerance(float absoluteTolerance)
{
    mAbsoluteTolerance = absoluteTolerance;
}

float
Welder::absoluteTolerance() const
{
    return mAbsoluteTolerance;
}

void
Welder::setRelativeTolerance(float relativeTolerance)
{
    mRelativeTolerance = relativeTolerance;
}

float
Welder::relativeTolerance() const
{
    return mRelativeTolerance;
}

void
Welder::setCandidateEdgeAttributeKey(const mesh::AttributeKey &candidateEdgeAttributeKey)
{
    mCandidateEdgeAttributeKey = candidateEdgeAttributeKey;
}

const mesh::AttributeKey &
Welder::candidateEdgeAttributeKey() const
{
    return mCandidateEdgeAttributeKey;
}

void
Welder::setMergeAny(bool mergeAny)
{
    mMergeAny = mergeAny;
}

void
Welder::setMarkFacesWithDebugColors(bool markFacesWithDebugColors)
{
    mMarkFacesWithDebugColors = markFacesWithDebugColors;
}

bool
Welder::markFacesWithDebugColors() const
{
    return mMarkFacesWithDebugColors;
}

void
Welder::weld()
{
    mMergedEdges = 0;

    mRegionIdAttributeKey = mMesh->getAttributeKey("__PerimeterWelder_RegionId",
        mesh::AttributeKey::INT, mesh::AttributeKey::TEMPORARY);

    mColor3fAttributeKey = mesh::GetColor3fAttributeKey(*mMesh);

    int regionId = 0;
    for (mesh::EdgePtr edgePtr = mMesh->edgeBegin();
         edgePtr != mMesh->edgeEnd(); ++edgePtr) {

        if (!edgePtr->hasAttribute(mRegionIdAttributeKey)) {
            ++regionId;
            floodFillRegion(edgePtr, regionId);
        }
    }

    buildEdgeAabbTree();

    weldEdges(mAbsoluteTolerance, mRelativeTolerance);

    mesh::RemoveAttributeFromMeshAndAllElements(mMesh, mRegionIdAttributeKey);

    assert(mesh::IsConsistent(*mMesh));
}

unsigned
Welder::mergedEdges() const
{
    return mMergedEdges;
}

void
Welder::floodFillRegion(mesh::EdgePtr edgePtr, int regionId)
{
    // Mark the edge.
    edgePtr->setInt(mRegionIdAttributeKey, regionId);

    // Loop over all vertices adjacent to the edge.
    for (mesh::AdjacentVertexIterator adjacentVertexIterator = edgePtr->adjacentVertexBegin();
         adjacentVertexIterator != edgePtr->adjacentVertexEnd(); ++adjacentVertexIterator) {
        mesh::VertexPtr vertexPtr = *adjacentVertexIterator;

        // If the vertex hasn't been marked, loop over all edges adjacent to the vertex.
        if (!vertexPtr->hasAttribute(mRegionIdAttributeKey)) {
            vertexPtr->setInt(mRegionIdAttributeKey, regionId);

            for (mesh::AdjacentEdgeIterator adjacentEdgeIterator = vertexPtr->adjacentEdgeBegin();
                 adjacentEdgeIterator != vertexPtr->adjacentEdgeEnd(); ++adjacentEdgeIterator) {
                mesh::EdgePtr edgePtr = *adjacentEdgeIterator;

                // If the edge hasn't been marked, flood fill again 
                // starting with that edge.
                if (!edgePtr->hasAttribute(mRegionIdAttributeKey)) {
                    floodFillRegion(edgePtr, regionId);
                }
            }
        }
    }

    if (mMarkFacesWithDebugColors) {
        cgmath::Vector3f color = convertRegionIdToColor(regionId);
        for (mesh::AdjacentFaceIterator adjacentFaceIterator = edgePtr->adjacentFaceBegin();
             adjacentFaceIterator != edgePtr->adjacentFaceEnd(); ++adjacentFaceIterator) {
            mesh::FacePtr facePtr = *adjacentFaceIterator;

            if (!facePtr->hasAttribute(mRegionIdAttributeKey)) {
                facePtr->setInt(mRegionIdAttributeKey, regionId);

                facePtr->setVector3f(mColor3fAttributeKey, color);
            }
        }
    }
}

cgmath::Vector3f
Welder::convertRegionIdToColor(int regionId) const
{
    // Convert an integer region ID to a pseudorandom color by mixing it up with prime numbers.

    unsigned red = (((unsigned(regionId) + 7369)*7603) % 128) + 128;
    unsigned green = (((unsigned(regionId) + 5009)*4583) % 128) + 128;
    unsigned blue = (((unsigned(regionId) + 7867)*3671) % 128) + 128;

    return cgmath::Vector3f(red, green, blue)/255.0;
}

void
Welder::buildEdgeAabbTree()
{
    EdgeAabbTree::ObjectVector objectVector;

    for (mesh::EdgePtr edgePtr = mMesh->edgeBegin();
         edgePtr != mMesh->edgeEnd(); ++edgePtr) {

        if (!edgeCanBeMerged(edgePtr)) {
            continue;
        }

        Edge edge;
        edge.setEdgePtr(edgePtr);

        objectVector.push_back(edge);
    }

    mEdgeAabbTree.initialize(objectVector);
}

void
Welder::weldEdges(float absoluteTolerance, float relativeTolerance)
{
    // This object is used to keep track of edges that are deleted
    // as a result of edge merging, so that we don't process them multiple times.
    mesh::DeletedElementTracker deletedElementTracker;
    deletedElementTracker.setElementMask(mesh::DeletedElementTracker::EDGES);

    EdgeMatcher edgeMatcher;
    edgeMatcher.setAbsoluteTolerance(absoluteTolerance);
    edgeMatcher.setRelativeTolerance(relativeTolerance);
    edgeMatcher.setDeletedElementTracker(&deletedElementTracker);

    // Build a vector of all the EdgePtrs first. This lets us skip
    // over edges that are deleted as we go along, without
    // having to worry about tripping over invalidated iterators
    // in the linked list of edges.
    std::vector<mesh::EdgePtr> edgePtrVector;
    edgePtrVector.reserve(mMesh->edgeCount());
    for (mesh::EdgePtr edgePtr = mMesh->edgeBegin();
         edgePtr != mMesh->edgeEnd(); ++edgePtr) {
        edgePtrVector.push_back(edgePtr);
    }

    for (size_t index = 0; index < edgePtrVector.size(); ++index) {
        mesh::EdgePtr edgePtr = edgePtrVector[index];

        // Skip edges that were already deleted.
        if (deletedElementTracker.hasEdge(edgePtr)) {
            continue;
        }

        if (!edgeCanBeMerged(edgePtr)) {
            continue;
        }

        edgeMatcher.setEdgePtr(edgePtr);
        edgeMatcher.testAgainstEdgeAabbTree(mEdgeAabbTree);

        const EdgeMatcher::EdgePtrVector &matchingEdgePtrVector
            = edgeMatcher.matchingEdgePtrVector();

        if (matchingEdgePtrVector.empty()) {
            continue;
        }

        mesh::VertexPtr v0;
        mesh::VertexPtr v1;
        mesh::GetEdgeAdjacentVertices(edgePtr, &v0, &v1);

        for (EdgeMatcher::EdgePtrVector::const_iterator iterator
                 = matchingEdgePtrVector.begin();
             iterator != matchingEdgePtrVector.end(); ++iterator) {
            mesh::EdgePtr matchingEdgePtr = *iterator;

            assert(matchingEdgePtr != edgePtr);

            // MergeEdgeIntoEdge, below, may result in the deletion of some of
            // the matching edges in the vector, so we must test for this
            // possibility and skip them.
            if (deletedElementTracker.hasEdge(matchingEdgePtr)) {
                continue;
            }

            mesh::VertexPtr w0;
            mesh::VertexPtr w1;
            mesh::GetEdgeAdjacentVertices(matchingEdgePtr, &w0, &w1);

            if (v0 == w0
                || v1 == w1) {
                // The edges already have a vertex in common,
                // and the vertex orders of the edges are suitable input
                // for MergeEdgeIntoEdge, so do nothing.
            } else if (v0 == w1
                || v1 == w0) {
                // The edges already have a vertex in common,
                // but the vertex orders of the edges are not suitable input
                // for MergeEdgeIntoEdge, so swap matchingEdgePtr's vertices.
                swapEdgeVertices(matchingEdgePtr);
            } else {
                if ((v0->position() - w1->position()).length() 
                    < (v0->position() - w0->position()).length()) {
                    // matchingEdgePtr's second vertex is closer to 
                    // edgePtr's first vertex than matchingEdgePtr's first vertex.
                    // We'll swap the edges so the vertices that are nearest
                    // to each other will be merged together by MergeEdgeIntoEdge.
                    swapEdgeVertices(matchingEdgePtr);
                }
            }

            mesh::MergeEdgeIntoEdge(mMesh, matchingEdgePtr, edgePtr, &deletedElementTracker);
        }
    }
}

void
Welder::swapEdgeVertices(mesh::EdgePtr edgePtr)
{
    mesh::VertexPtr v0;
    mesh::VertexPtr v1;
    mesh::GetEdgeAdjacentVertices(edgePtr, &v0, &v1);

    edgePtr->removeAdjacentVertex(v0);
    edgePtr->removeAdjacentVertex(v1);
    edgePtr->addAdjacentVertex(v1);
    edgePtr->addAdjacentVertex(v0);
}

bool
Welder::edgeCanBeMerged(mesh::EdgePtr edgePtr)
{
    if (mMergeAny) {
        return true;
    }

    if (mCandidateEdgeAttributeKey.isDefined()) {
        return edgePtr->getBool(mCandidateEdgeAttributeKey);
    }

    // The edge is allowed to be merged if it is on the perimeter of the mesh.
    // That is, it has less than two adjacent faces.
    return edgePtr->adjacentFaceCount() < 2;
}

} // namespace meshweld
