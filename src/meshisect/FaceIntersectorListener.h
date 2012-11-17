// Copyright 2009 Retroactive Fiasco.

#ifndef MESHISECT__FACE_INTERSECTOR_LISTENER__INCLUDED
#define MESHISECT__FACE_INTERSECTOR_LISTENER__INCLUDED

#include <mesh/Types.h>

namespace meshisect {

// FaceIntersectorListener
//
// Optional listener for FaceIntersector that can ignore certain faces.

class FaceIntersectorListener
{
public:
    FaceIntersectorListener() {}
    virtual ~FaceIntersectorListener() {}

    // Return true if an intersection test with the specified face
    // is allowed. Otherwise, the face will be ignored, as if it
    // was not present in the meshisect.
    virtual bool allowFaceIntersectionTest(mesh::ConstFacePtr facePtr, float t) = 0;
};

} // namespace meshisect

#endif // MESHISECT__FACE_INTERSECTOR_LISTENER__INCLUDED
