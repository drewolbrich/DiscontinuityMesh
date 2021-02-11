// Copyright 2008 Drew Olbrich

#include "WriteMatrix.h"

#include <cassert>
#include <cmath>
#include <iostream>
#include <sstream>
#include <algorithm>

namespace cgmath {

// Get the string representation of a floating point number.
static std::string ConvertDoubleToString(double value);

// Given a string representation of a floating point number, return the number of digits
// to the left of the decimal point, and to the right of, and including, the decimal
// point.
static void CountDigits(const std::string &string, int *leftDigits, int *rightDigits);
    
void
WriteMatrix(std::ostream &ostr, int rows, int columns, const float *values)
{
    int maxLeftDigits = 0;
    int maxRightDigits = 0;
    int firstColumnMaxLeftDigits = 0;

    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < columns; ++j) {

            // Get the value of the current matrix element.
            float value(*(values + j*rows + i));
            if (std::abs(value) < 0.00001) {
                value = 0.0;
            }
            
            // Count the digits to the left of the decimal point, and to the right of, and
            // including, the decimal point.
            int leftDigits = 0;
            int rightDigits = 0;
            CountDigits(ConvertDoubleToString(value), &leftDigits, &rightDigits);

            // Keep track of the maximum of the values we've seen
            // so far.
            if (j == 0) {
                // The left column is kept track of separately so we don't indent the
                // output unnecessarily.
                firstColumnMaxLeftDigits = std::max(firstColumnMaxLeftDigits, leftDigits);
            }
            maxLeftDigits = std::max(maxLeftDigits, leftDigits);
            maxRightDigits = std::max(maxRightDigits, rightDigits);
        }
    }

    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < columns; ++j) {

            // Get the value of the current matrix element.
            float value(*(values + j*rows + i));
            if (std::abs(value) < 0.00001) {
                value = 0.0;
            }

            // Get the string representation of the value.
            std::string string(ConvertDoubleToString(value));

            // Count the digits to the left of the decimal point, and to the right of, and
            // including, the decimal point.
            int leftDigits = 0;
            int rightDigits = 0;
            CountDigits(string, &leftDigits, &rightDigits);

            // Determine how much room should be made available for the digits to the
            // left of the decimal point. The first column is handled separately to avoid
            // unnecessary indenting.
            int columnMaxLeftDigits = 0;
            if (j == 0) {
                columnMaxLeftDigits = firstColumnMaxLeftDigits;
            } else {
                columnMaxLeftDigits = maxLeftDigits;
            }
            
            // Pad the left and right sides of the string with spaces
            // so that the decimal point will be properly aligned.
            assert(columnMaxLeftDigits - leftDigits >= 0);
            string = std::string(columnMaxLeftDigits - leftDigits, ' ') + string;
            if (j < columns - 1) {
                assert(maxRightDigits - rightDigits >= 0);
                string += std::string(maxRightDigits - rightDigits, ' ');
            }

            // If this isn't the first column in the row, add an extra
            // space to separate this value from the previous one.
            if (j > 0) {
                ostr << " ";
            }

            // Write out the value.
            ostr << string;
        }
        ostr << "\n";
    }
}

static std::string 
ConvertDoubleToString(double value)
{
    std::stringstream sstr;
    sstr << value;

    return sstr.str();
}

static void
CountDigits(const std::string &string, int *leftDigits, int *rightDigits)
{
    // If the number is displayed in scientific notation, don't align it with the decimal
    // point.
    std::string::size_type pos = string.find("e");
    if (pos != std::string::npos) {
        *leftDigits = string.length();
        *rightDigits = 0;
        return;
    }

    // Count how many digits appear to the left of the decimal point, and to the right of,
    // and including, the decimal point.
    pos = string.find(".");
    *leftDigits = 0;
    if (pos != std::string::npos) {
        *rightDigits = static_cast<int>(string.length() - pos);
        *leftDigits = string.length() - *rightDigits;
    } else {
        *leftDigits = string.length();
        *rightDigits = 0;
    }
}

} // namespace cgmath
