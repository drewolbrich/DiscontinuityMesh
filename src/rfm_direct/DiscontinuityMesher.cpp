// Copyright 2008 Retroactive Fiasco.

#include "DiscontinuityMesher.h"

#include <cassert>
#include <cstdlib>
#include <cmath>
#include <set>
#include <fstream>
#include <algorithm>
#include <limits>

#include <con/Streams.h>
#include <cgmath/Tolerance.h>
#include <cgmath/LineOperations.h>
#include <except/FailedOperationException.h>
#include <except/OpenFileException.h>
#include <mesh/Types.h>
#include <mesh/Face.h>
#include <mesh/StandardAttributes.h>
#include <mesh/EdgeOperations.h>
#include <mesh/FaceOperations.h>
#include <mesh/MeshOperations.h>
#include <mesh/AttributePossessor.h>
#include <meshretri/FaceLineSegment.h>
#include <meshretri/MeshAttributes.h>
#include <meshprim/BoxCreator.h>

#include "WedgeIntersector.h"
#include "LineSegmentCollection.h"
#include "MeshShader.h"

DiscontinuityMesher::DiscontinuityMesher()
    : mMesh(NULL),
      mMaterialTable(),
      mDistantAreaLightVector(),
      mEmissiveFaceLightSourcesAreEnabled(true),
      mMaterialVector(),
      mMaterialIndexAttributeKey(),
      mCreatedNearlyCoincidentDegreeZeroVertexAttributeKey(),
      mDebugLineSegmentCollection(),
      mLightVertexIndex(0),
      mRetriangulator(),
      mDebugPointVector(),
      mMarkDegreeZeroDiscontinuityVertices(false)
{
}

DiscontinuityMesher::~DiscontinuityMesher()
{
}

void
DiscontinuityMesher::setMesh(mesh::Mesh *mesh)
{
    mMesh = mesh;

    mMaterialTable.initialize(*mesh);

    mRetriangulator.setMesh(mesh);

    mMaterialIndexAttributeKey = mesh::GetMaterialIndexAttributeKey(*mMesh);

    mCreatedNearlyCoincidentDegreeZeroVertexAttributeKey = mMesh->getAttributeKey(
        "createdNearlyCoincidentDegreeZeroVertex",
        mesh::AttributeKey::BOOL, mesh::AttributeKey::TEMPORARY);
}

void
DiscontinuityMesher::addDistantAreaLight(const light::DistantAreaLight &distantAreaLight)
{
    mDistantAreaLightVector.push_back(distantAreaLight);
}

void
DiscontinuityMesher::setEmissiveFaceLightSourcesAreEnabled(bool emissiveFaceLightSourcesAreEnabled)
{
    mEmissiveFaceLightSourcesAreEnabled = emissiveFaceLightSourcesAreEnabled;
}

bool
DiscontinuityMesher::emissiveFaceLightSourcesAreEnabled() const
{
    return mEmissiveFaceLightSourcesAreEnabled;
}

void
DiscontinuityMesher::createDiscontinuityMesh()
{
    unsigned inputFaceCount = mMesh->faceCount();
    con::debug << "Input faces: " << inputFaceCount << std::endl;

    con::info << "Calculating shadow discontinuity line segments." << std::endl;
    calculateCriticalLineSegments();

    con::info << "Retriangulating mesh." << std::endl;
    mRetriangulator.retriangulate();

    unsigned discontinuityMeshFaceCount = mMesh->faceCount();
    con::debug << "Discontinuity mesh faces: " << discontinuityMeshFaceCount 
        << " (" << int(100.0*(float(discontinuityMeshFaceCount)/inputFaceCount))/100.0
        << " times input faces) "
        << std::endl;

    con::debug << "Discontinuity mesh vertices: " << mMesh->vertexCount() << std::endl;
}

void
DiscontinuityMesher::createCriticalLineSegmentsAsDegenerateTriangles()
{
    mDebugLineSegmentCollection.reset(new LineSegmentCollection);
    try {
        calculateCriticalLineSegments();
        createDegenerateTrianglesFromLineSegmentCollection(*mDebugLineSegmentCollection.get());
    } catch (...) {
        mDebugLineSegmentCollection.reset();
        throw;
    }
}

void
DiscontinuityMesher::writeCriticalLineSegments(const std::string &filename)
{
    mDebugLineSegmentCollection.reset(new LineSegmentCollection);
    try {
        calculateCriticalLineSegments();
        std::ofstream file(filename.c_str());
        if (!file) {
            throw except::OpenFileException(SOURCE_LINE, os::Error::fromSystemError())
                << "Could not open file \"" << filename << "\".";
        }
        for (LineSegmentCollection::const_iterator iterator 
                 = mDebugLineSegmentCollection->begin();
             iterator != mDebugLineSegmentCollection->end(); ++iterator) {
            const LineSegment &lineSegment = *iterator;
            file << lineSegment.point0().worldPosition()
                << " " << lineSegment.point1().worldPosition() << "\n";
        }
    } catch (...) {
        mDebugLineSegmentCollection.reset();
        throw;
    }
}

