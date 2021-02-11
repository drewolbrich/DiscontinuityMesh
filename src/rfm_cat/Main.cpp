// Copyright 2009 Drew Olbrich

// Concatenates together two RFM files.
//
// Note: This program blindly copies materials and textures, without making
// any attempt to resolve conflicts. Doing so would be nontrivial.
// Ideally we'd like to ensure that materials with the same name wound up with
// the same index, and materials with different names were assigned different indices.
// Optionally, materials that had the same name but that were not strictly
// indentical could be renamed.

#include <cstdlib>
#include <iostream>

#include <opt/ProgramOptionsParser.h>
#include <con/Streams.h>
#include <meshrfm/ReadRfmFile.h>
#include <meshrfm/WriteRfmFile.h>
#include <mesh/Mesh.h>
#include <mesh/Concatenate.h>

static opt::ProgramOptionsParser gOptions;

// Parse the command line arguments.
static void ParseCommandLineArguments(int argc, char **argv);

int
main(int argc, char **argv)
{
    try {

        ParseCommandLineArguments(argc, argv);

        mesh::Mesh targetMesh;
        meshrfm::ReadRfmFile(&targetMesh, gOptions.get("input-file-1").as<std::string>());

        mesh::Mesh sourceMesh;
        meshrfm::ReadRfmFile(&sourceMesh, gOptions.get("input-file-2").as<std::string>());

        mesh::Concatenate(&targetMesh, sourceMesh);

        meshrfm::WriteRfmFile(targetMesh, gOptions.get("output-file").as<std::string>());

        con::info << "Done." << std::endl;

    } catch (const std::exception &exception) {
        con::error << exception.what() << std::endl;
        exit(EXIT_FAILURE);
    }

    return EXIT_SUCCESS;
}

static void
ParseCommandLineArguments(int argc, char **argv)
{
    gOptions.setUsageSummary("input1.rfm input2.rfm output.rfm");
    gOptions.setProgramPurpose("Concatenates together two RFM files.");

    gOptions.addRequiredPositionalOptions()
        ("input-file-1", "First input file")
        ("input-file-2", "Second input file")
        ("output-file", "Output file")
        ;

    gOptions.parse(argc, argv);
}
