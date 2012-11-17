// Copyright 2009 Retroactive Fiasco.

#include "MeshShader.h"

#include <cstdlib>
#include <cassert>
#include <cmath>
#include <iostream>
#include <algorithm>
#include <limits>

#include <cgmath/Tolerance.h>
#include <cgmath/Constants.h>
#include <cgmath/Vector3d.h>
#include <cgmath/TriangleOperations.h>
#include <mesh/StandardAttributes.h>
#include <mesh/FaceOperations.h>
#include <mesh/EdgeOperations.h>
#include <mesh/MeshOperations.h>
#include <mesh/IsConsistent.h>
#include <mesh/MaterialTable.h>
#include <con/Streams.h>
#include <con/LogLevel.h>
#include <meshRetri/MeshAttributes.h>
#include <svg/SvgWriter.h>

#include "DiscontinuityMesher.h"
#include "WedgeIntersector.h"
#include "LineSegment.h"
#include "LineSegmentCollection.h"
#include "MeshShaderFaceListener.h"
#include "LightFace.h"
#include "LocalLightFace.h"
#include "DistantLightFace.h"

MeshShader::MeshShader()
    : mDiscontinuityMesher(NULL),
      mMesh(NULL),
      mMaterialTable(NULL),
      mIlluminatedColor3fAttributeKey(),
      mMaterialIndexAttributeKey(),
      mColor3fAttributeKey(),
      mNormal3fAttributeKey(),
      mRetriangulator(),
      mLocalLightFaceVector(),
      mFaceIntersector(),
      mEdgeIntersector(),
      mTriangleLineSegmentCollection(NULL),
      mTriangleLightFacePtr(),
      mBoundingBoxWedgeIntersector(NULL),
      mBoundingBoxVertexPtr(),
      mBoundingBoxLightFacePtr(),
      mIsDegreeZeroDiscontinuityAttributeKey(),
      mDistantAreaLightFace(),
      mDistantAreaLightVertex0(),
      mDistantAreaLightVertex1(),
      mDistantAreaLightVertex2(),
      mDistantAreaLightEdge01(),
      mDistantAreaLightEdge12(),
      mDistantAreaLightEdge20(),
      mDumpedBackprojectionTriangleVector(),
      mDumpedOccludedBackprojectionTriangleVector()
{
}

MeshShader::~MeshShader()
{
}

void
MeshShader::setDiscontinuityMesher(DiscontinuityMesher *discontinuityMesher)
{
    mDiscontinuityMesher = discontinuityMesher;
}

void
MeshShader::setMesh(mesh::Mesh *mesh)
{
    mMesh = mesh;

    mIlluminatedColor3fAttributeKey = mMesh->getAttributeKey("illuminatedColor3f",
        mesh::AttributeKey::VECTOR3F);

    mMaterialIndexAttributeKey = mesh::GetMaterialIndexAttributeKey(*mMesh);
    mColor3fAttributeKey = mesh::GetColor3fAttributeKey(*mMesh);
    mNormal3fAttributeKey = mesh::GetNormal3fAttributeKey(*mMesh);

    mRetriangulator.setMesh(mesh);

    mIsDegreeZeroDiscontinuityAttributeKey 
        = meshretri::GetIsDegreeZeroDiscontinuityAttributeKey(*mMesh);
}

void
MeshShader::setMaterialTable(mesh::MaterialTable *materialTable)
{
    mMaterialTable = materialTable;
}

