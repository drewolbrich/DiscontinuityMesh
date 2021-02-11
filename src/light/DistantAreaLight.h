// Copyright 2008 Drew Olbrich

#ifndef LIGHT__DISTANT_AREA_LIGHT__INCLUDED
#define LIGHT__DISTANT_AREA_LIGHT__INCLUDED

#include <vector>

#include <cgmath/Vector3f.h>

namespace light {

// DistantAreaLight
//
// A circular distant light source in the form of an isogonal polygon.

class DistantAreaLight
{
public:
    DistantAreaLight();
    ~DistantAreaLight();

    // Vector that points toward the position of the light source, from the origin.
    void setPosition(const cgmath::Vector3f &position);
    const cgmath::Vector3f &position() const;

    // Set the position of the light source as an azimuth and elevation (in degrees).
    // The azimuth angle is specified in the XZ plane, with zero degrees
    // on the -Z axis (south), and 90 degrees on the +X axis (east).
    // The elevation angle is 0 degrees everywhere in the XZ plane and 90 degrees 
    // on the +Y axis (up). According to Wikipedia, this is the way the
    // solar azimuth angle is most often defined.
    void setPositionFromAzimuthAndElevation(float azimuth, float elevation);

    // The up vector of the light source.
    void setUp(const cgmath::Vector3f &up);
    const cgmath::Vector3f &up() const;

    // The rotation of the light source polygon relative to the up vector, in degrees.
    void setRoll(float roll);
    float roll() const;

    // The angular diameter of the light source, in arcminutes.
    // The angular diameter of the sun varies between 31.6 and 32.7 arcminutes 
    // during on Earth year.
    static const float DEFAULT_ANGULAR_DIAMETER;
    void setAngularDiameter(float angularDiameter);
    float angularDiameter() const;

    // The number of sides of the light source polygon.
    static const int DEFAULT_SIDES;
    void setSides(int sides);
    int sides() const;

    // The intensity of the light source. Since the distant area light is
    // effectively infinitely far away, and the light rays are parallel,
    // the intensity should not fall off over distance.
    void setIntensity(float intensity);
    float intensity() const;

    // The color of the light source.
    void setColor(const cgmath::Vector3f &color);
    const cgmath::Vector3f &color() const;

    // The diameter of the bounding box of the scene. This is the distance from
    // one corner of the bounding box to the opposite corner.
    void setSceneDiameter(float sceneDiameter);
    float sceneDiameter() const;

    // Precompute state used in vertex calculation.
    void prepareForVertexCalculation();

    // Calculate the effective position of a light source vertex relative to 
    // a focal point. This simulates a light source that is infinitely far away,
    // but which in reality is offset from the focal point by the scene diameter.
    // The function prepareForVertexCalculation must be called first.
    // The parameter 'index' must be less than the number of sides.
    cgmath::Vector3f calculateVertex(const cgmath::Vector3f &focalPoint, int index) const;

    // Return the point located at the center of the light source.
    cgmath::Vector3f getCenter(const cgmath::Vector3f &focalPoint) const;

private:
    cgmath::Vector3f mPosition;
    cgmath::Vector3f mUp;
    float mRoll;
    float mAngularDiameter;
    int mSides;
    float mIntensity;
    cgmath::Vector3f mColor;
    float mSceneDiameter;

    // The vector of offsets for each vertex.
    std::vector<cgmath::Vector3f> mVertexOffsetVector;

    cgmath::Vector3f mCenter;
};

} // namespace light

#endif // LIGHT__DISTANT_AREA_LIGHT__INCLUDED
