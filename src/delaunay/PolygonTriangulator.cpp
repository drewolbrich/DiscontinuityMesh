// Copyright 2008 Drew Olbrich

#include "PolygonTriangulator.h"

#include <iostream>

#include <exact/GeometricPredicates.h>

#include "Mesh.h"
#include "MeshTypes.h"
#include "Vertex.h"
#include "Edge.h"
#include "Face.h"
#include "MeshHistory.h"
#include "MeshConsistency.h"
#include "ValidationDetails.h"

namespace delaunay {

PolygonTriangulator::PolygonTriangulator()
    : mInputPointVector(NULL),
      mInputPolygonVector(NULL),
      mInputEdgeVector(NULL),
      mSvgFilename(""),
      mAggregateInputEdgeVector(),
      mPointTriangulator(),
      mConnectivityEdgeVector(),
      mTriangleOrientationVector(),
      mOutputEdgeVector(),
      mOutputTriangleVector(),
      mShouldOutputPointTriangulatorOutputEdgeVector(),
      mPointTriangulatorOutputEdgeToPolygonTriangularOutputEdgeVector()
{
}

PolygonTriangulator::~PolygonTriangulator()
{
}

void
PolygonTriangulator::setInputPointVector(const PointVector *inputPointVector)
{
    mInputPointVector = inputPointVector;
}

const PolygonTriangulator::PointVector *
PolygonTriangulator::inputPointVector() const
{
    return mInputPointVector;
}

void
PolygonTriangulator::setInputPolygonVector(const PolygonVector *inputPolygonVector)
{
    mInputPolygonVector = inputPolygonVector;
}

const PolygonTriangulator::PolygonVector *
PolygonTriangulator::inputPolygonVector() const
{
    return mInputPolygonVector;
}

void
PolygonTriangulator::setInputEdgeVector(const EdgeVector *inputEdgeVector)
{
    mInputEdgeVector = inputEdgeVector;
}

const PolygonTriangulator::EdgeVector *
PolygonTriangulator::inputEdgeVector() const
{
    return mInputEdgeVector;
}

void
PolygonTriangulator::setSvgFilename(const std::string &svgFilename)
{
    mSvgFilename = svgFilename;
}

const std::string &
PolygonTriangulator::svgFilename() const
{
    
    return mSvgFilename;
}

bool
PolygonTriangulator::validate(ValidationDetails *validationDetails)
{
    ValidationDetails details;

    // First, perform all the point triangulation validation.
    mPointTriangulator.setInputPointVector(mInputPointVector);
    createAggregateInputEdgeVector();
    mPointTriangulator.setInputEdgeVector(&mAggregateInputEdgeVector);
    mPointTriangulator.validate(&details);

    // Find the leftmost point and the polygon that it is a member of.
    // That point and the two adjacent points should be convex when 
    // examined in counterclockwise order.

    bool first = true;
    cgmath::Vector2f leftmostPoint(0.0, 0.0);
    const Polygon *leftmostPointPolygon = NULL;
    size_t leftmostPointIndexInPolygon = 0;
    for (size_t index = 0; index < mInputPolygonVector->size(); ++index) {
        const Polygon &polygon = (*mInputPolygonVector)[index];
        for (size_t index = 0; index < polygon.mPointIndexVector.size(); ++index) {
            assert(index < polygon.mPointIndexVector.size());
            cgmath::Vector2f point = (*mInputPointVector)[polygon.mPointIndexVector[index]];

            // We actually find the leftmost point that is also 
            // the topmost point, if there are multiple leftmost points
            // that all have the same X coordinate.
            // This ensure that we will not wind up with a polygon
            // vertex that together with its two neighbors are all colinear.
            if (first
                || (point[0] < leftmostPoint[0])
                || (point[0] == leftmostPoint[0]
                    && point[1] < leftmostPoint[1])) {
                first = false;
                leftmostPoint = point;
                leftmostPointPolygon = &polygon;
                leftmostPointIndexInPolygon = index;
            }
        }
    }
    if (!first) {
        assert(leftmostPointPolygon != NULL);
        size_t polygonPointIndex0 = (leftmostPointIndexInPolygon
            + leftmostPointPolygon->mPointIndexVector.size() - 1)
            % leftmostPointPolygon->mPointIndexVector.size();
        size_t polygonPointIndex1 = leftmostPointIndexInPolygon;
        size_t polygonPointIndex2 = (leftmostPointIndexInPolygon + 1)
            % leftmostPointPolygon->mPointIndexVector.size();
        assert(polygonPointIndex0 < leftmostPointPolygon->mPointIndexVector.size());
        size_t index0 = leftmostPointPolygon->mPointIndexVector[polygonPointIndex0];
        assert(polygonPointIndex1 < leftmostPointPolygon->mPointIndexVector.size());
        size_t index1 = leftmostPointPolygon->mPointIndexVector[polygonPointIndex1];
        assert(polygonPointIndex2 < leftmostPointPolygon->mPointIndexVector.size());
        size_t index2 = leftmostPointPolygon->mPointIndexVector[polygonPointIndex2];
        cgmath::Vector2f p0 = (*mInputPointVector)[index0];
        cgmath::Vector2f p1 = (*mInputPointVector)[index1];
        cgmath::Vector2f p2 = (*mInputPointVector)[index2];
        // If any of the three points are coincident, we're already invalid.
        if (p0 != p1
            && p1 != p2
            && p2 != p0) {
            float orientation = exact::TestOrientation2d(p0, p1, p2);
            // This should never happen, because it'd mean that the points were
            // all colinear, and the logic above should ensure that 
            // we never got into that situation.
            assert(orientation != 0.0);
            if (orientation < 0.0) {
                // The exterior perimeter polygon is clockwise, defining a hole,
                // which is invalid input.
                details.setExteriorPerimeterIsNotCounterclockwise(true);
            }
        }
    }

    // There are other nontrivial cases that we're not bothering to check for,
    // like holes nested within holes, or non-holes nested within non-holes.

    // We also aren't handling the case where there is a non-hole
    // exterior polygon that is the leftmost polygon,
    // with a disjoint hole to the right of it.

    // We're really only properly testing the case where there is a single
    // polygon with zero or more holes in it, and no nesting.

    if (validationDetails != NULL) {
        *validationDetails = details;
    }

    return details.isValid();
}

void
PolygonTriangulator::triangulate()
{
    mPointTriangulator.setInputPointVector(mInputPointVector);

    createAggregateInputEdgeVector();
    mPointTriangulator.setInputEdgeVector(&mAggregateInputEdgeVector);

    mPointTriangulator.triangulate();

    createConnectivityEdgeVector();

    createOutputTriangleVector();

    createOutputEdgeVector();

    // Free up the temporary storage we no longer need.
    mConnectivityEdgeVector.clear();
    mTriangleOrientationVector.clear();
    mShouldOutputPointTriangulatorOutputEdgeVector.clear();
    mPointTriangulatorOutputEdgeToPolygonTriangularOutputEdgeVector.clear();

    if (mSvgFilename != "") {
        writeToSvgFile();
    }

#if 0
    for (size_t index = 0; index < mInputPointVector->size(); ++index) {
        std::cout << "point " << index << " = " << (*mInputPointVector)[index] << std::endl;
    }

    for (size_t index = 0; index < mOutputEdgeVector.size(); ++index) {
        std::cout << "edge " << index << " = points "
            << mOutputEdgeVector[index].mPointIndexArray[0]
            << " " << mOutputEdgeVector[index].mPointIndexArray[1] << std::endl;
    }

    for (size_t index = 0; index < mOutputTriangleVector.size(); ++index) {
        std::cout << "triangle " << index << " = points "
            << mOutputTriangleVector[index].mPointIndexArray[0]
            << " " << mOutputTriangleVector[index].mPointIndexArray[1] 
            << " " << mOutputTriangleVector[index].mPointIndexArray[2] 
            << ", edges = "
            << mOutputTriangleVector[index].mEdgeIndexArray[0]
            << " " << mOutputTriangleVector[index].mEdgeIndexArray[1] 
            << " " << mOutputTriangleVector[index].mEdgeIndexArray[2] << std::endl;
    }
#endif
}

const PolygonTriangulator::EdgeVector &
PolygonTriangulator::outputEdgeVector() const
{
    return mOutputEdgeVector;
}

const PolygonTriangulator::TriangleVector &
PolygonTriangulator::outputTriangleVector() const
{
    return mOutputTriangleVector;
}

void
PolygonTriangulator::createAggregateInputEdgeVector()
{
    mAggregateInputEdgeVector.clear();

    for (size_t index = 0; index < mInputPolygonVector->size(); ++index) {
        const Polygon &polygon = (*mInputPolygonVector)[index];
        for (size_t index = 0; index < polygon.mPointIndexVector.size(); ++index) {
            PointTriangulator::Edge edge;
            edge.mPointIndexArray[0] = polygon.mPointIndexVector[index];
            edge.mPointIndexArray[1] = polygon.mPointIndexVector[
                (index + 1) % polygon.mPointIndexVector.size()];
            mAggregateInputEdgeVector.push_back(edge);
        }
    }

    if (mInputEdgeVector != NULL) {
        for (size_t index = 0; index < mInputEdgeVector->size(); ++index) {
            const Edge &edge = (*mInputEdgeVector)[index];
            PointTriangulator::Edge pointTriangulatorEdge;
            pointTriangulatorEdge.mPointIndexArray[0] = edge.mPointIndexArray[0];
            pointTriangulatorEdge.mPointIndexArray[1] = edge.mPointIndexArray[1];
            mAggregateInputEdgeVector.push_back(pointTriangulatorEdge);
        }
    }
}

void
PolygonTriangulator::createConnectivityEdgeVector()
{
    const PointTriangulator::EdgeVector &edgeVector 
        = mPointTriangulator.outputEdgeVector();
    const PointTriangulator::TriangleVector &triangleVector 
        = mPointTriangulator.outputTriangleVector();

    ConnectivityEdge connectivityEdge;
    connectivityEdge.mTriangleCount = 0;
    mConnectivityEdgeVector.resize(edgeVector.size(), connectivityEdge);

    for (size_t triangleIndex = 0; triangleIndex < triangleVector.size(); ++triangleIndex) {
        const PointTriangulator::Triangle &triangle = triangleVector[triangleIndex];
        for (size_t index = 0; index < 3; ++index) {
            size_t edgeIndex = triangle.mEdgeIndexArray[index];
            assert(edgeIndex < mConnectivityEdgeVector.size());
            ConnectivityEdge &connectivityEdge = mConnectivityEdgeVector[edgeIndex];
            assert(connectivityEdge.mTriangleCount < 2);
            connectivityEdge.mTriangleIndexArray[connectivityEdge.mTriangleCount] = triangleIndex;
            ++connectivityEdge.mTriangleCount;
        }
    }

#ifdef DEBUG
    for (size_t index = 0; index < mConnectivityEdgeVector.size(); ++index) {
        assert(mConnectivityEdgeVector[index].mTriangleCount > 0);
    }
#endif
}

void
PolygonTriangulator::createOutputTriangleVector()
{
    const PointTriangulator::TriangleVector &triangleVector
        = mPointTriangulator.outputTriangleVector();
    mTriangleOrientationVector.resize(triangleVector.size(), UNDETERMINED);

    for (size_t triangleIndex = 0; triangleIndex < triangleVector.size(); ++triangleIndex) {
        if (mTriangleOrientationVector[triangleIndex] == UNDETERMINED) {
            const PointTriangulator::Triangle &triangle = triangleVector[triangleIndex];
            TriangleOrientation triangleOrientation = UNDETERMINED;
            for (size_t index = 0; index < 3; ++index) {
                size_t edgeIndex = triangle.mEdgeIndexArray[index];
                if (edgeIsInputPolygonEdge(edgeIndex)) {

                    // Determine whether the triangle is inside or outside
                    // the perimeter defined by the edges.

                    assert(edgeIndex < mAggregateInputEdgeVector.size());
                    const PointTriangulator::Edge &edge = mAggregateInputEdgeVector[edgeIndex];

                    // At this point, edge.mPointIndexArray[0] and edge.mPointIndexArray[1]
                    // are defined in the order specified in the original polygons
                    // (counterclockwise or clockwise).

                    assert((triangle.mPointIndexArray[index] 
                            == edge.mPointIndexArray[0]
                            && triangle.mPointIndexArray[(index + 1) % 3] 
                            == edge.mPointIndexArray[1])
                        || (triangle.mPointIndexArray[index] 
                            == edge.mPointIndexArray[1]
                            && triangle.mPointIndexArray[(index + 1) % 3] 
                            == edge.mPointIndexArray[0]));

                    if (triangle.mPointIndexArray[index] == edge.mPointIndexArray[0]) {
                        triangleOrientation = INSIDE;
                    } else {
                        triangleOrientation = OUTSIDE;
                    }
                }
            }
            if (triangleOrientation != UNDETERMINED) {
                setTriangleNeighborhoodOrientation(triangleIndex, triangleOrientation);
            }
        }
    }

    mOutputTriangleVector.clear();
    for (size_t triangleIndex = 0; triangleIndex < triangleVector.size(); ++triangleIndex) {
        if (mTriangleOrientationVector[triangleIndex] == INSIDE) {
            Triangle triangle;
            for (size_t index = 0; index < 3; ++index) {
                assert(triangleIndex < triangleVector.size());
                triangle.mPointIndexArray[index] 
                    = triangleVector[triangleIndex].mPointIndexArray[index];

                // We don't have enough information to 
                // set triangle.mEdgeIndexArray[index] yet at this point.
                // This is handled later in createOutputEdgeVector.
            }
            mOutputTriangleVector.push_back(triangle);
        }
    }
}

void
PolygonTriangulator::createOutputEdgeVector()
{
    // I need a vector of all edges that are adjacent to one or more
    // triangles whose orientation is INSIDE.
    // Each edge must reference two points in the input point vector.

    // I also need to update the vector of output triangles so that each
    // triangle references the three output edges that are adjacent to it.

    // Loop over the input triangles. For each triangle marked INSIDE,
    // loop over the triangle's edges and in another vector
    // that parallels the input edges, flag them so that they
    // will become output edges.

    const PointTriangulator::EdgeVector &pointTriangulatorOutputEdgeVector
        = mPointTriangulator.outputEdgeVector();
    mShouldOutputPointTriangulatorOutputEdgeVector.resize(
        pointTriangulatorOutputEdgeVector.size(), false);
    const PointTriangulator::TriangleVector &triangleVector
        = mPointTriangulator.outputTriangleVector();
    for (size_t triangleIndex = 0; triangleIndex < triangleVector.size(); ++triangleIndex) {
        if (mTriangleOrientationVector[triangleIndex] == INSIDE) {
            for (size_t index = 0; index < 3; ++index) {
                size_t edgeIndex = triangleVector[triangleIndex].mEdgeIndexArray[index];
                assert(edgeIndex < mShouldOutputPointTriangulatorOutputEdgeVector.size());
                mShouldOutputPointTriangulatorOutputEdgeVector[edgeIndex] = true;
            }
        }
    }

    // Then loop over all the new vector of edges, and create the
    // output vector of edges from it, adding only those edges that are
    // marked INSIDE. But also, while doing this, create another vector
    // that maps the input edges to the output edges.
    // (The default value for this vector should be the max size_t value.)

    mPointTriangulatorOutputEdgeToPolygonTriangularOutputEdgeVector.resize(
        pointTriangulatorOutputEdgeVector.size(),
        std::numeric_limits<size_t>::max());
    mOutputEdgeVector.clear();
    for (size_t index = 0; index < pointTriangulatorOutputEdgeVector.size(); ++index) {
        assert(index < mShouldOutputPointTriangulatorOutputEdgeVector.size());
        if (mShouldOutputPointTriangulatorOutputEdgeVector[index]) {
            assert(index 
                < mPointTriangulatorOutputEdgeToPolygonTriangularOutputEdgeVector.size());
            mPointTriangulatorOutputEdgeToPolygonTriangularOutputEdgeVector[index]
                = mOutputEdgeVector.size();
            Edge edge;
            edge.mPointIndexArray[0] 
                = pointTriangulatorOutputEdgeVector[index].mPointIndexArray[0];
            edge.mPointIndexArray[1] 
                = pointTriangulatorOutputEdgeVector[index].mPointIndexArray[1];
            mOutputEdgeVector.push_back(edge);
        }
    }

    // Then loop over all the input triangles again, and for all INSIDE
    // triangles, update the edge index arrays, using the mapping
    // of input edges to output edges.
    // Do an assert for the max size_t value.

    size_t outputTriangleIndex = 0;
    for (size_t triangleIndex = 0; triangleIndex < triangleVector.size(); ++triangleIndex) {
        if (mTriangleOrientationVector[triangleIndex] == INSIDE) {
            assert(outputTriangleIndex < mOutputTriangleVector.size());
            Triangle &triangle = mOutputTriangleVector[outputTriangleIndex];
            ++outputTriangleIndex;
            for (size_t index = 0; index < 3; ++index) {
                assert(triangleIndex < triangleVector.size());
                size_t edgeIndex = triangleVector[triangleIndex].mEdgeIndexArray[index];
                assert(edgeIndex 
                    < mPointTriangulatorOutputEdgeToPolygonTriangularOutputEdgeVector.size());

                triangle.mEdgeIndexArray[index] 
                    = mPointTriangulatorOutputEdgeToPolygonTriangularOutputEdgeVector[
                        edgeIndex];
            }
        }
    }
}

bool
PolygonTriangulator::edgeIsInputPolygonEdge(size_t index) const
{
    if (mInputEdgeVector != NULL) {
        return index < mAggregateInputEdgeVector.size() - mInputEdgeVector->size();
    } else {
        return index < mAggregateInputEdgeVector.size();
    }
}

void
PolygonTriangulator::setTriangleNeighborhoodOrientation(size_t triangleIndex,
    TriangleOrientation triangleOrientation)
{
    assert(triangleIndex < mTriangleOrientationVector.size());
    if (mTriangleOrientationVector[triangleIndex] != UNDETERMINED) {
        return;
    }

    mTriangleOrientationVector[triangleIndex] = triangleOrientation;

    const PointTriangulator::TriangleVector &triangleVector
        = mPointTriangulator.outputTriangleVector();

    assert(triangleIndex < triangleVector.size());
    const PointTriangulator::Triangle &triangle = triangleVector[triangleIndex];
    for (size_t index = 0; index < 3; ++index) {
        size_t edgeIndex = triangle.mEdgeIndexArray[index];
        if (!edgeIsInputPolygonEdge(edgeIndex)) {
            assert(edgeIndex < mConnectivityEdgeVector.size());
            const ConnectivityEdge &connectivityEdge = mConnectivityEdgeVector[edgeIndex];
            if (connectivityEdge.mTriangleCount == 2) {
                if (connectivityEdge.mTriangleIndexArray[0] == triangleIndex) {
                    setTriangleNeighborhoodOrientation(connectivityEdge.mTriangleIndexArray[1],
                        triangleOrientation);
                } else {
                    assert(connectivityEdge.mTriangleIndexArray[1] == triangleIndex);
                    setTriangleNeighborhoodOrientation(connectivityEdge.mTriangleIndexArray[0],
                        triangleOrientation);
                }
            }
        }
    }
}

void
PolygonTriangulator::writeToSvgFile()
{
    MeshHistory meshHistory;
    MeshPtr meshPtr(new Mesh);

    std::vector<VertexPtr> vertexPtrVector;
    for (size_t index = 0; index < mInputPointVector->size(); ++index) {
        VertexPtr vertexPtr = meshPtr->createVertex();
        vertexPtr->setPosition((*mInputPointVector)[index]);
        vertexPtrVector.push_back(vertexPtr);
    }

    std::vector<EdgePtr> edgePtrVector;
    for (size_t index = 0; index < mOutputEdgeVector.size(); ++index) {
        EdgePtr edgePtr = meshPtr->createEdge();

        assert(mOutputEdgeVector[index].mPointIndexArray[0] < vertexPtrVector.size());
        VertexPtr v1 = vertexPtrVector[mOutputEdgeVector[index].mPointIndexArray[0]];
        assert(mOutputEdgeVector[index].mPointIndexArray[1] < vertexPtrVector.size());
        VertexPtr v2 = vertexPtrVector[mOutputEdgeVector[index].mPointIndexArray[1]];

        edgePtr->addAdjacentVertex(v1);
        edgePtr->addAdjacentVertex(v2);

        v1->addAdjacentEdge(edgePtr);
        v2->addAdjacentEdge(edgePtr);

        edgePtrVector.push_back(edgePtr);
    }

    for (size_t index = 0; index < mOutputTriangleVector.size(); ++index) {
        FacePtr facePtr = meshPtr->createFace();

        assert(mOutputTriangleVector[index].mPointIndexArray[0] < vertexPtrVector.size());
        VertexPtr v1 = vertexPtrVector[mOutputTriangleVector[index].mPointIndexArray[0]];
        assert(mOutputTriangleVector[index].mPointIndexArray[1] < vertexPtrVector.size());
        VertexPtr v2 = vertexPtrVector[mOutputTriangleVector[index].mPointIndexArray[1]];
        assert(mOutputTriangleVector[index].mPointIndexArray[2] < vertexPtrVector.size());
        VertexPtr v3 = vertexPtrVector[mOutputTriangleVector[index].mPointIndexArray[2]];

        facePtr->addAdjacentVertex(v1);
        facePtr->addAdjacentVertex(v2);
        facePtr->addAdjacentVertex(v3);

        v1->addAdjacentFace(facePtr);
        v2->addAdjacentFace(facePtr);
        v3->addAdjacentFace(facePtr);

        assert(mOutputTriangleVector[index].mEdgeIndexArray[0] < edgePtrVector.size());
        EdgePtr e1 = edgePtrVector[mOutputTriangleVector[index].mEdgeIndexArray[0]];
        assert(mOutputTriangleVector[index].mEdgeIndexArray[1] < edgePtrVector.size());
        EdgePtr e2 = edgePtrVector[mOutputTriangleVector[index].mEdgeIndexArray[1]];
        assert(mOutputTriangleVector[index].mEdgeIndexArray[2] < edgePtrVector.size());
        EdgePtr e3 = edgePtrVector[mOutputTriangleVector[index].mEdgeIndexArray[2]];

        facePtr->addAdjacentEdge(e1);
        facePtr->addAdjacentEdge(e2);
        facePtr->addAdjacentEdge(e3);

        e1->addAdjacentFace(facePtr);
        e2->addAdjacentFace(facePtr);
        e3->addAdjacentFace(facePtr);
    }

    AssertMeshConsistency(meshPtr);

    meshHistory.createMeshSnapshot(meshPtr);
    meshHistory.writeToSvgFile(mSvgFilename);
}

} // namespace delaunay
