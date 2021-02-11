// Copyright 2009 Drew Olbrich

// Visual test of cgmath::BoundingBox3fIntersectsTetrahedron.

#include <cstdlib>
#include <vector>

#include <con/Streams.h>
#include <mesh/Mesh.h>
#include <mesh/StandardAttributes.h>
#include <mesh/AttributePossessor.h>
#include <meshprim/CreateTriangleMesh.h>
#include <meshprim/BoxCreator.h>
#include <meshrfm/WriteRfmFile.h>
#include <cgmath/Vector3f.h>
#include <cgmath/Matrix4f.h>
#include <cgmath/BoundingBox3f.h>
#include <cgmath/BoundingBox3fOperations.h>

int
main(int, char **)
{
    try {

        mesh::Mesh mesh;

        float vertexPositionArray[] = {
            -2, 0, -1,
            3, 0, 0,
            0, 4, 0,
            0, 0, 5
        };
        int faceVertexIndexArray[] = {
            0, 2, 1,
            0, 3, 2,
            0, 1, 3,
            1, 2, 3
        };
        std::vector<mesh::VertexPtr> vertexPtrArray;
        std::vector<mesh::EdgePtr> edgePtrArray;
        std::vector<mesh::FacePtr> facePtrArray;
        meshprim::CreateTriangleMesh(&mesh, 
            sizeof(vertexPositionArray)/sizeof(float)/3, vertexPositionArray,
            sizeof(faceVertexIndexArray)/sizeof(int)/3, faceVertexIndexArray,
            &vertexPtrArray, &edgePtrArray, &facePtrArray);

        static const int MATERIAL_INDEX = 0;

        mesh.setString(mesh::GetMaterialNameAttributeKey(mesh, MATERIAL_INDEX), "BoxMaterial");
        mesh.setVector4f(mesh::GetMaterialDiffuse4fAttributeKey(mesh, MATERIAL_INDEX),
            cgmath::Vector4f(1, 0.23, 0.2, 1));

        mesh::AttributePossessor boxFaceAttributes;
        boxFaceAttributes.setInt(mesh::GetMaterialIndexAttributeKey(mesh), MATERIAL_INDEX);

        for (size_t index = 0; index < 150000; ++index) {

            static const float BOX_DISTRIBUTION_WIDTH = 10.0;
            cgmath::Vector3f position(
                drand48()*BOX_DISTRIBUTION_WIDTH - BOX_DISTRIBUTION_WIDTH*0.5,
                drand48()*BOX_DISTRIBUTION_WIDTH - BOX_DISTRIBUTION_WIDTH*0.5,
                drand48()*BOX_DISTRIBUTION_WIDTH - BOX_DISTRIBUTION_WIDTH*0.5);

            static const float BOX_WIDTH = 0.1;

            if (cgmath::BoundingBox3fIntersectsTetrahedron(
                    cgmath::BoundingBox3f(
                        position - cgmath::Vector3f(BOX_WIDTH, BOX_WIDTH, BOX_WIDTH)*0.5,
                        position + cgmath::Vector3f(BOX_WIDTH, BOX_WIDTH, BOX_WIDTH)*0.5),
                    vertexPtrArray[0]->position(),
                    vertexPtrArray[1]->position(),
                    vertexPtrArray[2]->position(),
                    vertexPtrArray[3]->position())) {

                meshprim::BoxCreator boxCreator;
                boxCreator.setMesh(&mesh);
                boxCreator.setTransformationMatrix(cgmath::Matrix4f::fromTranslation(position));
                boxCreator.setWidth(BOX_WIDTH);
                boxCreator.setHeight(BOX_WIDTH);
                boxCreator.setDepth(BOX_WIDTH);
                boxCreator.setFaceAttributes(boxFaceAttributes);
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
