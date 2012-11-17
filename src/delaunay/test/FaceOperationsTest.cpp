// Copyright 2008 Retroactive Fiasco.

#include <cppunit/extensions/HelperMacros.h>

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

class FaceOperationsTest : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE(FaceOperationsTest);
    CPPUNIT_TEST(testSplitFace);
    CPPUNIT_TEST(testRotateFace);
    CPPUNIT_TEST(testDeleteFaceAndAllOrphanedAdjacentMeshElements);
    CPPUNIT_TEST(testDeleteFaceAndAllOrphanedNonconstrainedAdjacentEdges);
    CPPUNIT_TEST(testCreateFaceAndAdjacentEdgesThreeVertices);
    CPPUNIT_TEST(testCreateFaceAndAdjacentEdgesFourVertices);
    CPPUNIT_TEST(testCreateFaceAndAdjacentEdgesFiveVertices);
    CPPUNIT_TEST(testGetNeighboringFaceAcrossEdge);
    CPPUNIT_TEST(testGetVertexOppositeEdge);
    CPPUNIT_TEST(testGetEdgeOppositeVertex);
    CPPUNIT_TEST_SUITE_END();

private:
    // See the diagram in retro/src/delaunay/notes/Mesh-Operations.pdf.

    MeshPtr mMeshPtr;
    VertexPtr mV1;
    VertexPtr mV2;
    VertexPtr mV3;
    EdgePtr mE1;
    EdgePtr mE2;
    EdgePtr mE3;
    FacePtr mF1;

    // Not used for all tests.
    VertexPtr mV4;
    EdgePtr mE4;
    EdgePtr mE5;
    FacePtr mF2;

