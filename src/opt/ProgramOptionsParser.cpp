// Copyright 2008 Retroactive Fiasco.

#include "ProgramOptionsParser.h"

#include <cstdlib>
#include <cassert>
#include <exception>

#include <con/ProgramName.h>
#include <con/ProgramOptions.h>
#include <con/Streams.h>

namespace opt {

ProgramOptionsParser::ProgramOptionsParser()
    : mUsageSummary(),
      mProgramPurpose(),
      mRequiredPositionalOptionsDescription(),
      mOptionalPositionalOptionsDescription(),
      mVisibleOptionsDescription(""),
      mDebugOptionsDescription("Debugging Options"),
      mHelpOptionsDescription(""),
      mParsedOptions(),
      mVariablesMap(),
      mHasParsedOptions(false)
{
}

ProgramOptionsParser::~ProgramOptionsParser()
{
}

void
ProgramOptionsParser::setUsageSummary(const std::string &usageSummary)
{
    mUsageSummary = usageSummary;
}

void 
ProgramOptionsParser::setProgramPurpose(const std::string &programPurpose)
{
    mProgramPurpose = programPurpose;
}

boost::program_options::options_description_easy_init
ProgramOptionsParser::addRequiredPositionalOptions()
{
    return mRequiredPositionalOptionsDescription.add_options();
}

boost::program_options::options_description_easy_init
ProgramOptionsParser::addOptionalPositionalOptions()
{
    return mOptionalPositionalOptionsDescription.add_options();
}

boost::program_options::options_description_easy_init
ProgramOptionsParser::addOptions()
{
    return mVisibleOptionsDescription.add_options();
}

boost::program_options::options_description_easy_init
ProgramOptionsParser::addDebugOptions()
{
    return mDebugOptionsDescription.add_options();
}

void
ProgramOptionsParser::parse(int argc, char **argv)
{
    // It's assumed that this function will never be called twice.
    assert(!mHasParsedOptions);

    mHasParsedOptions = true;

    con::SetProgramName(argv[0]);

    mHelpOptionsDescription.add_options()
        ("help", "Show this message");

    // Only display the --help-debug flag in optimized builds.
    // In debug builds, the debug options help is always displayed.
#ifndef DEBUG
    // Only allow display and allow use of the '--help-debug' option
    // if the program has any debug options.
    if (!mDebugOptionsDescription.options().empty()) {
        mHelpOptionsDescription.add_options()
            ("help-debug", "Show debugging options");
    }
#endif

    // Get all optional options.
    boost::program_options::options_description optionsDescription;
    getOptionsDescription(&optionsDescription);

    // Get all positional options.
    boost::program_options::positional_options_description positionalOptionsDescription;
    getPositionalOptionsDescription(&positionalOptionsDescription);

    // Parse the options.
    parseOptions(argc, argv, optionsDescription, positionalOptionsDescription);

    // Check if any of the required positional options are missing.
    checkForMissingRequiredPositionalArguments(argc);
}

void
ProgramOptionsParser::printUsageMessage()
{
    // The function 'parse' must already have been called.
    assert(mHasParsedOptions);

    boost::program_options::options_description options("Options");

    concatenateOptions(&options, mVisibleOptionsDescription);
    concatenateOptions(&options, con::GetLogLevelProgramOptions());
    concatenateOptions(&options, mHelpOptionsDescription);

    if (!mDebugOptionsDescription.options().empty()) {
        // In debug builds, we always display the debug options help.
        // In optimized builds, we only display them when the --help-debug flag is used.
#ifndef DEBUG
        if (specified("help-debug")) {
#endif
            options.add(mDebugOptionsDescription);
#ifndef DEBUG
        }
#endif
    }

    con::indent << "Usage: " << con::GetProgramName() << " " << mUsageSummary << std::endl;
    con::indent << "Which: " << mProgramPurpose << std::endl;

    // This already includes a trailing newline, so we don't add another one.
    con::out << options;
    con::out.flush();

    exit(EXIT_SUCCESS);
}

bool
ProgramOptionsParser::specified(const std::string &name) const
{
    // The function 'parse' must already have been called.
    assert(mHasParsedOptions);

    return mVariablesMap.count(name) > 0;
}

std::string
ProgramOptionsParser::getLastSpecifiedOfOptionNameVector(
    const std::vector<std::string> &optionNameVector) const
{
    int maxIndex = -1;
    std::string result = "";

    for (size_t optionNameIndex = 0; optionNameIndex < optionNameVector.size(); 
         ++optionNameIndex) {
        const std::string &name = optionNameVector[optionNameIndex];
        int index = getOptionIndex(name);
        if (index > maxIndex) {
            maxIndex = index;
            result = name;
        }
    }

    return result;
}

const boost::program_options::variable_value &
ProgramOptionsParser::get(const std::string &name) const
{
    // The function 'parse' must already have been called.
    assert(mHasParsedOptions);

    return mVariablesMap[name];
}

int
ProgramOptionsParser::getOptionIndex(const std::string &name) const
{
    for (size_t index = 0; index < mParsedOptions->options.size(); ++index) {
        if (mParsedOptions->options[index].string_key == name) {
            return index;
        }
    }

    return -1;
}

void
ProgramOptionsParser::concatenateOptions(boost::program_options::options_description *lhs,
    const boost::program_options::options_description &rhs)
{
    typedef std::vector<boost::shared_ptr<boost::program_options::option_description> >
        BoostOptionsVector;

    const BoostOptionsVector &options = rhs.options();
    for (BoostOptionsVector::const_iterator iterator = options.begin();
         iterator != options.end(); ++iterator) {
        (*lhs).add(*iterator);
    }
}

void
ProgramOptionsParser::concatenatePositionalOptionsWithOptions(
    boost::program_options::positional_options_description *lhs,
    const boost::program_options::options_description &rhs)
{
    const OptionsDescriptionVector &optionalPositionalOptions = rhs.options();
    for (OptionsDescriptionVector::const_iterator iterator = optionalPositionalOptions.begin();
         iterator != optionalPositionalOptions.end(); ++iterator) {
        const boost::program_options::option_description &optionsDescription = **iterator;
        lhs->add(optionsDescription.long_name().c_str(), 1);
    }
}

void
ProgramOptionsParser::getOptionsDescription(boost::program_options::options_description 
    *optionsDescription)
{
    optionsDescription->add(mHelpOptionsDescription);
    optionsDescription->add(mRequiredPositionalOptionsDescription);
    optionsDescription->add(mOptionalPositionalOptionsDescription);
    optionsDescription->add(mVisibleOptionsDescription);
    optionsDescription->add(con::GetLogLevelProgramOptions());
    optionsDescription->add(mDebugOptionsDescription);
}

void
ProgramOptionsParser::getPositionalOptionsDescription(
    boost::program_options::positional_options_description *positionalOptionsDescription)
{
    concatenatePositionalOptionsWithOptions(positionalOptionsDescription,
        mRequiredPositionalOptionsDescription);
    concatenatePositionalOptionsWithOptions(positionalOptionsDescription,
        mOptionalPositionalOptionsDescription);
}

void
ProgramOptionsParser::parseOptions(int argc, char **argv,
    const boost::program_options::options_description &optionsDescription,
    boost::program_options::positional_options_description &positionalOptionsDescription)
{
    try {
        mParsedOptions.reset(
            new boost::program_options::basic_parsed_options<char>(
                boost::program_options::command_line_parser(argc, argv)
                .options(optionsDescription)
                .positional(positionalOptionsDescription)
                .run()));
        boost::program_options::store(*mParsedOptions.get(), mVariablesMap);
        boost::program_options::notify(mVariablesMap);
    } catch (const std::exception &exception) {
        con::error << "Error parsing command line arguments: " 
            << exception.what() << std::endl;
        exit(EXIT_FAILURE);
    }

    // Parse logging options like --quiet and --verbose.
    con::ParseLogLevelProgramOptions(*this);

    // Print the usage message if it's explicitly requested.
    if (specified("help")
        || specified("help-debug")) {
        printUsageMessage();
    }
}

void
ProgramOptionsParser::checkForMissingRequiredPositionalArguments(int argc)
{
    const OptionsDescriptionVector &requiredPositionalOptions 
        = mRequiredPositionalOptionsDescription.options();
    for (OptionsDescriptionVector::const_iterator iterator = requiredPositionalOptions.begin();
         iterator != requiredPositionalOptions.end(); ++iterator) {
        const boost::program_options::option_description &optionDescription = **iterator;
        if (!specified(optionDescription.long_name())) {

            // If required arguments are missing, but no options at all are
            // specified, print the usage message.
            if (argc == 1) {
                printUsageMessage();
            }

            con::error << "One or more required command line arguments are missing. " 
                << "Type" << con::HARD_SPACE << "\"" << con::GetProgramName()
                << con::HARD_SPACE << "--help\" for more information."
                << std::endl;
            exit(EXIT_FAILURE);
        }
    }
}

} // namespace opt
