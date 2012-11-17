// Copyright 2009 Retroactive Fiasco.

#ifndef RFM_DISCMESH__MESH_SHADER_FACE_LISTENER__INCLUDED
#define RFM_DISCMESH__MESH_SHADER_FACE_LISTENER__INCLUDED

#include <cgmath/Vector3f.h>
#include <mesh/Mesh.h>
#include <mesh/Types.h>
#include <meshisect/FaceIntersectorListener.h>

class DiscontinuityMesher;

// MeshShaderFaceListener
//
// Class that works with MeshShader and meshisect::FaceIntersector
// to selectively leave certain faces out of the ray intersection test.

class MeshShaderFaceListener : public meshisect::FaceIntersectorListener
{
public:
    MeshShaderFaceListener();
    virtual ~MeshShaderFaceListener();

    void setMesh(mesh::Mesh *mesh);
    void setDiscontinuityMesher(DiscontinuityMesher *discontinuityMesher);
    void setRayOrigin(const cgmath::Vector3f &rayOrigin);
    void setRayEndpoint(const cgmath::Vector3f &rayEndpoint);
    void setLocalVertexToIgnore(mesh::VertexPtr localVertexToIgnore);
    void setEmissiveFaceToIgnore(mesh::FacePtr emissiveFaceToIgnore);
    void setLocalFaceToIgnore(mesh::FacePtr localFaceToIgnore);

    // Must be called after all the parameters above are set.
    void initialize();

    // For mesh::IntersectorFaceListener:
    virtual bool allowFaceIntersectionTest(mesh::ConstFacePtr facePtr, float t);

private:
    mesh::Mesh *mMesh;
    DiscontinuityMesher *mDiscontinuityMesher;
    cgmath::Vector3f mRayOrigin;
    cgmath::Vector3f mRayEndpoint;
    mesh::VertexPtr mLocalVertexToIgnore;
    mesh::FacePtr mEmissiveFaceToIgnore;
    mesh::FacePtr mLocalFaceToIgnore;
    float mEpsilon;
};

#endif // RFM_DISCMESH__MESH_SHADER_FACE_LISTENER__INCLUDED