void
MeshShader::shadeMeshVertices()
{
    createLocalLightFaceVector();

    initializeFaceVertexColors();

    shadeLocalLightFaces();

    mFaceIntersector.setMesh(mMesh);
    mFaceIntersector.initialize();

    con::debug << "Distant area light sources: "
        << mDiscontinuityMesher->distantAreaLightVector().size() << std::endl;

    con::debug << "Distant light faces: " << getDistantLightFaceCount() << std::endl;

    con::debug << "Local light faces: " << mLocalLightFaceVector.size() << std::endl;

    con::debug << "AABB tree size statistics:\n"
        << mFaceIntersector.aabbSizeStatistics() << std::endl;

    mEdgeIntersector.setMesh(mMesh);
    mEdgeIntersector.initialize();

    con::info << "Shading mesh vertices." << std::endl;

    // Create the temporary face used to represent distant area lights.
    createDistantAreaLightFace();

    for (mesh::VertexPtr vertexPtr = mMesh->vertexBegin();
         vertexPtr != mMesh->vertexEnd(); ++vertexPtr) {
        shadeMeshVertex(vertexPtr);
    }

    destroyDistantAreaLightFace();

    con::debug << "AABB tree query statistics:\n"
        << mFaceIntersector.aabbQueryStatistics() << std::endl;

    con::debug << "AABB queries per mesh vertex: "
        << int((10.0*mFaceIntersector.queries())/mMesh->vertexCount())/10.0 << std::endl;

    con::debug << "AABB queries per mesh vertex per light source face: "
        << int((10.0*mFaceIntersector.queries())/mMesh->vertexCount()
            /(mLocalLightFaceVector.size() + getDistantLightFaceCount()))/10.0 << std::endl;

    copyIlluminatedVertexColorsToStandardVertexColors();

    printShadingStatistics();

    if (hasDumpedBackprojectionTriangles()) {
        dumpBackprojectionSvgFile();
    }
}

bool
MeshShader::applyObjectToTriangleVector(
    meshisect::FaceIntersectorAabbTreeNode &faceIntersectorAabbTreeNode,
    const meshisect::FaceIntersector::TriangleVector &)
{
    mesh::FacePtr facePtr = faceIntersectorAabbTreeNode.facePtr();

    WedgeIntersector *wedgeIntersector = mTriangleLineSegmentCollection->wedgeIntersector();

    // Don't cast shadows of edges onto a face which is adjacent
    // to the vertex or edge that form the wedge
    // (the light source or the occluder).
    if (!wedgeIntersector->faceIsAdjacentToWedge(facePtr)
        // Don't bother casting shadows onto other light sources.
        && !mDiscontinuityMesher->faceIsLightSource(facePtr)
        && facePtr != mTriangleLightFacePtr) {

        LineSegment *lineSegmentArray = NULL;
        int intersectionCount = wedgeIntersector->testTriangle(facePtr, 
            &lineSegmentArray);
        for (int index = 0; index < intersectionCount; ++index) {
            const LineSegment &lineSegment = lineSegmentArray[index];
            mTriangleLineSegmentCollection->addLineSegment(lineSegment);
        }
    }

    // Don't halt the AABB traversal. We want to consider every face
    // that may intersect the wedge.
    return false;
}

bool
MeshShader::applyObjectToBoundingBox(
    meshisect::EdgeIntersectorAabbTreeNode &edgeIntersectorAabbTreeNode,
    const cgmath::BoundingBox3f &)
{
    mesh::EdgePtr occluderEdgePtr = edgeIntersectorAabbTreeNode.edgePtr();

    if (!mDiscontinuityMesher->edgeIsAdjacentToLightSource(occluderEdgePtr)) {
        if (mBoundingBoxWedgeIntersector->setVeEventWedge(mBoundingBoxVertexPtr, 
                occluderEdgePtr)) {
            traceBackprojectionWedge(*mBoundingBoxWedgeIntersector, 
                mBoundingBoxLightFacePtr);
        }   
    }

    // Don't halt the AABB traversal. We want to consider every edge
    // that may intersect the tetrahedron.
    return false;
}

void
MeshShader::initializeFaceVertexColors()
{
    // Reset all the face vertex colors to zero.

    for (mesh::FacePtr facePtr = mMesh->faceBegin();
         facePtr != mMesh->faceEnd(); ++facePtr) {
        for (mesh::AdjacentVertexIterator iterator = facePtr->adjacentVertexBegin();
             iterator != facePtr->adjacentVertexEnd(); ++iterator) {
            mesh::VertexPtr vertexPtr = *iterator;
            facePtr->setVertexVector3f(vertexPtr, mIlluminatedColor3fAttributeKey,
                cgmath::Vector3f::ZERO);
        }
    }
}

