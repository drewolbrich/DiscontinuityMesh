// Copyright 2008 Retroactive Fiasco.

#include <cppunit/extensions/HelperMacros.h>

#include <meshrfm/ReadRfmFile.h>
#include <meshrfm/WriteRfmFile.h>
#include <mesh/Mesh.h>
#include <mesh/IsConsistent.h>
#include <mesh/StandardAttributes.h>

using mesh::AttributeKey;
using cgmath::Vector2f;
using cgmath::Vector3f;
using cgmath::Vector4f;
using cgmath::Matrix3f;
using cgmath::Matrix4f;
using cgmath::BoundingBox2f;
using cgmath::BoundingBox3f;

class ReadRfmFileTest : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE(ReadRfmFileTest);
    CPPUNIT_TEST(testReadRfmFile);
    CPPUNIT_TEST_SUITE_END();

public:
    void setUp() {
    }

    void tearDown() {
    }

    void testReadRfmFile() {
        mesh::Mesh mesh;

        mesh::FacePtr face = mesh.createFace();

        mesh::VertexPtr v1 = mesh.createVertex();
        mesh::VertexPtr v2 = mesh.createVertex();
        mesh::VertexPtr v3 = mesh.createVertex();

        mesh::EdgePtr e1 = mesh.createEdge();
        mesh::EdgePtr e2 = mesh.createEdge();
        mesh::EdgePtr e3 = mesh.createEdge();
    
        face->addAdjacentVertex(v1);
        face->addAdjacentVertex(v2);
        face->addAdjacentVertex(v3);

        face->addAdjacentEdge(e1);
        face->addAdjacentEdge(e2);
        face->addAdjacentEdge(e3);
    
        v1->addAdjacentFace(face);
        v2->addAdjacentFace(face);
        v3->addAdjacentFace(face);

        v1->addAdjacentEdge(e1);
        v1->addAdjacentEdge(e3);
        v2->addAdjacentEdge(e3);
        v2->addAdjacentEdge(e2);
        v3->addAdjacentEdge(e2);
        v3->addAdjacentEdge(e1);
    
        e1->addAdjacentFace(face);
        e2->addAdjacentFace(face);
        e3->addAdjacentFace(face);

        e1->addAdjacentVertex(v1);
        e1->addAdjacentVertex(v3);
        e2->addAdjacentVertex(v3);
        e2->addAdjacentVertex(v2);
        e3->addAdjacentVertex(v2);
        e3->addAdjacentVertex(v1);

        CPPUNIT_ASSERT(mesh::IsConsistent(mesh));

        AttributeKey boolKey 
            = mesh.getAttributeKey("bool", AttributeKey::BOOL);
        AttributeKey intKey 
            = mesh.getAttributeKey("int", AttributeKey::INT);
        AttributeKey floatKey 
            = mesh.getAttributeKey("float", AttributeKey::FLOAT);
        AttributeKey vector2fKey 
            = mesh.getAttributeKey("vector2f", AttributeKey::VECTOR2F);
        AttributeKey vector3fKey 
            = mesh.getAttributeKey("vector3f", AttributeKey::VECTOR3F);
        AttributeKey vector4fKey 
            = mesh.getAttributeKey("vector4f", AttributeKey::VECTOR4F);
        AttributeKey matrix3fKey 
            = mesh.getAttributeKey("matrix3f", AttributeKey::MATRIX3F);
        AttributeKey matrix4fKey 
            = mesh.getAttributeKey("matrix4f", AttributeKey::MATRIX4F);
        AttributeKey stringKey 
            = mesh.getAttributeKey("string", AttributeKey::STRING);
        AttributeKey unitVector3fKey 
            = mesh.getAttributeKey("unitVector3f", 
                AttributeKey::UNIT_VECTOR3F);
        AttributeKey temporaryBoolKey
            = mesh.getAttributeKey("temporaryBool", 
                AttributeKey::BOOL, AttributeKey::TEMPORARY);
        AttributeKey boundingBox2fKey 
            = mesh.getAttributeKey("boundingBox2f", AttributeKey::BOUNDINGBOX2F);
        AttributeKey boundingBox3fKey 
            = mesh.getAttributeKey("boundingBox3f", AttributeKey::BOUNDINGBOX3F);

        AttributeKey standardColor3fAttributeKey
            = mesh::GetColor3fAttributeKey(mesh);
        CPPUNIT_ASSERT(standardColor3fAttributeKey.isStandard());
        CPPUNIT_ASSERT(!standardColor3fAttributeKey.isTemporary());

        CPPUNIT_ASSERT(!boolKey.isStandard());
        CPPUNIT_ASSERT(!boolKey.isTemporary());
    
        face->setBool(boolKey, true);
        face->setInt(intKey, 100);
        face->setFloat(floatKey, 100);
        face->setVector2f(vector2fKey, Vector2f(100, 200));
        face->setVector3f(vector3fKey, Vector3f(100, 200, 300));
        face->setVector4f(vector4fKey, Vector4f(100, 200, 300, 400));
        face->setMatrix3f(matrix3fKey, 
            Matrix3f::fromRotation(Vector3f(1, 0, 0), 3.1415*0.25));
        face->setMatrix4f(matrix4fKey, 
            Matrix4f::fromRotation(Vector3f(1, 0, 0), 3.1415*0.25));
        face->setString(stringKey, "foo");
        face->setUnitVector3f(unitVector3fKey, 
            Vector3f(100, 200, 300));
        face->setBool(temporaryBoolKey, true);
        face->setBoundingBox2f(boundingBox2fKey,
            BoundingBox2f(100, 200, 300, 400));
        face->setBoundingBox3f(boundingBox3fKey,
            BoundingBox3f(100, 200, 300, 400, 500, 600));
    
        face->setVertexInt(v1, intKey, 100);
        face->setVertexInt(v2, intKey, 200);
        face->setVertexInt(v3, intKey, 300);


        // Write the mesh out.
    
        const char *filename = "/var/tmp/test.rfm";
        CPPUNIT_ASSERT_NO_THROW(meshrfm::WriteRfmFile(mesh, filename));


        // Read the mesh back in, and then check if it's the same
        // as what we just wrote out.
    
        mesh::Mesh readMesh;
        CPPUNIT_ASSERT_NO_THROW(meshrfm::ReadRfmFile(&readMesh, filename));

        CPPUNIT_ASSERT(mesh::IsConsistent(readMesh));

        CPPUNIT_ASSERT(readMesh.faceCount() == 1);
        CPPUNIT_ASSERT(readMesh.vertexCount() == 3);
        CPPUNIT_ASSERT(readMesh.edgeCount() == 3);
    
        boolKey = readMesh.getAttributeKey("bool", AttributeKey::BOOL);
        intKey = readMesh.getAttributeKey("int", AttributeKey::INT);
        floatKey = readMesh.getAttributeKey("float", AttributeKey::FLOAT);
        vector2fKey = readMesh.getAttributeKey("vector2f", 
            AttributeKey::VECTOR2F);
        vector3fKey = readMesh.getAttributeKey("vector3f", 
            AttributeKey::VECTOR3F);
        vector4fKey = readMesh.getAttributeKey("vector4f", 
            AttributeKey::VECTOR4F);
        matrix3fKey = readMesh.getAttributeKey("matrix3f", 
            AttributeKey::MATRIX3F);
        matrix4fKey = readMesh.getAttributeKey("matrix4f", 
            AttributeKey::MATRIX4F);
        stringKey = readMesh.getAttributeKey("string", 
            AttributeKey::STRING);
        unitVector3fKey = readMesh.getAttributeKey("unitVector3f", 
            AttributeKey::UNIT_VECTOR3F);
        boundingBox2fKey = readMesh.getAttributeKey("boundingBox2f", 
            AttributeKey::BOUNDINGBOX2F);
        boundingBox3fKey = readMesh.getAttributeKey("boundingBox3f", 
            AttributeKey::BOUNDINGBOX3F);

        standardColor3fAttributeKey = mesh::GetColor3fAttributeKey(readMesh);
        CPPUNIT_ASSERT(standardColor3fAttributeKey.isStandard());
        CPPUNIT_ASSERT(!standardColor3fAttributeKey.isTemporary());
    
        // Because the temporaryBool key was marked temporary,
        // it should not have survived being written to disk.
        CPPUNIT_ASSERT(!readMesh.hasAttributeKey("temporaryBool"));
    
        temporaryBoolKey = readMesh.getAttributeKey("temporaryBool", 
            AttributeKey::BOOL, AttributeKey::TEMPORARY);

        face = readMesh.faceBegin();
        CPPUNIT_ASSERT(face != readMesh.faceEnd());

        CPPUNIT_ASSERT(face->adjacentVertexCount() == 3);
        CPPUNIT_ASSERT(face->adjacentEdgeCount() == 3);
        
        v1 = readMesh.vertexBegin();
        CPPUNIT_ASSERT(v1 != readMesh.vertexEnd());
        v2 = v1;
        ++v2;
        v3 = v2;
        ++v3;

        CPPUNIT_ASSERT(v1->adjacentEdgeCount() == 2);
        CPPUNIT_ASSERT(v1->adjacentFaceCount() == 1);
        CPPUNIT_ASSERT(*(v1->adjacentFaceBegin()) == face);
        
        e1 = readMesh.edgeBegin();
        CPPUNIT_ASSERT(e1 != readMesh.edgeEnd());
        CPPUNIT_ASSERT(e1->adjacentVertexCount() == 2);
        CPPUNIT_ASSERT(e1->adjacentFaceCount() == 1);
        CPPUNIT_ASSERT(*(e1->adjacentFaceBegin()) == face);

        CPPUNIT_ASSERT(face->getBool(boolKey));
        CPPUNIT_ASSERT(face->getInt(intKey) == 100);
        CPPUNIT_ASSERT(face->getFloat(floatKey) == 100);
        CPPUNIT_ASSERT(face->getVector2f(vector2fKey) 
            == Vector2f(100, 200));
        CPPUNIT_ASSERT(face->getVector3f(vector3fKey) 
            == Vector3f(100, 200, 300));
        CPPUNIT_ASSERT(face->getVector4f(vector4fKey) 
            == Vector4f(100, 200, 300, 400));
        CPPUNIT_ASSERT(face->getMatrix3f(matrix3fKey) 
            == Matrix3f::fromRotation(Vector3f(1, 0, 0), 3.1415*0.25));
        CPPUNIT_ASSERT(face->getMatrix4f(matrix4fKey) 
            == Matrix4f::fromRotation(Vector3f(1, 0, 0), 3.1415*0.25));
        CPPUNIT_ASSERT(face->getString(stringKey) == "foo");
        CPPUNIT_ASSERT(face->getUnitVector3f(unitVector3fKey) 
            == Vector3f(100, 200, 300));
        CPPUNIT_ASSERT(face->getBoundingBox2f(boundingBox2fKey) 
            == BoundingBox2f(100, 200, 300, 400));
        CPPUNIT_ASSERT(face->getBoundingBox3f(boundingBox3fKey) 
            == BoundingBox3f(100, 200, 300, 400, 500, 600));

        // When the mesh is read back off disk, it should no longer
        // have the attribute flagged earlier as temporary.
        CPPUNIT_ASSERT(!face->hasAttribute(temporaryBoolKey));
    
        CPPUNIT_ASSERT(face->getVertexInt(v1, intKey) == 100);
        CPPUNIT_ASSERT(face->getVertexInt(v2, intKey) == 200);
        CPPUNIT_ASSERT(face->getVertexInt(v3, intKey) == 300);

        unlink(filename);
    }
};

CPPUNIT_TEST_SUITE_REGISTRATION(ReadRfmFileTest);
