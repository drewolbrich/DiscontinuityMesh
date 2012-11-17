// Copyright 2009 Retroactive Fiasco.

#ifndef RFM_DISCMESH__MESH_SHADER__INCLUDED
#define RFM_DISCMESH__MESH_SHADER__INCLUDED

#include <mesh/AttributeKey.h>
#include <mesh/Types.h>
#include <meshisect/FaceIntersector.h>
#include <meshisect/EdgeIntersector.h>
#include <meshretri/Retriangulator.h>
#include <meshretri/TriangleVector.h>
#include <cgmath/Vector3f.h>

#include "LocalLightFace.h"

namespace mesh {
class Mesh;
class MaterialTable;
}

class WedgeIntersector;
class DiscontinuityMesher;
class LineSegmentCollection;

// MeshShader
//
// Class that shades mesh vertices.

class MeshShader : public meshisect::FaceIntersector::TriangleListener,
                     public meshisect::EdgeIntersector::BoundingBoxListener
{
public:
    MeshShader();
    ~MeshShader();

    // The DiscontinuityMesher that contains the MeshShader.
    void setDiscontinuityMesher(DiscontinuityMesher *discontinuityMesher);

    // The mesh being shaded.
    void setMesh(mesh::Mesh *mesh);

    // Table of materials defined in the mesh.
    void setMaterialTable(mesh::MaterialTable *materialTable);

    // Shade the mesh face vertices.
    void shadeMeshVertices();

    // For mesh::FaceIntersector::TriangleListener:
    virtual bool applyObjectToTriangleVector(
        meshisect::FaceIntersectorAabbTreeNode &faceIntersectorAabbTreeNode,
        const meshisect::FaceIntersector::TriangleVector &triangleVector); 

    // For mesh::EdgeIntersector::BoundingBoxListener:
    virtual bool applyObjectToBoundingBox(
        meshisect::EdgeIntersectorAabbTreeNode &edgeIntersectorAabbTreeNode,
        const cgmath::BoundingBox3f &boundingBox);

private:
    void initializeFaceVertexColors();
    void shadeLocalLightFaces();
    void createLocalLightFaceVector();
    void copyIlluminatedVertexColorsToStandardVertexColors();
    void shadeMeshVertex(mesh::VertexPtr vertexPtr);
    void shadeMeshVertexWithLightFace(mesh::VertexPtr vertexPtr, const LightFace &lightFace);
    void traceBackprojectionWedge(WedgeIntersector &wedgeIntersector, 
        mesh::FacePtr lightFacePtr);
    void shadeFaceVerticesAdjacentToVertex(mesh::VertexPtr vertexPtr, 
        const LightFace &lightFace, const meshretri::TriangleVector &triangleVector);
    bool rayIntersectsMesh(const cgmath::Vector3f &rayOrigin, 
        const cgmath::Vector3f &rayEndpoint, mesh::VertexPtr localVertexToIgnore,
        mesh::FacePtr emissiveFaceToIgnore, mesh::FacePtr localFaceToIgnore);

    // Returns true if a face is backfacing with respect to any of the vertices
    // of a triangle on an emissive face.
    // If true is returned, the face is considered entirely in shadow, and should
    // receive no illumination from the face.
    bool faceIsBackfacing(mesh::FacePtr facePtr, const meshretri::Triangle &triangle);

    // Returns true if a face vertex shares a normal with any backfaces adjacent
    // to the vertex.
    bool faceVertexSharesNormalWithAdjacentBackfacingFace(mesh::FacePtr facePtr,
        mesh::VertexPtr vertexPtr, const meshretri::Triangle &triangle);

    // Count the number of distant light faces.
    size_t getDistantLightFaceCount() const;

    void printShadingStatistics();

    void createDistantAreaLightFace();
    void destroyDistantAreaLightFace();

    enum IlluminationState {
        ILLUMINATED,
        OCCLUDED
    };
    void dumpBackprojectionTriangle(const meshretri::Triangle &triangle,
        IlluminationState illuminationState);
    bool hasDumpedBackprojectionTriangles() const;
    void dumpBackprojectionSvgFile();

    DiscontinuityMesher *mDiscontinuityMesher;

    mesh::Mesh *mMesh;

    mesh::MaterialTable *mMaterialTable;

    mesh::AttributeKey mIlluminatedColor3fAttributeKey;
    mesh::AttributeKey mMaterialIndexAttributeKey;
    mesh::AttributeKey mColor3fAttributeKey;
    mesh::AttributeKey mNormal3fAttributeKey;

    meshretri::Retriangulator mRetriangulator;

    typedef std::vector<LocalLightFace> LocalLightFaceVector;
    LocalLightFaceVector mLocalLightFaceVector;

    meshisect::FaceIntersector mFaceIntersector;
    meshisect::EdgeIntersector mEdgeIntersector;

    // Used by traceBackprojectionWedge.
    LineSegmentCollection *mTriangleLineSegmentCollection;
    mesh::FacePtr mTriangleLightFacePtr;

    // Used by shadeMeshVertexWithLightFace.
    WedgeIntersector *mBoundingBoxWedgeIntersector;
    mesh::VertexPtr mBoundingBoxVertexPtr;
    mesh::FacePtr mBoundingBoxLightFacePtr;

    mesh::AttributeKey mIsDegreeZeroDiscontinuityAttributeKey;

    // This temporary face is moved around as needed to
    // calculate the illumination from distant area light triangles.
    mesh::FacePtr mDistantAreaLightFace;
    mesh::VertexPtr mDistantAreaLightVertex0;
    mesh::VertexPtr mDistantAreaLightVertex1;
    mesh::VertexPtr mDistantAreaLightVertex2;
    mesh::EdgePtr mDistantAreaLightEdge01;
    mesh::EdgePtr mDistantAreaLightEdge12;
    mesh::EdgePtr mDistantAreaLightEdge20;

    meshretri::TriangleVector mDumpedBackprojectionTriangleVector;
    meshretri::TriangleVector mDumpedOccludedBackprojectionTriangleVector;
};

#endif // RFM_DISCMESH__MESH_SHADER__INCLUDED
