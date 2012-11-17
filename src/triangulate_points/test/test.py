#!/usr/bin/python

# This script runs triangulate_points.debug several times with
# various configurations of input data and tests for failure.

import os
import sys
import math
import random

result = os.system('cd .. ; src_build debug')
if result != 0:
    print "    Failed"
    sys.exit(result)

failures = 0

def runAllTests():
    runTest(constrainedHoleTest)
    runTest(constrainedTest)
    runTest(clusterTest)
    runTest(skewedExponentialTest)
    runTest(centeredExponentialTest)
    runTest(exponentialTest)
    runTest(spiralTest)
    runTest(alignedGridTest)
    runTest(rotatedGridTest)
    runTest(randomTest)
    runTest(wheelTest)
    runTest(circleTest)
    runTest(binaryTest)

# Return only the unique items in the list, while preserving order.
def unique(list):
    set = {}
    return [set.setdefault(e, e) for e in list if e not in set]
    
def runTest(test):
    testName = test.__name__
    pointList = []
    edgeList = []
    random.seed(0)
    test(pointList, edgeList)
    pointList = unique(pointList)
    pointsFilename = "input/" + testName + ".points.csv"
    edgesFilename = "input/" + testName + ".edges.csv"
    svgFilename = "output/" + testName + ".svg"
    file = open(pointsFilename, "w")
    for point in pointList:
        print >> file, str(point[0]) + ", " + str(point[1])
    file.close()
    command = "triangulate_points.debug " + pointsFilename + " --svg-file " + svgFilename
    if len(edgeList) > 0:
        file = open(edgesFilename, "w")
        for edge in edgeList:
            print >> file, str(edge[0]) + ", " + str(edge[1])
        file.close()
        command += " --edge-file " + edgesFilename
    print command
    result = os.system(command)
    if result & 255 != 0:
        print "    Interrupted"
        sys.exit(result)
    if result != 0:
        print "    Failed"
        global failures
        failures = failures + 1

# Test of constrained Delaunay triangulation with a hole
def constrainedHoleTest(pointList, edgeList):
    pointList.append((20, 20))
    pointList.append((20, 80))
    pointList.append((80, 80))
    pointList.append((80, 20))
    edgeList.append((0, 1))
    edgeList.append((1, 2))
    edgeList.append((2, 3))
    edgeList.append((3, 0))
    pointList.append((0, 0))
    pointList.append((0, 100))
    pointList.append((100, 100))
    pointList.append((100, 0))
    for index in range(0, 300):
        x = random.random()*100
        y = random.random()*100
        if x >= 20 and x <= 80 and y >= 20 and y <= 80:
            continue
        pointList.append((x, y))

# Test of constrained Delaunay triangulation
def constrainedTest(pointList, edgeList):
    pointList.append((20, 20))
    pointList.append((20, 80))
    pointList.append((80, 80))
    pointList.append((80, 20))
    edgeList.append((0, 1))
    edgeList.append((1, 2))
    edgeList.append((2, 3))
    edgeList.append((3, 0))
    pointList.append((0, 0))
    pointList.append((0, 100))
    pointList.append((100, 100))
    pointList.append((100, 0))
    for index in range(0, 300):
        x = random.random()*100
        y = random.random()*100
        if x == 20 or x == 80 or y == 20 or y == 80:
            continue
        pointList.append((x, y))

# Points clustered together in the center.
def clusterTest(pointList, edgeList):
    pointList.append((0, 0))
    pointList.append((0, 100))
    pointList.append((100, 100))
    pointList.append((100, 0))
    for index in range(0, 350):
        while True:
            x = 2.0*random.random() - 1.0
            y = 2.0*random.random() - 1.0
            if x*x + y*y < 1.0:
                break
        x = x*10 + 50.0
        y = y*10 + 50.0
        pointList.append((x, y))