public:
    void setUp() {
        mMeshPtr.reset(new Mesh);

        mV1 = mMeshPtr->createVertex();
        mV2 = mMeshPtr->createVertex();
        mV3 = mMeshPtr->createVertex();

        mE1 = mMeshPtr->createEdge();
        mE2 = mMeshPtr->createEdge();
        mE3 = mMeshPtr->createEdge();

        mF1 = mMeshPtr->createFace();

        mV1->setPosition(Vector2f(0, 0));
        mV2->setPosition(Vector2f(10, 0));
        mV3->setPosition(Vector2f(0, 10));

        mV1->addAdjacentEdge(mE1);
        mV1->addAdjacentEdge(mE3);

        mV2->addAdjacentEdge(mE1);
        mV2->addAdjacentEdge(mE2);

        mV3->addAdjacentEdge(mE2);
        mV3->addAdjacentEdge(mE3);

        mV1->addAdjacentFace(mF1);
        mV2->addAdjacentFace(mF1);
        mV3->addAdjacentFace(mF1);

        mE1->addAdjacentVertex(mV1);
        mE1->addAdjacentVertex(mV2);
        mE1->addAdjacentFace(mF1);

        mE2->addAdjacentVertex(mV2);
        mE2->addAdjacentVertex(mV3);
        mE2->addAdjacentFace(mF1);

        mE3->addAdjacentVertex(mV3);
        mE3->addAdjacentVertex(mV1);
        mE3->addAdjacentFace(mF1);

        mF1->addAdjacentVertex(mV1);
        mF1->addAdjacentVertex(mV2);
        mF1->addAdjacentVertex(mV3);
        mF1->addAdjacentEdge(mE1);
        mF1->addAdjacentEdge(mE2);
        mF1->addAdjacentEdge(mE3);

        CPPUNIT_ASSERT(delaunay::TestMeshConsistency(mMeshPtr).isConsistent());
    }

    void tearDown() {
        mMeshPtr.reset();
    }

    void testSplitFace() {
        Vector2f newVertexPosition(3, 3);
        VertexPtr v4 = SplitFace(mF1, newVertexPosition, mMeshPtr);

        CPPUNIT_ASSERT(delaunay::TestMeshConsistency(mMeshPtr).isConsistent());

        CPPUNIT_ASSERT(v4->position() == newVertexPosition);

        CPPUNIT_ASSERT(mMeshPtr->vertexCount() == 4);
        CPPUNIT_ASSERT(mMeshPtr->edgeCount() == 6);
        CPPUNIT_ASSERT(mMeshPtr->faceCount() == 3);

        FacePtr f1 = mMeshPtr->faceBegin();
        FacePtr f2 = f1;
        ++f2;
        FacePtr f3 = f2;
        ++f3;
        FacePtr dummy = f3;
        ++dummy;
        CPPUNIT_ASSERT(dummy == mMeshPtr->faceEnd());

        CPPUNIT_ASSERT(f1->adjacentVertexCount() == 3);
        CPPUNIT_ASSERT(f1->adjacentEdgeCount() == 3);
        CPPUNIT_ASSERT(f2->adjacentVertexCount() == 3);
        CPPUNIT_ASSERT(f2->adjacentEdgeCount() == 3);
        CPPUNIT_ASSERT(f3->adjacentVertexCount() == 3);
        CPPUNIT_ASSERT(f3->adjacentEdgeCount() == 3);

        CPPUNIT_ASSERT(f1->adjacentVertex(0) == mV1
            || f1->adjacentVertex(1) == mV1
            || f1->adjacentVertex(2) == mV1);
        CPPUNIT_ASSERT(f1->adjacentVertex(0) == mV2
            || f1->adjacentVertex(1) == mV2
            || f1->adjacentVertex(2) == mV2);
        CPPUNIT_ASSERT(f1->adjacentVertex(0) == v4
            || f1->adjacentVertex(1) == v4
            || f1->adjacentVertex(2) == v4);

        CPPUNIT_ASSERT(f2->adjacentVertex(0) == mV2
            || f2->adjacentVertex(1) == mV2
            || f2->adjacentVertex(2) == mV2);
        CPPUNIT_ASSERT(f2->adjacentVertex(0) == mV2
            || f2->adjacentVertex(1) == mV3
            || f2->adjacentVertex(2) == mV3);
        CPPUNIT_ASSERT(f2->adjacentVertex(0) == v4
            || f2->adjacentVertex(1) == v4
            || f2->adjacentVertex(2) == v4);

        CPPUNIT_ASSERT(f3->adjacentVertex(0) == mV3
            || f3->adjacentVertex(1) == mV3
            || f3->adjacentVertex(2) == mV3);
        CPPUNIT_ASSERT(f3->adjacentVertex(0) == mV1
            || f3->adjacentVertex(1) == mV1
            || f3->adjacentVertex(2) == mV1);
        CPPUNIT_ASSERT(f3->adjacentVertex(0) == v4
            || f3->adjacentVertex(1) == v4
            || f3->adjacentVertex(2) == v4);

        CPPUNIT_ASSERT(mV1->adjacentEdgeCount() == 3);
        CPPUNIT_ASSERT(mV2->adjacentEdgeCount() == 3);
        CPPUNIT_ASSERT(mV3->adjacentEdgeCount() == 3);
        CPPUNIT_ASSERT(v4->adjacentEdgeCount() == 3);

        CPPUNIT_ASSERT(mV1->adjacentFaceCount() == 2);
        CPPUNIT_ASSERT(mV2->adjacentFaceCount() == 2);
        CPPUNIT_ASSERT(mV3->adjacentFaceCount() == 2);
        CPPUNIT_ASSERT(v4->adjacentFaceCount() == 3);
    }

    void testRotateFace() {
        CPPUNIT_ASSERT(mF1->adjacentVertexCount() == 3);
        CPPUNIT_ASSERT(mF1->adjacentEdgeCount() == 3);

        VertexPtr v1 = mF1->adjacentVertex(0);
        VertexPtr v2 = mF1->adjacentVertex(1);
        VertexPtr v3 = mF1->adjacentVertex(2);

        EdgePtr e1 = mF1->adjacentEdge(0);
        EdgePtr e2 = mF1->adjacentEdge(1);
        EdgePtr e3 = mF1->adjacentEdge(2);

        delaunay::RotateFace(mF1);

        CPPUNIT_ASSERT(delaunay::TestMeshConsistency(mMeshPtr).isConsistent());
        
        CPPUNIT_ASSERT(mF1->adjacentVertexCount() == 3);
        CPPUNIT_ASSERT(mF1->adjacentEdgeCount() == 3);

        CPPUNIT_ASSERT(mF1->adjacentVertex(0) == v3);
        CPPUNIT_ASSERT(mF1->adjacentVertex(1) == v1);
        CPPUNIT_ASSERT(mF1->adjacentVertex(2) == v2);

        CPPUNIT_ASSERT(mF1->adjacentEdge(0) == e3);
        CPPUNIT_ASSERT(mF1->adjacentEdge(1) == e1);
        CPPUNIT_ASSERT(mF1->adjacentEdge(2) == e2);
    }

    void testDeleteFaceAndAllOrphanedAdjacentMeshElements() {
        CPPUNIT_ASSERT(mMeshPtr->vertexCount() == 3);
        CPPUNIT_ASSERT(mMeshPtr->edgeCount() == 3);
        CPPUNIT_ASSERT(mMeshPtr->faceCount() == 1);

        delaunay::DeleteFaceAndAllOrphanedAdjacentMeshElements(mF1, mMeshPtr);

        CPPUNIT_ASSERT(mMeshPtr->vertexCount() == 0);
        CPPUNIT_ASSERT(mMeshPtr->edgeCount() == 0);
        CPPUNIT_ASSERT(mMeshPtr->faceCount() == 0);

        CPPUNIT_ASSERT(delaunay::TestMeshConsistency(mMeshPtr).isConsistent());
    }

    void testDeleteFaceAndAllOrphanedNonconstrainedAdjacentEdges() {
        CPPUNIT_ASSERT(mMeshPtr->vertexCount() == 3);
        CPPUNIT_ASSERT(mMeshPtr->edgeCount() == 3);
        CPPUNIT_ASSERT(mMeshPtr->faceCount() == 1);

        mE1->setInputEdgeIndex(0);

        delaunay::DeleteFaceAndAllOrphanedNonconstrainedAdjacentEdges(mF1, mMeshPtr);

        CPPUNIT_ASSERT(mMeshPtr->vertexCount() == 3);
        CPPUNIT_ASSERT(mMeshPtr->edgeCount() == 1);
        CPPUNIT_ASSERT(mMeshPtr->faceCount() == 0);

        // We've left vertices behind, so the mesh is not consistent.
        CPPUNIT_ASSERT(!delaunay::TestMeshConsistency(mMeshPtr).isConsistent());
    }

    void testCreateFaceAndAdjacentEdgesThreeVertices() {
        mMeshPtr.reset(new Mesh);

        mV1 = mMeshPtr->createVertex();
        mV2 = mMeshPtr->createVertex();
        mV3 = mMeshPtr->createVertex();

        mV1->setPosition(Vector2f(0, 0));
        mV2->setPosition(Vector2f(10, 0));
        mV3->setPosition(Vector2f(0, 10));

        mF1 = CreateFaceAndAdjacentEdges(mV1, mV2, mV3, mMeshPtr);

        CPPUNIT_ASSERT(delaunay::TestMeshConsistency(mMeshPtr).isConsistent());

        CPPUNIT_ASSERT(mMeshPtr->faceCount() == 1);
        CPPUNIT_ASSERT(mMeshPtr->edgeCount() == 3);
        CPPUNIT_ASSERT(mMeshPtr->vertexCount() == 3);
    }

    void testCreateFaceAndAdjacentEdgesFourVertices() {
        VertexPtr v4 = mMeshPtr->createVertex();
        v4->setPosition(Vector2f(10, 10));

        FacePtr f2 = CreateFaceAndAdjacentEdges(mV2, v4, mV3, mMeshPtr);

        CPPUNIT_ASSERT(delaunay::TestMeshConsistency(mMeshPtr).isConsistent());

        // The resulting two triangles share an edge.

        CPPUNIT_ASSERT(mMeshPtr->faceCount() == 2);
        CPPUNIT_ASSERT(mMeshPtr->edgeCount() == 5);
        CPPUNIT_ASSERT(mMeshPtr->vertexCount() == 4);
    }

    void testCreateFaceAndAdjacentEdgesFiveVertices() {
        VertexPtr v4 = mMeshPtr->createVertex();
        v4->setPosition(Vector2f(20, 0));

        VertexPtr v5 = mMeshPtr->createVertex();
        v5->setPosition(Vector2f(10, 10));

        FacePtr f2 = CreateFaceAndAdjacentEdges(mV2, v4, v5, mMeshPtr);
        
        // The resulting two triangles share a vertex but no edges.

        CPPUNIT_ASSERT(delaunay::TestMeshConsistency(mMeshPtr).isConsistent());

        CPPUNIT_ASSERT(mMeshPtr->faceCount() == 2);
        CPPUNIT_ASSERT(mMeshPtr->edgeCount() == 6);
        CPPUNIT_ASSERT(mMeshPtr->vertexCount() == 5);
    }

    void testGetNeighboringFaceAcrossEdge() {
        mMeshPtr.reset(new Mesh);

        mV4 = mMeshPtr->createVertex();
        mV2 = mMeshPtr->createVertex();
        mV1 = mMeshPtr->createVertex();
        mV3 = mMeshPtr->createVertex();

        mE1 = mMeshPtr->createEdge();
        mE2 = mMeshPtr->createEdge();
        mE3 = mMeshPtr->createEdge();
        mE4 = mMeshPtr->createEdge();
        mE5 = mMeshPtr->createEdge();

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

        CPPUNIT_ASSERT(delaunay::GetNeighboringFaceAcrossEdge(mF1, mE1) == mF2);
    }

    void testGetVertexOppositeEdge() {
        CPPUNIT_ASSERT(delaunay::GetVertexOppositeEdge(mF1, mE3) == mV2);
        CPPUNIT_ASSERT(delaunay::GetVertexOppositeEdge(mF1, mE2) == mV1);
        CPPUNIT_ASSERT(delaunay::GetVertexOppositeEdge(mF1, mE1) == mV3);
    }

    void testGetEdgeOppositeVertex() {
        CPPUNIT_ASSERT(delaunay::GetEdgeOppositeVertex(mF1, mV2) == mE3);
        CPPUNIT_ASSERT(delaunay::GetEdgeOppositeVertex(mF1, mV1) == mE2);
        CPPUNIT_ASSERT(delaunay::GetEdgeOppositeVertex(mF1, mV3) == mE1);
    }
};

CPPUNIT_TEST_SUITE_REGISTRATION(FaceOperationsTest);
