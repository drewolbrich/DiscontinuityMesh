#!/usr/bin/python

# This script tests random input for sequences of 50 to 500 points
# times the runs, and outputs the results as a CSV file so I can graph
# them in Excel, before and after I optimize the triangulation code.


import os
import sys
import random
import time

# Return only the unique items in the list, while preserving order.
def unique(list):
    set = {}
    return [set.setdefault(e, e) for e in list if e not in set]
    
pointsFilename = "input/timingTest.csv"
iteration = 1
random.seed(3373)
timingFile = open("timing_test.csv", "w");
for iteration in range(0, 15):
    pointCount = iteration*100 + 100
    pointList = []
    for index in range(0, pointCount):
        x = random.random()
        y = random.random()
        pointList.append((x, y))
    pointList = unique(pointList);
    file = open(pointsFilename, "w")
    for point in pointList:
        print >> file, str(point[0]) + ", " + str(point[1])
    file.close()

    command = "triangulate_points " + pointsFilename

    # Run the test seven times and use the median timing result.
    timingResultArray = []
    for index in range(0, 7):
        startTime = time.time()
        result = os.system(command)
        stopTime = time.time()
        if result & 255 != 0:
            sys.exit(result)
        if result != 0:
            print "    Failed"
            os.rename(pointsFilename, "input/timingTestFailure." + str(iteration) + ".csv")
        duration = stopTime - startTime
        timingResultArray.append(duration)
    timingResultArray.sort()
    duration = timingResultArray[3]
    
    iteration += 1
    print "Points:", pointCount, "Duration:", duration
    print >> timingFile, str(pointCount) + ", " + str(duration)
timingFile.close()
