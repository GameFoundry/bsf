cmake_minimum_required (VERSION 3.5.1)
project (Banshee)

# Version
set (Banshee_VERSION_MAJOR 0)
set (Banshee_VERSION_MINOR 3)

# Output
set(CMAKE_BINARY_DIR ../${PROJECT_SOURCE_DIR}/bin)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})
set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR})

# Sub-directories
## Layers
add_subdirectory(BansheeUtility)
#add_subdirectory(BansheeCore)
#add_subdirectory(BansheeEngine)
#add_subdirectory(BansheeEditor)

## Plugins
#add_subdirectory(BansheeD3D9RenderAPI)
#add_subdirectory(BansheeD3D11RenderAPI)
#add_subdirectory(BansheeGLRenderAPI)
#add_subdirectory(BansheeFBXImporter)
#add_subdirectory(BansheeFontImporter)
#add_subdirectory(BansheeFreeImgImporter)
#add_subdirectory(BansheeMono)
#add_subdirectory(BansheeOISInput)
#add_subdirectory(BansheePhysX)
#add_subdirectory(BansheeSL)
#add_subdirectory(RenderBeast)

## Script interop
#add_subdirectory(SBansheeEngine)
#add_subdirectory(SBansheeEditor)

## Executables
#add_subdirectory(BansheeEditorExec)
#add_subdirectory(Game)
#add_subdirectory(ExampleProject)