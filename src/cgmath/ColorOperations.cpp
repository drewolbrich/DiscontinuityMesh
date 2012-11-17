// Copyright 2008 Retroactive Fiasco.

#include "ColorOperations.h"

#include <cmath>

#include "Vector3f.h"

namespace cgmath {

static const float GAMMA = 2.2;

float 
GammaToLinear(float rhs)
{
    return powf(rhs, GAMMA);
}

Vector3f
GammaToLinear(const Vector3f &rhs)
{
    return Vector3f(
        powf(rhs[0], GAMMA),
        powf(rhs[1], GAMMA),
        powf(rhs[2], GAMMA));
}

float 
LinearToGamma(float rhs)
{
    return powf(rhs, 1.0/GAMMA);
}

Vector3f
LinearToGamma(const Vector3f &rhs)
{
    return Vector3f(
        powf(rhs[0], 1.0/GAMMA),
        powf(rhs[1], 1.0/GAMMA),
        powf(rhs[2], 1.0/GAMMA));
}

float
GammaColorToLuminance(const Vector3f &rhs)
{
    return 0.299*rhs[0] + 0.587*rhs[1] + 0.114*rhs[2];
}

float 
LinearColorToLuminance(const Vector3f &rhs)
{
    return 0.2126*rhs[0] + 0.7152*rhs[1] + 0.0722*rhs[2];
}

Vector3f
HsvToRgb(const Vector3f &hsv)
{
    float h = hsv[0];
    float s = hsv[1];
    float v = hsv[2];

    if (s == 0.0) {

        return Vector3f(v, v, v);

    } else {

        h -= floorf(h);

        h *= 6.0;

        int i = int(floorf(h));

        float f = h - i;
        float p = v*(1.0 - s);
        float q = v*(1.0 - (s*f));
        float t = v*(1.0 - (s*(1.0 - f)));

        switch (i) {
        case 0: 
            return Vector3f(v, t, p);
        case 1: 
            return Vector3f(q, v, p);
        case 2: 
            return Vector3f(p, v, t);
        case 3: 
            return Vector3f(p, q, v);
        case 4: 
            return Vector3f(t, p, v);
        case 5: 
            return Vector3f(v, p, q);
        }
    }

    return Vector3f(0, 0, 0);
}

Vector3f
RgbToHsv(const Vector3f &rgb)
{
    float r = rgb[0];
    float g = rgb[1];
    float b = rgb[2];

    float maxv = std::max(r, std::max(g, b));
    float minv = std::min(r, std::min(g, b));

    float h = 0.0;
    float s = 0.0;
    float v = 0.0;

    v = maxv;

    if (maxv != 0.0) {
        s = (maxv - minv)/maxv;
    } else {
        s = 0.0;
    }

    if (s == 0.0) {

        h = 0.0;

    } else {

        float delta = maxv - minv;

        if (r == maxv) {
            h = (g - b)/delta;
        } else if (g == maxv) {
            h = 2.0 + (b - r)/delta;
        } else {
            h = 4.0 + (r - g)/delta;
        }

        h = h/6.0;

        h -= floor(h);
    }

    return Vector3f(h, s, v);
}

} // namespace cgmath
