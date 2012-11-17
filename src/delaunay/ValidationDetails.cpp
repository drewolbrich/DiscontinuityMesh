// Copyright 2008 Retroactive Fiasco.

#include "ValidationDetails.h"

namespace delaunay {

ValidationDetails::ValidationDetails()
    : mHasLessThanThreePoints(false),
      mSomePointsAreCoincident(false),
      mAllPointsAreColinear(false),
      mSomePointsAreNaN(false),
      mSomeEdgesAreDegenerate(false),
      mSomePointsIntersectEdges(false),
      mSomeEdgesIntersect(false),
      mSomeEdgesAreDuplicates(false),
      mExteriorPerimeterIsNotCounterclockwise(false)
{
}

ValidationDetails::~ValidationDetails()
{
}

bool
ValidationDetails::isValid() const
{
    return !mHasLessThanThreePoints
        && !mSomePointsAreCoincident
        && !mAllPointsAreColinear
        && !mSomePointsAreNaN
        && !mSomeEdgesAreDegenerate
        && !mSomePointsIntersectEdges
        && !mSomeEdgesIntersect
        && !mSomeEdgesAreDuplicates
        && !mExteriorPerimeterIsNotCounterclockwise;
}

std::string
ValidationDetails::asString() const
{
    std::string result = "";

    if (mHasLessThanThreePoints) {
        result += (result != "") ? " " : "";
        result += "Less than three input points were specified.";
    }

    if (mSomePointsAreCoincident) {
        result += (result != "") ? " " : "";
        result += "Some input points are coincident.";
    }

    if (mAllPointsAreColinear) {
        result += (result != "") ? " " : "";
        result += "All input points are colinear.";
    }

    if (mSomePointsAreNaN) {
        result += (result != "") ? " " : "";
        result += "Some input points have NaN values.";
    }

    if (mSomePointsIntersectEdges) {
        result += (result != "") ? " " : "";
        result += "Some input points intersect edges.";
    }

    if (mSomeEdgesIntersect) {
        result += (result != "") ? " " : "";
        result += "Some input edges intersect.";
    }

    if (mSomeEdgesAreDegenerate) {
        result += (result != "") ? " " : "";
        result += "Some input edges are degenerate.";
    }

    if (mSomeEdgesAreDuplicates) {
        result += (result != "") ? " " : "";
        result += "Some input edges are duplicates.";
    }

    if (mExteriorPerimeterIsNotCounterclockwise) {
        result += (result != "") ? " " : "";
        result += "The exterior perimeter polygon is not counterclockwise.";
    }

    return result;
}

void
ValidationDetails::setHasLessThanThreePoints(bool hasLessThanThreePoints)
{
    mHasLessThanThreePoints = hasLessThanThreePoints;
}

bool
ValidationDetails::hasLessThanThreePoints() const
{
    return mHasLessThanThreePoints;
}

void
ValidationDetails::setSomePointsAreCoincident(bool somePointsAreCoincident)
{
    mSomePointsAreCoincident = somePointsAreCoincident;
}

bool
ValidationDetails::somePointsAreCoincident() const
{
    return mSomePointsAreCoincident;
}

void
ValidationDetails::setAllPointsAreColinear(bool allPointsAreColinear)
{
    mAllPointsAreColinear = allPointsAreColinear;
}

bool
ValidationDetails::allPointsAreColinear() const
{
    return mAllPointsAreColinear;
}

void
ValidationDetails::setSomePointsAreNaN(bool somePointsAreNaN)
{
    mSomePointsAreNaN = somePointsAreNaN;
}

bool
ValidationDetails::somePointsAreNaN() const
{
    return mSomePointsAreNaN;
}

void
ValidationDetails::setSomeEdgesAreDegenerate(bool someEdgesAreDegenerate)
{
    mSomeEdgesAreDegenerate = someEdgesAreDegenerate;
}

bool
ValidationDetails::someEdgesAreDegenerate() const
{
    return mSomeEdgesAreDegenerate;
}

void
ValidationDetails::setSomePointsIntersectEdges(bool somePointsIntersectEdges)
{
    mSomePointsIntersectEdges = somePointsIntersectEdges;
}

bool
ValidationDetails::somePointsIntersectEdges() const
{
    return mSomePointsIntersectEdges;
}

void
ValidationDetails::setSomeEdgesIntersect(bool someEdgesIntersect)
{
    mSomeEdgesIntersect = someEdgesIntersect;
}

bool
ValidationDetails::someEdgesIntersect() const
{
    return mSomeEdgesIntersect;
}

void
ValidationDetails::setSomeEdgesAreDuplicates(bool someEdgesAreDuplicates)
{
    mSomeEdgesAreDuplicates = someEdgesAreDuplicates;
}

bool
ValidationDetails::someEdgesAreDuplicates() const
{
    return mSomeEdgesAreDuplicates;
}

void
ValidationDetails::setExteriorPerimeterIsNotCounterclockwise(
    bool exteriorPerimeterIsNotCounterclockwise)
{
    mExteriorPerimeterIsNotCounterclockwise = exteriorPerimeterIsNotCounterclockwise;
}

bool
ValidationDetails::exteriorPerimeterIsNotCounterclockwise() const
{
    return mExteriorPerimeterIsNotCounterclockwise;
}

} // namespace delaunay