bool
DiscontinuityMesher::testCriticalLineSegments(const std::string &filename)
{
    mDebugLineSegmentCollection.reset(new LineSegmentCollection);
    try {
        calculateCriticalLineSegments();
        std::ifstream file(filename.c_str());
        if (!file) {
            throw except::OpenFileException(SOURCE_LINE, os::Error::fromSystemError())
                << "Could not open file \"" << filename << "\".";
        }
        while (true) {
            std::string line;
            std::getline(file, line);
            if (file.eof()) {
                break;
            }
            std::istringstream istr(line.c_str());
            cgmath::Vector3f p0;
            cgmath::Vector3f p1;
            istr >> p0[0] >> p0[1] >> p0[2] >> p1[0] >> p1[1] >> p1[2];
            bool found = false;
            float epsilon = std::max(p0.length(), p1.length())*cgmath::TOLERANCE;
            if (epsilon == 0.0) {
                epsilon = cgmath::TOLERANCE;
            }
            for (LineSegmentCollection::const_iterator iterator 
                     = mDebugLineSegmentCollection->begin();
                 iterator != mDebugLineSegmentCollection->end(); ++iterator) {
                const LineSegment &lineSegment = *iterator;
                if ((p0.equivalent(lineSegment.point0().worldPosition(), epsilon)
                        && p1.equivalent(lineSegment.point1().worldPosition(), epsilon))
                    || (p1.equivalent(lineSegment.point0().worldPosition(), epsilon)
                        && p0.equivalent(lineSegment.point1().worldPosition(), epsilon))) {
                    found = true;
                    break;
                }
            }
            if (!found) {
                return false;
            }
        }

    } catch (...) {
        mDebugLineSegmentCollection.reset();
        throw;
    }

    return true;
}

void
DiscontinuityMesher::shadeMeshVertices()
{
    MeshShader mMeshShader;
    mMeshShader.setDiscontinuityMesher(this);
    mMeshShader.setMesh(mMesh);
    mMeshShader.setMaterialTable(&mMaterialTable);
    mMeshShader.shadeMeshVertices();

    if (mMarkDegreeZeroDiscontinuityVertices) {
        markDegreeZeroDiscontinuityVertices();
        addDebugPointsToMesh();
    }
}

bool
DiscontinuityMesher::faceIsLightSource(mesh::FacePtr facePtr) const
{
    if (!facePtr->hasAttribute(mMaterialIndexAttributeKey)) {
        return false;
    }

    size_t materialIndex = facePtr->getInt(mMaterialIndexAttributeKey);
    assert(materialIndex < mMaterialVector.size());

    return mMaterialVector[materialIndex].mEmission != cgmath::Vector3f::ZERO;
}

bool
DiscontinuityMesher::vertexIsAdjacentToLightSource(mesh::VertexPtr vertexPtr) const
{
    for (mesh::AdjacentFaceIterator iterator = vertexPtr->adjacentFaceBegin();
         iterator != vertexPtr->adjacentFaceEnd(); ++iterator) {
        mesh::FacePtr facePtr = *iterator;
        if (faceIsLightSource(facePtr)) {
            return true;
        }
    }

    return false;
}

bool
DiscontinuityMesher::edgeIsAdjacentToLightSource(mesh::EdgePtr edgePtr) const
{
    for (mesh::AdjacentFaceIterator iterator = edgePtr->adjacentFaceBegin();
         iterator != edgePtr->adjacentFaceEnd(); ++iterator) {
        mesh::FacePtr facePtr = *iterator;
        if (faceIsLightSource(facePtr)) {
            return true;
        }
    }

    return false;
}

void
DiscontinuityMesher::setMarkDegreeZeroDiscontinuityVertices(
    bool markDegreeZeroDiscontinuityVertices)
{
    mMarkDegreeZeroDiscontinuityVertices = markDegreeZeroDiscontinuityVertices;
}

const DiscontinuityMesher::DistantAreaLightVector &
DiscontinuityMesher::distantAreaLightVector() const
{
    return mDistantAreaLightVector;
}

mesh::AttributeKey
DiscontinuityMesher::getCreatedNearlyCoincidentDegreeZeroVertexAttributeKey() const
{
    return mCreatedNearlyCoincidentDegreeZeroVertexAttributeKey;
}

void
DiscontinuityMesher::calculateCriticalLineSegments()
{
    ensureThatAllFacesAreTriangles();
    buildMaterialVector();

    if (!emissiveFacesExist()
        && mDistantAreaLightVector.empty()) {
        throw except::FailedOperationException(SOURCE_LINE)
            << "No light sources were defined.";
    }

    if (mEmissiveFaceLightSourcesAreEnabled) {
        projectEmissiveFaceLightSources();
    }

    projectDistantAreaLightSources();
}

void
DiscontinuityMesher::ensureThatAllFacesAreTriangles()
{
    if (!mesh::AllFacesAreTriangles(*mMesh)) {
        throw except::FailedOperationException(SOURCE_LINE)
            << "The mesh contains one or more non-triangular faces.";
    }
}

void
DiscontinuityMesher::buildMaterialVector()
{
    std::vector<int> materialIndexVector = mesh::GetMaterialIndexVector(*mMesh);
    if (materialIndexVector.empty()) {
        return;
    }

    // It's assumed the vector is already sorted.
    size_t maxIndex = materialIndexVector.back();

    mMaterialVector.resize(maxIndex + 1);

    for (size_t index = 0; index < mMaterialVector.size(); ++index) {
        mMaterialVector[index].mEmission = cgmath::Vector3f(
            mMesh->getVector4f(mesh::GetMaterialEmission4fAttributeKey(*mMesh, index)));
        mMaterialVector[index].mDiffuse = cgmath::Vector3f(
            mMesh->getVector4f(mesh::GetMaterialDiffuse4fAttributeKey(*mMesh, index)));
    }
}

bool
DiscontinuityMesher::emissiveFacesExist()
{
    for (size_t index = 0; index < mMaterialVector.size(); ++index) {
        if (mMaterialVector[index].mEmission != cgmath::Vector3f::ZERO) {
            return true;
        }
    }

    return false;
}

