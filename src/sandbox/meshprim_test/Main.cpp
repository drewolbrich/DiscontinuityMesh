// Copyright 2009 Drew Olbrich

#include <cstdlib>
#include <iostream>

#include <con/Streams.h>
#include <meshprim/CylinderCreator.h>
#include <meshprim/BoxCreator.h>
#include <cgmath/Matrix4fOperations.h>
#include <mesh/Mesh.h>
#include <meshrfm/WriteRfmFile.h>

int
main(int, char **)
{
    try {

        mesh::Mesh mesh;

        meshprim::CylinderCreator cylinderCreator;
        meshprim::BoxCreator boxCreator;

        cgmath::Vector3f pointArray[3] = {
            cgmath::Vector3f(-2.0, -1.0, 0.0),
            cgmath::Vector3f(2.0, -1.0, 0.0),
            cgmath::Vector3f(0.0, 3.0, 2.0),
        };

        for (unsigned index = 0; index < 3; ++index) {
            boxCreator.setMesh(&mesh);
            boxCreator.setTransformationMatrix(
                cgmath::Matrix4f::fromTranslation(pointArray[index]));
            boxCreator.setWidth(0.4);
            boxCreator.setHeight(0.4);
            boxCreator.setDepth(0.4);
            boxCreator.create();

            cylinderCreator.setMesh(&mesh);
            cylinderCreator.setTransformationMatrix(
                cgmath::GetPointConnectionMatrix(
                    pointArray[index],
                    pointArray[(index + 1) % 3]));

            cylinderCreator.setHeight(1.0);
            cylinderCreator.setRadius(0.2);
            cylinderCreator.setDivisions(12);
            cylinderCreator.create();
        }

        meshrfm::WriteRfmFile(mesh, "out.rfm");

    } catch (const std::exception &exception) {
        con::error << exception.what() << std::endl;
        exit(EXIT_FAILURE);
    }

    return EXIT_SUCCESS;
}
