// Copyright 2008 Retroactive Fiasco.

#ifndef MESHOBJ__OBJ_MATERIAL__INCLUDED
#define MESHOBJ__OBJ_MATERIAL__INCLUDED

#include <string>

#include <cgmath/Vector3f.h>

namespace meshobj {

// ObjMaterial
//
// A material definition read from a Wavefront MTL file. This is
// different from OpenGL materials, in that the opacity value is
// stored separately from the diffuse component.

class ObjMaterial
{
public:
    ObjMaterial();
    ~ObjMaterial();

    void setAmbient(const cgmath::Vector3f &ambient);
    bool ambientIsSet() const;
    const cgmath::Vector3f &ambient() const;
    
    void setDiffuse(const cgmath::Vector3f &diffuse);
    bool diffuseIsSet() const;
    const cgmath::Vector3f &diffuse() const;
    
    void setSpecular(const cgmath::Vector3f &specular);
    bool specularIsSet() const;
    const cgmath::Vector3f &specular() const;
    
    void setEmission(const cgmath::Vector3f &emission);
    bool emissionIsSet() const;
    const cgmath::Vector3f &emission() const;
    
    void setOpacity(float opacity);
    bool opacityIsSet() const;
    float opacity() const;
    
    void setShininess(float shininess);
    bool shininessIsSet() const;
    float shininess() const;

    void setDiffuseMap(const std::string &diffuseMap);
    bool diffuseMapIsSet() const;
    const std::string &diffuseMap() const;
    
private:
    bool mAmbientIsSet;
    bool mDiffuseIsSet;
    bool mSpecularIsSet;
    bool mEmissionIsSet;
    bool mOpacityIsSet;
    bool mShininessIsSet;
    bool mDiffuseMapIsSet;
    cgmath::Vector3f mAmbient;
    cgmath::Vector3f mDiffuse;
    cgmath::Vector3f mSpecular;
    cgmath::Vector3f mEmission;
    float mOpacity;
    float mShininess;
    std::string mDiffuseMap;
};

} // namespace meshobj

#endif // MESHOBJ__OBJ_MATERIAL__INCLUDED
