#!/usr/bin/python

# Builds and packages the editor. Make sure you have MSBuild
# installed and the path is valid.

# Usage: "build_editor_win_x64 $Configuration"
# Where: $Configuration - e.g. OptimizedDebug, Release

import os
import multiprocessing

msbuildPath = "C:\\Program Files (x86)\\MSBuild\\14.0\\Bin\\amd64"
buildPath = "..\\Build\\VS2015\\"

def buildCSWorker(args):
    projectPath = buildPath + "\\" + args[0] + "\\" + args[0] + ".csproj"
    os.system("msbuild {0} /t:ClCompile /p:Configuration={1};Platform=x64 /m".format(projectPath, args[1]))
    return

if __name__ == '__main__':
    if not os.path.exists(msbuildPath):
        print("MSBuild path is not valid. Used path {0}: ".format(msbuildPath))
        exit;

    os.environ["PATH"] += os.pathsep + msbuildPath

    # Build native projects
    os.system("fast_build.py OptimizedDebug x64")
    os.system("fast_build.py Release x64")

    # Build managed projects
    csProjectArgs = [("MBansheeEngine", "Debug"), ("MBansheeEngine", "Release"),
                     ("MBansheeEditor", "Debug"), ("MBansheeEditor", "Release")]

    pool = multiprocessing.Pool(multiprocessing.cpu_count())
    pool.map(buildCSWorker, csProjectArgs)
        
    os.system("package_editor.py " + configuration)
