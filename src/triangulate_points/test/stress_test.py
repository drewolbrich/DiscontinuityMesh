#!/usr/bin/python

# This script runs forever, feeding random input into triangulate.debug
# to try to get it to crash. Failures are recorded for later analysis.

import os
import sys
import math
import random
import operator

# Return only the unique items in the list, while preserving order.
def unique(list):
    set = {}
    return [set.setdefault(e, e) for e in list if e not in set]
    
pointsFilename = "input/stressTest.csv"
iteration = 1
failures = 0
random.seed(3373)
while True:
    print "Test:", iteration, " Failures:", failures
    pointList = []

    if iteration % 2 == 0:

        # Random points
        for index in range(0, 200):
            type = random.randint(0, 3)
            x = random.randint(0, 30)
            y = random.randint(0, 30)
            if type == 1:
                degrees = 3
                radians = degrees*0.0174532925
                rx = math.cos(radians)*x - math.sin(radians)*y
                ry = math.sin(radians)*x + math.cos(radians)*y
                x = rx
                y = ry
            if type == 2:
                x = random.random()*30
                y = random.random()*30
            pointList.append((x, y))

    else:

        # Randomly rotated uniform grid
        radians = random.random()*2.0*3.14159265
        for ix in range(0, 14):
            for iy in range(0, 14):
                x = math.cos(radians)*ix - math.sin(radians)*iy
                y = math.sin(radians)*ix + math.cos(radians)*iy
                pointList.append((x, y))

    pointList = unique(pointList);

    file = open(pointsFilename, "w")
    for point in pointList:
        print >> file, str(point[0]) + ", " + str(point[1])
    file.close()
    command = "triangulate_points.debug " + pointsFilename
    result = os.system(command)
    if result & 255 != 0:
        sys.exit(result)
    if result != 0:
        print "    Failed"
        os.rename(pointsFilename, "input/stressTestFailure." + str(iteration) + ".csv")
        failures += 1
    iteration += 1
