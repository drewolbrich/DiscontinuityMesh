// Copyright 2009 Drew Olbrich

#include <cppunit/extensions/HelperMacros.h>

#include <meshprim/CylinderCreator.h>
#include <cgmath/Vector3f.h>
#include <mesh/Mesh.h>
#include <mesh/IsConsistent.h>
#include <mesh/AttributePossessor.h>
#include <mesh/StandardAttributes.h>

using meshprim::CylinderCreator;

class CylinderCreatorTest : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE(CylinderCreatorTest);
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

        CylinderCreator cylinderCreator;
        cylinderCreator.setMesh(&mesh);
        cylinderCreator.setTransformationMatrix(
            cgmath::Matrix4f::fromTranslation(cgmath::Vector3f(10.0, 11.0, 12.0)));
        cylinderCreator.setHeight(8.0);
        cylinderCreator.setRadius(5.0);
        cylinderCreator.setDivisions(12);
        cylinderCreator.setFaceAttributes(faceAttributes);
        cylinderCreator.create();

        CPPUNIT_ASSERT(mesh::IsConsistent(mesh));
    }
};

CPPUNIT_TEST_SUITE_REGISTRATION(CylinderCreatorTest);
