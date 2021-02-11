// Copyright 2009 Drew Olbrich

#ifndef MESHWELD__EDGE__INCLUDED
#define MESHWELD__EDGE__INCLUDED

#include <mesh/Types.h>
#include <cgmath/BoundingBox3f.h>

namespace meshweld {

// Edge
//
// Edge that is a candidate for merging. Used to construct
// an AABB tree of edges.

class Edge
{
public:
    Edge();
    ~Edge();

    // The mesh::EdgePtr referenced by the Edge.
    void setEdgePtr(mesh::EdgePtr edgePtr);
    mesh::EdgePtr edgePtr() const;

    // Required by the cgmath::AabbTree template.
    const cgmath::BoundingBox3f &boundingBox() const;

private:
    mesh::EdgePtr mEdgePtr;
    cgmath::BoundingBox3f mBoundingBox;
};

} // namespace meshweld

#endif // MESHWELD__EDGE__INCLUDED
