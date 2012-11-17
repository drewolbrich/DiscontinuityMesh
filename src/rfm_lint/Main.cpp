// Copyright 2009 Retroactive Fiasco.

// Scans for errors or potential problems in an RFM file.

#include <cstdlib>
#include <iostream>

#include <opt/ProgramOptionsParser.h>
#include <con/Streams.h>
#include <mesh/Mesh.h>
#include <meshrfm/ReadRfmFile.h>
#include <meshrfm/WriteRfmFile.h>

#include "MeshLinter.h"

static opt::ProgramOptionsParser gOptions;

// Parse the command line arguments.
static void ParseCommandLineArguments(int argc, char **argv);

int
main(int argc, char **argv)
{
    try {

        ParseCommandLineArguments(argc, argv);

        con::info << "Reading RFM file \"" << gOptions.get("input-file").as<std::string>()
            << "\"." << std::endl;

        mesh::Mesh mesh;
        meshrfm::ReadRfmFile(&mesh, gOptions.get("input-file").as<std::string>());

        MeshLinter meshLinter;
        meshLinter.setMesh(&mesh);

        if (gOptions.specified("abs-tolerance")) {
            meshLinter.setAbsoluteTolerance(gOptions.get("abs-tolerance").as<float>());
        }

        if (gOptions.specified("rel-tolerance")) {
            meshLinter.setRelativeTolerance(gOptions.get("rel-tolerance").as<float>());
        }

        if (gOptions.specified("output-file")) {
            meshLinter.setShouldMarkErrors(true);
        }

        meshLinter.testMesh();

        if (gOptions.specified("output-file")) {
            con::info << "Writing RFM file \"" << gOptions.get("output-file").as<std::string>()
                << "\"." << std::endl;
            meshrfm::WriteRfmFile(mesh, gOptions.get("output-file").as<std::string>());
        }

    } catch (const std::exception &exception) {
        con::error << exception.what() << std::endl;
        exit(EXIT_FAILURE);
    }

    return EXIT_SUCCESS;
}

static void
ParseCommandLineArguments(int argc, char **argv)
{
    gOptions.setUsageSummary("input.rfm [options]");
    gOptions.setProgramPurpose("Scans for errors in an RFM file.");

    gOptions.addRequiredPositionalOptions()
        ("input-file", "Input file")
        ;

    gOptions.addOptions()
        ("output-file", opt::value<std::string>(), "Output file with marked errors")
        ("abs-tolerance", opt::value<float>(), "Absolute tolerance")
        ("rel-tolerance", opt::value<float>(), "Relative tolerance")
        ;

    gOptions.parse(argc, argv);
}
