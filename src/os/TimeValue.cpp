// Copyright 2008 Drew Olbrich

#include <sstream>

#include "TimeValue.h"

namespace os {

TimeValue::TimeValue()
    : mSeconds(0),
      mMicroseconds(0)
{
}

#if OS_UNIX
TimeValue::TimeValue(const struct timeval &timeVal)
    : mSeconds(timeVal.tv_sec),
      mMicroseconds(timeVal.tv_usec)
{
}
#endif

TimeValue::TimeValue(long int seconds, long int microseconds)
    : mSeconds(seconds),
      mMicroseconds(microseconds)
{
}

TimeValue::TimeValue(double seconds)
    : mSeconds((long int) seconds),
      mMicroseconds((long int) ((seconds - (double) mSeconds)*MICROSEC_PER_SEC))
{
}

TimeValue::~TimeValue()
{
}

const TimeValue 
operator+(const TimeValue &lhs, const TimeValue &rhs)
{
    // operator+ is implemented in terms of operator+= because
    // operator+= is most efficient, as it never
    // requires the creation of a temporary. If the caller really
    // cares about efficiency, they should use operator+=.

    return TimeValue(lhs) += rhs;
}

const TimeValue 
operator-(const TimeValue &lhs, const TimeValue &rhs)
{
    return TimeValue(lhs) -= rhs;
}

const TimeValue &
TimeValue::operator+=(const TimeValue &rhs)
{
    mSeconds += rhs.mSeconds;
    mMicroseconds += rhs.mMicroseconds;

    if (mSeconds >= 0) {
        if (mMicroseconds >= MICROSEC_PER_SEC) {
            mSeconds += 1;
            mMicroseconds -= MICROSEC_PER_SEC;
        } else if (mMicroseconds < 0) {
            mSeconds -= 1;
            mMicroseconds += MICROSEC_PER_SEC;
        }
    } else {
        if (mMicroseconds <= -MICROSEC_PER_SEC) {
            mSeconds -= 1;
            mMicroseconds += MICROSEC_PER_SEC;
        } else if (mMicroseconds > 0) {
            mSeconds += 1;
            mMicroseconds -= MICROSEC_PER_SEC;
        }
    }

    return *this;
}

const TimeValue &
TimeValue::operator-=(const TimeValue &rhs)
{
    mSeconds -= rhs.mSeconds;
    mMicroseconds -= rhs.mMicroseconds;

    if (mSeconds >= 0) {
        if (mMicroseconds >= MICROSEC_PER_SEC) {
            mSeconds += 1;
            mMicroseconds -= MICROSEC_PER_SEC;
        } else if (mMicroseconds < 0) {
            mSeconds -= 1;
            mMicroseconds += MICROSEC_PER_SEC;
        }
    } else {
        if (mMicroseconds <= -MICROSEC_PER_SEC) {
            mSeconds -= 1;
            mMicroseconds += MICROSEC_PER_SEC;
        } else if (mMicroseconds > 0) {
            mSeconds += 1;
            mMicroseconds -= MICROSEC_PER_SEC;
        }
    }

    return *this;
}

const TimeValue 
operator*(const TimeValue &lhs, const TimeValue &rhs)
{
    return TimeValue(lhs) *= rhs;
}

const TimeValue 
operator/(const TimeValue &lhs, const TimeValue &rhs)
{
    return TimeValue(lhs) /= rhs;
}

const TimeValue &
TimeValue::operator*=(const TimeValue &rhs)
{
    // This is currently implemented as a primitive operation on
    // doubles, which means that there will be a loss of precision.
    *this = TimeValue(asDouble()*rhs.asDouble());

    return *this;
}

const TimeValue &
TimeValue::operator/=(const TimeValue &rhs)
{
    // This is currently implemented as a primitive operation on
    // doubles, which means that there will be a loss of precision.
    *this = TimeValue(asDouble()/rhs.asDouble());

    return *this;
}

bool
operator==(const TimeValue &lhs, const TimeValue &rhs)
{
    return lhs.mSeconds == rhs.mSeconds
        && lhs.mMicroseconds == rhs.mMicroseconds;
}

bool
operator!=(const TimeValue &lhs, const TimeValue &rhs)
{
    return lhs.mSeconds != rhs.mSeconds
        || lhs.mMicroseconds != rhs.mMicroseconds;
}

bool
operator<(const TimeValue &lhs, const TimeValue &rhs)
{
    if (lhs.mSeconds < rhs.mSeconds) {
        return true;
    }

    if (lhs.mSeconds > rhs.mSeconds) {
        return false;
    }
    
    return lhs.mMicroseconds < rhs.mMicroseconds;
}

bool
operator>(const TimeValue &lhs, const TimeValue &rhs)
{
    if (lhs.mSeconds > rhs.mSeconds) {
        return true;
    }

    if (lhs.mSeconds < rhs.mSeconds) {
        return false;
    }
    
    return lhs.mMicroseconds > rhs.mMicroseconds;
}

bool
operator<=(const TimeValue &lhs, const TimeValue &rhs)
{
    if (lhs.mSeconds < rhs.mSeconds) {
        return true;
    }

    if (lhs.mSeconds > rhs.mSeconds) {
        return false;
    }
    
    return lhs.mMicroseconds <= rhs.mMicroseconds;
}

bool
operator>=(const TimeValue &lhs, const TimeValue &rhs)
{
    if (lhs.mSeconds > rhs.mSeconds) {
        return true;
    }

    if (lhs.mSeconds < rhs.mSeconds) {
        return false;
    }
    
    return lhs.mMicroseconds >= rhs.mMicroseconds;
}

#if OS_UNIX
struct timeval
TimeValue::asTimeVal() const
{
    struct timeval timeVal;
    timeVal.tv_sec = mSeconds;
    timeVal.tv_usec = mMicroseconds;

    return timeVal;
}
#endif

#if OS_UNIX
struct timespec
TimeValue::asTimeSpec() const
{
    struct timespec timeSpec;
    timeSpec.tv_sec = mSeconds;
    timeSpec.tv_nsec = mMicroseconds*1000;

    return timeSpec;
}
#endif

float
TimeValue::asFloat() const
{
    return (float) mSeconds + (float) mMicroseconds/MICROSEC_PER_SEC;
}

double
TimeValue::asDouble() const
{
    return (double) mSeconds + (double) mMicroseconds/MICROSEC_PER_SEC;
}

int 
TimeValue::asMilliseconds() const
{
    return mSeconds*1000 + mMicroseconds/1000;
}

const std::string 
TimeValue::asString() const
{
    // The purpose of this function is to display the time as a
    // human-readable string. This means that we want to display as
    // little superfluous information as possible. We would rather not
    // print something like "5 hours, 41 minutes, and 15.1209 seconds".
    // 
    // Instead, we want to display output like this:
    //
    //     "1 year and 9 months"
    //     "5 months"
    //     "2 months and 26 days"
    //     "8 hours"
    //     "1 hour and 17 minutes"
    //     "31 seconds"
    //     "1.4 seconds"
    //     "0.085 seconds"

    std::ostringstream result;

    long int seconds = mSeconds;
    double fractionalSeconds = asDouble();

    // If the TimeValue represents a negative amount of time,
    // prepend the word "minus" to the output string, and then
    // negate the values we base the string on so that
    // everything works out correctly.
    if (mSeconds < 0
        || mMicroseconds < 0) { 
        result << "minus ";
        seconds *= -1;
        fractionalSeconds *= -1.0;
    }

    // If there are less than a few seconds, this is treated as a special
    // case because the units are always seconds.
    if (seconds < PRIMARY_UNITS_LIMIT) {
        if (fractionalSeconds < 0.0001) {
            // Beyond one ten-thousandth of a second, we might
            // as well register no time at all.
            result << "0 seconds";
        } else {
            // We want to print out a fractional number of seconds,
            // without showing more than two digits of precision.
            double precision;
            if (fractionalSeconds < 0.01) {
                precision = 10000.0;
            } else if (fractionalSeconds < 0.1) {
                precision = 1000.0;
            } else if (fractionalSeconds < 1.0) {
                precision = 100.0;
            } else {
                precision = 10.0;
            }
            fractionalSeconds 
                = ((int) (fractionalSeconds*precision)/precision);
            result << fractionalSeconds << " second";
            if (fractionalSeconds != 1.0) {
                result << "s";
            }
        }
        return std::string(result.str());
    }

    // If the TimeValue is 3 or more seconds, display the time
    // value in a form like "7 hours and 32 minutes".

    int years = seconds/SECONDS_PER_YEAR;
    seconds %= SECONDS_PER_YEAR;

    int months = seconds/SECONDS_PER_MONTH;
    seconds %= SECONDS_PER_MONTH;

    int days = seconds/SECONDS_PER_DAY;
    seconds %= SECONDS_PER_DAY;

    int hours = seconds/SECONDS_PER_HOUR;
    seconds %= SECONDS_PER_HOUR;

    int minutes = seconds/SECONDS_PER_MINUTE;
    seconds %= SECONDS_PER_MINUTE;

    std::string primaryUnit;
    std::string secondaryUnit;
    int primaryValue;
    int secondaryValue;

    if (years > 0) {
        primaryUnit = "year";
        primaryValue = years;
        secondaryUnit = "month";
        secondaryValue = months;
    } else if (months > 0) {
        primaryUnit = "month";
        primaryValue = months;
        secondaryUnit = "day";
        secondaryValue = days;
    } else if (days > 0) {
        primaryUnit = "day";
        primaryValue = days;
        secondaryUnit = "hour";
        secondaryValue = hours;
    } else if (hours > 0) {
        primaryUnit = "hour";
        primaryValue = hours;
        secondaryUnit = "minute";
        secondaryValue = minutes;
    } else if (minutes > 0) {
        primaryUnit = "minute";
        primaryValue = minutes;
        secondaryUnit = "second";
        secondaryValue = seconds;
    } else {
        primaryUnit = "second";
        primaryValue = seconds;
        secondaryUnit = "";
        secondaryValue = 0;
    }

    // The logic here is that if we're dealing with more than a few primary
    // units, we omit the secondary units completely, as they start to
    // get lost in the noise and don't mean much to the user
    // (e.g. "17 minutes"). If there are only a few primary units,
    // secondary units become important, and are displayed 
    // ("2 minutes and 18 seconds").
    result << primaryValue << " " << primaryUnit;
    if (primaryValue != 1) {
        result << "s";
    }
    if (primaryValue < PRIMARY_UNITS_LIMIT
        && secondaryValue != 0) {
        result << " and " << secondaryValue << " " << secondaryUnit;
        if (secondaryValue != 1) {
            result << "s";
        }
    }

    return std::string(result.str());
}

void 
TimeValue::setSeconds(long int seconds)
{
    mSeconds = seconds;
}

void TimeValue::setMicroseconds(long int microseconds)
{
    mMicroseconds = microseconds;
}

long int 
TimeValue::seconds() const
{
    return mSeconds;
}

long int 
TimeValue::microseconds() const
{
    return mMicroseconds;
}

std::ostream &
operator<<(std::ostream &ostr, const TimeValue &timeValue)
{
    return ostr << timeValue.asString();
}

} // namespace os
