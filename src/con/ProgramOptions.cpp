// Copyright 2008 Drew Olbrich

#include "ProgramOptions.h"

#include <opt/ProgramOptionsParser.h>

#include "Streams.h"
#include "LogLevel.h"

namespace con {

boost::program_options::options_description 
GetLogLevelProgramOptions()
{
    boost::program_options::options_description optionsDescription;

    optionsDescription.add_options()
        ("quiet", "Suppress warnings")
        ("verbose", "Display progress information")
        ("debug", "Display debugging messages")
// This seems like overkill for now, so simplify the usage message by leaving it out.
#if 0
        ("trace", "Display extended debugging messages")
#endif
        ;

    return optionsDescription;
}

void
ParseLogLevelProgramOptions(const opt::ProgramOptionsParser &programOptionsParser)
{
    std::vector<std::string> optionNameVector;
    optionNameVector.push_back("quiet");
    optionNameVector.push_back("verbose");
    optionNameVector.push_back("debug");
    optionNameVector.push_back("trace");

    std::string option = programOptionsParser.getLastSpecifiedOfOptionNameVector(
        optionNameVector);

    if (option == "quiet") {
        SetLogLevel(LOG_LEVEL_ERROR);
    } else if (option == "verbose") {
        SetLogLevel(LOG_LEVEL_INFO);
    } else if (option == "debug") {
        SetLogLevel(LOG_LEVEL_DEBUG);
    } else if (option == "trace") {
        SetLogLevel(LOG_LEVEL_TRACE);
    }
}

} // namespace con
