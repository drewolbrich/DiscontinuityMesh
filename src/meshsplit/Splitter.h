// Copyright 2009 Drew Olbrich

#ifndef MESHSPLIT__SPLITTER__INCLUDED
#define MESHSPLIT__SPLITTER__INCLUDED

#include <vector>

#include <mesh/Mesh.h>
#include <meshisect/FaceIntersector.h>
#include <meshretri/Retriangulator.h>
#include <cgmath/Vector3f.h>

namespace meshsplit {

// Splitter
//
// Splits the faces of a mesh where they intersect.
// All of the input faces must be triangles.

class Splitter : public meshisect::FaceIntersector::BoundingBoxListener
{
public:
    Splitter();
    virtual ~Splitter();

    // The mesh that's going to be welded.
    void setMesh(mesh::Mesh *mesh);
    mesh::Mesh *mesh() const;

    // Absolute tolerance to use when testing for coincident vertices.
    void setAbsoluteTolerance(float absoluteTolerance);
    float absoluteTolerance() const;

    // Relative tolerance. When large values are compared, the relative tolerance
    // supercedes the absolute tolerance.
    void setRelativeTolerance(float relativeTolerance);
    float relativeTolerance() const;

    // If true, mark the intersections with cylinders in the output mesh.
    void setMarkIntersectionsWithCylinders(bool markIntersectionsWithCylinders);
    bool markIntersectionsWithCylinders() const;

    // Attribute assigned to new edges created by meshretri::Retriangulator,
    // and edges that cause these edges to be created.
    void setFlaggedEdgeBooleanAttributeKey(
        const mesh::AttributeKey &flaggedEdgeBooleanAttributeKey);
    const mesh::AttributeKey &flaggedEdgeBooleanAttributeKey() const;

    // Split faces where they intersect.
    void splitFaces();

    // For meshisect::FaceIntersector::BoundingBoxListener:
    virtual bool applyObjectToBoundingBox(meshisect::FaceIntersectorAabbTreeNode
        &faceIntersectorAabbTreeNode, const cgmath::BoundingBox3f &boundingBox);

private:
    void resetDebugEdgeVector();
    void addDebugEdge(const cgmath::Vector3f &p0, const cgmath::Vector3f &p1);
    void markDebugEdges();

    typedef std::vector<mesh::EdgePtr> EdgePtrVector;
    void getAdjacentEdgePtrVector(mesh::FacePtr facePtr, EdgePtrVector *edgePtrVector);

    mesh::Mesh *mMesh;

    float mAbsoluteTolerance;
    float mRelativeTolerance;

    bool mMarkIntersectionsWithCylinders;

    mesh::AttributeKey mFlaggedEdgeBooleanAttributeKey;

    mesh::EdgePtr mCurrentEdgePtr;

    meshretri::Retriangulator mRetriangulator;

    // Vector of edges for debugging. These are added back into
    // the mesh afterward to mark all the intersections.
    struct DebugEdge {
        cgmath::Vector3f mPoint0;
        cgmath::Vector3f mPoint1;
    };
    typedef std::vector<DebugEdge> DebugEdgeVector;
    DebugEdgeVector mDebugEdgeVector;

    // This is needed to create identifiers using
    // meshretri::EndpointIdentifier::fromEdgePtrAndIndex
    // that share the same edge but are otherwise unique.
    int mEndpointIdentifierIndex;
};

} // namespace meshsplit

#endif // MESHSPLIT__SPLITTER__INCLUDED
