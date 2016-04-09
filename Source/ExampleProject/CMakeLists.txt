# Source files and their filters
include(CMakeSources.cmake)

# Includes
set(ExampleProject_INC 
	"Include"
	"../BansheeUtility/Include" 
	"../BansheeCore/Include"
	"../BansheeEngine/Include"
	"../../Dependencies/Include")

include_directories(${ExampleProject_INC})	
	
# Target
add_library(ExampleProject SHARED ${BS_EXAMPLEPROJECT_SRC})

# Libraries
## Local libs
target_link_libraries(ExampleProject BansheeEngine BansheeUtility BansheeCore)

# IDE specific
set_property(TARGET ExampleProject PROPERTY FOLDER Executable)

# Plugin dependencies
add_dependencies(Game BansheeFBXImporter BansheeFontImporter BansheeFreeImgImporter BansheeGLRenderAPI BansheeMono BansheeOISInput BansheePhysX BansheeSL RenderBeast SBansheeEngine)

if(WIN32)
	add_dependencies(Game BansheeD3D11RenderAPI BansheeD3D9RenderAPI)
endif()