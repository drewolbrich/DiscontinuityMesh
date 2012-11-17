// Copyright 2010 Retroactive Fiasco.

#ifndef OS__UNIX_ERROR_VALUE__INCLUDED
#define OS__UNIX_ERROR_VALUE__INCLUDED

#include "ErrorValue.h"

namespace os {

// UnixErrorValue
//
// Implementation of ErrorValue for Unix.

class UnixErrorValue : public ErrorValue
{
public:
    UnixErrorValue();
    virtual ~UnixErrorValue();

    // For ErrorValue:
    virtual UnixErrorValue *clone();
    virtual void setToSystemError();
    virtual bool isDefined() const;
    virtual bool success() const;
    virtual const std::string asString() const;

private:
    bool mIsDefined;
    int mErrno;
};

} // namespace os

#endif // OS__UNIX_ERROR_VALUE__INCLUDED
