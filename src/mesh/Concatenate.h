// Copyright 2009 Retroactive Fiasco.

#ifndef MESH__CONCATENATE__INCLUDED
#define MESH__CONCATENATE__INCLUDED

namespace mesh {

class Mesh;

// Concatenate the source mesh onto the target mesh.
void Concatenate(Mesh *targetMesh, const Mesh &sourceMesh);

} // namespace mesh

#endif // MESH__CONCATENATE__INCLUDED
