// Copyright 2009 Drew Olbrich

#ifndef CGMATH__TOLERANCE__INCLUDED
#define CGMATH__TOLERANCE__INCLUDED

#include <cmath>
#include <limits>

namespace cgmath {

// Tolerance value to use when computing epsilons.
// Defined as the square root of the single precision machine epsilon.
extern float TOLERANCE;

// The following monstriosity ensure that TOLERANCE will always be initialized
// before use, regardless of the order of initialization of global objects.
struct ToleranceInitializer {
    ToleranceInitializer() {
        TOLERANCE = sqrtf(std::numeric_limits<float>::epsilon());
    }
};
static ToleranceInitializer toleranceInitializer;

} // namespace cgmath

#endif // CGMATH__TOLERANCE__INCLUDED