void
MeshShader::shadeLocalLightFaces()
{
    if (!mDiscontinuityMesher->emissiveFaceLightSourcesAreEnabled()) {
        return;
    }

    // This sets the colors of the emissive faces to their emissive intensity values.
    // It's possible that they will receive additional intensity
    // later on if they are lit by other emissive faces.

    for (LocalLightFaceVector::iterator iterator = mLocalLightFaceVector.begin();
         iterator != mLocalLightFaceVector.end(); ++iterator) {
        const LocalLightFace &localLightFace = *iterator;
        mesh::FacePtr facePtr = localLightFace.facePtr();
        for (mesh::AdjacentVertexIterator iterator = facePtr->adjacentVertexBegin();
             iterator != facePtr->adjacentVertexEnd(); ++iterator) {
            mesh::VertexPtr vertexPtr = *iterator;
            facePtr->setVertexVector3f(vertexPtr, mIlluminatedColor3fAttributeKey,
                localLightFace.intensity());
        }
    }    
}

void
MeshShader::createLocalLightFaceVector()
{
    if (!mDiscontinuityMesher->emissiveFaceLightSourcesAreEnabled()) {
        return;
    }

    assert(mLocalLightFaceVector.empty());
    for (mesh::FacePtr facePtr = mMesh->faceBegin();
         facePtr != mMesh->faceEnd(); ++facePtr) {
        if (mDiscontinuityMesher->faceIsLightSource(facePtr)) {
            LocalLightFace localLightFace;
            localLightFace.setFacePtr(facePtr);
            localLightFace.setIntensity(
                cgmath::Vector3f(mMaterialTable->getMaterialFromFace(facePtr).mEmission));
            mLocalLightFaceVector.push_back(localLightFace);
        }
    }
}

void
MeshShader::copyIlluminatedVertexColorsToStandardVertexColors()
{
    for (mesh::FacePtr facePtr = mMesh->faceBegin();
         facePtr != mMesh->faceEnd(); ++facePtr) {
        for (mesh::AdjacentVertexIterator iterator = facePtr->adjacentVertexBegin();
             iterator != facePtr->adjacentVertexEnd(); ++iterator) {
            mesh::VertexPtr vertexPtr = *iterator;
            facePtr->setVertexVector3f(vertexPtr, mColor3fAttributeKey,
                facePtr->getVertexVector3f(vertexPtr, mIlluminatedColor3fAttributeKey));
        }
    }
}

void
MeshShader::shadeMeshVertex(mesh::VertexPtr vertexPtr)
{
    for (LocalLightFaceVector::iterator iterator = mLocalLightFaceVector.begin();
         iterator != mLocalLightFaceVector.end(); ++iterator) {
        const LocalLightFace &localLightFace = *iterator;
        shadeMeshVertexWithLightFace(vertexPtr, localLightFace);
    }

    const DiscontinuityMesher::DistantAreaLightVector &distantAreaLightVector
        = mDiscontinuityMesher->distantAreaLightVector();
    for (size_t index = 0; index < distantAreaLightVector.size(); ++index) {
        const light::DistantAreaLight &distantAreaLight = distantAreaLightVector[index];
        for (int index = 0; index < distantAreaLight.sides(); ++index) {
            const cgmath::Vector3f lightCenter 
                = distantAreaLight.getCenter(vertexPtr->position());

            mDistantAreaLightVertex0->setPosition(lightCenter);
            mDistantAreaLightVertex1->setPosition(
                distantAreaLight.calculateVertex(vertexPtr->position(), 
                    (index + 1) % distantAreaLight.sides()));
            mDistantAreaLightVertex2->setPosition(
                distantAreaLight.calculateVertex(vertexPtr->position(), index));

            DistantLightFace distantLightFace;
            distantLightFace.setFacePtr(mDistantAreaLightFace);
            distantLightFace.setIntensity(distantAreaLight.intensity()
                *distantAreaLight.color()
                /distantAreaLight.sides());
            distantLightFace.setUnoccludedFaceArea(
                cgmath::GetTriangleArea(
                    mDistantAreaLightVertex0->position(),
                    mDistantAreaLightVertex1->position(),
                    mDistantAreaLightVertex2->position()));
            distantLightFace.setCenter(lightCenter);

            shadeMeshVertexWithLightFace(vertexPtr, distantLightFace);
        }
    }
}

