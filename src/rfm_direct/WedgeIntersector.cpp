// Copyright 2008 Retroactive Fiasco.

#include "WedgeIntersector.h"

#include <cassert>
#include <cmath>
#include <iostream>
#include <algorithm>
#include <limits>

#include <cgmath/LineOperations.h>
#include <cgmath/PlaneOperations.h>
#include <cgmath/Vector3fOperations.h>
#include <cgmath/Vector3d.h>
#include <cgmath/Tolerance.h>
#include <exact/GeometricPredicates.h>
#include <mesh/Edge.h>
#include <mesh/EdgeOperations.h>
#include <mesh/Face.h>
#include <mesh/FaceOperations.h>
#include <mesh/Vertex.h>

WedgeIntersector::WedgeIntersector()
    : mEventType(VE_EVENT),
      mVertexPtrIsDefined(false),
      mVertexPtr(),
      mEdgePtrIsDefined(false),
      mEdgePtr(),
      mLightVertexIndex0(0),
      mLightVertexIndex1(0),
      mV(),
      mW(),
      mP(),
      mQ(),
      mWedgeVertexP(),
      mWedgeVertexQ(),
      mWedgePositiveXAxis(),
      mWedgePositiveYAxis(),
      mWedgePositiveZAxis(),
      mEndpointIdentifierVP(),
      mEndpointIdentifierWQ(),
      mEndpointIdentifierPQ(),
      mFacePtr(),
      mVertexPtr0(),
      mVertexPtr1(),
      mVertexPtr2(),
      mEdgePtr0(),
      mEdgePtr1(),
      mEdgePtr2(),
      mA(),
      mB(),
      mC(),
      mOrientationA(0.0),
      mOrientationB(0.0),
      mOrientationC(0.0),
      mD(),
      mE(),
      mDIsDefined(false),
      mEIsDefined(false),
      mId0(),
      mId1(),
      mLineSegmentCount(0),
      mLineSegmentArray(),
      mWedgeIdentifier(0)
{
}

WedgeIntersector::~WedgeIntersector()
{
}

bool
WedgeIntersector::setVeEventWedge(mesh::VertexPtr vertexPtr, mesh::EdgePtr edgePtr)
{
    mEventType = VE_EVENT;
    mVertexPtrIsDefined = true;
    mVertexPtr = vertexPtr;
    mEdgePtrIsDefined = true;
    mEdgePtr = edgePtr;

    mV = mVertexPtr->position();
    mW = mV;

    // Initialize occluder edge PQ.
    initializeEdgePQ();

    mEndpointIdentifierVP = meshretri::EndpointIdentifier::fromVertexPtrPair(
        mVertexPtr, mWedgeVertexP);
    mEndpointIdentifierWQ = meshretri::EndpointIdentifier::fromVertexPtrPair(
        mVertexPtr, mWedgeVertexQ);
    mEndpointIdentifierPQ = meshretri::EndpointIdentifier::fromVertexPtrPair(
        mWedgeVertexP, mWedgeVertexQ);

    return initializeWedge();
}

bool
WedgeIntersector::setEvEventWedge(mesh::EdgePtr edgePtr, mesh::VertexPtr vertexPtr)
{
    mEventType = EV_EVENT;
    mVertexPtrIsDefined = true;
    mVertexPtr = vertexPtr;
    mEdgePtrIsDefined = true;
    mEdgePtr = edgePtr;

    mV = mVertexPtr->position();
    mW = mV;

    // Initialize light source edge PQ.
    initializeEdgePQ();

    mEndpointIdentifierVP = meshretri::EndpointIdentifier::fromVertexPtrPair(
        mVertexPtr, mWedgeVertexP);
    mEndpointIdentifierWQ = meshretri::EndpointIdentifier::fromVertexPtrPair(
        mVertexPtr, mWedgeVertexQ);
    mEndpointIdentifierPQ = meshretri::EndpointIdentifier::fromVertexPtrPair(
        mWedgeVertexP, mWedgeVertexQ);

    return initializeWedge();
}

bool
WedgeIntersector::setDistantLightEeEventWedge(const cgmath::Vector3f &lightVertex0,
    const cgmath::Vector3f &lightVertex1, unsigned lightVertexIndex, mesh::EdgePtr edgePtr)
{
    mEventType = DISTANT_LIGHT_EE_EVENT;
    mV = lightVertex0;
    mW = lightVertex1;
    mLightVertexIndex0 = lightVertexIndex;
    mVertexPtrIsDefined = false;
    mEdgePtrIsDefined = true;
    mEdgePtr = edgePtr;

    // Initialize occluder edge PQ.
    initializeEdgePQ();

    mEndpointIdentifierVP = meshretri::EndpointIdentifier::fromVertexPtrAndIndex(
        mWedgeVertexP, lightVertexIndex);
    mEndpointIdentifierWQ = meshretri::EndpointIdentifier::fromVertexPtrAndIndex(
        mWedgeVertexQ, lightVertexIndex);
    mEndpointIdentifierPQ = meshretri::EndpointIdentifier::fromVertexPtrPair(
        mWedgeVertexP, mWedgeVertexQ);

    return initializeWedge();
}

