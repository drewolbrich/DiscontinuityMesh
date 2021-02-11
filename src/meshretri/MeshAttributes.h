// Copyright 2009 Drew Olbrich

#ifndef MESHRETRI__MESH_ATTRIBUTES__INCLUDED
#define MESHRETRI__MESH_ATTRIBUTES__INCLUDED

#include <mesh/AttributeKey.h>
#include <mesh/Mesh.h>

namespace meshretri {

// Returns an attribute key used to mark vertices as D0 discontinuities.
mesh::AttributeKey GetIsDegreeZeroDiscontinuityAttributeKey(const mesh::Mesh &mesh);

} // namespace meshretri

#endif // MESHRETRI__MESH_ATTRIBUTES__INCLUDED
