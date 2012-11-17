// Copyright 2009 Retroactive Fiasco.

// Splits the faces of a mesh where they intersect each other.

#include <cstdlib>
#include <iostream>

#include <opt/ProgramOptionsParser.h>
#include <con/Streams.h>
#include <mesh/Mesh.h>
#include <meshsplit/Splitter.h>
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

        con::info << "Splitting mesh." << std::endl;

        meshsplit::Splitter splitter;
        splitter.setMesh(&mesh);

        if (gOptions.specified("abs-tolerance")) {
            splitter.setAbsoluteTolerance(gOptions.get("abs-tolerance").as<float>());
        }

        if (gOptions.specified("rel-tolerance")) {
            splitter.setRelativeTolerance(gOptions.get("rel-tolerance").as<float>());
        }

        if (gOptions.specified("mark-intersections")) {
            splitter.setMarkIntersectionsWithCylinders(true);
        }

        if (gOptions.specified("flag-new-edges")) {
            splitter.setFlaggedEdgeBooleanAttributeKey(
                mesh.getAttributeKey("__meshsplit_Splitter_FlaggedEdge",
                    mesh::AttributeKey::BOOL));
        }

        // Split the intersecting faces.
        splitter.splitFaces();

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
    gOptions.setProgramPurpose("Splits the faces in an RFM file where they intersect.");

    gOptions.addRequiredPositionalOptions()
        ("input-file", "Input file")
        ("output-file", "Output file")
        ;

    gOptions.addOptions()
        ("abs-tolerance", opt::value<float>(), "Absolute tolerance")
        ("rel-tolerance", opt::value<float>(), "Relative tolerance")
        ;

    gOptions.addDebugOptions()
        ("mark-intersections", "Mark intersections with cylinders")
        ("flag-new-edges", "Flag new edges for testing with rfm_weld")
        ;

    gOptions.parse(argc, argv);
}