void
MeshShader::shadeMeshVertexWithLightFace(mesh::VertexPtr vertexPtr,
    const LightFace &lightFace)
{
    mesh::FacePtr lightFacePtr = lightFace.facePtr();

    // If the vertex is on the back side of the emissive face,
    // the vertex can't be directly illuminated by the face,
    // so we return immediately.
    assert(lightFacePtr->adjacentVertexCount() > 0);
    if ((vertexPtr->position() - (*lightFacePtr->adjacentVertexBegin())->position())
        .dot(mesh::GetFaceGeometricNormal(lightFacePtr)) < 0.0) {
        return;
    }

    WedgeIntersector wedgeIntersector;

    mBoundingBoxWedgeIntersector = &wedgeIntersector;
    mBoundingBoxVertexPtr = vertexPtr;
    mBoundingBoxLightFacePtr = lightFacePtr;

    cgmath::BoundingBox3f boundingBox = cgmath::BoundingBox3f::EMPTY_SET;
    boundingBox.extendByVector3f(vertexPtr->position());
    assert(lightFacePtr->adjacentVertexCount() == 3);
    mesh::AdjacentVertexIterator iterator = lightFacePtr->adjacentVertexBegin();
    boundingBox.extendByVector3f((*iterator)->position());
    ++iterator;
    boundingBox.extendByVector3f((*iterator)->position());
    ++iterator;
    boundingBox.extendByVector3f((*iterator)->position());

    // Call applyObjectToBoundingBox on all edges that intersect
    // the bounding box.
    mEdgeIntersector.applyToBoundingBoxIntersection(boundingBox, this);

    meshretri::TriangleVector triangleVector;
    mRetriangulator.retriangulateBackprojectionFace(lightFacePtr, &triangleVector);

    shadeFaceVerticesAdjacentToVertex(vertexPtr, lightFace, triangleVector);
}

void
MeshShader::traceBackprojectionWedge(WedgeIntersector &wedgeIntersector,
    mesh::FacePtr lightFacePtr)
{
    LineSegment *lineSegmentArray = NULL;
    int intersectionCount = wedgeIntersector.testTriangle(lightFacePtr,
        &lineSegmentArray);
    if (intersectionCount == 0) {
        // If the light source face is not intersected by the wedge,
        // it's not worth testing any of the other faces in the scene.
        return;
    }

    LineSegmentCollection lineSegmentCollection;
    lineSegmentCollection.setWedgeIntersector(&wedgeIntersector);
    for (int index = 0; index < intersectionCount; ++index) {
        lineSegmentCollection.addLineSegment(lineSegmentArray[index]);
    }

    mTriangleLineSegmentCollection = &lineSegmentCollection;
    mTriangleLightFacePtr = lightFacePtr;

    // Create a triangle whose vertices are the VE wedge.
    meshisect::FaceIntersector::Triangle triangle;
    triangle.mPointArray[0] = wedgeIntersector.vertexPtr()->position();
    mesh::GetEdgeVertexPositions(wedgeIntersector.edgePtr(),
        &triangle.mPointArray[1], &triangle.mPointArray[2]);

    // Call the function 'applyToObject' on all of the faces in the scene
    // that intersect the VE wedge.
    meshisect::FaceIntersector::TriangleVector triangleVector;
    triangleVector.push_back(triangle);
    mFaceIntersector.applyToTriangleVectorIntersection(triangleVector, this);

    // From the set of all critical line segments, calculate the subsections
    // of those line segments that are visible from the point being shaded.
    lineSegmentCollection.calculateVisibleLineSegments();

    // Project all of the remaining line segments onto the emissive face.
    for (LineSegmentCollection::const_iterator iterator = lineSegmentCollection.begin();
         iterator != lineSegmentCollection.end(); ++iterator) {
        const LineSegment &lineSegment(*iterator);

        // We only care about line segments on the light source face.
        if (lineSegment.facePtr() != lightFacePtr) {
            continue;
        }

        meshretri::FaceLineSegment faceLineSegment;
        faceLineSegment.setWorldPosition(0, lineSegment.point0().worldPosition());
        faceLineSegment.setEndpointIdentifier(0, lineSegment.point0().endpointIdentifier());
        faceLineSegment.setWorldPosition(1, lineSegment.point1().worldPosition());
        faceLineSegment.setEndpointIdentifier(1, lineSegment.point1().endpointIdentifier());

        mRetriangulator.addFaceLineSegmentToFace(faceLineSegment, lightFacePtr);
    }
}

