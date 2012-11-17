// Copyright 2010 Retroactive Fiasco.

// Creates an RFM file containing a random array of boxes.

#include <cstdlib>
#include <iostream>

#include <opt/ProgramOptionsParser.h>
#include <con/Streams.h>
#include <meshrfm/ReadRfmFile.h>
#include <meshrfm/WriteRfmFile.h>
#include <mesh/Mesh.h>

#include "WorldCreator.h"

static opt::ProgramOptionsParser gOptions;

// Parse the command line arguments.
static void ParseCommandLineArguments(int argc, char **argv);

int
main(int argc, char **argv)
{
    try {

        ParseCommandLineArguments(argc, argv);

        mesh::Mesh inputMesh;
        meshrfm::ReadRfmFile(&inputMesh, gOptions.get("input-file").as<std::string>());

        mesh::Mesh outputMesh;
        WorldCreator worldCreator;
        worldCreator.setWidth(gOptions.get("width").as<int>());
        worldCreator.setHeight(gOptions.get("width").as<int>());
        worldCreator.setDepth(gOptions.get("depth").as<int>());
        worldCreator.setInputMesh(&inputMesh);
        worldCreator.setOutputMesh(&outputMesh);
        worldCreator.setBoxCount(gOptions.get("box-count").as<int>());
        worldCreator.create();

        meshrfm::WriteRfmFile(outputMesh, gOptions.get("output-file").as<std::string>());

    } catch (const std::exception &exception) {
        con::error << exception.what() << std::endl;
        exit(EXIT_FAILURE);
    }

    return EXIT_SUCCESS;
}

static void
ParseCommandLineArguments(int argc, char **argv)
{
    gOptions.setUsageSummary("input.rfm output.rfm [options]");
    gOptions.setProgramPurpose("Creates an RFM containing a random array of boxes.");

    gOptions.addRequiredPositionalOptions()
        ("input-file", "Input file")
        ("output-file", "Output file")
        ;

    gOptions.addOptions()
        ("width", opt::value<int>()->default_value(10), "Width")
        ("height", opt::value<int>()->default_value(10), "Height")
        ("depth", opt::value<int>()->default_value(10), "Depth")
        ("box-count", opt::value<int>()->default_value(100), "Number of boxes to create")
        ;

    // These only appear in the usage message when the --help-debug flag is specified.
    gOptions.addDebugOptions()
        ("validate", "Validate the output")
        ;

    gOptions.parse(argc, argv);
}
