// Copyright 2008 Retroactive Fiasco.

#include <cppunit/extensions/HelperMacros.h>

#include <boost/lexical_cast.hpp>

#include <os/Platform.h>
#include <os/TimeValue.h>

using os::TimeValue;

class TimeValueTest : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE(TimeValueTest);
    CPPUNIT_TEST(testConstructorTimeVal);
    CPPUNIT_TEST(testConstructorSecondsMilliseconds);
    CPPUNIT_TEST(testConstructorDouble);
    CPPUNIT_TEST(testAssignment);
    CPPUNIT_TEST(testCopyConstructor);
    CPPUNIT_TEST(testAddition);
    CPPUNIT_TEST(testSubtraction);
    CPPUNIT_TEST(testPlusEquals);
    CPPUNIT_TEST(testMinusEquals);
    CPPUNIT_TEST(testMultiplication);
    CPPUNIT_TEST(testDivision);
    CPPUNIT_TEST(testMultiplyEquals);
    CPPUNIT_TEST(testDivideEquals);
    CPPUNIT_TEST(testEqual);
    CPPUNIT_TEST(testNotEqual);
    CPPUNIT_TEST(testLessThan);
    CPPUNIT_TEST(testGreaterThan);
    CPPUNIT_TEST(testLessThanOrEqual);
    CPPUNIT_TEST(testGreaterThanOrEqual);
    CPPUNIT_TEST(testAsTimeVal);
    CPPUNIT_TEST(testAsTimeSpec);
    CPPUNIT_TEST(testAsFloat);
    CPPUNIT_TEST(testAsDouble);
    CPPUNIT_TEST(testAsMilliseconds);
    CPPUNIT_TEST(testAsString);
    CPPUNIT_TEST_SUITE_END();

