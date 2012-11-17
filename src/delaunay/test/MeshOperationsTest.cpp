// Copyright 2008 Retroactive Fiasco.

#include <cppunit/extensions/HelperMacros.h>

#include <delaunay/MeshOperations.h>
#include <delaunay/Mesh.h>
#include <delaunay/Vertex.h>

using delaunay::Mesh;
using delaunay::MeshPtr;
using delaunay::VertexPtr;
using cgmath::Vector2f;
using cgmath::BoundingBox2f;

class MeshOperationsTest : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE(MeshOperationsTest);
    CPPUNIT_TEST(testGetMeshBoundingBox);
    CPPUNIT_TEST_SUITE_END();

public:
    void setUp() {
    }

    void tearDown() {
    }

    void testGetMeshBoundingBox() {
        MeshPtr meshPtr(new Mesh);

        VertexPtr v1 = meshPtr->createVertex();
        VertexPtr v2 = meshPtr->createVertex();
        VertexPtr v3 = meshPtr->createVertex();

        v1->setPosition(Vector2f(2, 3));
        v2->setPosition(Vector2f(10, 5));
        v3->setPosition(Vector2f(4, 12));

        CPPUNIT_ASSERT(GetMeshBoundingBox(meshPtr) == BoundingBox2f(2, 10, 3, 12));
    }
};

CPPUNIT_TEST_SUITE_REGISTRATION(MeshOperationsTest);
