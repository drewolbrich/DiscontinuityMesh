// Copyright 2008 Retroactive Fiasco.

#include <cppunit/extensions/HelperMacros.h>

#include <vector>

#include <cgmath/Vector3f.h>
#include <mesh/Types.h>
#include <mesh/InterpolateAttributes.h>
#include <mesh/StandardAttributes.h>
#include <mesh/Mesh.h>

using cgmath::Vector2f;
using cgmath::Vector3f;
using cgmath::Vector4f;
using cgmath::Matrix3f;
using cgmath::Matrix4f;
using mesh::Mesh;
using mesh::VertexPtr;
using mesh::ConstVertexPtr;
using mesh::EdgePtr;
using mesh::FacePtr;
using mesh::AttributeKey;

class InterpolateAttributesTest : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE(InterpolateAttributesTest);
    CPPUNIT_TEST(testAssignInterpolatedVertexAttributes);
    CPPUNIT_TEST(testAssignInterpolatedFaceVertexAttributes);
    CPPUNIT_TEST(testAssignInterpolatedFaceVertexNormal);
    CPPUNIT_TEST_SUITE_END();

public:
    void setUp() {
    }

    void tearDown() {
    }

    void testAssignInterpolatedVertexAttributes() {
        Mesh mesh;
        
        VertexPtr v1 = mesh.createVertex();
        VertexPtr v2 = mesh.createVertex();
        VertexPtr v3 = mesh.createVertex();
        VertexPtr v4 = mesh.createVertex();
        
        v1->setPosition(Vector3f(0, 0, 0));
        v2->setPosition(Vector3f(1, 0, 0));
        v3->setPosition(Vector3f(1, 1, 0));
        v4->setPosition(Vector3f(0, 1, 0));
        
        std::vector<ConstVertexPtr> vertexPtrVector;
        vertexPtrVector.push_back(v1);
        vertexPtrVector.push_back(v2);
        vertexPtrVector.push_back(v3);
        vertexPtrVector.push_back(v4);
        
        AttributeKey key = mesh.getAttributeKey("testKey", AttributeKey::FLOAT);
        v1->setFloat(key, 1.0);
        v2->setFloat(key, 2.0);
        v3->setFloat(key, 2.0);
        v4->setFloat(key, 1.0);
        
        VertexPtr vertexPr = mesh.createVertex();
        vertexPr->setPosition(Vector3f(0.5, 0.5, 0.0));
        
        AssignInterpolatedVertexAttributes(vertexPr, vertexPtrVector);
        
        CPPUNIT_ASSERT(vertexPr->getFloat(key) == 1.5);
        
        vertexPr = mesh.createVertex();
        vertexPr->setPosition(Vector3f(0.25, 0.0, 0.0));
        
        AssignInterpolatedVertexAttributes(vertexPr, vertexPtrVector);
        
        CPPUNIT_ASSERT(vertexPr->getFloat(key) == 1.25);
    }

    void testAssignInterpolatedFaceVertexAttributes() {
        Mesh mesh;
        
        VertexPtr v1 = mesh.createVertex();
        VertexPtr v2 = mesh.createVertex();
        VertexPtr v3 = mesh.createVertex();
        VertexPtr v4 = mesh.createVertex();
        
        v1->setPosition(Vector3f(0, 0, 0));
        v2->setPosition(Vector3f(1, 0, 0));
        v3->setPosition(Vector3f(1, 1, 0));
        v4->setPosition(Vector3f(0, 1, 0));

        FacePtr facePtr = mesh.createFace();

        facePtr->addAdjacentVertex(v1);
        facePtr->addAdjacentVertex(v2);
        facePtr->addAdjacentVertex(v3);
        facePtr->addAdjacentVertex(v4);

        v1->addAdjacentFace(facePtr);
        v2->addAdjacentFace(facePtr);
        v3->addAdjacentFace(facePtr);
        v4->addAdjacentFace(facePtr);
        
        std::vector<ConstVertexPtr> vertexPtrVector;
        vertexPtrVector.push_back(v1);
        vertexPtrVector.push_back(v2);
        vertexPtrVector.push_back(v3);
        vertexPtrVector.push_back(v4);
        
        AttributeKey key = mesh.getAttributeKey("testKey", 
            AttributeKey::FLOAT);
        facePtr->setVertexFloat(v1, key, 1.0);
        facePtr->setVertexFloat(v2, key, 2.0);
        facePtr->setVertexFloat(v3, key, 2.0);
        facePtr->setVertexFloat(v4, key, 1.0);
        
        VertexPtr vertexPtr = mesh.createVertex();
        vertexPtr->setPosition(Vector3f(0.5, 0.5, 0.0));

        facePtr->addAdjacentVertex(vertexPtr);
        vertexPtr->addAdjacentFace(facePtr);
        
        AssignInterpolatedFaceVertexAttributes(facePtr, 
            vertexPtr, facePtr, vertexPtrVector);
        
        CPPUNIT_ASSERT(facePtr->getVertexFloat(vertexPtr, key) == 1.5);

        vertexPtr = mesh.createVertex();
        vertexPtr->setPosition(Vector3f(0.25, 0.0, 0.0));
        
        AssignInterpolatedFaceVertexAttributes(facePtr, 
            vertexPtr, facePtr, vertexPtrVector);
        
        CPPUNIT_ASSERT(facePtr->getVertexFloat(vertexPtr, key) == 1.25);
    }

    void testAssignInterpolatedFaceVertexNormal() {
        Mesh mesh;
        
        VertexPtr v1 = mesh.createVertex();
        VertexPtr v2 = mesh.createVertex();
        VertexPtr v3 = mesh.createVertex();
        VertexPtr v4 = mesh.createVertex();
        
        v1->setPosition(Vector3f(0, 0, 0));
        v2->setPosition(Vector3f(1, 0, 0));
        v3->setPosition(Vector3f(1, 1, 0));
        v4->setPosition(Vector3f(0, 1, 0));

        FacePtr facePtr = mesh.createFace();

        facePtr->addAdjacentVertex(v1);
        facePtr->addAdjacentVertex(v2);
        facePtr->addAdjacentVertex(v3);
        facePtr->addAdjacentVertex(v4);

        v1->addAdjacentFace(facePtr);
        v2->addAdjacentFace(facePtr);
        v3->addAdjacentFace(facePtr);
        v4->addAdjacentFace(facePtr);
        
        std::vector<ConstVertexPtr> vertexPtrVector;
        vertexPtrVector.push_back(v1);
        vertexPtrVector.push_back(v2);
        vertexPtrVector.push_back(v3);
        vertexPtrVector.push_back(v4);
        
        AttributeKey key = mesh::GetNormal3fAttributeKey(mesh);
        facePtr->setVertexUnitVector3f(v1, key, Vector3f(0.0, 0.0, 1.0));
        facePtr->setVertexUnitVector3f(v2, key, Vector3f(0.0, 0.0, 1.0));
        facePtr->setVertexUnitVector3f(v3, key, Vector3f(0.0, 0.0, 1.0));
        facePtr->setVertexUnitVector3f(v4, key, Vector3f(0.0, 0.0, 1.0));
        
        VertexPtr vertexPtr = mesh.createVertex();
        vertexPtr->setPosition(Vector3f(0.25, 0.333, 0.0));

        facePtr->addAdjacentVertex(vertexPtr);
        vertexPtr->addAdjacentFace(facePtr);
        
        AssignInterpolatedFaceVertexAttributes(facePtr, 
            vertexPtr, facePtr, vertexPtrVector);
        
        CPPUNIT_ASSERT(facePtr->getVertexUnitVector3f(vertexPtr, key) 
            == cgmath::Vector3f(0.0, 0.0, 1.0));

        vertexPtr = mesh.createVertex();
        vertexPtr->setPosition(Vector3f(0.25, 0.0, 0.0));
        
        AssignInterpolatedFaceVertexAttributes(facePtr, 
            vertexPtr, facePtr, vertexPtrVector);
        
        CPPUNIT_ASSERT(facePtr->getVertexUnitVector3f(vertexPtr, key) 
            == cgmath::Vector3f(0.0, 0.0, 1.0));
    }
};

CPPUNIT_TEST_SUITE_REGISTRATION(InterpolateAttributesTest);
