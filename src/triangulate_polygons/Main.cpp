// Copyright 2008 Retroactive Fiasco.

#include <cstdlib>
#include <cstdio>
#include <cassert>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <limits>

#include <sys/errno.h>

#include <opt/ProgramOptionsParser.h>
#include <con/Streams.h>
#include <cgmath/Vector2f.h>
#include <delaunay/PolygonTriangulator.h>
#include <delaunay/ValidationDetails.h>

static opt::ProgramOptionsParser gOptions;

// Parse the command line arguments.
static void ParseCommandLineArguments(int argc, char **argv);

// Read the CSV file of input points.
static void ReadPointFile(const std::string &filename, 
    delaunay::PolygonTriangulator::PointVector &pointVector);

// Read the CSV file of input polygons.
static void ReadPolygonFile(const std::string &filename,
    delaunay::PolygonTriangulator::PolygonVector &polygonVector);

int
main(int argc, char **argv)
{
    ParseCommandLineArguments(argc, argv);

    assert(gOptions.specified("point-file"));
    std::string pointFilename = gOptions.get("point-file").as<std::string>();

    assert(gOptions.specified("polygon-file"));
    std::string polygonFilename = gOptions.get("polygon-file").as<std::string>();

    delaunay::PolygonTriangulator::PointVector pointVector;
    ReadPointFile(pointFilename, pointVector);

    delaunay::PolygonTriangulator::PolygonVector polygonVector;
    ReadPolygonFile(polygonFilename, polygonVector);

    delaunay::PolygonTriangulator polygonTriangulator;
    polygonTriangulator.setInputPointVector(&pointVector);
    polygonTriangulator.setInputPolygonVector(&polygonVector);

    if (gOptions.specified("svg-file")) {
        polygonTriangulator.setSvgFilename(gOptions.get("svg-file").as<std::string>());
    }

    delaunay::ValidationDetails validationDetails;
    if (!polygonTriangulator.validate(&validationDetails)) {
        con::error << "Could not triangulate input polygons. "
            << validationDetails.asString() << std::endl;
        exit(EXIT_FAILURE);
    }

    polygonTriangulator.triangulate();

    exit(EXIT_SUCCESS);
}

static void 
ParseCommandLineArguments(int argc, char **argv)
{
    gOptions.setUsageSummary("points.csv polygons.csv [options]");
    gOptions.setProgramPurpose("Triangulates a set of 2D polygons.");

    gOptions.addRequiredPositionalOptions()
        ("point-file", "CSV file containing points to triangulate")
        ("polygon-file", "CSV file containing polygons to triangulate")
        ;

    gOptions.addOptions()
        ("svg-file", opt::value<std::string>(), "Output SVG file of triangles")
        ;

    gOptions.parse(argc, argv);
}

static void 
ReadPointFile(const std::string &filename, 
    delaunay::PolygonTriangulator::PointVector &pointVector)
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
ReadPolygonFile(const std::string &filename,
    delaunay::PolygonTriangulator::PolygonVector &polygonVector)
{
    polygonVector.clear();

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

        delaunay::PolygonTriangulator::Polygon polygon;

        char comma;
        bool first = true;
        while (true) {

            if (!first) {
                istr >> comma;
                if (comma != ',') {
                    con::error << "Syntax error on line " << lineNumber 
                        << " of file " << filename << "." << std::endl;
                    exit(EXIT_FAILURE);
                }
            }
            first = false;

            if (istr.eof()) {
                con::error << "Syntax error on line " << lineNumber 
                    << " of file " << filename << "." << std::endl;
                exit(EXIT_FAILURE);
            }

            size_t index = std::numeric_limits<size_t>::max();
            istr >> index;

            if (index == std::numeric_limits<size_t>::max()) {
                con::error << "Syntax error on line " << lineNumber 
                    << " of file " << filename << "." << std::endl;
                exit(EXIT_FAILURE);
            }

            polygon.mPointIndexVector.push_back(index);
        
            if (istr.eof()) {
                break;
            }
        }

        polygonVector.push_back(polygon);
    }

    file.close();
}
