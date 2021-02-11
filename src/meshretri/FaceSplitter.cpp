// Copyright 2009 Drew Olbrich

#include "FaceSplitter.h"

#include <con/Streams.h>
#include <delaunay/ValidationDetails.h>
#include <exact/GeometricPredicates.h>
#include <mesh/InterpolateAttributes.h>
#include <mesh/MeshOperations.h>
#include <mesh/FaceOperations.h>
#include <mesh/EdgeOperations.h>
#include <svg/SvgWriter.h>

#include "Retriangulator.h"
#include "RetriangulatorFace.h"
#include "RetriangulatorEdge.h"
#include "MeshAttributes.h"

namespace meshretri {

FaceSplitter::FaceSplitter()
    : mMesh(NULL),
      mRetriangulator(NULL),
      mIsDegreeZeroDiscontinuityAttributeKey(),
      mRetriangulatorFace(NULL),
      mRetriangulatorEdgeArray(NULL),
      mVector2fToPointIndexMap(),
      mPointVector(),
      mPolygonVector(),
      mEdgeVector(),
      mPointIndexToVertexPtrVector(),
      mPointIndexToVector3fVector(),
      mEdgeIndexToEdgePtrVector(),
      mEdgesAlreadySubmitted()
{
}

FaceSplitter::~FaceSplitter()
{
}

void
FaceSplitter::setMesh(mesh::Mesh *mesh)
{
    mMesh = mesh;

    mIsDegreeZeroDiscontinuityAttributeKey = GetIsDegreeZeroDiscontinuityAttributeKey(*mMesh);
}

mesh::Mesh *
FaceSplitter::mesh() const
{
    return mMesh;
}

void
FaceSplitter::setRetriangulator(Retriangulator *retriangulator)
{
    mRetriangulator = retriangulator;
}

Retriangulator *
FaceSplitter::retriangulator() const
{
    return mRetriangulator;
}

void
FaceSplitter::setRetriangulatorFace(RetriangulatorFace *retriangulatorFace)
{
    mRetriangulatorFace = retriangulatorFace;
}

RetriangulatorFace *
FaceSplitter::retriangulatorFace() const
{
    return mRetriangulatorFace;
}

void
FaceSplitter::splitFace()
{
    assert(mMesh != NULL);
    assert(mRetriangulatorEdgeArray == NULL);

    assert(mRetriangulatorFace != NULL);

    initializePolygonTriangulator();

    mesh::FacePtr oldFacePtr = mRetriangulatorFace->facePtr();

    createPolygonFromFaceVertices(oldFacePtr);

    createEdges();

    triangulate();

    // Add all the CDT output edges to the mesh if they 
    // don't already exist.
    const delaunay::PolygonTriangulator::EdgeVector &outputEdgeVector
        = mPolygonTriangulator.outputEdgeVector();
    mEdgeIndexToEdgePtrVector.resize(outputEdgeVector.size());
    for (size_t edgeIndex = 0; edgeIndex < outputEdgeVector.size(); ++edgeIndex) {
        const delaunay::PolygonTriangulator::Edge &outputEdge = outputEdgeVector[edgeIndex];

        assert(outputEdge.mPointIndexArray[0] < mPointIndexToVertexPtrVector.size());
        mesh::VertexPtr v0 = mPointIndexToVertexPtrVector[outputEdge.mPointIndexArray[0]];

        assert(outputEdge.mPointIndexArray[1] < mPointIndexToVertexPtrVector.size());
        mesh::VertexPtr v1 = mPointIndexToVertexPtrVector[outputEdge.mPointIndexArray[1]];

        mesh::AdjacentEdgeIterator iterator = v0->findAdjacentEdgeByVertex(v1);
        if (iterator != v0->adjacentEdgeEnd()) {
            mEdgeIndexToEdgePtrVector[edgeIndex] = *iterator;
        } else {
            assert(v1->findAdjacentEdgeByVertex(v0) == v1->adjacentEdgeEnd());
            mesh::EdgePtr edgePtr = mMesh->createEdge();
            edgePtr->addAdjacentVertex(v0);
            edgePtr->addAdjacentVertex(v1);
            v0->addAdjacentEdge(edgePtr);
            v1->addAdjacentEdge(edgePtr);
            mEdgeIndexToEdgePtrVector[edgeIndex] = edgePtr;

            assert(mRetriangulator != NULL);
            if (mRetriangulator->newEdgeBooleanAttributeKey().isDefined()) {
                edgePtr->setBool(mRetriangulator->newEdgeBooleanAttributeKey(), true);
            }
        }
    }

    // Add all the CDT output triangles to the mesh.
    const delaunay::PolygonTriangulator::TriangleVector &outputTriangleVector
        = mPolygonTriangulator.outputTriangleVector();
    for (size_t triangleIndex = 0; triangleIndex < outputTriangleVector.size(); 
         ++triangleIndex) {
        const delaunay::PolygonTriangulator::Triangle &outputTriangle 
            = outputTriangleVector[triangleIndex];

        // Create the new face in the mesh.
        mesh::FacePtr facePtr = mMesh->createFace();

        // Copy the original face's attributes to the new face.
        facePtr->copyAttributes(*oldFacePtr);

        for (size_t index = 0; index < 3; ++index) {
            size_t pointIndex = outputTriangle.mPointIndexArray[index];
            assert(pointIndex < mPointIndexToVertexPtrVector.size());
            mesh::VertexPtr vertexPtr = mPointIndexToVertexPtrVector[pointIndex];

            facePtr->addAdjacentVertex(vertexPtr);
            vertexPtr->addAdjacentFace(facePtr);

            // Assign interpolated face vertex attributes to the
            // new face for this vertex, interpolating from the original
            // face's face vertices.
            assert(mRetriangulatorFace->originalVertexPtrVector().size() == 3);
            mesh::AssignInterpolatedFaceVertexAttributes(facePtr, vertexPtr, 
                oldFacePtr, 
                mRetriangulatorFace->originalVertexPtrVector());
        }

        for (size_t index = 0; index < 3; ++index) {
            size_t edgeIndex = outputTriangle.mEdgeIndexArray[index];
            assert(edgeIndex < mEdgeIndexToEdgePtrVector.size());
            mesh::EdgePtr edgePtr = mEdgeIndexToEdgePtrVector[edgeIndex];
            facePtr->addAdjacentEdge(edgePtr);
            edgePtr->addAdjacentFace(facePtr);
        }
    }

    // Interpolate new vertex attributes for the newly added vertices in the middle of
    // the new face, and the newly added vertices along the edges of the new face.
    for (size_t index = 0; index < mPointIndexToVertexPtrVector.size(); ++index) {
        mesh::VertexPtr vertexPtr = mPointIndexToVertexPtrVector[index];
        if (std::find(
                mRetriangulatorFace->originalVertexPtrVector().begin(),
                mRetriangulatorFace->originalVertexPtrVector().end(),
                vertexPtr)
            != mRetriangulatorFace->originalVertexPtrVector().end()) {
            mesh::AssignInterpolatedVertexAttributes(vertexPtr,
                mRetriangulatorFace->originalVertexPtrVector());
        }
    }

    // Unwire the original face from all its neighbors and delete it.
    mesh::DeleteFace(mMesh, oldFacePtr);
}

void
FaceSplitter::setRetriangulatorEdgeArray(const RetriangulatorEdge *retriangulatorEdgeArray)
{
    mRetriangulatorEdgeArray = retriangulatorEdgeArray;
}

const RetriangulatorEdge *
FaceSplitter::retriangulatorEdgeArray() const
{
    return mRetriangulatorEdgeArray;
}

void
FaceSplitter::splitBackprojectionFace(TriangleVector *triangleVector)
{
    assert(mMesh == NULL);
    assert(mRetriangulatorEdgeArray != NULL);

    initializePolygonTriangulator();

    createPolygonFromRetriangulatorEdgeArray();

    createEdges();

    triangulate();

    // Return the vector of triangles calculated by the CDT.
    const delaunay::PolygonTriangulator::TriangleVector &cdtTriangleVector
        = mPolygonTriangulator.outputTriangleVector();
    (*triangleVector).clear();
    for (size_t index = 0; index < cdtTriangleVector.size(); ++index) {
        const delaunay::PolygonTriangulator::Triangle cdtTriangle = cdtTriangleVector[index];
        Triangle triangle;
        for (size_t index = 0; index < 3; ++index) {
            size_t cdtPointIndex = cdtTriangle.mPointIndexArray[index];
            assert(cdtPointIndex < mPointIndexToVector3fVector.size());
            triangle.mPointArray[index] = mPointIndexToVector3fVector[cdtPointIndex];
        }
        (*triangleVector).push_back(triangle);
    }
}

cgmath::Vector2f
FaceSplitter::createVector2f(const cgmath::Vector3f &vector3f)
{
    assert(mRetriangulatorFace != NULL);

    return cgmath::Vector2f(vector3f, 
        mRetriangulatorFace->axis0(), mRetriangulatorFace->axis1());
}

size_t
FaceSplitter::addPointFromVector3f(const cgmath::Vector3f &point,
    bool isDegreeZeroDiscontinuity)
{
    cgmath::Vector2f point2f = createVector2f(point);

    size_t index = 0;
    if (findPoint(point2f, &index)) {
        if (isDegreeZeroDiscontinuity) {
            assert(index < mPointIndexToVertexPtrVector.size());
            mesh::VertexPtr vertexPtr = mPointIndexToVertexPtrVector[index];
            vertexPtr->setBool(mIsDegreeZeroDiscontinuityAttributeKey, true);
        }
        return index;
    }

    mesh::VertexPtr vertexPtr = mMesh->createVertex();
    vertexPtr->setPosition(point);

    if (isDegreeZeroDiscontinuity) {
        vertexPtr->setBool(mIsDegreeZeroDiscontinuityAttributeKey, true);
    }

    return addPoint(point2f, vertexPtr);
}

size_t
FaceSplitter::addPointFromVertex(mesh::VertexPtr vertexPtr)
{
    cgmath::Vector2f point2f = createVector2f(vertexPtr->position());

    size_t index = 0;
    if (findPoint(point2f, &index)) {
        return index;
    }

    return addPoint(point2f, vertexPtr);
}

size_t
FaceSplitter::addBackprojectionPointFromVertex(mesh::VertexPtr vertexPtr)
{
    return addBackprojectionPointFromVector3f(vertexPtr->position());
}

size_t
FaceSplitter::addBackprojectionPointFromVector3f(const cgmath::Vector3f &point)
{
    cgmath::Vector2f point2f = createVector2f(point);

    size_t index = 0;
    if (findPoint(point2f, &index)) {
        return index;
    }


    index = addPointToVector2fToPointIndexMap(point2f);

    assert(index == mPointIndexToVector3fVector.size());
    mPointIndexToVector3fVector.push_back(point);

    return index;
}

bool
FaceSplitter::findPoint(const cgmath::Vector2f &point2f, size_t *index)
{
    Vector2fToPointIndexMap::const_iterator iterator 
        = mVector2fToPointIndexMap.find(point2f); 
    if (iterator != mVector2fToPointIndexMap.end()) {
        // We've already encountered a point at this location.
        *index = (*iterator).second;
        return true;
    }

    return false;
}

size_t
FaceSplitter::addPoint(const cgmath::Vector2f &point2f, mesh::VertexPtr vertexPtr)
{
    size_t index = addPointToVector2fToPointIndexMap(point2f);

    assert(mPointIndexToVertexPtrVector.size() == index);
    mPointIndexToVertexPtrVector.push_back(vertexPtr);

    return index;
}

size_t
FaceSplitter::addPointToVector2fToPointIndexMap(const cgmath::Vector2f &point2f)
{
    size_t index = mPointVector.size();
    mPointVector.push_back(point2f);
    mVector2fToPointIndexMap[point2f] = index;

    return index;
}

void
FaceSplitter::initializePolygonTriangulator()
{
    assert(mPointVector.size() == 0);
    assert(mPolygonVector.size() == 0);
    assert(mEdgeVector.size() == 0);
    assert(mPointIndexToVertexPtrVector.size() == 0);
    assert(mEdgeIndexToEdgePtrVector.size() == 0);

    mPolygonTriangulator.setInputPointVector(&mPointVector);
    mPolygonTriangulator.setInputPolygonVector(&mPolygonVector);
    mPolygonTriangulator.setInputEdgeVector(&mEdgeVector);
}

void
FaceSplitter::createPolygonFromFaceVertices(mesh::FacePtr facePtr)
{
    // Assign the points that form the perimeter of the polygon.
    // Since we have already split the mesh edges, there may be
    // more than three vertices.
    assert(facePtr->adjacentVertexCount() > 2);
    delaunay::PolygonTriangulator::Polygon polygon;
    size_t lastPointIndex = 0;
    size_t firstPointIndex = 0;
    for (mesh::AdjacentVertexIterator iterator = facePtr->adjacentVertexBegin();
         iterator != facePtr->adjacentVertexEnd(); ++iterator) {

        mesh::VertexPtr vertexPtr = *iterator;
        size_t pointIndex = addPointFromVertex(vertexPtr);

        for (size_t index = 0; index < mRetriangulatorFace->faceLineSegmentCount(); ++index) {
            const FaceLineSegment &faceLineSegment = mRetriangulatorFace->faceLineSegment(index);
            if ((faceLineSegment.worldPosition(0) == vertexPtr->position()
                    && faceLineSegment.isDegreeZeroDiscontinuity(0))
                || (faceLineSegment.worldPosition(1) == vertexPtr->position()
                    && faceLineSegment.isDegreeZeroDiscontinuity(1))) {
                vertexPtr->setBool(mIsDegreeZeroDiscontinuityAttributeKey, true);
            }
        }

        // Only add edges that are not degenerate.
        if (iterator == facePtr->adjacentVertexBegin()) {
            polygon.mPointIndexVector.push_back(pointIndex);
            firstPointIndex = pointIndex;
            lastPointIndex = pointIndex;
        } else if (pointIndex != lastPointIndex
            && pointIndex != firstPointIndex) {
            polygon.mPointIndexVector.push_back(pointIndex);
            assert(mEdgesAlreadySubmitted.find(std::make_pair(lastPointIndex, pointIndex))
                == mEdgesAlreadySubmitted.end()
                && mEdgesAlreadySubmitted.find(std::make_pair(pointIndex, lastPointIndex))
                == mEdgesAlreadySubmitted.end());
            mEdgesAlreadySubmitted.insert(std::make_pair(lastPointIndex, pointIndex));
            lastPointIndex = pointIndex;
        }
    }

    if (lastPointIndex != firstPointIndex) {
        assert(mEdgesAlreadySubmitted.find(std::make_pair(lastPointIndex, firstPointIndex))
            == mEdgesAlreadySubmitted.end()
            && mEdgesAlreadySubmitted.find(std::make_pair(firstPointIndex, lastPointIndex))
            == mEdgesAlreadySubmitted.end());
        mEdgesAlreadySubmitted.insert(std::make_pair(lastPointIndex, firstPointIndex));
    }

    // Set the single CDT polygon that's being triangulated.
    mPolygonVector.push_back(polygon);
}

void
FaceSplitter::createPolygonFromRetriangulatorEdgeArray()
{
    delaunay::PolygonTriangulator::Polygon polygon;

    mesh::FacePtr facePtr = mRetriangulatorFace->facePtr();
    assert(facePtr->adjacentVertexCount() == 3);

    size_t lastPointIndex = 0;
    size_t firstPointIndex = 0;
    mesh::AdjacentVertexIterator iterator = facePtr->adjacentVertexBegin();
    for (size_t index = 0; index < 3; ++index) {
        const RetriangulatorEdge &retriangulatorEdge = mRetriangulatorEdgeArray[index];

        mesh::VertexPtr vertexPtr = *iterator;
        size_t pointIndex = addBackprojectionPointFromVertex(vertexPtr);

        // Only add edges that are not degenerate.
        if (iterator == facePtr->adjacentVertexBegin()) {
            polygon.mPointIndexVector.push_back(pointIndex);
            firstPointIndex = pointIndex;
            lastPointIndex = pointIndex;
        } else if (pointIndex != lastPointIndex
            && pointIndex != firstPointIndex) {
            polygon.mPointIndexVector.push_back(pointIndex);
            assert(mEdgesAlreadySubmitted.find(std::make_pair(lastPointIndex, pointIndex))
                == mEdgesAlreadySubmitted.end()
                && mEdgesAlreadySubmitted.find(std::make_pair(pointIndex, lastPointIndex))
                == mEdgesAlreadySubmitted.end());
            mEdgesAlreadySubmitted.insert(std::make_pair(lastPointIndex, pointIndex));
            lastPointIndex = pointIndex;
        }

        mesh::VertexPtr edgeVertex0; 
        mesh::VertexPtr edgeVertex1;
        mesh::GetEdgeAdjacentVertices(retriangulatorEdge.edgePtr(), &edgeVertex0, &edgeVertex1);

        assert(vertexPtr == edgeVertex0 || vertexPtr == edgeVertex1);

        // Depending on the orientation of the edge with respect to the face vertices,
        // we may have to traverse the EdgePoints along the edge in reverse order.
        bool reverseEdge = (vertexPtr == edgeVertex1);

        size_t edgePointCount = retriangulatorEdge.edgePointCount();
        for (size_t index = 0; index < edgePointCount; ++index) {
            const EdgePoint &edgePoint = retriangulatorEdge.edgePoint(
                reverseEdge ? edgePointCount - 1 - index : index);

            const cgmath::Vector3f point = edgePoint.position();
            size_t pointIndex = addBackprojectionPointFromVector3f(point);

            // Only add edges that are not degenerate.
            if (pointIndex != lastPointIndex
                && pointIndex != firstPointIndex) {
                polygon.mPointIndexVector.push_back(pointIndex);
                assert(mEdgesAlreadySubmitted.find(std::make_pair(lastPointIndex, pointIndex))
                    == mEdgesAlreadySubmitted.end()
                    && mEdgesAlreadySubmitted.find(std::make_pair(pointIndex, lastPointIndex))
                    == mEdgesAlreadySubmitted.end());
                mEdgesAlreadySubmitted.insert(std::make_pair(lastPointIndex, pointIndex));
                lastPointIndex = pointIndex;
            }
        }

        ++iterator;
    }

    if (lastPointIndex != firstPointIndex) {
        assert(mEdgesAlreadySubmitted.find(std::make_pair(lastPointIndex, firstPointIndex))
            == mEdgesAlreadySubmitted.end()
            && mEdgesAlreadySubmitted.find(std::make_pair(firstPointIndex, lastPointIndex))
            == mEdgesAlreadySubmitted.end());
        mEdgesAlreadySubmitted.insert(std::make_pair(lastPointIndex, firstPointIndex));
    }

    // Set the single CDT polygon that's being triangulated.
    mPolygonVector.push_back(polygon);
}

void
FaceSplitter::createEdges()
{
    // Assign the constraining edges within the interior of the polygon.
    for (size_t index = 0; index < mRetriangulatorFace->faceLineSegmentCount(); ++index) {
        const FaceLineSegment &faceLineSegment = mRetriangulatorFace->faceLineSegment(index);

        if (faceLineSegment.isColinearWithExistingMeshEdge()) {
            // The line segment is colinear with an existing mesh edge.
            // We don't want to submit it to the CDT as an constraining edge,
            // because that should already be taken care of when the
            // polygon perimeter edges are submitted.
            continue;
        }

        delaunay::PolygonTriangulator::Edge edge;
        if (mMesh == NULL) {
            edge.mPointIndexArray[0] = addBackprojectionPointFromVector3f(
                faceLineSegment.worldPosition(0));
            edge.mPointIndexArray[1] = addBackprojectionPointFromVector3f(
                faceLineSegment.worldPosition(1));
        } else {
            edge.mPointIndexArray[0] = addPointFromVector3f(
                faceLineSegment.worldPosition(0),
                faceLineSegment.isDegreeZeroDiscontinuity(0));
            edge.mPointIndexArray[1] = addPointFromVector3f(
                faceLineSegment.worldPosition(1),
                faceLineSegment.isDegreeZeroDiscontinuity(1));
        }

        if (edge.mPointIndexArray[0] == edge.mPointIndexArray[1]) {
            // The edge is degenerate, so don't add it.
            continue;
        }

        if (mEdgesAlreadySubmitted.find(
                std::make_pair(edge.mPointIndexArray[0], edge.mPointIndexArray[1]))
            != mEdgesAlreadySubmitted.end()
            || mEdgesAlreadySubmitted.find(
                std::make_pair(edge.mPointIndexArray[1], edge.mPointIndexArray[0]))
            != mEdgesAlreadySubmitted.end()) {
            // We've already encountered this edge, so don't submit it again.
            continue;
        }

        mEdgeVector.push_back(edge);
        mEdgesAlreadySubmitted.insert(
            std::make_pair(edge.mPointIndexArray[0], edge.mPointIndexArray[1]));
    }
}

void
FaceSplitter::triangulate()
{
#ifdef DEBUG
    delaunay::ValidationDetails validationDetails;
    if (!mPolygonTriangulator.validate(&validationDetails)) {
        con::fatal << "CDT input is invalid: " << validationDetails.asString() << std::endl;
        assert(mRetriangulator != NULL);
        mRetriangulatorFace->writeToSvgFile(mRetriangulator);
        assert(0);
    }
#endif

    // Compute the CDT.
    mPolygonTriangulator.triangulate();
}

} // namespace meshretri
