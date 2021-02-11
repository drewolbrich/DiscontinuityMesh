// Copyright 2009 Drew Olbrich

#include "Triangulator.h"

#include <cassert>
#include <cstdlib>

#include <cgmath/Vector3fOperations.h>
#include <mesh/FaceOperations.h>
#include <mesh/MeshOperations.h>
#include <mesh/InterpolateAttributes.h>
#include <con/Streams.h>
#include <delaunay/ValidationDetails.h>

namespace mesh {

Triangulator::Triangulator()
    : mMesh(NULL),
      mPolygonTriangulator(),
      mPointVector(),
      mPolygonVector(),
      mEdgeVector(),
      mPointIndexToVertexPtrVector(),
      mEdgeIndexToEdgePtrVector()
{
}

Triangulator::~Triangulator()
{
}

void
Triangulator::setMesh(Mesh *mesh)
{
    mMesh = mesh;
}

Mesh *
Triangulator::mesh() const
{
    return mMesh;
}

void
Triangulator::triangulate()
{
    initializePolygonTriangulator();

    mTriangulatedFaces = 0;

    FacePtr nextFacePtr;
    for (FacePtr facePtr = mMesh->faceBegin(); 
         nextFacePtr = facePtr, ++nextFacePtr, facePtr != mMesh->faceEnd();
         facePtr = nextFacePtr) {

        if (facePtr->adjacentVertexCount() < 4) {
            // The face is already a triangle, so there's nothing to do.
            continue;
        }

        triangulateFace(facePtr);
        ++mTriangulatedFaces;
    }
}

unsigned
Triangulator::triangulatedFaces() const
{
    return mTriangulatedFaces;
}

void
Triangulator::initializePolygonTriangulator()
{
    mPolygonTriangulator.setInputPointVector(&mPointVector);
    mPolygonTriangulator.setInputPolygonVector(&mPolygonVector);
    mPolygonTriangulator.setInputEdgeVector(&mEdgeVector);
}

void
Triangulator::triangulateFace(FacePtr facePtr)
{
    // Handle quadrilaterals as a special case, with a function that
    // won't fail when passed a degenerate face.
    if (facePtr->adjacentVertexCount() == 4) {
        TriangulateQuadrilateralFace(mMesh, facePtr);
        return;
    }

    loadPolygonTriangulatorWithFace(facePtr);

    // TODO: Check for faces with degenerate or self-intersecting edges,
    // and use some other method to triangulate them,
    // since the Delaunay triangulation code can't handle them.
    // We're not sure what should happen here.

    // Test for cases that the CDT code can't handle.
    delaunay::ValidationDetails validationDetails;
    if (!mPolygonTriangulator.validate(&validationDetails)) {
        con::fatal << "(Triangulator) Cannot triangulate face. "
            << "(" << validationDetails.asString() << ")" << std::endl;
#if 0
        con::fatal << "Face vertices = " << facePtr->adjacentVertexCount() << std::endl;
        con::fatal << "Face area = " << GetFaceArea(facePtr) << std::endl;
        con::fatal << "Face centroid = " << GetFaceCentroid(facePtr) << std::endl;
        size_t index = 0;
        for (AdjacentVertexIterator iterator = facePtr->adjacentVertexBegin();
             iterator != facePtr->adjacentVertexEnd(); ++iterator) {
            con::fatal << "Vertex " << index << " position = " 
                << (*iterator)->position() << std::endl;
            ++index;
        }
#endif
        abort();
    }

    // Compute the CDT.
    mPolygonTriangulator.triangulate();

    replaceMeshFaceWithTriangles(facePtr);
}

void
Triangulator::loadPolygonTriangulatorWithFace(FacePtr facePtr)
{
    unsigned axis0 = 0;
    unsigned axis1 = 0;
    cgmath::GetPrimaryAxesMostOrthogonalToVector(
        GetFaceGeometricNormal(facePtr), &axis0, &axis1);

    mPointVector.clear();
    mPolygonVector.clear();
    mEdgeVector.clear();
    mPointIndexToVertexPtrVector.clear();
    mEdgeIndexToEdgePtrVector.clear();

    delaunay::PolygonTriangulator::Polygon polygon;

    for (AdjacentVertexIterator iterator = facePtr->adjacentVertexBegin();
         iterator != facePtr->adjacentVertexEnd(); ++iterator) {
        VertexPtr vertexPtr = *iterator;

        cgmath::Vector2f point(vertexPtr->position(), axis0, axis1);

        unsigned pointIndex = mPointVector.size();
        mPointVector.push_back(point);

        assert(mPointIndexToVertexPtrVector.size() == pointIndex);
        mPointIndexToVertexPtrVector.push_back(vertexPtr);

        polygon.mPointIndexVector.push_back(pointIndex);
    }

    mPolygonVector.push_back(polygon);
}

void
Triangulator::replaceMeshFaceWithTriangles(FacePtr oldFacePtr)
{
    std::vector<ConstVertexPtr> oldFaceVertexPtrVector;
    for (AdjacentVertexConstIterator iterator = oldFacePtr->adjacentVertexBegin();
         iterator != oldFacePtr->adjacentVertexEnd(); ++iterator) {
        oldFaceVertexPtrVector.push_back(*iterator);
    }

    typedef std::vector<EdgePtr> EdgeIndexToEdgePtrVector; 
    EdgeIndexToEdgePtrVector edgeIndexToEdgePtrVector; 

    // Add all the CDT output edges to the mesh if they 
    // don't already exist.
    const delaunay::PolygonTriangulator::EdgeVector &outputEdgeVector
        = mPolygonTriangulator.outputEdgeVector();
    edgeIndexToEdgePtrVector.resize(outputEdgeVector.size());
    for (size_t edgeIndex = 0; edgeIndex < outputEdgeVector.size(); ++edgeIndex) {
        const delaunay::PolygonTriangulator::Edge &outputEdge = outputEdgeVector[edgeIndex];

        assert(outputEdge.mPointIndexArray[0] < mPointIndexToVertexPtrVector.size());
        VertexPtr v0 = mPointIndexToVertexPtrVector[outputEdge.mPointIndexArray[0]];

        assert(outputEdge.mPointIndexArray[1] < mPointIndexToVertexPtrVector.size());
        VertexPtr v1 = mPointIndexToVertexPtrVector[outputEdge.mPointIndexArray[1]];

        AdjacentEdgeIterator iterator = v0->findAdjacentEdgeByVertex(v1);
        if (iterator != v0->adjacentEdgeEnd()) {
            edgeIndexToEdgePtrVector[edgeIndex] = *iterator;
        } else {
            assert(v1->findAdjacentEdgeByVertex(v0) == v1->adjacentEdgeEnd());
            EdgePtr edgePtr = mMesh->createEdge();
            edgePtr->addAdjacentVertex(v0);
            edgePtr->addAdjacentVertex(v1);
            v0->addAdjacentEdge(edgePtr);
            v1->addAdjacentEdge(edgePtr);
            edgeIndexToEdgePtrVector[edgeIndex] = edgePtr;
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
        FacePtr facePtr = mMesh->createFace();

        // Copy the original face's attributes to the new face.
        facePtr->copyAttributes(*oldFacePtr);

        for (size_t index = 0; index < 3; ++index) {
            size_t pointIndex = outputTriangle.mPointIndexArray[index];
            assert(pointIndex < mPointIndexToVertexPtrVector.size());
            VertexPtr vertexPtr = mPointIndexToVertexPtrVector[pointIndex];

            facePtr->addAdjacentVertex(vertexPtr);
            vertexPtr->addAdjacentFace(facePtr);

            // Assign interpolated face vertex attributes to the
            // new face for this vertex, interpolating from the original
            // face's face vertices.
            AssignInterpolatedFaceVertexAttributes(facePtr, vertexPtr, 
                oldFacePtr, oldFaceVertexPtrVector);
        }

        for (size_t index = 0; index < 3; ++index) {
            size_t edgeIndex = outputTriangle.mEdgeIndexArray[index];
            assert(edgeIndex < edgeIndexToEdgePtrVector.size());
            EdgePtr edgePtr = edgeIndexToEdgePtrVector[edgeIndex];
            facePtr->addAdjacentEdge(edgePtr);
            edgePtr->addAdjacentFace(facePtr);
        }
    }

    // Interpolate new vertex attributes for the newly added vertices in the middle of
    // the new face, and the newly added vertices along the edges of the new face.
    for (size_t index = 0; index < mPointIndexToVertexPtrVector.size(); ++index) {
        VertexPtr vertexPtr = mPointIndexToVertexPtrVector[index];
        if (std::find(oldFaceVertexPtrVector.begin(), oldFaceVertexPtrVector.end(),
                vertexPtr)
            != oldFaceVertexPtrVector.end()) {
            AssignInterpolatedVertexAttributes(vertexPtr,
                oldFaceVertexPtrVector);
        }
    }

    // Unwire the original face from all its neighbors and delete it.
    DeleteFace(mMesh, oldFacePtr);
}

} // namespace mesh