void
MeshShader::shadeFaceVerticesAdjacentToVertex(mesh::VertexPtr vertexPtr,
    const LightFace &lightFace, const meshretri::TriangleVector &triangleVector)
{
    bool shouldDumpBackprojectionTriangle = false;

#if 0
    if (vertexPtr->position().equivalent(cgmath::Vector3f(0.433592, 0, 0.190901), 0.001)) {
        shouldDumpBackprojectionTriangle = true;
    }
#endif

    mesh::FacePtr lightFacePtr = lightFace.facePtr();

    const cgmath::Vector3f &rayOrigin = vertexPtr->position();

    bool isDegreeZeroDiscontinuity = vertexPtr->getBool(mIsDegreeZeroDiscontinuityAttributeKey);

    for (size_t index = 0; index < triangleVector.size(); ++index) {
        const meshretri::Triangle &triangle = triangleVector[index];

        const cgmath::Vector3f triangleCenter = (triangle.mPointArray[0]
            + triangle.mPointArray[1] + triangle.mPointArray[2])/3.0;

        bool vertexIsIlluminated = !rayIntersectsMesh(rayOrigin, 
            triangleCenter, vertexPtr, lightFacePtr, mMesh->faceEnd());

        if (shouldDumpBackprojectionTriangle) {
            dumpBackprojectionTriangle(triangle, vertexIsIlluminated ? ILLUMINATED : OCCLUDED);
        }

        if (vertexIsIlluminated) {
            for (mesh::AdjacentFaceIterator iterator = vertexPtr->adjacentFaceBegin();
                 iterator != vertexPtr->adjacentFaceEnd(); ++iterator) {
                mesh::FacePtr facePtr = *iterator;

                // Never illuminate triangles that are backfacing
                // with respect to the light source.
                if (faceIsBackfacing(facePtr, triangle)) {
                    continue;
                }

                // If the vertex normal is shared by an adjacent face
                // that is backfacing, don't illuminate this face vertex.
                // This test and the test above help us avoid
                // problematic faces along shadow terminators of
                // curve surfaces simulated using vertex normals,
                // for which geometric face normals point away from a light source
                // (meaning it's impossible to record discontinuity
                // line segments on them), but which have one or more vertex normals
                // that point toward the light source, which would otherwise
                // cause them to be partially illuminated.
                if (faceVertexSharesNormalWithAdjacentBackfacingFace(facePtr, vertexPtr,
                        triangle)) {
                    continue;
                }

                // If the vertex lies on a degree zero discontinuity,
                // and is occluded by adjacent faces with respect to the
                // light source, don't illuminate it.
                if (isDegreeZeroDiscontinuity) {
                    cgmath::Vector3f faceCenter = mesh::GetFaceAverageVertexPosition(facePtr);
                    const cgmath::Vector3f triangleCenter = (triangle.mPointArray[0]
                        + triangle.mPointArray[1] + triangle.mPointArray[2])/3.0;
                    if (rayIntersectsMesh((faceCenter + vertexPtr->position())*0.5f, 
                            triangleCenter, mMesh->vertexEnd(), lightFacePtr, facePtr)) {
                        continue;
                    }
                }

                const cgmath::Vector3f normal = mesh::GetFaceVertexNormal(facePtr, 
                    vertexPtr, mNormal3fAttributeKey);

                // If the normal vector is somehow zero, don't bother shading it.
                // We think this shows up occasionally for degenerate faces.
                if (normal == cgmath::Vector3f::ZERO) {
                    continue;
                }

                // The normal must not contain NaN values.
                assert(normal*0.0 == cgmath::Vector3f::ZERO);

                cgmath::Vector3f intensity 
                    = lightFace.computeIntensityAtPoint(vertexPtr->position(), normal, triangle)
                    *mMaterialTable->getFaceVertexDiffuseColor(facePtr, vertexPtr);

                cgmath::Vector3f oldIntensity = facePtr->getVertexVector3f(vertexPtr, 
                    mIlluminatedColor3fAttributeKey);
                facePtr->setVertexVector3f(vertexPtr, mIlluminatedColor3fAttributeKey,
                    oldIntensity + intensity);
            }
        }
    }
}

