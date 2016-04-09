# Source files and their filters
include(CMakeSources.cmake)

# Includes
set(BansheeFontImporter_INC 
	"Include" 
	"Dependencies/Include"
	"../BansheeUtility/Include" 
	"../BansheeCore/Include"
	"../../Dependencies/Include")

include_directories(${BansheeFontImporter_INC})	
	
# Target
add_library(BansheeFontImporter SHARED ${BS_BANSHEEFONTIMPORTER_SRC})

# Defines
target_compile_definitions(BansheeFontImporter PRIVATE -DBS_FONT_EXPORTS -DUSE_FREETYPE2_STATIC)

# Libraries
if(BS_64BIT)
	set(BS_LOCAL_LIB_PATH ${CMAKE_CURRENT_SOURCE_DIR}/Dependencies/lib/x64)
else()
	set(BS_LOCAL_LIB_PATH ${CMAKE_CURRENT_SOURCE_DIR}/Dependencies/lib/x86)
endif()

## External lib: FreeType
add_library(FreeType STATIC IMPORTED)
set_target_properties(FreeType PROPERTIES IMPORTED_LOCATION_DEBUG ${BS_LOCAL_LIB_PATH}/Debug/freetype${BS_LIBRARY_EXTENSION})
set_target_properties(FreeType PROPERTIES IMPORTED_LOCATION_OPTIMIZEDDEBUG ${BS_LOCAL_LIB_PATH}/OptimizedDebug/freetype${BS_LIBRARY_EXTENSION})
set_target_properties(FreeType PROPERTIES IMPORTED_LOCATION_RELEASE ${BS_LOCAL_LIB_PATH}/Release/freetype${BS_LIBRARY_EXTENSION})	
	
target_link_libraries(BansheeFontImporter FreeType)	

## Local libs
target_link_libraries(BansheeFontImporter BansheeUtility BansheeCore)

# IDE specific
set_property(TARGET BansheeFontImporter PROPERTY FOLDER Plugins)