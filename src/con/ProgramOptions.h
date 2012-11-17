// Copyright 2008 Retroactive Fiasco.

#ifndef CON__PROGRAM_OPTIONS__INCLUDED
#define CON__PROGRAM_OPTIONS__INCLUDED

#include <boost/program_options.hpp>

namespace opt {
class ProgramOptionsParser;
}

namespace con {

// Return a Boost program options description for the following flags:
//     --quiet    Suppress warnings
//     --verbose  Display progress information
//     --debug    Display debugging messages
boost::program_options::options_description GetLogLevelProgramOptions();

// Parse the logging program options returned by GetLogLevelProgramOptions.
void ParseLogLevelProgramOptions(const opt::ProgramOptionsParser &programOptionsParser);

} // namespace con

#endif // CON__PROGRAM_OPTIONS__INCLUDED
