// Copyright 2008 Retroactive Fiasco.

#include "ProgramName.h"

#include <cassert>
#include <iostream>

#include <boost/filesystem/path.hpp>

namespace con {

static std::string gProgramName;

static std::string gPushedProgramName;

void
SetProgramName(const std::string &programName)
{
    // The 'leaf' function was changed to 'filename' in Boost 1.37,
    // so it might be necessary to change the following line at some point.
    gProgramName = boost::filesystem::path(programName).leaf();
}

const std::string &
GetProgramName()
{
    // con::SetProgramName must have been called
    // with argv[0] as an argument when the program started.
    assert(gProgramName != "");

    return gProgramName;
}

void 
PushProgramName()
{
    // There isn't really a stack of program name values.
    // Only one level of pushing is supported.
    assert(gPushedProgramName == "");

    gPushedProgramName = gProgramName;
}

void 
PopProgramName()
{
    gProgramName = gPushedProgramName;
    gPushedProgramName = "";
}

} // namespace con
