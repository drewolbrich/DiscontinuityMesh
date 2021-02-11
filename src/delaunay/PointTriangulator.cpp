// Copyright 2008 Drew Olbrich

#include "PointTriangulator.h"

#include <cmath>
#include <cassert>
#include <cstdlib>
#include <iostream>
#include <vector>
#include <algorithm>
#include <limits>

#include <cgmath/CircleOperations.h>
#include <exact/GeometricPredicates.h>

#include "Mesh.h"
#include "MeshHistory.h"
#include "MeshConsistency.h"
#include "MeshDelaunay.h"
#include "MeshConvexPerimeter.h"
#include "Vertex.h"
#include "Edge.h"
#include "Face.h"
#include "VertexOperations.h"
#include "EdgeOperations.h"
#include "FaceOperations.h"
#include "ValidationDetails.h"

namespace delaunay {

PointTriangulator::PointTriangulator()
    : mInputPointVector(NULL),
      mInputEdgeVector(NULL),
      mSvgFilename(""),
      mWriteEntireSvgHistory(false),
      mShouldShuffleInputPoints(true),
      mShuffledPointIndexVector(),
      mOutputEdgeVector(),
      mOutputTriangleVector(),
      mMeshPtr(),
      mMeshHistoryPtr(),
      mMeshSnapshotPtr(),
      mNearestFaceVertexPtrVector(),
      mInputPointVectorVertexPtrVector(),
      mFindNearestFaceRandomContext(0)
{
    for (int index = 0; index < 3; ++index) {
        mUnshuffledTriangleIndexArray[index] = std::numeric_limits<size_t>::max();
    }
}

PointTriangulator::~PointTriangulator()
{
}

void
PointTriangulator::setInputPointVector(const PointVector *inputPointVector)
{
    mInputPointVector = inputPointVector;
}

const PointTriangulator::PointVector *
PointTriangulator::inputPointVector() const
{
    return mInputPointVector;
}

void
PointTriangulator::setInputEdgeVector(const EdgeVector *inputEdgeVector)
{
    mInputEdgeVector = inputEdgeVector;
}

const PointTriangulator::EdgeVector *
PointTriangulator::inputEdgeVector() const
{
    return mInputEdgeVector;
}

void
PointTriangulator::setSvgFilename(const std::string &svgFilename)
{
    mSvgFilename = svgFilename;
}

const std::string &
PointTriangulator::svgFilename() const
{
    return mSvgFilename;
}

void
PointTriangulator::setWriteEntireSvgHistory(bool writeEntireSvgHistory)
{
    mWriteEntireSvgHistory = writeEntireSvgHistory;
}

bool
PointTriangulator::writeEntireSvgHistory() const
{
    return mWriteEntireSvgHistory;
}

void
PointTriangulator::setShouldShuffleInputPoints(bool shouldShuffleInputPoints)
{
    mShouldShuffleInputPoints = shouldShuffleInputPoints;
}

bool
PointTriangulator::shouldShuffleInputPoints() const
{
    return mShouldShuffleInputPoints;
}

bool
PointTriangulator::validate(ValidationDetails *validationDetails) const
{
    ValidationDetails details;

    assert(mInputPointVector != NULL);
    PointVector pointVector = *mInputPointVector;

    std::sort(pointVector.begin(), pointVector.end());

    // Check if there are less than three input points.
    if (pointVector.size() < 3) {
        details.setHasLessThanThreePoints(true);
    }

    // Check if there are any coincident input points.
    for (size_t index = 0; index < pointVector.size() - 1; ++index) {
        if (pointVector[index] == pointVector[index + 1]) {
            details.setSomePointsAreCoincident(true);
        }
    }

    // Check if all of the input points are colinear.
    for (size_t index2 = 1; index2 < pointVector.size(); ++index2) {
        for (size_t index3 = index2 + 1; index3 < pointVector.size(); ++index3) {
            if (exact::TestOrientation2d(pointVector[0], 
                    pointVector[index2], pointVector[index3]) != 0.0) {
                goto foundNoncolinearPoints;
            }
        }
    }
    details.setAllPointsAreColinear(true);
foundNoncolinearPoints:
    ;

    // Check if any of the input points have NaN values.
    for (size_t index = 0; index < pointVector.size(); ++index) {
        cgmath::Vector2f point = pointVector[index];
        if (point[0]*0.0 != 0.0
            || point[1]*0.0 != 0.0) {
            details.setSomePointsAreNaN(true);
            break;
        }
    }

    // Check if any of the input edges are degenerate.
    if (mInputEdgeVector != NULL) {
        for (size_t index = 0; index < (*mInputEdgeVector).size(); ++index) {
            if ((*mInputEdgeVector)[index].mPointIndexArray[0]
                == (*mInputEdgeVector)[index].mPointIndexArray[1]) {
                details.setSomeEdgesAreDegenerate(true);
                break;
            }
        }
    }

    // None of the input vertices may intersect the input edges.
    if (mInputEdgeVector != NULL) {
        for (size_t vertexIndex = 0; vertexIndex < mInputPointVector->size(); ++vertexIndex) {
            const cgmath::Vector2f &point = (*mInputPointVector)[vertexIndex];
            for (size_t edgeIndex = 0; edgeIndex < mInputEdgeVector->size(); ++edgeIndex) {
                const Edge &edge = (*mInputEdgeVector)[edgeIndex];
                if (point != (*mInputPointVector)[edge.mPointIndexArray[0]]
                    && point != (*mInputPointVector)[edge.mPointIndexArray[1]]) {
                    if (exact::TestLineSegmentIntersectsPoint2f(
                            (*mInputPointVector)[edge.mPointIndexArray[0]],
                            (*mInputPointVector)[edge.mPointIndexArray[1]], point)) {
                        details.setSomePointsIntersectEdges(true);
                        goto foundVerticesIntersectingEdges;
                    }
                }
            }
        }
    }
foundVerticesIntersectingEdges:
    ;

    // Check if any of the input edges intersect.
    // This is an O(N^2) test that could be improved with an AABB tree.
    if (mInputEdgeVector != NULL) {
        for (size_t index1 = 0; index1 < (*mInputEdgeVector).size(); ++index1) {
            for (size_t index2 = index1 + 1; index2 < (*mInputEdgeVector).size(); ++index2) {
                if ((*mInputEdgeVector)[index1].mPointIndexArray[0]
                    != (*mInputEdgeVector)[index2].mPointIndexArray[0]
                    && (*mInputEdgeVector)[index1].mPointIndexArray[0]
                    != (*mInputEdgeVector)[index2].mPointIndexArray[1]
                    && (*mInputEdgeVector)[index1].mPointIndexArray[1]
                    != (*mInputEdgeVector)[index2].mPointIndexArray[0]
                    && (*mInputEdgeVector)[index1].mPointIndexArray[1]
                    != (*mInputEdgeVector)[index2].mPointIndexArray[1]) {
                    assert((*mInputEdgeVector)[index1].mPointIndexArray[0]
                        < mInputPointVector->size());
                    assert((*mInputEdgeVector)[index1].mPointIndexArray[1]
                        < mInputPointVector->size());
                    assert((*mInputEdgeVector)[index2].mPointIndexArray[0]
                        < mInputPointVector->size());
                    assert((*mInputEdgeVector)[index2].mPointIndexArray[1]
                        < mInputPointVector->size());
                    if (exact::TestLineSegmentsIntersect2f(
                            (*mInputPointVector)[(*mInputEdgeVector)[index1].mPointIndexArray[0]],
                            (*mInputPointVector)[(*mInputEdgeVector)[index1].mPointIndexArray[1]],
                            (*mInputPointVector)[(*mInputEdgeVector)[index2].mPointIndexArray[0]],
                            (*mInputPointVector)[(*mInputEdgeVector)[index2].mPointIndexArray[1]]
                            )) {
#if 0
                        std::cout << "Edge " << index1 << " intersects with edge "
                            << index2 << std::endl;
                        std::cout << "First edge" << std::endl;
                        std::cout.precision(12);
                        std::cout << "    " << (*mInputPointVector)[(*mInputEdgeVector)[index1]
                            .mPointIndexArray[0]] << std::endl;
                        std::cout << "    " << (*mInputPointVector)[(*mInputEdgeVector)[index1]
                            .mPointIndexArray[1]] << std::endl;
                        std::cout << "Second edge" << std::endl;
                        std::cout << "    " << (*mInputPointVector)[(*mInputEdgeVector)[index2]
                            .mPointIndexArray[0]] << std::endl;
                        std::cout << "    " << (*mInputPointVector)[(*mInputEdgeVector)[index2]
                            .mPointIndexArray[1]] << std::endl;
#endif
                        details.setSomeEdgesIntersect(true);
                        goto foundIntersectingEdges;
                    }
                }
            }
        }
    }
foundIntersectingEdges:
    ;

    // Check if any of the input edges are duplicates.
    if (mInputEdgeVector != NULL) {
        for (size_t index1 = 0; index1 + 1 < (*mInputEdgeVector).size(); ++index1) {
            for (size_t index2 = index1 + 1; index2 < (*mInputEdgeVector).size(); ++index2) {
                if (((*mInputEdgeVector)[index1].mPointIndexArray[0]
                        == (*mInputEdgeVector)[index2].mPointIndexArray[0]
                        && (*mInputEdgeVector)[index1].mPointIndexArray[1]
                        == (*mInputEdgeVector)[index2].mPointIndexArray[1])
                    || ((*mInputEdgeVector)[index1].mPointIndexArray[0]
                        == (*mInputEdgeVector)[index2].mPointIndexArray[1]
                        && (*mInputEdgeVector)[index1].mPointIndexArray[1]
                        == (*mInputEdgeVector)[index2].mPointIndexArray[0])) {
                    details.setSomeEdgesAreDuplicates(true);
                    goto foundDuplicateEdges;
                }
            }
        }
    }
foundDuplicateEdges:

    if (validationDetails != NULL) {
        *validationDetails = details;
    }

    return details.isValid();
}

void
PointTriangulator::triangulate()
{
    // If there are less than three input points, that's an error.
    assert(mInputPointVector->size() >= 3);

    createShuffledPointIndexVector();

    mMeshPtr.reset(new Mesh);
    mMeshHistoryPtr.reset(new MeshHistory);
    mMeshSnapshotPtr.reset(new MeshSnapshot);
    mNearestFaceVertexPtrVector.reserve(mInputPointVector->size());
    mInputPointVectorVertexPtrVector.resize(mInputPointVector->size());

    // Show all the vertices before we add them to the mesh.
    if (createMeshSnapshot()) {
        for (size_t index = 0; index < mInputPointVector->size(); ++index) {
            mMeshSnapshotPtr->addPoint(getShuffledPoint(index));
        }
    }

    // Create the first triangle as a separate step. The rest of the algorithm
    // depends on the existence of at least one triangle.
    createInitialTriangle();

    createMeshSnapshot();

    // Reset this so the results are deterministic.
    mFindNearestFaceRandomContext = 0;

    assert(mInputPointVector != NULL);
    for (size_t unshuffledIndex = 0; unshuffledIndex < mInputPointVector->size(); 
         ++unshuffledIndex) {

        if (unshuffledIndex == mUnshuffledTriangleIndexArray[0]
            || unshuffledIndex == mUnshuffledTriangleIndexArray[1]
            || unshuffledIndex == mUnshuffledTriangleIndexArray[2]) {
            continue;
        }

        // The next point to insert into the mesh.
        cgmath::Vector2f point = getShuffledPoint(unshuffledIndex);

        // Find the face that's closest to or intersects the point.
        FacePtr nearestFacePtr = findNearestFace(point);

#ifdef DEBUG
        // The point must not be coincident with any existing vertices.
        // All of the points in the input data must be unique.
        VertexPtr vertexPtr;
        assert(!findCoincidentVertex(nearestFacePtr, point, &vertexPtr));
#endif

        // Show the point in isolation before it's added to the mesh.
        if (createMeshSnapshot()) {
            mMeshSnapshotPtr->addPoint(point);
        }

        EdgePtr edgePtr;
        FacePtr facePtr;
        if (findIntersectingEdge(nearestFacePtr, point, &edgePtr)) {

            // The point intersects an existing edge.
            VertexPtr newVertexPtr = SplitEdge(edgePtr, point, mMeshPtr);
            processNewVertex(unshuffledIndex, newVertexPtr);

        } else if (findIntersectingFace(nearestFacePtr, point, &facePtr)) {

            // The point intersects an existing face.
            VertexPtr newVertexPtr = SplitFace(facePtr, point, mMeshPtr);
            processNewVertex(unshuffledIndex, newVertexPtr);

        } else {

            // The point must lie outside the perimeter of the mesh.
            VertexPtr newVertexPtr = addVertexOutsidePerimeter(nearestFacePtr, point);
            processNewVertex(unshuffledIndex, newVertexPtr);
        }
    }

    // It must be the case that all of the vertices that we created for the mesh
    // were also added to the VertexPtrVector.
    assert(mInputPointVector->size() == mNearestFaceVertexPtrVector.size());

    // The vector that maps input point indices to VertexPtr's must have
    // one entry for every input point.
    assert(mInputPointVectorVertexPtrVector.size() == mInputPointVector->size());

    // Insert the edges for constrained Delaunay triangulation if any were specified.
    insertConstrainedEdges();

    // If we haven't been writing out the entire history,
    // take a snapshot of the mesh now.
    if (mSvgFilename != ""
        && !mWriteEntireSvgHistory) {
        mMeshSnapshotPtr = mMeshHistoryPtr->createMeshSnapshot(mMeshPtr);
    }

    if (mSvgFilename != "") {
        mMeshHistoryPtr->writeToSvgFile(mSvgFilename);
    }

    createEdgeVector();
    createTriangleVector();

    destroyShuffledPointIndexVector();

    mMeshPtr.reset();
    mMeshHistoryPtr.reset();
    mMeshSnapshotPtr.reset();
    mNearestFaceVertexPtrVector.clear();
    mInputPointVectorVertexPtrVector.clear();
}

const PointTriangulator::EdgeVector &
PointTriangulator::outputEdgeVector() const
{
    return mOutputEdgeVector;
}

const PointTriangulator::TriangleVector &
PointTriangulator::outputTriangleVector() const
{
    return mOutputTriangleVector;
}

void
PointTriangulator::createInitialTriangle()
{
    assert(mInputPointVector->size() >= 3);
    cgmath::Vector2f p1 = getShuffledPoint(0);
    cgmath::Vector2f p2(0, 0);
    cgmath::Vector2f p3(0, 0);
    size_t index2 = 0;
    size_t index3 = 0;

    assert(mInputPointVector != NULL);
    for (index2 = 1; index2 < mInputPointVector->size(); ++index2) {
        p2 = getShuffledPoint(index2);
        for (index3 = index2 + 1; index3 < mInputPointVector->size(); ++index3) {
            p3 = getShuffledPoint(index3);
            if (exact::TestOrientation2d(p1, p2, p3) > 0.0) {
                goto found;
            }
            if (exact::TestOrientation2d(p1, p2, p3) < 0.0) {
                std::swap(p2, p3);
                std::swap(index2, index3);
                assert(exact::TestOrientation2d(p1, p2, p3) > 0.0);
                goto found;
            }
        }
    }

    // No set of three noncoincident, noncolinear vertices could be found in the mesh.
    assert(0);

found:
    VertexPtr v1 = mMeshPtr->createVertex();
    mNearestFaceVertexPtrVector.push_back(v1);
    v1->setPosition(p1);
    v1->setInputVertexIndex(mShuffledPointIndexVector[0]);
    mInputPointVectorVertexPtrVector[mShuffledPointIndexVector[0]] = v1;

    VertexPtr v2 = mMeshPtr->createVertex();
    mNearestFaceVertexPtrVector.push_back(v2);
    v2->setPosition(p2);
    v2->setInputVertexIndex(mShuffledPointIndexVector[index2]);
    mInputPointVectorVertexPtrVector[mShuffledPointIndexVector[index2]] = v2;

    VertexPtr v3 = mMeshPtr->createVertex();
    mNearestFaceVertexPtrVector.push_back(v3);
    v3->setPosition(p3);
    v3->setInputVertexIndex(mShuffledPointIndexVector[index3]);
    mInputPointVectorVertexPtrVector[mShuffledPointIndexVector[index3]] = v3;

    // Create the edges and the face and wire them all up.

    mUnshuffledTriangleIndexArray[0] = 0;
    mUnshuffledTriangleIndexArray[1] = index2;
    mUnshuffledTriangleIndexArray[2] = index3;

    EdgePtr e1 = mMeshPtr->createEdge();
    EdgePtr e2 = mMeshPtr->createEdge();
    EdgePtr e3 = mMeshPtr->createEdge();

    FacePtr f1 = mMeshPtr->createFace();

    e1->addAdjacentVertex(v1);
    e1->addAdjacentVertex(v2);
    e1->addAdjacentFace(f1);

    e2->addAdjacentVertex(v2);
    e2->addAdjacentVertex(v3);
    e2->addAdjacentFace(f1);

    e3->addAdjacentVertex(v3);
    e3->addAdjacentVertex(v1);
    e3->addAdjacentFace(f1);

    f1->addAdjacentVertex(v1);
    f1->addAdjacentVertex(v2);
    f1->addAdjacentVertex(v3);
    f1->addAdjacentEdge(e1);
    f1->addAdjacentEdge(e2);
    f1->addAdjacentEdge(e3);

    v1->addAdjacentEdge(e1);
    v1->addAdjacentEdge(e3);

    v2->addAdjacentEdge(e1);
    v2->addAdjacentEdge(e2);

    v3->addAdjacentEdge(e2);
    v3->addAdjacentEdge(e3);

    v1->addAdjacentFace(f1);
    v2->addAdjacentFace(f1);
    v3->addAdjacentFace(f1);

    AssertMeshConsistency(mMeshPtr);
}

bool
PointTriangulator::findCoincidentVertex(FacePtr nearestFacePtr, 
    const cgmath::Vector2f &point, VertexPtr *resultVertexPtr)
{
    for (int index = 0; index < nearestFacePtr->adjacentVertexCount(); ++index) {
        VertexPtr vertexPtr = nearestFacePtr->adjacentVertex(index);
        if (vertexPtr->position() == point) {
            *resultVertexPtr = vertexPtr;
            return true;
        }
    }

    *resultVertexPtr = mMeshPtr->vertexEnd();
    return false;
}

bool
PointTriangulator::findIntersectingEdge(FacePtr nearestFacePtr, 
    const cgmath::Vector2f &point, EdgePtr *resultEdgePtr)
{
    for (int index = 0; index < nearestFacePtr->adjacentEdgeCount(); ++index) {
        EdgePtr edgePtr = nearestFacePtr->adjacentEdge(index);
        assert(edgePtr->adjacentVertexCount() == 2);
        VertexPtr v1 = edgePtr->adjacentVertex(0);
        VertexPtr v2 = edgePtr->adjacentVertex(1);
        if (exact::TestLineSegmentIntersectsPoint2f(v1->position(), v2->position(), point)) {
            *resultEdgePtr = edgePtr;
            return true;
        }
    }

    *resultEdgePtr = mMeshPtr->edgeEnd();
    return false;
}

bool
PointTriangulator::findIntersectingFace(FacePtr nearestFacePtr, 
    const cgmath::Vector2f &point, FacePtr *resultFacePtr)
{
    assert(nearestFacePtr->adjacentVertexCount() == 3);
    VertexPtr v1 = nearestFacePtr->adjacentVertex(0);
    VertexPtr v2 = nearestFacePtr->adjacentVertex(1);
    VertexPtr v3 = nearestFacePtr->adjacentVertex(2);
    if (exact::TestOrientation2d(v1->position(), v2->position(), point) > 0.0
        && exact::TestOrientation2d(v2->position(), v3->position(), point) > 0.0
        && exact::TestOrientation2d(v3->position(), v1->position(), point) > 0.0) {
        *resultFacePtr = nearestFacePtr;
        return true;
    }

    *resultFacePtr = mMeshPtr->faceEnd();
    return false;
}

VertexPtr
PointTriangulator::addVertexOutsidePerimeter(FacePtr nearestFacePtr, const cgmath::Vector2f &point)
{
    assert(MeshHasConvexPerimeter(mMeshPtr));

    // Find the existing vertex on the mesh that's nearest to the
    // one we're about to add. This will be our starting point when
    // adding new triangles to the mesh.
    VertexPtr nearestExistingVertex = findNearestPerimeterVertex(nearestFacePtr, point);

    // Add the new vertex to the mesh.
    VertexPtr newVertexPtr = mMeshPtr->createVertex();
    newVertexPtr->setPosition(point);

    // Now we want to march around the perimeter of the mesh, starting
    // with the nearest existing vertex, adding new triangles until we're
    // faced with the possibility of adding a triangle that would be inverted.

    // Counterclockwise.
    VertexPtr vertexPtr = nearestExistingVertex;
    while (true) {
        VertexPtr nextVertexPtr = GetAdjacentVertexAroundPerimeter(vertexPtr, COUNTERCLOCKWISE);
        if (exact::TestOrientation2d(point, nextVertexPtr->position(), 
                vertexPtr->position()) > 0.0) {
            CreateFaceAndAdjacentEdges(newVertexPtr, nextVertexPtr, vertexPtr, mMeshPtr);
        } else {
            break;
        }
        vertexPtr = nextVertexPtr;
    }

    // Clockwise.
    vertexPtr = nearestExistingVertex;
    while (true) {
        VertexPtr nextVertexPtr = GetAdjacentVertexAroundPerimeter(vertexPtr, CLOCKWISE);
        if (exact::TestOrientation2d(point, vertexPtr->position(), 
                nextVertexPtr->position()) > 0.0) {
            CreateFaceAndAdjacentEdges(newVertexPtr, vertexPtr, nextVertexPtr, mMeshPtr);
        } else {
            break;
        }
        vertexPtr = nextVertexPtr;
    }

    AssertMeshConsistency(mMeshPtr);

    assert(MeshHasConvexPerimeter(mMeshPtr));

    return newVertexPtr;
}

void
PointTriangulator::processNewVertex(size_t unshuffledIndex, VertexPtr vertexPtr)
{
    mNearestFaceVertexPtrVector.push_back(vertexPtr);

    assert(unshuffledIndex < mShuffledPointIndexVector.size());
    size_t shuffledIndex = mShuffledPointIndexVector[unshuffledIndex];

    assert(shuffledIndex < mInputPointVectorVertexPtrVector.size());
    mInputPointVectorVertexPtrVector[shuffledIndex] = vertexPtr;

    vertexPtr->setInputVertexIndex(shuffledIndex);

    createMeshSnapshot();

    // It's possible we just made portions of the mesh non-Delaunay,
    // so test for these cases and correct them.
    correctNonDelaunayTrianglesAdjacentToVertex(vertexPtr);

    // The mesh must be consisitent.
    AssertMeshConsistency(mMeshPtr);

    // The mesh must satisfy the Delaunay criterion.
    assert(MeshIsDelaunay(mMeshPtr));

    // The mesh's perimeter must be convex.
    assert(MeshHasConvexPerimeter(mMeshPtr));
}

void
PointTriangulator::correctNonDelaunayTrianglesAdjacentToVertex(VertexPtr vertexPtr)
{
    // Iterate over all of the faces that are now adjacent to the
    // vertex we just added to the mesh.

    // Put all the adjacent faces in a list so we can iterate over them
    // without worrying about the edge swapping function modifying the list
    // as we're iterating over it.
    typedef std::list<FacePtr> FacePtrList;
    FacePtrList adjacentFacePtrList;
    assert(vertexPtr->adjacentFaceCount() >= 2
        || vertexPtr->adjacentFaceCount() <= 4);
    for (int faceIndex = 0; faceIndex < vertexPtr->adjacentFaceCount(); ++faceIndex) {
        FacePtr facePtr = vertexPtr->adjacentFace(faceIndex);
        adjacentFacePtrList.push_back(facePtr);
    }

    // Now iterate over the adjacent faces.
    for (FacePtrList::const_iterator iterator = adjacentFacePtrList.begin();
         iterator != adjacentFacePtrList.end(); ++iterator) {

        // Find the index of the vertex in the face.
        FacePtr facePtr = *iterator;
        assert(facePtr->adjacentVertexCount() == 3);
        int vertexIndex = 0;
        for (; vertexIndex < facePtr->adjacentVertexCount();
             ++vertexIndex) {
            if (facePtr->adjacentVertex(vertexIndex) == vertexPtr) {
                break;
            }
        }
        // We must have found the newly added vertex.
        assert(vertexIndex != facePtr->adjacentVertexCount());

        // Get the edge that's opposite the new vertex we added.
        int oppositeEdgeIndex = (vertexIndex + 1) % facePtr->adjacentEdgeCount();
        EdgePtr oppositeEdgePtr = facePtr->adjacentEdge(oppositeEdgeIndex);

        // The opposite edge's adjacent vertices must not be
        // the newly created vertex.
        assert(oppositeEdgePtr->adjacentVertexCount() == 2);
        assert(oppositeEdgePtr->adjacentVertex(0) != vertexPtr
            && oppositeEdgePtr->adjacentVertex(1) != vertexPtr);

        // If the edge only has one adjacent face, there are no faces adjacent to
        // the face we're considering, so there are no opposite vertices to
        // perform the in-circle test on.
        if (oppositeEdgePtr->adjacentFaceCount() < 2) {
            continue;
        }

        // Now find the adjacent face on the other side of the
        // opposite edge.
        FacePtr adjacentFacePtr = mMeshPtr->faceEnd();
        for (int faceIndex = 0; faceIndex < oppositeEdgePtr->adjacentFaceCount();
             ++faceIndex) {
            if (oppositeEdgePtr->adjacentFace(faceIndex) != facePtr) {
                adjacentFacePtr = oppositeEdgePtr->adjacentFace(faceIndex);
            }
        }
        // We must have found an adjacent face.
        assert(adjacentFacePtr != mMeshPtr->faceEnd());

        // Now, on the adjacent face, find the vertex opposite the edge.
        int edgeIndex = 0;
        for (; edgeIndex < adjacentFacePtr->adjacentEdgeCount();
             ++edgeIndex) {
            if (adjacentFacePtr->adjacentEdge(edgeIndex) == oppositeEdgePtr) {
                break;
            }
        }
        assert(edgeIndex != adjacentFacePtr->adjacentEdgeCount());
        vertexIndex = (edgeIndex + 2) % adjacentFacePtr->adjacentVertexCount();
        VertexPtr oppositeVertexPtr = adjacentFacePtr->adjacentVertex(vertexIndex);

        // The opposite vertex must not be adjacent to the opposite edge.
        assert(oppositeEdgePtr->adjacentVertexCount() == 2);
        assert(oppositeEdgePtr->adjacentVertex(0) != oppositeVertexPtr
            && oppositeEdgePtr->adjacentVertex(1) != oppositeVertexPtr);

        // If that vertex lies inside the bounding circle of the original triangle,
        // flip the edge.
        assert(facePtr->adjacentVertexCount() == 3);
        if (exact::TestInCircle(
                facePtr->adjacentVertex(0)->position(),
                facePtr->adjacentVertex(1)->position(),
                facePtr->adjacentVertex(2)->position(),
                oppositeVertexPtr->position()) > 0.0) {

            if (mSvgFilename != ""
                && mWriteEntireSvgHistory) {
                mMeshSnapshotPtr = mMeshHistoryPtr->createMeshSnapshot(mMeshPtr);
                cgmath::Vector2f circleCenter;
                float circleRadius = 0.0;
                if (CalculateCircleFromThreePoints(
                        facePtr->adjacentVertex(0)->position(),
                        facePtr->adjacentVertex(1)->position(),
                        facePtr->adjacentVertex(2)->position(),
                        circleCenter, circleRadius)) {
                    mMeshSnapshotPtr->setCircleCenter(circleCenter);
                    mMeshSnapshotPtr->setCircleRadius(circleRadius);
                }
            }

            // The opposite vertex lies inside the bounding circle
            // of the face being tested.
            SwapEdge(oppositeEdgePtr);

            if (mSvgFilename != ""
                && mWriteEntireSvgHistory) {
                mMeshSnapshotPtr = mMeshHistoryPtr->createMeshSnapshot(mMeshPtr);
            }

            // Then add the two triangles adjacent to the flipped edge to
            // the vector of adjacent faces so they'll both be tested again.
            assert(oppositeEdgePtr->adjacentFaceCount() == 2);
            adjacentFacePtrList.push_back(oppositeEdgePtr->adjacentFace(0));
            adjacentFacePtrList.push_back(oppositeEdgePtr->adjacentFace(1));
        }
    }
}

VertexPtr
PointTriangulator::findNearestPerimeterVertex(FacePtr nearestFacePtr, 
    const cgmath::Vector2f &point)
{
    VertexPtr nearestVertex = mMeshPtr->vertexEnd();
    float nearestVertexDistanceSquared = std::numeric_limits<float>::max();

    for (int index = 0; index < nearestFacePtr->adjacentVertexCount(); ++index) {
        VertexPtr vertexPtr = nearestFacePtr->adjacentVertex(index);

        // If the vertex is not on the perimeter, skip it.
        // Vertices on the perimeter will have one more adjacent edges
        // than adjacent faces.
        if (vertexPtr->adjacentEdgeCount() != vertexPtr->adjacentFaceCount() + 1) {
            continue;
        }

        // If the vertex is on the far side of the mesh, skip it.
        // We want the closest vertex that is visible to the point
        // being added, that isn't occluded by the mesh.
        VertexPtr v0 = GetAdjacentVertexAroundPerimeter(vertexPtr, COUNTERCLOCKWISE);
        VertexPtr v1 = GetAdjacentVertexAroundPerimeter(vertexPtr, CLOCKWISE);
        if (exact::TestOrientation2d(v0->position(), vertexPtr->position(), point) <= 0.0
            && exact::TestOrientation2d(vertexPtr->position(), v1->position(), point) <= 0.0) {
            continue;
        }

        float distanceSquared = (point - vertexPtr->position()).lengthSquared(); 
        if (distanceSquared < nearestVertexDistanceSquared) {
            nearestVertexDistanceSquared = distanceSquared;
            nearestVertex = vertexPtr;
        }
    }

    assert(nearestVertex != mMeshPtr->vertexEnd());
    assert(nearestVertexDistanceSquared != 0.0);

    return nearestVertex;
}

void
PointTriangulator::createEdgeVector()
{
    assert(mMeshPtr.get() != NULL);

    size_t outputEdgeIndex = 0;

    mOutputEdgeVector.clear();

    // First, add all the edges that were explicitly defined as constraining edges,
    // in the order that they were originally defined by the caller.
    // This allows the caller to assume edge correspondence.
    if (mInputEdgeVector != NULL) {
        mOutputEdgeVector.resize(mInputEdgeVector->size());
        for (EdgePtr edgePtr = mMeshPtr->edgeBegin(); 
             edgePtr != mMeshPtr->edgeEnd(); ++edgePtr) {

            assert(edgePtr->adjacentVertexCount() == 2);

            if (edgePtr->inputEdgeIndexIsDefined()) {

                assert(edgePtr->adjacentVertex(0)->inputVertexIndexIsDefined());
                assert(edgePtr->adjacentVertex(1)->inputVertexIndexIsDefined());

                size_t index = edgePtr->inputEdgeIndex();

                // To be consistent, we also ensure that order that the
                // vertices were specified in the original edge is maintained
                // in the output edge.
                assert(edgePtr->adjacentVertex(0)->inputVertexIndex()
                    == (*mInputEdgeVector)[index].mPointIndexArray[0]
                    || edgePtr->adjacentVertex(0)->inputVertexIndex()
                    == (*mInputEdgeVector)[index].mPointIndexArray[1]);
                assert(edgePtr->adjacentVertex(1)->inputVertexIndex() 
                    == (*mInputEdgeVector)[index].mPointIndexArray[0]
                    || edgePtr->adjacentVertex(1)->inputVertexIndex() 
                    == (*mInputEdgeVector)[index].mPointIndexArray[1]);
                mOutputEdgeVector[index] = (*mInputEdgeVector)[index];

                edgePtr->setOutputEdgeIndex(index);

                ++outputEdgeIndex;
            }
        }
    }

    assert(outputEdgeIndex == mOutputEdgeVector.size());

    // Then add all the rest of nonconstrained edges. The order does not matter.
    for (EdgePtr edgePtr = mMeshPtr->edgeBegin(); 
         edgePtr != mMeshPtr->edgeEnd(); ++edgePtr) {

        assert(edgePtr->adjacentVertexCount() == 2);

        if (!edgePtr->inputEdgeIndexIsDefined()) {
            Edge edge;
            for (size_t index = 0; index < 2; ++index) {
                assert(edgePtr->adjacentVertex(index)->inputVertexIndexIsDefined());
                edge.mPointIndexArray[index] = edgePtr->adjacentVertex(index)->inputVertexIndex();
            }
            mOutputEdgeVector.push_back(edge);

            edgePtr->setOutputEdgeIndex(outputEdgeIndex);
            ++outputEdgeIndex;
        }
    }
}

void
PointTriangulator::createTriangleVector()
{
    assert(mMeshPtr.get() != NULL);

    mOutputTriangleVector.clear();
    for (FacePtr facePtr = mMeshPtr->faceBegin(); 
         facePtr != mMeshPtr->faceEnd(); ++facePtr) {

        assert(facePtr->adjacentVertexCount() == 3);
        assert(facePtr->adjacentEdgeCount() == 3);

        Triangle triangle;
        for (size_t index = 0; index < 3; ++index) {
            assert(facePtr->adjacentVertex(index)->inputVertexIndexIsDefined());
            triangle.mPointIndexArray[index] = facePtr->adjacentVertex(index)->inputVertexIndex();

            assert(facePtr->adjacentEdge(index)->outputEdgeIndexIsDefined());
            triangle.mEdgeIndexArray[index] = facePtr->adjacentEdge(index)->outputEdgeIndex();
        }
        mOutputTriangleVector.push_back(triangle);
    }
}

bool
PointTriangulator::createMeshSnapshot()
{
    if (mSvgFilename != ""
        && mWriteEntireSvgHistory) {
        mMeshSnapshotPtr = mMeshHistoryPtr->createMeshSnapshot(mMeshPtr);
        return true;
    }

    return false;
}

void
PointTriangulator::createShuffledPointIndexVector()
{
    assert(mInputPointVector != NULL);
    mShuffledPointIndexVector.resize(mInputPointVector->size());

    for (size_t index = 0; index < mShuffledPointIndexVector.size(); ++index) {
        mShuffledPointIndexVector[index] = index;
    }

    if (!mShouldShuffleInputPoints) {
        return;
    }

    // We use a fixed seed so that the results, although random, are deterministic.
    unsigned context = 0x12345678;
    for (size_t index = 0; index < mShuffledPointIndexVector.size() - 1; ++index) {
        size_t randomIndex = (rand_r(&context) % (mShuffledPointIndexVector.size() - index))
            + index;
        std::swap(mShuffledPointIndexVector[index], mShuffledPointIndexVector[randomIndex]);
    }
}

void
PointTriangulator::destroyShuffledPointIndexVector()
{
    mShuffledPointIndexVector.clear();
}

const cgmath::Vector2f &
PointTriangulator::getShuffledPoint(size_t unshuffledIndex) const
{
    assert(unshuffledIndex < mShuffledPointIndexVector.size());

    assert(mInputPointVector != NULL);
    size_t shuffledIndex = mShuffledPointIndexVector[unshuffledIndex];

    assert(shuffledIndex < (*mInputPointVector).size());
    return (*mInputPointVector)[shuffledIndex];
}

FacePtr
PointTriangulator::findNearestFace(const cgmath::Vector2f &point) const
{
    // The idea here is, we randomly pick a face in the mesh, and then
    // walk from face to face in the direction of the target point
    // until we reach it or hit the edge of the mesh.

    // For our starting point, we want a random face. We maintain a vector
    // of vertices to choose from, and choose a random adjacent face to the randomly
    // chosen vertex, rather than maintain a vector of faces. This is because
    // vertices are only ever added to the mesh, and never removed.
    // This isn't true of faces, which can be removed and replaced with new faces.
    // Consequently, maintaining a vector of faces would be more complicated.

    // Rather than choose a single vertex as the starting point,
    // we randomly consider several and start with the closest one.

    VertexPtr minVertexPtr = mMeshPtr->vertexEnd();
    float minDistanceSquared = std::numeric_limits<float>::max();

    // A paper I read said to use the cube root of the number of faces.
    int cubeRoot = cbrtl(mMeshPtr->faceCount());
    for (int index = 0; index < cubeRoot; ++index) {

        // Randomly choose a vertex.
        assert(mNearestFaceVertexPtrVector.size() > 0);
        int vertexIndex = rand_r(&mFindNearestFaceRandomContext) 
            % mNearestFaceVertexPtrVector.size();
        VertexPtr vertexPtr = mNearestFaceVertexPtrVector[vertexIndex];

        float distanceSquared = (point - vertexPtr->position()).lengthSquared();
        if (distanceSquared < minDistanceSquared) {
            minDistanceSquared = distanceSquared;
            minVertexPtr = vertexPtr;
        }
    }

    VertexPtr vertexPtr = minVertexPtr;

    // Randomly choose a face that's adjacent to the vertex we randomly chose.
    assert(vertexPtr->adjacentFaceCount() > 0);
    int faceIndex = rand_r(&mFindNearestFaceRandomContext) % vertexPtr->adjacentFaceCount();
    FacePtr facePtr = vertexPtr->adjacentFace(faceIndex);

    // Now walk from face to face toward the target point until we reach it,
    // or the perimeter of the mesh.

    bool found = false;
    while (!found) {

        FacePtr previousFace = facePtr;

        // Pick a random edge of the face.
        assert(facePtr->adjacentEdgeCount() == 3);
        int edgeIndex = rand_r(&mFindNearestFaceRandomContext) % 3;

        int edgeCount = 0;
        for (; edgeCount < 3; ++edgeCount) {

            EdgePtr edgePtr = facePtr->adjacentEdge(edgeIndex);

            // Get the vertex on the face that is not contained in the edge.
            assert(facePtr->adjacentVertexCount() == 3);
            int vertexIndex = (edgeIndex + 2) % 3;
            VertexPtr vertexPtr = facePtr->adjacentVertex(vertexIndex);

            // Get the face that's on the opposite side of the edge.
            FacePtr neighboringFacePtr = mMeshPtr->faceEnd();
            if (edgePtr->adjacentFaceCount() == 2) {
                if (edgePtr->adjacentFace(0) == facePtr) {
                    neighboringFacePtr = edgePtr->adjacentFace(1);
                } else {
                    assert(edgePtr->adjacentFace(1) == facePtr);
                    neighboringFacePtr = edgePtr->adjacentFace(0);
                }
            }

            // If a neighboring face exists,
            // and the neighboring face is not the previous one,
            // and the vertex on the face is on the opposite side of the edge
            // relative to the target point, then step to the
            // neighboring face.
            if (neighboringFacePtr != mMeshPtr->faceEnd()
                && neighboringFacePtr != previousFace) {
                assert(edgePtr->adjacentVertexCount() == 2);
                const cgmath::Vector2f &e0 = edgePtr->adjacentVertex(0)->position();
                const cgmath::Vector2f &e1 = edgePtr->adjacentVertex(1)->position();
                float orientation1 = exact::TestOrientation2d(e0, e1, point);
                float orientation2 = exact::TestOrientation2d(e0, e1, vertexPtr->position());
                if ((orientation1 < 0.0 && orientation2 > 0.0)
                    || (orientation1 > 0.0 && orientation2 < 0.0)) {
                    previousFace = facePtr;
                    facePtr = neighboringFacePtr;
                    break;
                }
            }

            // Consider the next edge.
            edgeIndex = (edgeIndex + 1) % 3;
        }

        // We've considered all the edges and the target point is not
        // on the far side of any of them, so it's either inside 
        // the face, or intersects one of the vertices or edges,
        // or we've reached the perimeter of the mesh.
        if (edgeCount == 3) {
            found = true;
        }
    }

    return facePtr;
}

void
PointTriangulator::insertConstrainedEdges()
{
    if (mInputEdgeVector == NULL) {
        return;
    }

#ifdef DEBUG
    // The point indices of the edge's endpoints must all correspond
    // to valid point vector indices.
    for (size_t index = 0; index < mInputEdgeVector->size(); ++index) {
        const Edge &edge = (*mInputEdgeVector)[index];
        assert(edge.mPointIndexArray[0] < mInputPointVector->size());
        assert(edge.mPointIndexArray[1] < mInputPointVector->size());
    }
#endif

#ifdef DEBUG
    // None of the input edges may intersect with each other.
    for (size_t index1 = 0; index1 < mInputEdgeVector->size(); ++index1) {
        const Edge &e1 = (*mInputEdgeVector)[index1];
        for (size_t index2 = 0; index2 < mInputEdgeVector->size(); ++index2) {
            if (index1 != index2) {
                const Edge &e2 = (*mInputEdgeVector)[index2];
                if (e1.mPointIndexArray[0] != e2.mPointIndexArray[0]
                    && e1.mPointIndexArray[0] != e2.mPointIndexArray[1]
                    && e1.mPointIndexArray[1] != e2.mPointIndexArray[0]
                    && e1.mPointIndexArray[1] != e2.mPointIndexArray[1]) {
                    assert(!exact::TestLineSegmentsIntersect2f(
                               (*mInputPointVector)[e1.mPointIndexArray[0]],
                               (*mInputPointVector)[e1.mPointIndexArray[1]],
                               (*mInputPointVector)[e2.mPointIndexArray[0]],
                               (*mInputPointVector)[e2.mPointIndexArray[1]]));
                }
            }
        }
    }
#endif

#ifdef DEBUG
    // None of the input edges may be degenerate.
    for (size_t index = 0; index < mInputEdgeVector->size(); ++index) {
        const Edge &edge = (*mInputEdgeVector)[index];
        assert(edge.mPointIndexArray[0] != edge.mPointIndexArray[1]);
        assert((*mInputPointVector)[edge.mPointIndexArray[0]]
            != (*mInputPointVector)[edge.mPointIndexArray[1]]);
    }
#endif

#ifdef DEBUG
    // None of the input vertices may intersect the input edges.
    for (size_t vertexIndex = 0; vertexIndex < mInputPointVector->size(); ++vertexIndex) {
        const cgmath::Vector2f &point = (*mInputPointVector)[vertexIndex];
        for (size_t edgeIndex = 0; edgeIndex < mInputEdgeVector->size(); ++edgeIndex) {
            const Edge &edge = (*mInputEdgeVector)[edgeIndex];
            if (point != (*mInputPointVector)[edge.mPointIndexArray[0]]
                && point != (*mInputPointVector)[edge.mPointIndexArray[1]]) {
                assert(!exact::TestLineSegmentIntersectsPoint2f(
                           (*mInputPointVector)[edge.mPointIndexArray[0]],
                           (*mInputPointVector)[edge.mPointIndexArray[1]], point));
            }
        }
    }
#endif

    for (size_t index = 0; index < mInputEdgeVector->size(); ++index) {
        const Edge &edge = (*mInputEdgeVector)[index];

        size_t p0 = edge.mPointIndexArray[0];
        assert(p0 < mInputPointVectorVertexPtrVector.size());
        VertexPtr v0 = mInputPointVectorVertexPtrVector[p0];

        size_t p1 = edge.mPointIndexArray[1];
        assert(p1 < mInputPointVectorVertexPtrVector.size());
        VertexPtr v1 = mInputPointVectorVertexPtrVector[p1];

        EdgePtr newEdgePtr = insertConstrainedEdge(v0, v1);
        assert(!newEdgePtr->inputEdgeIndexIsDefined());
        newEdgePtr->setInputEdgeIndex(index);
    }

    // The mesh must be consisitent.
    AssertMeshConsistency(mMeshPtr);

    // Because we've inserted constrained edges, the mesh may or may not
    // satisfy the Delaunay criterion.

    // The mesh's perimeter must be convex.
    assert(MeshHasConvexPerimeter(mMeshPtr));
}

EdgePtr
PointTriangulator::insertConstrainedEdge(VertexPtr v0, VertexPtr v1)
{
    // If the edge we're trying to insert already exist, there's nothing to do.
    EdgePtr newEdgePtr = GetEdgeLeadingToNeighboringVertex(v0, v1, mMeshPtr);
    if (newEdgePtr != mMeshPtr->edgeEnd()) {
        return newEdgePtr;
    }

    // Maintain a list of vertices that are to the left of, and to the right of, the edge.
    VertexPtrVector leftVertexPtrVector;
    VertexPtrVector rightVertexPtrVector;
    leftVertexPtrVector.push_back(v0);
    rightVertexPtrVector.push_back(v0);

    // Starting at point v0, examine all adjacent faces and see which one's
    // opposite edge intersects our edge.
    EdgePtr oppositeEdgePtr = mMeshPtr->edgeEnd();
    FacePtr intersectedFacePtr = mMeshPtr->faceEnd();
    for (int index = 0; index < v0->adjacentFaceCount(); ++index) {
        FacePtr facePtr = v0->adjacentFace(index);
        oppositeEdgePtr = GetEdgeOppositeVertex(facePtr, v0);
        assert(oppositeEdgePtr->adjacentVertexCount() == 2);
        if (exact::TestLineSegmentsIntersect2f(v0->position(), v1->position(),
                oppositeEdgePtr->adjacentVertex(0)->position(),
                oppositeEdgePtr->adjacentVertex(1)->position())) {
            intersectedFacePtr = facePtr;
            break;
        }
    }
    assert(oppositeEdgePtr != mMeshPtr->edgeEnd());
    assert(intersectedFacePtr != mMeshPtr->faceEnd());

    // Get a reference to the next face, on the opposite side of that edge.
    // If there isn't one, something is very wrong.
    FacePtr nextFacePtr = GetNeighboringFaceAcrossEdge(intersectedFacePtr, oppositeEdgePtr);

    // Delete the original face.
    DeleteFaceAndAllOrphanedNonconstrainedAdjacentEdges(intersectedFacePtr, mMeshPtr);

    while (true) {

        assert(oppositeEdgePtr->adjacentVertexCount() == 2);
        float orientation0 = exact::TestOrientation2d(v0->position(), v1->position(),
            oppositeEdgePtr->adjacentVertex(0)->position());
#ifdef DEBUG
        float orientation1 = exact::TestOrientation2d(v0->position(), v1->position(),
            oppositeEdgePtr->adjacentVertex(1)->position());
#endif
        // Neither endpoint may intersect the constraining edge.
        assert(orientation0 != 0.0);
        assert(orientation1 != 0.0);

        if (orientation0 > 0.0) {
            assert(orientation1 < 0.0);
            if (leftVertexPtrVector[leftVertexPtrVector.size() - 1]
                != oppositeEdgePtr->adjacentVertex(0)) {
                leftVertexPtrVector.push_back(oppositeEdgePtr->adjacentVertex(0));
            }
            if (rightVertexPtrVector[rightVertexPtrVector.size() - 1]
                != oppositeEdgePtr->adjacentVertex(1)) {
                rightVertexPtrVector.push_back(oppositeEdgePtr->adjacentVertex(1));
            }
        } else {
            assert(orientation0 < 0.0);
            assert(orientation1 > 0.0);
            if (leftVertexPtrVector[leftVertexPtrVector.size() - 1]
                != oppositeEdgePtr->adjacentVertex(1)) {
                leftVertexPtrVector.push_back(oppositeEdgePtr->adjacentVertex(1));
            }
            if (rightVertexPtrVector[rightVertexPtrVector.size() - 1]
                != oppositeEdgePtr->adjacentVertex(0)) {
                rightVertexPtrVector.push_back(oppositeEdgePtr->adjacentVertex(0));
            }
        }

        // Find the vertex opposite the edge.
        VertexPtr oppositeVertexPtr = GetVertexOppositeEdge(nextFacePtr, oppositeEdgePtr);

        // If we've reached the final vertex, we're done.
        // Remove the face, and retriangulate the region.
        if (oppositeVertexPtr == v1) {
            DeleteFaceAndAllOrphanedNonconstrainedAdjacentEdges(nextFacePtr, mMeshPtr);

            createMeshSnapshot();

            // Retriangulate the two possibly concave polygonal regions.
            leftVertexPtrVector.push_back(v1);
            rightVertexPtrVector.push_back(v1);

            // The vertices must be specified in counterclockwise order.
            std::reverse(leftVertexPtrVector.begin(), leftVertexPtrVector.end());
            triangulateConstrainedEdgeHalfRegion(leftVertexPtrVector,
                0, leftVertexPtrVector.size() - 1);

            // These vertices are already in counterclockwise order and do not
            // need to be reversed.
            triangulateConstrainedEdgeHalfRegion(rightVertexPtrVector,
                0, rightVertexPtrVector.size() - 1);

            AssertMeshConsistency(mMeshPtr);
            
            createMeshSnapshot();

            EdgePtr newEdgePtr = GetEdgeLeadingToNeighboringVertex(v0, v1, mMeshPtr);
            assert(newEdgePtr != mMeshPtr->edgeEnd());

            return newEdgePtr;
        }

        // On the next face, see which of the two other edges intersect the v0 v1 edge.
        EdgePtr newOppositeEdgePtr = mMeshPtr->edgeEnd();
        for (int index = 0; index < nextFacePtr->adjacentEdgeCount(); ++index) {
            EdgePtr edgePtr = nextFacePtr->adjacentEdge(index);
            if (edgePtr != oppositeEdgePtr) {
                assert(edgePtr->adjacentVertexCount() == 2);
                if (exact::TestLineSegmentsIntersect2f(v0->position(), v1->position(),
                        edgePtr->adjacentVertex(0)->position(),
                        edgePtr->adjacentVertex(1)->position())) {
                    newOppositeEdgePtr = edgePtr;
                    break;
                }
            }
        }
        assert(newOppositeEdgePtr != mMeshPtr->edgeEnd());

        // Find the face on the opposite side of that edge.
        FacePtr newNextFacePtr = GetNeighboringFaceAcrossEdge(nextFacePtr, newOppositeEdgePtr);

        DeleteFaceAndAllOrphanedNonconstrainedAdjacentEdges(nextFacePtr, mMeshPtr);

        // Advance to the next face.
        oppositeEdgePtr = newOppositeEdgePtr;
        nextFacePtr = newNextFacePtr;
    }
}

void
PointTriangulator::triangulateConstrainedEdgeHalfRegion(const VertexPtrVector &vertexPtrVector,
    size_t first, size_t last)
{
    assert(first < vertexPtrVector.size());
    assert(last < vertexPtrVector.size());

    // The region describes a single edge, so there's nothing to do.
    if (last - first + 1 < 3) {
        return;
    }

    // The region describes a single triangle. Create it, 
    // and then there's nothing left to do.
    if (last - first + 1 == 3) {
        assert(vertexPtrVector[first + 2] == vertexPtrVector[last]);
        CreateFaceAndAdjacentEdges(vertexPtrVector[first],
            vertexPtrVector[first + 1], vertexPtrVector[first + 2], mMeshPtr);
        return;
    }

    // Find the vertex that, with the two endpoints of the region, 
    // forms a triangle whose circumcircle contains none of the other vertices.
    size_t middle = first + 1;
    for (size_t index = first + 1; index <= last - 1; ++index) {
        if (exact::TestInCircle(
                vertexPtrVector[first]->position(),
                vertexPtrVector[middle]->position(),
                vertexPtrVector[last]->position(),
                vertexPtrVector[index]->position()) > 0.0) {
            // Point 'index' is inside the circle intersecting points first, middle, and last.
            middle = index;
        }
    }

    // Create the triangle.
    CreateFaceAndAdjacentEdges(vertexPtrVector[first],
        vertexPtrVector[middle], vertexPtrVector[last], mMeshPtr);

    // Triangulate the two remaining half-regions.
    triangulateConstrainedEdgeHalfRegion(vertexPtrVector, first, middle);
    triangulateConstrainedEdgeHalfRegion(vertexPtrVector, middle, last);
}

} // namespace delaunay
