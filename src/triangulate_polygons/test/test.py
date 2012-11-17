#!/usr/bin/python

import os
import sys

result = os.system('cd .. ; src_build debug')
if result != 0:
    print "    Failed"
    sys.exit(result)

inputDir = "input"
failures = 0

def runTest(file):
    global failures
    command = "svg2csv.py " + inputDir + "/" + file + " points.csv edges.csv"
    print command
    result = os.system(command)
    if result & 255 != 0:
        print "    Interrupted"
        os.unlink("points.csv")
        os.unlink("edges.csv")
        sys.exit(result)
    if result != 0:
        print "    Failed"
        failures = failures + 1

    command = "triangulate_polygons.debug points.csv edges.csv --svg-file output/" \
              + file.split('.')[0] + ".svg"
    print command
    result = os.system(command)
    if result & 255 != 0:
        print "    Interrupted"
        os.unlink("points.csv")
        os.unlink("edges.csv")
        sys.exit(result)
    if result != 0:
        print "    Failed"
        failures = failures + 1
    
    os.unlink("points.csv")
    os.unlink("edges.csv")

for file in os.listdir(inputDir):
    if file != '.svn':
        runTest(file)
sys.exit(failures > 0)
