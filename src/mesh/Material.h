// Copyright 2010 Retroactive Fiasco.

#ifndef MESH__MATERIAL__INCLUDED
#define MESH__MATERIAL__INCLUDED

#include <string>

#include <cgmath/Vector4f.h>

namespace mesh {

// Material
//
// Material defined in a mesh file. The values match the fixed function
// material definitions used by OpenGL.

struct Material
{
    Material() 
        : mName(""),
          mAmbient(cgmath::Vector4f::ZERO),
          mDiffuse(cgmath::Vector4f::ZERO),
          mSpecular(cgmath::Vector4f::ZERO),
          mEmission(cgmath::Vector4f::ZERO),
          mShininess(0.0) {
    }

    std::string mName;
    cgmath::Vector4f mAmbient;
    cgmath::Vector4f mDiffuse;
    cgmath::Vector4f mSpecular;
    cgmath::Vector4f mEmission;
    float mShininess;
};

} // namespace mesh

#endif // MESH__MATERIAL__INCLUDED
