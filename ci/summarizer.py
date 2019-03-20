#!/bin/env python3

from sys import argv
from collections import Counter

script, reportfile = argv

def analyze():
    errorsMap = {}
    file = open(reportfile, 'r')
    fileLines = file.readlines()
    cppcheckNumOfErrors = len(fileLines)
    linesSeen = set()
    
    for line in fileLines:  # remove duplicate lines
        if line not in linesSeen:
            linesSeen.add(line)
            
    for line in linesSeen:
        lineContents = line.split(':')
        fileName = lineContents[0]
        error = (lineContents[1], lineContents[2].strip('\n'))

        if fileName not in errorsMap.keys():
            errorsMap[fileName] = list()
            errorsMap[fileName].append(error)
        else:
            errorsMap[fileName].append(error)
            
    return errorsMap

def prettyprint(errors):
    print("\033[1m\033[91m=================================================\n")
    print("\033[1m       Static analysis results: Error Summary        \n")
    for key in errors:
        for error in errors[key]:
            print("\033[1mFile \033[93m{0}\033[91m violates rule \033[93m#{1}\033[91m of the MISRA C 2012 standard at line \033[93m{2}\033[91m".format(key, error[1], error[0]))
    print()
    print("\033[1m=================================================\033[0m\n")

    

if __name__ == "__main__":
    errors = analyze()
    if len(errors) == 0:
        print("\033[1m\033[92mStatic analysis for MISRA compliance complete. No errors found.")
        exit(0)
    else:
        prettyprint(errors)
        exit(1)
    
