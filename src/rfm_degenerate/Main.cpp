// Copyright 2009 Retroactive Fiasco.

// Removes degenerate faces from an RFM file.

#include <cstdlib>
#include <iostream>

#include <opt/ProgramOptionsParser.h>
#include <con/Streams.h>
#include <mesh/Mesh.h>
#include <mesh/MeshOperations.h>
#include <mesh/DegenerateFaceCollapser.h>
#include <meshrfm/ReadRfmFile.h>
#include <meshrfm/WriteRfmFile.h>

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

        if (!mesh::AllFacesAreTriangles(mesh)) {
            con::error << "Some faces are not triangles." << std::endl;
            exit(EXIT_FAILURE);
        }

        mesh::DegenerateFaceCollapser degenerateFaceCollapser;
        degenerateFaceCollapser.setMesh(&mesh);

        if (gOptions.specified("abs-tolerance")) {
            degenerateFaceCollapser.setAbsoluteTolerance(
                gOptions.get("abs-tolerance").as<float>());
        }

        if (gOptions.specified("rel-tolerance")) {
            degenerateFaceCollapser.setRelativeTolerance(
                gOptions.get("rel-tolerance").as<float>());
        }

#if 0
        size_t oldVertexCount = mesh.vertexCount();
        size_t oldEdgeCount = mesh.edgeCount();
#endif
        size_t oldFaceCount = mesh.faceCount();

        con::info << "Collapsing degenerate faces." << std::endl;

        degenerateFaceCollapser.collapseDegenerateFaces();

#if 0
        con::info << "Removed " << oldVertexCount - mesh.vertexCount()
            << " vertices." << std::endl;
        con::info << "Removed " << oldEdgeCount - mesh.edgeCount()
            << " edges." << std::endl;
#endif
        con::info << "Removed " << oldFaceCount - mesh.faceCount()
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
    gOptions.setProgramPurpose("Removes degenerate faces from an RFM file.");

    gOptions.addRequiredPositionalOptions()
        ("input-file", "Input file")
        ("output-file", "Output file")
        ;

    gOptions.addOptions()
        ("abs-tolerance", opt::value<float>(), "Absolute tolerance")
        ("rel-tolerance", opt::value<float>(), "Relative tolerance")
        ;

    gOptions.parse(argc, argv);
}