public:
    void setUp() {
    }

    void tearDown() {
    }

    void testConstructorTimeVal() {
#if OS_UNIX
        struct timeval timeVal;
        timeVal.tv_sec = 100;
        timeVal.tv_usec = 500000;
        TimeValue t1(timeVal);
        CPPUNIT_ASSERT(t1 == 100.5);
#endif
    }

    void testConstructorSecondsMilliseconds() {
        TimeValue t1(100, 500000);
        CPPUNIT_ASSERT(t1 == 100.5);
    }

    void testConstructorDouble() {
        TimeValue t1(100.5);
        CPPUNIT_ASSERT(t1 == 100.5);
    }

    void testAssignment() {
        TimeValue t1(100.0);
        TimeValue t2(200.0);
        t1 = t2;
        CPPUNIT_ASSERT(t1 == 200.0);
    }

    void testCopyConstructor() {
        TimeValue t1(100.0);
        TimeValue t2(t1);
        CPPUNIT_ASSERT(t2 == 100.0);
    }

    void testAddition() {
        TimeValue t1(100.0);
        TimeValue t2(200.0);
        TimeValue t3 = t1 + t2;
        CPPUNIT_ASSERT(t3 == 300.0);
    }

    void testSubtraction() {
        TimeValue t1(300.0);
        TimeValue t2(200.0);
        TimeValue t3 = t1 - t2;
        CPPUNIT_ASSERT(t3 == 100.0);
    }

    void testPlusEquals() {
        TimeValue t1(100.0);
        TimeValue t2(200.0);
        t1 += t2;
        CPPUNIT_ASSERT(t1 == 300.0);
    }

    void testMinusEquals() {
        TimeValue t1(300.0);
        TimeValue t2(200.0);
        t1 -= t2;
        CPPUNIT_ASSERT(t1 == 100.0);
    }

    void testMultiplication() {
        TimeValue t1(100.0);
        TimeValue t2(2.0);
        TimeValue t3 = t1*t2;
        CPPUNIT_ASSERT(t3 == 200.0);
    }

    void testDivision() {
        TimeValue t1(100.0);
        TimeValue t2(2.0);
        TimeValue t3 = t1/t2;
        CPPUNIT_ASSERT(t3 == 50.0);
    }

    void testMultiplyEquals() {
        TimeValue t1(100.0);
        TimeValue t2(2.0);
        t1 *= t2;
        CPPUNIT_ASSERT(t1 == 200.0);
    }

    void testDivideEquals() {
        TimeValue t1(100.0);
        TimeValue t2(2.0);
        t1 /= t2;
        CPPUNIT_ASSERT(t1 == 50.0);
    }

    void testEqual() {
        TimeValue t1(100.0);
        TimeValue t2(100.0);
        CPPUNIT_ASSERT(t1 == t2);
    }

    void testNotEqual() {
        TimeValue t1(100.0);
        TimeValue t2(200.0);
        CPPUNIT_ASSERT(t1 != t2);
    }

    void testLessThan() {
        TimeValue t1(100.0);
        TimeValue t2(200.0);
        CPPUNIT_ASSERT(t1 < t2);
    }

    void testGreaterThan() {
        TimeValue t1(200.0);
        TimeValue t2(100.0);
        CPPUNIT_ASSERT(t1 > t2);
    }

    void testLessThanOrEqual() {
        TimeValue t1(100.0);
        TimeValue t2(200.0);
        TimeValue t3(100.0);
        CPPUNIT_ASSERT(t1 <= t2);
        CPPUNIT_ASSERT(t1 <= t3);
    }

    void testGreaterThanOrEqual() {
        TimeValue t1(200.0);
        TimeValue t2(100.0);
        TimeValue t3(200.0);
        CPPUNIT_ASSERT(t1 >= t2);
        CPPUNIT_ASSERT(t1 >= t3);
    }

    void testAsTimeVal() {
#if OS_UNIX
        TimeValue t1(100.5);
        struct timeval timeVal = t1.asTimeVal();
        CPPUNIT_ASSERT(timeVal.tv_sec == 100);
        CPPUNIT_ASSERT(timeVal.tv_usec == 500000);
#endif
    }

    void testAsTimeSpec() {
#if OS_UNIX
        TimeValue t1(100.5);
        struct timespec timeSpec = t1.asTimeSpec();
        CPPUNIT_ASSERT(timeSpec.tv_sec == 100);
        CPPUNIT_ASSERT(timeSpec.tv_nsec == 500000000);
#endif        
    }

    void testAsFloat() {
        TimeValue t1(100.5);
        float d = t1.asFloat();
        CPPUNIT_ASSERT(d == 100.5);
    }

    void testAsDouble() {
        TimeValue t1(100.5);
        double d = t1.asDouble();
        CPPUNIT_ASSERT(d == 100.5);
    }

    void testAsMilliseconds() {
        TimeValue t1(100.5);
        int i1 = t1.asMilliseconds();
        CPPUNIT_ASSERT(i1 == 100500);

        TimeValue t2(100.007);
        int i2 = t2.asMilliseconds();
        CPPUNIT_ASSERT(i2 == 100007);

        TimeValue t3(0.0001);
        int i3 = t3.asMilliseconds();
        CPPUNIT_ASSERT(i3 == 0);
    }

    void testAsString() {
        TimeValue t1;

        // Negative values.
        t1 = -1.5;
        CPPUNIT_ASSERT(t1.asString() == "minus 1.5 seconds");

        // Values less than 3 seconds.
        t1 = 0.00001;
        CPPUNIT_ASSERT(t1.asString() == "0 seconds");
        t1 = 1.0;
        CPPUNIT_ASSERT(t1.asString() == "1 second");
        t1 = 2.51;
        CPPUNIT_ASSERT(t1.asString() == "2.5 seconds");
        t1 = 0.251;
        CPPUNIT_ASSERT(t1.asString() == "0.25 seconds");
        t1 = 0.0251;
        CPPUNIT_ASSERT(t1.asString() == "0.025 seconds");
        t1 = 0.00251;
        CPPUNIT_ASSERT(t1.asString() == "0.0025 seconds");
        t1 = 0.000251;
        CPPUNIT_ASSERT(t1.asString() == "0.0002 seconds");
        t1 = 0.0000251;
        CPPUNIT_ASSERT(t1.asString() == "0 seconds");

        // Values of 3 or more seconds.
        t1 = 3;
        CPPUNIT_ASSERT(t1.asString() == "3 seconds");
        t1 = 4;
        CPPUNIT_ASSERT(t1.asString() == "4 seconds");
        t1 = 59.9;
        CPPUNIT_ASSERT(t1.asString() == "59 seconds");

        std::string limitMinusOne = boost::lexical_cast<std::string>(
            TimeValue::PRIMARY_UNITS_LIMIT - 1);
        std::string limit = boost::lexical_cast<std::string>(
            TimeValue::PRIMARY_UNITS_LIMIT + 0);

        // Minutes
        t1 = 60;
        CPPUNIT_ASSERT(t1.asString() == "1 minute");
        t1 = 61;
        CPPUNIT_ASSERT(t1.asString() == "1 minute and 1 second");
        t1 = 60 + 59;
        CPPUNIT_ASSERT(t1.asString() == "1 minute and 59 seconds");
        t1 = 60*(TimeValue::PRIMARY_UNITS_LIMIT - 1) + 59;
        CPPUNIT_ASSERT(t1.asString() == limitMinusOne + " minutes and 59 seconds");
        t1 = 60*TimeValue::PRIMARY_UNITS_LIMIT;
        CPPUNIT_ASSERT(t1.asString() == limit + " minutes");
        t1 = 60*59 + 59;
        CPPUNIT_ASSERT(t1.asString() == "59 minutes");

        // Hours
        t1 = 60*60;
        CPPUNIT_ASSERT(t1.asString() == "1 hour");
        t1 = 60*60 + 60*59;
        CPPUNIT_ASSERT(t1.asString() == "1 hour and 59 minutes");
        t1 = 60*60*(TimeValue::PRIMARY_UNITS_LIMIT - 1) + 60*59;
        CPPUNIT_ASSERT(t1.asString() == limitMinusOne + " hours and 59 minutes");
        t1 = 60*60*TimeValue::PRIMARY_UNITS_LIMIT + 60*59;
        CPPUNIT_ASSERT(t1.asString() == limit + " hours");
        t1 = 60*60*23 + 60*59;
        CPPUNIT_ASSERT(t1.asString() == "23 hours");

        // Days
        t1 = 24*60*60;
        CPPUNIT_ASSERT(t1.asString() == "1 day");
        t1 = 24*60*60 + 60*60*23;
        CPPUNIT_ASSERT(t1.asString() == "1 day and 23 hours");
        t1 = 24*60*60*(TimeValue::PRIMARY_UNITS_LIMIT - 1) + 60*60*23;
        CPPUNIT_ASSERT(t1.asString() == limitMinusOne + " days and 23 hours");
        t1 = 24*60*60*TimeValue::PRIMARY_UNITS_LIMIT + 60*60*23;
        CPPUNIT_ASSERT(t1.asString() == limit + " days");
        t1 = 24*60*60*29 + 60*60*23;
        CPPUNIT_ASSERT(t1.asString() == "29 days");

        // Months
        t1 = 30*24*60*60;
        CPPUNIT_ASSERT(t1.asString() == "1 month");
        t1 = 30*24*60*60 + 24*60*60*29;
        CPPUNIT_ASSERT(t1.asString() == "1 month and 29 days");
        t1 = 30*24*60*60*(TimeValue::PRIMARY_UNITS_LIMIT - 1) + 24*60*60*29;
        CPPUNIT_ASSERT(t1.asString() == limitMinusOne + " months and 29 days");
        t1 = 30*24*60*60*TimeValue::PRIMARY_UNITS_LIMIT + 24*60*60*29;
        CPPUNIT_ASSERT(t1.asString() == limit + " months");
        t1 = 30*24*60*60*11 + 24*60*60*29;
        CPPUNIT_ASSERT(t1.asString() == "11 months");

        // Years
        t1 = 365*24*60*60;
        CPPUNIT_ASSERT(t1.asString() == "1 year");
        t1 = 365*24*60*60 + 30*24*60*60*11;
        CPPUNIT_ASSERT(t1.asString() == "1 year and 11 months");
        t1 = 365*24*60*60*(TimeValue::PRIMARY_UNITS_LIMIT - 1) + 30*24*60*60*11;
        CPPUNIT_ASSERT(t1.asString() == limitMinusOne + " years and 11 months");
        t1 = 365*24*60*60*TimeValue::PRIMARY_UNITS_LIMIT + 30*24*60*60*11;
        CPPUNIT_ASSERT(t1.asString() == limit + " years");
        t1 = 365*24*60*60*67 + 30*24*60*60*11;
        CPPUNIT_ASSERT(t1.asString() == "67 years");
    }
};

CPPUNIT_TEST_SUITE_REGISTRATION(TimeValueTest);
