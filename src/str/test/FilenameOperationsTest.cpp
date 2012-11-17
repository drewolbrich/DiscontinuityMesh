// Copyright 2008 Retroactive Fiasco.

#include <cppunit/extensions/HelperMacros.h>

#include <str/FilenameOperations.h>

class FilenameOperationsTest : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE(FilenameOperationsTest);
    CPPUNIT_TEST(testGetIncludedFilename);
    CPPUNIT_TEST(testGetFilenameExtension);
    CPPUNIT_TEST(testGetFilenamePath);
    CPPUNIT_TEST(testRemoveFilenameExtension);
    CPPUNIT_TEST(testRemovePathFromFilename);
    CPPUNIT_TEST(testLocalizeFilename);
    CPPUNIT_TEST_SUITE_END();

public:
    void setUp() {
    }

    void tearDown() {
    }

    void testGetIncludedFilename() {
        CPPUNIT_ASSERT(str::GetIncludedFilename("a.x", "b.x")
            == "b.x");
        CPPUNIT_ASSERT(str::GetIncludedFilename("foo/bar/a.x", "b.x")
            == "foo/bar/b.x");
        CPPUNIT_ASSERT(str::GetIncludedFilename("foo/bar/a.x", "baz/b.x")
            == "foo/bar/baz/b.x");
        CPPUNIT_ASSERT(str::GetIncludedFilename("foo/bar/a.x", "/usr/tmp/b.x")
            == "/usr/tmp/b.x");
        CPPUNIT_ASSERT(str::GetIncludedFilename("foo/bar/a.x", "../b.x")
            == "foo/bar/../b.x");
    }

    void testGetFilenameExtension() {
        CPPUNIT_ASSERT(str::GetFilenameExtension("foo.bar.baz") == "baz");
        CPPUNIT_ASSERT(str::GetFilenameExtension("foo") == "");
        CPPUNIT_ASSERT(str::GetFilenameExtension("foo.") == "");
    }

    void testGetFilenamePath() {
        CPPUNIT_ASSERT(str::GetFilenamePath("/foo/bar/baz") == "/foo/bar/");
        CPPUNIT_ASSERT(str::GetFilenamePath("/foo/bar/baz/") == "/foo/bar/baz/");
        CPPUNIT_ASSERT(str::GetFilenamePath("foo") == "");
        CPPUNIT_ASSERT(str::GetFilenamePath("/foo") == "/");
    }

    void testRemoveFilenameExtension() {
        CPPUNIT_ASSERT(str::RemoveFilenameExtension("foo.bar.baz") == "foo.bar");
        CPPUNIT_ASSERT(str::RemoveFilenameExtension("foo") == "foo");
        CPPUNIT_ASSERT(str::RemoveFilenameExtension("foo.") == "foo");
    }

    void testRemovePathFromFilename() {
        CPPUNIT_ASSERT(str::RemovePathFromFilename("foo/bar/baz.xxx") 
            == "baz.xxx");
        CPPUNIT_ASSERT(str::RemovePathFromFilename("baz.xxx") == "baz.xxx");
    }

    void testLocalizeFilename() {
        CPPUNIT_ASSERT(str::LocalizeFilename("foo/bar/baz.xxx", "foo/") 
            == "bar/baz.xxx");
        CPPUNIT_ASSERT(str::LocalizeFilename("foo/bar/baz.xxx", "") 
            == "foo/bar/baz.xxx");
        CPPUNIT_ASSERT(str::LocalizeFilename("/foo/bar/baz.xxx", "/") 
            == "foo/bar/baz.xxx");
        CPPUNIT_ASSERT(str::LocalizeFilename("baz.xxx", "") == "baz.xxx");
    }
};

CPPUNIT_TEST_SUITE_REGISTRATION(FilenameOperationsTest);
