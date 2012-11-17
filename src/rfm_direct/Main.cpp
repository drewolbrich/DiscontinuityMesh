// Copyright 2008 Retroactive Fiasco.

// This program generates a discontinuity mesh within an RFM file,
// given one or more light sources defined as polygons
// with nonzero emissive attributes.

#include <cstdlib>
#include <map>

#include <opt/ProgramOptionsParser.h>
#include <con/Streams.h>
#include <mesh/Mesh.h>
#include <mesh/Face.h>
#include <mesh/StandardAttributes.h>
#include <meshweld/Welder.h>
#include <meshrfm/ReadRfmFile.h>
#include <meshrfm/WriteRfmFile.h>
#include <cgmath/Vector2f.h>
#include <cgmath/Vector3f.h>
#include <cgmath/Vector3fProgramOption.h>
#include <light/DistantAreaLight.h>
#include <svg/SvgWriter.h>
#include <os/Time.h>
#include <os/Memory.h>
#include <mesh/Triangulator.h>

#include "DiscontinuityMesher.h"

static opt::ProgramOptionsParser gOptions;

// Parse the command line arguments.
static void ParseCommandLineArguments(int argc, char **argv);

// Parse arguments defining distant area light source that represents the sun.
static void ParseSunArguments(DiscontinuityMesher &discontinuityMesher);

// Clean up mesh geometry so it is suitable input to the discontinuity meshing code.
static void CleanMesh(mesh::Mesh *mesh);

