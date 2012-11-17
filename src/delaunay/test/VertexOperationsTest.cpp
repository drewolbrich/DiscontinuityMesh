// Copyright 2008 Retroactive Fiasco.

#include <cppunit/extensions/HelperMacros.h>

#include <delaunay/VertexOperations.h>

#include <delaunay/EdgeOperations.h>
#include <delaunay/FaceOperations.h>
#include <delaunay/MeshConsistency.h>
#include <delaunay/Mesh.h>
#include <delaunay/Vertex.h>
#include <delaunay/Edge.h>
#include <delaunay/Face.h>

using delaunay::Mesh;
using delaunay::MeshPtr;
using delaunay::Vertex;
using delaunay::Edge;
using delaunay::Face;
using delaunay::VertexPtr;
using delaunay::EdgePtr;
using delaunay::FacePtr;
using cgmath::Vector2f;

class VertexOperationsTest : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE(VertexOperationsTest);
    CPPUNIT_TEST(testDeleteVertexAndAllOrphanedAdjacentMeshElementsV4);
    CPPUNIT_TEST(testDeleteVertexAndAllOrphanedAdjacentMeshElementsV1);
    CPPUNIT_TEST(testGetAdjacentVertexAroundPerimeter);
    CPPUNIT_TEST(testVertexHasVertexAsNeighbor);
    CPPUNIT_TEST(testGetEdgeLeadingToNeighboringVertex);
    CPPUNIT_TEST(testGetNeighboringVertexAlongEdge);
    CPPUNIT_TEST_SUITE_END();

private:
    // See the diagram in retro/src/delaunay/notes/Mesh-Operations.pdf.

    MeshPtr mMeshPtr;
    VertexPtr mV1;
    VertexPtr mV2;
    VertexPtr mV3;
    VertexPtr mV4;
    EdgePtr mE1;
    EdgePtr mE2;
    EdgePtr mE3;
    EdgePtr mE4;
    EdgePtr mE5;
    FacePtr mF1;
    FacePtr mF2;

