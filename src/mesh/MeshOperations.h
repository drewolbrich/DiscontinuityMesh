// Copyright 2008 Drew Olbrich

#ifndef MESH__MESH_OPERATIONS__INCLUDED
#define MESH__MESH_OPERATIONS__INCLUDED

#include "Types.h"
#include "AttributeKey.h"

namespace cgmath {
class Matrix4f;
}

namespace mesh {

class Mesh;

// Delete all mesh elements that are are not fully connected.
void DeleteOrphanedElements(Mesh *mesh);

// Delete all vertices in the mesh that have no adjacent edges or faces.
void DeleteOrphanedVertices(Mesh *mesh);

// Delete all edges in the mesh that have no adjacent faces or vertices.
void DeleteOrphanedEdges(Mesh *mesh);

// Delete all faces in the mesh that have no adjacent vertices or edges.
void DeleteOrphanedFaces(Mesh *mesh);

// Transform a mesh by a transformation matrix.
// Normals are transformed by the inverse transpose of the matrix.
void Transform(Mesh *mesh, const cgmath::Matrix4f &matrix);

// Remove an attribute from the mesh and all of its elements.
void RemoveAttributeFromMeshAndAllElements(Mesh *mesh, AttributeKey attributeKey);

// Compute a mesh's bounding box. This requires looping
// over all the vertices, so it's expensive.
cgmath::BoundingBox3f ComputeBoundingBox(const Mesh &mesh);

// Returns true if all of the mesh's faces are triangles.
bool AllFacesAreTriangles(const Mesh &mesh);

} // namespace mesh

#endif // MESH__MESH_OPERATIONS__INCLUDED