bool
WedgeIntersector::setDistantLightEvEventWedge(const cgmath::Vector3f &lightVertex0,
    const cgmath::Vector3f &lightVertex1, unsigned lightVertexIndex0, unsigned lightVertexIndex1, 
    mesh::VertexPtr vertexPtr)
{
    mEventType = DISTANT_LIGHT_EV_EVENT;
    mV = vertexPtr->position();
    mW = mV;
    mLightVertexIndex0 = lightVertexIndex0;
    mLightVertexIndex1 = lightVertexIndex1;
    mVertexPtrIsDefined = true;
    mVertexPtr = vertexPtr;
    mEdgePtrIsDefined = false;

    mP = lightVertex0;
    mQ = lightVertex1;

    mEndpointIdentifierVP = meshretri::EndpointIdentifier::fromVertexPtrAndIndex(
        mVertexPtr, lightVertexIndex0);
    mEndpointIdentifierWQ = meshretri::EndpointIdentifier::fromVertexPtrAndIndex(
        mVertexPtr, lightVertexIndex1);
    mEndpointIdentifierPQ = meshretri::EndpointIdentifier::createUniqueIdentifier();

    return initializeWedge();
}

WedgeIntersector::EventType
WedgeIntersector::eventType() const
{
    return mEventType;
}

bool
WedgeIntersector::vertexPtrIsDefined() const
{
    return mVertexPtrIsDefined;
}

mesh::VertexPtr
WedgeIntersector::vertexPtr() const
{
    return mVertexPtr;
}

bool
WedgeIntersector::edgePtrIsDefined() const
{
    return mEdgePtrIsDefined;
}

mesh::EdgePtr
WedgeIntersector::edgePtr() const
{
    return mEdgePtr;
}

unsigned
WedgeIntersector::lightVertexIndex0() const
{
    return mLightVertexIndex0;
}

unsigned
WedgeIntersector::lightVertexIndex1() const
{
    return mLightVertexIndex1;
}

int 
WedgeIntersector::testTriangle(mesh::FacePtr facePtr, LineSegment **lineSegmentArray)
{
    mLineSegmentCount = 0;

    mFacePtr = facePtr;

    if (!initializeTriangle()) {
        // The triangle has zero area.
        return 0;
    }

    // Trivially reject the triangle if it doesn't intersect the
    // plane of the wedge at all.
    if (!triangleIntersectsWedgePlane()) {
        return 0;
    }

    // Everything works more robustly if we don't cull out backfacing triangles.
    // This better handles models with degenerate triangles that result in 
    // false silhouette edges. When wedges are traced from these edges,
    // the backfacing triangles will occlude them and they won't cast
    // critical line segments onto visible geometry.
    // If later we write rfm_cleanup to clean up degenerate geometry, 
    // and it really works robustly, then it's possible we could reenable 
    // this test as an optimization.
#if 0
    // If the triangle is backfacing, discard it.
    if (!triangleIsFrontfacing()) {
        return 0;
    }
#endif

    mDIsDefined = false;
    mEIsDefined = false;

    // Check if any of the triangle vertices exactly intersect the wedge plane.
    testTriangleVertices();

    // Test if any of the triangle edges intersect the wedge plane.
    testTriangleEdges();

    if (!mDIsDefined) {
        // None of the vertices or edges of the triangle
        // intersected the wedge.
        return 0;
    }

    // It's possible that only a single point intersects with the wedge,
    // in which case we want both endpoints of the line segment
    // to be identical.
    // This actually happens quite often, perhaps on all faces that
    // are adjacent to the vertices that are the endpoints of the wedge's mesh edge,
    // and intersect with it only at a single point.
    // TODO: Try discarding these line segments.
    if (!mEIsDefined) {
        mE = mD;
        mId1 = mId0;
    }

    mLineSegmentCount = 0;

    ClippableEndpoint ce0;
    ce0.mPoint = mD;
    ce0.mId = mId0;
    ce0.mVisibilityParameter = projectPointOntoEdgePQ(mD);

    ClippableEndpoint ce1;
    ce1.mPoint = mE;
    ce1.mId = mId1;
    ce1.mVisibilityParameter = projectPointOntoEdgePQ(mE);

    // Clip the line segment to the wedge.
    switch (mEventType) {
    case VE_EVENT:
    case DISTANT_LIGHT_EE_EVENT:
        clipToVeWedge(ce0, ce1);
        break;
    case EV_EVENT:
    case DISTANT_LIGHT_EV_EVENT:
        clipToEvWedge(ce0, ce1);
        break;
    }

    *lineSegmentArray = mLineSegmentArray;
    return mLineSegmentCount;
}

