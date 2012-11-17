// Copyright 2008 Retroactive Fiasco.

#include <cppunit/extensions/HelperMacros.h>

#include <meshrfm/WriteRfmFile.h>
#include <mesh/Mesh.h>

class WriteRfmFileTest : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE(WriteRfmFileTest);
    CPPUNIT_TEST(testWriteRfmFile);
    CPPUNIT_TEST_SUITE_END();

public:
    void setUp() {
    }

    void tearDown() {
    }

    void testWriteRfmFile() {
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

        e1->addAdjacentFace(face);
        e2->addAdjacentFace(face);
        e3->addAdjacentFace(face);

        e1->addAdjacentVertex(v1);
        e1->addAdjacentVertex(v2);
        e2->addAdjacentVertex(v2);
        e2->addAdjacentVertex(v3);
        e3->addAdjacentVertex(v3);
        e3->addAdjacentVertex(v1);

        v1->addAdjacentEdge(e1);
        v2->addAdjacentEdge(e1);
        v2->addAdjacentEdge(e2);
        v3->addAdjacentEdge(e2);
        v3->addAdjacentEdge(e3);
        v1->addAdjacentEdge(e3);
    
        mesh::AttributeKey key = mesh.getAttributeKey("key", mesh::AttributeKey::INT);

        face->setVertexInt(v1, key, 100);

        const char *filename = "/var/tmp/test.rfm";
        CPPUNIT_ASSERT_NO_THROW(meshrfm::WriteRfmFile(mesh, filename));

        unlink(filename);
    }
};

CPPUNIT_TEST_SUITE_REGISTRATION(WriteRfmFileTest);
