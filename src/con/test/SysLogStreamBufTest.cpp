// Copyright 2008 Drew Olbrich

#include <cstdio>
#include <iostream>

#include <cstring>

#include <cppunit/extensions/HelperMacros.h>

#include <con/SysLogStreamBuf.h>
#include <con/StreamBuf.h>
#include <con/ProgramName.h>

using con::SysLogStreamBuf;

class SysLogStreamBufTest : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE(SysLogStreamBufTest);
    CPPUNIT_TEST(testSysLogStreamBuf);
    CPPUNIT_TEST_SUITE_END();

public:
    void setUp() {
    }

    void tearDown() {
    }

    void testSysLogStreamBuf() {
        struct SafePushProgramName {
            SafePushProgramName() { con::PushProgramName(); }
            ~SafePushProgramName() { con::PopProgramName(); }
        } safePushProgramName;

        con::SetProgramName("test");

        con::SysLogStreamBuf sysLogStreamBuf(LOG_ERR);
        std::ostream ostr(&sysLogStreamBuf);
    
        ostr << "SysLogStreamBufUnitTest" << std::endl;
    
        /* If the log file does not exist, we're probably not running
           under Linux, so skip this test. */
        FILE *file = fopen("/var/log/messages", "r");
        if (file == NULL) {
            return;
        }
    
        int line_count = 0;
        int found_line = 0;
        char buffer[4096];
        while (fgets(buffer, sizeof(buffer) - 1, file) != NULL) {
            if (strstr(buffer, "SysLogStreamBufUnitTest") != NULL) {
                found_line = line_count;
            }
            line_count++;
        }

        // The test output must have been found somewhere near
        // the end of the log file.
        CPPUNIT_ASSERT(found_line > line_count - 10);
        CPPUNIT_ASSERT(found_line < line_count);

        fclose(file);
    }
};

CPPUNIT_TEST_SUITE_REGISTRATION(SysLogStreamBufTest);
