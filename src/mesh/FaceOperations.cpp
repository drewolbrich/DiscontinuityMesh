// Copyright 2008 Drew Olbrich

#include "FaceOperations.h"

#include <cmath>
#include <cassert>
#include <limits>
#include <algorithm>

#include <cgmath/LineOperations.h>
#include <cgmath/Vector3fOperations.h>
#include <cgmath/Tolerance.h>
#include <cgmath/Vector3d.h>

#include "EdgeOperations.h"
#include "VertexOperations.h"
#include "DeletedElementTracker.h"

using cgmath::Vector2f;
using cgmath::Vector3f;
using cgmath::Vector3d;

namespace mesh {

void 
DeleteFace(Mesh *mesh, FacePtr facePtr, DeletedElementTracker *deletedElementTracker)
{
    for (AdjacentVertexIterator iterator = facePtr->adjacentVertexBegin();
         iterator != facePtr->adjacentVertexEnd(); ++iterator) {
        VertexPtr vertexPtr = *iterator;

        // The adjacent vertex must refer back to the face.
        assert(vertexPtr->hasAdjacentFace(facePtr));
            
        vertexPtr->removeAdjacentFace(facePtr);
    }
    
    for (AdjacentEdgeIterator iterator = facePtr->adjacentEdgeBegin();
         iterator != facePtr->adjacentEdgeEnd(); ++iterator) {
        EdgePtr edgePtr = *iterator;

        // The adjacent edge must refer back to the face.
        assert(edgePtr->hasAdjacentFace(facePtr));
            
        edgePtr->removeAdjacentFace(facePtr);
    }

    mesh->destroyFace(facePtr);

    // Track the deleted face
    if (deletedElementTracker != NULL) {
        deletedElementTracker->addFace(facePtr);
    }
}

bool 
FaceIsDegenerate(ConstFacePtr facePtr, float epsilon)
{
    // Faces with less than three vertices are degenerate.
    if (facePtr->adjacentVertexCount() < 3) {
        return true;
    }

    // If any of the vertices are within epsilon units of any other vertices,
    // the face is degenerate.
    for (AdjacentVertexConstIterator iterator = facePtr->adjacentVertexBegin();
         iterator != facePtr->adjacentVertexEnd(); ++iterator) {
        VertexPtr v0 = *iterator;
        AdjacentVertexConstIterator secondaryIterator = iterator;
        ++secondaryIterator;
        for (; secondaryIterator != facePtr->adjacentVertexEnd(); ++secondaryIterator) {
            VertexPtr v1 = *secondaryIterator;
            if ((v0->position() - v1->position()).length() <= epsilon) {
                return true;
            }
        }
    }

    // If any of the vertices are within epsilon units of any non-adjacent edges,
    // the face is degenerate.
    for (AdjacentVertexConstIterator iterator = facePtr->adjacentVertexBegin();
         iterator != facePtr->adjacentVertexEnd(); ++iterator) {
        VertexPtr v0 = *iterator;
        for (AdjacentEdgeConstIterator iterator = facePtr->adjacentEdgeBegin();
             iterator != facePtr->adjacentEdgeEnd(); ++iterator) {
            EdgePtr e0 = *iterator;
            cgmath::Vector3f q0;
            cgmath::Vector3f q1;
            GetEdgeVertexPositions(e0, &q0, &q1);
            if (!v0->hasAdjacentEdge(e0)
                && cgmath::GetDistanceFromLineSegmentToPoint3f(q0, q1, v0->position())
                <= epsilon) {
                return true;
            }
        }
    }

    return false;
}

bool
ProjectedFaceIsDegenerate(ConstFacePtr facePtr, int axis1, int axis2, float epsilon)
{
    if (facePtr->adjacentVertexCount() < 3) {
        return true;
    }
    
    // If any of the vertices are within epsilon units of any other vertices,
    // the face is degenerate.
    for (AdjacentVertexConstIterator iterator = facePtr->adjacentVertexBegin();
         iterator != facePtr->adjacentVertexEnd(); ++iterator) {
        VertexPtr v0 = *iterator;
        AdjacentVertexConstIterator secondaryIterator = iterator;
        ++secondaryIterator;
        for (; secondaryIterator != facePtr->adjacentVertexEnd(); ++secondaryIterator) {
            VertexPtr v1 = *secondaryIterator;
            if (cgmath::Vector2f(v0->position(), axis1, axis2)
                .equivalent(cgmath::Vector2f(v1->position(), axis1, axis2), epsilon)) {
                return true;
            }
        }
    }

    // If any of the vertices are within epsilon units of any non-adjacent edges,
    // the face is degenerate.
    for (AdjacentVertexConstIterator iterator = facePtr->adjacentVertexBegin();
         iterator != facePtr->adjacentVertexEnd(); ++iterator) {
        VertexPtr v0 = *iterator;
        for (AdjacentEdgeConstIterator iterator = facePtr->adjacentEdgeBegin();
             iterator != facePtr->adjacentEdgeEnd(); ++iterator) {
            EdgePtr e0 = *iterator;
            cgmath::Vector3f q0;
            cgmath::Vector3f q1;
            GetEdgeVertexPositions(e0, &q0, &q1);
            if (!v0->hasAdjacentEdge(e0)
                && cgmath::GetDistanceFromLineSegmentToPoint2f(
                    cgmath::Vector2f(q0, axis1, axis2), 
                    cgmath::Vector2f(q1, axis1, axis2),
                    cgmath::Vector2f(v0->position(), axis1, axis2))
                <= epsilon) {
                return true;
            }
        }
    }

    return false;
}

bool 
FaceIsNonplanar(ConstFacePtr facePtr, float epsilon)
{
    if (facePtr->adjacentVertexCount() < 4) {
        return false;
    }

    Vector3f normal = GetFaceGeometricNormal(facePtr);

    AdjacentVertexConstIterator iterator = facePtr->adjacentVertexBegin();
    VertexPtr v0 = *iterator;
    ++iterator;
    while (iterator != facePtr->adjacentVertexEnd()) {
        VertexPtr v1 = *iterator;

        // If we project the vertex against the normal vector,
        // it should wind up very close to the first vertex.
        // If it doesn't, the face is not planar.
        Vector3f dx = v1->position() - v0->position();
        if (fabsf(dx.dot(normal)) > epsilon) {
            return true;
        }
        
        ++iterator;
    }

    return false;
}

bool 
FaceIsSelfIntersecting(ConstFacePtr facePtr)
{
    // Determine which two axes to use to check
    // for self-intersecting edges. Line segment
    // intersection is a 2D test.
    unsigned axis0 = 0;
    unsigned axis1 = 0;
    cgmath::GetPrimaryAxesMostOrthogonalToVector(GetFaceGeometricNormal(facePtr), 
        &axis0, &axis1);

    for (AdjacentVertexConstIterator iterator = facePtr->adjacentVertexBegin();
         iterator != facePtr->adjacentVertexEnd(); ++iterator) {

        VertexPtr v0 = *iterator;

        // Get the next vertex.
        AdjacentVertexConstIterator nextIterator = iterator;
        ++nextIterator;
        if (nextIterator == facePtr->adjacentVertexEnd()) {
            nextIterator = facePtr->adjacentVertexBegin();
        }
        VertexPtr v1 = *nextIterator;

        // Loop over all the other edges to see if the
        // edge intersects with any of them.
        for (AdjacentVertexConstIterator secondaryIterator = facePtr->adjacentVertexBegin();
             secondaryIterator != facePtr->adjacentVertexEnd(); 
             ++secondaryIterator) {

            VertexPtr v2 = *secondaryIterator;
            
            // Get the next vertex.
            AdjacentVertexConstIterator nextSecondaryIterator = secondaryIterator;
            ++nextSecondaryIterator;
            if (nextSecondaryIterator == facePtr->adjacentVertexEnd()) {
                nextSecondaryIterator = facePtr->adjacentVertexBegin();
            }
            VertexPtr v3 = *nextSecondaryIterator;

            // Don't check for the intersection of an edge
            // against itself, or against adjacent edges,
            // since these always intersect at their
            // shared vertices.
            if (nextIterator == secondaryIterator
                || nextIterator == nextSecondaryIterator
                || iterator == secondaryIterator
                || iterator == nextSecondaryIterator) {
                continue;
            }
            
            // The intersection test is performed in 2D.
            if (cgmath::GetIntersectionOfTwoLineSegments(
                    Vector2f(v0->position(), axis0, axis1),
                    Vector2f(v1->position(), axis0, axis1),
                    Vector2f(v2->position(), axis0, axis1),
                    Vector2f(v3->position(), axis0, axis1),
                    NULL)) {
                return true;
            }
        }        
    }

    return false;
}

Vector3f
GetFaceGeometricNormal(ConstFacePtr facePtr)
{
    // If the face has less than three vertices, then the
    // normal is ill-defined.
    if (facePtr->adjacentVertexCount() < 3) {
        return Vector3f::ZERO;
    }

    // If the face has three vertices, just compute the
    // cross product of two of the edges.
    if (facePtr->adjacentVertexCount() == 3) {
        AdjacentVertexConstIterator iterator = facePtr->adjacentVertexBegin();
        Vector3f p1 = (*iterator)->position();
        ++iterator;
        Vector3f p2 = (*iterator)->position();
        ++iterator;
        Vector3f p3 = (*iterator)->position();

        Vector3f result = (p2 - p1).cross(p3 - p1);

        if (result != Vector3f::ZERO) {
            result.normalize();
        }
        
        return result;
    }
    
    // If the face has four or more vertices, find the three vertices
    // that are farthest apart from each other, and use them to
    // determine the the geometric normal. This algorithm has O(N^3)
    // complexity, but it is robust.
    const Vector3f *maxP1 = NULL;
    const Vector3f *maxP2 = NULL;
    const Vector3f *maxP3 = NULL;
    float maxTotalDistance = -1.0;
    for (AdjacentVertexConstIterator iterator1 = facePtr->adjacentVertexBegin();
         iterator1 != facePtr->adjacentVertexEnd(); ++iterator1) {
        const Vector3f &p1 = (*iterator1)->position();

        AdjacentVertexConstIterator iterator2 = iterator1;
        for (++iterator2; iterator2 != facePtr->adjacentVertexEnd(); ++iterator2) {
            const Vector3f &p2 = (*iterator2)->position();
            
            AdjacentVertexConstIterator iterator3 = iterator2;
            for (++iterator3; iterator3 != facePtr->adjacentVertexEnd(); ++iterator3) {
                const Vector3f &p3 = (*iterator3)->position();
                
                float totalDistance = (p2 - p1).length()
                    + (p3 - p2).length() + (p1 - p3).length();
                if (totalDistance > maxTotalDistance) {
                    maxTotalDistance = totalDistance;
                    maxP1 = &p1;
                    maxP2 = &p2;
                    maxP3 = &p3;
                }
            }
        }
    }

    if (maxP1 != NULL
        && maxP2 != NULL
        && maxP3 != NULL) {

        Vector3f result = (*maxP2 - *maxP1).cross(*maxP3 - *maxP1);
        if (result != Vector3f::ZERO) {
            result.normalize();
        }

        return result;
    }

    return Vector3f::ZERO;
}

float
GetFaceArea(ConstFacePtr facePtr)
{
    Vector3f total(Vector3f::ZERO);
    
    for (AdjacentVertexConstIterator iterator = facePtr->adjacentVertexBegin();
         iterator != facePtr->adjacentVertexEnd(); ++iterator) {
        
        AdjacentVertexConstIterator nextIterator = iterator;
        ++nextIterator;
        if (nextIterator == facePtr->adjacentVertexEnd()) {
            nextIterator = facePtr->adjacentVertexBegin();
        }

        const ConstVertexPtr v0(*iterator);
        const ConstVertexPtr v1(*nextIterator);
        total += v0->position().cross(v1->position());
    }

    return GetFaceGeometricNormal(facePtr).dot(total)*0.5;
}

float
GetProjectedFaceArea(ConstFacePtr facePtr, int axis1, int axis2)
{
    float total = 0.0;
    
    for (AdjacentVertexConstIterator iterator = facePtr->adjacentVertexBegin();
         iterator != facePtr->adjacentVertexEnd(); ++iterator) {
        
        AdjacentVertexConstIterator nextIterator = iterator;
        ++nextIterator;
        if (nextIterator == facePtr->adjacentVertexEnd()) {
            nextIterator = facePtr->adjacentVertexBegin();
        }

        const ConstVertexPtr v0(*iterator);
        const ConstVertexPtr v1(*nextIterator);
        const Vector2f x0(v0->position(), axis1, axis2);
        const Vector2f x1(v1->position(), axis1, axis2);
        total += x0[0]*x1[1] - x1[0]*x0[1];
    }

    return fabsf(total*0.5);
}

Vector3f 
GetFaceVertexNormal(ConstFacePtr facePtr, ConstVertexPtr vertexPtr,
    const AttributeKey &normal3fKey)
{
    if (facePtr->hasVertexAttribute(vertexPtr, normal3fKey)) {
        return facePtr->getVertexUnitVector3f(vertexPtr, normal3fKey);
    } else if (facePtr->hasAttribute(normal3fKey)) {
        return facePtr->getUnitVector3f(normal3fKey);
    } else {
        return GetFaceGeometricNormal(facePtr);
    }
}

Vector3f 
GetFaceCentroid(ConstFacePtr facePtr)
{
    Vector3f normal = GetFaceGeometricNormal(facePtr);

    int axis1;
    int axis2;
    int axis3;
    if (fabsf(normal[0]) > fabsf(normal[1]) 
        && fabsf(normal[0]) > fabsf(normal[2])) {
        axis1 = 1;
        axis2 = 2;
        axis3 = 0;
    } else if (fabsf(normal[1]) > fabsf(normal[0]) 
        && fabsf(normal[1]) > fabsf(normal[2])) {
        axis1 = 2;
        axis2 = 0;
        axis3 = 1;
    } else {
        axis1 = 0;
        axis2 = 1;
        axis3 = 2;
    }

    Vector3f result = Vector3f::ZERO;
    float area = 0.0;
    for (AdjacentVertexConstIterator iterator = facePtr->adjacentVertexBegin();
         iterator != facePtr->adjacentVertexEnd(); ++iterator) {
        VertexPtr vertexPtr = *iterator;
        AdjacentVertexConstIterator secondaryIterator = iterator;
        ++secondaryIterator;
        if (secondaryIterator == facePtr->adjacentVertexEnd()) {
            secondaryIterator = facePtr->adjacentVertexBegin();
        }
        VertexPtr secondaryVertexPtr = *secondaryIterator;

        float f = vertexPtr->position()[axis1]
            *secondaryVertexPtr->position()[axis2]
            - secondaryVertexPtr->position()[axis1]
            *vertexPtr->position()[axis2];

        result[axis1] += (vertexPtr->position()[axis1]
            + secondaryVertexPtr->position()[axis1])*f;
        result[axis2] += (vertexPtr->position()[axis2]
            + secondaryVertexPtr->position()[axis2])*f;
        
        area += f;
    }

    area *= 0.5;
    
    result[axis1] /= 6.0*area;
    result[axis2] /= 6.0*area;

    const Vector3f &point = (*facePtr->adjacentVertexBegin())->position();

    float d = normal[axis1]*point[axis1]
        + normal[axis2]*point[axis2]
        + normal[axis3]*point[axis3];

    if (normal[axis3] != 0.0) {
        result[axis3] = (d - normal[axis1]*result[axis1]
            - normal[axis2]*result[axis2])/normal[axis3];
    } else {
        result[axis3] = point[axis3];
    }

    // Check to see that the vector does not contain infinity.
    assert(result[0]*0.0 == 0.0);
    assert(result[1]*0.0 == 0.0);
    assert(result[2]*0.0 == 0.0);
    
    return result;
}

cgmath::Vector3f
GetFaceAverageVertexPosition(ConstFacePtr facePtr)
{
    Vector3f result = Vector3f::ZERO;
    for (AdjacentVertexConstIterator iterator = facePtr->adjacentVertexBegin();
         iterator != facePtr->adjacentVertexEnd(); ++iterator) {
        VertexPtr vertexPtr = *iterator;
        result += vertexPtr->position();
    }

    result /= facePtr->adjacentVertexCount();

    return result;
}

bool 
RaySegmentIntersectsFace(ConstFacePtr facePtr, const cgmath::Vector3f &origin, 
    const cgmath::Vector3f &endpoint, float *t)
{
    // This code is derived from Graphics Gems I, p. 360 and 735,
    // "An Efficient Ray-Polygon Intersection".

    const Vector3d direction = Vector3d(endpoint) - Vector3d(origin);

    Vector3d normal = Vector3d(GetFaceGeometricNormal(facePtr));

    // Use the position of the first vertex as our reference
    // point for the plane that the polygon lies in.
    Vector3d point = Vector3d((*facePtr->adjacentVertexBegin())->position());

    // Determine the two axes defining the axis-aligned plane
    // that the polygon normal is most perpendicular to.
    int axis1;
    int axis2;
    if (fabsf(normal[2]) > fabsf(normal[0])
        && fabsf(normal[2]) > fabsf(normal[1])) {
        axis1 = 0;
        axis2 = 1;
    } else if (fabsf(normal[1]) > fabsf(normal[0])
        && fabsf(normal[1]) > fabsf(normal[2])) {
        axis1 = 2;
        axis2 = 0;
    } else {
        axis1 = 1;
        axis2 = 2;
    }

    double d = -(point.dot(normal));

    double divisor = normal.dot(direction);

    // If the ray is parallel to the plane of the polygon,
    // they do not intersect.
    if (divisor == 0.0) {
        return false;
    }

    // Compute the parameter of the intersection point of the
    // ray with the polygon.
    double s = -(d + normal.dot(Vector3d(origin)))/divisor;

    // If the parameter of the intersection point occurs
    // before the starting point of the ray, it does not intersect it.
    if (s < 0.0) {
        return false;
    }

    // If the parameter of the intersection point occurs
    // beyond the end point of the ray, it does not intersect it.
    if (s > 1.0) {
        return false;
    }

    double p1 = origin[axis1] + direction[axis1]*s;
    double p2 = origin[axis2] + direction[axis2]*s;

    // The code below assumes that the face has three or more vertices.
    assert(facePtr->adjacentVertexCount() > 2);
    
    AdjacentVertexConstIterator iterator = facePtr->adjacentVertexBegin();
    AdjacentVertexConstIterator nextIterator = iterator;
    ++nextIterator;

    const VertexPtr &startVertexPtr = *iterator;
    const Vector3d startPosition = Vector3d(startVertexPtr->position());
    
    double u0 = p1 - startPosition[axis1];
    double v0 = p2 - startPosition[axis2];
    bool intersected = false;
    double alpha = 0.0;
    double beta = 0.0;
    double u1 = 0.0;
    double v1 = 0.0;
    double u2 = 0.0;
    double v2 = 0.0;
    
    ++iterator;
    ++nextIterator;

    assert(iterator != facePtr->adjacentVertexEnd());
    assert(nextIterator != facePtr->adjacentVertexEnd());
    
    do {
        const VertexPtr &vertexPtr = *iterator;
        const VertexPtr &nextVertexPtr = *nextIterator;

        const Vector3f &position = vertexPtr->position();
        const Vector3f &nextPosition = nextVertexPtr->position();

        u1 = position[axis1] - startPosition[axis1];
        v1 = position[axis2] - startPosition[axis2];
        u2 = nextPosition[axis1] - startPosition[axis1];
        v2 = nextPosition[axis2] - startPosition[axis2];

        if (u1 == 0.0) {
            beta = u0/u2;
            if (beta >= 0.0 && beta <= 1.0) {
                alpha = (v0 - beta*v2)/v1;
                intersected = alpha >= 0.0 && alpha + beta <= 1.0;
            }
        } else {
            beta = (v0*u1 - u0*v1)/(v2*u1 - u2*v1);
            if (beta >= 0.0 && beta <= 1.0) {
                alpha = (u0 - beta*u2)/u1;
                intersected = alpha >= 0.0 && alpha + beta <= 1.0;
            }
        }

        ++iterator;
        ++nextIterator;
    } while (!intersected 
        && nextIterator != facePtr->adjacentVertexEnd());

    if (t != NULL) {
        *t = float(s);
    }

    return intersected;
}

float
GetEpsilonFromFace(ConstFacePtr facePtr, float absoluteTolerance, float relativeTolerance)
{
    float epsilon = 0.0;
    for (AdjacentVertexConstIterator iterator = facePtr->adjacentVertexBegin();
         iterator != facePtr->adjacentVertexEnd(); ++iterator) {
        ConstVertexPtr vertexPtr = *iterator;
        epsilon = std::max(epsilon, vertexPtr->position().maxAbs()*relativeTolerance);
    }

    return std::max(epsilon, absoluteTolerance);
}

cgmath::BoundingBox3f
GetFaceBoundingBox(ConstFacePtr facePtr)
{
    cgmath::BoundingBox3f boundingBox;
    boundingBox.reset();
    for (AdjacentVertexConstIterator iterator = facePtr->adjacentVertexBegin();
         iterator != facePtr->adjacentVertexEnd(); ++iterator) {
        ConstVertexPtr vertexPtr = *iterator;
        boundingBox.extendByVector3f(vertexPtr->position());
    }

    return boundingBox;
}

void
GetTriangularFaceAdjacentVertices(ConstFacePtr facePtr,
    VertexPtr *v0, VertexPtr *v1, VertexPtr *v2)
{
    // The face must be a triangle.
    assert(facePtr->adjacentVertexCount() == 3);

    AdjacentVertexConstIterator iterator = facePtr->adjacentVertexBegin();
    *v0 = *iterator;
    ++iterator;
    *v1 = *iterator;
    ++iterator;
    *v2 = *iterator;
#ifdef DEBUG
    ++iterator;
    assert(iterator == facePtr->adjacentVertexEnd());
#endif
}

void
GetTriangularFaceAdjacentEdges(ConstFacePtr facePtr,
    EdgePtr *e0, EdgePtr *e1, EdgePtr *e2)
{
    // The face must be a triangle.
    assert(facePtr->adjacentEdgeCount() == 3);

    AdjacentEdgeConstIterator iterator = facePtr->adjacentEdgeBegin();
    *e0 = *iterator;
    ++iterator;
    *e1 = *iterator;
    ++iterator;
    *e2 = *iterator;
#ifdef DEBUG
    ++iterator;
    assert(iterator == facePtr->adjacentEdgeEnd());
#endif
}

void
GetTriangularFaceVertexPositions(ConstFacePtr facePtr,
    cgmath::Vector3f *p0, cgmath::Vector3f *p1, cgmath::Vector3f *p2)
{
    // The face must be a triangle.
    assert(facePtr->adjacentVertexCount() == 3);

    AdjacentVertexConstIterator iterator = facePtr->adjacentVertexBegin();
    *p0 = (*iterator)->position();
    ++iterator;
    *p1 = (*iterator)->position();
    ++iterator;
    *p2 = (*iterator)->position();
#ifdef DEBUG
    ++iterator;
    assert(iterator == facePtr->adjacentVertexEnd());
#endif
}

void
GetTriangularFaceOtherEdges(ConstFacePtr facePtr,
    ConstEdgePtr edgePtr, EdgePtr *otherEdgePtr1, EdgePtr *otherEdgePtr2)
{
    // The face must be a triangle.
    assert(facePtr->adjacentVertexCount() == 3);

    assert(facePtr->hasAdjacentEdge(edgePtr));

    size_t edgeIndex = 0;
    for (AdjacentEdgeConstIterator iterator = facePtr->adjacentEdgeBegin();
         iterator != facePtr->adjacentEdgeEnd(); ++iterator) {
        EdgePtr candidateEdgePtr = *iterator;
        if (candidateEdgePtr != edgePtr) {
            assert(edgeIndex < 2);
            if (edgeIndex == 0) {
                *otherEdgePtr1 = candidateEdgePtr;
            } else {
                *otherEdgePtr2 = candidateEdgePtr;
            }
            ++edgeIndex;
        }
    }

    assert(edgeIndex == 2);
}

void
GetTriangularFaceOtherEdge(ConstFacePtr facePtr,
    ConstEdgePtr edgePtr1, ConstEdgePtr edgePtr2, EdgePtr *otherEdgePtr)
{
    // The face must be a triangle.
    assert(facePtr->adjacentVertexCount() == 3);

    assert(facePtr->hasAdjacentEdge(edgePtr1));
    assert(facePtr->hasAdjacentEdge(edgePtr2));

    for (AdjacentEdgeConstIterator iterator = facePtr->adjacentEdgeBegin();
         iterator != facePtr->adjacentEdgeEnd(); ++iterator) {
        EdgePtr candidateEdgePtr = *iterator;
        if (candidateEdgePtr != edgePtr1
            && candidateEdgePtr != edgePtr2) {
            *otherEdgePtr = candidateEdgePtr;
            return;
        }
    }

    // We should never reach this point.
    assert(0);
}

void
GetTriangularFaceEdgeOpposingVertex(ConstFacePtr facePtr,
    ConstVertexPtr vertexPtr, EdgePtr *opposingEdgePtr)
{
    // The face must be a triangle.
    assert(facePtr->adjacentVertexCount() == 3);

    assert(facePtr->hasAdjacentVertex(vertexPtr));

    for (AdjacentEdgeConstIterator iterator = facePtr->adjacentEdgeBegin();
         iterator != facePtr->adjacentEdgeEnd(); ++iterator) {
        EdgePtr candidateEdgePtr = *iterator;
        if (!candidateEdgePtr->hasAdjacentVertex(vertexPtr)) {
            *opposingEdgePtr = candidateEdgePtr;
            return;
        }
    }

    // We should never reach this point.
    assert(0);
}

FacePtr
CreateTriangularFaceAndEdgesFromVertices(Mesh *mesh,
    VertexPtr vertexPtr0, VertexPtr vertexPtr1, VertexPtr vertexPtr2)
{
    FacePtr facePtr = mesh->createFace();

    // Wire up the vertices to the face.
    facePtr->addAdjacentVertex(vertexPtr0);
    vertexPtr0->addAdjacentFace(facePtr);
    facePtr->addAdjacentVertex(vertexPtr1);
    vertexPtr1->addAdjacentFace(facePtr);
    facePtr->addAdjacentVertex(vertexPtr2);
    vertexPtr2->addAdjacentFace(facePtr);

    // Create the edges that do not already exist.
    VertexPtr vertexPtrArray[3] = { vertexPtr0, vertexPtr1, vertexPtr2 };
    for (size_t index = 0; index < 3; ++index) {
        VertexPtr v0 = vertexPtrArray[index];
        VertexPtr v1 = vertexPtrArray[(index + 1) % 3];

        EdgePtr edgePtr;
        if (!FindEdgeConnectingVertices(v0, v1, &edgePtr)) {
            // No edge aleady exists between the two vertices, so create it.
            edgePtr = mesh->createEdge();

            // Wire up the edge to the vertices.
            edgePtr->addAdjacentVertex(v0);
            v0->addAdjacentEdge(edgePtr);
            edgePtr->addAdjacentVertex(v1);
            v1->addAdjacentEdge(edgePtr);
        }

        // Wire up the edges to the face.
        edgePtr->addAdjacentFace(facePtr);
        facePtr->addAdjacentEdge(edgePtr);
    }

    return facePtr;
}

bool
AllFaceVerticesAreCoincident(ConstFacePtr facePtr, float epsilon)
{
    cgmath::Vector3f averagePosition = GetFaceAverageVertexPosition(facePtr);

    for (AdjacentVertexConstIterator iterator = facePtr->adjacentVertexBegin();
         iterator != facePtr->adjacentVertexEnd(); ++iterator) {
        VertexPtr vertexPtr = *iterator;
        if ((vertexPtr->position() - averagePosition).length() > epsilon) {
            return false;
        }
    }

    return true;
}

VertexPtr
CollapseFace(Mesh *mesh, FacePtr facePtr, DeletedElementTracker *deletedElementTracker)
{
#ifdef DEBUG
    // Each of the faces adjacent to the face's edges must be triangles.
    // We don't currently handle any other case.
    for (AdjacentEdgeIterator iterator = facePtr->adjacentEdgeBegin();
         iterator != facePtr->adjacentEdgeEnd(); ++iterator) {
        EdgePtr edgePtr = *iterator;
        for (AdjacentFaceIterator iterator = edgePtr->adjacentFaceBegin();
             iterator != edgePtr->adjacentFaceEnd(); ++iterator) {
            FacePtr facePtr = *iterator;
            assert(facePtr->adjacentVertexCount() == 3);
        }
    }
#endif

    VertexPtr v0;
    VertexPtr v1;
    VertexPtr v2;
    GetTriangularFaceAdjacentVertices(facePtr, &v0, &v1, &v2);

    MergeVertexIntoVertex(mesh, v1, v0, deletedElementTracker);
    MergeVertexIntoVertex(mesh, v2, v0, deletedElementTracker);

    return v0;
}

void
TriangulateQuadrilateralFaceFromVertex(Mesh *mesh, FacePtr facePtr, VertexPtr vertexPtr,
    DeletedElementTracker *deletedElementTracker)
{
    // The face must be a quadrilateral.
    assert(facePtr->adjacentVertexCount() == 4);

    // Copy the four vertices adjacent to the face into an array.
    VertexPtr adjacentVertexArray[4];
    std::copy(facePtr->adjacentVertexBegin(), facePtr->adjacentVertexEnd(),
        adjacentVertexArray);

    // Find the index of the specified vertex in the array.
    size_t vertexIndex = 0;
    while (adjacentVertexArray[vertexIndex] != vertexPtr) {
        ++vertexIndex;
        assert(vertexIndex != 4);
    }

    // Create the two triangles.
    FacePtr face0 = CreateTriangularFaceAndEdgesFromVertices(mesh,
        adjacentVertexArray[(vertexIndex + 0) % 4],
        adjacentVertexArray[(vertexIndex + 1) % 4],
        adjacentVertexArray[(vertexIndex + 2) % 4]);

    // Create the two triangles.
    FacePtr face1 = CreateTriangularFaceAndEdgesFromVertices(mesh,
        adjacentVertexArray[(vertexIndex + 0) % 4],
        adjacentVertexArray[(vertexIndex + 2) % 4],
        adjacentVertexArray[(vertexIndex + 3) % 4]);

    // Copy all attributes from the quadrilateral to the two triangles.
    face0->copyAttributes(*facePtr);
    face1->copyAttributes(*facePtr);

    // Copy all face vertex attributes from the old face to the new faces.
    for (AdjacentVertexIterator iterator = facePtr->adjacentVertexBegin();
         iterator != facePtr->adjacentVertexEnd(); ++iterator) {
        VertexPtr vertexPtr = *iterator;
        if (face0->hasAdjacentVertex(vertexPtr)) {
            if (facePtr->hasFaceVertex(vertexPtr)) {
                face0->getFaceVertex(vertexPtr)->copyAttributes(
                    *facePtr->getFaceVertex(vertexPtr));
            }
        }
        if (face1->hasAdjacentVertex(vertexPtr)) {
            if (facePtr->hasFaceVertex(vertexPtr)) {
                face1->getFaceVertex(vertexPtr)->copyAttributes(
                    *facePtr->getFaceVertex(vertexPtr));
            }
        }
    }

    // Remove the original quadrilateral.
    DeleteFace(mesh, facePtr);

    if (deletedElementTracker != NULL) {
        deletedElementTracker->addFace(facePtr);
    }
}

void
TriangulateQuadrilateralFace(Mesh *mesh, FacePtr facePtr,
    DeletedElementTracker *deletedElementTracker)
{
    // The face must be a quadrilateral.
    assert(facePtr->adjacentVertexCount() == 4);

    // Copy the four vertices adjacent to the face into an array.
    VertexPtr adjacentVertexArray[4];
    std::copy(facePtr->adjacentVertexBegin(), facePtr->adjacentVertexEnd(),
        adjacentVertexArray);

    float distance02 = (adjacentVertexArray[2]->position()
        - adjacentVertexArray[0]->position()).length();
    float distance13 = (adjacentVertexArray[3]->position()
        - adjacentVertexArray[1]->position()).length();

    if (distance02 < distance13) {
        TriangulateQuadrilateralFaceFromVertex(mesh, facePtr, 
            adjacentVertexArray[0], deletedElementTracker);
    } else {
        TriangulateQuadrilateralFaceFromVertex(mesh, facePtr, 
            adjacentVertexArray[1], deletedElementTracker);
    }
}

} // namespace mesh
