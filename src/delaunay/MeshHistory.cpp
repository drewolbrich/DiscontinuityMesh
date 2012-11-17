// Copyright 2008 Retroactive Fiasco.

#include "MeshHistory.h"

#include <cmath>
#include <cassert>
#include <sstream>
#include <algorithm>

#include <except/OpenFileException.h>
#include <exact/GeometricPredicates.h>
#include <cgmath/BoundingBox2f.h>
#include <cgmath/PointOperations.h>

#include "MeshSnapshot.h"
#include "MeshOperations.h"
#include "Mesh.h"
#include "Vertex.h"
#include "Edge.h"
#include "Face.h"

using cgmath::Vector2f;

namespace delaunay {

// This is the page size (North American letter) that Adobe Illustrator seems to be
// assuming when it loads an SVG file.
static const float PAGE_WIDTH = 612;
static const float PAGE_HEIGHT = 792;

// The margins around the edge of the page.
static const float LEFT_MARGIN = 50;
static const float RIGHT_MARGIN = 50;
static const float TOP_MARGIN = 75;
static const float BOTTOM_MARGIN = 100;

// The space between each bounding box.
static const float HORIZONTAL_SPACING = 35;
static const float VERTICAL_SPACING = 35;

// The radius of each vertex.
static const float MESH_VERTEX_RADIUS = 0.75;

// The widths of the lines.
static const float BOUNDING_BOX_STROKE_WIDTH = 0.5;
static const float MESH_EDGE_STROKE_WIDTH = 0.5;
static const float CIRCLE_STROKE_WIDTH = 0.5;

// The colors of the drawing features.
static const char *BOUNDING_BOX_COLOR = "#f0f0f0";
static const char *MESH_FACE_COLOR = "#fafafa";
static const char *MESH_EDGE_COLOR = "#a0a0a0";
static const char *MESH_VERTEX_COLOR = "#000000";
static const char *CIRCLE_COLOR = "#a00000";

// The relative amount to grow the clipping rectangle.
static const float CLIP_RECTANGLE_GROW_FACTOR = 0.44;

MeshHistory::MeshHistory()
    : mMeshSnapshotPtrVector(),
      mFile(NULL)
{
}

MeshHistory::~MeshHistory()
{
}

MeshSnapshotPtr
MeshHistory::createMeshSnapshot(MeshPtr meshPtr)
{
    MeshSnapshotPtr meshSnapshotPtr(new MeshSnapshot);

    // Make a copy of the whole mesh.
    meshSnapshotPtr->setMeshPtr(meshPtr->clone());

    mMeshSnapshotPtrVector.push_back(meshSnapshotPtr);

    return meshSnapshotPtr;
}

void
MeshHistory::writeToSvgFile(const std::string &filename)
{
    // Open the SVG file. By declaring the std::ofstream locally and then
    // referencing it elsewhere in the class via a pointer, we ensure that the
    // file will be closed if an exception is thrown.
    std::ofstream file(filename.c_str());
    mFile = &file;

    if (!*mFile) {
        throw except::OpenFileException(SOURCE_LINE, os::Error::fromSystemError())
            << "Could not open file \"" << filename << "\".";
    }

    writeHeader();

    // Determine the dimensions of the grid and the size of the meshes
    // we'd like to print out.
    cgmath::BoundingBox2f meshBoundingBox = getCommonBoundingBox();
    int gridWidth = 1;
    int gridHeight = 1;
    float meshWidth = 0.0;
    float meshHeight = 0.0;
    getGridDimensions(meshBoundingBox, gridWidth, gridHeight, meshWidth, meshHeight);

    *mFile << "\n";

    // The grid is drawn in three passes, to allow for explicit control
    // over the depth of the elements.
    for (int pass = 0; pass < 4; ++pass) {

        if (pass == 0) {
            *mFile << "<defs>\n";
        }

        int meshIndex = 0;
        for (MeshSnapshotPtrVector::const_iterator iterator = mMeshSnapshotPtrVector.begin();
             iterator != mMeshSnapshotPtrVector.end(); ++iterator) {
            MeshSnapshotPtr meshSnapshotPtr = *iterator;
            MeshPtr meshPtr = meshSnapshotPtr->meshPtr();

            Vector2f scale(1.0, 1.0);
            Vector2f offset(0.0, 0.0);
            getMeshScaleAndOffset(meshBoundingBox, gridWidth, gridHeight, meshWidth, meshHeight,
                meshIndex, scale, offset);

            switch (pass) {

            case 0:
                writeClipPath(meshBoundingBox, scale, offset, meshIndex);
                break;

            case 1:
                writeMeshFaces(meshPtr, scale, offset);
                break;

            case 2:
                // Write the mesh edges before the circles.
                writeMeshEdges(meshPtr, scale, offset);

                // Draw all of the circles next, because they can potentially
                // extend outside the bounding boxes and overlap other features.
                // We want the features that we draw later to always be drawn
                // on top of the circles.
                if (meshSnapshotPtr->circleRadius() > 0.0) {
                    *mFile << "<g clip-path=\"url(#clip" << meshIndex << ")\">\n";
                    writeCircle(meshSnapshotPtr->circleCenter(), meshSnapshotPtr->circleRadius(),
                        scale, offset);
                    *mFile << "</g>\n";
                }
                break;

            case 3:
                // Draw the vertices last so they're on top of everything else.
                writeMeshVertices(meshPtr, scale, offset);
                writePoints(meshSnapshotPtr->pointVector(), scale, offset);
                break;
            }

            ++meshIndex;
        }

        if (pass == 0) {
            *mFile << "</defs>\n";
        }
    }

    writeFooter();
}

cgmath::BoundingBox2f
MeshHistory::getCommonBoundingBox() const
{
    cgmath::BoundingBox2f bbox;
    bbox.reset();

    // Compute the bounding box that contains all meshes in the history.
    for (MeshSnapshotPtrVector::const_iterator iterator = mMeshSnapshotPtrVector.begin();
         iterator != mMeshSnapshotPtrVector.end(); ++iterator) {
        MeshSnapshotPtr meshSnapshotPtr = *iterator;
        cgmath::BoundingBox2f newBbox = GetMeshBoundingBox(meshSnapshotPtr->meshPtr());
        if (!newBbox.empty()) {
            bbox.extendByBoundingBox2f(newBbox);
        }
    }

    return bbox;
}

void
MeshHistory::getGridDimensions(const cgmath::BoundingBox2f &meshBoundingBox, 
    int &gridWidth, int &gridHeight, float &meshWidth, float &meshHeight) const
{
    int meshCount = mMeshSnapshotPtrVector.size();
    assert(meshCount > 0);

    float meshAspectRatio = meshBoundingBox.sizeX()/meshBoundingBox.sizeY();

    meshWidth = 0.0;
    meshHeight = 0.0;
    for (int width = 1; width <= meshCount; ++width) {
        assert(width != 0);

        // Given the current grid width, calculate the mesh height that would be required
        // to accomodate all of the meshes we'd like to draw.
        int height = (meshCount + width - 1)/width;
        assert(height != 0);
        assert(width*height >= meshCount);

        // Calculate what the size of each mesh would be if the mesh
        // had these dimensions.
        float candidateMeshWidth = 0.0;
        float candidateMeshHeight = 0.0;
        getMeshSizeFromGridDimensions(width, height, meshAspectRatio, 
            candidateMeshWidth, candidateMeshHeight);
        assert(candidateMeshWidth != 0.0);
        assert(candidateMeshHeight != 0.0);

        // If all of the meshes don't fit on the page, skip this configuration.
        if (TOP_MARGIN + height*candidateMeshHeight
            + (height - 1)*VERTICAL_SPACING + BOTTOM_MARGIN > PAGE_HEIGHT) {
            continue;
        }

        // If this is the largest mesh size we've seen so far,
        // remember it, and record this grid size as the optimal grid width and height.
        if (candidateMeshWidth > meshWidth) {
            meshWidth = candidateMeshWidth;
            meshHeight = candidateMeshHeight;
            gridWidth = width;
            gridHeight = height;
        }
    }
}

void
MeshHistory::getMeshSizeFromGridDimensions(int gridWidth, int gridHeight, float meshAspectRatio,
    float &meshWidth, float &meshHeight) const
{
    // Starting with the page size, subtract out the margins and the spacing
    // to determine how much space is available to place grid cells.
    float availableHorizontalSpace = PAGE_WIDTH - LEFT_MARGIN - RIGHT_MARGIN
        - HORIZONTAL_SPACING*(gridWidth - 1);
    float availableVerticalSpace = PAGE_HEIGHT - TOP_MARGIN - BOTTOM_MARGIN
        - VERTICAL_SPACING*(gridHeight - 1);

    // Calculate the size of the grid cells.
    assert(gridWidth != 0);
    float gridCellWidth = availableHorizontalSpace/gridWidth;
    assert(gridHeight != 0);
    float gridCellHeight = availableVerticalSpace/gridHeight;

    // Now determine how large each mesh could be within each grid cell,
    // given its aspect ratio.
    float gridCellAspectRatio = gridCellWidth/gridCellHeight;
    if (gridCellAspectRatio > meshAspectRatio) {
        meshWidth = gridCellHeight*meshAspectRatio;
        meshHeight = gridCellHeight;
    } else {
        meshWidth = gridCellWidth;
        meshHeight = gridCellWidth/meshAspectRatio;
    }
}

void
MeshHistory::getMeshScaleAndOffset(const cgmath::BoundingBox2f &meshBoundingBox,
    int gridWidth, int, float meshWidth, float meshHeight,
    int meshIndex, cgmath::Vector2f &scale, cgmath::Vector2f &offset)
{
    scale[0] = meshWidth/meshBoundingBox.sizeX();

    // In SVG files, the origin is in the upper left and +Y is down,
    // but in our coordinate system, +Y is up, so we have to flip everything.
    scale[1] = -meshHeight/meshBoundingBox.sizeY();

    // An additional offset that centers the grid of meshes on the page.
    Vector2f centeringOffset((PAGE_WIDTH - LEFT_MARGIN - RIGHT_MARGIN 
            - gridWidth*meshWidth - (gridWidth - 1)*HORIZONTAL_SPACING)*0.5,
        0.0);

    assert(gridWidth != 0);
    int x = meshIndex % gridWidth;
    int y = meshIndex/gridWidth;

    offset = Vector2f(LEFT_MARGIN + x*(meshWidth + HORIZONTAL_SPACING),
        TOP_MARGIN + y*(meshHeight + VERTICAL_SPACING));

    // This adjustment to the offset positions the upper left corner of the mesh
    // bounding box at the upper left corner of the grid cell.
    offset -= meshBoundingBox.min()*scale;

    // This adjustment centers the columns of meshes horizontally page.
    // Without this, the grid would be left-justified on the page.
    offset += centeringOffset;

    // Compensate for the fact that we flipped the Y axis.
    offset[1] += meshHeight;
}

void
MeshHistory::writeHeader() const
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
}

