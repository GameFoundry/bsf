# Source files and their filters
include(CMakeSources.cmake)

# Includes
set(BansheeOISInput_INC 
	"Include"
	"Dependencies/Include"
	"../BansheeUtility/Include" 
	"../BansheeCore/Include"
	"../../Dependencies/Include")

include_directories(${BansheeOISInput_INC})	
	
# Target
add_library(BansheeOISInput SHARED ${BS_BANSHEEOISINPUT_SRC})

# Defines
target_compile_definitions(BansheeOISInput PRIVATE -DBS_OIS_EXPORTS -DOIS_DYNAMIC_LIB -DOIS_NONCLIENT_BUILD)

# Libraries
if(BS_64BIT)
	set(BS_LOCAL_LIB_PATH ${CMAKE_CURRENT_SOURCE_DIR}/Dependencies/lib/x64)
else()
	set(BS_LOCAL_LIB_PATH ${CMAKE_CURRENT_SOURCE_DIR}/Dependencies/lib/x86)
endif()

## External lib: OIS
add_library(OIS STATIC IMPORTED)
set_target_properties(OIS PROPERTIES IMPORTED_LOCATION_DEBUG ${BS_LOCAL_LIB_PATH}/Debug/BansheeOIS${BS_LIBRARY_EXTENSION})
set_target_properties(OIS PROPERTIES IMPORTED_LOCATION_OPTIMIZEDDEBUG ${BS_LOCAL_LIB_PATH}/OptimizedDebug/BansheeOIS${BS_LIBRARY_EXTENSION})
set_target_properties(OIS PROPERTIES IMPORTED_LOCATION_RELEASE ${BS_LOCAL_LIB_PATH}/Release/BansheeOIS${BS_LIBRARY_EXTENSION})	
	
target_link_libraries(BansheeOISInput OIS)	

## Local libs
target_link_libraries(BansheeOISInput BansheeUtility BansheeCore)

# IDE specific
set_property(TARGET BansheeOISInput PROPERTY FOLDER Plugins)