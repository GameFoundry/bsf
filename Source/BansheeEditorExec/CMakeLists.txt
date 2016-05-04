# Source files and their filters
include(CMakeSources.cmake)

# Includes
set(BansheeEditorExec_INC 
	"Include"
	"../BansheeUtility/Include" 
	"../BansheeCore/Include"
	"../BansheeEngine/Include"
	"../BansheeEditor/Include")

include_directories(${BansheeEditorExec_INC})	
	
# Target
add_library(BansheeEditorExec SHARED ${BS_BANSHEEEDITOREXEC_SRC})

# Libraries
## Local libs
target_link_libraries(BansheeEditorExec BansheeEditor BansheeEngine BansheeUtility BansheeCore)

# IDE specific
set_property(TARGET BansheeEditorExec PROPERTY FOLDER Executable)

# Plugin dependencies
add_dependencies(BansheeEditorExec BansheeFBXImporter BansheeFontImporter BansheeFreeImgImporter BansheeGLRenderAPI BansheeMono BansheeOISInput BansheePhysX BansheeSL RenderBeast SBansheeEngine SBansheeEditor Game)

if(WIN32)
	add_dependencies(BansheeEditorExec BansheeD3D11RenderAPI BansheeD3D9RenderAPI)
endif()

# Compiler flags
target_compile_options(BansheeEditorExec PUBLIC /wd4509)