float
WedgeIntersector::projectPointOntoEdgePQ(const cgmath::Vector3f &worldSpacePoint) const
{
    if (mV == worldSpacePoint) {
        return 0.0;
    }

    cgmath::Vector3f m;
    cgmath::Vector3f n;

    switch (mEventType) {
    case VE_EVENT:
    case EV_EVENT:
    case DISTANT_LIGHT_EV_EVENT:
        if (!cgmath::GetClosestPointsOnLines(mV, worldSpacePoint, mP, mQ, &m, &n)) {
            return Endpoint::UNDEFINED_VISIBILITY_PARAMETER;
        }
        break;
    case DISTANT_LIGHT_EE_EVENT:
        if (!cgmath::GetClosestPointsOnLines(worldSpacePoint + (mV - mP), 
                worldSpacePoint, mP, mQ, &m, &n)) {
            return Endpoint::UNDEFINED_VISIBILITY_PARAMETER;
        }
        break;
    }

    // Returns 0 at P and 1 at Q.
    return (m - mP).length()/(mQ - mP).length();
}

bool
WedgeIntersector::projectEdgePQPointOntoLineSegment(float s,
    const LineSegment &lineSegment, cgmath::Vector3f *point)
{
    cgmath::Vector3f a = cgmath::InterpolateVector3f(mP, mQ, s);
    
    cgmath::Vector3f b;
    switch (mEventType) {
    case VE_EVENT:
    case EV_EVENT:
    case DISTANT_LIGHT_EV_EVENT:
        b = mV;
        break;
    case DISTANT_LIGHT_EE_EVENT:
        b = a + (mV - mP);
        break;
    }

    if (a == b) {
        // Calling GetClosestPointsOnLines would fail because no unique line
        // would be defined by points a and b.
        return false;
    }

    cgmath::Vector3f c = lineSegment.point0().worldPosition();
    cgmath::Vector3f d = lineSegment.point1().worldPosition();

    if (c == d) {
        // Calling GetClosestPointsOnLines would fail because no unique line
        // would be defined by points c and d.
        return false;
    }

    cgmath::Vector3f m;
    cgmath::Vector3f n;
    if (!cgmath::GetClosestPointsOnLines(a, b, c, d, &m, &n)) {
        return false;
    }

    *point = m;
    return true;
}

void
WedgeIntersector::transformWorldSpacePointToWedgeSpacePoint(
    const cgmath::Vector3f &worldSpacePoint, cgmath::Vector2f *wedgeSpacePoint)
{
    switch (mEventType) {
    case VE_EVENT:
    case EV_EVENT:
    case DISTANT_LIGHT_EV_EVENT:
        (*wedgeSpacePoint)[0] = (worldSpacePoint - mV).dot(mWedgePositiveXAxis);
        (*wedgeSpacePoint)[1] = (worldSpacePoint - mV).dot(mWedgePositiveYAxis);
        break;
    case DISTANT_LIGHT_EE_EVENT:
        (*wedgeSpacePoint)[0] = (worldSpacePoint - (mV + mW)/2.0).dot(mWedgePositiveXAxis);
        (*wedgeSpacePoint)[1] = (worldSpacePoint - (mV + mW)/2.0).dot(mWedgePositiveYAxis);
        break;
    }
}

void
WedgeIntersector::transformWedgeSpacePointToWorldSpacePoint(
    const cgmath::Vector2f &wedgeSpacePoint, cgmath::Vector3f *worldSpacePoint)
{
    switch (mEventType) {
    case VE_EVENT:
    case EV_EVENT:
    case DISTANT_LIGHT_EV_EVENT:
        *worldSpacePoint = mV 
            + mWedgePositiveXAxis*wedgeSpacePoint[0]
            + mWedgePositiveYAxis*wedgeSpacePoint[1];
        break;
    case DISTANT_LIGHT_EE_EVENT:
        *worldSpacePoint = (mV + mW)/2.0
            + mWedgePositiveXAxis*wedgeSpacePoint[0]
            + mWedgePositiveYAxis*wedgeSpacePoint[1];
        break;
    }
}