bool
MeshShader::rayIntersectsMesh(const cgmath::Vector3f &rayOrigin,
    const cgmath::Vector3f &rayEndpoint, mesh::VertexPtr localVertexToIgnore,
    mesh::FacePtr emissiveFaceToIgnore, mesh::FacePtr localFaceToIgnore)
{
    MeshShaderFaceListener meshShaderFaceListener;
    meshShaderFaceListener.setMesh(mMesh);
    meshShaderFaceListener.setDiscontinuityMesher(mDiscontinuityMesher);
    meshShaderFaceListener.setRayOrigin(rayOrigin);
    meshShaderFaceListener.setRayEndpoint(rayEndpoint);
    meshShaderFaceListener.setLocalVertexToIgnore(localVertexToIgnore);
    meshShaderFaceListener.setEmissiveFaceToIgnore(emissiveFaceToIgnore);
    meshShaderFaceListener.setLocalFaceToIgnore(localFaceToIgnore);
    meshShaderFaceListener.initialize();

    mFaceIntersector.setIntersectorFaceListener(&meshShaderFaceListener);
    bool result = mFaceIntersector.occludesRaySegment(rayOrigin, rayEndpoint);
    mFaceIntersector.setIntersectorFaceListener(NULL);

    return result;
}

bool
MeshShader::faceIsBackfacing(mesh::FacePtr facePtr, const meshretri::Triangle &triangle)
{
    const cgmath::Vector3f faceNormal = mesh::GetFaceGeometricNormal(facePtr);
    for (mesh::AdjacentVertexIterator iterator = facePtr->adjacentVertexBegin();
         iterator != facePtr->adjacentVertexEnd(); ++iterator) {
        mesh::VertexPtr vertexPtr = *iterator;
        const cgmath::Vector3f &vertexPosition = vertexPtr->position();
        for (size_t index = 0; index < 3; ++index) {
            const cgmath::Vector3f &trianglePoint = triangle.mPointArray[index];
            if ((trianglePoint - vertexPosition).dot(faceNormal) < 0.0) {
                return true;
            }
        }
    }

    return false;
}

bool
MeshShader::faceVertexSharesNormalWithAdjacentBackfacingFace(mesh::FacePtr facePtr,
    mesh::VertexPtr vertexPtr, const meshretri::Triangle &triangle)
{
    const cgmath::Vector3f vertexNormal = mesh::GetFaceVertexNormal(facePtr, 
        vertexPtr, mNormal3fAttributeKey);

    for (mesh::AdjacentFaceIterator iterator = vertexPtr->adjacentFaceBegin();
         iterator != vertexPtr->adjacentFaceEnd(); ++iterator) {
        mesh::FacePtr adjacentFacePtr = *iterator;

        if (adjacentFacePtr == facePtr) {
            // Don't compare the original face with itself.
            continue;
        }

        const cgmath::Vector3f adjacentVertexNormal = mesh::GetFaceVertexNormal(
            adjacentFacePtr, vertexPtr, mNormal3fAttributeKey);

        if ((adjacentVertexNormal - vertexNormal).length() > cgmath::TOLERANCE) {
            // We're only concerned with adjacent faces that share this vertex normal.
            continue;
        }

        // At this point we have an adjacent face that shares the vertex normal
        // of the original face.

        if (!mesh::FaceIsDegenerate(adjacentFacePtr,
                mesh::GetEpsilonFromFace(adjacentFacePtr))
            && faceIsBackfacing(adjacentFacePtr, triangle)) {
            // The input face vertex is adjacent to another face sharing the
            // same face vertex normal, and that face is backfacing
            // with respect to the light source triangle.
            return true;
        }
    }

    return false;
}

size_t
MeshShader::getDistantLightFaceCount() const
{
    size_t result = 0;

    const DiscontinuityMesher::DistantAreaLightVector &distantAreaLightVector
        = mDiscontinuityMesher->distantAreaLightVector();
    for (size_t index = 0; index < distantAreaLightVector.size(); ++index) {
        const light::DistantAreaLight &distantAreaLight = distantAreaLightVector[index];
        result += distantAreaLight.sides();
    }

    return result;
}

