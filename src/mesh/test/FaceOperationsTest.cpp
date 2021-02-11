// Copyright 2008 Drew Olbrich

#include <cmath>

#include <cppunit/extensions/HelperMacros.h>

#include <mesh/FaceOperations.h>
#include <mesh/EdgeOperations.h>
#include <mesh/StandardAttributes.h>
#include <mesh/IsConsistent.h>
#include <mesh/DeletedElementTracker.h>
#include <meshprim/CreateTriangleMesh.h>

using mesh::Mesh;
using mesh::VertexPtr;
using mesh::EdgePtr;
using mesh::FacePtr;
using cgmath::Vector3f;
using cgmath::Matrix4f;

class FaceOperationsTest : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE(FaceOperationsTest);
    CPPUNIT_TEST(testDeleteFace);
    CPPUNIT_TEST(testFaceIsDegenerateNotDegenerate);
    CPPUNIT_TEST(testFaceIsDegenerateNoVertices);
    CPPUNIT_TEST(testFaceIsDegenerateOneVertex);
    CPPUNIT_TEST(testFaceIsDegenerateTwoVertices);
    CPPUNIT_TEST(testFaceIsDegenerateZeroLengthEdge);
    CPPUNIT_TEST(testFaceIsDegenerateZeroArea);
    CPPUNIT_TEST(testProjectedFaceIsDegenerateNotDegenerate);
    CPPUNIT_TEST(testProjectedFaceIsDegenerateNoVertices);
    CPPUNIT_TEST(testProjectedFaceIsDegenerateOneVertex);
    CPPUNIT_TEST(testProjectedFaceIsDegenerateTwoVertices);
    CPPUNIT_TEST(testProjectedFaceIsDegenerateZeroLengthEdge);
    CPPUNIT_TEST(testProjectedFaceIsDegenerateZeroArea);
    CPPUNIT_TEST(testFaceIsNonplanarSuccess);
    CPPUNIT_TEST(testFaceIsNonplanarFailure);
    CPPUNIT_TEST(testFaceIsSelfIntersectingSuccess);
    CPPUNIT_TEST(testFaceIsSelfIntersectingFailure);
    CPPUNIT_TEST(testGetFaceGeometricNormal);
    CPPUNIT_TEST(testGetFaceArea);
    CPPUNIT_TEST(testGetProjectedFaceArea);
    CPPUNIT_TEST(testGetFaceVertexNormal);
    CPPUNIT_TEST(testGetFaceCentroid);
    CPPUNIT_TEST(testGetFaceAverageVertexPosition);
    CPPUNIT_TEST(testRaySegmentIntersectsFace);
    CPPUNIT_TEST(testRaySegmentIntersectsFaceTriangle);
    CPPUNIT_TEST(testGetEpsilonFromFace);
    CPPUNIT_TEST(testGetFaceBoundingBox);
    CPPUNIT_TEST(testGetTriangularFaceAdjacentVertices);
    CPPUNIT_TEST(testGetTriangularFaceAdjacentEdges);
    CPPUNIT_TEST(testGetTriangularFaceVertexPositions);
    CPPUNIT_TEST(testGetTriangularFaceOtherEdges);
    CPPUNIT_TEST(testGetTriangularFaceOtherEdge);
    CPPUNIT_TEST(testGetTriangularFaceEdgeOpposingVertex);
    CPPUNIT_TEST(testCreateTriangularFaceAndEdgesFromVertices);
    CPPUNIT_TEST(testAllFaceVerticesAreCoincident);
    CPPUNIT_TEST(testCollapseFace);
    CPPUNIT_TEST(testTriangulateQuadrilateralFaceFromVertex);
    CPPUNIT_TEST(testTriangulateQuadrilateralFace13);
    CPPUNIT_TEST(testTriangulateQuadrilateralFace24);
    CPPUNIT_TEST_SUITE_END();

