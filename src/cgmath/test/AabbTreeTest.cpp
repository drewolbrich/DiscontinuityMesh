// Copyright 2009 Drew Olbrich

#include <cppunit/extensions/HelperMacros.h>

#include <cgmath/AabbTree.h>
#include <cgmath/Vector3f.h>

using cgmath::AabbTree;
using cgmath::Vector3f;

bool gCalled = false;

class Object 
{
public:
    cgmath::BoundingBox3f boundingBox() const {
        return cgmath::BoundingBox3f(0, 1, 0, 1, 0, 1);
    }
};

class BoundingBoxListener : public AabbTree<Object>::BoundingBoxListener 
{
public:
    virtual bool applyObjectToBoundingBox(Object &, 
        const cgmath::BoundingBox3f &) {
        gCalled = true;
        return true;
    }
};

class TriangleListener : public AabbTree<Object>::TriangleListener
{
public:
    virtual bool applyObjectToTriangleVector(Object &, 
        const AabbTree<Object>::TriangleVector &) {
        gCalled = true;
        return true;
    }
};

class TetrahedronListener : public AabbTree<Object>::TetrahedronListener
{
public:
    virtual bool applyObjectToTetrahedron(Object &, 
        const Vector3f &, const Vector3f &, const Vector3f &, const Vector3f &) {
        gCalled = true;
        return true;
    }
};

class AabbTreeTest : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE(AabbTreeTest);
    CPPUNIT_TEST(testCreation);
    CPPUNIT_TEST(testBoundingBoxListener);
    CPPUNIT_TEST(testTriangleListener);
    CPPUNIT_TEST(testTetrahedronListener);
    CPPUNIT_TEST_SUITE_END();

public:
    void setUp() {
    }

    void tearDown() {
    }

    void testCreation() {
        typedef AabbTree<Object> ObjectAabbTree;
        ObjectAabbTree mObjectAabbTree;

        ObjectAabbTree::ObjectVector objectVector;
        objectVector.push_back(Object());

        mObjectAabbTree.initialize(objectVector);
    }

    void testBoundingBoxListener() {
        typedef AabbTree<Object> ObjectAabbTree;
        ObjectAabbTree mObjectAabbTree;

        ObjectAabbTree::ObjectVector objectVector;
        objectVector.push_back(Object());

        mObjectAabbTree.initialize(objectVector);

        BoundingBoxListener boundingBoxListener;

        gCalled = false;
        mObjectAabbTree.applyToBoundingBoxIntersection(
            cgmath::BoundingBox3f(0, 1, 0, 1, 0, 1), &boundingBoxListener);
        CPPUNIT_ASSERT(gCalled);

        gCalled = false;
        mObjectAabbTree.applyToBoundingBoxIntersection(
            cgmath::BoundingBox3f(100, 101, 0, 1, 0, 1), &boundingBoxListener);
        CPPUNIT_ASSERT(!gCalled);
    }

    void testTriangleListener() {
        typedef AabbTree<Object> ObjectAabbTree;
        ObjectAabbTree mObjectAabbTree;

        ObjectAabbTree::ObjectVector objectVector;
        objectVector.push_back(Object());

        mObjectAabbTree.initialize(objectVector);

        TriangleListener triangleListener;

        AabbTree<Object>::TriangleVector triangleVector;

        AabbTree<Object>::Triangle triangle;
        triangle.mPointArray[0] = Vector3f(0.5, -1.0, 0.0);
        triangle.mPointArray[1] = Vector3f(0.5, 2.0, 2.0);
        triangle.mPointArray[2] = Vector3f(0.5, 0.5, 0.0);
        triangleVector.push_back(triangle);

        gCalled = false;
        mObjectAabbTree.applyToTriangleVectorIntersection(triangleVector, &triangleListener);
        CPPUNIT_ASSERT(gCalled);

        triangle.mPointArray[0] = Vector3f(1.5, -1.0, 0.0);
        triangle.mPointArray[1] = Vector3f(1.5, 2.0, 2.0);
        triangle.mPointArray[2] = Vector3f(1.5, 0.5, 0.0);
        triangleVector.clear();
        triangleVector.push_back(triangle);

        gCalled = false;
        mObjectAabbTree.applyToTriangleVectorIntersection(triangleVector, &triangleListener);
        CPPUNIT_ASSERT(!gCalled);
    }

    void testTetrahedronListener() {
        typedef AabbTree<Object> ObjectAabbTree;
        ObjectAabbTree mObjectAabbTree;

        ObjectAabbTree::ObjectVector objectVector;
        objectVector.push_back(Object());

        mObjectAabbTree.initialize(objectVector);

        TetrahedronListener tetrahedronListener;

        gCalled = false;
        mObjectAabbTree.applyToTetrahedronIntersection(
            Vector3f(-5.0, 0.0, -5.0),
            Vector3f(5.0, 0.0, -5.0),
            Vector3f(0.0, -5.0, 5.0),
            Vector3f(0.0, 5.0, 5.0),
            &tetrahedronListener);
        CPPUNIT_ASSERT(gCalled);

        gCalled = false;
        mObjectAabbTree.applyToTetrahedronIntersection(
            Vector3f(-5.0, 0.0, 95.0),
            Vector3f(5.0, 0.0, 95.0),
            Vector3f(0.0, -5.0, 105.0),
            Vector3f(0.0, 5.0, 105.0),
            &tetrahedronListener);
        CPPUNIT_ASSERT(!gCalled);
    }
};

CPPUNIT_TEST_SUITE_REGISTRATION(AabbTreeTest);
