// Copyright 2008 Retroactive Fiasco.

#include <cstdlib>
#include <cstdio>
#include <cassert>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>

#include <sys/errno.h>

#include <opt/ProgramOptionsParser.h>
#include <con/Streams.h>
#include <cgmath/Vector2f.h>
#include <delaunay/PointTriangulator.h>
#include <delaunay/ValidationDetails.h>

static opt::ProgramOptionsParser gOptions;

// Parse the command line arguments.
static void ParseCommandLineArguments(int argc, char **argv);

// Read the CSV file of input points.
static void ReadInputFile(const std::string &filename, 
    delaunay::PointTriangulator::PointVector &pointVector);

// Read the CSV file of constrained input edges.
static void ReadInputEdgeFile(const std::string &filename,
    delaunay::PointTriangulator::EdgeVector &edgeVector);

// Write out the CSV file of triangles.
static void WriteTriangleFile(const delaunay::PointTriangulator &triangulator, 
    const std::string &filename);

int
main(int argc, char **argv)
{
    ParseCommandLineArguments(argc, argv);

    assert(gOptions.specified("input-file"));
    std::string inputFilename = gOptions.get("input-file").as<std::string>();

    delaunay::PointTriangulator::PointVector pointVector;
    ReadInputFile(inputFilename, pointVector);

    delaunay::PointTriangulator pointTriangulator;
    pointTriangulator.setInputPointVector(&pointVector);

    delaunay::PointTriangulator::EdgeVector inputEdgeVector;
    if (gOptions.specified("edge-file")) {
        ReadInputEdgeFile(gOptions.get("edge-file").as<std::string>(), inputEdgeVector);
        pointTriangulator.setInputEdgeVector(&inputEdgeVector);
    }

    if (gOptions.specified("svg-file")) {
        pointTriangulator.setSvgFilename(gOptions.get("svg-file").as<std::string>());
    }

    if (gOptions.specified("svg-history")) {
        pointTriangulator.setWriteEntireSvgHistory(true);
    }

    if (gOptions.specified("no-shuffle-input")) {
        pointTriangulator.setShouldShuffleInputPoints(false);
    }

    delaunay::ValidationDetails validationDetails;
    if (!pointTriangulator.validate(&validationDetails)) {
        con::error << "Could not triangulate input points. "
            << validationDetails.asString() << std::endl;
        exit(EXIT_FAILURE);
    }

    pointTriangulator.triangulate();

    if (gOptions.specified("triangle-file")) {
        WriteTriangleFile(pointTriangulator, gOptions.get("triangle-file").as<std::string>());
    }

    exit(EXIT_SUCCESS);
}

static void 
ParseCommandLineArguments(int argc, char **argv)
{
    gOptions.setUsageSummary("input.csv [options]");
    gOptions.setProgramPurpose("Computes the Delaunay triangulation of a set of 2D points.");

    gOptions.addRequiredPositionalOptions()
        ("input-file", "CSV file containing points to triangulate")
        ;

    gOptions.addOptions()
        ("edge-file", opt::value<std::string>(), "Input CSV file of constraining edges")
        ("triangle-file", opt::value<std::string>(), "Output CSV file of triangles")
        ("svg-file", opt::value<std::string>(), "Output SVG file of triangles")
        ("svg-history", "Write entire algorithm history to SVG file")
        ("no-shuffle-input", "Do not shuffle the input points")
        ;

    gOptions.parse(argc, argv);
}

static void 
ReadInputFile(const std::string &filename, delaunay::PointTriangulator::PointVector &pointVector)
{
    pointVector.clear();

    std::ifstream file(filename.c_str());
    if (!file) {
        con::error << "Could not open file " << filename 
            << ". (" << strerror(errno) << ")" << std::endl;
        exit(EXIT_FAILURE);
    }

    int lineNumber = 0;
    while (true) {
        ++lineNumber;
        std::string line;
        std::getline(file, line);
        if (file.eof()) {
            break;
        }

        std::istringstream istr(line);
        float x, y;
        char comma;

        istr >> x >> comma >> y;
        if (comma != ',') {
            con::error << "Syntax error on line " << lineNumber 
                << " of file " << filename << "." << std::endl;
            exit(EXIT_FAILURE);
        }

        pointVector.push_back(cgmath::Vector2f(x, y));
    }

    file.close();
}

static void 
ReadInputEdgeFile(const std::string &filename,
    delaunay::PointTriangulator::EdgeVector &edgeVector)
{
    edgeVector.clear();

    std::ifstream file(filename.c_str());
    if (!file) {
        con::error << "Could not open file " << filename 
            << ". (" << strerror(errno) << ")" << std::endl;
        exit(EXIT_FAILURE);
    }

    int lineNumber = 0;
    while (true) {
        ++lineNumber;
        std::string line;
        std::getline(file, line);
        if (file.eof()) {
            break;
        }

        std::istringstream istr(line);
        int index0, index1;
        char comma;

        istr >> index0 >> comma >> index1;
        if (comma != ',') {
            con::error << "Syntax error on line " << lineNumber 
                << " of file " << filename << "." << std::endl;
            exit(EXIT_FAILURE);
        }

        delaunay::PointTriangulator::Edge edge;
        edge.mPointIndexArray[0] = index0;
        edge.mPointIndexArray[1] = index1;
        edgeVector.push_back(edge);
    }

    file.close();
}

static void 
WriteTriangleFile(const delaunay::PointTriangulator &pointTriangulator, 
    const std::string &filename)
{
    std::ofstream file(filename.c_str());
    if (!file) {
        con::error << "Could not open file " << filename
            << ". (" << strerror(errno) << ")" << std::endl;
    }

    const delaunay::PointTriangulator::TriangleVector &triangleVector 
        = pointTriangulator.outputTriangleVector();
    for (size_t index = 0; index < triangleVector.size(); ++index) {
        const delaunay::PointTriangulator::Triangle &triangle = triangleVector[index];
        file << triangle.mPointIndexArray[0] << ", "
            << triangle.mPointIndexArray[1] << ", "
            << triangle.mPointIndexArray[2] << std::endl;
    }
}