public:
    void setUp() {
    }

    void tearDown() {
    }

    void testDeleteFace() {
        mesh::Mesh mesh;

        float vertexPositionArray[] = {
            0, 0, 0,
            5, 0, 0,
            5, 5, 0,
            0, 5, 0
        };
        int faceVertexIndexArray[] = {
            0, 1, 2,
            0, 2, 3
        };
        std::vector<mesh::VertexPtr> vertexPtrVector;
        std::vector<mesh::EdgePtr> edgePtrVector;
        std::vector<mesh::FacePtr> facePtrVector;
        meshprim::CreateTriangleMesh(&mesh,
            sizeof(vertexPositionArray)/sizeof(float)/3, vertexPositionArray,
            sizeof(faceVertexIndexArray)/sizeof(int)/3, faceVertexIndexArray,
            &vertexPtrVector, &edgePtrVector, &facePtrVector);

        CPPUNIT_ASSERT(mesh::IsConsistent(mesh));

        CPPUNIT_ASSERT(mesh.vertexCount() == 4);
        CPPUNIT_ASSERT(mesh.edgeCount() == 5);
        CPPUNIT_ASSERT(mesh.faceCount() == 2);

        mesh::FacePtr deletedFace = facePtrVector[0];
        mesh::DeleteFace(&mesh, deletedFace);

        CPPUNIT_ASSERT(mesh.vertexCount() == 4);
        CPPUNIT_ASSERT(mesh.edgeCount() == 5);
        CPPUNIT_ASSERT(mesh.faceCount() == 1);

        for (size_t index = 0; index < vertexPtrVector.size(); ++index) {
            CPPUNIT_ASSERT(!vertexPtrVector[index]->hasAdjacentFace(deletedFace));
        }
        for (size_t index = 0; index < edgePtrVector.size(); ++index) {
            CPPUNIT_ASSERT(!edgePtrVector[index]->hasAdjacentFace(deletedFace));
        }
    }

    void testFaceIsDegenerateNotDegenerate() {
        Mesh mesh;

        FacePtr face = mesh.createFace();
        
        VertexPtr v1 = mesh.createVertex();
        VertexPtr v2 = mesh.createVertex();
        VertexPtr v3 = mesh.createVertex();
        
        face->addAdjacentVertex(v1);
        face->addAdjacentVertex(v2);
        face->addAdjacentVertex(v3);
        
        v1->addAdjacentFace(face);
        v2->addAdjacentFace(face);
        v3->addAdjacentFace(face);
        
        v1->setPosition(Vector3f(0, 0, 0));
        v2->setPosition(Vector3f(1, 0, 0));
        v3->setPosition(Vector3f(0, 1, 0));

        CPPUNIT_ASSERT(!FaceIsDegenerate(face, 0.0001));
    }

    void testFaceIsDegenerateNoVertices() {
        Mesh mesh;

        FacePtr face = mesh.createFace();
        
        CPPUNIT_ASSERT(FaceIsDegenerate(face, 0.0001));
    }

    void testFaceIsDegenerateOneVertex() {
        Mesh mesh;

        FacePtr face = mesh.createFace();
        
        VertexPtr v1 = mesh.createVertex();
        
        face->addAdjacentVertex(v1);
        
        v1->addAdjacentFace(face);
        
        v1->setPosition(Vector3f(0, 0, 0));

        CPPUNIT_ASSERT(FaceIsDegenerate(face, 0.0001));
    }

    void testFaceIsDegenerateTwoVertices() {
        Mesh mesh;

        FacePtr face = mesh.createFace();
        
        VertexPtr v1 = mesh.createVertex();
        VertexPtr v2 = mesh.createVertex();
        
        face->addAdjacentVertex(v1);
        face->addAdjacentVertex(v2);
        
        v1->addAdjacentFace(face);
        v2->addAdjacentFace(face);
        
        v1->setPosition(Vector3f(0, 0, 0));
        v2->setPosition(Vector3f(1, 0, 0));

        CPPUNIT_ASSERT(FaceIsDegenerate(face, 0.0001));
    }

    void testFaceIsDegenerateZeroLengthEdge() {
        Mesh mesh;

        FacePtr face = mesh.createFace();
        
        VertexPtr v1 = mesh.createVertex();
        VertexPtr v2 = mesh.createVertex();
        VertexPtr v3 = mesh.createVertex();
        
        face->addAdjacentVertex(v1);
        face->addAdjacentVertex(v2);
        face->addAdjacentVertex(v3);
        
        v1->addAdjacentFace(face);
        v2->addAdjacentFace(face);
        v3->addAdjacentFace(face);
        
        v1->setPosition(Vector3f(0, 0, 0));
        v2->setPosition(Vector3f(1, 0, 0));
        v3->setPosition(Vector3f(0, 0.0001, 0));

        CPPUNIT_ASSERT(FaceIsDegenerate(face, 0.001));
    }

    void testFaceIsDegenerateZeroArea() {
        Mesh mesh;

        FacePtr face = mesh.createFace();
        
        VertexPtr v1 = mesh.createVertex();
        VertexPtr v2 = mesh.createVertex();
        VertexPtr v3 = mesh.createVertex();

        EdgePtr e1 = mesh.createEdge();
        EdgePtr e2 = mesh.createEdge();
        EdgePtr e3 = mesh.createEdge();
        
        face->addAdjacentVertex(v1);
        face->addAdjacentVertex(v2);
        face->addAdjacentVertex(v3);

        face->addAdjacentEdge(e1);
        face->addAdjacentEdge(e2);
        face->addAdjacentEdge(e3);
        
        v1->addAdjacentFace(face);
        v2->addAdjacentFace(face);
        v3->addAdjacentFace(face);

        e1->addAdjacentFace(face);
        e2->addAdjacentFace(face);
        e3->addAdjacentFace(face);

        v1->addAdjacentEdge(e1);
        v1->addAdjacentEdge(e3);
        v2->addAdjacentEdge(e1);
        v2->addAdjacentEdge(e2);
        v3->addAdjacentEdge(e2);
        v3->addAdjacentEdge(e3);

        e1->addAdjacentVertex(v1);
        e1->addAdjacentVertex(v2);
        e2->addAdjacentVertex(v2);
        e2->addAdjacentVertex(v3);
        e3->addAdjacentVertex(v1);
        e3->addAdjacentVertex(v3);

        v1->setPosition(Vector3f(0, 0, 0));
        v2->setPosition(Vector3f(1, 0, 0));
        v3->setPosition(Vector3f(0.5, 0.0001, 0));

        CPPUNIT_ASSERT(FaceIsDegenerate(face, 0.00011));
    }

    void testProjectedFaceIsDegenerateNotDegenerate() {
        Mesh mesh;

        FacePtr face = mesh.createFace();
        
        VertexPtr v1 = mesh.createVertex();
        VertexPtr v2 = mesh.createVertex();
        VertexPtr v3 = mesh.createVertex();
        
        face->addAdjacentVertex(v1);
        face->addAdjacentVertex(v2);
        face->addAdjacentVertex(v3);
        
        v1->addAdjacentFace(face);
        v2->addAdjacentFace(face);
        v3->addAdjacentFace(face);
        
        v1->setPosition(Vector3f(0, 0, 0));
        v2->setPosition(Vector3f(1, 0, 1));
        v3->setPosition(Vector3f(0, 1, 2));

        CPPUNIT_ASSERT(!ProjectedFaceIsDegenerate(face, 0, 1, 0.0001));
    }

    void testProjectedFaceIsDegenerateNoVertices() {
        Mesh mesh;

        FacePtr face = mesh.createFace();
        
        CPPUNIT_ASSERT(ProjectedFaceIsDegenerate(face, 0, 1, 0.0001));
    }

    void testProjectedFaceIsDegenerateOneVertex() {
        Mesh mesh;

        FacePtr face = mesh.createFace();
        
        VertexPtr v1 = mesh.createVertex();
        
        face->addAdjacentVertex(v1);
        
        v1->addAdjacentFace(face);
        
        v1->setPosition(Vector3f(0, 0, 0));

        CPPUNIT_ASSERT(ProjectedFaceIsDegenerate(face, 0, 1, 0.0001));
    }

    void testProjectedFaceIsDegenerateTwoVertices() {
        Mesh mesh;

        FacePtr face = mesh.createFace();
        
        VertexPtr v1 = mesh.createVertex();
        VertexPtr v2 = mesh.createVertex();
        
        face->addAdjacentVertex(v1);
        face->addAdjacentVertex(v2);
        
        v1->addAdjacentFace(face);
        v2->addAdjacentFace(face);
        
        v1->setPosition(Vector3f(0, 0, 0));
        v2->setPosition(Vector3f(1, 0, 1));

        CPPUNIT_ASSERT(ProjectedFaceIsDegenerate(face, 0, 1, 0.0001));
    }

    void testProjectedFaceIsDegenerateZeroLengthEdge() {
        Mesh mesh;

        FacePtr face = mesh.createFace();
        
        VertexPtr v1 = mesh.createVertex();
        VertexPtr v2 = mesh.createVertex();
        VertexPtr v3 = mesh.createVertex();
        
        face->addAdjacentVertex(v1);
        face->addAdjacentVertex(v2);
        face->addAdjacentVertex(v3);
        
        v1->addAdjacentFace(face);
        v2->addAdjacentFace(face);
        v3->addAdjacentFace(face);
        
        v1->setPosition(Vector3f(0, 0, 0));
        v2->setPosition(Vector3f(1, 0, 1));
        v3->setPosition(Vector3f(0, 0.0001, 2));

        CPPUNIT_ASSERT(ProjectedFaceIsDegenerate(face, 0, 1, 0.001));
    }

    void testProjectedFaceIsDegenerateZeroArea() {
        Mesh mesh;

        FacePtr face = mesh.createFace();
        
        VertexPtr v1 = mesh.createVertex();
        VertexPtr v2 = mesh.createVertex();
        VertexPtr v3 = mesh.createVertex();
        
        EdgePtr e1 = mesh.createEdge();
        EdgePtr e2 = mesh.createEdge();
        EdgePtr e3 = mesh.createEdge();

        face->addAdjacentVertex(v1);
        face->addAdjacentVertex(v2);
        face->addAdjacentVertex(v3);
        
        face->addAdjacentEdge(e1);
        face->addAdjacentEdge(e2);
        face->addAdjacentEdge(e3);

        v1->addAdjacentFace(face);
        v2->addAdjacentFace(face);
        v3->addAdjacentFace(face);

        e1->addAdjacentFace(face);
        e2->addAdjacentFace(face);
        e3->addAdjacentFace(face);

        v1->addAdjacentEdge(e1);
        v1->addAdjacentEdge(e3);
        v2->addAdjacentEdge(e1);
        v2->addAdjacentEdge(e2);
        v3->addAdjacentEdge(e2);
        v3->addAdjacentEdge(e3);

        e1->addAdjacentVertex(v1);
        e1->addAdjacentVertex(v2);
        e2->addAdjacentVertex(v2);
        e2->addAdjacentVertex(v3);
        e3->addAdjacentVertex(v1);
        e3->addAdjacentVertex(v3);

        v1->setPosition(Vector3f(0, 0, 0));
        v2->setPosition(Vector3f(1, 0, 1));
        v3->setPosition(Vector3f(0.5, 0.0001, 2));

        CPPUNIT_ASSERT(ProjectedFaceIsDegenerate(face, 0, 1, 0.001));
    }

    void testFaceIsNonplanarSuccess() {
        Mesh mesh;

        FacePtr face = mesh.createFace();
        
        VertexPtr v1 = mesh.createVertex();
        VertexPtr v2 = mesh.createVertex();
        VertexPtr v3 = mesh.createVertex();
        VertexPtr v4 = mesh.createVertex();
        
        face->addAdjacentVertex(v1);
        face->addAdjacentVertex(v2);
        face->addAdjacentVertex(v3);
        face->addAdjacentVertex(v4);
        
        v1->addAdjacentFace(face);
        v2->addAdjacentFace(face);
        v3->addAdjacentFace(face);
        v4->addAdjacentFace(face);
        
        v1->setPosition(Vector3f(0, 0, 0));
        v2->setPosition(Vector3f(1, 0, 0));
        v3->setPosition(Vector3f(1, 1, 0.5));
        v4->setPosition(Vector3f(0, 1, 0));

        CPPUNIT_ASSERT(FaceIsNonplanar(face, 0.0001));
    }

    void testFaceIsNonplanarFailure() {
        Mesh mesh;

        FacePtr face = mesh.createFace();
        
        VertexPtr v1 = mesh.createVertex();
        VertexPtr v2 = mesh.createVertex();
        VertexPtr v3 = mesh.createVertex();
        VertexPtr v4 = mesh.createVertex();
        
        face->addAdjacentVertex(v1);
        face->addAdjacentVertex(v2);
        face->addAdjacentVertex(v3);
        face->addAdjacentVertex(v4);
        
        v1->addAdjacentFace(face);
        v2->addAdjacentFace(face);
        v3->addAdjacentFace(face);
        v4->addAdjacentFace(face);
        
        v1->setPosition(Vector3f(0, 0, 0));
        v2->setPosition(Vector3f(1, 0, 0));
        v3->setPosition(Vector3f(1, 1, 0));
        v4->setPosition(Vector3f(0, 1, 0));

        CPPUNIT_ASSERT(!FaceIsNonplanar(face, 0.0001));
    }

    void testFaceIsSelfIntersectingSuccess() {
        Mesh mesh;

        FacePtr face = mesh.createFace();
        
        VertexPtr v1 = mesh.createVertex();
        VertexPtr v2 = mesh.createVertex();
        VertexPtr v3 = mesh.createVertex();
        VertexPtr v4 = mesh.createVertex();
        
        face->addAdjacentVertex(v1);
        face->addAdjacentVertex(v2);
        face->addAdjacentVertex(v3);
        face->addAdjacentVertex(v4);
        
        v1->addAdjacentFace(face);
        v2->addAdjacentFace(face);
        v3->addAdjacentFace(face);
        v4->addAdjacentFace(face);
        
        v1->setPosition(Vector3f(0, 0, 0));
        v2->setPosition(Vector3f(1, 0, 0));
        v3->setPosition(Vector3f(0, 1, 0));
        v4->setPosition(Vector3f(1, 1, 0));

        CPPUNIT_ASSERT(FaceIsSelfIntersecting(face));
    }

    void testFaceIsSelfIntersectingFailure() {
        Mesh mesh;

        FacePtr face = mesh.createFace();
        
        VertexPtr v1 = mesh.createVertex();
        VertexPtr v2 = mesh.createVertex();
        VertexPtr v3 = mesh.createVertex();
        VertexPtr v4 = mesh.createVertex();
        
        face->addAdjacentVertex(v1);
        face->addAdjacentVertex(v2);
        face->addAdjacentVertex(v3);
        face->addAdjacentVertex(v4);
        
        v1->addAdjacentFace(face);
        v2->addAdjacentFace(face);
        v3->addAdjacentFace(face);
        v4->addAdjacentFace(face);
        
        v1->setPosition(Vector3f(0, 0, 0));
        v2->setPosition(Vector3f(1, 0, 0));
        v3->setPosition(Vector3f(1, 1, 0));
        v4->setPosition(Vector3f(0, 1, 0));

        CPPUNIT_ASSERT(!FaceIsSelfIntersecting(face));
    }

    void testGetFaceGeometricNormal() {
        Mesh mesh;
        
        FacePtr face = mesh.createFace();
        
        VertexPtr v1 = mesh.createVertex();
        VertexPtr v2 = mesh.createVertex();
        VertexPtr v3 = mesh.createVertex();
        
        face->addAdjacentVertex(v1);
        face->addAdjacentVertex(v2);
        face->addAdjacentVertex(v3);
        
        v1->addAdjacentFace(face);
        v2->addAdjacentFace(face);
        v3->addAdjacentFace(face);
        
        v1->setPosition(Vector3f(0, 0, 0));
        v2->setPosition(Vector3f(1, 0, 0));
        v3->setPosition(Vector3f(0, 1, 0));
        
        CPPUNIT_ASSERT(equivalent(GetFaceGeometricNormal(face), Vector3f(0, 0, 1)));
        
        VertexPtr v4 = mesh.createVertex();
        face->addAdjacentVertex(v4);
        v4->addAdjacentFace(face);
        
        v3->setPosition(Vector3f(1, 1, 0));
        v4->setPosition(Vector3f(0, 1, 0));
        
        CPPUNIT_ASSERT(equivalent(GetFaceGeometricNormal(face), Vector3f(0, 0, 1)));
    }

    void testGetFaceArea() {
        Mesh mesh;
        
        FacePtr face = mesh.createFace();
        
        VertexPtr v1 = mesh.createVertex();
        VertexPtr v2 = mesh.createVertex();
        VertexPtr v3 = mesh.createVertex();
        
        face->addAdjacentVertex(v1);
        face->addAdjacentVertex(v2);
        face->addAdjacentVertex(v3);
        
        v1->addAdjacentFace(face);
        v2->addAdjacentFace(face);
        v3->addAdjacentFace(face);
        
        v1->setPosition(Vector3f(0, 0, 0));
        v2->setPosition(Vector3f(3, 0, 0));
        v3->setPosition(Vector3f(0, 4, 0));

        CPPUNIT_ASSERT(GetFaceArea(face) == 6);

        v1->setPosition(Vector3f(0, 0, 0));
        v2->setPosition(Vector3f(0, 3, 0));
        v3->setPosition(Vector3f(0, 0, 4));

        CPPUNIT_ASSERT(GetFaceArea(face) == 6);

        v1->setPosition(Vector3f(0, 0, 0));
        v2->setPosition(Vector3f(0, 4, 0));
        v3->setPosition(Vector3f(3, 0, 0));

        CPPUNIT_ASSERT(GetFaceArea(face) == 6);
    }

    void testGetProjectedFaceArea() {
        Mesh mesh;
        
        FacePtr face = mesh.createFace();
        
        VertexPtr v1 = mesh.createVertex();
        VertexPtr v2 = mesh.createVertex();
        VertexPtr v3 = mesh.createVertex();
        
        face->addAdjacentVertex(v1);
        face->addAdjacentVertex(v2);
        face->addAdjacentVertex(v3);
        
        v1->addAdjacentFace(face);
        v2->addAdjacentFace(face);
        v3->addAdjacentFace(face);
        
        v1->setPosition(Vector3f(0, 0, 1));
        v2->setPosition(Vector3f(3, 0, 2));
        v3->setPosition(Vector3f(0, 4, 3));

        CPPUNIT_ASSERT(GetProjectedFaceArea(face, 0, 1) == 6);

        v1->setPosition(Vector3f(1, 0, 0));
        v2->setPosition(Vector3f(2, 3, 0));
        v3->setPosition(Vector3f(3, 0, 4));

        CPPUNIT_ASSERT(GetProjectedFaceArea(face, 1, 2) == 6);

        v1->setPosition(Vector3f(0, 0, 1));
        v2->setPosition(Vector3f(0, 4, 2));
        v3->setPosition(Vector3f(3, 0, 3));

        CPPUNIT_ASSERT(GetProjectedFaceArea(face, 0, 1) == 6);
    }

    void testGetFaceVertexNormal() {
        Mesh mesh;

        FacePtr face = mesh.createFace();
        
        VertexPtr v1 = mesh.createVertex();
        VertexPtr v2 = mesh.createVertex();
        VertexPtr v3 = mesh.createVertex();
        
        face->addAdjacentVertex(v1);
        face->addAdjacentVertex(v2);
        face->addAdjacentVertex(v3);
        
        v1->addAdjacentFace(face);
        v2->addAdjacentFace(face);
        v3->addAdjacentFace(face);
        
        v1->setPosition(Vector3f(0, 0, 0));
        v2->setPosition(Vector3f(1, 0, 0));
        v3->setPosition(Vector3f(0, 1, 0));

        mesh::AttributeKey normal3fKey = GetNormal3fAttributeKey(mesh);

        CPPUNIT_ASSERT(GetFaceVertexNormal(face, v1, normal3fKey) == Vector3f(0, 0, 1));

        Vector3f faceNormal(-0.1, -0.1, 1);
        faceNormal.normalize();
        face->setUnitVector3f(normal3fKey, faceNormal);
        CPPUNIT_ASSERT(GetFaceVertexNormal(face, v1, normal3fKey) == faceNormal);

        Vector3f faceVertexNormal(-0.5, -0.5, 1);
        faceVertexNormal.normalize();
        face->setVertexUnitVector3f(v1, normal3fKey, faceVertexNormal);
        CPPUNIT_ASSERT(GetFaceVertexNormal(face, v1, normal3fKey) == faceVertexNormal);
    }

    void testGetFaceCentroid() {
        Mesh mesh;

        FacePtr face = mesh.createFace();
        
        VertexPtr v1 = mesh.createVertex();
        VertexPtr v2 = mesh.createVertex();
        VertexPtr v3 = mesh.createVertex();
        VertexPtr v4 = mesh.createVertex();

        v1->setPosition(Vector3f(0, 0, 0));
        v2->setPosition(Vector3f(4, 0, 0));
        v3->setPosition(Vector3f(4, 2, 0));
        v4->setPosition(Vector3f(0, 2, 0));

        face->addAdjacentVertex(v1);
        face->addAdjacentVertex(v2);
        face->addAdjacentVertex(v3);
        face->addAdjacentVertex(v4);
        
        v1->addAdjacentFace(face);
        v2->addAdjacentFace(face);
        v3->addAdjacentFace(face);
        v4->addAdjacentFace(face);

        CPPUNIT_ASSERT(equivalent(GetFaceCentroid(face), Vector3f(2, 1, 0)));

        v1->setPosition(Vector3f(0, 8, 6));
        v2->setPosition(Vector3f(4, 0, 6));
        v3->setPosition(Vector3f(4, 0, 0));
        v4->setPosition(Vector3f(0, 8, 0));

        CPPUNIT_ASSERT(equivalent(GetFaceCentroid(face), Vector3f(2, 4, 3)));

        v1->setPosition(Vector3f(1, -3, 0));
        v2->setPosition(Vector3f(5, -1, 0));
        v3->setPosition(Vector3f(3, 5, 0));
        v4->setPosition(Vector3f(-1, 3, 0));

        CPPUNIT_ASSERT(equivalent(GetFaceCentroid(face), Vector3f(2, 1, 0)));

        v1->setPosition(Vector3f(0, 0, 0));
        v2->setPosition(Vector3f(4, 0, 0));
        v3->setPosition(Vector3f(4, 4, 6));
        v4->setPosition(Vector3f(0, 4, 6));

        CPPUNIT_ASSERT(equivalent(GetFaceCentroid(face), Vector3f(2, 2, 3)));

        // Make a symmetrical polygon, rotate it three times
        // around the origin, and then translate it. The centroid
        // should be equivalent to the translation.
        v1->setPosition(Vector3f(-4, -6, 0));
        v2->setPosition(Vector3f(4, -6, 0));
        v3->setPosition(Vector3f(4, 6, 0));
        v4->setPosition(Vector3f(-4, 6, 0));
        Matrix4f matrix(
            Matrix4f::fromTranslation(Vector3f(1, 2, 3))
            *Matrix4f::fromRotation(Vector3f(0, 0, 1), 3.1415927F/6.0)
            *Matrix4f::fromRotation(Vector3f(0, 1, 0), 3.1415927F/7.0)
            *Matrix4f::fromRotation(Vector3f(1, 0, 0), 3.1415927F/8.0));
        v1->setPosition(matrix*v1->position());
        v2->setPosition(matrix*v2->position());
        v3->setPosition(matrix*v3->position());
        v4->setPosition(matrix*v4->position());

        CPPUNIT_ASSERT(equivalent(GetFaceCentroid(face), Vector3f(1, 2, 3)));
    }

    void testGetFaceAverageVertexPosition() {
        mesh::Mesh mesh;

        float vertexPositionArray[] = {
            0, 0, 0,
            5, 0, 0,
            5, 5, 0
        };
        int faceVertexIndexArray[] = {
            0, 1, 2
        };
        std::vector<mesh::VertexPtr> vertexPtrVector;
        std::vector<mesh::EdgePtr> edgePtrVector;
        std::vector<mesh::FacePtr> facePtrVector;
        meshprim::CreateTriangleMesh(&mesh,
            sizeof(vertexPositionArray)/sizeof(float)/3, vertexPositionArray,
            sizeof(faceVertexIndexArray)/sizeof(int)/3, faceVertexIndexArray,
            &vertexPtrVector, &edgePtrVector, &facePtrVector);

        CPPUNIT_ASSERT(mesh::GetFaceAverageVertexPosition(facePtrVector[0])
            .equivalent(
                (cgmath::Vector3f(0, 0, 0) 
                    + cgmath::Vector3f(5, 0, 0) 
                    + cgmath::Vector3f(5, 5, 0))/3.0, 0.0001));
    }

    void testRaySegmentIntersectsFace() {
        Mesh mesh;

        FacePtr face = mesh.createFace();
        
        VertexPtr v1 = mesh.createVertex();
        VertexPtr v2 = mesh.createVertex();
        VertexPtr v3 = mesh.createVertex();
        VertexPtr v4 = mesh.createVertex();

        v1->setPosition(Vector3f(0, 0, 0));
        v2->setPosition(Vector3f(3, 0, 0));
        v3->setPosition(Vector3f(3, 3, 0));
        v4->setPosition(Vector3f(0, 3, 0));

        face->addAdjacentVertex(v1);
        face->addAdjacentVertex(v2);
        face->addAdjacentVertex(v3);
        face->addAdjacentVertex(v4);
        
        v1->addAdjacentFace(face);
        v2->addAdjacentFace(face);
        v3->addAdjacentFace(face);
        v4->addAdjacentFace(face);

        float t = 500.0;
        CPPUNIT_ASSERT(RaySegmentIntersectsFace(face, 
                Vector3f(1, 1, 1), Vector3f(1, 1, 1) + Vector3f(0, 0, -100), &t));
        CPPUNIT_ASSERT(RaySegmentIntersectsFace(face, 
                Vector3f(1, 1, -1), Vector3f(1, 1, -1) + Vector3f(0, 0, 100), &t));

        CPPUNIT_ASSERT(!RaySegmentIntersectsFace(face, 
                Vector3f(5, 5, 1), Vector3f(5, 5, 1) + Vector3f(0, 0, -100), &t));
        CPPUNIT_ASSERT(!RaySegmentIntersectsFace(face, 
                Vector3f(-1, 5, 1), Vector3f(-1, 5, 1) + Vector3f(0, 0, -100), &t));
        CPPUNIT_ASSERT(!RaySegmentIntersectsFace(face, 
                Vector3f(-1, 1, 1), Vector3f(-1, 1, 1) + Vector3f(0, 0, -100), &t));
        CPPUNIT_ASSERT(RaySegmentIntersectsFace(face, 
                Vector3f(2.5, 2.5, 1), Vector3f(2.5, 2.5, 1) + Vector3f(0, 0, -100), &t));
        CPPUNIT_ASSERT(RaySegmentIntersectsFace(face, 
                Vector3f(0.5, 2.5, 1), Vector3f(0.5, 2.5, 1) + Vector3f(0, 0, -100), &t));
        CPPUNIT_ASSERT(RaySegmentIntersectsFace(face, 
                Vector3f(0.5, 0.5, 1), Vector3f(0.5, 0.5, 1) + Vector3f(0, 0, -100), &t));
        CPPUNIT_ASSERT(RaySegmentIntersectsFace(face, 
                Vector3f(2.5, 0.5, 1), Vector3f(2.5, 0.5, 1) + Vector3f(0, 0, -100), &t));
        CPPUNIT_ASSERT(!RaySegmentIntersectsFace(face, 
                Vector3f(2.5, 2.5, 1), Vector3f(2.5, 2.5, 1) + Vector3f(200, 200, -100), &t));

        CPPUNIT_ASSERT(!RaySegmentIntersectsFace(face, 
                Vector3f(1, 1, 8), Vector3f(1, 1, 8) + Vector3f(0, 0, -1), &t));
        CPPUNIT_ASSERT(!RaySegmentIntersectsFace(face, 
                Vector3f(1, 1, 8), Vector3f(1, 1, 8) + Vector3f(0, 0, -7.5), &t));
        CPPUNIT_ASSERT(RaySegmentIntersectsFace(face, 
                Vector3f(1, 1, 8), Vector3f(1, 1, 8) + Vector3f(0, 0, -8.5), &t));
    }

    void testRaySegmentIntersectsFaceTriangle() {
        Mesh mesh;

        FacePtr face = mesh.createFace();
        
        VertexPtr v1 = mesh.createVertex();
        VertexPtr v2 = mesh.createVertex();
        VertexPtr v3 = mesh.createVertex();

        v1->setPosition(Vector3f(-0.5, 0, -0.5));
        v2->setPosition(Vector3f(-0.5, 1, -0.5));
        v3->setPosition(Vector3f(0.5, 1, -0.5));

        face->addAdjacentVertex(v1);
        face->addAdjacentVertex(v2);
        face->addAdjacentVertex(v3);
        
        v1->addAdjacentFace(face);
        v2->addAdjacentFace(face);
        v3->addAdjacentFace(face);

        float t = 0.0;

        CPPUNIT_ASSERT(RaySegmentIntersectsFace(face, 
                Vector3f(-0.497046, -1.58102e-09, -0.400044),
                Vector3f(-0.497046, -1.58102e-09, -0.400044) 
                + Vector3f(1.23185, 3.55159, -1.14696),
                &t));

        CPPUNIT_ASSERT(RaySegmentIntersectsFace(face, 
                Vector3f(-0.497046, -1.58102e-09, -0.400044)
                + Vector3f(1.23185, 3.55159, -1.14696),
                Vector3f(-0.497046, -1.58102e-09, -0.400044),
                &t));
    }

    void testGetEpsilonFromFace() {
        mesh::Mesh mesh;

        float vertexPositionArray[] = {
            1, 2, 3,
            4, 5, 6,
            7, 8, 9
        };
        int faceVertexIndexArray[] = {
            0, 1, 2
        };
        std::vector<mesh::VertexPtr> vertexPtrVector;
        std::vector<mesh::EdgePtr> edgePtrVector;
        std::vector<mesh::FacePtr> facePtrVector;
        meshprim::CreateTriangleMesh(&mesh,
            sizeof(vertexPositionArray)/sizeof(float)/3, vertexPositionArray,
            sizeof(faceVertexIndexArray)/sizeof(int)/3, faceVertexIndexArray,
            &vertexPtrVector, &edgePtrVector, &facePtrVector);

        CPPUNIT_ASSERT(mesh::GetEpsilonFromFace(facePtrVector[0], 1.0, 1.0) == 9.0);
        CPPUNIT_ASSERT(mesh::GetEpsilonFromFace(facePtrVector[0], 10.0, 1.0) == 10.0);

        vertexPtrVector[2]->setPosition(cgmath::Vector3f(-7, -8, -9));
        CPPUNIT_ASSERT(mesh::GetEpsilonFromFace(facePtrVector[0], 1.0, 1.0) == 9.0);
    }

    void testGetFaceBoundingBox() {
        mesh::Mesh mesh;

        float vertexPositionArray[] = {
            1, 2, 3,
            4, 5, 6,
            7, 8, 9
        };
        int faceVertexIndexArray[] = {
            0, 1, 2
        };
        std::vector<mesh::VertexPtr> vertexPtrVector;
        std::vector<mesh::EdgePtr> edgePtrVector;
        std::vector<mesh::FacePtr> facePtrVector;
        meshprim::CreateTriangleMesh(&mesh,
            sizeof(vertexPositionArray)/sizeof(float)/3, vertexPositionArray,
            sizeof(faceVertexIndexArray)/sizeof(int)/3, faceVertexIndexArray,
            &vertexPtrVector, &edgePtrVector, &facePtrVector);

        CPPUNIT_ASSERT(mesh::GetFaceBoundingBox(facePtrVector[0])
            == cgmath::BoundingBox3f(Vector3f(1, 2, 3), Vector3f(7, 8, 9)));
    }

    void testGetTriangularFaceAdjacentVertices() {
        mesh::Mesh mesh;

        float vertexPositionArray[] = {
            1, 2, 3,
            4, 5, 6,
            7, 8, 9
        };
        int faceVertexIndexArray[] = {
            0, 1, 2
        };
        std::vector<mesh::VertexPtr> vertexPtrVector;
        std::vector<mesh::EdgePtr> edgePtrVector;
        std::vector<mesh::FacePtr> facePtrVector;
        meshprim::CreateTriangleMesh(&mesh,
            sizeof(vertexPositionArray)/sizeof(float)/3, vertexPositionArray,
            sizeof(faceVertexIndexArray)/sizeof(int)/3, faceVertexIndexArray,
            &vertexPtrVector, &edgePtrVector, &facePtrVector);

        mesh::VertexPtr v0;
        mesh::VertexPtr v1;
        mesh::VertexPtr v2;
        mesh::GetTriangularFaceAdjacentVertices(facePtrVector[0], &v0, &v1, &v2);
        CPPUNIT_ASSERT(v0 == vertexPtrVector[0]);
        CPPUNIT_ASSERT(v1 == vertexPtrVector[1]);
        CPPUNIT_ASSERT(v2 == vertexPtrVector[2]);
    }

    void testGetTriangularFaceAdjacentEdges() {
        mesh::Mesh mesh;

        float vertexPositionArray[] = {
            1, 2, 3,
            4, 5, 6,
            7, 8, 9
        };
        int faceVertexIndexArray[] = {
            0, 1, 2
        };
        std::vector<mesh::VertexPtr> vertexPtrVector;
        std::vector<mesh::EdgePtr> edgePtrVector;
        std::vector<mesh::FacePtr> facePtrVector;
        meshprim::CreateTriangleMesh(&mesh,
            sizeof(vertexPositionArray)/sizeof(float)/3, vertexPositionArray,
            sizeof(faceVertexIndexArray)/sizeof(int)/3, faceVertexIndexArray,
            &vertexPtrVector, &edgePtrVector, &facePtrVector);

        mesh::EdgePtr e0;
        mesh::EdgePtr e1;
        mesh::EdgePtr e2;
        mesh::GetTriangularFaceAdjacentEdges(facePtrVector[0], &e0, &e1, &e2);
        CPPUNIT_ASSERT(e0 == edgePtrVector[0]);
        CPPUNIT_ASSERT(e1 == edgePtrVector[1]);
        CPPUNIT_ASSERT(e2 == edgePtrVector[2]);
    }

    void testGetTriangularFaceVertexPositions() {
        Mesh mesh;

        FacePtr face = mesh.createFace();
        
        VertexPtr v1 = mesh.createVertex();
        VertexPtr v2 = mesh.createVertex();
        VertexPtr v3 = mesh.createVertex();

        v1->setPosition(Vector3f(1, 2, 3));
        v2->setPosition(Vector3f(4, 5, 6));
        v3->setPosition(Vector3f(7, 8, 9));

        face->addAdjacentVertex(v1);
        face->addAdjacentVertex(v2);
        face->addAdjacentVertex(v3);

        cgmath::Vector3f p0;
        cgmath::Vector3f p1;
        cgmath::Vector3f p2;
        mesh::GetTriangularFaceVertexPositions(face, &p0, &p1, &p2);
        CPPUNIT_ASSERT(p0 == cgmath::Vector3f(1, 2, 3));
        CPPUNIT_ASSERT(p1 == cgmath::Vector3f(4, 5, 6));
        CPPUNIT_ASSERT(p2 == cgmath::Vector3f(7, 8, 9));
    }

    void testGetTriangularFaceOtherEdges() {
        mesh::Mesh mesh;

        float vertexPositionArray[] = {
            1, 2, 3,
            4, 5, 6,
            7, 8, 9
        };
        int faceVertexIndexArray[] = {
            0, 1, 2
        };
        std::vector<mesh::VertexPtr> vertexPtrVector;
        std::vector<mesh::EdgePtr> edgePtrVector;
        std::vector<mesh::FacePtr> facePtrVector;
        meshprim::CreateTriangleMesh(&mesh,
            sizeof(vertexPositionArray)/sizeof(float)/3, vertexPositionArray,
            sizeof(faceVertexIndexArray)/sizeof(int)/3, faceVertexIndexArray,
            &vertexPtrVector, &edgePtrVector, &facePtrVector);

        CPPUNIT_ASSERT(edgePtrVector.size() == 3);
        mesh::EdgePtr e1;
        mesh::EdgePtr e2;
        mesh::GetTriangularFaceOtherEdges(facePtrVector[0],
            edgePtrVector[0], &e1, &e2);
        CPPUNIT_ASSERT(
            (e1 == edgePtrVector[1] && e2 == edgePtrVector[2])
            || (e1 == edgePtrVector[2] && e2 == edgePtrVector[1]));
    }

    void testGetTriangularFaceOtherEdge() {
        mesh::Mesh mesh;

        float vertexPositionArray[] = {
            1, 2, 3,
            4, 5, 6,
            7, 8, 9
        };
        int faceVertexIndexArray[] = {
            0, 1, 2
        };
        std::vector<mesh::VertexPtr> vertexPtrVector;
        std::vector<mesh::EdgePtr> edgePtrVector;
        std::vector<mesh::FacePtr> facePtrVector;
        meshprim::CreateTriangleMesh(&mesh,
            sizeof(vertexPositionArray)/sizeof(float)/3, vertexPositionArray,
            sizeof(faceVertexIndexArray)/sizeof(int)/3, faceVertexIndexArray,
            &vertexPtrVector, &edgePtrVector, &facePtrVector);

        CPPUNIT_ASSERT(edgePtrVector.size() == 3);
        mesh::EdgePtr e1;
        mesh::GetTriangularFaceOtherEdge(facePtrVector[0],
            edgePtrVector[0], edgePtrVector[1], &e1);
        CPPUNIT_ASSERT(e1 == edgePtrVector[2]);
    }

    void testGetTriangularFaceEdgeOpposingVertex() {
        mesh::Mesh mesh;

        float vertexPositionArray[] = {
            1, 2, 3,
            4, 5, 6,
            7, 8, 9
        };
        int faceVertexIndexArray[] = {
            0, 1, 2
        };
        std::vector<mesh::VertexPtr> vertexPtrVector;
        std::vector<mesh::EdgePtr> edgePtrVector;
        std::vector<mesh::FacePtr> facePtrVector;
        meshprim::CreateTriangleMesh(&mesh,
            sizeof(vertexPositionArray)/sizeof(float)/3, vertexPositionArray,
            sizeof(faceVertexIndexArray)/sizeof(int)/3, faceVertexIndexArray,
            &vertexPtrVector, &edgePtrVector, &facePtrVector);

        mesh::EdgePtr edgePtr;

        mesh::GetTriangularFaceEdgeOpposingVertex(facePtrVector[0], vertexPtrVector[0], &edgePtr);
        CPPUNIT_ASSERT(edgePtr == edgePtrVector[1]);
        mesh::GetTriangularFaceEdgeOpposingVertex(facePtrVector[0], vertexPtrVector[1], &edgePtr);
        CPPUNIT_ASSERT(edgePtr == edgePtrVector[2]);
        mesh::GetTriangularFaceEdgeOpposingVertex(facePtrVector[0], vertexPtrVector[2], &edgePtr);
        CPPUNIT_ASSERT(edgePtr == edgePtrVector[0]);
    }

    void testCreateTriangularFaceAndEdgesFromVertices() {
        Mesh mesh;
        VertexPtr v0 = mesh.createVertex();
        VertexPtr v1 = mesh.createVertex();
        VertexPtr v2 = mesh.createVertex();

        mesh::FacePtr facePtr = mesh::CreateTriangularFaceAndEdgesFromVertices(&mesh, v0, v1, v2);

        CPPUNIT_ASSERT(facePtr->adjacentVertexCount() == 3);
        CPPUNIT_ASSERT(facePtr->adjacentEdgeCount() == 3);

        CPPUNIT_ASSERT(mesh::IsConsistent(mesh));

        CPPUNIT_ASSERT(mesh.vertexCount() == 3);
        CPPUNIT_ASSERT(mesh.edgeCount() == 3);
        CPPUNIT_ASSERT(mesh.faceCount() == 1);
        
        VertexPtr v3 = mesh.createVertex();
        
        facePtr = mesh::CreateTriangularFaceAndEdgesFromVertices(&mesh, v0, v1, v3);

        CPPUNIT_ASSERT(mesh::IsConsistent(mesh));

        CPPUNIT_ASSERT(facePtr->adjacentVertexCount() == 3);
        CPPUNIT_ASSERT(facePtr->adjacentEdgeCount() == 3);

        CPPUNIT_ASSERT(mesh.vertexCount() == 4);
        CPPUNIT_ASSERT(mesh.edgeCount() == 5);
        CPPUNIT_ASSERT(mesh.faceCount() == 2);
    }

    void testAllFaceVerticesAreCoincident() {
        mesh::Mesh mesh;

        float noncoincidentVertexPositionArray[] = {
            1, 2, 3,
            4, 5, 6,
            7, 8, 9
        };
        int faceVertexIndexArray[] = {
            0, 1, 2
        };
        std::vector<mesh::VertexPtr> vertexPtrVector;
        std::vector<mesh::EdgePtr> edgePtrVector;
        std::vector<mesh::FacePtr> facePtrVector;
        meshprim::CreateTriangleMesh(&mesh,
            sizeof(noncoincidentVertexPositionArray)/sizeof(float)/3, 
            noncoincidentVertexPositionArray,
            sizeof(faceVertexIndexArray)/sizeof(int)/3, faceVertexIndexArray,
            &vertexPtrVector, &edgePtrVector, &facePtrVector);

        CPPUNIT_ASSERT(!mesh::AllFaceVerticesAreCoincident(facePtrVector[0], 0.1));

        float coincidentVertexPositionArray[] = {
            1.01, 2, 3,
            1, 2.01, 3,
            1, 2, 3.01
        };
        meshprim::CreateTriangleMesh(&mesh,
            sizeof(coincidentVertexPositionArray)/sizeof(float)/3, 
            coincidentVertexPositionArray,
            sizeof(faceVertexIndexArray)/sizeof(int)/3, faceVertexIndexArray,
            &vertexPtrVector, &edgePtrVector, &facePtrVector);

        CPPUNIT_ASSERT(mesh::AllFaceVerticesAreCoincident(facePtrVector[0], 0.1));
    }

    void testCollapseFace() {
        mesh::Mesh mesh;

        //  4---3---2---5   4---3       5
        //   \  |  /|  /     \  |     /
        //    \ | / | /       \ |   / 
        //     \|/  |/         \| /
        //      0---1           0
        float vertexPositionArray[] = {
            0, 0, 0,
            5, 0, 0,
            5, 5, 0,
             0, 5, 0,
            -5, 5, 0,
            10, 5, 0
        };
        int faceVertexIndexArray[] = {
            0, 1, 2,
            0, 2, 3,
            0, 3, 4,
            1, 5, 2
        };
        std::vector<mesh::VertexPtr> vertexPtrVector;
        std::vector<mesh::EdgePtr> edgePtrVector;
        std::vector<mesh::FacePtr> facePtrVector;
        meshprim::CreateTriangleMesh(&mesh,
            sizeof(vertexPositionArray)/sizeof(float)/3, vertexPositionArray,
            sizeof(faceVertexIndexArray)/sizeof(int)/3, faceVertexIndexArray,
            &vertexPtrVector, &edgePtrVector, &facePtrVector);

        CPPUNIT_ASSERT(mesh::IsConsistent(mesh));

        CPPUNIT_ASSERT(mesh.vertexCount() == 6);
        CPPUNIT_ASSERT(mesh.edgeCount() == 9);
        CPPUNIT_ASSERT(mesh.faceCount() == 4);

        mesh::VertexPtr v1;
        mesh::VertexPtr v2;
        mesh::GetEdgeAdjacentVertices(edgePtrVector[3], &v1, &v2);
        CPPUNIT_ASSERT(v1 == vertexPtrVector[2]);
        CPPUNIT_ASSERT(v2 == vertexPtrVector[3]);

        mesh::DeletedElementTracker deletedElementTracker;
        mesh::VertexPtr remainingVertex = mesh::CollapseFace(&mesh, facePtrVector[0], 
            &deletedElementTracker);

        CPPUNIT_ASSERT(mesh::IsConsistent(mesh));

        CPPUNIT_ASSERT(mesh.vertexCount() == 4);
        CPPUNIT_ASSERT(mesh.edgeCount() == 4);
        CPPUNIT_ASSERT(mesh.faceCount() == 1);

        CPPUNIT_ASSERT(deletedElementTracker.hasVertex(vertexPtrVector[1]));
        CPPUNIT_ASSERT(deletedElementTracker.hasVertex(vertexPtrVector[2]));
        CPPUNIT_ASSERT(deletedElementTracker.hasEdge(edgePtrVector[0]));
        CPPUNIT_ASSERT(deletedElementTracker.hasEdge(edgePtrVector[1]));
        CPPUNIT_ASSERT(deletedElementTracker.hasEdge(edgePtrVector[2]));
        CPPUNIT_ASSERT(deletedElementTracker.hasEdge(edgePtrVector[8]));
        CPPUNIT_ASSERT(deletedElementTracker.hasFace(facePtrVector[0]));
        CPPUNIT_ASSERT(deletedElementTracker.hasFace(facePtrVector[1]));
        CPPUNIT_ASSERT(deletedElementTracker.hasFace(facePtrVector[3]));

        CPPUNIT_ASSERT(remainingVertex == vertexPtrVector[0]);
    }

    void testTriangulateQuadrilateralFaceFromVertex() {
        Mesh mesh;

        FacePtr face = mesh.createFace();
        
        VertexPtr v1 = mesh.createVertex();
        VertexPtr v2 = mesh.createVertex();
        VertexPtr v3 = mesh.createVertex();
        VertexPtr v4 = mesh.createVertex();

        EdgePtr e12 = mesh.createEdge();
        EdgePtr e23 = mesh.createEdge();
        EdgePtr e34 = mesh.createEdge();
        EdgePtr e41 = mesh.createEdge();

        v1->setPosition(Vector3f(0, 0, 0));
        v2->setPosition(Vector3f(5, 0, 0));
        v3->setPosition(Vector3f(5, 5, 0));
        v4->setPosition(Vector3f(0, 5, 0));

        face->addAdjacentVertex(v1);
        face->addAdjacentVertex(v2);
        face->addAdjacentVertex(v3);
        face->addAdjacentVertex(v4);

        face->addAdjacentEdge(e12);
        face->addAdjacentEdge(e23);
        face->addAdjacentEdge(e34);
        face->addAdjacentEdge(e41);

        v1->addAdjacentFace(face);
        v2->addAdjacentFace(face);
        v3->addAdjacentFace(face);
        v4->addAdjacentFace(face);

        v1->addAdjacentEdge(e12);
        v1->addAdjacentEdge(e41);
        v2->addAdjacentEdge(e12);
        v2->addAdjacentEdge(e23);
        v3->addAdjacentEdge(e23);
        v3->addAdjacentEdge(e34);
        v4->addAdjacentEdge(e34);
        v4->addAdjacentEdge(e41);

        e12->addAdjacentFace(face);
        e23->addAdjacentFace(face);
        e34->addAdjacentFace(face);
        e41->addAdjacentFace(face);

        e12->addAdjacentVertex(v1);
        e12->addAdjacentVertex(v2);
        e23->addAdjacentVertex(v2);
        e23->addAdjacentVertex(v3);
        e34->addAdjacentVertex(v3);
        e34->addAdjacentVertex(v4);
        e41->addAdjacentVertex(v4);
        e41->addAdjacentVertex(v1);

        CPPUNIT_ASSERT(mesh::IsConsistent(mesh));

        TriangulateQuadrilateralFaceFromVertex(&mesh, face, v1);

        CPPUNIT_ASSERT(mesh::IsConsistent(mesh));

        CPPUNIT_ASSERT(mesh.vertexCount() == 4);
        CPPUNIT_ASSERT(mesh.edgeCount() == 5);
        CPPUNIT_ASSERT(mesh.faceCount() == 2);

        CPPUNIT_ASSERT(v1->adjacentEdgeCount() == 3);
        CPPUNIT_ASSERT(v2->adjacentEdgeCount() == 2);
        CPPUNIT_ASSERT(v3->adjacentEdgeCount() == 3);
        CPPUNIT_ASSERT(v4->adjacentEdgeCount() == 2);
    }

    void testTriangulateQuadrilateralFace13() {
        Mesh mesh;

        FacePtr face = mesh.createFace();
        
        VertexPtr v1 = mesh.createVertex();
        VertexPtr v2 = mesh.createVertex();
        VertexPtr v3 = mesh.createVertex();
        VertexPtr v4 = mesh.createVertex();

        EdgePtr e12 = mesh.createEdge();
        EdgePtr e23 = mesh.createEdge();
        EdgePtr e34 = mesh.createEdge();
        EdgePtr e41 = mesh.createEdge();

        v1->setPosition(Vector3f(2, 2, 0));
        v2->setPosition(Vector3f(5, 0, 0));
        v3->setPosition(Vector3f(3, 3, 0));
        v4->setPosition(Vector3f(0, 5, 0));

        face->addAdjacentVertex(v1);
        face->addAdjacentVertex(v2);
        face->addAdjacentVertex(v3);
        face->addAdjacentVertex(v4);

        face->addAdjacentEdge(e12);
        face->addAdjacentEdge(e23);
        face->addAdjacentEdge(e34);
        face->addAdjacentEdge(e41);

        v1->addAdjacentFace(face);
        v2->addAdjacentFace(face);
        v3->addAdjacentFace(face);
        v4->addAdjacentFace(face);

        v1->addAdjacentEdge(e12);
        v1->addAdjacentEdge(e41);
        v2->addAdjacentEdge(e12);
        v2->addAdjacentEdge(e23);
        v3->addAdjacentEdge(e23);
        v3->addAdjacentEdge(e34);
        v4->addAdjacentEdge(e34);
        v4->addAdjacentEdge(e41);

        e12->addAdjacentFace(face);
        e23->addAdjacentFace(face);
        e34->addAdjacentFace(face);
        e41->addAdjacentFace(face);

        e12->addAdjacentVertex(v1);
        e12->addAdjacentVertex(v2);
        e23->addAdjacentVertex(v2);
        e23->addAdjacentVertex(v3);
        e34->addAdjacentVertex(v3);
        e34->addAdjacentVertex(v4);
        e41->addAdjacentVertex(v4);
        e41->addAdjacentVertex(v1);

        CPPUNIT_ASSERT(mesh::IsConsistent(mesh));

        TriangulateQuadrilateralFace(&mesh, face);

        CPPUNIT_ASSERT(mesh::IsConsistent(mesh));

        CPPUNIT_ASSERT(mesh.vertexCount() == 4);
        CPPUNIT_ASSERT(mesh.edgeCount() == 5);
        CPPUNIT_ASSERT(mesh.faceCount() == 2);

        CPPUNIT_ASSERT(v1->adjacentEdgeCount() == 3);
        CPPUNIT_ASSERT(v2->adjacentEdgeCount() == 2);
        CPPUNIT_ASSERT(v3->adjacentEdgeCount() == 3);
        CPPUNIT_ASSERT(v4->adjacentEdgeCount() == 2);
    }

    void testTriangulateQuadrilateralFace24() {
        Mesh mesh;

        FacePtr face = mesh.createFace();
        
        VertexPtr v1 = mesh.createVertex();
        VertexPtr v2 = mesh.createVertex();
        VertexPtr v3 = mesh.createVertex();
        VertexPtr v4 = mesh.createVertex();

        EdgePtr e12 = mesh.createEdge();
        EdgePtr e23 = mesh.createEdge();
        EdgePtr e34 = mesh.createEdge();
        EdgePtr e41 = mesh.createEdge();

        v1->setPosition(Vector3f(0, 0, 0));
        v2->setPosition(Vector3f(3, 2, 0));
        v3->setPosition(Vector3f(5, 5, 0));
        v4->setPosition(Vector3f(2, 3, 0));

        face->addAdjacentVertex(v1);
        face->addAdjacentVertex(v2);
        face->addAdjacentVertex(v3);
        face->addAdjacentVertex(v4);

        face->addAdjacentEdge(e12);
        face->addAdjacentEdge(e23);
        face->addAdjacentEdge(e34);
        face->addAdjacentEdge(e41);

        v1->addAdjacentFace(face);
        v2->addAdjacentFace(face);
        v3->addAdjacentFace(face);
        v4->addAdjacentFace(face);

        v1->addAdjacentEdge(e12);
        v1->addAdjacentEdge(e41);
        v2->addAdjacentEdge(e12);
        v2->addAdjacentEdge(e23);
        v3->addAdjacentEdge(e23);
        v3->addAdjacentEdge(e34);
        v4->addAdjacentEdge(e34);
        v4->addAdjacentEdge(e41);

        e12->addAdjacentFace(face);
        e23->addAdjacentFace(face);
        e34->addAdjacentFace(face);
        e41->addAdjacentFace(face);

        e12->addAdjacentVertex(v1);
        e12->addAdjacentVertex(v2);
        e23->addAdjacentVertex(v2);
        e23->addAdjacentVertex(v3);
        e34->addAdjacentVertex(v3);
        e34->addAdjacentVertex(v4);
        e41->addAdjacentVertex(v4);
        e41->addAdjacentVertex(v1);

        CPPUNIT_ASSERT(mesh::IsConsistent(mesh));

        TriangulateQuadrilateralFace(&mesh, face);

        CPPUNIT_ASSERT(mesh::IsConsistent(mesh));

        CPPUNIT_ASSERT(mesh.vertexCount() == 4);
        CPPUNIT_ASSERT(mesh.edgeCount() == 5);
        CPPUNIT_ASSERT(mesh.faceCount() == 2);

        CPPUNIT_ASSERT(v1->adjacentEdgeCount() == 2);
        CPPUNIT_ASSERT(v2->adjacentEdgeCount() == 3);
        CPPUNIT_ASSERT(v3->adjacentEdgeCount() == 2);
        CPPUNIT_ASSERT(v4->adjacentEdgeCount() == 3);
    }

    bool equivalent(const Vector3f &a, const Vector3f &b) {
        const float tolerance = 0.0001;
        return std::abs(a[0] - b[0]) < tolerance
            && std::abs(a[1] - b[1]) < tolerance
            && std::abs(a[2] - b[2]) < tolerance;
    }
};

CPPUNIT_TEST_SUITE_REGISTRATION(FaceOperationsTest);
