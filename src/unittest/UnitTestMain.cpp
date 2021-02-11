// Copyright 2008 Drew Olbrich

// This file is named UnitTestMain.cpp, and not Main.cpp, so the build system
// doesn't mistakenly build an executable for this directory.
// Normally, when the build system detects the presence of the file Main.cpp,
// it'll build an executable, rather than a library.

#include <exception>
#include <iostream>
#include <sstream>

#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/TestResult.h>
#include <cppunit/TestResultCollector.h>
#include <cppunit/TestRunner.h>

#include <con/Streams.h>
#include <con/ProgramName.h>
#include <con/LogLevel.h>
#include <opt/ProgramOptionsParser.h>

#include "PrettyProgressListener.h"
#include "PrettyCompilerOutputter.h"

static opt::ProgramOptionsParser gOptions;

static void SetProgramPurpose();

// This function acts as the "main" of every unit test executable
// that is created using this library. The individual unit tests do not
// define "main" themselves.

int
main(int argc, char **argv)
{
    // By default, unit tests have verbose output, because if we're running
    // them individually, we're probably debugging them. The build system
    // runs them with the --quiet flag unless it is run with verbose mode enabled.
    con::SetLogLevel(con::LOG_LEVEL_INFO);

    // The call to SetProgramPurpose needs to access the program name,
    // which is normally set via opt::ProgramOptionsParser::parse call.
    con::SetProgramName(argv[0]);

    SetProgramPurpose();

    gOptions.setUsageSummary("[options]");
    gOptions.parse(argc, argv);

    // Create a Test from all TextFixtures in the registry.
    CppUnit::Test *test = CppUnit::TestFactoryRegistry::getRegistry().makeTest();

    CppUnit::TestResult testResult;

    unittest::PrettyProgressListener progressListener;
    testResult.addListener(&progressListener);

    CppUnit::TestResultCollector testResultCollector;
    testResult.addListener(&testResultCollector);

    CppUnit::TestRunner testRunner;
    testRunner.addTest(test);

    try {

        testRunner.run(testResult);

        unittest::PrettyCompilerOutputter outputter(&testResultCollector, std::cerr);
        outputter.write();

    } catch (std::exception &exception) {

        std::cerr << "Exception: " << exception.what() << std::endl;
        return EXIT_FAILURE;

    } catch (...) {

        std::cerr << "Unexpected exception." << std::endl;
        return EXIT_FAILURE;
    }

    return testResultCollector.wasSuccessful() ? EXIT_SUCCESS : EXIT_FAILURE;
}

void
SetProgramPurpose()
{
    // It's assumed that all unit test executable filenames begin with this prefix.
    static const std::string unitTestExecutablePrefix = "test_";

    std::string libraryName = con::GetProgramName();
    if (libraryName.find(unitTestExecutablePrefix) != 0) {
        // The name of the executable does not follow the expected convention.
        return;
    }

    // Strip off the ".debug" suffix, if present.
    libraryName = libraryName.substr(unitTestExecutablePrefix.length());
    size_t pos = libraryName.find('.');
    if (pos != std::string::npos) {
        libraryName = libraryName.substr(0, pos);
    }

    std::ostringstream ostr;
    ostr << "Runs unit tests for the '" << libraryName << "' library.";

    gOptions.setProgramPurpose(ostr.str());
}
