// Copyright 2009 Retroactive Fiasco.

#include "Splitter.h"

#include <cassert>
#include <iostream>

#include <cgmath/Tolerance.h>
#include <cgmath/BoundingBox3fOperations.h>
#include <cgmath/TriangleOperations.h>
#include <cgmath/Matrix4fOperations.h>
#include <mesh/MeshOperations.h>
#include <mesh/EdgeOperations.h>
#include <mesh/FaceOperations.h>
#include <mesh/StandardAttributes.h>
#include <mesh/AttributePossessor.h>
#include <meshisect/FaceIntersector.h>
#include <meshprim/CylinderCreator.h>

namespace meshsplit {

Splitter::Splitter()
    : mMesh(NULL),
      mAbsoluteTolerance(cgmath::TOLERANCE),
      mRelativeTolerance(cgmath::TOLERANCE),
      mMarkIntersectionsWithCylinders(false),
      mFlaggedEdgeBooleanAttributeKey(),
      mCurrentEdgePtr(),
      mRetriangulator(),
      mDebugEdgeVector(),
      mEndpointIdentifierIndex(0)
{
}

Splitter::~Splitter()
{
}

void
Splitter::setMesh(mesh::Mesh *mesh)
{
    mMesh = mesh;
}

mesh::Mesh *
Splitter::mesh() const
{
    return mMesh;
}

void
Splitter::setAbsoluteTolerance(float absoluteTolerance)
{
    mAbsoluteTolerance = absoluteTolerance;
}

float
Splitter::absoluteTolerance() const
{
    return mAbsoluteTolerance;
}

void
Splitter::setRelativeTolerance(float relativeTolerance)
{
    mRelativeTolerance = relativeTolerance;
}

float
Splitter::relativeTolerance() const
{
    return mRelativeTolerance;
}

void
Splitter::setMarkIntersectionsWithCylinders(bool markIntersectionsWithCylinders)
{
    mMarkIntersectionsWithCylinders = markIntersectionsWithCylinders;
}

bool
Splitter::markIntersectionsWithCylinders() const
{
    return mMarkIntersectionsWithCylinders;
}

void
Splitter::setFlaggedEdgeBooleanAttributeKey(
    const mesh::AttributeKey &flaggedEdgeBooleanAttributeKey)
{
    mFlaggedEdgeBooleanAttributeKey = flaggedEdgeBooleanAttributeKey;
}

const mesh::AttributeKey &
Splitter::flaggedEdgeBooleanAttributeKey() const
{
    return mFlaggedEdgeBooleanAttributeKey;
}

void
Splitter::splitFaces()
{
    assert(mMesh != NULL);

    // Splitter can only operate on meshes of triangles.
    assert(mesh::AllFacesAreTriangles(*mMesh));

    if (mMarkIntersectionsWithCylinders) {
        resetDebugEdgeVector();
    }

    meshisect::FaceIntersector faceIntersector;
    faceIntersector.setMesh(mMesh);
    faceIntersector.initialize();

    mRetriangulator.setMesh(mMesh);
    mRetriangulator.setAbsoluteTolerance(mAbsoluteTolerance);
    mRetriangulator.setRelativeTolerance(mRelativeTolerance);
    mRetriangulator.setNewEdgeBooleanAttributeKey(mFlaggedEdgeBooleanAttributeKey);

    for (mCurrentEdgePtr = mMesh->edgeBegin();
         mCurrentEdgePtr != mMesh->edgeEnd(); ++mCurrentEdgePtr) {

        // Get the bounding box of the edge we're testing for intersections.
        cgmath::BoundingBox3f boundingBox = mesh::GetEdgeBoundingBox(mCurrentEdgePtr);

        // Grow the bounding box to account for the tolerances.
        boundingBox = cgmath::GrowBoundingBox3fByAbsoluteAndRelativeTolerance(
            boundingBox, mAbsoluteTolerance, mRelativeTolerance);

        // Call applyObjectToBoundingBox (see below) on all faces
        // that intersect the bounding box.
        faceIntersector.applyToBoundingBoxIntersection(boundingBox, this);
    }

    // Split all faces on the recorded lines of intesection.
    mRetriangulator.retriangulate();

    if (mMarkIntersectionsWithCylinders) {
        markDebugEdges();
    }
}

bool
Splitter::applyObjectToBoundingBox(meshisect::FaceIntersectorAabbTreeNode
    &faceIntersectorAabbTreeNode, const cgmath::BoundingBox3f &)
{
    mesh::FacePtr facePtr = faceIntersectorAabbTreeNode.facePtr();

    // Find all intersections between mCurrentEdgePtr and facePtr,
    // and record them on mCurrentFacePtr using the meshretri library.

    mesh::VertexPtr v0;
    mesh::VertexPtr v1;
    mesh::GetEdgeAdjacentVertices(mCurrentEdgePtr, &v0, &v1);

    if (facePtr->hasAdjacentEdge(mCurrentEdgePtr)
        || facePtr->hasAdjacentVertex(v0)
        || facePtr->hasAdjacentVertex(v1)) {
        // The two faces are adjacent to each other, so don't test
        // if they intersect.
        // Returning false means that we don't stop traversing the AABB tree.
        // We want to consider all possible intersections.
        return false;
    }

    cgmath::Vector3f p0;
    cgmath::Vector3f p1;
    cgmath::Vector3f p2;
    mesh::GetTriangularFaceVertexPositions(facePtr, &p0, &p1, &p2);

    cgmath::Vector3f q0;
    cgmath::Vector3f q1;
    mesh::GetEdgeVertexPositions(mCurrentEdgePtr, &q0, &q1);

    EdgePtrVector adjacentEdgePtrVector;
    getAdjacentEdgePtrVector(facePtr, &adjacentEdgePtrVector);

    cgmath::Vector3f r0;
    cgmath::Vector3f r1;
    int clippingEdgeIndex0 = -1;
    int clippingEdgeIndex1 = -1;
    if (cgmath::GetIntersectionOfTriangleWithCoplanarLineSegment(
            p0, p1, p2, q0, q1, mAbsoluteTolerance, mRelativeTolerance,
            &r0, &r1, &clippingEdgeIndex0, &clippingEdgeIndex1)) {

        if (mFlaggedEdgeBooleanAttributeKey.isDefined()) {
            mCurrentEdgePtr->setBool(mFlaggedEdgeBooleanAttributeKey, true);
        }

        if (mMarkIntersectionsWithCylinders) {
            addDebugEdge(r0, r1);
        }

        meshretri::FaceLineSegment faceLineSegment;
        faceLineSegment.setWorldPosition(0, r0);
        faceLineSegment.setWorldPosition(1, r1);

        faceLineSegment.setEndpointIdentifier(0,
            meshretri::EndpointIdentifier::fromVertexPtr(v0));
        faceLineSegment.setEndpointIdentifier(1,
            meshretri::EndpointIdentifier::fromVertexPtr(v1));

        if (clippingEdgeIndex0 != -1) {
            assert(size_t(clippingEdgeIndex0) < adjacentEdgePtrVector.size());
            faceLineSegment.setEndpointIdentifier(0,
                meshretri::EndpointIdentifier::fromEdgePtrAndIndex(
                    adjacentEdgePtrVector[clippingEdgeIndex0], mEndpointIdentifierIndex));
            ++mEndpointIdentifierIndex;
        }
        if (clippingEdgeIndex1 != -1) {
            assert(size_t(clippingEdgeIndex1) < adjacentEdgePtrVector.size());
            faceLineSegment.setEndpointIdentifier(1,
                meshretri::EndpointIdentifier::fromEdgePtrAndIndex(
                    adjacentEdgePtrVector[clippingEdgeIndex1], mEndpointIdentifierIndex));
            ++mEndpointIdentifierIndex;
        }

        mRetriangulator.addFaceLineSegmentToFace(faceLineSegment, facePtr);
    }

    // Returning false means that we don't stop traversing the AABB tree.
    // We want to consider all possible intersections.
    return false;
}

void
Splitter::resetDebugEdgeVector()
{
    mDebugEdgeVector.clear();
}

void
Splitter::addDebugEdge(const cgmath::Vector3f &p0, const cgmath::Vector3f &p1)
{
    DebugEdge debugEdge;
    debugEdge.mPoint0 = p0;
    debugEdge.mPoint1 = p1;
    mDebugEdgeVector.push_back(debugEdge);
}

void
Splitter::markDebugEdges()
{
    meshprim::CylinderCreator cylinderCreator;

    cylinderCreator.setMesh(mMesh);

    cylinderCreator.setDivisions(6);

    cgmath::BoundingBox3f bbox = mesh::ComputeBoundingBox(*mMesh);
    float sceneDiameter = (bbox.max() - bbox.min()).length();
    cylinderCreator.setRadius(sceneDiameter*0.001f);

    mesh::MaterialIndexVector materialIndexVector = mesh::GetMaterialIndexVector(*mMesh);
    int materialIndex = 0;
    if (materialIndexVector.size() > 0) {
        materialIndex = materialIndexVector.back() + 1;
    }

    mMesh->setVector4f(mesh::GetMaterialDiffuse4fAttributeKey(*mMesh, materialIndex),
        cgmath::Vector4f(1.0, 0.0, 0.0, 1.0));

    mesh::AttributePossessor faceAttributes;
    faceAttributes.setInt(mesh::GetMaterialIndexAttributeKey(*mMesh), materialIndex);
    
    cylinderCreator.setFaceAttributes(faceAttributes);

    for (DebugEdgeVector::const_iterator iterator = mDebugEdgeVector.begin();
         iterator != mDebugEdgeVector.end(); ++iterator) {
        const DebugEdge &debugEdge = *iterator;

        cylinderCreator.setTransformationMatrix(
            cgmath::GetPointConnectionMatrix(debugEdge.mPoint0, debugEdge.mPoint1));
            
        cylinderCreator.create();
    }
}

void
Splitter::getAdjacentEdgePtrVector(mesh::FacePtr facePtr, EdgePtrVector *edgePtrVector)
{
    // Return an array of three EdgePtrs for a face, corresponding to the order
    // of the vertices of the face.

    (*edgePtrVector).clear();
    for (mesh::AdjacentVertexIterator iterator = facePtr->adjacentVertexBegin();
         iterator != facePtr->adjacentVertexEnd(); ++iterator) {

        mesh::AdjacentVertexIterator nextIterator = iterator;
        ++nextIterator;
        if (nextIterator == facePtr->adjacentVertexEnd()) {
            nextIterator = facePtr->adjacentVertexBegin();
        }

        mesh::VertexPtr v0 = *iterator;
        mesh::VertexPtr v1 = *nextIterator;

        // Find the edge that is adjacent to these two vertices.
        for (mesh::AdjacentEdgeIterator iterator = facePtr->adjacentEdgeBegin();
             iterator != facePtr->adjacentEdgeEnd(); ++iterator) {
            mesh::EdgePtr edgePtr = *iterator;
            if (edgePtr->hasAdjacentVertex(v0)
                && edgePtr->hasAdjacentVertex(v1)) {
                edgePtrVector->push_back(edgePtr);
            }
        }
    }

    assert(edgePtrVector->size() == 3);
}

} // namespace meshsplit
