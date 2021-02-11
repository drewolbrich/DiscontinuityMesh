// Copyright 2010 Drew Olbrich

#ifndef RFM_RELIGHT__MESH_RELIGHTER__INCLUDED
#define RFM_RELIGHT__MESH_RELIGHTER__INCLUDED

#include <mesh/Types.h>
#include <mesh/AttributeKey.h>

namespace mesh {
class Mesh;
}

// MeshRelighter
//
// Adjusts the precomputed direct and indirect lighting stored in a mesh.

class MeshRelighter
{
public:
    MeshRelighter();
    ~MeshRelighter();

    // The mesh to relight.
    void setMesh(mesh::Mesh *mesh);
    mesh::Mesh *mesh() const;

    // Intensity scale for combined lighting.
    void setIntensity(float intensity);
    float intensity() const;

    // Saturation scale for combined lighting.
    void setSaturation(float saturation);
    float saturation() const;

    // Intensity scale for direct lighting.
    void setDirectIntensity(float directIntensity);
    float directIntensity() const;

    // Saturation scale for direct lighting.
    void setDirectSaturation(float directSaturation);
    float directSaturation() const;

    // Intensity scale for indirect lighting.
    void setIndirectIntensity(float indirectIntensity);
    float indirectIntensity() const;

    // Saturation csale for indirect lighting.
    void setIndirectSaturation(float indirectSaturation);
    float indirectSaturation() const;

    // Relight the mesh.
    void relightMesh();

private:
    void relightFace(mesh::FacePtr facePtr);

    static cgmath::Vector3f scaleSaturation(const cgmath::Vector3f &color, float scale);

    mesh::Mesh *mMesh;

    float mIntensity;
    float mSaturation;
    float mDirectIntensity;
    float mDirectSaturation;
    float mIndirectIntensity;
    float mIndirectSaturation;

    mesh::AttributeKey mIlluminatedColor3fAttributeKey;
    mesh::AttributeKey mIndirectIllumination3fAttributeKey;
    mesh::AttributeKey mColor3fAttributeKey;
};

#endif // RFM_RELIGHT__MESH_RELIGHTER__INCLUDED
