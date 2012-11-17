// Copyright 2008 Retroactive Fiasco.

#ifndef CGMATH__LINE_SET_INTERSECTOR__INCLUDED
#define CGMATH__LINE_SET_INTERSECTOR__INCLUDED

#include <vector>
#include <map>

#include "Vector2f.h"

namespace cgmath {

// LineSetIntersector
//
// Computes the intersections of a set of 2D line segments.

class LineSetIntersector
{
public:
    LineSetIntersector();
    ~LineSetIntersector();

    // Define the input set of line segments that will be tested
    // for intersection.
    struct LineSegment {
        LineSegment() {};
        LineSegment(const Vector2f &a, const Vector2f &b) : mPoint0(a), mPoint1(b) {};
        Vector2f mPoint0;
        Vector2f mPoint1;
    };
    typedef std::vector<LineSegment> LineSegmentVector;
    void setLineSegmentVector(LineSegmentVector *lineSegmentVector);
    LineSegmentVector *lineSegmentVector() const;

    // Finds the intersections. Returns false if no intersections
    // are found. The current implementation of this algorithm is not
    // terribly robust, and will return information which, if used to
    // clip input line segments that are nearly colinear, may result in 
    // line segments which still intersect.
    bool findIntersections();

    struct Intersection {
        // The point of intersection.
        Vector2f mPoint;
        // The two or more line segments that intersect at the point of intersection.
        typedef std::vector<size_t> LineSegmentIndexVector;
        LineSegmentIndexVector mLineSegmentIndexVector;
    };
    typedef std::vector<Intersection> IntersectionVector;
    const IntersectionVector &getIntersectionVector() const;

private:
    // Create an intersection at the specified point if it doesn't already exist,
    // and add the specified line segment index to that intersection
    // if it's not already associated with the intersection.
    void updateIntersection(const Vector2f &point, size_t index1, size_t index2);

    LineSegmentVector *mLineSegmentVector;
    IntersectionVector mIntersectionVector;

    typedef std::map<Vector2f, int> PointToIntersectionIndexMap;
    PointToIntersectionIndexMap mPointToIntersectionIndexMap;
};

} // namespace cgmath

#endif // CGMATH__LINE_SET_INTERSECTOR__INCLUDED
