// Copyright 2008 Retroactive Fiasco.

#ifndef CGMATH__COLOR_OPERATIONS__INCLUDED
#define CGMATH__COLOR_OPERATIONS__INCLUDED

class Vector3f;

namespace cgmath {

// Convert gamma intensity space (RGB monitor colors) to
// linear intensity space (perceptual intensity).
float GammaToLinear(float rhs);
Vector3f GammaToLinear(const Vector3f &rhs);

// Convert linear intensity space to gamma intensity space.
float LinearToGamma(float rhs);
Vector3f LinearToGamma(const Vector3f &rhs);

// Converts a gamma space RGB color to luminance.
float GammaColorToLuminance(const Vector3f &rhs);

// Convert a linear space RGB color to luminance.
float LinearColorToLuminance(const Vector3f &rhs);

// Convert HSV to RGB.
Vector3f HsvToRgb(const Vector3f &hsv);

// Convert RGB to HSV.
Vector3f RgbToHsv(const Vector3f &rgb);

} // namespace cgmath

#endif // CGMATH__COLOR_OPERATIONS__INCLUDED