void
DiscontinuityMesher::projectEmissiveFaceLightSources()
{
    WedgeIntersector wedgeIntersector;

    // Process all VE events.

    for (mesh::VertexPtr lightSourceVertexPtr = mMesh->vertexBegin();
         lightSourceVertexPtr != mMesh->vertexEnd(); ++lightSourceVertexPtr) {

        if (vertexIsAdjacentToLightSource(lightSourceVertexPtr)) {

            // Project the light source vertex against all the edges of the mesh.
            for (mesh::EdgePtr occluderEdgePtr = mMesh->edgeBegin();
                 occluderEdgePtr != mMesh->edgeEnd(); ++occluderEdgePtr) {

                cgmath::Vector3f ev0;
                cgmath::Vector3f ev1;
                mesh::GetEdgeVertexPositions(occluderEdgePtr, &ev0, &ev1);

                if ((vertexSharesFaceWithEdge(lightSourceVertexPtr, occluderEdgePtr)
                        || (edgeIsInFrontOfLightSourceFaceAdjacentToVertex(occluderEdgePtr,
                            lightSourceVertexPtr)
                            && wedgeIsExtremal(lightSourceVertexPtr, occluderEdgePtr,
                                INTERIOR_EXTREMAL, EXTERIOR_EXTREMAL)))
                    && edgeIsSilhouette(occluderEdgePtr, 
                        lightSourceVertexPtr->position() - ev0)
                    && (vertexIsSilhouette(lightSourceVertexPtr, 
                            ev0 - lightSourceVertexPtr->position())
                        || vertexIsSilhouette(lightSourceVertexPtr, 
                            ev1 - lightSourceVertexPtr->position()))) {

                    // The light source vertex and the occluder edge form 
                    // the VE event wedge.
                    // Project it against all of the occluder triangles in the scene.

                    if (wedgeIntersector.setVeEventWedge(lightSourceVertexPtr, 
                            occluderEdgePtr)) {
                        traceWedge(wedgeIntersector);
                    }
                }
            }
        }
    }

    // Process all EV events.

    for (mesh::EdgePtr lightSourceEdgePtr = mMesh->edgeBegin();
         lightSourceEdgePtr != mMesh->edgeEnd(); ++lightSourceEdgePtr) {

        if (edgeIsAdjacentToLightSource(lightSourceEdgePtr)) {

            // Project the light source edge against all the vertices in the mesh.
            for (mesh::VertexPtr occluderVertexPtr = mMesh->vertexBegin();
                 occluderVertexPtr != mMesh->vertexEnd(); ++occluderVertexPtr) {

                cgmath::Vector3f ev0;
                cgmath::Vector3f ev1;
                mesh::GetEdgeVertexPositions(lightSourceEdgePtr, &ev0, &ev1);

                if ((vertexSharesFaceWithEdge(occluderVertexPtr, lightSourceEdgePtr)
                        || (vertexIsInFrontOfLightSourceFaceAdjacentToEdge(occluderVertexPtr,
                            lightSourceEdgePtr)
                            && wedgeIsExtremal(occluderVertexPtr, lightSourceEdgePtr,
                                NO_INTERIOR_EXTREMAL, EXTERIOR_EXTREMAL)))
                    && (vertexIsSilhouette(occluderVertexPtr, 
                            ev0 - occluderVertexPtr->position())
                        || vertexIsSilhouette(occluderVertexPtr, 
                            ev1 - occluderVertexPtr->position()))
                    && edgeIsSilhouette(lightSourceEdgePtr, 
                        occluderVertexPtr->position() - ev0)) {

                    // The light source edge and the occluder vertex form
                    // the EV event wedge.
                    // Project it against all of the occluder triangles in the scene.

                    if (wedgeIntersector.setEvEventWedge(lightSourceEdgePtr, 
                            occluderVertexPtr)) {
                        traceWedge(wedgeIntersector);
                    }
                }
            }
        }
    }
}

void
DiscontinuityMesher::projectDistantAreaLightSources()
{
    cgmath::BoundingBox3f bbox = mesh::ComputeBoundingBox(*mMesh);
    float sceneDiameter = (bbox.max() - bbox.min()).length();

    mLightVertexIndex = 0;

    for (DistantAreaLightVector::iterator iterator = mDistantAreaLightVector.begin(); 
         iterator != mDistantAreaLightVector.end(); ++iterator) {
        light::DistantAreaLight &distantAreaLight = *iterator;

        distantAreaLight.setSceneDiameter(sceneDiameter);
        distantAreaLight.prepareForVertexCalculation();

        projectDistantAreaLight(distantAreaLight);
    }
}

