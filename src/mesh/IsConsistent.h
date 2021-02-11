// Copyright 2008 Drew Olbrich

#ifndef MESH__IS_CONSISTENCY__INCLUDED
#define MESH__IS_CONSISTENCY__INCLUDED

namespace mesh {

class Mesh;

// This function returns true if the mesh is topologically consistent.
bool IsConsistent(const Mesh &mesh);

} // namespace mesh

#endif // MESH__IS_CONSISTENCY__INCLUDED
