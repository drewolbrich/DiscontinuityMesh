// Copyright 2009 Drew Olbrich

#ifndef MESH__ATTRIBUTE_POSSESSOR_OPERATIONS__INCLUDED
#define MESH__ATTRIBUTE_POSSESSOR_OPERATIONS__INCLUDED

namespace mesh {

class Mesh;
class AttributePossessor;

// Copy all attributes from the source AttributePossessor to the 
// target AttributePossessor, except when they are already
// possessed by the target AttributePossessor.
void MergeAttributePossessorAttributes(const Mesh &mesh, 
    const AttributePossessor &sourceAttributePossessor,
    AttributePossessor *targetAttributePossessor);

} // namespace mesh

#endif // MESH__ATTRIBUTE_POSSESSOR_OPERATIONS__INCLUDED