void
DiscontinuityMesher::projectDistantAreaLight(const light::DistantAreaLight &distantAreaLight)
{
    WedgeIntersector wedgeIntersector;

    // Process all distant area light EE events.

    for (int index = 0; index < distantAreaLight.sides(); ++index) {

        // Project the light source vertex against all the edges of the mesh.
        for (mesh::EdgePtr occluderEdgePtr = mMesh->edgeBegin();
             occluderEdgePtr != mMesh->edgeEnd(); ++occluderEdgePtr) {

            // Get the edge endpoint positions.
            cgmath::Vector3f edgeVertex0;
            cgmath::Vector3f edgeVertex1;
            mesh::GetEdgeVertexPositions(occluderEdgePtr, &edgeVertex0, &edgeVertex1);

            // For each edge endpoint, find the corresponding light source position.
            cgmath::Vector3f lightVertex0 = distantAreaLight.calculateVertex(edgeVertex0, index);
            cgmath::Vector3f lightVertex1 = distantAreaLight.calculateVertex(edgeVertex1, index);

            if (distantAreaLightEeWedgeIsExtremal(lightVertex0, lightVertex1,
                    occluderEdgePtr, distantAreaLight, 
                    (index + distantAreaLight.sides() - 1) % distantAreaLight.sides(),
                    (index + 1) % distantAreaLight.sides())
                && edgeIsSilhouette(occluderEdgePtr, lightVertex0 - edgeVertex0)) {
                unsigned lightVertexIndex = mLightVertexIndex + index;
                if (wedgeIntersector.setDistantLightEeEventWedge(lightVertex0, lightVertex1, 
                        lightVertexIndex, occluderEdgePtr)) {
                    traceWedge(wedgeIntersector);
                }
            }
        }
    }

    // Process all distant area light EV events.

    for (int index = 0; index < distantAreaLight.sides(); ++index) {

        // Project the light source edge against all the vertices of the mesh.
        for (mesh::VertexPtr occluderVertexPtr = mMesh->vertexBegin();
             occluderVertexPtr != mMesh->vertexEnd(); ++occluderVertexPtr) {

            // For each vertex in the mesh, find the positions of the endpoints
            // of the corresponding light source vertex.
            cgmath::Vector3f lightVertex0 = distantAreaLight.calculateVertex(
                occluderVertexPtr->position(), index);
            cgmath::Vector3f lightVertex1 = distantAreaLight.calculateVertex(
                occluderVertexPtr->position(), 
                (index + 1) % distantAreaLight.sides());

            if (distantAreaLightEvWedgeIsExtremal(lightVertex0, lightVertex1,
                    occluderVertexPtr, distantAreaLight, 
                    (index + 2) % distantAreaLight.sides())
                && (vertexIsSilhouette(occluderVertexPtr, 
                        lightVertex0 - occluderVertexPtr->position())
                    || vertexIsSilhouette(occluderVertexPtr, 
                        lightVertex1 - occluderVertexPtr->position()))) {
                unsigned lightVertexIndex0 = mLightVertexIndex + index;
                unsigned lightVertexIndex1 = mLightVertexIndex 
                    + ((index + 1) % distantAreaLight.sides());
                if (wedgeIntersector.setDistantLightEvEventWedge(lightVertex0, lightVertex1, 
                        lightVertexIndex0, lightVertexIndex1, occluderVertexPtr)) {
                    traceWedge(wedgeIntersector);
                }
            }
        }
    }

    mLightVertexIndex += distantAreaLight.sides();
}

void
DiscontinuityMesher::traceWedge(WedgeIntersector &wedgeIntersector)
{
    // Compute the intersection of the wedge and all of the faces in the scene.
    // This results in a set of line segments that lie in the plane of the wedge.
    LineSegmentCollection lineSegmentCollection;
    lineSegmentCollection.setWedgeIntersector(&wedgeIntersector);
    for (mesh::FacePtr facePtr = mMesh->faceBegin();
         facePtr != mMesh->faceEnd(); ++facePtr) {

        // Don't cast shadows of edges onto a face which is adjacent
        // to the vertex or edge that form the wedge
        // (the light source or the occluder).
        if (!wedgeIntersector.faceIsAdjacentToWedge(facePtr)
            // Don't bother casting shadows onto other light sources.
            && !faceIsLightSource(facePtr)) {

            LineSegment *lineSegmentArray = NULL;
            int intersectionCount = wedgeIntersector.testTriangle(facePtr, 
                &lineSegmentArray);
            for (int index = 0; index < intersectionCount; ++index) {
                lineSegmentCollection.addLineSegment(lineSegmentArray[index]);
            }
        }
    }

    // From the set of all critical line segments, calculate the subsections
    // of those line segments that are visible from the light source.
    lineSegmentCollection.calculateVisibleLineSegments();

    // Project all of the remaining line segments onto the faces that
    // they are associated with.
    for (LineSegmentCollection::const_iterator iterator = lineSegmentCollection.begin();
         iterator != lineSegmentCollection.end(); ++iterator) {
        const LineSegment &lineSegment(*iterator);

        // Don't record line segments on backfacing triangles.
        if (mesh::GetFaceGeometricNormal(lineSegment.facePtr()).dot(
                wedgeIntersector.getVectorTowardLight(lineSegment.point0().worldPosition()))
            <= 0.0) {
            continue;
        }

        // For testing, add the line segment back into the mesh as a degenerate triangle.
        if (mDebugLineSegmentCollection.get() != NULL) {
            mDebugLineSegmentCollection->addLineSegment(lineSegment);
        }

        meshretri::FaceLineSegment faceLineSegment;
        faceLineSegment.setWorldPosition(0, lineSegment.point0().worldPosition());
        faceLineSegment.setEndpointIdentifier(0, lineSegment.point0().endpointIdentifier());
        faceLineSegment.setWorldPosition(1, lineSegment.point1().worldPosition());
        faceLineSegment.setEndpointIdentifier(1, lineSegment.point1().endpointIdentifier());

        faceLineSegment.setIsDegreeZeroDiscontinuity(0, 
            lineSegment.point0().isDegreeZeroDiscontinuity());
        faceLineSegment.setIsDegreeZeroDiscontinuity(1, 
            lineSegment.point1().isDegreeZeroDiscontinuity());

        // Perform an additional test to flag D0 discontinuity vertices
        // that are near the wedge's edge, but do not exactly intersect it.
        if ((wedgeIntersector.eventType() == WedgeIntersector::VE_EVENT
                || wedgeIntersector.eventType() == WedgeIntersector::DISTANT_LIGHT_EE_EVENT)
            && wedgeIntersector.edgePtrIsDefined()) {
            mesh::EdgePtr edgePtr = wedgeIntersector.edgePtr();

            mesh::VertexPtr v0;
            mesh::VertexPtr v1;
            mesh::GetEdgeAdjacentVertices(edgePtr, &v0, &v1);

            cgmath::Vector3f p0;
            cgmath::Vector3f p1;
            mesh::GetEdgeVertexPositions(edgePtr, &p0, &p1);

            float epsilon = 1.0;
            epsilon = std::max(epsilon, p0.maxAbs());
            epsilon = std::max(epsilon, p1.maxAbs());
            if (wedgeIntersector.vertexPtrIsDefined()) {
                epsilon = std::max(epsilon, 
                    wedgeIntersector.vertexPtr()->position().maxAbs());
            }
            epsilon = std::max(epsilon, 
                lineSegment.point0().worldPosition().maxAbs());
            epsilon = std::max(epsilon, 
                lineSegment.point1().worldPosition().maxAbs());
            epsilon *= cgmath::TOLERANCE*10.0f;

            if (cgmath::GetDistanceFromLineSegmentToPoint3f(p0, p1,
                    lineSegment.point0().worldPosition()) <= epsilon) {
                faceLineSegment.setIsDegreeZeroDiscontinuity(0, true);
            }
            if (cgmath::GetDistanceFromLineSegmentToPoint3f(p0, p1,
                    lineSegment.point1().worldPosition()) <= epsilon) {
                faceLineSegment.setIsDegreeZeroDiscontinuity(1, true);
            }

            // Test for the case where D0 vertices are created and are
            // nearly coincident with the wedge's edge. We mark these with
            // a special attribute, so that later we can use a larger epsilon
            // value when doing backprojection visibility tests.
            // This is necessary when objects are in contact with each other
            // but there is no mesh connectivity. In these situations,
            // geometry may slightly interpenetrate, which would cause
            // false shadowing if we didn't use the larger epsilon value.
            if (cgmath::GetDistanceFromLineSegmentToPoint3f(
                    lineSegment.point0().worldPosition(),
                    lineSegment.point1().worldPosition(), p0) <= epsilon) {
                v0->setBool(mCreatedNearlyCoincidentDegreeZeroVertexAttributeKey, true);
            }
            if (cgmath::GetDistanceFromLineSegmentToPoint3f(
                    lineSegment.point0().worldPosition(),
                    lineSegment.point1().worldPosition(), p1) <= epsilon) {
                v1->setBool(mCreatedNearlyCoincidentDegreeZeroVertexAttributeKey, true);
            }
        }

// TODO: This is just a hack for testing. I should replace this by
// some more explicit calculation.
#if 0
        weldFaceLineSegmentEndpointsToFaceVertices(&faceLineSegment,
            lineSegment.facePtr());
#endif
        
        mRetriangulator.addFaceLineSegmentToFace(faceLineSegment, lineSegment.facePtr());
    }
}

