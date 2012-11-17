// Copyright 2009 Retroactive Fiasco.

#ifndef MESH__MATERIAL_TABLE__INCLUDED
#define MESH__MATERIAL_TABLE__INCLUDED

#include <vector>

#include <cgmath/Vector3f.h>
#include <mesh/Types.h>
#include <mesh/AttributeKey.h>

#include "Material.h"

namespace mesh {

class Mesh;

// MaterialTable
//
// Keeps track of material definitions in a Mesh.

class MaterialTable
{
public:
    MaterialTable();
    ~MaterialTable();

    // Initialize the table of materials from a mesh.
    void initialize(const Mesh &mesh);

    // Returns true if the face has a material index assigned
    // that corresponds to a material defined in the mesh.
    bool faceHasValidMaterialIndex(FacePtr facePtr) const;

    // Return the material definition of a face.
    const Material &getMaterialFromFace(FacePtr facePtr) const;

    // Return a face vertex's diffuse color, by folding together
    // face material diffuse color, face color, or vertex color, if present.
    cgmath::Vector3f getFaceVertexDiffuseColor(FacePtr facePtr,
        VertexPtr vertexPtr);

private:
    AttributeKey mMaterialIndexAttributeKey;
    AttributeKey mColor3fAttributeKey;

    std::vector<Material> mMaterialVector;
};

} // namespace mesh

#endif // MESH__MATERIAL_TABLE__INCLUDED
