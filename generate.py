#!/usr/bin/python
import sys, getopt

def generateGraph(n=100, avgNeigbors=10):
    from random import randint
    for i in range(n):
        print "v "+str(i+1)+" "+str(randint(1, 100))
		
    for i in range(n):
        for m in range(avgNeigbors-randint(-avgNeigbors/2,avgNeigbors/2)):
            print "e "+str(i+1)+" "+str(randint(1,n))+" "+str(randint(1, 100))
			
generateGraph(int(sys.argv[1]), int(sys.argv[2]))