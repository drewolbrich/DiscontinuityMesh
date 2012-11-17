// Copyright 2009 Retroactive Fiasco.

#include <cppunit/extensions/HelperMacros.h>

#include <meshprim/TriangleMeshCreator.h>
#include <cgmath/Vector3f.h>
#include <mesh/Mesh.h>
#include <mesh/IsConsistent.h>
#include <mesh/AttributePossessor.h>
#include <mesh/StandardAttributes.h>

using meshprim::TriangleMeshCreator;

class TriangleMeshCreatorTest : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE(TriangleMeshCreatorTest);
    CPPUNIT_TEST(testCreate);
    CPPUNIT_TEST_SUITE_END();

public:
    void setUp() {
    }

    void tearDown() {
    }

    void testCreate() {
        mesh::Mesh mesh;

        mesh::AttributePossessor faceAttributes;
        faceAttributes.setVector3f(mesh::GetColor3fAttributeKey(mesh),
            cgmath::Vector3f(1.0, 0.0, 0.0));

        TriangleMeshCreator::VertexPositionVector vertexPositionVector;
        vertexPositionVector.push_back(cgmath::Vector3f(0.0, 0.0, 0.0));
        vertexPositionVector.push_back(cgmath::Vector3f(5.0, 0.0, 0.0));
        vertexPositionVector.push_back(cgmath::Vector3f(5.0, 5.0, 0.0));
        vertexPositionVector.push_back(cgmath::Vector3f(0.0, 5.0, 0.0));

        TriangleMeshCreator::TriangleVector triangleVector;
        triangleVector.push_back(TriangleMeshCreator::Triangle(0, 1, 2));
        triangleVector.push_back(TriangleMeshCreator::Triangle(0, 2, 3));

        TriangleMeshCreator triangleMeshCreator;
        triangleMeshCreator.setMesh(&mesh);
        triangleMeshCreator.setTransformationMatrix(
            cgmath::Matrix4f::fromTranslation(cgmath::Vector3f(10.0, 11.0, 12.0)));
        triangleMeshCreator.setVertexPositionVector(&vertexPositionVector);
        triangleMeshCreator.setTriangleVector(&triangleVector);
        triangleMeshCreator.setFaceAttributes(faceAttributes);
        triangleMeshCreator.create();

        const TriangleMeshCreator::VertexPtrVector &vertexPtrVector
            = triangleMeshCreator.vertexPtrVector();
        const TriangleMeshCreator::EdgePtrVector &edgePtrVector
            = triangleMeshCreator.edgePtrVector();
        const TriangleMeshCreator::FacePtrVector &facePtrVector
            = triangleMeshCreator.facePtrVector();

        CPPUNIT_ASSERT(vertexPtrVector.size() == 4);
        CPPUNIT_ASSERT(edgePtrVector.size() == 5);
        CPPUNIT_ASSERT(facePtrVector.size() == 2);

        CPPUNIT_ASSERT(mesh::IsConsistent(mesh));

        CPPUNIT_ASSERT(facePtrVector[0]->hasAdjacentVertex(vertexPtrVector[0]));
        CPPUNIT_ASSERT(facePtrVector[0]->hasAdjacentVertex(vertexPtrVector[1]));
        CPPUNIT_ASSERT(facePtrVector[0]->hasAdjacentVertex(vertexPtrVector[2]));

        CPPUNIT_ASSERT(facePtrVector[1]->hasAdjacentVertex(vertexPtrVector[0]));
        CPPUNIT_ASSERT(facePtrVector[1]->hasAdjacentVertex(vertexPtrVector[2]));
        CPPUNIT_ASSERT(facePtrVector[1]->hasAdjacentVertex(vertexPtrVector[3]));

        CPPUNIT_ASSERT(vertexPtrVector[0]->hasAdjacentFace(facePtrVector[0]));
        CPPUNIT_ASSERT(vertexPtrVector[1]->hasAdjacentFace(facePtrVector[0]));
        CPPUNIT_ASSERT(vertexPtrVector[2]->hasAdjacentFace(facePtrVector[0]));

        CPPUNIT_ASSERT(vertexPtrVector[0]->hasAdjacentFace(facePtrVector[1]));
        CPPUNIT_ASSERT(vertexPtrVector[2]->hasAdjacentFace(facePtrVector[1]));
        CPPUNIT_ASSERT(vertexPtrVector[3]->hasAdjacentFace(facePtrVector[1]));

        CPPUNIT_ASSERT(edgePtrVector[0]->hasAdjacentVertex(vertexPtrVector[0]));
        CPPUNIT_ASSERT(edgePtrVector[0]->hasAdjacentVertex(vertexPtrVector[1]));

        CPPUNIT_ASSERT(edgePtrVector[1]->hasAdjacentVertex(vertexPtrVector[1]));
        CPPUNIT_ASSERT(edgePtrVector[1]->hasAdjacentVertex(vertexPtrVector[2]));

        CPPUNIT_ASSERT(edgePtrVector[2]->hasAdjacentVertex(vertexPtrVector[0]));
        CPPUNIT_ASSERT(edgePtrVector[2]->hasAdjacentVertex(vertexPtrVector[2]));

        CPPUNIT_ASSERT(edgePtrVector[3]->hasAdjacentVertex(vertexPtrVector[2]));
        CPPUNIT_ASSERT(edgePtrVector[3]->hasAdjacentVertex(vertexPtrVector[3]));

        CPPUNIT_ASSERT(edgePtrVector[4]->hasAdjacentVertex(vertexPtrVector[0]));
        CPPUNIT_ASSERT(edgePtrVector[4]->hasAdjacentVertex(vertexPtrVector[3]));

        CPPUNIT_ASSERT(vertexPtrVector[0]->hasAdjacentEdge(edgePtrVector[0]));
        CPPUNIT_ASSERT(vertexPtrVector[1]->hasAdjacentEdge(edgePtrVector[0]));

        CPPUNIT_ASSERT(vertexPtrVector[1]->hasAdjacentEdge(edgePtrVector[1]));
        CPPUNIT_ASSERT(vertexPtrVector[2]->hasAdjacentEdge(edgePtrVector[1]));

        CPPUNIT_ASSERT(vertexPtrVector[0]->hasAdjacentEdge(edgePtrVector[2]));
        CPPUNIT_ASSERT(vertexPtrVector[2]->hasAdjacentEdge(edgePtrVector[2]));

        CPPUNIT_ASSERT(vertexPtrVector[2]->hasAdjacentEdge(edgePtrVector[3]));
        CPPUNIT_ASSERT(vertexPtrVector[3]->hasAdjacentEdge(edgePtrVector[3]));

        CPPUNIT_ASSERT(vertexPtrVector[0]->hasAdjacentEdge(edgePtrVector[4]));
        CPPUNIT_ASSERT(vertexPtrVector[3]->hasAdjacentEdge(edgePtrVector[4]));
    }
};

CPPUNIT_TEST_SUITE_REGISTRATION(TriangleMeshCreatorTest);
