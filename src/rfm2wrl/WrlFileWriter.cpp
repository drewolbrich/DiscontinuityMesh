// Copyright 2008 Retroactive Fiasco.

#include "WrlFileWriter.h"

#include <cmath>
#include <cstdlib>
#include <cassert>
#include <algorithm>

#include <mesh/IsConsistent.h>
#include <mesh/MeshOperations.h>
#include <mesh/FaceVertex.h>
#include <mesh/StandardAttributes.h>
#include <except/FailedOperationException.h>
#include <except/OpenFileException.h>
#include <os/Error.h>
#include <cgmath/Vector3f.h>

using except::FailedOperationException;
using except::OpenFileException;
using os::Error;
using cgmath::Vector3f;

WrlFileWriter::WrlFileWriter()
    : mSkyColor(0.0, 0.0, 0.0),
      mFile(NULL),
      mMesh(NULL),
      mWrlFilename(),
      mOffset(Vector3f::ZERO),
      mVertexPtrIndexMap()
{
}

WrlFileWriter::~WrlFileWriter()
{
}

void
WrlFileWriter::setSkyColor(const cgmath::Vector3f &skyColor)
{
    mSkyColor = skyColor;
}

void
WrlFileWriter::writeWrlFile(const mesh::Mesh &mesh, const std::string &wrlFilename)
{
    mMesh = &mesh;

    // Make sure the references between the mesh components
    // are consistent. (Only has an effect in debug builds.)
    assert(mesh::IsConsistent(*mMesh));

    mWrlFilename = wrlFilename;

    // Try to open the WRL file. By declaring the std::ofstream in
    // local storage, and then referencing it throughout the class via
    // a pointer, we ensure that the file will be closed if an
    // exception is thrown.
    std::ofstream file(mWrlFilename.c_str());

    mFile = &file;

    if (!*mFile) {
        throw OpenFileException(SOURCE_LINE, Error::fromSystemError())
            << "Could not open file \"" << mWrlFilename << "\".";
    }

    writeHeader();
    writeCoordinates();
    writeColors();
    writeCoordinateIndices();
    writeColorIndices();
    writeFooter();
}

void
WrlFileWriter::writeHeader()
{
    cgmath::BoundingBox3f bbox = mesh::ComputeBoundingBox(*mMesh);

    // If the origin does not lie within the object's bounding box,
    // apply an offset to the object's coordinates so that it
    // is centered at the origin.
    // A better way to do this might be to calculate an appropriate
    // position and orientation for the viewpoint so that it points at the center
    // of the object's bounding box.
    if (bbox.minX() > 0.0
        || bbox.minY() > 0.0
        || bbox.minZ() > 0.0
        || bbox.maxX() < 0.0
        || bbox.maxY() < 0.0
        || bbox.maxZ() < 0.0) {
        mOffset = -bbox.center();
    } else {
        mOffset = Vector3f::ZERO;
    }

    // Default VRML field of view, in radians.
    const float DEFAULT_VRML_VIEWING_ANGLE = 0.785398;   // 45 degrees

    // Calculate the position of the camera such that the longest axis
    // of the object's bounding box will fit in the window.
    float distance = std::max(bbox.sizeX(), std::max(bbox.sizeY(), bbox.sizeZ()))/2.0
        /tan(DEFAULT_VRML_VIEWING_ANGLE/2.0);

    // Back off the camera a little to provide a margin.
    distance *= 1.2;

    Vector3f viewpointPosition = Vector3f::ZERO;
    viewpointPosition[2] = distance;

    *mFile << "#VRML V2.0 utf8\n";
    *mFile << "\n";
    *mFile << "Group {\n";
    *mFile << "    children [\n";
    *mFile << "        Viewpoint {\n";
    *mFile << "            position " 
        << viewpointPosition[0]
        << " " << viewpointPosition[1]
        << " " << viewpointPosition[2]
        << "\n";
    *mFile << "        },\n";
    *mFile << "        NavigationInfo {\n";
    *mFile << "            headlight FALSE\n";
    *mFile << "        },\n";
    *mFile << "        Background {\n";
    *mFile << "            skyColor " << mSkyColor << "\n";
    *mFile << "        },\n";
    *mFile << "        Shape {\n";
    *mFile << "            geometry IndexedFaceSet {\n";
}

