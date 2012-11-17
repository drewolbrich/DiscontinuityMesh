// Copyright 2008 Retroactive Fiasco.

#ifndef DELAUNAY__VALIDATION_DETAILS__INCLUDED
#define DELAUNAY__VALIDATION_DETAILS__INCLUDED

#include <string>

namespace delaunay {

// ValidationDetails
//
// A class that describes specific details about why the input to
// PointTriangulator or PolygonTriangulator is invalid.

class ValidationDetails
{
public:
    ValidationDetails();
    ~ValidationDetails();

    // Returns true if the input is valid.
    bool isValid() const;

    // Return a string describing what is wrong with the input data.
    std::string asString() const;

    // The input data contains less than three points.
    void setHasLessThanThreePoints(bool hasLessThanThreePoints);
    bool hasLessThanThreePoints() const;

    // Two or more input points are coincident.
    void setSomePointsAreCoincident(bool somePointsAreCoincident);
    bool somePointsAreCoincident() const;

    // All of the input points are colinear.
    void setAllPointsAreColinear(bool allPointsAreColinear);
    bool allPointsAreColinear() const;

    // Some of the input points have NaN values.
    void setSomePointsAreNaN(bool somePointsAreNaN);
    bool somePointsAreNaN() const;

    // Some of the input edges are degenerate.
    void setSomeEdgesAreDegenerate(bool someEdgesAreDegenerate);
    bool someEdgesAreDegenerate() const;

    // Some of the input points intersect input edges.
    void setSomePointsIntersectEdges(bool somePointsIntersectEdges);
    bool somePointsIntersectEdges() const;

    // Some of the input edges intersect.
    void setSomeEdgesIntersect(bool someEdgesIntersect);
    bool someEdgesIntersect() const;

    // Some of the input edges are duplicates.
    void setSomeEdgesAreDuplicates(bool someEdgesAreDuplicates);
    bool someEdgesAreDuplicates() const;

    // The exterior perimeter polygon is not counterclockwise.
    void setExteriorPerimeterIsNotCounterclockwise(bool exteriorPerimeterIsNotCounterclockwise);
    bool exteriorPerimeterIsNotCounterclockwise() const;

private:
    bool mHasLessThanThreePoints;
    bool mSomePointsAreCoincident;
    bool mAllPointsAreColinear;
    bool mSomePointsAreNaN;
    bool mSomeEdgesAreDegenerate;
    bool mSomePointsIntersectEdges;
    bool mSomeEdgesIntersect;
    bool mSomeEdgesAreDuplicates;
    bool mExteriorPerimeterIsNotCounterclockwise;
};

} // namespace delaunay

#endif // DELAUNAY__VALIDATION_DETAILS__INCLUDED
