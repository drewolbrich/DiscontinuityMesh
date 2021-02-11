// Copyright 2009 Drew Olbrich

#ifndef RFM_INDIRECT__MESH_SHADER_FACE_LISTENER__INCLUDED
#define RFM_INDIRECT__MESH_SHADER_FACE_LISTENER__INCLUDED

#include <meshisect/FaceIntersectorListener.h>
#include <mesh/Types.h>

// MeshShaderFaceListener
//
// Class that works with MeshShader and meshisect::FaceIntersector
// to selectively leave certain faces out of the ray intersection test.

class MeshShaderFaceListener : public meshisect::FaceIntersectorListener
{
public:
    MeshShaderFaceListener();
    virtual ~MeshShaderFaceListener();

    // Face ignored by the intersection test.
    void setFacePtrToIgnore(mesh::FacePtr facePtrToIgnore);
    mesh::FacePtr facePtrToIgnore() const;

    // For mesh::IntersectorFaceListener:
    virtual bool allowFaceIntersectionTest(mesh::ConstFacePtr facePtr, float t);

private:
    mesh::FacePtr mFacePtrToIgnore;
};

#endif // RFM_INDIRECT__MESH_SHADER_FACE_LISTENER__INCLUDED
