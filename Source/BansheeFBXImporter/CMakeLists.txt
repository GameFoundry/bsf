# Source files and their filters
include(CMakeSources.cmake)

# Includes
set(BansheeFBXImporter_INC 
	"Include" 
	"Dependencies/Include"
	"../BansheeUtility/Include" 
	"../BansheeCore/Include"
	"../../Dependencies/Include")

include_directories(${BansheeFBXImporter_INC})	
	
# Target
add_library(BansheeFBXImporter SHARED ${BS_BANSHEEFBXIMPORTER_SRC})

# Defines
target_compile_definitions(BansheeFBXImporter PRIVATE -DBS_FBX_EXPORTS)

# Libraries
if(BS_64BIT)
	set(BS_LOCAL_LIB_PATH ${CMAKE_CURRENT_SOURCE_DIR}/Dependencies/lib/x64)
else()
	set(BS_LOCAL_LIB_PATH ${CMAKE_CURRENT_SOURCE_DIR}/Dependencies/lib/x86)
endif()

## External lib: FBX
add_library(FBX STATIC IMPORTED)
set_target_properties(FBX PROPERTIES IMPORTED_LOCATION_DEBUG ${BS_LOCAL_LIB_PATH}/Debug/libfbxsdk-md${BS_LIBRARY_EXTENSION})
set_target_properties(FBX PROPERTIES IMPORTED_LOCATION_OPTIMIZEDDEBUG ${BS_LOCAL_LIB_PATH}/OptimizedDebug/libfbxsdk-md${BS_LIBRARY_EXTENSION})
set_target_properties(FBX PROPERTIES IMPORTED_LOCATION_RELEASE ${BS_LOCAL_LIB_PATH}/Release/libfbxsdk-md${BS_LIBRARY_EXTENSION})	
	
target_link_libraries(BansheeFBXImporter FBX)	

## Local libs
target_link_libraries(BansheeFBXImporter BansheeUtility BansheeCore)

# IDE specific
set_property(TARGET BansheeFBXImporter PROPERTY FOLDER Plugins)