cgmath::Vector3f
WedgeIntersector::getVectorTowardLight(const cgmath::Vector3f &point) const
{
    switch (mEventType) {
    case VE_EVENT:
    case EV_EVENT:
    case DISTANT_LIGHT_EV_EVENT:
        if ((point - mV).dot(mWedgePositiveYAxis) > 0.0) {
            // Between point V and the light source.
            return -(mV - point).normalized();
        } else {
            // Behind point V, with respect to the light source.
            return (mV - point).normalized();
        }
        break;
    case DISTANT_LIGHT_EE_EVENT:
        // The light rays are parallel, so the location of 'point' does not matter.
        return (mV - mP).normalized();
        break;
    }

    // This should never happen.
    abort();
}

bool
WedgeIntersector::faceIsAdjacentToWedge(mesh::FacePtr facePtr) const
{
    return (mEdgePtrIsDefined 
        && facePtr->hasAdjacentEdge(mEdgePtr))
        || (mVertexPtrIsDefined
            && facePtr->hasAdjacentVertex(mVertexPtr));
}

unsigned long
WedgeIntersector::wedgeIdentifier() const
{
    return mWedgeIdentifier;
}

void
WedgeIntersector::initializeEdgePQ()
{
    assert(mEdgePtrIsDefined);

    mesh::GetEdgeAdjacentVertices(mEdgePtr, &mWedgeVertexP, &mWedgeVertexQ);
    mesh::GetEdgeVertexPositions(mEdgePtr, &mP, &mQ);
}

bool
WedgeIntersector::initializeWedge()
{
    // The wedge must not be degenerate.
    if (mV == mP
        || mV == mQ
        || mP == mQ) {
        return false;
    }

    // The area of the wedge must not be zero.
    if (((mP - mV).cross(mQ - mV)).length() == 0.0) {
        return false;
    }

    using cgmath::Vector3f;
    using cgmath::Vector3d;

    cgmath::Vector3d xAxis;
    cgmath::Vector3d yAxis;
    cgmath::Vector3d zAxis;

    // The normal vector of the wedge.
    // This should work in the DISTANT_LIGHT_EE_EVENT case
    // because quad VWQP is planar.
    zAxis = ((Vector3d(mP) - Vector3d(mV)).cross(Vector3d(mQ) - Vector3d(mV))).normalize();

    // The wedge normal vector must not be zero and must not be infinity or NaN.
    if (zAxis == Vector3d::ZERO
        || zAxis*0.0 != Vector3d::ZERO) {
        return false;
    }

    // The +Y axis always points back along the wedge toward the light source.
    switch (mEventType) {
    case VE_EVENT:
    case EV_EVENT:
    case DISTANT_LIGHT_EV_EVENT:
        yAxis = (Vector3d(mV) - (Vector3d(mP) + Vector3d(mQ))/2.0).normalize();
        if (mEventType == EV_EVENT
            || mEventType == DISTANT_LIGHT_EV_EVENT) {
            yAxis = -yAxis;
        }
        break;
    case DISTANT_LIGHT_EE_EVENT:
        yAxis = ((Vector3d(mV) + Vector3d(mW))/2.0 
            - (Vector3d(mP) + Vector3d(mQ))/2.0).normalize();
        break;
    }

    xAxis = yAxis.cross(zAxis).normalize();

    mWedgePositiveXAxis = Vector3f(xAxis);
    mWedgePositiveYAxis = Vector3f(yAxis);
    mWedgePositiveZAxis = Vector3f(zAxis);

    ++mWedgeIdentifier;

    // The origin of the 2D wedge space (in the wedge XY plane) is 0,0.

    return true;
}

