// Copyright 2008 Drew Olbrich

#ifndef MESH__STANDARD_ATTRIBUTES__INCLUDED
#define MESH__STANDARD_ATTRIBUTES__INCLUDED

#include <vector>

// This file defines functions for accessing standard mesh attribute keys.

#include "AttributeKey.h"

namespace mesh {

class Mesh;

// A reference to polygons that have no material index assigned.
const int UNDEFINED_MATERIAL_INDEX = -1;

// A reference to polygons that have no texture index assigned.
const int UNDEFINED_TEXTURE_INDEX = -1;

// Standard attribute keys that apply to faces and face vertices.
AttributeKey GetTexCoord2fAttributeKey(const Mesh &mesh);
AttributeKey GetNormal3fAttributeKey(const Mesh &mesh);
AttributeKey GetColor3fAttributeKey(const Mesh &mesh);
// The following apply only to faces, not face vertices.
AttributeKey GetMaterialIndexAttributeKey(const Mesh &mesh);
AttributeKey GetTextureIndexAttributeKey(const Mesh &mesh);

// Mesh material table attribute keys. These are assigned to
// the Mesh object, not to its individual elements.
AttributeKey GetMaterialNameAttributeKey(const Mesh &mesh, int materialIndex);
AttributeKey GetMaterialAmbient4fAttributeKey(const Mesh &mesh, int materialIndex);
AttributeKey GetMaterialDiffuse4fAttributeKey(const Mesh &mesh, int materialIndex);
AttributeKey GetMaterialSpecular4fAttributeKey(const Mesh &mesh, int materialIndex);
AttributeKey GetMaterialEmission4fAttributeKey(const Mesh &mesh, int materialIndex);
AttributeKey GetMaterialShininessAttributeKey(const Mesh &mesh, int materialIndex);

// Mesh texture table attribute keys.
AttributeKey GetTextureFilenameAttributeKey(const Mesh &mesh, int textureIndex);

// Returns true if the mesh has a material defined for the
// specified material index.
bool HasMaterialDefinition(const Mesh &mesh, int materialIndex);

// Returns true if the mesh has a texture defined for the
// specified texture index.
bool HasTextureDefinition(const Mesh &mesh, int textureIndex);

// Get a vector of material indices present in a mesh.
typedef std::vector<int> MaterialIndexVector;
MaterialIndexVector GetMaterialIndexVector(const Mesh &mesh);

// Get a vector of texture indices present in a mesh.
typedef std::vector<int> TextureIndexVector;
TextureIndexVector GetTextureIndexVector(const Mesh &mesh);

} // namespace mesh

#endif // MESH__STANDARD_ATTRIBUTES__INCLUDED
