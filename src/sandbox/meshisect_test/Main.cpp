// Copyright 2009 Drew Olbrich

// Program to test firing rays into a mesh.

#include <cstdlib>
#include <iostream>

#include <con/Streams.h>
#include <mesh/Mesh.h>
#include <mesh/StandardAttributes.h>
#include <mesh/AttributePossessor.h>
#include <mesh/MeshOperations.h>
#include <meshisect/FaceIntersector.h>
#include <meshprim/CylinderCreator.h>
#include <meshprim/BoxCreator.h>
#include <meshrfm/ReadRfmFile.h>
#include <meshrfm/WriteRfmFile.h>
#include <cgmath/Vector3f.h>
#include <cgmath/Matrix4f.h>
#include <cgmath/Matrix4fOperations.h>
#include <cgmath/BoundingBox3f.h>

int
main(int, char **)
{
    try {

        mesh::Mesh mesh;

        meshrfm::ReadRfmFile(&mesh, "craftsman-welded.rfm");

        cgmath::BoundingBox3f bbox = mesh::ComputeBoundingBox(mesh);
        float bboxDiameter = (bbox.max() - bbox.min()).length();

        meshisect::FaceIntersector faceIntersector;

        faceIntersector.setMesh(&mesh);
        faceIntersector.initialize();

        int materialIndex = GetMaterialIndexVector(mesh).size();

        mesh.setString(mesh::GetMaterialNameAttributeKey(mesh, materialIndex), "BoxMaterial");
        mesh.setVector4f(mesh::GetMaterialDiffuse4fAttributeKey(mesh, materialIndex),
            cgmath::Vector4f(1, 0.23, 0.2, 1));

        mesh::AttributePossessor faceAttributes;
        faceAttributes.setInt(mesh::GetMaterialIndexAttributeKey(mesh), materialIndex);

        for (size_t index = 0; index < 1000; ++index) {

            cgmath::Vector3f midpoint(drand48()*2.0 - 1.0, 
                drand48()*2.0 - 1.0, drand48()*2.0 - 1.0);
            midpoint *= bboxDiameter*0.5;

#if 0
            cgmath::Vector3f direction;
            do {
                direction = cgmath::Vector3f(drand48()*2.0 - 1.0, 
                    drand48()*2.0 - 1.0, drand48()*2.0 - 1.0);
            } while (direction.length() > 1.0 
                || direction.length() < 0.001
                || direction[1] > 0.0);
            direction.normalize();
#endif

            cgmath::Vector3f direction(1.0, -0.2, 0.0);
            direction.normalize();

            cgmath::Vector3f origin = midpoint - direction*bboxDiameter;
            cgmath::Vector3f endpoint = midpoint + direction*bboxDiameter;

            cgmath::Vector3f intersectionPoint(0, 100, 0);
            mesh::FacePtr facePtr;
            if (faceIntersector.intersectsRaySegment(origin, endpoint,
                    &intersectionPoint, &facePtr)) {

                meshprim::CylinderCreator cylinderCreator;
                cylinderCreator.setMesh(&mesh);
                cylinderCreator.setTransformationMatrix(
                    cgmath::GetPointConnectionMatrix(origin, intersectionPoint));
                cylinderCreator.setRadius(0.025);
                cylinderCreator.setDivisions(6);
                cylinderCreator.setFaceAttributes(faceAttributes);
                cylinderCreator.create();

                meshprim::BoxCreator boxCreator;
                boxCreator.setMesh(&mesh);
                boxCreator.setTransformationMatrix(
                    cgmath::Matrix4f::fromTranslation(intersectionPoint));
                boxCreator.setWidth(0.125);
                boxCreator.setHeight(boxCreator.width());
                boxCreator.setDepth(boxCreator.width());
                boxCreator.setFaceAttributes(faceAttributes);
                boxCreator.create();
            }
        }

        meshrfm::WriteRfmFile(mesh, "out.rfm");

    } catch (const std::exception &exception) {
        con::error << exception.what() << std::endl;
        exit(EXIT_FAILURE);
    }

    return EXIT_SUCCESS;
}
