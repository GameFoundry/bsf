#!/usr/bin/python

# Script that parses Visual Studio projects and generates a list of source
# and header files used by those projects. The generated files are in CMake
# format. This script is primarily meant to be used by those who use the
# VS projects as their primary workflow, but want to keep the CMake build
# up to date. Simply run this script to re-generate the source/header file
# list in CMakeLists.txt for all projects.
#
# The output is generated as CMakeSources.cmake in every project's sub-directory,
# which is generally included and used by CMakeLists.txt.
#
# This will only generate a list of include/source files, and will not preserve
# any other VS options (like compiler/linker settings). For those you must edit
# CMakeLists.txt manually. This script will however preserve VS filters (folders).

import os
import sys
import shutil
import xml.etree.ElementTree

def getLocalPath(path):
    remainingPath = path
    localPath = ''

    while True:
        split = os.path.split(remainingPath)
    
        remainingPath = split[0]
        tail = split[1]
        
        if localPath:
            localPath = tail + '/' + localPath
        else:
            localPath = tail;
        
        if tail == 'Source' or tail == 'Include':
            break

        if not remainingPath:
            localPath = os.path.split(path)[1]
            break

    return localPath

def getFilterVarName(prefix, filter):
    output = filter.replace('Source Files\\', 'SRC_')
    output = output.replace('Header Files\\', 'INC_')

    output = output.replace('Source Files', 'SRC_NOFILTER')
    output = output.replace('Header Files', 'INC_NOFILTER')

    output = output.replace('\\', '_')
    output = output.replace('/', '_')

    return prefix + output.upper()

def processEntries(tag, group, filters):
    for child in group.iter('{http://schemas.microsoft.com/developer/msbuild/2003}' + tag):
        filterElem = child.find('{http://schemas.microsoft.com/developer/msbuild/2003}Filter')

        if filterElem is not None:
            filterName = filterElem.text
            if filterName not in filters:
                filters[filterName] = []
                
            filters[filterName].append(getLocalPath(child.get('Include')))

# Process a .filter file and output a corresponding .txt file containing the necessary CMake commands
def processFile(file):
    tree = xml.etree.ElementTree.parse(file)
    root = tree.getroot()

    filters = {}

    for group in root.iter('{http://schemas.microsoft.com/developer/msbuild/2003}ItemGroup'):
        processEntries('ClInclude', group, filters)
        processEntries('ClCompile', group, filters)      

    outputName = os.path.splitext(file)[0]
    outputName = os.path.splitext(outputName)[0]
    outputName = os.path.split(outputName)[1]
    
    varPrefix = 'BS_' + outputName.upper() + '_'
    outputFolder = '..\\Source\\' + outputName + '\\CMakeSources.cmake'

    with open(outputFolder, 'w') as outputFile:
        # Output variables containing all the .h and .cpp files
        for key in filters.keys():
            outputFile.write('set(%s\n' % getFilterVarName(varPrefix, key))

            for entry in filters[key]:
                outputFile.write('\t\"%s\"\n' % entry)
            
            outputFile.write(')\n\n')

        # Output source groups (filters)
        for key in filters.keys():
            filter = key.replace('\\', '\\\\')
            outputFile.write('source_group(\"{0}\" FILES ${{{1}}})\n'.format(filter, getFilterVarName(varPrefix, key)))

        outputFile.write('\n')

        # Output a variable containing all variables from the previous step so we can use it for compiling
        outputFile.write('set(%sSRC\n' % varPrefix)
        for key in filters.keys():
            outputFile.write('\t${%s}\n' % getFilterVarName(varPrefix, key))

        outputFile.write(')')

# Go through all .filter files
def processAllFiles():
    solutionFolder = '..\\Build\\VS2015'

    for root, dirs, files in os.walk(solutionFolder):
        for file in files:
            if(file.lower().endswith('.filters')):
                filePath = os.path.join(root, file)
                processFile(filePath)

processAllFiles()
