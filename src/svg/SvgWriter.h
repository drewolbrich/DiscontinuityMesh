// Copyright 2008 Retroactive Fiasco.

#ifndef SVG__SVG_WRITER__INCLUDED
#define SVG__SVG_WRITER__INCLUDED

#include <string>
#include <vector>
#include <fstream>

#include <cgmath/Vector2f.h>
#include <cgmath/BoundingBox2f.h>

namespace svg {

// SvgWriter
//
// This class collects a set of 2D line segements and then writes them
// to an SVG file.

class SvgWriter
{
public:
    SvgWriter();
    ~SvgWriter();

    // Clear out the set of line segments.
    void clear();

    // Add a triangle.
    void addTriangle(const cgmath::Vector2f &p0, const cgmath::Vector2f &p1,
        const cgmath::Vector2f &p2);

    // Add a new line segement.
    void addLineSegment(const cgmath::Vector2f &p0, const cgmath::Vector2f &p1,
        float strokeWidth);

    // Write all the line segments to an SVG file. If the file cannot be opened for writing,
    // an except::OpenFileException is thrown.
    void writeToSvgFile(const std::string &filename);

private:
    // Compute the bounding box of all the primitives.
    cgmath::BoundingBox2f getBoundingBox() const;

    // Compute the transformation of 2D world coordinates to page coordinates,
    // such that the line segment bounding box will fit neatly on the page.
    void computeWorldToPageTransform(const cgmath::BoundingBox2f &bbox);

    // Write the SVG file header.
    void writeHeader();

    // Write the SVG file footer.
    void writeFooter();

    void writeTriangles();

    void writeLineSegments();

    struct Triangle {
        cgmath::Vector2f mPoint0;
        cgmath::Vector2f mPoint1;
        cgmath::Vector2f mPoint2;
    };
    std::vector<Triangle> mTriangleVector;

    struct LineSegment {
        cgmath::Vector2f mPoint0;
        cgmath::Vector2f mPoint1;
        float mStrokeWidth;
    };
    std::vector<LineSegment> mLineSegmentVector;

    // Vectors that transform line segment vertices from world space 
    // to page space, such that
    // pageSpaceVector = worldSpaceVector*mWorldToPageScale + mWorldToPageOffset
    float mWorldToPageScale;
    cgmath::Vector2f mWorldToPageOffset;

    std::ofstream *mFile;
};

} // namespace svg

#endif // SVG__SVG_WRITER__INCLUDED
