#!/usr/bin/python

import sys
import re

if len(sys.argv) != 4:
    print >> sys.stderr, "Usage: svg2csv input.svg points.csv edges.csv"
    print >> sys.stderr, "Which: Converts an Illustrator SVG file to point and edge CSV files"
    print >> sys.stderr, "       suitable for input to triangulate_polygons."
    sys.exit(1)

svgFilename = sys.argv[1]
pointFilename = sys.argv[2]
edgeFilename = sys.argv[3]

file = open(svgFilename, 'r')
text = file.read()

file.close()

pattern = re.compile('<pattern.*</pattern>(.*)</svg>', re.DOTALL)
matches = pattern.findall(text)

if len(matches) > 0:
    text = matches[0]

pattern = re.compile('<polygon.*?points="(.*?)".*?/>', re.DOTALL)
polygonList = pattern.findall(text)

pointFile = open(pointFilename, 'w')
edgeFile = open(edgeFilename, 'w')

pointIndex = 0
for polygon in polygonList:
    pattern = re.compile('([0-9\.\-e,]+)', re.DOTALL)
    pointList = pattern.findall(polygon)
    pointIndexList = ""
    for point in pointList:
        coord = point.split(',')
        print >> pointFile, coord[0] + ', ' + str(-1.0*float(coord[1]))
        if pointIndexList == "":
            pointIndexList = str(pointIndex)
        else:
            pointIndexList = pointIndexList + ", " + str(pointIndex)
        pointIndex += 1
    print >> edgeFile, pointIndexList
