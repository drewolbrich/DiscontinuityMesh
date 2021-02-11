// Copyright 2008 Drew Olbrich

#include <cppunit/extensions/HelperMacros.h>
#include <boost/shared_ptr.hpp>

#include <mesh/Face.h>
#include <mesh/Mesh.h>

using mesh::Mesh;
using mesh::VertexPtr;
using mesh::FacePtr;
using mesh::AttributeKey;

class FaceTest : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE(FaceTest);
    CPPUNIT_TEST(testIntAttribute);
    CPPUNIT_TEST_SUITE_END();

public:
    void setUp() {
        mMeshPtr.reset(new Mesh);

        mF1 = mMeshPtr->createFace();

        mV1 = mMeshPtr->createVertex();
        mV2 = mMeshPtr->createVertex();
        mV3 = mMeshPtr->createVertex();

        mF1->addAdjacentVertex(mV1);
        mF1->addAdjacentVertex(mV2);
        mF1->addAdjacentVertex(mV3);

        mV1->addAdjacentFace(mF1);
        mV2->addAdjacentFace(mF1);
        mV3->addAdjacentFace(mF1);
    }

    void tearDown() {
        mMeshPtr.reset();
    }

    boost::shared_ptr<Mesh> mMeshPtr;
    VertexPtr mV1;
    VertexPtr mV2;
    VertexPtr mV3;
    FacePtr mF1;

    void testIntAttribute() {
        AttributeKey key = mMeshPtr->getAttributeKey("key", AttributeKey::INT);

        CPPUNIT_ASSERT(!mF1->hasVertexAttribute(mV1, key));
        CPPUNIT_ASSERT(mF1->getVertexInt(mV1, key) == 0);
        CPPUNIT_ASSERT(!mF1->hasVertexAttribute(mV1, key));

        mF1->setVertexInt(mV1, key, 100);
    
        CPPUNIT_ASSERT(mF1->hasVertexAttribute(mV1, key));
        CPPUNIT_ASSERT(mF1->getVertexInt(mV1, key) == 100);

        mF1->eraseVertexAttribute(mV1, key);
    
        CPPUNIT_ASSERT(!mF1->hasVertexAttribute(mV1, key));
    }
};

CPPUNIT_TEST_SUITE_REGISTRATION(FaceTest);
