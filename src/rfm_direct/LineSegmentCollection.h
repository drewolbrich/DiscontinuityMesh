// Copyright 2008 Drew Olbrich

#ifndef RFM_DISCMESH__LINE_SEGMENT_COLLECTION__INCLUDED
#define RFM_DISCMESH__LINE_SEGMENT_COLLECTION__INCLUDED

#include <vector>

#include "LineSegment.h"

class WedgeIntersector;

// LineSegmentCollection
//
// A collection of LineSegments.

class LineSegmentCollection
{
public:
    LineSegmentCollection();
    ~LineSegmentCollection();

    // Sets the WedgeIntersector used to project coordinates
    // onto edge PQ of the mesh and find the resulting parameteric value.
    void setWedgeIntersector(WedgeIntersector *wedgeIntersector);
    WedgeIntersector *wedgeIntersector() const;

    // Add a new line segment.
    void addLineSegment(const LineSegment &lineSegment);

    // Calculate a new set of line segments that represent the visible
    // portion of the input line segments.
    // (when viewed in the direction of the +T axis in wedge space).
    void calculateVisibleLineSegments();

    // Standard STL-compatible container types and accessor functions.
    typedef LineSegment value_type;
    typedef std::vector<value_type> LineSegmentVector;
    typedef LineSegmentVector::size_type size_type;
    typedef LineSegmentVector::iterator iterator;
    typedef LineSegmentVector::const_iterator const_iterator;
    iterator begin();
    iterator end();
    const_iterator begin() const;
    const_iterator end() const;
    size_type size() const;
    bool empty() const;
    void clear();

private:
    // Allow the unit test to access private member functions.
    friend class LineSegmentCollectionTest;

    void findLineSegmentIntersections();
    void findLineSegmentAbutments();
    void splitLineSegments();
    void sortLineSegments();
    void findVisibleLineSegments();

    WedgeIntersector *mWedgeIntersector;
    LineSegmentVector mLineSegmentVector;
};

#endif // RFM_DISCMESH__LINE_SEGMENT_COLLECTION__INCLUDED
