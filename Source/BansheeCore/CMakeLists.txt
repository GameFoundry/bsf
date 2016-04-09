# Source files and their filters
include(CMakeSources.cmake)

# Includes
set(BansheeCore_INC 
	"Include" 
	"../BansheeUtility/Include" 
	"../../Dependencies/Include")

if(WIN32)
	set(BansheeCore_INC ${BansheeCore_INC} "Include/Win32")
endif()

include_directories(${BansheeCore_INC})	
	
# Target
add_library(BansheeCore SHARED ${BS_BANSHEECORE_SRC})

# Defines
target_compile_definitions(BansheeCore PRIVATE -DBS_CORE_EXPORTS)

# Libraries
## External lib: NVTT
add_library(nvtt STATIC IMPORTED)
set_target_properties(nvtt PROPERTIES IMPORTED_LOCATION_DEBUG ${BS_LIBRARY_DEPENDENCIES_DIR_DEBUG}/nvtt${BS_LIBRARY_EXTENSION})
set_target_properties(nvtt PROPERTIES IMPORTED_LOCATION_OPTIMIZEDDEBUG ${BS_LIBRARY_DEPENDENCIES_DIR_OPTIMIZEDDEBUG}/nvtt${BS_LIBRARY_EXTENSION})
set_target_properties(nvtt PROPERTIES IMPORTED_LOCATION_RELEASE ${BS_LIBRARY_DEPENDENCIES_DIR_RELEASE}/nvtt${BS_LIBRARY_EXTENSION})	
	
target_link_libraries(BansheeCore nvtt)	
	
## Local libs
target_link_libraries(BansheeCore BansheeUtility)	
	
## OS libs
if(WIN32)
target_link_libraries(BansheeCore Winmm)
else()
	# TODO_OTHER_PLATFORMS_GO_HERE
endif()

# IDE specific
set_property(TARGET BansheeCore PROPERTY FOLDER Layers)