// Copyright 2010 Retroactive Fiasco.

#include <cppunit/extensions/HelperMacros.h>

#include <iostream>

#include <cgmath/Vector3f.h>
#include <cgmath/ColorOperations.h>

using cgmath::HsvToRgb;
using cgmath::RgbToHsv;

class ColorOperationsTest : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE(ColorOperationsTest);
    CPPUNIT_TEST(testRgbToHsv);
    CPPUNIT_TEST_SUITE_END();

public:
    void setUp() {
    }

    void tearDown() {
    }

    void testRgbToHsv() {
        for (float r = 0.0; r <= 2.0; r += 0.25) {
            for (float g = 0.0; g <= 2.0; g += 0.25) {
                for (float b = 0.0; b <= 2.0; b += 0.25) {
                    cgmath::Vector3f rgbInput(r, g, b);
                    cgmath::Vector3f rgbOutput = HsvToRgb(RgbToHsv(rgbInput));
                    CPPUNIT_ASSERT(rgbInput.equivalent(rgbOutput, 0.001));
                }
            }
        }
    }
};

CPPUNIT_TEST_SUITE_REGISTRATION(ColorOperationsTest);
