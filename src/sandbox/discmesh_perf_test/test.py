#!/usr/bin/python

import sys
import os
import time

def run(command):
    print command
    result = os.system(command)
    if result != 0:
        sys.exit(result != 0)

run("obj2rfm null.obj null.rfm")
run("obj2rfm box-stack.obj box-stack.rfm --reset-normals")
run("mkdir -p output")

csvFile = open("performance.csv", "w")

for step in range(1, 100):
    print ""
    print "--------------", step, "--------------"
    print ""

    run("cp null.rfm model.rfm")

    for i in range(0, step):
        x = i*10
        for j in range(0, step):
            z = j*10
            ry = ((i*67 + j*79) % 4)*90;
            run("rfm_trans box-stack.rfm temp.rfm --translate " + str(x) + " 0 " + str(z) + " " \
                + "--rotate 0 " + str(ry) + " 0")
            run("rfm_cat model.rfm temp.rfm temp.rfm")
            run("mv temp.rfm model.rfm")
    rfmFilename = "output/model." + str(step) + ".rfm"
    objFilename = "output/model." + str(step) + ".obj"
    wrlFilename = "output/model." + str(step) + ".wrl"

    run("mv model.rfm " + rfmFilename)
    run("rfm2obj " + rfmFilename + " " + objFilename)

    startTime = time.time()

    faces = os.popen("rfm_print " + rfmFilename + " | grep Faces | sed s/Faces://") \
            .readline().strip()

    run("rfm_discmesh " + rfmFilename + " temp.rfm --sun-azimuth 210 --sun-elevation 65 --debug")

    stopTime = time.time()
    
    run("rfm2wrl temp.rfm " + wrlFilename + " --sky-color 0.2 0.2 0.2")

    print >> csvFile, str(faces) + ",", stopTime - startTime
    csvFile.flush()

sys.exit(0)
