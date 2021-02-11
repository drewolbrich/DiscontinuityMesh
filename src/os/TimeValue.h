// Copyright 2008 Drew Olbrich

#ifndef OS__TIME_VALUE__INCLUDED
#define OS__TIME_VALUE__INCLUDED

#include <iostream>
#include <string>

#include <os/Platform.h>

#if OS_UNIX
#include <sys/time.h>
#include <time.h>
#endif

#if OS_WIN32
#include <windows.h>
#endif

namespace os {

// TimeValue
//
// This class is a wrapper around the C standard library timeval struct. It can accurately
// represent an amount of time between one microsecond and about 68 years. Beyond this
// range, time is undefined.

class TimeValue
{
public:
    TimeValue();
#if OS_UNIX
    explicit TimeValue(const struct timeval &timeVal);
#endif
    TimeValue(long int seconds, long int microseconds);
    TimeValue(double seconds);
    ~TimeValue();

    static const int SECONDS_PER_MINUTE = 60;
    static const int SECONDS_PER_HOUR = 60*SECONDS_PER_MINUTE;
    static const int SECONDS_PER_DAY = 24*SECONDS_PER_HOUR;
    static const int SECONDS_PER_MONTH = 30*SECONDS_PER_DAY;
    static const int SECONDS_PER_YEAR = 365*SECONDS_PER_DAY;

    friend const TimeValue operator+(const TimeValue &lhs, const TimeValue &rhs);
    friend const TimeValue operator-(const TimeValue &lhs, const TimeValue &rhs);

    const TimeValue &operator+=(const TimeValue &rhs);
    const TimeValue &operator-=(const TimeValue &rhs);

    // These functions are currently implemented as primitive operations
    // on doubles, and so will result in a loss of precision.
    friend const TimeValue operator*(const TimeValue &lhs, const TimeValue &rhs);
    friend const TimeValue operator/(const TimeValue &lhs, const TimeValue &rhs);
    const TimeValue &operator*=(const TimeValue &rhs);
    const TimeValue &operator/=(const TimeValue &rhs);

    friend bool operator==(const TimeValue &lhs, const TimeValue &rhs);
    friend bool operator!=(const TimeValue &lhs, const TimeValue &rhs);

    friend bool operator<(const TimeValue &lhs, const TimeValue &rhs);
    friend bool operator>(const TimeValue &lhs, const TimeValue &rhs);
    friend bool operator<=(const TimeValue &lhs, const TimeValue &rhs);
    friend bool operator>=(const TimeValue &lhs, const TimeValue &rhs);

#if OS_UNIX
    // Return the TimeValue in the form of a C standard library
    // timeval struct.
    struct timeval asTimeVal() const;
#endif

#if OS_UNIX
    // Return the TimeValue in the form of a C standard library
    // timespec struct.
    struct timespec asTimeSpec() const;
#endif

    // Convert the TimeValue to a float, as seconds.
    float asFloat() const;

    // Convert the TimeValue to a double, as seconds.
    double asDouble() const;

    // Return the TimeValue as a number of milliseconds.
    int asMilliseconds() const;

    // When TimeValue::asString is outputting a string consisting of primary and
    // secondary units (e.g. "8 minutes and 23 seconds"), if the number of
    // primary units is this many or larger, the secondary units
    // will be omitted. 
    static const int PRIMARY_UNITS_LIMIT = 10;

    // Convert the TimeValue to a human-readable string.
    const std::string asString() const;

    void setSeconds(long int seconds);
    void setMicroseconds(long int microseconds);

    long int seconds() const;
    long int microseconds() const;

    // This allows TimeValues to be displayed using 
    // C++ output streams.
    friend std::ostream &operator<<(std::ostream &ostr, const TimeValue &timeValue);

private:
    // Number of microseconds in a second.
    static const long int MICROSEC_PER_SEC = 1000000L;

    long int mSeconds;
    long int mMicroseconds;
};

} // namespace os

#endif // OS__TIME_VALUE__INCLUDED