void
MeshHistory::writeFooter() const
{
    *mFile << "\n";
    *mFile << "</svg>\n";
}

void
MeshHistory::writeBoundingBox(const cgmath::BoundingBox2f &boundingBox,
    const cgmath::Vector2f &scale, const cgmath::Vector2f &offset) const
{
    Vector2f p1 = boundingBox.min()*scale + offset;
    Vector2f p2 = boundingBox.max()*scale + offset;

    // If the scale is negative, the ordering will wind up reversed,
    // so we may have to swap them. SVG only supports positive
    // rectangle widths and heights.
    if (p1[0] > p2[0]) {
        std::swap(p1[0], p2[0]);
    }
    if (p1[1] > p2[1]) {
        std::swap(p1[1], p2[1]);
    }

    *mFile << "<rect";
    *mFile << " x=\"" << p1[0] << "\"";
    *mFile << " y=\"" << p1[1] << "\"";
    *mFile << " width=\"" << p2[0] - p1[0] << "\"";
    *mFile << " height=\"" << p2[1] - p1[1] << "\"";
    *mFile << " fill=\"none\"";
    *mFile << " stroke=\"" << BOUNDING_BOX_COLOR << "\"";
    *mFile << " stroke-width=\"" << BOUNDING_BOX_STROKE_WIDTH << "\"/>\n";
}