bool
WedgeIntersector::initializeTriangle()
{
    // The input face must be a triangle.
    assert(mFacePtr->adjacentVertexCount() == 3);
    assert(mFacePtr->adjacentEdgeCount() == 3);

    // Get the vertices of the triangle.
    mesh::AdjacentVertexIterator vertexIterator = mFacePtr->adjacentVertexBegin();
    mVertexPtr0 = *vertexIterator;
    ++vertexIterator;
    mVertexPtr1 = *vertexIterator;
    ++vertexIterator;
    mVertexPtr2 = *vertexIterator;
    ++vertexIterator;
    assert(vertexIterator == mFacePtr->adjacentVertexEnd());

    // Get the edges of the triangle.
    mesh::AdjacentEdgeIterator edgeIterator = mFacePtr->adjacentEdgeBegin();
    mEdgePtr0 = *edgeIterator;
    ++edgeIterator;
    mEdgePtr1 = *edgeIterator;
    ++edgeIterator;
    mEdgePtr2 = *edgeIterator;
    ++edgeIterator;
    assert(edgeIterator == mFacePtr->adjacentEdgeEnd());

    mA = mVertexPtr0->position();
    mB = mVertexPtr1->position();
    mC = mVertexPtr2->position();

    if (((mB - mA).cross(mC - mA)).length() == 0.0) {
        // The area of the triangle is exactly zero.
        return false;
    }

    mFaceNormal = (mA - mC).cross(mB - mC).normalized();

    // Ensure that the input triangle vertices are defined in a consistent order,
    // even if we visit shared edges repeatedly via multiple adjacent triangles.
    // This should improve floating point robustness.
    //
    //    0
    //b1----a0
    //  \  /
    // 1 \/ 2
    //   c2
    //
    if (mB < mA) {
        mA.swap(mB);
        std::swap(mVertexPtr0, mVertexPtr1);
        std::swap(mEdgePtr1, mEdgePtr2);
    }
    if (mC < mA) {
        mC.swap(mA);
        std::swap(mVertexPtr2, mVertexPtr0);
        std::swap(mEdgePtr0, mEdgePtr1);
    }
    if (mC < mB) {
        mC.swap(mB);
        std::swap(mVertexPtr2, mVertexPtr1);
        std::swap(mEdgePtr0, mEdgePtr2);
    }

    assert(!(mB < mA));
    assert(!(mC < mA));
    assert(!(mC < mB));

    mOrientationA = exact::TestOrientation3d(mV, mP, mQ, mA);
    mOrientationB = exact::TestOrientation3d(mV, mP, mQ, mB);
    mOrientationC = exact::TestOrientation3d(mV, mP, mQ, mC);

    return true;
}

bool
WedgeIntersector::triangleIntersectsWedgePlane()
{
    // If points A, B and C all lie on the same side of wedge VPQ,
    // then there is no intersection.
    if (mOrientationA < 0.0
        && mOrientationB < 0.0
        && mOrientationC < 0.0) {
        return false;
    }
    if (mOrientationA > 0.0
        && mOrientationB > 0.0
        && mOrientationC > 0.0) {
        return false;
    }

    // If the triangle is coplanar with the wedge,
    // we're not considering this an intersection,
    // because the shadow of the occluder can't cast a critical line segment
    // onto the triangle in this case.
    if (mOrientationA == 0.0
        && mOrientationB == 0.0
        && mOrientationC == 0.0) {
        return false;
    }

    return true;
}

bool
WedgeIntersector::triangleIsFrontfacing()
{
    cgmath::Vector3f closestPoint;

    switch (mEventType) {
    case VE_EVENT:
        return (mV - mA).dot(mFaceNormal) > 0.0;
        break;
    case EV_EVENT:
    case DISTANT_LIGHT_EV_EVENT:
        if (!GetClosestPointOnPlane(mA, mFaceNormal, mV, &closestPoint)) {
            return false;
        }
        // (The +Y axis points toward the light source.)
        if ((mV - closestPoint).dot(mWedgePositiveYAxis) > 0.0) {
            // The face is on the far side of V with respect to the light source.
            return (mV - mA).dot(mFaceNormal) > 0.0;
        } else {
            // The face is on the near side of V with respect to the light source.
            return (mP - mA).dot(mFaceNormal) > 0.0
                || (mQ - mA).dot(mFaceNormal) > 0.0;
        }
        break;
    case DISTANT_LIGHT_EE_EVENT:
        return (mV - mP).dot(mFaceNormal) > 0.0;
        break;
    }

    return true;
}

void
WedgeIntersector::testTriangleVertices()
{
    // If any of the points of the triangle lie in the plane of the wedge,
    // handle those cases explicitly.
    if (mOrientationA == 0.0) {
        mD = mA;
        mDIsDefined = true;
        mId0.setVertexPtr(mVertexPtr0);
    } 
    if (mOrientationB == 0.0) {
        if (mDIsDefined) {
            mE = mB;
            mEIsDefined = true;
            mId1.setVertexPtr(mVertexPtr1);
        } else {
            mD = mB;
            mDIsDefined = true;
            mId0.setVertexPtr(mVertexPtr1);
        }
    }
    if (mOrientationC == 0.0) {
        if (mDIsDefined) {
            assert(!mEIsDefined);
            mE = mC;
            mEIsDefined = true;
            mId1.setVertexPtr(mVertexPtr2);
        } else {
            mD = mC;
            mDIsDefined = true;
            mId0.setVertexPtr(mVertexPtr2);
        }
    }
}

