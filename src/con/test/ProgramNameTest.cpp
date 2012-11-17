// Copyright 2008 Retroactive Fiasco.

#include <cppunit/extensions/HelperMacros.h>

#include <os/Platform.h>
#include <con/ProgramName.h>

class ProgramNameTest : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE(ProgramNameTest);
    CPPUNIT_TEST(testProgramName);
    CPPUNIT_TEST_SUITE_END();

public:
    void setUp() {
    }

    void tearDown() {
    }

    void testProgramName() {
        struct SafePushProgramName {
            SafePushProgramName() { con::PushProgramName(); }
            ~SafePushProgramName() { con::PopProgramName(); }
        } safePushProgramName;
    
#if OS_UNIX
        con::SetProgramName("/usr/tmp/foo");
        CPPUNIT_ASSERT(con::GetProgramName() == "foo");
#endif

#if OS_WIN32
        con::SetProgramName("C:\\windows\\foo.exe");
        CPPUNIT_ASSERT(con::GetProgramName() == "foo");
#endif
    }
};

CPPUNIT_TEST_SUITE_REGISTRATION(ProgramNameTest);