bool
DiscontinuityMesher::vertexIsAdjacentToOccluder(mesh::VertexPtr vertexPtr) const
{
    for (mesh::AdjacentFaceIterator iterator = vertexPtr->adjacentFaceBegin();
         iterator != vertexPtr->adjacentFaceEnd(); ++iterator) {
        mesh::FacePtr facePtr = *iterator;
        if (!faceIsLightSource(facePtr)) {
            return true;
        }
    }

    return false;
}

bool
DiscontinuityMesher::edgeIsAdjacentToOccluder(mesh::EdgePtr edgePtr) const
{
    for (mesh::AdjacentFaceIterator iterator = edgePtr->adjacentFaceBegin();
         iterator != edgePtr->adjacentFaceEnd(); ++iterator) {
        mesh::FacePtr facePtr = *iterator;
        if (!faceIsLightSource(facePtr)) {
            return true;
        }
    }

    return false;
}

bool 
DiscontinuityMesher::edgeIsSilhouette(mesh::EdgePtr edgePtr, 
    const cgmath::Vector3f &vectorTowardLight) const
{
    // Returns true if the specified edge is a silhouette edge on the mesh
    // from the point of view of the specified vertex.

    if (edgePtr->adjacentFaceCount() < 2) {
        // The edge has less than two adjacent faces, so by definition
        // it has to be a silhouette edge.
        return true;
    }

    assert(edgePtr->adjacentVertexBegin() != edgePtr->adjacentVertexEnd());

    int frontfacingCount = 0;
    int backfacingCount = 0;
    int lightSourceCount = 0;
    int occluderCount = 0;
    for (mesh::AdjacentFaceIterator iterator = edgePtr->adjacentFaceBegin();
         iterator != edgePtr->adjacentFaceEnd(); ++iterator) {
        // The value 0.001 below helps avoid projecting unnecessary edges
        // into the scene when the light source consists of multiple
        // nearly coplanar polygons.
        if (mesh::GetFaceGeometricNormal(*iterator).dot(vectorTowardLight) > 0.001) {
            ++frontfacingCount;
        } else {
            ++backfacingCount;
        }
        if (faceIsLightSource(*iterator)) {
            ++lightSourceCount;
        } else {
            ++occluderCount;
        }
    }

    // If one of the faces is frontfacing and the other is backfacing
    // with respect to the vertex, it's a silhouette edge.
    if (frontfacingCount > 0
        && backfacingCount > 0) {
        return true;
    }

    // If the edge is adjacent to both a light source face and an occluder face,
    // it's considered a silhouette edge.
    if (lightSourceCount > 0
        && occluderCount > 0) {
        return true;
    }

    return false;
}

bool
DiscontinuityMesher::vertexIsSilhouette(mesh::VertexPtr vertexPtr,
    const cgmath::Vector3f &vectorTowardLight) const
{
    for (mesh::AdjacentEdgeIterator iterator = vertexPtr->adjacentEdgeBegin();
         iterator != vertexPtr->adjacentEdgeEnd(); ++iterator) {
        if (edgeIsSilhouette(*iterator, vectorTowardLight)) {
            return true;
        }
    }

    return false;
}

