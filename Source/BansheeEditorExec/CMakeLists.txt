# Source files and their filters
include(CMakeSources.cmake)

# Includes
set(BansheeEditorExec_INC 
	"./"
	"../BansheeUtility" 
	"../BansheeCore"
	"../BansheeEngine"
	"../BansheeEditor")

include_directories(${BansheeEditorExec_INC})	
	
# Target
if(WIN32)
	add_executable(BansheeEditorExec WIN32 ${BS_BANSHEEEDITOREXEC_SRC})
else()
	add_executable(BansheeEditorExec ${BS_BANSHEEEDITOREXEC_SRC})
endif()

# Libraries
## Local libs
target_link_libraries(BansheeEditorExec BansheeEditor BansheeEngine BansheeUtility BansheeCore)

# IDE specific
set_property(TARGET BansheeEditorExec PROPERTY FOLDER Executable)

# Plugin dependencies
add_engine_dependencies(BansheeEditorExec)
add_dependencies(BansheeEditorExec BansheeFBXImporter BansheeFontImporter BansheeFreeImgImporter SBansheeEditor MonoExec)

# Compiler flags
if(MSVC)
	target_compile_options(BansheeEditorExec PUBLIC /wd4509)
endif()