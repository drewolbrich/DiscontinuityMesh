// Copyright 2009 Drew Olbrich

// Test of SphericalPointDistributor.

#include <cstdlib>
#include <iostream>

#include <opt/ProgramOptionsParser.h>
#include <con/Streams.h>
#include <cgmath/HemisphericalPointDistributor.h>
#include <mesh/Mesh.h>
#include <meshprim/BoxCreator.h>
#include <cgmath/Matrix4f.h>
#include <cgmath/Matrix3f.h>
#include <meshrfm/WriteRfmFile.h>

static opt::ProgramOptionsParser gOptions;

// Parse the command line arguments.
static void ParseCommandLineArguments(int argc, char **argv);

int
main(int argc, char **argv)
{
    try {

        ParseCommandLineArguments(argc, argv);

        cgmath::HemisphericalPointDistributor hemisphericalPointDistributor;
        hemisphericalPointDistributor.setPointCount(gOptions.get("points").as<unsigned>());
        hemisphericalPointDistributor.setDistribution(
            cgmath::HemisphericalPointDistributor::UNIFORM);
        hemisphericalPointDistributor.initialize();

        mesh::Mesh mesh;

        meshprim::BoxCreator boxCreator;
        boxCreator.setMesh(&mesh);
        boxCreator.setWidth(0.1);
        boxCreator.setHeight(boxCreator.width());
        boxCreator.setDepth(boxCreator.width());

        static const float SPHERE_RADIUS = 1.0;

        for (unsigned index = 0; index < hemisphericalPointDistributor.pointCount(); ++index) {

            cgmath::Vector3f direction = hemisphericalPointDistributor.getPoint(index);

            direction = cgmath::Vector3f(
                direction[0],
                direction[2],
                direction[1]);

            cgmath::Matrix3f orientation;
            cgmath::Vector3f up(0, 1, 0);
            if (direction.equivalent(up, 0.001)) {
                orientation = cgmath::Matrix3f::IDENTITY;
            } else {
                cgmath::Vector3f x = direction;
                cgmath::Vector3f y = direction.cross(up);
                cgmath::Vector3f z = x.cross(y);
                x.normalize();
                y.normalize();
                z.normalize();
                orientation = cgmath::Matrix3f(x, y, z);
            }

            boxCreator.setTransformationMatrix(
                cgmath::Matrix4f::fromTranslation(direction)*SPHERE_RADIUS
                *cgmath::Matrix4f(orientation));
            boxCreator.create();
        }

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
    gOptions.setUsageSummary("points output.rfm");
    gOptions.setProgramPurpose("Creates a hemispherical distribution of points on a sphere.");
    gOptions.addRequiredPositionalOptions()
        ("points", opt::value<unsigned>(), "Number of points")
        ("output-file", "Output file")
        ;

    gOptions.parse(argc, argv);
}
