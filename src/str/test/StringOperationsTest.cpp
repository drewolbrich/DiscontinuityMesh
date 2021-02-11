// Copyright 2008 Drew Olbrich

#include <cppunit/extensions/HelperMacros.h>

#include <str/StringOperations.h>

class StringOperationsTest : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE(StringOperationsTest);
    CPPUNIT_TEST(testReplaceAllSubstrings);
    CPPUNIT_TEST(testRemoveLeadingAndTrailingWhitespace);
    CPPUNIT_TEST_SUITE_END();

public:
    void setUp() {
    }

    void tearDown() {
    }

    void testReplaceAllSubstrings() {
        CPPUNIT_ASSERT(str::ReplaceAllSubstrings(
                        "foobarfoobarfoo", "foo", "XYZZY")
            == "XYZZYbarXYZZYbarXYZZY");

        CPPUNIT_ASSERT(str::ReplaceAllSubstrings(
                        "foobarfoobarfoo", "foo", "fool")
            == "foolbarfoolbarfool");
    }

    void testRemoveLeadingAndTrailingWhitespace() {
        CPPUNIT_ASSERT(str::RemoveLeadingAndTrailingWhitespace("  foo  ")
            == "foo");

        CPPUNIT_ASSERT(str::RemoveLeadingAndTrailingWhitespace("  foo  bar  ")
            == "foo  bar");

        CPPUNIT_ASSERT(str::RemoveLeadingAndTrailingWhitespace("foo") == "foo");

        CPPUNIT_ASSERT(str::RemoveLeadingAndTrailingWhitespace("  ") == "");

        CPPUNIT_ASSERT(str::RemoveLeadingAndTrailingWhitespace("") == "");
    }
};

CPPUNIT_TEST_SUITE_REGISTRATION(StringOperationsTest);