void
WedgeIntersector::testTriangleEdges()
{
    // Compute the intersection of the triangle edges with the wedge.
    if (!mDIsDefined || !mEIsDefined) {

        if ((mOrientationA < 0.0
                && mOrientationB > 0.0)
            || (mOrientationA > 0.0
                && mOrientationB < 0.0)) {
            // Compute the intersection point of line segment AB with the wedge.
            cgmath::Vector3f intersectionPoint = cgmath::Vector3f::ZERO;
            if (cgmath::GetIntersectionOfLineWithPlane(
                    mA, mB, mV, mWedgePositiveZAxis, &intersectionPoint)) {
                if (!mDIsDefined) {
                    mD = intersectionPoint;
                    mDIsDefined = true;
                    mId0.setEdgePtrAndIndex(mEdgePtr0, mWedgeIdentifier);
                } else {
                    assert(!mEIsDefined);
                    mE = intersectionPoint;
                    mEIsDefined = true;
                    mId1.setEdgePtrAndIndex(mEdgePtr0, mWedgeIdentifier);
                }
            }
        }

        if ((mOrientationB < 0.0
                && mOrientationC > 0.0)
            || (mOrientationB > 0.0
                && mOrientationC < 0.0)) {
            // Compute the intersection point of line segment BC with the wedge.
            cgmath::Vector3f intersectionPoint = cgmath::Vector3f::ZERO;
            if (cgmath::GetIntersectionOfLineWithPlane(
                    mB, mC, mV, mWedgePositiveZAxis, &intersectionPoint)) {
                if (!mDIsDefined) {
                    mD = intersectionPoint;
                    mDIsDefined = true;
                    mId0.setEdgePtrAndIndex(mEdgePtr1, mWedgeIdentifier);
                } else {
                    assert(!mEIsDefined);
                    mE = intersectionPoint;
                    mEIsDefined = true;
                    mId1.setEdgePtrAndIndex(mEdgePtr1, mWedgeIdentifier);
                }
            }
        }

        if ((mOrientationC < 0.0
                && mOrientationA > 0.0)
            || (mOrientationC > 0.0
                && mOrientationA < 0.0)) {
            // Compute the intersection point of line segment AC with the wedge.
            cgmath::Vector3f intersectionPoint = cgmath::Vector3f::ZERO;
            if (cgmath::GetIntersectionOfLineWithPlane(
                    mA, mC, mV, mWedgePositiveZAxis, &intersectionPoint)) {
                if (!mDIsDefined) {
                    mD = intersectionPoint;
                    mDIsDefined = true;
                    mId0.setEdgePtrAndIndex(mEdgePtr2, mWedgeIdentifier);
                } else {
                    assert(!mEIsDefined);
                    mE = intersectionPoint;
                    mEIsDefined = true;
                    mId1.setEdgePtrAndIndex(mEdgePtr2, mWedgeIdentifier);
                }
            }
        }
    }
}

void
WedgeIntersector::clipToVeWedge(const ClippableEndpoint &inputCe0, 
    const ClippableEndpoint &inputCe1)
{
    ClippableEndpoint ce0 = inputCe0;
    ClippableEndpoint ce1 = inputCe1;

    // Clip against line VP of the wedge.
    if (!clipToPlane(mV, mWedgePositiveZAxis.cross(mP - mV).normalized(),
            mEndpointIdentifierVP, 0.0, false, &ce0, &ce1)) {
        return;
    }

    // Clip against line WQ of the wedge.
    // (In the VE_EVENT case, V and W are coincident.)
    if (!clipToPlane(mW, (mQ - mW).cross(mWedgePositiveZAxis).normalized(),
            mEndpointIdentifierWQ, 1.0, false, &ce0, &ce1)) {
        return;
    }

    // Test if edge PQ lies in the plane of the face we're testing
    // for intersection with the wedge.
    // In that case, clipping against edge PQ would be ill-defined because of
    // floating point error. Furthermore, in that case, we must assume that
    // the resulting line segment lies below the occluder, so it is not
    // discarded by LineSegmentCollection::findVisibleLineSegments.
    // It's critical that these line segments are maintained, because they'll
    // happen when the base of an object is in contact with a larger surface
    // in situations when no connectivity is explicitly defined.
    // If we allowed these line segments to be discarded, critical edges
    // would be missing from the discontinuity mesh, resulting in unsightly artifacts.
    assert(mEdgePtrIsDefined);
    float epsilon = std::max(  
        mesh::GetEpsilonFromFace(mFacePtr),  
        mesh::GetEpsilonFromEdge(mEdgePtr));  
    const cgmath::Vector3f &planePoint = (*mFacePtr->adjacentVertexBegin())->position();  
    const cgmath::Vector3f planeNormal = mesh::GetFaceGeometricNormal(mFacePtr);  
    cgmath::Vector3f closestPointToP;  
    cgmath::Vector3f closestPointToQ;  
    if (GetClosestPointOnPlane(planePoint, planeNormal, mP, &closestPointToP)  
        && GetClosestPointOnPlane(planePoint, planeNormal, mQ, &closestPointToQ)) {  
        float distanceToP = (mP - closestPointToP).length();  
        float distanceToQ = (mQ - closestPointToQ).length();  
        if (distanceToP <= epsilon  
            && distanceToQ <= epsilon) {  
            ClippableEndpoint p0 = ce0;
            ClippableEndpoint p1 = ce1;
            p0.mIsDegreeZeroDiscontinuity = true;
            p1.mIsDegreeZeroDiscontinuity = true;
            snapClippableEndpointToPQ(&p0);
            snapClippableEndpointToPQ(&p1);
            recordClippableEndpointsAsLineSegment(p0, p1, BELOW_OCCLUDER);
            return;
        }  
    }

    // Everything above the occluder (inside triangle VPQ).
    ClippableEndpoint p0 = ce0;
    ClippableEndpoint p1 = ce1;
    if (clipToPlane(mP, mWedgePositiveZAxis.cross(mQ - mP).normalized(),
            mEndpointIdentifierPQ, Endpoint::UNDEFINED_VISIBILITY_PARAMETER, true, &p0, &p1)) {
        snapClippableEndpointToPQ(&p0);
        snapClippableEndpointToPQ(&p1);
        recordClippableEndpointsAsLineSegment(p0, p1, ABOVE_OCCLUDER);
    }

    // Everything below line PQ.
    p0 = ce0;
    p1 = ce1;
    if (clipToPlane(mP, -mWedgePositiveZAxis.cross(mQ - mP).normalized(),
            mEndpointIdentifierPQ, Endpoint::UNDEFINED_VISIBILITY_PARAMETER, true, &p0, &p1)) {
        snapClippableEndpointToPQ(&p0);
        snapClippableEndpointToPQ(&p1);
        recordClippableEndpointsAsLineSegment(p0, p1, BELOW_OCCLUDER);
    }
}

