// Copyright 2009 Retroactive Fiasco.

#include "Retriangulator.h"

#include <cassert>
#include <cmath>
#include <iostream>
#include <algorithm>

#include <boost/lexical_cast.hpp>

#include <con/Streams.h>
#include <cgmath/Vector3fOperations.h>
#include <cgmath/LineOperations.h>
#include <cgmath/Tolerance.h>
#include <cgmath/BoundingBox2f.h>
#include <except/FailedOperationException.h>
#include <mesh/FaceOperations.h>
#include <mesh/IsConsistent.h>

#include "MeshAttributes.h"

namespace meshretri {

Retriangulator::Retriangulator()
    : mMesh(NULL),
      mRetriangulatorFaceIndexKey(),
      mRetriangulatorEdgeIndexKey(),
      mAbsoluteTolerance(cgmath::TOLERANCE),
      mRelativeTolerance(cgmath::TOLERANCE),
      mNewEdgeBooleanAttributeKey(),
      mRetriangulatorFaceVector(),
      mAdditionalRetriangulatorFaceVector(),
      mRetriangulatorEdgeVector(),
      mRetriangulatingBackprojectionFace(false)
{
}

Retriangulator::~Retriangulator()
{
}

void
Retriangulator::setMesh(mesh::Mesh *mesh)
{
    mMesh = mesh;

    mRetriangulatorFaceIndexKey = mMesh->getAttributeKey("RetriangulatorFaceIndex",
        mesh::AttributeKey::INT, mesh::AttributeKey::TEMPORARY);
    mRetriangulatorEdgeIndexKey = mMesh->getAttributeKey("RetriangulatorEdgeIndex",
        mesh::AttributeKey::INT, mesh::AttributeKey::TEMPORARY);
}

mesh::Mesh *
Retriangulator::mesh() const
{
    return mMesh;
}

void
Retriangulator::setAbsoluteTolerance(float absoluteTolerance)
{
    mAbsoluteTolerance = absoluteTolerance;
}

float
Retriangulator::absoluteTolerance() const
{
    return mAbsoluteTolerance;
}

void
Retriangulator::setRelativeTolerance(float relativeTolerance)
{
    mRelativeTolerance = relativeTolerance;
}

float
Retriangulator::relativeTolerance() const
{
    return mRelativeTolerance;
}

void
Retriangulator::setNewEdgeBooleanAttributeKey(const mesh::AttributeKey &newEdgeBooleanAttributeKey)
{
    mNewEdgeBooleanAttributeKey = newEdgeBooleanAttributeKey;
}

const mesh::AttributeKey &
Retriangulator::newEdgeBooleanAttributeKey() const
{
    return mNewEdgeBooleanAttributeKey;
}

void
Retriangulator::addFaceLineSegmentToFace(const FaceLineSegment &faceLineSegment, 
    mesh::FacePtr facePtr)
{
    // Retriangulator::setMesh must already have been called.
    assert(mRetriangulatorFaceIndexKey.isDefined());

    size_t index = 0;
    if (facePtr->hasAttribute(mRetriangulatorFaceIndexKey)) {
        index = facePtr->getInt(mRetriangulatorFaceIndexKey);
    } else {
        index = addRetriangulatorFace(facePtr);
    }

    RetriangulatorFace &retriangulatorFace = mRetriangulatorFaceVector[index];
    retriangulatorFace.addFaceLineSegment(faceLineSegment);
}

void
Retriangulator::retriangulate()
{
    try {

        mRetriangulatingBackprojectionFace = false;

        filterFaceLineSegments();

        // Split all the mesh edges, based on the EdgePoints
        // we stored along each edge.
        con::debug << "Splitting mesh edges." << std::endl;
        splitMeshEdges();

        // Split all mesh faces, based on the FaceLineSegments 
        // stored in each RetriangulatorFace.
        con::debug << "Splitting mesh faces." << std::endl;
        splitMeshFaces();

#ifdef DEBUG
        con::debug << "Verifying mesh consistency." << std::endl;
        mesh::IsConsistent(*mMesh);
#endif

        reset();

    } catch (...) {

        reset();
        throw;
    }
}

void
Retriangulator::retriangulateBackprojectionFace(mesh::FacePtr facePtr, 
    TriangleVector *triangleVector)
{
    try {

        mRetriangulatingBackprojectionFace = true;

        // If no faces have recorded line segments, it's a lot faster to
        // return the single triangle directly rather than invoking the CDT code.
        if (mRetriangulatorFaceVector.empty()) {
            copyFaceToTriangleVector(facePtr, triangleVector);
            goto cleanup;
        }

        // There should be only one face with line segments recorded on it.
        assert(mRetriangulatorFaceVector.size() == 1);
        assert(mRetriangulatorFaceVector[0].facePtr() == facePtr);

        {
            RetriangulatorFace *retriangulatorFace = &mRetriangulatorFaceVector[0];

            filterFaceLineSegments();

            // If no face line segments remain after filtering, return the single face
            // with copyFaceToTriangleVector.
            if (retriangulatorFace->faceLineSegmentCount() == 0) {
                copyFaceToTriangleVector(facePtr, triangleVector);
                goto cleanup;
            }

            // Compute the CDT of the line segments on the face, returning
            // the result as a vector of triangles.
            assert(mRetriangulatorEdgeVector.size() == 3);
            retriangulatorFace->splitBackprojectionFace(&mRetriangulatorEdgeVector[0], 
                triangleVector, this);
        }

    cleanup:
        resetBackprojectionFaceAttributes(facePtr);
        reset();

    } catch (...) {

        reset();
        throw;
    }
}

void
Retriangulator::filterFaceLineSegments()
{
    initialize();

#if 0
    getLargestRetriangulatorFace()->writeToSvgFile(this, "out.svg");
#endif

#define DUMP_FACE  0

#if DUMP_FACE
    dumpFace("0-0.svg");
#endif

    // (1) Merge FaceLineSegment endpoints that are identified as originating 
    // from the same occluder edge endpoint during the wedge tracing. 
    weldFaceLineSegmentEndpoints();

#if DUMP_FACE
    dumpFace("0-1.svg");
#endif

    // (2) Create the edge points based on EdgePtrs assigned to
    // face line segment endpoints via EndpointIdentifiers.
    createEdgePointsFromEndpointIdentifierEdgePtrs();

#if DUMP_FACE
    dumpFace("0-2.svg");
#endif

    // (3) Merge EdgePoints whose origin was the same wedge.
    weldEdgePointsCreatedFromCommonWedge();

#if DUMP_FACE
    dumpFace("0-3.svg");
#endif

    // (3b) Merge EdgePoints that have the same value of 't'.
    // This eliminates funky cases where the edge folds back on itself
    // multiple times at extremely small scales.
    weldParametricallyEqualEdgePoints();

#if DUMP_FACE
    dumpFace("0-3b.svg");
#endif

    int loopCount = 0;

again:
    ++loopCount;

    if (loopCount > 100) {
        // If this happens, we're most likely stuck in an infinite loop
        // because of an unexpected geometric configuration.
        throw except::FailedOperationException(SOURCE_LINE)
            << "Exceeded loop limit in retriangulator while filtering face line segments.";
    }

    // (4) Create EdgePoints from line segments that intersect the perimeters
    // of the faces.
    createEdgePointsFromPerimeterIntersections();

#if DUMP_FACE
    dumpFace(boost::lexical_cast<std::string>(loopCount) + "-4.svg");
#endif

    // (4b) Find faces adjacent to edges with EdgePoints, and keep track of them
    // in a separate vector, so that RetriangulatorFace pointers
    // of EdgePoints won't be invalidated.
    createAdditionalRetriangulatorFaces();

#if DUMP_FACE
    dumpFace(boost::lexical_cast<std::string>(loopCount) + "-4b.svg");
#endif

    // (5) For each face, test if any of the perimeter line segments 
    // intersect each other, and correct this by merging EdgePoints
    // with the nearest mesh vertex as necessary.
    // If this happens, it means that we've moved FaceLineSegment endpoints,
    // and the mesh perimeter has been modified, which means that
    // we have to repeat step 4 and 5 again.
    if (correctSelfIntersectingFacePerimeterLineSegments()) {
#if DUMP_FACE
        dumpFace(boost::lexical_cast<std::string>(loopCount) + "-5.svg");
#endif
        goto again;
    }


    // (5b) Correct perimeter edges that intersect at EdgePoints.
    if (correctSelfIntersectingFacePerimeterEdgePoints()) {
#if DUMP_FACE
    dumpFace(boost::lexical_cast<std::string>(loopCount) + "-5b.svg");
#endif
        goto again;
    }

    // (5c) It's still possible that line segments intersect the face perimeters
    // because of floating point error.
    // This function only tests line segments that are explicitly attached
    // to the perimeter via EdgePoints.
    // Where this happens, collapse neighboring EdgePoints.
    if (correctEdgePointsFromLineSegmentIntersections()) {
#if DUMP_FACE
        dumpFace(boost::lexical_cast<std::string>(loopCount) + "-5c.svg");
#endif
        goto again;
    }

    // (5d) It's still possible that line segments intersect the face perimeter.
    if (correctRemainingLineSegmentPerimeterIntersections()) {
#if DUMP_FACE
        dumpFace(boost::lexical_cast<std::string>(loopCount) + "-5d.svg");
#endif
        goto again;
    }

    // (5e) Collapse all line segments that stretch from one vertex edge to another.
    collapseEdgeCoincidentLineSegments();

#if DUMP_FACE
    dumpFace(boost::lexical_cast<std::string>(loopCount) + "-5e.svg");
#endif

    // (6) (7) (8) When two FaceLineSegments intersect, split them.
    // We can't 'goto again' after calling this because it
    // invalidates the EdgePoint references to FaceLineSegment endpoints.
    splitAllMutuallyIntersectingFaceLineSegments();

#if DUMP_FACE
    dumpFace(boost::lexical_cast<std::string>(loopCount) + "-6.svg");
#endif
}

void
Retriangulator::reset()
{
    mRetriangulatorFaceVector.clear();
    mAdditionalRetriangulatorFaceVector.clear();
    mRetriangulatorEdgeVector.clear();
}

void
Retriangulator::addEdgePointToRetriangulatorEdge(mesh::EdgePtr edgePtr,
    RetriangulatorFace *retriangulatorFace, size_t faceLineSegmentIndex, size_t endpointIndex)
{
    assert(edgePtr->hasAttribute(mRetriangulatorEdgeIndexKey));
    size_t retriangulatorEdgeIndex = edgePtr->getInt(mRetriangulatorEdgeIndexKey);

    assert(retriangulatorEdgeIndex < mRetriangulatorEdgeVector.size());
    RetriangulatorEdge &retriangulatorEdge = mRetriangulatorEdgeVector[retriangulatorEdgeIndex];

    EdgePoint edgePoint;
    edgePoint.setRetriangulatorFace(retriangulatorFace);
    edgePoint.setFaceLineSegmentIndex(faceLineSegmentIndex);
    edgePoint.setEndpointIndex(endpointIndex);

    retriangulatorEdge.addEdgePoint(edgePoint);
}

RetriangulatorEdge *
Retriangulator::getRetriangulatorEdgeFromEdgePtr(mesh::EdgePtr edgePtr)
{
    assert(edgePtr->hasAttribute(mRetriangulatorEdgeIndexKey));
    size_t retriangulatorEdgeIndex = edgePtr->getInt(mRetriangulatorEdgeIndexKey);

    assert(retriangulatorEdgeIndex < mRetriangulatorEdgeVector.size());
    return &mRetriangulatorEdgeVector[retriangulatorEdgeIndex];
}

void
Retriangulator::initialize()
{
    initializeRetriangulatorFaces();
    initializeRetriangulatorEdges();
}

void
Retriangulator::initializeRetriangulatorFaces()
{
    for (RetriangulatorFaceVector::iterator iterator = mRetriangulatorFaceVector.begin();
         iterator != mRetriangulatorFaceVector.end(); ++iterator) {
        RetriangulatorFace &retriangulatorFace = *iterator;

        mesh::FacePtr facePtr = retriangulatorFace.facePtr();

        unsigned axis0 = 0;
        unsigned axis1 = 1;
        unsigned axis2 = 2;
        cgmath::GetPrimaryAxesMostOrthogonalToVector(
            mesh::GetFaceGeometricNormal(facePtr), &axis0, &axis1, &axis2);
        retriangulatorFace.setAxis0(axis0);
        retriangulatorFace.setAxis1(axis1);
        retriangulatorFace.setAxis2(axis2);

        float max = 0.0;
        for (mesh::AdjacentVertexIterator iterator = facePtr->adjacentVertexBegin();
             iterator != facePtr->adjacentVertexEnd(); ++iterator) {
            mesh::VertexPtr vertexPtr = *iterator;
            max = std::max(max, cgmath::Vector2f(vertexPtr->position(),
                    retriangulatorFace.axis0(), retriangulatorFace.axis1()).maxAbs());
        }
        retriangulatorFace.setEpsilon(
            std::max(mAbsoluteTolerance, max*mRelativeTolerance));
    }
}

void
Retriangulator::initializeRetriangulatorEdges()
{
    if (mRetriangulatingBackprojectionFace) {

        // For each face, loop over all of its edges, and make sure they're
        // all represented in the RetriangulatorEdgeVector.
        for (RetriangulatorFaceVector::iterator iterator = mRetriangulatorFaceVector.begin();
             iterator != mRetriangulatorFaceVector.end(); ++iterator) {
            RetriangulatorFace &retriangulatorFace = *iterator;
            mesh::FacePtr facePtr = retriangulatorFace.facePtr();

            for (mesh::AdjacentEdgeIterator iterator = facePtr->adjacentEdgeBegin();
                 iterator != facePtr->adjacentEdgeEnd(); ++iterator) {
                mesh::EdgePtr edgePtr = *iterator;
                initializeRetriangulatorEdgeFromEdgePtr(edgePtr);
            }        
        }

    } else {

        for (mesh::EdgePtr edgePtr = mMesh->edgeBegin(); 
             edgePtr != mMesh->edgeEnd(); ++edgePtr) {
            initializeRetriangulatorEdgeFromEdgePtr(edgePtr);
        }
    }
}

void
Retriangulator::initializeRetriangulatorEdgeFromEdgePtr(mesh::EdgePtr edgePtr)
{
    // If we haven't visited this edge yet,
    // add it to the RetriangulatorEdgeVector.

    assert(mRetriangulatorEdgeIndexKey.isDefined());
    if (!edgePtr->hasAttribute(mRetriangulatorEdgeIndexKey)) {

        size_t index = mRetriangulatorEdgeVector.size();
        edgePtr->setInt(mRetriangulatorEdgeIndexKey, index);
        mRetriangulatorEdgeVector.push_back(RetriangulatorEdge());

        RetriangulatorEdge &retriangulatorEdge = mRetriangulatorEdgeVector[index];
        retriangulatorEdge.setEdgePtr(edgePtr);

        float max = 0.0;
        for (mesh::AdjacentVertexIterator iterator = edgePtr->adjacentVertexBegin();
             iterator != edgePtr->adjacentVertexEnd(); ++iterator) {
            mesh::VertexPtr vertexPtr = *iterator;
            max = std::max(max, vertexPtr->position().maxAbs());
        }
        retriangulatorEdge.setEpsilon(
            std::max(mAbsoluteTolerance, max*mRelativeTolerance));
    }
}

RetriangulatorFace *
Retriangulator::getRetriangulatorFaceFromMeshFace(mesh::FacePtr facePtr)
{
    assert(mRetriangulatorFaceIndexKey.isDefined());
    assert(facePtr->hasAttribute(mRetriangulatorFaceIndexKey));
    size_t index = facePtr->getInt(mRetriangulatorFaceIndexKey);

    assert(index < mRetriangulatorFaceVector.size());
    return &mRetriangulatorFaceVector[index];
}

bool
Retriangulator::meshFaceHasRetriangulatorFace(mesh::FacePtr facePtr) const
{
    assert(mRetriangulatorFaceIndexKey.isDefined());

    return facePtr->hasAttribute(mRetriangulatorFaceIndexKey);
}

RetriangulatorEdge *
Retriangulator::getRetriangulatorEdgeFromMeshEdge(mesh::EdgePtr edgePtr)
{
    assert(mRetriangulatorEdgeIndexKey.isDefined());
    assert(edgePtr->hasAttribute(mRetriangulatorEdgeIndexKey));
    size_t index = edgePtr->getInt(mRetriangulatorEdgeIndexKey);

    assert(index < mRetriangulatorEdgeVector.size());
    return &mRetriangulatorEdgeVector[index];
}

bool
Retriangulator::meshEdgeHasRetriangulatorEdge(mesh::EdgePtr edgePtr) const
{
    assert(mRetriangulatorEdgeIndexKey.isDefined());

    return edgePtr->hasAttribute(mRetriangulatorEdgeIndexKey);
}

void
Retriangulator::weldFaceLineSegmentEndpoints()
{
    for (RetriangulatorFaceVector::iterator iterator = mRetriangulatorFaceVector.begin();
         iterator != mRetriangulatorFaceVector.end(); ++iterator) {
        RetriangulatorFace &retriangulatorFace = *iterator;

        retriangulatorFace.weldFaceLineSegmentEndpoints();
    }
}

void
Retriangulator::createEdgePointsFromEndpointIdentifierEdgePtrs()
{
    for (RetriangulatorFaceVector::iterator iterator = mRetriangulatorFaceVector.begin();
         iterator != mRetriangulatorFaceVector.end(); ++iterator) {
        RetriangulatorFace &retriangulatorFace = *iterator;

        retriangulatorFace.createEdgePointsFromEndpointIdentifierEdgePtrs(this);
    }
}

void
Retriangulator::weldEdgePointsCreatedFromCommonWedge()
{
    for (RetriangulatorEdgeVector::iterator iterator = mRetriangulatorEdgeVector.begin();
         iterator != mRetriangulatorEdgeVector.end(); ++iterator) {
        RetriangulatorEdge &retriangulatorEdge = *iterator;
        retriangulatorEdge.weldEdgePointsCreatedFromCommonWedge();
    }
}

void
Retriangulator::weldParametricallyEqualEdgePoints()
{
    // The code below relies on correct EdgePoint positions, and that
    // the EdgePoints are in sorted order along each edge.
    updateEdgePoints();

    for (RetriangulatorEdgeVector::iterator iterator = mRetriangulatorEdgeVector.begin();
         iterator != mRetriangulatorEdgeVector.end(); ++iterator) {
        RetriangulatorEdge &retriangulatorEdge = *iterator;
        retriangulatorEdge.weldParametricallyEqualEdgePoints();
    }
}

void
Retriangulator::updateEdgePoints()
{
    for (RetriangulatorEdgeVector::iterator iterator = mRetriangulatorEdgeVector.begin();
         iterator != mRetriangulatorEdgeVector.end(); ++iterator) {
        RetriangulatorEdge &retriangulatorEdge = *iterator;
        retriangulatorEdge.updateEdgePoints();
    }
}

void
Retriangulator::createEdgePointsFromPerimeterIntersections()
{
    // The code below relies on correct EdgePoint positions, and that
    // the EdgePoints are in sorted order along each edge.
    updateEdgePoints();

    // We keep looping until no more line segments intersecting
    // face perimeters are detected, in case adding new Endpoints
    // to a perimeter edge perturbs it just enough that it
    // intersects additional line segments.
    bool createdEdgePoints = false;
    do {
        createdEdgePoints = false;
        for (RetriangulatorFaceVector::iterator iterator = mRetriangulatorFaceVector.begin();
             iterator != mRetriangulatorFaceVector.end(); ++iterator) {
            RetriangulatorFace &retriangulatorFace = *iterator;

            if (retriangulatorFace.createEdgePointsFromPerimeterIntersections(this)) {
                createdEdgePoints = true;
            }
        }
    } while (createdEdgePoints);
}

void
Retriangulator::createAdditionalRetriangulatorFaces()
{
    if (mRetriangulatingBackprojectionFace) {
        return;
    }

    mAdditionalRetriangulatorFaceVector.clear();

    for (RetriangulatorEdgeVector::iterator iterator = mRetriangulatorEdgeVector.begin();
         iterator != mRetriangulatorEdgeVector.end(); ++iterator) {
        RetriangulatorEdge &retriangulatorEdge = *iterator;

        if (retriangulatorEdge.edgePointCount() > 0) {
            mesh::EdgePtr edgePtr = retriangulatorEdge.edgePtr();
            for (mesh::AdjacentFaceIterator iterator = edgePtr->adjacentFaceBegin();
                 iterator != edgePtr->adjacentFaceEnd(); ++iterator) {
                mesh::FacePtr facePtr = *iterator;
                if (!facePtr->hasAttribute(mRetriangulatorFaceIndexKey)) {
                    // Put the face in a separate vector, so the RetriangulatorFace pointers
                    // of EdgePoints won't be invalidated.
                    addAdditionalRetriangulatorFace(facePtr);

                    // This prevents us from adding the same face twice.
                    facePtr->setInt(mRetriangulatorFaceIndexKey, -1);
                }
            }
        }
    }

    // Reset the attributes we set in the loop above,
    // so if we have to make two passes through this function,
    // everything will work OK.
    for (mesh::FacePtr facePtr = mMesh->faceBegin();
         facePtr != mMesh->faceEnd(); ++facePtr) {
        if (facePtr->getInt(mRetriangulatorFaceIndexKey) == -1) {
            facePtr->eraseAttribute(mRetriangulatorFaceIndexKey);
        }
    }
}

bool
Retriangulator::correctSelfIntersectingFacePerimeterLineSegments()
{
    bool faceLineSegmentEndpointsWereMoved = false;

    for (RetriangulatorFaceVector::iterator iterator = mRetriangulatorFaceVector.begin();
         iterator != mRetriangulatorFaceVector.end(); ++iterator) {
        RetriangulatorFace &retriangulatorFace = *iterator;

        if (retriangulatorFace.correctSelfIntersectingFacePerimeterLineSegments(this)) {
            faceLineSegmentEndpointsWereMoved = true;
        }
    }

    for (RetriangulatorFaceVector::iterator iterator = mAdditionalRetriangulatorFaceVector.begin();
         iterator != mAdditionalRetriangulatorFaceVector.end(); ++iterator) {
        RetriangulatorFace &retriangulatorFace = *iterator;

        if (retriangulatorFace.correctSelfIntersectingFacePerimeterLineSegments(this)) {
            faceLineSegmentEndpointsWereMoved = true;
        }
    }

    return faceLineSegmentEndpointsWereMoved;
}

bool
Retriangulator::correctSelfIntersectingFacePerimeterEdgePoints()
{
    bool faceLineSegmentEndpointsWereMoved = false;

    for (RetriangulatorFaceVector::iterator iterator = mRetriangulatorFaceVector.begin();
         iterator != mRetriangulatorFaceVector.end(); ++iterator) {
        RetriangulatorFace &retriangulatorFace = *iterator;

        if (retriangulatorFace.correctSelfIntersectingFacePerimeterEdgePoints(this)) {
            faceLineSegmentEndpointsWereMoved = true;
        }
    }

    for (RetriangulatorFaceVector::iterator iterator = mAdditionalRetriangulatorFaceVector.begin();
         iterator != mAdditionalRetriangulatorFaceVector.end(); ++iterator) {
        RetriangulatorFace &retriangulatorFace = *iterator;

        if (retriangulatorFace.correctSelfIntersectingFacePerimeterEdgePoints(this)) {
            faceLineSegmentEndpointsWereMoved = true;
        }
    }

    return faceLineSegmentEndpointsWereMoved;
}

bool
Retriangulator::correctEdgePointsFromLineSegmentIntersections()
{
    bool faceLineSegmentEndpointsWereMoved = false;

    for (RetriangulatorFaceVector::iterator iterator = mRetriangulatorFaceVector.begin();
         iterator != mRetriangulatorFaceVector.end(); ++iterator) {
        RetriangulatorFace &retriangulatorFace = *iterator;

        if (retriangulatorFace.correctEdgePointsFromLineSegmentIntersections(this)) {
            faceLineSegmentEndpointsWereMoved = true;
        }
    }    

    return faceLineSegmentEndpointsWereMoved;
}

bool
Retriangulator::correctRemainingLineSegmentPerimeterIntersections()
{
    bool faceLineSegmentEndpointsWereMoved = false;

    for (RetriangulatorFaceVector::iterator iterator = mRetriangulatorFaceVector.begin();
         iterator != mRetriangulatorFaceVector.end(); ++iterator) {
        RetriangulatorFace &retriangulatorFace = *iterator;

        if (retriangulatorFace.correctRemainingLineSegmentPerimeterIntersections(this)) {
            faceLineSegmentEndpointsWereMoved = true;
        }
    }    

    return faceLineSegmentEndpointsWereMoved;
}

void
Retriangulator::collapseEdgeCoincidentLineSegments()
{
    for (RetriangulatorFaceVector::iterator iterator = mRetriangulatorFaceVector.begin();
         iterator != mRetriangulatorFaceVector.end(); ++iterator) {
        RetriangulatorFace &retriangulatorFace = *iterator;
        retriangulatorFace.collapseEdgeCoincidentLineSegments();
    }    
}

void
Retriangulator::splitAllMutuallyIntersectingFaceLineSegments()
{
    // Keep repeating this until there are no more line segments
    // left to intersect.
    // It's possible that splitting pairs of line segments against each other,
    // because of floating point rounding at the intersection point,
    // perturbs the resulting line segments just enough that they
    // intersect with additional line segments, requiring more splitting.

    bool faceLineSegmentsWereSplit = false;
    do {
        faceLineSegmentsWereSplit = false;
        for (RetriangulatorFaceVector::iterator iterator = mRetriangulatorFaceVector.begin();
             iterator != mRetriangulatorFaceVector.end(); ++iterator) {
            RetriangulatorFace &retriangulatorFace = *iterator;

            if (retriangulatorFace.splitAllMutuallyIntersectingFaceLineSegments()) {
                faceLineSegmentsWereSplit = true;
            }
        }
    } while (faceLineSegmentsWereSplit);
}

void
Retriangulator::splitMeshEdges()
{
    mesh::AttributeKey isDegreeZeroDiscontinuityAttributeKey
        = GetIsDegreeZeroDiscontinuityAttributeKey(*mMesh);

    for (RetriangulatorEdgeVector::iterator iterator = mRetriangulatorEdgeVector.begin();
         iterator != mRetriangulatorEdgeVector.end(); ++iterator) {
        RetriangulatorEdge &retriangulatorEdge = *iterator;

        mesh::EdgePtr edgePtr = retriangulatorEdge.edgePtr();
        edgePtr->eraseAttribute(mRetriangulatorEdgeIndexKey);
        retriangulatorEdge.splitMeshEdge(mMesh,
            isDegreeZeroDiscontinuityAttributeKey);
    }
}

void
Retriangulator::splitMeshFaces()
{
    for (RetriangulatorFaceVector::iterator iterator 
             = mRetriangulatorFaceVector.begin();
         iterator != mRetriangulatorFaceVector.end(); ++iterator) {
        RetriangulatorFace &retriangulatorFace = *iterator;

        retriangulatorFace.facePtr()->eraseAttribute(mRetriangulatorFaceIndexKey);
        retriangulatorFace.splitMeshFace(mMesh, this);
    }    

    // Split the additional faces that had no FaceLineSegments,
    // but were adjacent to edges that had to be split.
    for (RetriangulatorFaceVector::iterator iterator 
             = mAdditionalRetriangulatorFaceVector.begin();
         iterator != mAdditionalRetriangulatorFaceVector.end(); ++iterator) {
        RetriangulatorFace &retriangulatorFace = *iterator;

        retriangulatorFace.splitMeshFace(mMesh, this);
    }
}

size_t
Retriangulator::addRetriangulatorFace(mesh::FacePtr facePtr)
{
    size_t index = mRetriangulatorFaceVector.size();
    facePtr->setInt(mRetriangulatorFaceIndexKey, index);

    RetriangulatorFace retriangulatorFace;
    retriangulatorFace.setFacePtr(facePtr);

    mRetriangulatorFaceVector.push_back(retriangulatorFace);

    return index;
}

size_t
Retriangulator::addAdditionalRetriangulatorFace(mesh::FacePtr facePtr)
{
    size_t index = mAdditionalRetriangulatorFaceVector.size();

    RetriangulatorFace retriangulatorFace;
    retriangulatorFace.setFacePtr(facePtr);

    unsigned axis0 = 0;
    unsigned axis1 = 1;
    unsigned axis2 = 2;
    cgmath::GetPrimaryAxesMostOrthogonalToVector(
        mesh::GetFaceGeometricNormal(facePtr), &axis0, &axis1, &axis2);
        retriangulatorFace.setAxis0(axis0);
        retriangulatorFace.setAxis1(axis1);
        retriangulatorFace.setAxis2(axis2);

    mAdditionalRetriangulatorFaceVector.push_back(retriangulatorFace);

    return index;
}

void
Retriangulator::copyFaceToTriangleVector(mesh::FacePtr facePtr, TriangleVector *triangleVector)
{
    (*triangleVector).clear();
    Triangle triangle;
    size_t index = 0;
    assert(facePtr->adjacentVertexCount() == 3);
    for (mesh::AdjacentVertexIterator iterator = facePtr->adjacentVertexBegin();
         iterator != facePtr->adjacentVertexEnd(); ++iterator) {
        mesh::VertexPtr vertexPtr = *iterator;
        triangle.mPointArray[index] = vertexPtr->position();
        ++index;
    }
    (*triangleVector).push_back(triangle);
}

void
Retriangulator::resetBackprojectionFaceAttributes(mesh::FacePtr facePtr)
{
    facePtr->eraseAttribute(mRetriangulatorFaceIndexKey);
    for (mesh::AdjacentEdgeIterator iterator = facePtr->adjacentEdgeBegin();
         iterator != facePtr->adjacentEdgeEnd(); ++iterator) {
        mesh::EdgePtr edgePtr = *iterator;
        edgePtr->eraseAttribute(mRetriangulatorEdgeIndexKey);
    }
}

RetriangulatorFace *
Retriangulator::getLargestRetriangulatorFace()
{
    RetriangulatorFace *largestRetriangulatorFace = NULL;
    float largestArea = 0.0;

    for (RetriangulatorFaceVector::iterator iterator = mRetriangulatorFaceVector.begin();
         iterator != mRetriangulatorFaceVector.end(); ++iterator) {
        RetriangulatorFace &retriangulatorFace = *iterator;
        mesh::FacePtr facePtr = retriangulatorFace.facePtr();

        float area = mesh::GetFaceArea(facePtr);
        if (area > largestArea) {
            largestArea = area;
            largestRetriangulatorFace = &retriangulatorFace;
        }
    }

    return largestRetriangulatorFace;
}

bool
Retriangulator::isDebugFace(mesh::FacePtr facePtr)
{
    bool found0 = false;
    bool found1 = false;
    bool found2 = false;

#define POINT0  15.5, 0.0, 4.5
#define POINT1  24.5, 0.0, -4.5
#define POINT2  15.5, 0.0, -4.5

    for (mesh::AdjacentVertexIterator iterator = facePtr->adjacentVertexBegin();
         iterator != facePtr->adjacentVertexEnd(); ++iterator) {
        mesh::VertexPtr vertexPtr = *iterator;
        if (vertexPtr->position().equivalent(
                cgmath::Vector3f(POINT0), 0.001)) {
            found0 = true;
        }
        if (vertexPtr->position().equivalent(
                cgmath::Vector3f(POINT1), 0.001)) {
            found1 = true;
        }
        if (vertexPtr->position().equivalent(
                cgmath::Vector3f(POINT2), 0.001)) {
            found2 = true;
        }
    }

    return found0 && found1 && found2;
}

bool
Retriangulator::dumpFace(const std::string &filename)
{
    for (mesh::FacePtr facePtr = mMesh->faceBegin();
         facePtr != mMesh->faceEnd(); ++facePtr) {
        if (isDebugFace(facePtr)) {
            if (meshFaceHasRetriangulatorFace(facePtr)) {
                RetriangulatorFace *retriangulatorFace 
                    = getRetriangulatorFaceFromMeshFace(facePtr);
                retriangulatorFace->writeToSvgFile(this, filename);
            } else {
                con::debug << "Not writing debug face to SVG file because it has "
                    << "no assigned FaceLineSegments." << std::endl;
            }
            return true;
        }
    }

    return false;
}

} // namespace meshretri