int
main(int argc, char **argv)
{
    try {

        ParseCommandLineArguments(argc, argv);

        DiscontinuityMesher discontinuityMesher;
        ParseSunArguments(discontinuityMesher);

        os::TimeValue startTime = os::GetProcessUserTime();

        con::info << "Reading RFM file \"" << gOptions.get("input-file").as<std::string>()
            << "\"." << std::endl;

        mesh::Mesh mesh;
        meshrfm::ReadRfmFile(&mesh, gOptions.get("input-file").as<std::string>());

        CleanMesh(&mesh);

        discontinuityMesher.setMesh(&mesh);

        if (gOptions.specified("no-emissive")) {
            con::info << "Emissive faces will not act as light sources." << std::endl;
            discontinuityMesher.setEmissiveFaceLightSourcesAreEnabled(false);
        }

        if (gOptions.specified("write-lines")) {

            std::string filename = gOptions.get("write-lines").as<std::string>();
            con::info << "Writing critical line segments to file "
                << filename << std::endl;
            discontinuityMesher.writeCriticalLineSegments(filename);

        } else if (gOptions.specified("test-lines")) {

            std::string filename = gOptions.get("test-lines").as<std::string>();
            con::info << "Testing critical line segments against those in file "
                << filename << std::endl;
            if (!discontinuityMesher.testCriticalLineSegments(filename)) {
                con::error << "Critical line segments do not match those in file "
                    << filename << std::endl;
                exit(EXIT_FAILURE);
            }

        } else {

            if (gOptions.specified("debug-lines")) {
                
                con::info << "Writing critical line segments as degenerate triangles." 
                    << std::endl;
                discontinuityMesher.createCriticalLineSegmentsAsDegenerateTriangles();

            } else {

                discontinuityMesher.createDiscontinuityMesh();

                if (!gOptions.specified("no-shade")) {

                    if (gOptions.specified("mark-d0-vertices")) {
                        discontinuityMesher.setMarkDegreeZeroDiscontinuityVertices(true);
                    }

                    discontinuityMesher.shadeMeshVertices();
                }
            }

            con::info << "Writing RFM file \"" << gOptions.get("output-file").as<std::string>()
                << "\"." << std::endl;
            meshrfm::WriteRfmFile(mesh, gOptions.get("output-file").as<std::string>());
        }

        os::TimeValue stopTime = os::GetProcessUserTime();
        os::TimeValue duration = stopTime - startTime;

        con::debug << "CPU time used: " << duration;
        if (duration > 60.0) {
            con::debug << " (" << int(duration.asFloat()*10.0)/10.0 << " seconds)";
        }
        con::debug << std::endl;

        con::debug << "Memory used: " << os::GetProcessMemoryUsedAsString()
            << std::endl;

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
    gOptions.setProgramPurpose("Calculates direct illumination for an RFM file.");

    gOptions.addRequiredPositionalOptions()
        ("input-file", "Input RFM file")
        ;

    gOptions.addOptionalPositionalOptions()
        ("output-file", "Input RFM file")
        ;

    gOptions.addOptions()
        ("sun-azimuth", opt::value<float>(), "Sun azimuth (degrees)")
        ("sun-elevation", opt::value<float>(), "Sun elevation (degrees)")
        ("sun-diameter", opt::value<float>(), 
            (std::string("Sun angular diameter (arcminutes, default ")
                + boost::lexical_cast<std::string>(
                    light::DistantAreaLight::DEFAULT_ANGULAR_DIAMETER) + ")").c_str())
        ("sun-sides", opt::value<int>(), 
            (std::string("Number of sides of sun polygon (default ")
                + boost::lexical_cast<std::string>(
                    light::DistantAreaLight::DEFAULT_SIDES) + ")").c_str())
        ("sun-intensity", opt::value<float>(), "Sun intensity")
        ("sun-color", opt::value<cgmath::Vector3f>()->set_name("r g b"), "Sun color (0..1)")
        ("no-emissive", "Disable emissive face light sources")
        ;

    gOptions.addDebugOptions()
        ("debug-lines", "Output critical line segments as degenerate triangles")
        ("write-lines", opt::value<std::string>(), "File to output critical line segments to")
        ("test-lines", opt::value<std::string>(), "File to compare critical line segments against")
        ("no-shade", "Do not shade the vertices of the discontinuity mesh")
        ("mark-d0-vertices", "Mark degree zero discontinuity vertices in output mesh")
        ;

    gOptions.parse(argc, argv);

    if (gOptions.specified("debug-lines") 
        + gOptions.specified("write-lines")     
        + gOptions.specified("test-lines") > 1) {
        con::error << "Only one of --debug-lines, --write-lines, and --test-lines "
            << "may be specified at a time." << std::endl;
        exit(EXIT_FAILURE);
    }

    if (gOptions.specified("write-lines")
        || gOptions.specified("test-lines")) {
        if (gOptions.specified("output-file")) {
            con::error << "An output file may not be specified in combination with "
                << "the --write-lines or --test-lines flags." << std::endl;
            exit(EXIT_FAILURE);
        } 
    } else {
        if (!gOptions.specified("output-file")) {
            gOptions.printUsageMessage();
        }
    }
}

static void 
ParseSunArguments(DiscontinuityMesher &discontinuityMesher)
{
    light::DistantAreaLight distantAreaLight;
    bool defined = false;

    if (gOptions.specified("sun-azimuth")
        || gOptions.specified("sun-elevation")) {
        if (gOptions.specified("sun-azimuth")
            + gOptions.specified("sun-elevation") != 2) {
            con::error << "Both the --sun-azimuth and --sun-elevation flags must be "
                << "specified together." << std::endl;
            exit(EXIT_FAILURE);
        }
        defined = true;
        distantAreaLight.setPositionFromAzimuthAndElevation(
            gOptions.get("sun-azimuth").as<float>(),
            gOptions.get("sun-elevation").as<float>());
    }

    if (gOptions.specified("sun-diameter")) {
        defined = true;
        distantAreaLight.setAngularDiameter(gOptions.get("sun-diameter").as<float>());
    }

    if (gOptions.specified("sun-sides")) {
        defined = true;
        distantAreaLight.setSides(gOptions.get("sun-sides").as<int>());
    }

    if (gOptions.specified("sun-intensity")) {
        defined = true;
        distantAreaLight.setIntensity(gOptions.get("sun-intensity").as<float>());
    }

    if (gOptions.specified("sun-color")) {
        defined = true;
        distantAreaLight.setColor(gOptions.get("sun-color").as<cgmath::Vector3f>());
    }

    if (defined) {
        discontinuityMesher.addDistantAreaLight(distantAreaLight);
    }
}

static void 
CleanMesh(mesh::Mesh *mesh)
{
    con::info << "Welding perimeter edges." << std::endl;

    meshweld::Welder weld;
    weld.setMesh(mesh);
    weld.weld();

    con::info << "Merged " << weld.mergedEdges()
        << " edges into other edges." << std::endl;

    con::info << "Triangulating faces." << std::endl;

    mesh::Triangulator triangulator;
    triangulator.setMesh(mesh);
    triangulator.triangulate();

    con::info << "Triangulated " << triangulator.triangulatedFaces() 
        << " faces." << std::endl;
}
