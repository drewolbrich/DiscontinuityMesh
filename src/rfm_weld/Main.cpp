// Copyright 2009 Retroactive Fiasco.

// This program detects perimeter edges of a mesh (edges with only one adjacent face),
// and welds together the vertices and edges that lie close together.

#include <cstdlib>
#include <iostream>

#include <con/Streams.h>
#include <meshweld/Welder.h>
#include <opt/ProgramOptionsParser.h>
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

        con::info << "Welding mesh perimeter." << std::endl;

        meshweld::Welder welder;
        welder.setMesh(&mesh);

        if (gOptions.specified("abs-tolerance")) {
            welder.setAbsoluteTolerance(gOptions.get("abs-tolerance").as<float>());
        }

        if (gOptions.specified("rel-tolerance")) {
            welder.setRelativeTolerance(gOptions.get("rel-tolerance").as<float>());
        }

        if (gOptions.specified("merge-any")) {
            welder.setMergeAny(true);
        }

        if (gOptions.specified("mark-regions")) {
            welder.setMarkFacesWithDebugColors(true);
        }

        if (gOptions.specified("weld-flagged-edges")) {
            welder.setCandidateEdgeAttributeKey(
                mesh.getAttributeKey("__meshsplit_Splitter_FlaggedEdge",
                    mesh::AttributeKey::BOOL));
        }

        size_t oldVertexCount = mesh.vertexCount();
        size_t oldEdgeCount = mesh.edgeCount();

        welder.weld();

        con::info << "Removed " << oldVertexCount - mesh.vertexCount()
            << " vertices and " << oldEdgeCount - mesh.edgeCount()
            << " edges." << std::endl;

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
    gOptions.setProgramPurpose("Welds together neighboring perimeter edges of a mesh.");

    gOptions.addRequiredPositionalOptions()
        ("input-file", "Input file")
        ("output-file", "Output file")
        ;

    gOptions.addOptions()
        ("abs-tolerance", opt::value<float>(), "Absolute tolerance")
        ("rel-tolerance", opt::value<float>(), "Relative tolerance")
        ("merge-any", "Allow any edges to be merged, not just perimeter edges");
        ;

    gOptions.addDebugOptions()
        ("mark-regions", "Mark faces of each region with a color")
        ("weld-flagged-edges", "Weld only edges flagged with rfm_split")
        ;

    gOptions.parse(argc, argv);
}