bool
DiscontinuityMesher::edgeIsInFrontOfLightSourceFaceAdjacentToVertex(mesh::EdgePtr edgePtr,
    mesh::VertexPtr vertexPtr) const
{
    // Return true if, adjacent to the vertex, there is at least one emissive face that at
    // least one vertex of the specified edge is in front of.

    for (mesh::AdjacentFaceIterator iterator = vertexPtr->adjacentFaceBegin();
         iterator != vertexPtr->adjacentFaceEnd(); ++iterator) {
        mesh::FacePtr facePtr = *iterator;
        if (faceIsLightSource(facePtr)) {
            cgmath::Vector3f faceNormal = mesh::GetFaceGeometricNormal(facePtr);
            assert(facePtr->adjacentVertexCount() > 0);
            cgmath::Vector3f faceV0 = (*facePtr->adjacentVertexBegin())->position();

            cgmath::Vector3f edgeV0;
            cgmath::Vector3f edgeV1;
            mesh::GetEdgeVertexPositions(edgePtr, &edgeV0, &edgeV1);

            if ((edgeV0 - faceV0).dot(faceNormal) >= 0.0
                || (edgeV1 - faceV0).dot(faceNormal) >= 0.0) {
                return true;
            }
        }
    }

    return false;
}

bool
DiscontinuityMesher::vertexIsInFrontOfLightSourceFaceAdjacentToEdge(mesh::VertexPtr vertexPtr,
    mesh::EdgePtr edgePtr) const
{
    // Return true if, adjacent to the edge, there is at least one emissive face that the
    // specified vertex is in front of.

    for (mesh::AdjacentFaceIterator iterator = edgePtr->adjacentFaceBegin();
         iterator != edgePtr->adjacentFaceEnd(); ++iterator) {
        mesh::FacePtr facePtr = *iterator;
        if (faceIsLightSource(facePtr)) {
            cgmath::Vector3f faceNormal = mesh::GetFaceGeometricNormal(facePtr);
            assert(facePtr->adjacentVertexCount() > 0);
            cgmath::Vector3f faceV0 = (*facePtr->adjacentVertexBegin())->position();
            if ((vertexPtr->position() - faceV0).dot(faceNormal) >= 0.0) {
                return true;
            }
        }
    }

    return false;
}

bool
DiscontinuityMesher::vertexSharesFaceWithEdge(mesh::VertexPtr vertexPtr,
    mesh::EdgePtr edgePtr) const
{
    // Return true if the vertex and the edge have an adjacent face in common.
    for (mesh::AdjacentFaceIterator iterator = edgePtr->adjacentFaceBegin();
         iterator != edgePtr->adjacentFaceEnd(); ++iterator) {
        mesh::FacePtr facePtr = *iterator;
        if (facePtr->hasAdjacentVertex(vertexPtr)) {
            return true;
        }
    }

    return false;
}

bool
DiscontinuityMesher::wedgeIsExtremal(mesh::VertexPtr vertexPtr, mesh::EdgePtr edgePtr,
    InteriorExtremalMode interiorExtremalMode, 
    ExteriorExtremalMode exteriorExtremalMode) const
{
    // Returns true if all of the faces adjacent to the vertex and the edge
    // are on the same side of the plane formed by the vertex and the edge,
    // or if all of the faces adjacent to the vertex are on the opposite
    // side of the plane relative to the faces adjacent to the edge.

    cgmath::Vector3f v = vertexPtr->position();

    // Get the two endpoint vertices of the edge.
    cgmath::Vector3f p;
    cgmath::Vector3f q;
    mesh::GetEdgeVertexPositions(edgePtr, &p, &q);

    // If the wedge is degenerate, ignore it.
    if (p == q
        || p == v
        || v == q) {
        return false;
    }

    cgmath::Vector3f normal = ((p - v).cross(q - v)).normalized();

    // The wedge normal vector must not be zero or infinity or NaN.
    if (normal == cgmath::Vector3f::ZERO
        || normal*0.0 != cgmath::Vector3f::ZERO) {
        return false;
    }

    bool foundLeftVertexFace = false;
    bool foundRightVertexFace = false;
    for (mesh::AdjacentFaceIterator iterator = vertexPtr->adjacentFaceBegin();
         iterator != vertexPtr->adjacentFaceEnd(); ++iterator) {
        mesh::FacePtr facePtr = *iterator;
        for (mesh::AdjacentVertexIterator iterator = facePtr->adjacentVertexBegin();
             iterator != facePtr->adjacentVertexEnd(); ++iterator) {
            mesh::VertexPtr faceVertexPtr = *iterator;
            if (faceVertexPtr != vertexPtr
                && !edgePtr->hasAdjacentVertex(faceVertexPtr)) {
                float dot = (faceVertexPtr->position() - v).dot(normal);
                if (dot > 0.0) {
                    foundLeftVertexFace = true;
                } else if (dot < 0.0) {
                    foundRightVertexFace = true;
                }
            }
        }
    }

    bool foundLeftEdgeFace = false;
    bool foundRightEdgeFace = false;
    for (mesh::AdjacentFaceIterator iterator = edgePtr->adjacentFaceBegin();
         iterator != edgePtr->adjacentFaceEnd(); ++iterator) {
        mesh::FacePtr facePtr = *iterator;
        for (mesh::AdjacentVertexIterator iterator = facePtr->adjacentVertexBegin();
             iterator != facePtr->adjacentVertexEnd(); ++iterator) {
            mesh::VertexPtr faceVertexPtr = *iterator;
            if (faceVertexPtr != vertexPtr
                && !edgePtr->hasAdjacentVertex(faceVertexPtr)) {
                float dot = (faceVertexPtr->position() - v).dot(normal);
                if (dot > 0.0) {
                    foundLeftEdgeFace = true;
                } else if (dot < 0.0) {
                    foundRightEdgeFace = true;
                }
            }
        }
    }

    if (interiorExtremalMode == INTERIOR_EXTREMAL) {
        if (foundLeftVertexFace == foundLeftEdgeFace
            && foundRightVertexFace == foundRightEdgeFace) {
            // It's an interior extremal wedge.
            return true;
        }
    }

    if (exteriorExtremalMode == EXTERIOR_EXTREMAL) {
        if (foundLeftVertexFace != foundLeftEdgeFace
            && foundRightVertexFace != foundRightEdgeFace) {
            // It's an exterior extremal wedge.
            return true;
        }
    }

    return false;
}

