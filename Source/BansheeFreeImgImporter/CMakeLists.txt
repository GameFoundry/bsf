# Source files and their filters
include(CMakeSources.cmake)

# Includes
set(BansheeFreeImgImporter_INC 
	"Include" 
	"Dependencies/Include"
	"../BansheeUtility/Include" 
	"../BansheeCore/Include"
	"../../Dependencies/Include")

include_directories(${BansheeFreeImgImporter_INC})	
	
# Target
add_library(BansheeFreeImgImporter SHARED ${BS_BANSHEEFREEIMGIMPORTER_SRC})

# Defines
target_compile_definitions(BansheeFreeImgImporter PRIVATE -DBS_FREEIMG_EXPORTS -D_MBCS -DFREEIMAGE_LIB)

# Libraries
if(BS_64BIT)
	set(BS_LOCAL_LIB_PATH ${CMAKE_CURRENT_SOURCE_DIR}/Dependencies/lib/x64)
else()
	set(BS_LOCAL_LIB_PATH ${CMAKE_CURRENT_SOURCE_DIR}/Dependencies/lib/x86)
endif()

## External lib: FreeImage
add_library(FreeImage STATIC IMPORTED)
set_target_properties(FreeImage PROPERTIES IMPORTED_LOCATION_DEBUG ${BS_LOCAL_LIB_PATH}/Debug/FreeImage${BS_LIBRARY_EXTENSION})
set_target_properties(FreeImage PROPERTIES IMPORTED_LOCATION_OPTIMIZEDDEBUG ${BS_LOCAL_LIB_PATH}/OptimizedDebug/FreeImage${BS_LIBRARY_EXTENSION})
set_target_properties(FreeImage PROPERTIES IMPORTED_LOCATION_RELEASE ${BS_LOCAL_LIB_PATH}/Release/FreeImage${BS_LIBRARY_EXTENSION})	
	
target_link_libraries(BansheeFreeImgImporter FreeImage)	

## Local libs
target_link_libraries(BansheeFreeImgImporter BansheeUtility BansheeCore)

# IDE specific
set_property(TARGET BansheeFreeImgImporter PROPERTY FOLDER Plugins)