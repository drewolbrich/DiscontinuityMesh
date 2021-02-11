// Copyright 2010 Drew Olbrich

#ifndef RFM_INDIRECT__SAMPLE__INCLUDED
#define RFM_INDIRECT__SAMPLE__INCLUDED

#include <cgmath/Vector3f.h>
#include <cgmath/BoundingBox3f.h>

// Sample
//
// Sample point managed by OutputIlluminationAssigner.

class Sample
{
public:
    Sample();
    ~Sample();

    // The position of the sample.
    void setPosition(const cgmath::Vector3f &position);
    const cgmath::Vector3f &position() const;

    // The normal at the sample point.
    void setNormal(const cgmath::Vector3f &normal);
    const cgmath::Vector3f &normal() const;

    // The illumination at the sample point.
    void setIllumination(const cgmath::Vector3f &illumination);
    const cgmath::Vector3f &illumination() const;

    // Area of the face containing the sample point.
    void setFaceArea(float faceArea);
    float faceArea() const;

    // Required by the cgmath::AabbTree template.
    cgmath::BoundingBox3f boundingBox() const;

private:
    cgmath::Vector3f mPosition;
    cgmath::Vector3f mNormal;
    cgmath::Vector3f mIllumination;
    float mFaceArea;
};

#endif // RFM_INDIRECT__SAMPLE__INCLUDED
