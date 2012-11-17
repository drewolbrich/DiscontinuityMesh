#!/usr/bin/python

import os
import sys

failed = False

for file in os.listdir('models'):
    pair = os.path.splitext(file)
    if pair[1] == '.obj':
        baseName = pair[0]
        objFile = 'models/' + file
        linesFile = 'models/' + baseName + '.lines'

        rfmFile = '/var/tmp/rfm_discmesh.temp.rfm'
        command = 'obj2rfm.debug ' + objFile + ' ' + rfmFile
        if os.system(command) != 0:
            failed = True

        if not os.path.exists(linesFile):
            print 'Writing ' + linesFile
            command = 'rfm_discmesh.debug ' + rfmFile + ' --write-lines ' + linesFile
            if os.system(command) != 0:
                failed = True
        else:
            print 'Testing ' + linesFile
            command = 'rfm_discmesh.debug ' + rfmFile + ' --test-lines ' + linesFile
            if os.system(command) != 0:
                failed = True

if failed:
    sys.exit(1)
