#!/usr/bin/python

# Builds and packages the editor. Make sure you have MSBuild
# installed and the path is valid.

# Usage: "fastBuild $Configuration $Platform"
# Where: $Configuration - e.g. Debug, OptimizedDebug, Release
# Where: $Platform - x86, x64

import sys
import os
import multiprocessing

msbuildPath = "C:\\Program Files (x86)\\MSBuild\\14.0\\Bin\\amd64"
buildPath = "..\\Build\\VS2015\\"
configuration = sys.argv[1]
platform = sys.argv[2]

def compileWorker(project):
    projectPath = buildPath + "\\" + project + "\\" + project + ".vcxproj"
    os.system("msbuild {0} /t:ClCompile /p:Configuration={1};Platform={2}".format(projectPath, configuration, platform))
    return

if __name__ == '__main__':
    solutionPath = buildPath + "Banshee.sln"
    projects = ["BansheeUtility", "BansheeCore", "BansheeEngine", "BansheeMono",
                "BansheeEditor", "SBansheeEngine", "BansheeFBXImporter", "BansheeFontImporter",
                "BansheeFreeImgImporter", "BansheeD3D11RenderAPI", "BansheeOISInput", "BansheeOpenAudio",
                "BansheePhysX", "BansheeSL", "RenderBeast", "SBansheeEditor", "BansheeGLRenderAPI",
                "BansheeVulkanRenderAPI", "BansheeEditorExec", "Game"]

    if not os.path.exists(msbuildPath):
        print("MSBuild path is not valid. Used path {0}: ".format(msbuildPath))
        exit;

    os.environ["PATH"] += os.pathsep + msbuildPath

    # Clean entire solution
    os.system("msbuild {0} /t:Clean /p:Configuration={1};Platform={2} /m".format(solutionPath, configuration, platform))

    # Launch a compile thread for every project
    pool = multiprocessing.Pool(multiprocessing.cpu_count())
    pool.map(compileWorker, projects)

    # Link all projects
    os.system("msbuild {0} /p:Configuration={1};Platform={2} /m".format(solutionPath, configuration, platform))

    #for project in projects:
    #    projectPath = buildPath + "\\" + project + "\\" + project + ".vcxproj"
    #    os.system("msbuild {0} /t:Link /p:Configuration={1};Platform={2} /m".format(projectPath, configuration, platform))