void
WrlFileWriter::writeCoordinates()
{
    *mFile << "                coord Coordinate {\n";
    *mFile << "                    point [\n";

    uint32_t index = 0;
    for (mesh::ConstVertexPtr vertexPtr = mMesh->vertexBegin();
         vertexPtr != mMesh->vertexEnd(); ++vertexPtr) {

        Vector3f position = vertexPtr->position();

        position += mOffset;

        *mFile << "                        ";
        *mFile << position[0] << " " << position[1] << " " << position[2];
        *mFile << "\n";

        if (mFile->fail()) {
            throw FailedOperationException(SOURCE_LINE, Error::fromSystemError())
                << "Could not write vertex to file \"" 
                    << mWrlFilename << "\".";
        }
        
        mVertexPtrIndexMap.insert(std::make_pair(vertexPtr, index));
        ++index;
    }

    *mFile << "                    ]\n";
    *mFile << "                }\n";
}

void
WrlFileWriter::writeColors()
{
    mesh::AttributeKey color3fKey = mesh::GetColor3fAttributeKey(*mMesh);

    *mFile << "                color Color {\n";
    *mFile << "                    color [\n";

    for (mesh::ConstFacePtr facePtr = mMesh->faceBegin();
         facePtr != mMesh->faceEnd(); ++facePtr) {

        for (mesh::AdjacentVertexConstIterator iterator = facePtr->adjacentVertexBegin();
             iterator != facePtr->adjacentVertexEnd(); ++iterator) {
            const mesh::FaceVertex *faceVertex = facePtr->findFaceVertex(*iterator);

            // This color is used when a face's vertex color is undefined.
            Vector3f color(0.2, 0.2, 0.2);
            if (faceVertex != NULL
                && faceVertex->hasAttribute(color3fKey)) {
                color = faceVertex->getVector3f(color3fKey);
            } else if (facePtr->hasAttribute(color3fKey)) {
                color = facePtr->getVector3f(color3fKey);
            }

            // If the color values are NaN, write out 1.0 instead,
            // because the Instant Player VRML viewer chokes on them.
            for (size_t index = 0; index < 3; ++index) {
                if (color[index]*0.0 != 0.0) {
                    color[index] = 1.0;
                }
            }

            *mFile << "                        ";
            *mFile << color[0] << " " << color[1] << " " << color[2];
            *mFile << "\n";

            if (mFile->fail()) {
                throw FailedOperationException(SOURCE_LINE, Error::fromSystemError())
                    << "Could not write face vertex color to file \"" 
                        << mWrlFilename << "\".";
            }
        }
    }

    *mFile << "                    ]\n";
    *mFile << "                }\n";    
}

void
WrlFileWriter::writeCoordinateIndices()
{
    *mFile << "                coordIndex [\n";

    for (mesh::ConstFacePtr facePtr = mMesh->faceBegin();
         facePtr != mMesh->faceEnd(); ++facePtr) {

        *mFile << "                    ";

        // Loop over all the Face's adjacent vertices.
        for (mesh::AdjacentVertexConstIterator iterator
                 = facePtr->adjacentVertexBegin();
             iterator != facePtr->adjacentVertexEnd(); ++iterator) {
            const mesh::VertexPtr &vertexPtr(*iterator);

            // The VertexPtr must exist in the map.
            VertexPtrIndexMap::iterator vertexPtrIndexMapIterator
                = mVertexPtrIndexMap.find(vertexPtr);
            assert(vertexPtrIndexMapIterator != mVertexPtrIndexMap.end());

            uint32_t vertexIndex = (*vertexPtrIndexMapIterator).second;
            *mFile << vertexIndex << " ";

            if (mFile->fail()) {
                throw FailedOperationException(SOURCE_LINE, Error::fromSystemError())
                    << "Could not write coordinate index to file \"" 
                        << mWrlFilename << "\".";
            }
        }

        *mFile << "-1";
        *mFile << "\n";
    }

    *mFile << "                ]\n";
}

void
WrlFileWriter::writeColorIndices()
{
    *mFile << "                colorIndex [\n";

    uint32_t index = 0;
    for (mesh::ConstFacePtr facePtr = mMesh->faceBegin();
         facePtr != mMesh->faceEnd(); ++facePtr) {

        *mFile << "                    ";

        for (mesh::AdjacentVertexConstIterator iterator = facePtr->adjacentVertexBegin();
             iterator != facePtr->adjacentVertexEnd(); ++iterator) {

            *mFile << index << " ";

            if (mFile->fail()) {
                throw FailedOperationException(SOURCE_LINE, Error::fromSystemError())
                    << "Could not write color index to file \"" 
                        << mWrlFilename << "\".";
            }

            ++index;
        }

        *mFile << "-1";
        *mFile << "\n";
    }

    *mFile << "                ]\n";
}

void
WrlFileWriter::writeFooter()
{
    *mFile << "                solid TRUE\n";
    *mFile << "                colorPerVertex TRUE\n";
    *mFile << "            }\n";
    *mFile << "        }\n";
    *mFile << "    ]\n";
    *mFile << "}\n";
}

