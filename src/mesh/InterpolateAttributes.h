// Copyright 2008 Retroactive Fiasco.

#ifndef MESH__INTERPOLATE_ATTRIBUTES__INCLUDED
#define MESH__INTERPOLATE_ATTRIBUTES__INCLUDED

#include <vector>

#include "Types.h"

namespace mesh {

// Assign attributes to the specified vertex based on the interpolated
// attributes of a vector of neighboring vertices. Mean value
// coordinates are used to weight the attributes of the neighboring
// vertices, based on their relative position.
void AssignInterpolatedVertexAttributes(VertexPtr vertexPtr,
    const std::vector<ConstVertexPtr> &vertexPtrVector);

// For the specified vertex and face, assign face vertex attributes. 
// Mean value coordinates are used to weight the attributes of the
// neighboring face vertices, based on their relative position.
void AssignInterpolatedFaceVertexAttributes(FacePtr facePtr, 
    VertexPtr vertexPtr, FacePtr vertexPtrVectorFacePtr,
    const std::vector<ConstVertexPtr> &vertexPtrVector);

} // namespace mesh

#endif // MESH__INTERPOLATE_ATTRIBUTES__INCLUDED
