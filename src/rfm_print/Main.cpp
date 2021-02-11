// Copyright 2008 Drew Olbrich

// This program prints out a concise description of the
// contents of an rfm file.

#include <cstdlib>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>
#include <exception>
#include <set>

#include <os/Platform.h>

#if OS_APPLE
#include <machine/endian.h>
#else
#include <endian.h>
#endif

#include <opt/ProgramOptionsParser.h>
#include <con/Streams.h>
#include <mesh/Mesh.h>
#include <mesh/AttributeKey.h>
#include <mesh/AttributeKeyMap.h>
#include <mesh/StandardAttributes.h>
#include <meshrfm/ReadRfmFile.h>
#include <meshrfm/RfmFileHeader.h>
#include <cgmath/WriteMatrix.h>
#include <str/StringOperations.h>

static opt::ProgramOptionsParser gOptions;

// The width of the field for right-aligned titles,
// including the colon and trailing space.
static int gTitleWidth = 0;

static void ParseCommandLineArguments(int argc, char **argv);

static void PrintByteOrder(const meshrfm::RfmFileHeader &header);
static void PrintBoundingBox(const meshrfm::RfmFileHeader &header);

int
main(int argc, char **argv)
{
    try {

        ParseCommandLineArguments(argc, argv);

        if (gOptions.specified("file-version")) {
            gTitleWidth = 23;
        } else {
            gTitleWidth = 16;
        }

        con::info << "Reading file header." << std::endl;
    
        mesh::Mesh mesh;
        meshrfm::RfmFileHeader header;
        meshrfm::ReadRfmFileAttributesAndHeader(&mesh, &header, 
            gOptions.get("input-file").as<std::string>());

        con::info << "Printing file summary." << std::endl;

        std::cout << std::setw(gTitleWidth) << "Faces: " 
            << header.mFaceCount << std::endl;
        std::cout << std::setw(gTitleWidth) << "Edges: " 
            << header.mEdgeCount << std::endl;
        std::cout << std::setw(gTitleWidth) << "Vertices: " 
            << header.mVertexCount << std::endl;

        std::cout << std::setw(gTitleWidth) << "Materials: "
            << mesh::GetMaterialIndexVector(mesh).size() << std::endl;
        std::cout << std::setw(gTitleWidth) << "Textures: "
            << mesh::GetTextureIndexVector(mesh).size() << std::endl;
        
        PrintBoundingBox(header);

        if (gOptions.specified("file-version")) {
            std::cout << std::setw(gTitleWidth) << "File format version: " 
                << header.mMajorVersion
                << "." << header.mMinorVersion << std::endl;
            
            PrintByteOrder(header);
        }

    } catch (const std::exception &exception) {
        con::error << exception.what() << std::endl;
        exit(EXIT_FAILURE);
    }

    return EXIT_SUCCESS;
}


static void 
ParseCommandLineArguments(int argc, char **argv)
{
    gOptions.setUsageSummary("input.rfm [options]");
    gOptions.setProgramPurpose("Prints out a summary of the contents of an RFM file.");

    gOptions.addRequiredPositionalOptions()
        ("input-file", "RFM file to print out")
        ;

    gOptions.addOptions()
        ("file-version", "Print file version and byte order")
        ;

    gOptions.parse(argc, argv);
}

static void 
PrintByteOrder(const meshrfm::RfmFileHeader &header)
{
    std::string nativeByteOrder("little-endian");
    std::string foreignByteOrder("big-endian");
    if (BYTE_ORDER != LITTLE_ENDIAN) {
        std::swap(nativeByteOrder, foreignByteOrder);
    }
    
    std::cout << std::setw(gTitleWidth) << "Byte order: ";
    if (header.mByteOrder == meshrfm::RfmFileHeader::NATIVE_BYTE_ORDER) {
        std::cout << "Native (" << nativeByteOrder << ")";
    } else {
        std::cout << "Foreign (" << foreignByteOrder << ")";
    }
    std::cout << std::endl;
}

static void 
PrintBoundingBox(const meshrfm::RfmFileHeader &header)
{
    // The function cgmath::WriteMatrix expects a matrix in column-major
    // order (not row-major order, which is what C/C++ use), so the
    // following matrix is treated as having two columns and three rows.
    float boundingBox[2][3];

    boundingBox[0][0] = header.mBoundingMin[0];
    boundingBox[0][1] = header.mBoundingMin[1];
    boundingBox[0][2] = header.mBoundingMin[2];

    boundingBox[1][0] = header.mBoundingMax[0];
    boundingBox[1][1] = header.mBoundingMax[1];
    boundingBox[1][2] = header.mBoundingMax[2];

    std::ostringstream ostr;
    cgmath::WriteMatrix(ostr, 3, 2, &boundingBox[0][0]);

    // Remove the trailing newline.
    std::string string(ostr.str());
    std::string::size_type pos = string.rfind("\n");
    if (pos != std::string::npos) {
        string.resize(pos);
    }

    std::string indent;
    indent.resize(gTitleWidth, ' ');
    string = str::ReplaceAllSubstrings(string, "\n", 
        std::string("\n") + indent);

    std::cout << std::setw(gTitleWidth) << "Bounding box: "
        << string << std::endl;
}