void
MeshHistory::writeClipPath(const cgmath::BoundingBox2f &boundingBox,
    const cgmath::Vector2f &scale, const cgmath::Vector2f &offset, int meshIndex) const
{
    Vector2f p1 = boundingBox.min()*scale + offset;
    Vector2f p2 = boundingBox.max()*scale + offset;

    // If the scale is negative, the ordering will wind up reversed,
    // so we may have to swap them. SVG only supports positive
    // rectangle widths and heights.
    if (p1[0] > p2[0]) {
        std::swap(p1[0], p2[0]);
    }
    if (p1[1] > p2[1]) {
        std::swap(p1[1], p2[1]);
    }

    // Grow the bounding box a little bit.
    float maxSize = std::max(p2[0] - p1[0], p2[1] - p1[1]);
    cgmath::Vector2f growOffset(maxSize*CLIP_RECTANGLE_GROW_FACTOR, 
        maxSize*CLIP_RECTANGLE_GROW_FACTOR);
    p1 -= growOffset;
    p2 += growOffset;

    *mFile << "<clipPath id=\"clip" << meshIndex << "\">\n";
    *mFile << "<rect";
    *mFile << " x=\"" << p1[0] << "\"";
    *mFile << " y=\"" << p1[1] << "\"";
    *mFile << " width=\"" << p2[0] - p1[0] << "\"";
    *mFile << " height=\"" << p2[1] - p1[1] << "\"";
    *mFile << " fill=\"none\"";
    *mFile << " stroke=\"" << BOUNDING_BOX_COLOR << "\"";
    *mFile << " stroke-width=\"" << BOUNDING_BOX_STROKE_WIDTH << "\"/>\n";
    *mFile << "</clipPath>\n";
}

