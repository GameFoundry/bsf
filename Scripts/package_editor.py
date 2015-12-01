#!/usr/bin/python

# Packages the editor by copying all relevant files to the Builds directory.
# This will not execute the editor build, and is implied the editor has been
# been built before executing this script.

# Usage: "package_editor $Configuration"
# Where: $Configuration - e.g. Debug, DebugRelease

import os
import sys
import shutil

configuration = 'DebugRelease' #sys.argv[1]
dataFoldersToIgnore = ['Examples', 'Raw']

dataFolder = 'Data'
assembliesFolder = 'Assemblies'
monoFolder = 'Mono'
libFolder = 'x64\\' + configuration + '\\'

inputDataFolder = '..\\' + dataFolder
inputBinFolder = '..\\bin\\'
inputAssembliesFolder = inputBinFolder + assembliesFolder
inputMonoFolder = inputBinFolder + monoFolder
inputLibFolder = inputBinFolder + libFolder

outputBaseFolder = '..\\Builds\Banshee Editor\\'
outputDataFolder = outputBaseFolder + dataFolder
outputBinFolder = outputBaseFolder + '\\bin\\'
outputAssembliesFolder = outputBinFolder + assembliesFolder
outputMonoFolder = outputBinFolder + monoFolder
outputLibFolder = outputBinFolder + libFolder

def ignore_data(path, entries):
    if path != inputDataFolder:
        return []

    return list(set(dataFoldersToIgnore) & set(entries))    

if os.path.exists(outputBaseFolder):
    shutil.rmtree(outputBaseFolder)

os.makedirs(outputBaseFolder)
shutil.copytree(inputDataFolder, outputDataFolder, False, ignore_data)
shutil.copytree(inputAssembliesFolder, outputAssembliesFolder)
shutil.copytree(inputMonoFolder, outputMonoFolder)
shutil.copytree(inputLibFolder, outputLibFolder)
