// Copyright 2009 Retroactive Fiasco.

// This program triangulates all faces of a mesh that are not already triangles.

#include <cstdlib>
#include <iostream>

#include <con/Streams.h>
#include <opt/ProgramOptionsParser.h>
#include <mesh/Mesh.h>
#include <meshrfm/ReadRfmFile.h>
#include <meshrfm/WriteRfmFile.h>
#include <mesh/Triangulator.h>

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

        unsigned oldFaceCount = mesh.faceCount();

        mesh::Triangulator triangulator;
        triangulator.setMesh(&mesh);
        triangulator.triangulate();

        con::info << "Triangulated " << triangulator.triangulatedFaces()
            << " faces." << std::endl;

        con::info << "Created " << mesh.faceCount() - oldFaceCount
            << " faces." << std::endl;

        con::info << "Writing RFM file \"" << gOptions.get("output-file").as<std::string>()
            << "\"." << std::endl;
        meshrfm::WriteRfmFile(mesh, gOptions.get("output-file").as<std::string>());

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
    gOptions.setProgramPurpose("Triangulates non-triangular faces of a mesh.");

    gOptions.addRequiredPositionalOptions()
        ("input-file", "Input file")
        ("output-file", "Output file")
        ;

    gOptions.parse(argc, argv);
}