void 
MeshHistory::writeMeshFaces(MeshPtr meshPtr, const cgmath::Vector2f &scale, 
    const cgmath::Vector2f &offset) const
{
    for (FacePtr facePtr = meshPtr->faceBegin(); 
         facePtr != meshPtr->faceEnd(); ++facePtr) {

        if (facePtr->adjacentVertexCount() != 3) {
            continue;
        }

        Vector2f v1 = facePtr->adjacentVertex(0)->position();
        Vector2f v2 = facePtr->adjacentVertex(1)->position();
        Vector2f v3 = facePtr->adjacentVertex(2)->position();

        v1 = v1*scale + offset;
        v2 = v2*scale + offset;
        v3 = v3*scale + offset;

        *mFile << "<polygon points=\"";
        *mFile << v1[0] << "," << v1[1];
        *mFile << " " << v2[0] << "," << v2[1];
        *mFile << " " << v3[0] << "," << v3[1];
        *mFile << "\"";
        *mFile << " stroke=\"none\"";
        *mFile << " fill=\"" << MESH_FACE_COLOR << "\"";
        *mFile << "/>\n";
    }
}

void
MeshHistory::writeMeshEdges(MeshPtr meshPtr, const cgmath::Vector2f &scale, 
    const cgmath::Vector2f &offset) const
{
    for (EdgePtr edgePtr = meshPtr->edgeBegin(); 
         edgePtr != meshPtr->edgeEnd(); ++edgePtr) {

        if (edgePtr->adjacentVertexCount() != 2) {
            continue;
        }

        Vector2f v1 = edgePtr->adjacentVertex(0)->position()*scale + offset;
        Vector2f v2 = edgePtr->adjacentVertex(1)->position()*scale + offset;

        *mFile << "<line";
        *mFile << " x1=\"" << v1[0] << "\"";
        *mFile << " y1=\"" << v1[1] << "\"";
        *mFile << " x2=\"" << v2[0] << "\"";
        *mFile << " y2=\"" << v2[1] << "\"";
        *mFile << " stroke=\"" << MESH_EDGE_COLOR << "\"";
        *mFile << " stroke-width=\"" << MESH_EDGE_STROKE_WIDTH << "\"";
        *mFile << "/>\n";
    }
}

void
MeshHistory::writeMeshVertices(MeshPtr meshPtr, const cgmath::Vector2f &scale, 
    const cgmath::Vector2f &offset) const
{
    for (VertexPtr vertexPtr = meshPtr->vertexBegin(); 
         vertexPtr != meshPtr->vertexEnd(); ++vertexPtr) {

        Vector2f position = vertexPtr->position()*scale + offset;

        *mFile << "<circle";
        *mFile << " cx=\"" << position[0] << "\"";
        *mFile << " cy=\"" << position[1] << "\"";
        *mFile << " r=\"" << MESH_VERTEX_RADIUS << "\"";
        *mFile << " fill=\"" << MESH_VERTEX_COLOR << "\"";
        *mFile << " stroke=\"none\"";
        *mFile << "/>\n";
    }
}

void
MeshHistory::writeCircle(const cgmath::Vector2f &center, float radius,
    const cgmath::Vector2f &scale, const cgmath::Vector2f &offset) const
{
    Vector2f newCenter = center*scale + offset;
    float newRadius = radius*scale[0];

    *mFile << "<circle";
    *mFile << " cx=\"" << newCenter[0] << "\"";
    *mFile << " cy=\"" << newCenter[1] << "\"";
    *mFile << " r=\"" << newRadius << "\"";
    *mFile << " fill=\"none\"";
    *mFile << " stroke=\"" << CIRCLE_COLOR << "\"";
    *mFile << " stroke-width=\"" << CIRCLE_STROKE_WIDTH << "\"";
    *mFile << "/>\n";
}

void
MeshHistory::writePoints(const MeshSnapshot::PointVector &pointVector,
    const cgmath::Vector2f &scale, const cgmath::Vector2f &offset)
{
    for (MeshSnapshot::PointVector::const_iterator iterator = pointVector.begin();
         iterator != pointVector.end(); ++iterator) {
        const cgmath::Vector2f &point(*iterator);
        Vector2f position = point*scale + offset;

        *mFile << "<circle";
        *mFile << " cx=\"" << position[0] << "\"";
        *mFile << " cy=\"" << position[1] << "\"";
        *mFile << " r=\"" << MESH_VERTEX_RADIUS << "\"";
        *mFile << " fill=\"" << MESH_VERTEX_COLOR << "\"";
        *mFile << " stroke=\"none\"";
        *mFile << "/>\n";
    }
}

} // namespace delaunay
