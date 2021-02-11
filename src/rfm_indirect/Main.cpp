// Copyright 2009 Drew Olbrich

// This program computes indirect illumination on a mesh in an RFM file.
// It assumes that the direct illumination has already been computed by rfm_discmesh.

#include <cstdlib>
#include <iostream>
#include <string>

#include <boost/lexical_cast.hpp>

#include <opt/ProgramOptionsParser.h>
#include <con/Streams.h>
#include <mesh/Mesh.h>
#include <meshrfm/ReadRfmFile.h>
#include <meshrfm/WriteRfmFile.h>
#include <cgmath/Vector3fProgramOption.h>

#include "MeshShader.h"

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

        MeshShader meshShader;
        meshShader.setMesh(&mesh);

        if (gOptions.specified("samples-per-vertex")) {
            meshShader.setSamplesPerVertex(
                gOptions.get("samples-per-vertex").as<unsigned>());
        }

        if (gOptions.specified("error-tolerance")) {
            meshShader.setAdaptiveSubdivisionErrorTolerance(
                gOptions.get("error-tolerance").as<float>());
        }

        if (gOptions.specified("min-edge-length")) {
            meshShader.setAdaptiveSubdivisionMinimumEdgeLength(
                gOptions.get("min-edge-length").as<float>());
        }

        if (gOptions.specified("max-pass")) {
            meshShader.setAdaptiveSubdivisionMaximumPass(
                gOptions.get("max-pass").as<int>());
        }

        if (gOptions.specified("sky-color")) {
            meshShader.setSkyColor(gOptions.get("sky-color").as<cgmath::Vector3f>());
        }

        if (gOptions.specified("direct-illumination-scale")) {
            meshShader.setDirectIlluminationScale(
                gOptions.get("direct-illumination-scale").as<float>());
        }

        if (gOptions.specified("diffuse-coefficient")) {
            meshShader.setDiffuseCoefficient(
                gOptions.get("diffuse-coefficient").as<float>());
        }

        if (gOptions.specified("bounces")) {
            meshShader.setBounces(gOptions.get("bounces").as<unsigned>());
        }

        meshShader.shadeMesh();

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
    gOptions.setProgramPurpose("Calculates indirect illumination for an RFM file.");

    gOptions.addRequiredPositionalOptions()
        ("input-file", "Input file")
        ("output-file", "Output file")
        ;

    gOptions.addOptions()
        ("sky-color", opt::value<cgmath::Vector3f>()->set_name("r g b"),
            "Sky/background color (0..1)")
        ("samples-per-vertex", opt::value<unsigned>(), 
            (std::string("Samples per vertex (default ")
                + boost::lexical_cast<std::string>(MeshShader::DEFAULT_SAMPLES_PER_VERTEX)
                + ")").c_str())
        ("error-tolerance", opt::value<float>(),
            (std::string("Adaptive subdivision error tolerance (default ")
                + boost::lexical_cast<std::string>(
                    MeshShader::DEFAULT_ADAPTIVE_SUBDIVISION_ERROR_TOLERANCE)
                + ")").c_str())
        ("min-edge-length", opt::value<float>(),
            (std::string("Adaptive subdivision minimum edge length (default ")
                + boost::lexical_cast<std::string>(
                    MeshShader::DEFAULT_ADAPTIVE_SUBDIVISION_MINIMUM_EDGE_LENGTH)
                + ")").c_str())
        ("max-pass", opt::value<int>(),
            "Adaptive subdvision maximum pass")
        ("direct-illumination-scale", opt::value<float>(), "Direct illumination scale")
        ("diffuse-coefficient", opt::value<float>(), "Diffuse coefficient")
        ("bounces", opt::value<unsigned>(), "Indirect illumination bounces")
        ;

    gOptions.parse(argc, argv);
}
