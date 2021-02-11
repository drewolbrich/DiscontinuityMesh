// Copyright 2009 Drew Olbrich

// Transforms the vertices and normals of a mesh.

#include <cstdlib>
#include <iostream>

#include <opt/ProgramOptionsParser.h>
#include <con/Streams.h>
#include <cgmath/Vector3fProgramOption.h>
#include <cgmath/Vector3f.h>
#include <cgmath/Matrix4f.h>
#include <cgmath/AngleOperations.h>
#include <mesh/Mesh.h>
#include <mesh/MeshOperations.h>
#include <meshrfm/ReadRfmFile.h>
#include <meshrfm/WriteRfmFile.h>

static opt::ProgramOptionsParser gOptions;

// Parse the command line arguments.
static void ParseCommandLineArguments(int argc, char **argv);

// Create the transformation matrix to apply to the mesh.
static cgmath::Matrix4f CreateTransformationMatrix();

int
main(int argc, char **argv)
{
    try {

        ParseCommandLineArguments(argc, argv);

        cgmath::Matrix4f matrix = CreateTransformationMatrix();

        con::info << "Reading RFM file \"" << gOptions.get("input-file").as<std::string>()
            << "\"." << std::endl;

        mesh::Mesh mesh;
        meshrfm::ReadRfmFile(&mesh, gOptions.get("input-file").as<std::string>());

        mesh::Transform(&mesh, matrix);

        con::info << "Writing RFM file \"" << gOptions.get("output-file").as<std::string>()
            << "\"." << std::endl;

        meshrfm::WriteRfmFile(mesh, gOptions.get("output-file").as<std::string>());

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
    gOptions.setUsageSummary("input.rfm output.rfm [options]");
    gOptions.setProgramPurpose("Applies a linear transformation to an RFM file. "
        "First a scale is applied, then a rotation, then a translation.");

    gOptions.addRequiredPositionalOptions()
        ("input-file", "Input file")
        ("output-file", "Output file")
        ;

    gOptions.addOptions()
        ("scale", opt::value<cgmath::Vector3f>()->set_name("sx sy sz"), 
            "Scale vector")
        ("rotate", opt::value<cgmath::Vector3f>()->set_name("rx ry rz"), 
            "Rotation vector (degrees, rotation order YXZ)")
        ("translate", opt::value<cgmath::Vector3f>()->set_name("tx ty tz"), 
            "Translation vector")
        ;

    gOptions.parse(argc, argv);
}

static cgmath::Matrix4f 
CreateTransformationMatrix()
{
    cgmath::Matrix4f matrix = cgmath::Matrix4f::IDENTITY;

    if (gOptions.specified("scale")) {
        matrix = cgmath::Matrix4f::fromScale(
            gOptions.get("scale").as<cgmath::Vector3f>())*matrix;
    }

    if (gOptions.specified("rotate")) {
        cgmath::Vector3f angleVector = gOptions.get("rotate").as<cgmath::Vector3f>();

        unsigned rotationAxisOrder[3] = { 1, 0, 2 };  // YXZ
        for (unsigned index = 0; index < 3; ++index) {
            unsigned rotationAxis = rotationAxisOrder[index];

            cgmath::Vector3f rotationVector = cgmath::Vector3f::ZERO;
            rotationVector[rotationAxis] = 1.0;

            matrix = cgmath::Matrix4f::fromRotation(rotationVector,
                cgmath::DegreesToRadians(angleVector[rotationAxis]))*matrix;
        }
    }

    if (gOptions.specified("translate")) {
        matrix = cgmath::Matrix4f::fromTranslation(
            gOptions.get("translate").as<cgmath::Vector3f>())*matrix;
    }

    return matrix;
}
