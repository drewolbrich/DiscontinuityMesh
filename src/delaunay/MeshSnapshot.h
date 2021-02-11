// Copyright 2008 Drew Olbrich

#ifndef DELAUNAY__MESH_SNAPSHOT__INCLUDED
#define DELAUNAY__MESH_SNAPSHOT__INCLUDED

#include <vector>
#include <boost/shared_ptr.hpp>

#include <cgmath/Vector2f.h>

#include "Mesh.h"

namespace delaunay {

class MeshSnapshot;
typedef boost::shared_ptr<MeshSnapshot> MeshSnapshotPtr;

// MeshSnapshot
//
// A snapshot of the state of a mesh at one step of the triangulation algorithm.

class MeshSnapshot
{
public:
    MeshSnapshot();
    ~MeshSnapshot();

    // The state of the mesh when this snapshot was taken.
    void setMeshPtr(MeshPtr meshPtr);
    MeshPtr meshPtr() const;

    // A circle to draw along with the mesh as a reference.
    void setCircleCenter(const cgmath::Vector2f &circleCenter);
    const cgmath::Vector2f &circleCenter() const;
    void setCircleRadius(float circleRadius);
    float circleRadius() const;

    // This function can be called multiple times to display
    // additional points which are not part of the mesh.
    void addPoint(const cgmath::Vector2f &point);

    // Return all the additional point.
    typedef std::vector<cgmath::Vector2f> PointVector;
    const PointVector &pointVector() const;

private:
    MeshPtr mMeshPtr;

    cgmath::Vector2f mCircleCenter;
    float mCircleRadius;

    PointVector mPointVector;
};

} // namespace delaunay

#endif // DELAUNAY__MESH_SNAPSHOT__INCLUDED
