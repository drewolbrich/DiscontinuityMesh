// Copyright 2008 Retroactive Fiasco.

#ifndef OPT__PROGRAM_OPTIONS_PARSER__INCLUDED
#define OPT__PROGRAM_OPTIONS_PARSER__INCLUDED

#include <string>
#include <vector>

#include <boost/program_options.hpp>
#include <boost/shared_ptr.hpp>

namespace opt {

// This effectively makes 'opt' an alias for boost::program_options,
// so that the namespace may be more conveniently accessed.
using namespace boost::program_options;

// ProgramOptionsParser
//
// This class is a wrapper around boost::program_options to simplify its use.
//
// Example:
//
//     opt::ProgramOptionsParser gOptions;
//    
//     gOptions.setUsageSummary("input.jpg output.jpg [options]");
//     gOptions.setProgramPurpose("Inverts a JPG file.");
//    
//     gOptions.addRequiredPositionalOptions()
//         ("input-file", "Input JPG file")
//         ("output-file", "Output JPG file")
//         ;
//    
//     gOptions.addOptions()
//         ("hsv", "Invert image in HSV space")
//         ("quality", opt::value<int>(), "Quality (0..100)")
//         ("filter", opt::value<std::string>(), "Filter to use (box, tent, quadratic)")
//         ;
//
//     // The following options are only displayed in debug builds, 
//     // or when the --debug-help flag is specified. This allows the program
//     // to have lots of debugging options without cluttering up 
//     // the standard usage message.
//     gOptions.addDebugOptions()
//         ("histogram", "Print histogram of the image")
//         ;
//
//     gOptions.parse(argc, argv);
//    
//     if (gOptions.specified("hsv")) {
//         ...
//     }
//    
//     if (gOptions.specified("quality")) {
//         int quality = gOptions.get("quality").as<int>();
//         ...
//     }

class ProgramOptionsParser
{
public:
    ProgramOptionsParser();
    ~ProgramOptionsParser();

    // The one-line list of program options for the usage message.
    void setUsageSummary(const std::string &usageSummary);

    // The one-line textual description of the program for the usage message.
    void setProgramPurpose(const std::string &programPurpose);

    // Add required positional command line options that are not specified with flags.
    boost::program_options::options_description_easy_init addRequiredPositionalOptions();

    // Add optional positional command line options that are not specified with flags.
    boost::program_options::options_description_easy_init addOptionalPositionalOptions();

    // Add optional command line options.
    boost::program_options::options_description_easy_init addOptions();

    // Add debugging command line options. These are only displayed in debug builds,
    // or when the '--debug-help' flag is used.
    boost::program_options::options_description_easy_init addDebugOptions();

    // Parse the command line arguments specified by the user.
    // This function will throw an exception if there's a syntax error.
    void parse(int argc, char **argv);

    // Print the usage message. This is only valid after 'parse' has been called.
    void printUsageMessage();

    // Returns true if a program option was specified on the command line.
    bool specified(const std::string &name) const;

    // Returns the last specified option of all options in a vector of option names,
    // or the empty sring if none is found. This allows for a sequence of
    // mutually exclusive options to be specified, with only the final
    // one having effect.
    std::string getLastSpecifiedOfOptionNameVector(
        const std::vector<std::string> &optionNameVector) const;

    // Returns the value of a program option.
    // Example: 
    //     gOptions.get("output-file").as<std::string>()
    const boost::program_options::variable_value &get(const std::string &name) const;

    // Returns the index of a program option, or -1 if the option was not
    // specified on the command line. This allows programs to have options
    // override other options, depending on their position on the command line.
    int getOptionIndex(const std::string &name) const;

private:
    // Concatenate one options description with another.
    // This is different than boost::program_options::options_description::add
    // in that the options are individidually added, rather than
    // adding them together as a group. This lumps all options together
    // in the usage message in a single block, rather than separating each group
    // with a blank line.
    void concatenateOptions(boost::program_options::options_description *lhs,
        const boost::program_options::options_description &rhs);

    // Concatenate an options description to a positional options description.
    void concatenatePositionalOptionsWithOptions(
        boost::program_options::positional_options_description *lhs,
        const boost::program_options::options_description &rhs);

    // Get all optional options.
    void getOptionsDescription(boost::program_options::options_description *optionsDescription);

    // Get all positional options.
    void getPositionalOptionsDescription(boost::program_options::positional_options_description 
        *positionalOptionsDescription);

    // Parse the command line options.
    void parseOptions(int argc, char **argv,
        const boost::program_options::options_description &optionsDescription,
        boost::program_options::positional_options_description &positionalOptionsDescription);

    // Prints an error message if one or more required arguments are missing.
    void checkForMissingRequiredPositionalArguments(int argc);

    typedef std::vector<boost::shared_ptr<boost::program_options::option_description> >
        OptionsDescriptionVector;

    std::string mUsageSummary;
    std::string mProgramPurpose;

    boost::program_options::options_description mRequiredPositionalOptionsDescription;
    boost::program_options::options_description mOptionalPositionalOptionsDescription;
    boost::program_options::options_description mVisibleOptionsDescription;
    boost::program_options::options_description mDebugOptionsDescription;
    boost::program_options::options_description mHelpOptionsDescription;

    boost::shared_ptr<boost::program_options::basic_parsed_options<char> > mParsedOptions;
    boost::program_options::variables_map mVariablesMap;

    bool mHasParsedOptions;
};

} // namespace opt

#endif // OPT__PROGRAM_OPTIONS_PARSER__INCLUDED
