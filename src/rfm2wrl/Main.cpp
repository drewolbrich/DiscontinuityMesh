// Copyright 2008 Retroactive Fiasco.

// This program converts an RFM file to a VRML 2.0 WRL file.

#include <exception>

#include <opt/ProgramOptionsParser.h>
#include <con/Streams.h>
#include <mesh/Mesh.h>
#include <mesh/AssignNormals.h>
#include <meshrfm/ReadRfmFile.h>
#include <cgmath/Vector3fProgramOption.h>

#include "WrlFileWriter.h"

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

        con::info << "Writing WRL file \"" << gOptions.get("output-file").as<std::string>()
            << "\"." << std::endl;

        WrlFileWriter wrlFileWriter;

        if (gOptions.specified("sky-color")) {
            wrlFileWriter.setSkyColor(gOptions.get("sky-color").as<cgmath::Vector3f>());
        }

        wrlFileWriter.writeWrlFile(mesh, gOptions.get("output-file").as<std::string>());

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
    gOptions.setUsageSummary("input.rfm output.wrl [options]");
    gOptions.setProgramPurpose("Converts an RFM file to a VRML 2.0 WRL file.");

    gOptions.addRequiredPositionalOptions()
        ("input-file", "Input RFM file")
        ("output-file", "Output WRL file")
        ;

    gOptions.addOptions()
        ("sky-color", opt::value<cgmath::Vector3f>()->set_name("r g b"),
            "Sky/background color (0..1)")
        ;

    gOptions.parse(argc, argv);
}