# Points arranged in an skewed exponential pattern.
def skewedExponentialTest(pointList, edgeList):
    pointList.append((0, 0))
    pointList.append((0, 100))
    pointList.append((100, 50))
    pointList.append((100, 100))
    for index in range(0, 350):
        x = random.random()*100.0
        y = pow(random.random(), 20.0)*100.0
        y = (y/100.0*(1.0 - x/200.0) + x/100.0*0.5)*100.0
        pointList.append((x, y))

# Points arranged in an centered exponential pattern.
def centeredExponentialTest(pointList, edgeList):
    pointList.append((0, 0))
    pointList.append((0, 100))
    pointList.append((100, 0))
    pointList.append((100, 100))
    for index in range(0, 350):
        x = random.random()*100.0
        y = pow(2.0*random.random() - 1.0, 19.0)*50.0 + 50.0
        pointList.append((x, y))

# Points arranged in an exponential pattern.
def exponentialTest(pointList, edgeList):
    pointList.append((0, 0))
    pointList.append((0, 100))
    pointList.append((100, 0))
    pointList.append((100, 100))
    for index in range(0, 350):
        x = random.random()*100.0
        y = pow(random.random(), 20.0)*100.0
        pointList.append((x, y))

# A spiral.
def spiralTest(pointList, edgeList):
    radians = 0
    for index in range(10, 150):
        radius = index
        radians += 300.0*3.1415/180.0/radius
        px = math.cos(radians)*radius
        py = math.sin(radians)*radius
        pointList.append((px, py))

# A regular grid aligned to the principal axes.
def alignedGridTest(pointList, edgeList):
    for x in range(0, 10):
        for y in range(0, 10):
            pointList.append((x, y))

# A regular grid that's slightly rotated. This creates many sets of points
# that are nearly colinear and nearly cocircular.
def rotatedGridTest(pointList, edgeList):
    degrees = 3
    radians = degrees*0.0174532925
    for x in range(0, 10):
        for y in range(0, 10):
            px = math.cos(radians)*x - math.sin(radians)*y
            py = math.sin(radians)*x + math.cos(radians)*y
            pointList.append((px, py))

# Points randomly placed along grid lines.
def randomTest(pointList, edgeList):
    random.seed(42)
    for index in range(0, 100):
        x = random.randint(0, 100)
        y = random.randint(0, 100)
        pointList.append((x, y))

# Three concentric rings of points with a single point in the middle.
def wheelTest(pointList, edgeList):
    radius = 100
    pointList.append((0, 0))
    for index in range(0, 90):
        radians = 4*index*3.1415/180.0
        px = math.cos(radians)*radius
        py = math.sin(radians)*radius
        pointList.append((px, py))
        px = math.cos(radians + 2*3.1415/180.0)*radius*0.9
        py = math.sin(radians + 2*3.1415/180.0)*radius*0.9
        pointList.append((px, py))
        px = math.cos(radians + 0*3.1415/180.0)*radius*0.8
        py = math.sin(radians + 0*3.1415/180.0)*radius*0.8
        pointList.append((px, py))

# A circle filled with roughly uniformly positioned random points.
def circleTest(pointList, edgeList):
    random.seed(42)
    ringCount = 10
    pointList.append((0, 0))
    for ring in range(1, ringCount):
        maxIndex = int(ring*3.1415*2)
        randomAngle = 2.0*3.1415*random.random()
        for index in range(0, maxIndex):
            angle = 2.0*3.1415*index/maxIndex + randomAngle
            angle += random.random()*2.0*3.1415/maxIndex*0.25
            radius = ring
            if ring < ringCount - 1:
                radius += random.random()*0.25 - 0.125
            point = (math.cos(angle)*radius, math.sin(angle)*radius)
            pointList.append(point)

# Points arranged in a binary subdivision pattern.
def binaryTest(pointList, edgeList):
    rows = 8
    y = 0
    yIncrement = 128
    for row in range(1, rows):
        columns = pow(2, row)
        for column in range(0, columns + 1):
            x = 256/columns*column
            pointList.append((x, 256 - y))
        y += yIncrement
        yIncrement /= 2

runAllTests()
sys.exit(failures > 0)
