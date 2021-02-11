// Copyright 2008 Drew Olbrich

#include <cppunit/extensions/HelperMacros.h>
#include <sstream>

#include <except/SourceLine.h>

using except::SourceLine;

class SourceLineTest : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE(SourceLineTest);
    CPPUNIT_TEST(testConstructor);
    CPPUNIT_TEST(testStreamOutput);
    CPPUNIT_TEST_SUITE_END();

public:
    void setUp() {
    }

    void tearDown() {
    }

    void testConstructor() {
        SourceLine sourceLine("file", 100);
        CPPUNIT_ASSERT(sourceLine.file() == "file");
        CPPUNIT_ASSERT(sourceLine.line() == 100);
    }

    void testStreamOutput() {
        SourceLine sourceLine("file", 100);
        std::ostringstream ostr;
        ostr << sourceLine;
        CPPUNIT_ASSERT(ostr.str() == "file, line 100");
    }
};

CPPUNIT_TEST_SUITE_REGISTRATION(SourceLineTest);
