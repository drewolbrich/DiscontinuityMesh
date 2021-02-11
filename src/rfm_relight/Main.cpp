// Copyright 2010 Drew Olbrich

// Adjusts the precomputed direct and indirect lighting stored in an RFM file.

#include <cstdlib>
#include <iostream>

#include <opt/ProgramOptionsParser.h>
#include <con/Streams.h>
#include <mesh/Mesh.h>
#include <meshrfm/ReadRfmFile.h>
#include <meshrfm/WriteRfmFile.h>

#include "MeshRelighter.h"

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

        MeshRelighter meshRelighter;

        meshRelighter.setMesh(&mesh);

        if (gOptions.specified("intensity")) {
            meshRelighter.setIntensity(gOptions.get("intensity").as<float>());
        }
        if (gOptions.specified("saturation")) {
            meshRelighter.setSaturation(gOptions.get("saturation").as<float>());
        }
        if (gOptions.specified("direct-intensity")) {
            meshRelighter.setDirectIntensity(gOptions.get("direct-intensity").as<float>());
        }
        if (gOptions.specified("direct-saturation")) {
            meshRelighter.setDirectSaturation(gOptions.get("direct-saturation").as<float>());
        }
        if (gOptions.specified("indirect-intensity")) {
            meshRelighter.setIndirectIntensity(gOptions.get("indirect-intensity").as<float>());
        }
        if (gOptions.specified("indirect-saturation")) {
            meshRelighter.setIndirectSaturation(gOptions.get("indirect-saturation").as<float>());
        }

        meshRelighter.relightMesh();
        
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
    gOptions.setProgramPurpose("Adjusts the precomputed direct and indirect "
        "lighting stored in an RFM file.");

    gOptions.addRequiredPositionalOptions()
        ("input-file", "Input file")
        ("output-file", "Output file")
        ;

    gOptions.addOptions()
        ("intensity", opt::value<float>(), "Global intensity")
        ("saturation", opt::value<float>(), "Global saturation")
        ("direct-intensity", opt::value<float>(), "Direct lighting intensity")
        ("direct-saturation", opt::value<float>(), "Direct lighting saturation")
        ("indirect-intensity", opt::value<float>(), "Indirect lighting intensity")
        ("indirect-saturation", opt::value<float>(), "Indirect lighting saturation")
        ;

    gOptions.parse(argc, argv);
}
