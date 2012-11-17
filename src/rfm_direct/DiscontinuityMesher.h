// Copyright 2008 Retroactive Fiasco.

#ifndef RFM_DISCMESH__DISCONTINUITY_MESHER__INCLUDED
#define RFM_DISCMESH__DISCONTINUITY_MESHER__INCLUDED

#include <string>
#include <map>

#include <boost/scoped_ptr.hpp>

#include <mesh/Mesh.h>
#include <mesh/MaterialTable.h>
#include <meshretri/Retriangulator.h>
#include <light/DistantAreaLight.h>

class WedgeIntersector;
class LineSegmentCollection;

// DiscontinuityMesher
//
// A class for creating discontinuity meshes from a Mesh that has one or more
// polygons whose material definition has a nonzero emission component,
// and therefore act as a light source.

class DiscontinuityMesher
{
public:
    DiscontinuityMesher();
    ~DiscontinuityMesher();

    // Set the mesh for which a discontinuity mesh will be created.
    void setMesh(mesh::Mesh *mesh);

    // Add a distant area light.
    void addDistantAreaLight(const light::DistantAreaLight &distantAreaLight);

    // If true (the default), faces with emissive colors defined are
    // treated as light sources.
    void setEmissiveFaceLightSourcesAreEnabled(bool emissiveFaceLightSourcesAreEnabled);
    bool emissiveFaceLightSourcesAreEnabled() const;

    // Create the discontinuity mesh. An except::FailedOperationException is thrown if the
    // mesh has faces that are not triangles, or does not have any polygons with an
    // emissive component defined.
    // Throws an exception if no light sources are defined.
    void createDiscontinuityMesh();

    // Like createDiscontinuityMesh, except that critical line segments
    // are represented as degenerate line segments for debugging.
    void createCriticalLineSegmentsAsDegenerateTriangles();

    // Writes critical line segments to the specified file.
    // The mesh is left unmodified.
    // Throws an exception if there was an error writing the file.
    void writeCriticalLineSegments(const std::string &filename);

    // Compares critical line segments to those in the specified file.
    // Returns false if there is a discrepancy between the two sets of critical lines.
    // Throws an exception if there was an error reading the file.
    bool testCriticalLineSegments(const std::string &filename);

    // Shade the vertices of the discontinuity mesh that has already been calculated.
    void shadeMeshVertices();

    bool faceIsLightSource(mesh::FacePtr facePtr) const;
    bool vertexIsAdjacentToLightSource(mesh::VertexPtr vertexPtr) const;
    bool edgeIsAdjacentToLightSource(mesh::EdgePtr edgePtr) const;

    // Mark D0 discontinuity vertices with red boxes in the output mesh.
    void setMarkDegreeZeroDiscontinuityVertices(bool markDegreeZeroDiscontinuityVertices);

    // The distant area lights used to illuminate the mesh.
    typedef std::vector<light::DistantAreaLight> DistantAreaLightVector;
    const DistantAreaLightVector &distantAreaLightVector() const;

    // Return the attribute key used to mark vertices that create D0 vertices
    // that are nearly coincident with them. Later, these vertices
    // will use a larger epsilon value when testing visibility
    // during backprojection calculation. This helps resolve situations
    // where objects are in contact with each other but have no
    // connectivity, and so are prone to slightly interpenetrating.
    mesh::AttributeKey getCreatedNearlyCoincidentDegreeZeroVertexAttributeKey() const;

private:
    void calculateCriticalLineSegments();
    void ensureThatAllFacesAreTriangles();
    void buildMaterialVector();
    bool emissiveFacesExist();
    void projectEmissiveFaceLightSources();
    void projectDistantAreaLightSources();
    void projectDistantAreaLight(const light::DistantAreaLight &distantAreaLight);
    void traceWedge(WedgeIntersector &wedgeIntersector);
    bool vertexIsAdjacentToOccluder(mesh::VertexPtr vertexPtr) const;
    bool edgeIsAdjacentToOccluder(mesh::EdgePtr edgePtr) const;
    bool edgeIsSilhouette(mesh::EdgePtr edgePtr, 
        const cgmath::Vector3f &vectorTowardLight) const;
    bool vertexIsSilhouette(mesh::VertexPtr vertexPtr, 
        const cgmath::Vector3f &vectorTowardLight) const;
    bool edgeIsInFrontOfLightSourceFaceAdjacentToVertex(mesh::EdgePtr edgePtr,
        mesh::VertexPtr vertexPtr) const;
    bool vertexIsInFrontOfLightSourceFaceAdjacentToEdge(mesh::VertexPtr vertexPtr,
        mesh::EdgePtr edgePtr) const;
    bool vertexSharesFaceWithEdge(mesh::VertexPtr vertexPtr, mesh::EdgePtr edgePtr) const;

    enum InteriorExtremalMode {
        NO_INTERIOR_EXTREMAL,
        INTERIOR_EXTREMAL
    };
    enum ExteriorExtremalMode {
        NO_EXTERIOR_EXTREMAL,
        EXTERIOR_EXTREMAL
    };
    bool wedgeIsExtremal(mesh::VertexPtr vertexPtr, mesh::EdgePtr edgePtr,
        InteriorExtremalMode interiorExtremalMode, 
        ExteriorExtremalMode exteriorExtremalMode) const;
    bool distantAreaLightEeWedgeIsExtremal(const cgmath::Vector3f &lightVertex0,
        const cgmath::Vector3f &lightVertex1, mesh::EdgePtr occluderEdgePtr,
        const light::DistantAreaLight &distantAreaLight, 
        int previousDistantAreaLightVertexIndex, int nextDistantAreaLightVertexIndex) const;
    bool distantAreaLightEvWedgeIsExtremal(const cgmath::Vector3f &lightVertex0,
        const cgmath::Vector3f &lightVertex1, mesh::VertexPtr occluderVertexPtr,
        const light::DistantAreaLight &distantAreaLight, 
        int nextDistantAreaLightVertexIndex) const;

    void createDegenerateTrianglesFromLineSegmentCollection(
        const LineSegmentCollection &lineSegmentCollection);

    void markDegreeZeroDiscontinuityVertices();

    void addDebugPoint(const cgmath::Vector3f &point);
    void addDebugPointsToMesh();

    // This is just a big hack for debugging.
    void weldFaceLineSegmentEndpointsToFaceVertices(
        meshretri::FaceLineSegment *faceLineSegment, mesh::FacePtr facePtr);

    mesh::Mesh *mMesh;

    mesh::MaterialTable mMaterialTable;

    DistantAreaLightVector mDistantAreaLightVector;

    bool mEmissiveFaceLightSourcesAreEnabled;

    struct Material {
        Material() : mDiffuse(cgmath::Vector3f::ZERO),
                     mEmission(cgmath::Vector3f::ZERO) { }
        cgmath::Vector3f mDiffuse;
        cgmath::Vector3f mEmission;
    };
    std::vector<Material> mMaterialVector;

    mesh::AttributeKey mMaterialIndexAttributeKey;
    mesh::AttributeKey mCreatedNearlyCoincidentDegreeZeroVertexAttributeKey;

    boost::scoped_ptr<LineSegmentCollection> mDebugLineSegmentCollection;

    unsigned mLightVertexIndex;

    meshretri::Retriangulator mRetriangulator;

    typedef std::vector<cgmath::Vector3f> DebugPointVector;
    DebugPointVector mDebugPointVector;

    bool mMarkDegreeZeroDiscontinuityVertices;
};

#endif // RFM_DISCMESH__DISCONTINUITY_MESHER__INCLUDED