void
WedgeIntersector::clipToEvWedge(const ClippableEndpoint &inputCe0, 
    const ClippableEndpoint &inputCe1)
{
    // The top portion of the wedge, inside triangle VPQ.

    ClippableEndpoint ce0 = inputCe0;
    ClippableEndpoint ce1 = inputCe1;

    // Clip the line segment against line VP.
    assert(mVertexPtrIsDefined);
    if (clipToPlane(mV, mWedgePositiveZAxis.cross(mP - mV).normalized(),
            mEndpointIdentifierVP, 0.0, false, &ce0, &ce1)) {

        // Clip the line segment against line VQ.
        if (clipToPlane(mV, (mQ - mV).cross(mWedgePositiveZAxis).normalized(),
                mEndpointIdentifierWQ, 1.0, false, &ce0, &ce1)) {

            // Clip the line segment against the plane that passes through
            // edge PQ, which is at the top of the wedge (the light source edge).
            if (clipToPlane(mP, mWedgePositiveZAxis.cross(mQ - mP).normalized(),
                    mEndpointIdentifierPQ, 
                    Endpoint::UNDEFINED_VISIBILITY_PARAMETER, false, &ce0, &ce1)) {

                snapClippableEndpointToV(&ce0);
                snapClippableEndpointToV(&ce1);
                recordClippableEndpointsAsLineSegment(ce0, ce1, ABOVE_OCCLUDER);
            }
        }
    }

    // The bottom portion of the wedge.

    ce0 = inputCe0;
    ce1 = inputCe1;

    // Clip the line segment against line VP.
    if (clipToPlane(mV, (mP - mV).cross(mWedgePositiveZAxis).normalized(),
            mEndpointIdentifierVP, 0.0, false, &ce0, &ce1)) {

        // Clip the line segment against line VQ.
        if (clipToPlane(mV, mWedgePositiveZAxis.cross(mQ - mV).normalized(),
                mEndpointIdentifierWQ, 1.0, false, &ce0, &ce1)) {

            snapClippableEndpointToV(&ce0);
            snapClippableEndpointToV(&ce1);
            recordClippableEndpointsAsLineSegment(ce0, ce1, BELOW_OCCLUDER);
        }
    }
}

