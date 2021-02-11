// Copyright 2008 Drew Olbrich

#include "SvgWriter.h"

#include <algorithm>
#include <sstream>

#include <except/OpenFileException.h>

namespace svg {

// This is the page size (North American letter) that Adobe Illustrator seems to be
// assuming when it loads an SVG file.
static const float PAGE_WIDTH = 612;
static const float PAGE_HEIGHT = 792;

// The margins around the edge of the page.
static const float LEFT_MARGIN = 50;
static const float RIGHT_MARGIN = 50;
static const float TOP_MARGIN = 75;
static const float BOTTOM_MARGIN = 100;

// The colors of the geometric features.
static const char *LINE_SEGMENT_COLOR = "#000000";
static const char *TRIANGLE_COLOR = "#dddddd";

SvgWriter::SvgWriter()
    : mTriangleVector(),
      mLineSegmentVector(),
      mWorldToPageScale(1.0),
      mWorldToPageOffset(0.0, 0.0)
{
}

SvgWriter::~SvgWriter()
{
}

void
SvgWriter::clear()
{
    mLineSegmentVector.clear();
}

void
SvgWriter::addTriangle(const cgmath::Vector2f &p0, const cgmath::Vector2f &p1,
    const cgmath::Vector2f &p2)
{
    Triangle triangle;
    triangle.mPoint0 = p0;
    triangle.mPoint1 = p1;
    triangle.mPoint2 = p2;
    mTriangleVector.push_back(triangle);
}

void
SvgWriter::addLineSegment(const cgmath::Vector2f &p0, const cgmath::Vector2f &p1,
    float strokeWidth)
{
    LineSegment lineSegment;
    lineSegment.mPoint0 = p0;
    lineSegment.mPoint1 = p1;
    lineSegment.mStrokeWidth = strokeWidth;
    mLineSegmentVector.push_back(lineSegment);
}

void
SvgWriter::writeToSvgFile(const std::string &filename)
{
    // Compute the 2D bounding box of all the line segments.
    cgmath::BoundingBox2f bbox = getBoundingBox();

    // Figure out how to scale the line segment coordinate space so it'll
    // fit neatly on the page defined by the SVG file.
    computeWorldToPageTransform(bbox);

    // Open the SVG file. By declaring the std::ofstream locally and then
    // referencing it elsewhere in the class via a pointer, we ensure that the
    // file will be closed if an exception is thrown.
    std::ofstream file(filename.c_str());
    mFile = &file;

    if (!*mFile) {
        throw except::OpenFileException(SOURCE_LINE, os::Error::fromSystemError())
            << "Could not open file \"" << filename << "\".";
    }

    // Write out the SVG file header.
    writeHeader();

    writeTriangles();

    writeLineSegments();

    // Write out the SVG file footer.
    writeFooter();
}

cgmath::BoundingBox2f
SvgWriter::getBoundingBox() const
{
    cgmath::BoundingBox2f bbox;
    bbox.reset();

    for (size_t index = 0; index < mTriangleVector.size(); ++index) {
        bbox.extendByVector2f(mTriangleVector[index].mPoint0);
        bbox.extendByVector2f(mTriangleVector[index].mPoint1);
        bbox.extendByVector2f(mTriangleVector[index].mPoint2);
    }

    for (size_t index = 0; index < mLineSegmentVector.size(); ++index) {
        bbox.extendByVector2f(mLineSegmentVector[index].mPoint0);
        bbox.extendByVector2f(mLineSegmentVector[index].mPoint1);
    }

    // This zooms in on a specific point, which is useful when
    // debugging extreme floating point problems.
#if 0
    cgmath::Vector2f padding = cgmath::Vector2f(1, 1)*0.000005;
    cgmath::Vector2f center(1.3164358139, 0.999999046326);
    bbox = cgmath::BoundingBox2f(center - padding, center + padding);
#endif

    return bbox;
}

void
SvgWriter::computeWorldToPageTransform(const cgmath::BoundingBox2f &bbox)
{
    mWorldToPageScale = std::min(
        (PAGE_WIDTH - LEFT_MARGIN - RIGHT_MARGIN)/bbox.sizeX(),
        (PAGE_HEIGHT - TOP_MARGIN - BOTTOM_MARGIN)/bbox.sizeY());

    // This centers the image horizontally.
    mWorldToPageOffset[0] = -bbox.minX()*mWorldToPageScale
        + (PAGE_WIDTH - bbox.sizeX()*mWorldToPageScale)*0.5;

    mWorldToPageOffset[1] = -bbox.minY()*mWorldToPageScale + TOP_MARGIN;
}

void
SvgWriter::writeHeader()
{
    *mFile << "<?xml version=\"1.0\" standalone=\"no\"?>\n";

    *mFile << "<!DOCTYPE svg PUBLIC \"-//W3C//DTD SVG 1.1//EN\" "
        << "\"http://www.w3.org/Graphics/SVG/1.1/DTD/svg11.dtd\">\n";
    *mFile << "\n";

    *mFile << "<!--\n";
    *mFile << "This SVG file is intended to be loaded in Adobe Illustrator\n";
    *mFile << "and printed on North American letter size paper.\n";
    *mFile << "-->\n";
    *mFile << "\n";

    *mFile << "<svg "
        << "width=\"" << PAGE_WIDTH << "\" "
        << "height=\"" << PAGE_HEIGHT << "\" "
        << "version=\"1.1\" "
        << "xmlns=\"http://www.w3.org/2000/svg\">\n";
    *mFile << "\n";
}

void
SvgWriter::writeFooter()
{
    *mFile << "\n";
    *mFile << "</svg>\n";
}

void
SvgWriter::writeTriangles()
{
    for (size_t index = 0; index < mTriangleVector.size(); ++index) {
        const Triangle &triangle = mTriangleVector[index];

        cgmath::Vector2f v1 = triangle.mPoint0*mWorldToPageScale + mWorldToPageOffset;
        cgmath::Vector2f v2 = triangle.mPoint1*mWorldToPageScale + mWorldToPageOffset;
        cgmath::Vector2f v3 = triangle.mPoint2*mWorldToPageScale + mWorldToPageOffset;

        *mFile << "<polygon points=\"";
        *mFile << v1[0] << "," << v1[1];
        *mFile << " " << v2[0] << "," << v2[1];
        *mFile << " " << v3[0] << "," << v3[1];
        *mFile << "\"";
        *mFile << " stroke=\"none\"";
        *mFile << " fill=\"" << TRIANGLE_COLOR << "\"";
        *mFile << "/>\n";
    }
}

void
SvgWriter::writeLineSegments()
{
    for (size_t index = 0; index < mLineSegmentVector.size(); ++index) {
        const LineSegment &lineSegment = mLineSegmentVector[index];

        cgmath::Vector2f v1 = lineSegment.mPoint0*mWorldToPageScale + mWorldToPageOffset;
        cgmath::Vector2f v2 = lineSegment.mPoint1*mWorldToPageScale + mWorldToPageOffset;

        *mFile << "<line";
        *mFile << " x1=\"" << v1[0] << "\"";
        *mFile << " y1=\"" << v1[1] << "\"";
        *mFile << " x2=\"" << v2[0] << "\"";
        *mFile << " y2=\"" << v2[1] << "\"";
        *mFile << " stroke=\"" << LINE_SEGMENT_COLOR << "\"";
        *mFile << " stroke-width=\"" << lineSegment.mStrokeWidth << "\"";
        *mFile << "/>\n";
    }
}

} // namespace svg
