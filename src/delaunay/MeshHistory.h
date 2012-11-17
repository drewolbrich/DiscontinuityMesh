// Copyright 2008 Retroactive Fiasco.

#ifndef DELAUNAY__MESH_HISTORY__INCLUDED
#define DELAUNAY__MESH_HISTORY__INCLUDED

#include <vector>
#include <string>
#include <fstream>

#include <cgmath/BoundingBox2f.h>

#include "MeshTypes.h"
#include "MeshSnapshot.h"

namespace delaunay {

// MeshHistory
//
// A series of mesh snapshots, representing the progress of the Delaunay triangulation
// algorithm over time, so that it can be printed out for debugging purposes.

class MeshHistory
{
public:
    MeshHistory();
    ~MeshHistory();

    // Create a new MeshSnapshot from a mesh. The MeshSnapshot is added to the
    // end of the list of existing MeshSnapshots.
    MeshSnapshotPtr createMeshSnapshot(MeshPtr meshPtr);

    // Write a visual representation of the mesh history to an SVG file.
    // If the file cannot be opened for writing, an except::OpenFileException is thrown.
    void writeToSvgFile(const std::string &filename);

private:
    // Calculate the union of the bounding boxes of all of the meshes in the history.
    cgmath::BoundingBox2f getCommonBoundingBox() const;

    // Calculate the size of the grid (the number of rows and columns), based on
    // the aspect ratio of the specified mesh bounding box and the size of the page.
    void getGridDimensions(const cgmath::BoundingBox2f &meshBoundingBox, 
        int &gridWidth, int &gridHeight, float &meshWidth, float &meshHeight) const;

    // Given a grid size and a mesh aspect ratio, return the size of the meshes that would
    // fit within the cells of that grid.
    void getMeshSizeFromGridDimensions(int gridWidth, int gridHeight, float meshAspectRatio,
        float &meshWidth, float &meshHeight) const;

    // Calculate the scale and offset for a given mesh index, as it should
    // appear on the page.
    void getMeshScaleAndOffset(const cgmath::BoundingBox2f &meshBoundingBox, 
        int gridWidth, int gridHeight, float meshWidth, float meshHeight,
        int meshIndex, cgmath::Vector2f &scale, cgmath::Vector2f &offset);

    // Write the header and footer of the SVG file.
    void writeHeader() const;
    void writeFooter() const;

    // Write a bounding box to the SVG file, with a scale and offset.
    void writeBoundingBox(const cgmath::BoundingBox2f &boundingBox,
        const cgmath::Vector2f &scale, const cgmath::Vector2f &offset) const;

    // Write a clipping path to the SVG file, with a scale and offset.
    void writeClipPath(const cgmath::BoundingBox2f &boundingBox,
        const cgmath::Vector2f &scale, const cgmath::Vector2f &offset, int meshIndex) const;

    // Write a mesh to the SVG file, applying the specified scale and offset
    // to the mesh vertex coordinates.
    void writeMeshFaces(MeshPtr meshPtr, const cgmath::Vector2f &scale, 
        const cgmath::Vector2f &offset) const;
    void writeMeshEdges(MeshPtr meshPtr, const cgmath::Vector2f &scale, 
        const cgmath::Vector2f &offset) const;
    void writeMeshVertices(MeshPtr meshPtr, const cgmath::Vector2f &scale, 
        const cgmath::Vector2f &offset) const;

    // Write a circle to the SVG file.
    void writeCircle(const cgmath::Vector2f &center, float radius,
        const cgmath::Vector2f &scale, const cgmath::Vector2f &offset) const;

    // Write additional points to the SVG file.
    void writePoints(const MeshSnapshot::PointVector &pointVector,
        const cgmath::Vector2f &scale, const cgmath::Vector2f &offset);

    typedef std::vector<MeshSnapshotPtr> MeshSnapshotPtrVector;
    MeshSnapshotPtrVector mMeshSnapshotPtrVector;

    std::ofstream *mFile;
};

} // namespace delaunay

#endif // DELAUNAY__MESH_HISTORY__INCLUDED