bool
DiscontinuityMesher::distantAreaLightEeWedgeIsExtremal(const cgmath::Vector3f &lightVertex0,
    const cgmath::Vector3f &, mesh::EdgePtr occluderEdgePtr,
    const light::DistantAreaLight &distantAreaLight, 
    int previousDistantAreaLightVertexIndex, int nextDistantAreaLightVertexIndex) const
{
    // Get the two endpoint vertices of the edge.
    cgmath::Vector3f p;
    cgmath::Vector3f q;
    mesh::GetEdgeVertexPositions(occluderEdgePtr, &p, &q);

    // If the wedge is degenerate, ignore it.
    if (p == q
        || p == lightVertex0
        || lightVertex0 == q) {
        return false;
    }

    cgmath::Vector3f normal = ((p - lightVertex0).cross(q - lightVertex0)).normalized();

    // The wedge normal vector must not be zero or infinity or NaN.
    if (normal == cgmath::Vector3f::ZERO
        || normal*0.0 != cgmath::Vector3f::ZERO) {
        return false;
    }

    bool foundLeftEdgeFace = false;
    bool foundRightEdgeFace = false;
    for (mesh::AdjacentFaceIterator iterator = occluderEdgePtr->adjacentFaceBegin();
         iterator != occluderEdgePtr->adjacentFaceEnd(); ++iterator) {
        mesh::FacePtr facePtr = *iterator;
        for (mesh::AdjacentVertexIterator iterator = facePtr->adjacentVertexBegin();
             iterator != facePtr->adjacentVertexEnd(); ++iterator) {
            mesh::VertexPtr faceVertexPtr = *iterator;
            if (!occluderEdgePtr->hasAdjacentVertex(faceVertexPtr)) {
                float dot = (faceVertexPtr->position() - lightVertex0).dot(normal);
                if (dot > 0.0) {
                    foundLeftEdgeFace = true;
                } else if (dot < 0.0) {
                    foundRightEdgeFace = true;
                }
            }
        }
    }

    // This assumes the light source is round.
    bool foundLeftLightVertex = false;
    bool foundRightLightVertex = false;
    cgmath::Vector3f nextLightVertex = distantAreaLight.calculateVertex(p, 
        nextDistantAreaLightVertexIndex);
    float dot = (nextLightVertex - lightVertex0).dot(normal);
    if (dot > 0.0) {
        foundLeftLightVertex = true;
    } else if (dot < 0.0) {
        foundRightLightVertex = true;
    }
    cgmath::Vector3f previousLightVertex = distantAreaLight.calculateVertex(p, 
        previousDistantAreaLightVertexIndex);
    dot = (previousLightVertex - lightVertex0).dot(normal);
    if (dot > 0.0) {
        foundLeftLightVertex = true;
    } else if (dot < 0.0) {
        foundRightLightVertex = true;
    }

    if (foundLeftLightVertex == foundLeftEdgeFace
        && foundRightLightVertex == foundRightEdgeFace) {
        // It's an interior extremal wedge.
        return true;
    }

    if (foundLeftLightVertex != foundLeftEdgeFace
        && foundRightLightVertex != foundRightEdgeFace) {
        // It's an exterior extremal wedge.
        return true;
    }

    return false;
}

bool
DiscontinuityMesher::distantAreaLightEvWedgeIsExtremal(const cgmath::Vector3f &lightVertex0,
    const cgmath::Vector3f &lightVertex1, mesh::VertexPtr occluderVertexPtr,
    const light::DistantAreaLight &distantAreaLight, int nextDistantAreaLightVertexIndex) const
{
    cgmath::Vector3f v = occluderVertexPtr->position();

    // If the wedge is degenerate, ignore it.
    if (lightVertex0 == lightVertex1
        || lightVertex0 == v
        || v == lightVertex1) {
        return false;
    }

    cgmath::Vector3f normal = ((lightVertex0 - v).cross(lightVertex1 - v)).normalized();

    // The wedge normal vector must not be zero or infinity or NaN.
    if (normal == cgmath::Vector3f::ZERO
        || normal*0.0 != cgmath::Vector3f::ZERO) {
        return false;
    }

    bool foundLeftVertexFace = false;
    bool foundRightVertexFace = false;
    for (mesh::AdjacentFaceIterator iterator = occluderVertexPtr->adjacentFaceBegin();
         iterator != occluderVertexPtr->adjacentFaceEnd(); ++iterator) {
        mesh::FacePtr facePtr = *iterator;
        for (mesh::AdjacentVertexIterator iterator = facePtr->adjacentVertexBegin();
             iterator != facePtr->adjacentVertexEnd(); ++iterator) {
            mesh::VertexPtr faceVertexPtr = *iterator;
            if (faceVertexPtr != occluderVertexPtr) {
                float dot = (faceVertexPtr->position() - v).dot(normal);
                if (dot > 0.0) {
                    foundLeftVertexFace = true;
                } else if (dot < 0.0) {
                    foundRightVertexFace = true;
                }
            }
        }
    }

    // This assumes the light source is round.
    bool foundLeftLightVertex = false;
    bool foundRightLightVertex = false;
    cgmath::Vector3f nextLightVertex = distantAreaLight.calculateVertex(v, 
        nextDistantAreaLightVertexIndex);
    float dot = (nextLightVertex - lightVertex0).dot(normal);
    if (dot > 0.0) {
        foundLeftLightVertex = true;
    } else if (dot < 0.0) {
        foundRightLightVertex = true;
    }

    if (foundLeftVertexFace != foundLeftLightVertex
        && foundRightVertexFace != foundRightLightVertex) {
        // It's an exterior extremal wedge.
        return true;
    }
    
    return false;
}