void
MeshShader::printShadingStatistics()
{
    if (!con::LogLevelIsEnabled(con::LOG_LEVEL_INFO)) {
        return;
    }

    float minIntensity = std::numeric_limits<float>::max();
    float maxIntensity = 0.0;
    float totalIntensity = 0.0;
    unsigned sampleCount = 0;

    for (mesh::VertexPtr vertexPtr = mMesh->vertexBegin();
         vertexPtr != mMesh->vertexEnd(); ++vertexPtr) {

        for (mesh::AdjacentFaceIterator iterator = vertexPtr->adjacentFaceBegin();
             iterator != vertexPtr->adjacentFaceEnd(); ++iterator) {
            mesh::FacePtr facePtr = *iterator;

            // Ignore emissive faces.
            if (mDiscontinuityMesher->faceIsLightSource(facePtr)) {
                continue;
            }

            cgmath::Vector3f intensity = facePtr->getVertexVector3f(vertexPtr, 
                mIlluminatedColor3fAttributeKey);

            for (unsigned index = 0; index < 3; ++index) {
                minIntensity = std::min(minIntensity, intensity[index]);
                maxIntensity = std::max(maxIntensity, intensity[index]);
                totalIntensity += intensity[index];
                ++sampleCount;
            }
        }
    }

    float averageIntensity = totalIntensity/sampleCount;

    con::debug << "Average face vertex color intensity: " << averageIntensity << std::endl;
    con::debug << "Min face vertex color intensity: " << minIntensity << std::endl;
    con::debug << "Max face vertex color intensity: " << maxIntensity << std::endl;

    con::debug << "Scale all light source intensities by "
        << 1.0/maxIntensity << " to normalize the max intensity." << std::endl;
}

void
MeshShader::createDistantAreaLightFace()
{
    mDistantAreaLightFace = mMesh->createFace();

    mDistantAreaLightVertex0 = mMesh->createVertex();
    mDistantAreaLightVertex1 = mMesh->createVertex();
    mDistantAreaLightVertex2 = mMesh->createVertex();
    mDistantAreaLightVertex0->addAdjacentFace(mDistantAreaLightFace);
    mDistantAreaLightVertex1->addAdjacentFace(mDistantAreaLightFace);
    mDistantAreaLightVertex2->addAdjacentFace(mDistantAreaLightFace);
    mDistantAreaLightFace->addAdjacentVertex(mDistantAreaLightVertex0);
    mDistantAreaLightFace->addAdjacentVertex(mDistantAreaLightVertex1);
    mDistantAreaLightFace->addAdjacentVertex(mDistantAreaLightVertex2);

    mDistantAreaLightEdge01 = mMesh->createEdge();
    mDistantAreaLightEdge12 = mMesh->createEdge();
    mDistantAreaLightEdge20 = mMesh->createEdge();
    mDistantAreaLightEdge01->addAdjacentVertex(mDistantAreaLightVertex0);
    mDistantAreaLightEdge01->addAdjacentVertex(mDistantAreaLightVertex1);
    mDistantAreaLightEdge12->addAdjacentVertex(mDistantAreaLightVertex1);
    mDistantAreaLightEdge12->addAdjacentVertex(mDistantAreaLightVertex2);
    mDistantAreaLightEdge20->addAdjacentVertex(mDistantAreaLightVertex2);
    mDistantAreaLightEdge20->addAdjacentVertex(mDistantAreaLightVertex0);
    mDistantAreaLightVertex0->addAdjacentEdge(mDistantAreaLightEdge01);
    mDistantAreaLightVertex0->addAdjacentEdge(mDistantAreaLightEdge20);
    mDistantAreaLightVertex1->addAdjacentEdge(mDistantAreaLightEdge01);
    mDistantAreaLightVertex1->addAdjacentEdge(mDistantAreaLightEdge12);
    mDistantAreaLightVertex2->addAdjacentEdge(mDistantAreaLightEdge20);
    mDistantAreaLightVertex2->addAdjacentEdge(mDistantAreaLightEdge12);

    mDistantAreaLightEdge01->addAdjacentFace(mDistantAreaLightFace);
    mDistantAreaLightEdge12->addAdjacentFace(mDistantAreaLightFace);
    mDistantAreaLightEdge20->addAdjacentFace(mDistantAreaLightFace);
    mDistantAreaLightFace->addAdjacentEdge(mDistantAreaLightEdge01);
    mDistantAreaLightFace->addAdjacentEdge(mDistantAreaLightEdge12);
    mDistantAreaLightFace->addAdjacentEdge(mDistantAreaLightEdge20);

    assert(mesh::IsConsistent(*mMesh));
}