bool
WedgeIntersector::clipToPlane(const cgmath::Vector3f &origin, const cgmath::Vector3f &normal,
    const meshretri::EndpointIdentifier &newIdIfClipped, float newVisibilityParameterIfClipped,
    bool markIsDegreeZeroDiscontinuityIfClipped, ClippableEndpoint *ep0, ClippableEndpoint *ep1)
{
    cgmath::Vector3f oldP0 = ep0->mPoint;
    cgmath::Vector3f oldP1 = ep1->mPoint;

    if (!cgmath::ClipLineSegmentAgainstPlane(&ep0->mPoint, &ep1->mPoint, origin, normal)) {
        // The line segment is entirely clipped away because both points lie
        // behind the backside of the plane.
        return false;
    }

    if (ep0->mPoint != oldP0) {
        ep0->mId = newIdIfClipped;
        ep0->mVisibilityParameter = newVisibilityParameterIfClipped;
        if (markIsDegreeZeroDiscontinuityIfClipped) {
            ep0->mIsDegreeZeroDiscontinuity = true;
        }
    }

    if (ep1->mPoint != oldP1) {
        ep1->mId = newIdIfClipped;
        ep1->mVisibilityParameter = newVisibilityParameterIfClipped;
        if (markIsDegreeZeroDiscontinuityIfClipped) {
            ep1->mIsDegreeZeroDiscontinuity = true;
        }
    }

    return true;
}

void
WedgeIntersector::recordClippableEndpointsAsLineSegment(const ClippableEndpoint &ce0,
    const ClippableEndpoint &ce1, PositionRelativeToOccluder positionRelativeToOccluder)
{
    // Only record the resulting clipped edge if it's not degenerate.
    if (ce0.mPoint == ce1.mPoint) {
        return;
    }

    cgmath::Vector2f ep0;
    transformWorldSpacePointToWedgeSpacePoint(ce0.mPoint, &ep0);

    cgmath::Vector2f ep1;
    transformWorldSpacePointToWedgeSpacePoint(ce1.mPoint, &ep1);

    float vp0 = ce0.mVisibilityParameter;
    if (vp0 == Endpoint::UNDEFINED_VISIBILITY_PARAMETER) {
        vp0 = projectPointOntoEdgePQ(ce0.mPoint);
    }

    float vp1 = ce1.mVisibilityParameter;
    if (vp1 == Endpoint::UNDEFINED_VISIBILITY_PARAMETER) {
        vp1 = projectPointOntoEdgePQ(ce1.mPoint);
    }

    Endpoint point0;
    point0.setWorldPosition(ce0.mPoint);
    point0.setWedgePosition(ep0);
    point0.setVisibilityParameter(vp0);
    point0.setIsDegreeZeroDiscontinuity(ce0.mIsDegreeZeroDiscontinuity);
    point0.setEndpointIdentifier(ce0.mId);

    Endpoint point1;
    point1.setWorldPosition(ce1.mPoint);
    point1.setWedgePosition(ep1);
    point1.setVisibilityParameter(vp1);
    point1.setIsDegreeZeroDiscontinuity(ce1.mIsDegreeZeroDiscontinuity);
    point1.setEndpointIdentifier(ce1.mId);

    assert(mLineSegmentCount < 2);
    mLineSegmentArray[mLineSegmentCount].setPoint0(point0);
    mLineSegmentArray[mLineSegmentCount].setPoint1(point1);
    mLineSegmentArray[mLineSegmentCount].setFacePtr(mFacePtr);
    mLineSegmentArray[mLineSegmentCount].setIsAboveOccluder(
        positionRelativeToOccluder == ABOVE_OCCLUDER);

    ++mLineSegmentCount;
}

void
WedgeIntersector::snapClippableEndpointToPQ(ClippableEndpoint *p)
{
    float epsilon = std::max(p->mPoint.length(), mP.length())*cgmath::TOLERANCE;
    if (epsilon == 0.0) {
        epsilon = cgmath::TOLERANCE;
    }
    if (p->mPoint.equivalent(mP, epsilon)) {
        p->mPoint = mP;
        p->mId.setVertexPtr(mWedgeVertexP);
        p->mVisibilityParameter = 0.0;
    }

    epsilon = std::max(p->mPoint.length(), mQ.length())*cgmath::TOLERANCE;
    if (epsilon == 0.0) {
        epsilon = cgmath::TOLERANCE;
    }
    if (p->mPoint.equivalent(mQ, epsilon)) {
        p->mPoint = mQ;
        p->mId.setVertexPtr(mWedgeVertexQ);
        p->mVisibilityParameter = 1.0;
    }
}

void
WedgeIntersector::snapClippableEndpointToV(ClippableEndpoint *p)
{
    float epsilon = std::max(p->mPoint.length(), mV.length())*cgmath::TOLERANCE;
    if (epsilon == 0.0) {
        epsilon = cgmath::TOLERANCE;
    }
    if (p->mPoint.equivalent(mV, epsilon)) {
        p->mPoint = mV;

        // If this assignment is included, it makes a unit test fail.
#if 0
        p->mId = meshretri::EndpointIdentifier::fromVertexPtr(mVertexPtr);
#endif

        p->mVisibilityParameter = 0.0;
    }
}
