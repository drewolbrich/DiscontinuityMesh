// Copyright 2008 Retroactive Fiasco.

#include "NestedAssert.h"

namespace unittest {

static std::string gNestedAssertMessage;

void
SetNestedAssertMessage(const std::string &message)
{
    gNestedAssertMessage = message;
}

const std::string &
GetNestedAssertMessage()
{
    return gNestedAssertMessage;
}

} // namespace unittest