void
MeshShader::destroyDistantAreaLightFace()
{
    mMesh->destroyVertex(mDistantAreaLightVertex0);
    mMesh->destroyVertex(mDistantAreaLightVertex1);
    mMesh->destroyVertex(mDistantAreaLightVertex2);
    mMesh->destroyEdge(mDistantAreaLightEdge01);
    mMesh->destroyEdge(mDistantAreaLightEdge12);
    mMesh->destroyEdge(mDistantAreaLightEdge20);
    mMesh->destroyFace(mDistantAreaLightFace);
}

void
MeshShader::dumpBackprojectionTriangle(const meshretri::Triangle &triangle,
    IlluminationState illuminationState)
{
    mDumpedBackprojectionTriangleVector.push_back(triangle);

    if (illuminationState == OCCLUDED) {
        mDumpedOccludedBackprojectionTriangleVector.push_back(triangle);
    }
}

bool
MeshShader::hasDumpedBackprojectionTriangles() const
{
    return !mDumpedBackprojectionTriangleVector.empty();
}

void
MeshShader::dumpBackprojectionSvgFile()
{
    svg::SvgWriter svgWriter;

    const meshretri::Triangle &triangle = mDumpedBackprojectionTriangleVector[0];
    cgmath::Vector3f xAxis = triangle.mPointArray[1] - triangle.mPointArray[0];
    cgmath::Vector3f yAxis = triangle.mPointArray[2] - triangle.mPointArray[0];
    cgmath::Vector3f zAxis = xAxis.cross(yAxis).normalized();
    cgmath::Vector3f up = cgmath::Vector3f(0.0, 1.0, 0.0);
    if (zAxis.equivalent(up, cgmath::TOLERANCE)) {
        yAxis = zAxis.cross(xAxis).normalized();
    } else {
        // Orient the coordinate space so that the object space Y axis points
        // in the world space +Y direction.
        xAxis = up.cross(zAxis).normalized();
        yAxis = zAxis.cross(xAxis).normalized();
    }

    for (meshretri::TriangleVector::const_iterator iterator 
             = mDumpedOccludedBackprojectionTriangleVector.begin();
         iterator != mDumpedOccludedBackprojectionTriangleVector.end(); ++iterator) {
        const meshretri::Triangle &triangle = *iterator;
        for (size_t index = 0; index < 3; ++index) {
            svgWriter.addTriangle(
                cgmath::Vector2f(
                    triangle.mPointArray[0].dot(xAxis),
                    triangle.mPointArray[0].dot(yAxis)),
                cgmath::Vector2f(
                    triangle.mPointArray[1].dot(xAxis),
                    triangle.mPointArray[1].dot(yAxis)),
                cgmath::Vector2f(
                    triangle.mPointArray[2].dot(xAxis),
                    triangle.mPointArray[2].dot(yAxis)));
        }
    }

    for (meshretri::TriangleVector::const_iterator iterator 
             = mDumpedBackprojectionTriangleVector.begin();
         iterator != mDumpedBackprojectionTriangleVector.end(); ++iterator) {
        const meshretri::Triangle &triangle = *iterator;
        for (size_t index = 0; index < 3; ++index) {
            svgWriter.addLineSegment(
                cgmath::Vector2f(
                    triangle.mPointArray[index].dot(xAxis),
                    triangle.mPointArray[index].dot(yAxis)),
                cgmath::Vector2f(
                    triangle.mPointArray[(index + 1) % 3].dot(xAxis),
                    triangle.mPointArray[(index + 1) % 3].dot(yAxis)), 0.25);
        }
    }

    std::string filename = "/var/tmp/backprojection.svg";

    con::info << "Writing backprojection to \"" << filename << "\"." << std::endl;

    svgWriter.writeToSvgFile(filename);
}
