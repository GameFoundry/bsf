#!/usr/bin/python

# Builds all projects in the provided solution in a parallel way.

# Usage: "fast_build $Solution $Configuration $Platform"
# Where: $Solution - Path to a .sln file, e.g. Banshee.sln
# Where: $Configuration - e.g. Debug, OptimizedDebug, Release
# Where: $Platform - x86, x64

# Usage example: fast_build Banshee.sln Debug x64

# msbuild must be available in the PATH variable, or the script must be ran from VS or VS Command Prompt

import sys
import os
import multiprocessing
import re

slnPath = sys.argv[1]
configuration = sys.argv[2]
platform = sys.argv[3]
slnDir = os.path.dirname(slnPath)

# Triggers compile for a single project
def compileWorker(projectPath):
    if not os.path.isabs(projectPath):
        projectPath = os.path.join(slnDir, projectPath)
        
    os.system("msbuild {0} /t:ClCompile /p:Configuration={1};Platform={2}".format(projectPath, configuration, platform))
    return

# Since we'll be running this script using multiple processes, ensure this bit only runs on the main process
if __name__ == '__main__':

    # Parse the solution file to find all C++ projects
    slnContents = open(slnPath, 'r').read()
    projects = re.findall(r"Project\(\".*\"\) = \".*\", \"(.*\.vcxproj)\", \".*\"\n", slnContents);

    # Launch a compile thread for every project
    pool = multiprocessing.Pool(multiprocessing.cpu_count())
    pool.map(compileWorker, projects)
