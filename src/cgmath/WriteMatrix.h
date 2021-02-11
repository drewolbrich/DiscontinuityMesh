// Copyright 2008 Drew Olbrich

#ifndef CGMATH__WRITE_MATRIX__INCLUDED
#define CGMATH__WRITE_MATRIX__INCLUDED

#include <ostream>

namespace cgmath {

// Write out the values in a matrix, lining up the decimal points in
// each column with a minimal amount of extra whitespace.
void WriteMatrix(std::ostream &ostr, int rows, int columns, const float *values);

} // namespace cgmath

#endif // CGMATH__WRITE_MATRIX__INCLUDED
