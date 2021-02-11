// Copyright 2008 Drew Olbrich

#include "LineSetIntersector.h"

#include <cassert>
#include <algorithm>
#include <iostream>

#include <exact/GeometricPredicates.h>

#include "LineOperations.h"

namespace cgmath {

LineSetIntersector::LineSetIntersector()
    : mLineSegmentVector(NULL),
      mPointToIntersectionIndexMap()
{
}

LineSetIntersector::~LineSetIntersector()
{
}

void
LineSetIntersector::setLineSegmentVector(LineSegmentVector *lineSegmentVector)
{
    mLineSegmentVector = lineSegmentVector;
}

LineSetIntersector::LineSegmentVector *
LineSetIntersector::lineSegmentVector() const
{
    return mLineSegmentVector;
}

bool
LineSetIntersector::findIntersections()
{
    // This implementation is O(n^2), which is non-optimal for large
    // numbers of line segments. If this becomes a problem,
    // this function should be rewritten to use the Bentley-Ottman algorithm.

    mIntersectionVector.clear();

    assert(mPointToIntersectionIndexMap.empty());

    for (size_t index1 = 0; index1 < (*mLineSegmentVector).size(); ++index1) {
        const LineSegment &ls1 = (*mLineSegmentVector)[index1];
        for (size_t index2 = index1 + 1; index2 < (*mLineSegmentVector).size(); ++index2) {
            const LineSegment &ls2 = (*mLineSegmentVector)[index2];
            if (exact::TestLineSegmentsIntersect2f(ls1.mPoint0, ls1.mPoint1, 
                    ls2.mPoint0, ls2.mPoint1)) {
                Vector2f point;
                if (ls1.mPoint0 == ls2.mPoint0
                    || ls1.mPoint0 == ls2.mPoint1) {
                    updateIntersection(ls1.mPoint0, index1, index2);
                } else if (ls1.mPoint1 == ls2.mPoint0
                    || ls1.mPoint1 == ls2.mPoint1) {
                    updateIntersection(ls1.mPoint1, index1, index2);
                } else if (GetIntersectionOfTwoLines(ls1.mPoint0, ls1.mPoint1, 
                        ls2.mPoint0, ls2.mPoint1, &point)) {

                    // If floating point error has caused either line segment to
                    // double back on itself, use the nearest endpoint as the point of
                    // intersection instead of the calculated point of intersection.
                    float t1 = (ls1.mPoint1 - ls1.mPoint0).normalized()
                        .dot(point - ls1.mPoint0)/(ls1.mPoint1 - ls1.mPoint0).length();
                    if (t1 < 0.0) {
                        point = ls1.mPoint0;
                    } else if (t1 > 1.0) {
                        point = ls1.mPoint1;
                    }
                    float t2 = (ls2.mPoint1 - ls2.mPoint0).normalized()
                        .dot(point - ls2.mPoint0)/(ls2.mPoint1 - ls2.mPoint0).length();
                    if (t2 < 0.0) {
                        point = ls2.mPoint0;
                    } else if (t2 > 1.0) {
                        point = ls2.mPoint1;
                    }

// I've disabled this test because it'll occur if input polygons are coplanar.
#if 0
#ifdef DEBUG
                    t1 = (ls1.mPoint1 - ls1.mPoint0).normalized()
                        .dot(point - ls1.mPoint0)/(ls1.mPoint1 - ls1.mPoint0).length();
                    assert(t1 >= 0.0);
                    assert(t1 <= 1.0);
#endif
#endif

                    updateIntersection(point, index1, index2);

                } else {

// I've disabled this test because it'll occur if input polygons are coplanar.
#if 0
                    assert(0);
#endif
                }
            }
        }
    }

    mPointToIntersectionIndexMap.clear();

    return !mIntersectionVector.empty();
}

const LineSetIntersector::IntersectionVector &
LineSetIntersector::getIntersectionVector() const
{
    return mIntersectionVector;
}

void
LineSetIntersector::updateIntersection(const Vector2f &point, size_t index1, size_t index2)
{
    PointToIntersectionIndexMap::const_iterator iterator 
        = mPointToIntersectionIndexMap.find(point);

    size_t index = 0;
    if (iterator == mPointToIntersectionIndexMap.end()) {
        index = mIntersectionVector.size();
        mIntersectionVector.push_back(Intersection());
        mIntersectionVector[index].mPoint = point;
        mPointToIntersectionIndexMap.insert(std::make_pair(point, index));
    } else {
        index = (*iterator).second;
    }

    Intersection &intersection = mIntersectionVector[index];
    Intersection::LineSegmentIndexVector &lineSegmentIndexVector 
        = intersection.mLineSegmentIndexVector;

    // We are assuming that the number of line segments that intersect at any
    // particular point is small, so it's OK to iterate over the list in linear time.
    if (std::find(lineSegmentIndexVector.begin(), lineSegmentIndexVector.end(), index1)
        == lineSegmentIndexVector.end()) {
        lineSegmentIndexVector.push_back(index1);
    }
    if (std::find(lineSegmentIndexVector.begin(), lineSegmentIndexVector.end(), index2)
        == lineSegmentIndexVector.end()) {
        lineSegmentIndexVector.push_back(index2);
    }
}

} // namespace cgmath
