// Copyright 2008 Drew Olbrich

#ifndef MESH__ASSIGN_NORMALS__INCLUDED
#define MESH__ASSIGN_NORMALS__INCLUDED

namespace mesh {

class Mesh;

// The default crease angle is 59 so that a hexagonal cylinder will be faceted
// instead of round.
const float DEFAULT_ASSIGN_NORMALS_CREASE_ANGLE = 59.0;

// Assign normals to a Mesh. For each edge, the normals of the two adjacent faces will be
// examined. If the angle between these normals is larger than the specified crease angle
// (measured in degrees), the face vertices adjacent to the edge will be assigned
// discontinuous normals.
void AssignNormals(Mesh *mesh, float creaseAngle = DEFAULT_ASSIGN_NORMALS_CREASE_ANGLE);

} // namespace mesh

#endif // MESH__ASSIGN_NORMALS__INCLUDED
