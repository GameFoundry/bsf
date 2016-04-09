# Source files and their filters
include(CMakeSources.cmake)

# Includes
set(BansheeMono_INC 
	"Include" 
	"../BansheeUtility/Include" 
	"../BansheeCore/Include"
	"../BansheeEngine/Include"
	"../../Dependencies/Include")

include_directories(${BansheeMono_INC})	
	
# Target
add_library(BansheeMono SHARED ${BS_BANSHEEMONO_SRC})

# Defines
target_compile_definitions(BansheeMono PRIVATE -DBS_MONO_EXPORTS)

# Libraries
if(BS_64BIT)
	set(BS_LOCAL_LIB_PATH ${CMAKE_CURRENT_SOURCE_DIR}/Dependencies/lib/x64)
else()
	set(BS_LOCAL_LIB_PATH ${CMAKE_CURRENT_SOURCE_DIR}/Dependencies/lib/x86)
endif()

## External lib: Mono
add_library(Mono STATIC IMPORTED)
set_target_properties(Mono PROPERTIES IMPORTED_LOCATION_DEBUG ${BS_LIBRARY_DEPENDENCIES_DIR_DEBUG}/mono-2.0${BS_LIBRARY_EXTENSION})
set_target_properties(Mono PROPERTIES IMPORTED_LOCATION_OPTIMIZEDDEBUG ${BS_LIBRARY_DEPENDENCIES_DIR_OPTIMIZEDEBUG}/mono-2.0${BS_LIBRARY_EXTENSION})
set_target_properties(Mono PROPERTIES IMPORTED_LOCATION_RELEASE ${BS_LIBRARY_DEPENDENCIES_DIR_RELEASE}/mono-2.0${BS_LIBRARY_EXTENSION})	
	
target_link_libraries(BansheeMono Mono)	

## Local libs
target_link_libraries(BansheeMono BansheeEngine BansheeUtility BansheeCore)

# IDE specific
set_property(TARGET BansheeMono PROPERTY FOLDER Plugins)