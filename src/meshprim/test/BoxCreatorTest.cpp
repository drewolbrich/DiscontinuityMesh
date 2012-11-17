// Copyright 2009 Retroactive Fiasco.

#include <cppunit/extensions/HelperMacros.h>

#include <meshprim/BoxCreator.h>
#include <cgmath/Vector3f.h>
#include <mesh/Mesh.h>
#include <mesh/IsConsistent.h>
#include <mesh/AttributePossessor.h>
#include <mesh/StandardAttributes.h>

using meshprim::BoxCreator;

class BoxCreatorTest : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE(BoxCreatorTest);
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

        BoxCreator boxCreator;
        boxCreator.setMesh(&mesh);
        boxCreator.setTransformationMatrix(
            cgmath::Matrix4f::fromTranslation(cgmath::Vector3f(10.0, 11.0, 12.0)));
        boxCreator.setWidth(3.0);
        boxCreator.setHeight(4.0);
        boxCreator.setDepth(5.0);
        boxCreator.setFaceAttributes(faceAttributes);
        boxCreator.create();

        CPPUNIT_ASSERT(mesh::IsConsistent(mesh));
    }
};

CPPUNIT_TEST_SUITE_REGISTRATION(BoxCreatorTest);
