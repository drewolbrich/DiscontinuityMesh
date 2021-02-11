// Copyright 2008 Drew Olbrich

// This program converts a Wavefront OBJ file to an RFM file.

#include <exception>

#include <boost/lexical_cast.hpp>

#include <opt/ProgramOptionsParser.h>
#include <con/Streams.h>
#include <mesh/Mesh.h>
#include <mesh/AssignNormals.h>
#include <mesh/StandardAttributes.h>
#include <meshobj/ReadObjFile.h>
#include <meshrfm/WriteRfmFile.h>
#include <cgmath/Vector3fProgramOption.h>

static opt::ProgramOptionsParser gOptions;

// Parse the command line arguments.
static void ParseCommandLineArguments(int argc, char **argv);

// Set the light source material's emission attribute.
static void SetLightSourceMaterialEmissionAttribute(mesh::Mesh &mesh);

int
main(int argc, char **argv)
{
    try {

        ParseCommandLineArguments(argc, argv);

        con::info << "Reading OBJ file \""
            << gOptions.get("input-file").as<std::string>()
            << "\"." << std::endl;

        mesh::Mesh mesh;
        meshobj::ReadObjFile(&mesh, gOptions.get("input-file").as<std::string>());

        if (!gOptions.specified("keep-normals")) {

            con::info << "Calculating new mesh normals." << std::endl;

            if (gOptions.specified("crease-angle")) {
                mesh::AssignNormals(&mesh, gOptions.get("crease-angle").as<float>());
            } else {
                mesh::AssignNormals(&mesh);
            }
        }

        if (gOptions.specified("light-source-material")) {
            SetLightSourceMaterialEmissionAttribute(mesh);
        }

        con::info << "Writing RFM file \""
            << gOptions.get("output-file").as<std::string>()
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
    gOptions.setUsageSummary("input.obj output.rfm [options]");
    gOptions.setProgramPurpose("Converts a Wavefront OBJ file to an RFM file.");

    gOptions.addRequiredPositionalOptions()
        ("input-file", "Input OBJ file")
        ("output-file", "Output RFM file")
        ;

    gOptions.addOptions()
        ("reset-normals", 
            "Assign new vertex normals based on mesh topology (the default behavior)")
        ("keep-normals", "Keep the existing OBJ file normals")
        ("crease-angle", opt::value<float>(),
            (std::string("Angle (in degrees) between adjacent faces, above which, ")
                + "discontinuous normals will be assigned to the shared edge"
                + " (default " 
                + boost::lexical_cast<std::string>(mesh::DEFAULT_ASSIGN_NORMALS_CREASE_ANGLE)
                + ")").c_str())
        ("light-source-material", opt::value<std::string>(),
            "OBJ material name that will be assigned an emission color attribute in the RFM file")
        ("light-source-intensity", opt::value<float>(),
            "Intensity of light source emission attribute")
        ("light-source-color", opt::value<cgmath::Vector3f>()->set_name("r g b"),
            "Color of light source emission attribute (0..1)")
        ;

    gOptions.parse(argc, argv);
}

static void 
SetLightSourceMaterialEmissionAttribute(mesh::Mesh &mesh)
{
    std::string lightSourceMaterialName = gOptions.get("light-source-material").as<std::string>();
    mesh::MaterialIndexVector materialIndexVector = mesh::GetMaterialIndexVector(mesh);

    cgmath::Vector3f emissionColor = cgmath::Vector3f::UNIT;
    if (gOptions.specified("light-source-color")) {
        emissionColor = gOptions.get("light-source-color").as<cgmath::Vector3f>();
    }

    float intensity = 1.0;
    if (gOptions.specified("light-source-intensity")) {
        intensity = gOptions.get("light-source-intensity").as<float>();
    }

    cgmath::Vector4f emission(emissionColor*intensity);

    bool found = false;
    for (mesh::MaterialIndexVector::const_iterator iterator = materialIndexVector.begin(); 
         iterator != materialIndexVector.end(); ++iterator) {
        int materialIndex = *iterator;
        mesh::AttributeKey materialNameAttributeKey
            = mesh::GetMaterialNameAttributeKey(mesh, materialIndex);
        if (mesh.getString(materialNameAttributeKey) == lightSourceMaterialName) {
            mesh::AttributeKey materialEmission4fAttributeKey 
                = mesh::GetMaterialEmission4fAttributeKey(mesh, materialIndex);
            mesh.setVector4f(materialEmission4fAttributeKey, emission);
            found = true;
        }
    }

    if (!found) {
        con::error << "Light source material \"" << lightSourceMaterialName 
            << "\" was not specified in MTL file." << std::endl;
        exit(EXIT_FAILURE);
    }
}