void
DiscontinuityMesher::createDegenerateTrianglesFromLineSegmentCollection(
    const LineSegmentCollection &lineSegmentCollection)
{
    for (LineSegmentCollection::const_iterator iterator 
             = lineSegmentCollection.begin();
         iterator != lineSegmentCollection.end(); ++iterator) {
        const LineSegment &lineSegment(*iterator);

        mesh::VertexPtr v1 = mMesh->createVertex();
        mesh::VertexPtr v2 = mMesh->createVertex();
        mesh::VertexPtr v3 = mMesh->createVertex();
        mesh::EdgePtr e12 = mMesh->createEdge();
        mesh::EdgePtr e23 = mMesh->createEdge();
        mesh::EdgePtr e31 = mMesh->createEdge();
        mesh::FacePtr face = mMesh->createFace();

        v1->setPosition(lineSegment.point0().worldPosition());
        v2->setPosition(lineSegment.point1().worldPosition());
        v3->setPosition(lineSegment.point1().worldPosition());

        face->addAdjacentVertex(v1);
        face->addAdjacentVertex(v2);
        face->addAdjacentVertex(v3);
        v1->addAdjacentFace(face);
        v2->addAdjacentFace(face);
        v3->addAdjacentFace(face);

        v1->addAdjacentEdge(e12);
        v1->addAdjacentEdge(e31);
        v2->addAdjacentEdge(e23);
        v2->addAdjacentEdge(e12);
        v3->addAdjacentEdge(e23);
        v3->addAdjacentEdge(e31);
        face->addAdjacentEdge(e12);
        face->addAdjacentEdge(e23);
        face->addAdjacentEdge(e31);

        e12->addAdjacentFace(face);
        e23->addAdjacentFace(face);
        e31->addAdjacentFace(face);
        e12->addAdjacentVertex(v1);
        e12->addAdjacentVertex(v2);
        e23->addAdjacentVertex(v2);
        e23->addAdjacentVertex(v3);
        e31->addAdjacentVertex(v3);
        e31->addAdjacentVertex(v1);
    }    
}

void
DiscontinuityMesher::markDegreeZeroDiscontinuityVertices()
{
    mesh::AttributeKey isDegreeZeroDiscontinuityAttributeKey
        = meshretri::GetIsDegreeZeroDiscontinuityAttributeKey(*mMesh);

    for (mesh::VertexPtr vertexPtr = mMesh->vertexBegin();
         vertexPtr != mMesh->vertexEnd(); ++vertexPtr) {

        if (vertexPtr->getBool(isDegreeZeroDiscontinuityAttributeKey)) {
            addDebugPoint(vertexPtr->position());
        }
    }
}

void
DiscontinuityMesher::addDebugPoint(const cgmath::Vector3f &point)
{
    mDebugPointVector.push_back(point);
}

void
DiscontinuityMesher::addDebugPointsToMesh()
{
    cgmath::BoundingBox3f bbox = mesh::ComputeBoundingBox(*mMesh);
    float sceneDiameter = (bbox.max() - bbox.min()).length();

    float pointSize = sceneDiameter*0.001f;

    mesh::MaterialIndexVector materialIndexVector = mesh::GetMaterialIndexVector(*mMesh);
    int materialIndex = 0;
    if (materialIndexVector.size() > 0) {
        materialIndex = materialIndexVector.back() + 1;
    }

    mMesh->setVector4f(mesh::GetMaterialDiffuse4fAttributeKey(*mMesh, materialIndex),
        cgmath::Vector4f(1.0, 0.0, 0.0, 1.0));

    mesh::AttributePossessor faceAttributes;
    faceAttributes.setInt(mesh::GetMaterialIndexAttributeKey(*mMesh), materialIndex);
    faceAttributes.setVector3f(mesh::GetColor3fAttributeKey(*mMesh),
        cgmath::Vector3f(1.0, 0.0, 0.0));

    for (DebugPointVector::const_iterator iterator = mDebugPointVector.begin();
         iterator != mDebugPointVector.end(); ++iterator) {
        const cgmath::Vector3f &point = *iterator;

        meshprim::BoxCreator boxCreator;
        boxCreator.setMesh(mMesh);
        boxCreator.setTransformationMatrix(cgmath::Matrix4f::fromTranslation(point));
        boxCreator.setWidth(pointSize);
        boxCreator.setHeight(pointSize);
        boxCreator.setDepth(pointSize);
        boxCreator.setFaceAttributes(faceAttributes);
        boxCreator.create();
    }
}

void
DiscontinuityMesher::weldFaceLineSegmentEndpointsToFaceVertices(
    meshretri::FaceLineSegment *faceLineSegment, mesh::FacePtr facePtr)
{
    for (unsigned endpointIndex = 0; endpointIndex < 2; ++endpointIndex) {
        for (mesh::AdjacentVertexConstIterator iterator = facePtr->adjacentVertexBegin();
             iterator != facePtr->adjacentVertexEnd(); ++iterator) {
            const cgmath::Vector3f &vertexPosition = (*iterator)->position();
            float distance = (faceLineSegment->worldPosition(endpointIndex)
                - vertexPosition).length();
            if (distance < 0.001) {
                faceLineSegment->setWorldPosition(endpointIndex, vertexPosition);
                break;
            }
        }
    }
}
