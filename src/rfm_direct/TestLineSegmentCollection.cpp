// Copyright 2008 Drew Olbrich

#include "TestLineSegmentCollection.h"

#include <cstdlib>
#include <cmath>
#include <iostream>

#include <cgmath/Vector3f.h>
#include <cgmath/Constants.h>
#include <mesh/Mesh.h>
#include <mesh/Types.h>
#include <svg/SvgWriter.h>

#include "WedgeIntersector.h"
#include "LineSegmentCollection.h"
#include "Endpoint.h"

static void AddLineSegmentCollectionToSvgWriter(svg::SvgWriter &svgWriter, 
    const LineSegmentCollection &lineSegmentCollection, float strokeWidth);
static void AddLineSegmentToSvgWriter(svg::SvgWriter &svgWriter, const LineSegment &lineSegment,
    float strokeWidth);

void
TestLineSegmentCollection()
{
    mesh::Mesh mesh;

    mesh::VertexPtr v = mesh.createVertex();
    mesh::VertexPtr p = mesh.createVertex();
    mesh::VertexPtr q = mesh.createVertex();

#if 0
    // OK.
    v->setPosition(cgmath::Vector3f(0.5, -5.0, 0.0));
    p->setPosition(cgmath::Vector3f(0.0, 0.0, 0.0));
    q->setPosition(cgmath::Vector3f(1.0, 0.0, 0.0));
#endif

#if 0
    // OK.
    v->setPosition(cgmath::Vector3f(0.0, 0.0, 0.0));
    p->setPosition(cgmath::Vector3f(0.0, 1.0, 0.0));
    q->setPosition(cgmath::Vector3f(1.0, 0.0, 0.0));
#endif

#if 0
    // OK.
    v->setPosition(cgmath::Vector3f(1.0, 0.0, 0.0));
    p->setPosition(cgmath::Vector3f(0.0, 0.0, 0.0));
    q->setPosition(cgmath::Vector3f(1.0, 1.0, 0.0));
#endif

#if 0
    // OK.
    v->setPosition(cgmath::Vector3f(-5.0, -5.0, 0.0));
    p->setPosition(cgmath::Vector3f(0.0, 1.0, 0.0));
    q->setPosition(cgmath::Vector3f(1.0, 0.0, 0.0));
#endif

#if 0
    // OK.
    v->setPosition(cgmath::Vector3f(-24.0, -9.0, 0.0));
    p->setPosition(cgmath::Vector3f(0.0, 1.0, 0.0));
    q->setPosition(cgmath::Vector3f(1.0, 0.0, 0.0));
#endif

#if 0
    // OK.
    v->setPosition(cgmath::Vector3f(-9.0, -24.0, 0.0));
    p->setPosition(cgmath::Vector3f(0.0, 1.0, 0.0));
    q->setPosition(cgmath::Vector3f(1.0, 0.0, 0.0));
#endif

#if 0
    // OK.
    v->setPosition(cgmath::Vector3f(-7.0, -11.0, 0.0));
    p->setPosition(cgmath::Vector3f(0.0, 1.0, 0.0));
    q->setPosition(cgmath::Vector3f(1.0, 0.0, 0.0));
#endif

#if 0
    // OK.
    v->setPosition(cgmath::Vector3f(7.0, 11.0, 0.0));
    p->setPosition(cgmath::Vector3f(0.0, 1.0, 0.0));
    q->setPosition(cgmath::Vector3f(1.0, 0.0, 0.0));
#endif

#if 1
    // This looks mostly OK, but the rightmost line segment on the circle in the
    // lower right doesn't quite match up with one of the vertices.
    // This is where the line segment was hanging off the edge of the circle
    // before I clipped it. Maybe there are really bad floating point 
    // roundoff problems here.
    v->setPosition(cgmath::Vector3f(-5.0, 6.0, 0.0));
    p->setPosition(cgmath::Vector3f(1.0, 1.0, 0.0));
    q->setPosition(cgmath::Vector3f(0.0, 0.0, 0.0));
#endif

    mesh::EdgePtr pq = mesh.createEdge();
    pq->addAdjacentVertex(p);
    pq->addAdjacentVertex(q);
    p->addAdjacentEdge(pq);
    q->addAdjacentEdge(pq);

    WedgeIntersector wedgeIntersector;
    wedgeIntersector.setVeEventWedge(v, pq);

    LineSegmentCollection lineSegmentCollection;
    lineSegmentCollection.setWedgeIntersector(&wedgeIntersector);

    srand(10);
    srand48(10);

    static const size_t SHAPE_COUNT = 9;
    for (size_t shapeIndex = 0; shapeIndex < SHAPE_COUNT; ++shapeIndex) {

        float outerRadius = drand48()*0.1 + 0.05;
        float innerRadius = (drand48()*0.5 + 0.5)*outerRadius;

        cgmath::Vector2f center(
            drand48()*(1.0 - 2.0*outerRadius*1.1) + outerRadius*1.1,
            drand48()*(1.0 - 2.0*outerRadius*1.1) + outerRadius*1.1);

        float baseAngle = drand48()*cgmath::PI;

        Endpoint firstEndpoint;
        Endpoint previousEndpoint;
        size_t pointCount = (rand() % 10)*2 + 8;
        for (size_t pointIndex = 0; pointIndex <= pointCount; ++pointIndex) {

            float angle = baseAngle + pointIndex/float(pointCount)*2.0*cgmath::PI;

            float radius = 0.0;
            if ((pointIndex % 2) == 0) {
                radius = innerRadius;
            } else {
                radius = outerRadius;
            }

            cgmath::Vector2f position = center
                + cgmath::Vector2f(cos(angle)*radius, sin(angle)*radius);

            Endpoint endpoint;
            if (pointIndex == pointCount) {
                endpoint = firstEndpoint;
            } else {
                endpoint.setWorldPosition(cgmath::Vector3f(position[0], position[1], 0.0));
                cgmath::Vector2f wedgePosition;
                wedgeIntersector.transformWorldSpacePointToWedgeSpacePoint(
                    endpoint.worldPosition(), &wedgePosition);
                endpoint.setWedgePosition(wedgePosition);
                endpoint.setVisibilityParameter(
                    wedgeIntersector.projectPointOntoEdgePQ(endpoint.worldPosition()));
                
                endpoint.setEndpointIdentifier(
                    meshretri::EndpointIdentifier::createUniqueIdentifier());
            }

            if (pointIndex == 0) {
                firstEndpoint = endpoint;
            } else {
                LineSegment lineSegment;
                lineSegment.setPoint0(previousEndpoint);
                lineSegment.setPoint1(endpoint);
                lineSegmentCollection.addLineSegment(lineSegment);
            }

            previousEndpoint = endpoint;
        }
    }

    svg::SvgWriter svgWriter;
    AddLineSegmentCollectionToSvgWriter(svgWriter, lineSegmentCollection, 0.5);

    lineSegmentCollection.calculateVisibleLineSegments();

    AddLineSegmentCollectionToSvgWriter(svgWriter, lineSegmentCollection, 2.5);

    svgWriter.writeToSvgFile("out.svg");

    exit(EXIT_SUCCESS);
}

static void 
AddLineSegmentCollectionToSvgWriter(svg::SvgWriter &svgWriter, 
    const LineSegmentCollection &lineSegmentCollection, float strokeWidth)
{
    for (LineSegmentCollection::const_iterator iterator = lineSegmentCollection.begin();
         iterator != lineSegmentCollection.end(); ++iterator) {
        AddLineSegmentToSvgWriter(svgWriter, *iterator, strokeWidth);
    }
}

static void 
AddLineSegmentToSvgWriter(svg::SvgWriter &svgWriter, const LineSegment &lineSegment,
    float strokeWidth)
{
    svgWriter.addLineSegment(
        cgmath::Vector2f(
            lineSegment.point0().wedgePosition()[0],
            -lineSegment.point0().wedgePosition()[1]),
        cgmath::Vector2f(
            lineSegment.point1().wedgePosition()[0],
            -lineSegment.point1().wedgePosition()[1]),
        strokeWidth);
}
