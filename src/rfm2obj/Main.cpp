// Copyright 2008 Retroactive Fiasco.

// This program converts an RFM file to a Wavefront OBJ file.

#include <exception>

#include <opt/ProgramOptionsParser.h>
#include <con/Streams.h>
#include <mesh/Mesh.h>
#include <mesh/AssignNormals.h>
#include <meshrfm/ReadRfmFile.h>
#include <meshobj/WriteObjFile.h>

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

        con::info << "Writing OBJ file \""
            << gOptions.get("output-file").as<std::string>()
            << "\"." << std::endl;

        meshobj::WriteObjFile(mesh, gOptions.get("output-file").as<std::string>());

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
    gOptions.setUsageSummary("input.rfm output.obj [options]");
    gOptions.setProgramPurpose("Converts an RFM file to a Wavefront OBJ file.");

    gOptions.addRequiredPositionalOptions()
        ("input-file", "Input RFM file")
        ("output-file", "Output OBJ file")
        ;

    gOptions.parse(argc, argv);
}