public:
    void setUp() {
        mMeshPtr.reset(new Mesh);

        mV4 = mMeshPtr->createVertex();
        mV2 = mMeshPtr->createVertex();
        mV1 = mMeshPtr->createVertex();
        mV3 = mMeshPtr->createVertex();

        mE5 = mMeshPtr->createEdge();
        mE1 = mMeshPtr->createEdge();
        mE4 = mMeshPtr->createEdge();
        mE2 = mMeshPtr->createEdge();
        mE3 = mMeshPtr->createEdge();

        mF2 = mMeshPtr->createFace();
        mF1 = mMeshPtr->createFace();

        mV4->setPosition(Vector2f(0, 0));
        mV2->setPosition(Vector2f(10, 0));
        mV1->setPosition(Vector2f(0, 10));
        mV3->setPosition(Vector2f(10, 10));

        mV1->addAdjacentEdge(mE3);
        mV1->addAdjacentEdge(mE4);
        mV1->addAdjacentEdge(mE1);
        mV1->addAdjacentFace(mF1);
        mV1->addAdjacentFace(mF2);

        mV2->addAdjacentEdge(mE2);
        mV2->addAdjacentEdge(mE5);
        mV2->addAdjacentEdge(mE1);
        mV2->addAdjacentFace(mF1);
        mV2->addAdjacentFace(mF2);

        mV3->addAdjacentEdge(mE2);
        mV3->addAdjacentEdge(mE3);
        mV3->addAdjacentFace(mF1);

        mV4->addAdjacentEdge(mE4);
        mV4->addAdjacentEdge(mE5);
        mV4->addAdjacentFace(mF2);

        mE5->addAdjacentVertex(mV4);
        mE5->addAdjacentVertex(mV2);
        mE5->addAdjacentFace(mF2);

        mE1->addAdjacentVertex(mV2);
        mE1->addAdjacentVertex(mV1);
        mE1->addAdjacentFace(mF2);
        mE1->addAdjacentFace(mF1);

        mE4->addAdjacentVertex(mV4);
        mE4->addAdjacentVertex(mV1);
        mE4->addAdjacentFace(mF2);

        mE2->addAdjacentVertex(mV2);
        mE2->addAdjacentVertex(mV3);
        mE2->addAdjacentFace(mF1);

        mE3->addAdjacentVertex(mV3);
        mE3->addAdjacentVertex(mV1);
        mE3->addAdjacentFace(mF1);

        mF2->addAdjacentVertex(mV4);
        mF2->addAdjacentVertex(mV2);
        mF2->addAdjacentVertex(mV1);
        mF2->addAdjacentEdge(mE5);
        mF2->addAdjacentEdge(mE1);
        mF2->addAdjacentEdge(mE4);

        mF1->addAdjacentVertex(mV2);
        mF1->addAdjacentVertex(mV3);
        mF1->addAdjacentVertex(mV1);
        mF1->addAdjacentEdge(mE2);
        mF1->addAdjacentEdge(mE3);
        mF1->addAdjacentEdge(mE1);

        CPPUNIT_ASSERT(delaunay::TestMeshConsistency(mMeshPtr).isConsistent());

        CPPUNIT_ASSERT(mE1->adjacentVertexCount() == 2);
        CPPUNIT_ASSERT(mE1->adjacentVertex(0) == mV2
            || mE1->adjacentVertex(0) == mV1);
        CPPUNIT_ASSERT(mE1->adjacentVertex(1) == mV2
            || mE1->adjacentVertex(1) == mV1);

        CPPUNIT_ASSERT(mE1->adjacentFaceCount() == 2);
        CPPUNIT_ASSERT(mE1->adjacentFace(0) == mF2
            || mE1->adjacentFace(0) == mF1);
        CPPUNIT_ASSERT(mE1->adjacentFace(1) == mF2
            || mE1->adjacentFace(1) == mF1);

        CPPUNIT_ASSERT(mF2->adjacentEdgeCount() == 3);
        CPPUNIT_ASSERT(mF2->adjacentEdge(0) == mE1
            || mF2->adjacentEdge(1) == mE1
            || mF2->adjacentEdge(2) == mE1);

        CPPUNIT_ASSERT(mF2->adjacentVertexCount() == 3);
        CPPUNIT_ASSERT(mF2->adjacentVertex(0) == mV4
            || mF2->adjacentVertex(0) == mV2
            || mF2->adjacentVertex(0) == mV1);
        CPPUNIT_ASSERT(mF2->adjacentVertex(1) == mV4
            || mF2->adjacentVertex(1) == mV2
            || mF2->adjacentVertex(1) == mV1);
        CPPUNIT_ASSERT(mF2->adjacentVertex(2) == mV4
            || mF2->adjacentVertex(2) == mV2
            || mF2->adjacentVertex(2) == mV1);

        CPPUNIT_ASSERT(mF1->adjacentEdgeCount() == 3);
        CPPUNIT_ASSERT(mF1->adjacentEdge(0) == mE1
            || mF1->adjacentEdge(1) == mE1
            || mF1->adjacentEdge(2) == mE1);

        CPPUNIT_ASSERT(mF1->adjacentVertexCount() == 3);
        CPPUNIT_ASSERT(mF1->adjacentVertex(0) == mV2
            || mF1->adjacentVertex(0) == mV3
            || mF1->adjacentVertex(0) == mV1);
        CPPUNIT_ASSERT(mF1->adjacentVertex(1) == mV2
            || mF1->adjacentVertex(1) == mV3
            || mF1->adjacentVertex(1) == mV1);
        CPPUNIT_ASSERT(mF1->adjacentVertex(2) == mV2
            || mF1->adjacentVertex(2) == mV3
            || mF1->adjacentVertex(2) == mV1);
    }

    void tearDown() {
        mMeshPtr.reset();
    }

    void testDeleteVertexAndAllOrphanedAdjacentMeshElementsV4() {
        CPPUNIT_ASSERT(delaunay::TestMeshConsistency(mMeshPtr).isConsistent());

        CPPUNIT_ASSERT(mMeshPtr->vertexCount() == 4);
        CPPUNIT_ASSERT(mMeshPtr->edgeCount() == 5);
        CPPUNIT_ASSERT(mMeshPtr->faceCount() == 2);

        delaunay::DeleteVertexAndAllOrphanedAdjacentMeshElements(mV4, mMeshPtr);

        CPPUNIT_ASSERT(mMeshPtr->vertexCount() == 3);
        CPPUNIT_ASSERT(mMeshPtr->edgeCount() == 3);
        CPPUNIT_ASSERT(mMeshPtr->faceCount() == 1);

        CPPUNIT_ASSERT(delaunay::TestMeshConsistency(mMeshPtr).isConsistent());
    }

    void testDeleteVertexAndAllOrphanedAdjacentMeshElementsV1() {
        CPPUNIT_ASSERT(delaunay::TestMeshConsistency(mMeshPtr).isConsistent());

        CPPUNIT_ASSERT(mMeshPtr->vertexCount() == 4);
        CPPUNIT_ASSERT(mMeshPtr->edgeCount() == 5);
        CPPUNIT_ASSERT(mMeshPtr->faceCount() == 2);

        delaunay::DeleteVertexAndAllOrphanedAdjacentMeshElements(mV1, mMeshPtr);

        CPPUNIT_ASSERT(mMeshPtr->vertexCount() == 0);
        CPPUNIT_ASSERT(mMeshPtr->edgeCount() == 0);
        CPPUNIT_ASSERT(mMeshPtr->faceCount() == 0);

        CPPUNIT_ASSERT(delaunay::TestMeshConsistency(mMeshPtr).isConsistent());
    }

    void testGetAdjacentVertexAroundPerimeter() {
        CPPUNIT_ASSERT(delaunay::GetAdjacentVertexAroundPerimeter(mV1, 
                delaunay::COUNTERCLOCKWISE) == mV4);
        CPPUNIT_ASSERT(delaunay::GetAdjacentVertexAroundPerimeter(mV4, 
                delaunay::COUNTERCLOCKWISE) == mV2);
        CPPUNIT_ASSERT(delaunay::GetAdjacentVertexAroundPerimeter(mV2, 
                delaunay::COUNTERCLOCKWISE) == mV3);
        CPPUNIT_ASSERT(delaunay::GetAdjacentVertexAroundPerimeter(mV3, 
                delaunay::COUNTERCLOCKWISE) == mV1);

        CPPUNIT_ASSERT(delaunay::GetAdjacentVertexAroundPerimeter(mV1, 
                delaunay::CLOCKWISE) == mV3);
        CPPUNIT_ASSERT(delaunay::GetAdjacentVertexAroundPerimeter(mV4, 
                delaunay::CLOCKWISE) == mV1);
        CPPUNIT_ASSERT(delaunay::GetAdjacentVertexAroundPerimeter(mV2, 
                delaunay::CLOCKWISE) == mV4);
        CPPUNIT_ASSERT(delaunay::GetAdjacentVertexAroundPerimeter(mV3, 
                delaunay::CLOCKWISE) == mV2);
    }

    void testVertexHasVertexAsNeighbor() {
        CPPUNIT_ASSERT(delaunay::VertexHasVertexAsNeighbor(mV1, mV2));
        CPPUNIT_ASSERT(delaunay::VertexHasVertexAsNeighbor(mV1, mV3));
        CPPUNIT_ASSERT(delaunay::VertexHasVertexAsNeighbor(mV1, mV4));

        CPPUNIT_ASSERT(delaunay::VertexHasVertexAsNeighbor(mV2, mV1));
        CPPUNIT_ASSERT(delaunay::VertexHasVertexAsNeighbor(mV2, mV3));
        CPPUNIT_ASSERT(delaunay::VertexHasVertexAsNeighbor(mV2, mV4));

        CPPUNIT_ASSERT(delaunay::VertexHasVertexAsNeighbor(mV3, mV1));
        CPPUNIT_ASSERT(delaunay::VertexHasVertexAsNeighbor(mV3, mV2));
        CPPUNIT_ASSERT(!delaunay::VertexHasVertexAsNeighbor(mV3, mV4));

        CPPUNIT_ASSERT(delaunay::VertexHasVertexAsNeighbor(mV4, mV1));
        CPPUNIT_ASSERT(delaunay::VertexHasVertexAsNeighbor(mV4, mV2));
        CPPUNIT_ASSERT(!delaunay::VertexHasVertexAsNeighbor(mV4, mV3));
    }

    void testGetEdgeLeadingToNeighboringVertex() {
        CPPUNIT_ASSERT(delaunay::GetEdgeLeadingToNeighboringVertex(mV1, mV2, mMeshPtr) == mE1);
        CPPUNIT_ASSERT(delaunay::GetEdgeLeadingToNeighboringVertex(mV1, mV3, mMeshPtr) == mE3);
        CPPUNIT_ASSERT(delaunay::GetEdgeLeadingToNeighboringVertex(mV1, mV4, mMeshPtr) == mE4);

        CPPUNIT_ASSERT(delaunay::GetEdgeLeadingToNeighboringVertex(mV2, mV1, mMeshPtr) == mE1);
        CPPUNIT_ASSERT(delaunay::GetEdgeLeadingToNeighboringVertex(mV2, mV3, mMeshPtr) == mE2);
        CPPUNIT_ASSERT(delaunay::GetEdgeLeadingToNeighboringVertex(mV2, mV4, mMeshPtr) == mE5);

        CPPUNIT_ASSERT(delaunay::GetEdgeLeadingToNeighboringVertex(mV3, mV1, mMeshPtr) == mE3);
        CPPUNIT_ASSERT(delaunay::GetEdgeLeadingToNeighboringVertex(mV3, mV2, mMeshPtr) == mE2);
        CPPUNIT_ASSERT(delaunay::GetEdgeLeadingToNeighboringVertex(mV3, mV4, mMeshPtr) 
            == mMeshPtr->edgeEnd());

        CPPUNIT_ASSERT(delaunay::GetEdgeLeadingToNeighboringVertex(mV4, mV1, mMeshPtr) == mE4);
        CPPUNIT_ASSERT(delaunay::GetEdgeLeadingToNeighboringVertex(mV4, mV2, mMeshPtr) == mE5);
        CPPUNIT_ASSERT(delaunay::GetEdgeLeadingToNeighboringVertex(mV4, mV3, mMeshPtr)
            == mMeshPtr->edgeEnd());
    }

    void testGetNeighboringVertexAlongEdge() {
        CPPUNIT_ASSERT(delaunay::GetNeighboringVertexAlongEdge(mV1, mE3) == mV3);
        CPPUNIT_ASSERT(delaunay::GetNeighboringVertexAlongEdge(mV1, mE1) == mV2);
        CPPUNIT_ASSERT(delaunay::GetNeighboringVertexAlongEdge(mV1, mE4) == mV4);
    }
};

CPPUNIT_TEST_SUITE_REGISTRATION(VertexOperationsTest);
