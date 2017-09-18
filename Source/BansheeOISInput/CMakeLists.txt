# Source files and their filters
include(CMakeSources.cmake)

# Find packages
find_package(OIS)

# Includes
set(BansheeOISInput_INC 
	"./"
	"../BansheeUtility" 
	"../BansheeCore")

include_directories(${BansheeOISInput_INC})	
	
# Target
add_library(BansheeOISInput SHARED ${BS_BANSHEEOISINPUT_SRC})

# Defines
target_compile_definitions(BansheeOISInput PRIVATE -DBS_OIS_EXPORTS -DOIS_DYNAMIC_LIB -DOIS_NONCLIENT_BUILD)

# Libraries
## External lib: OIS
target_link_libraries(BansheeOISInput PRIVATE ${OIS_LIBRARIES})

## Local libs
target_link_libraries(BansheeOISInput PRIVATE BansheeUtility BansheeCore)

# IDE specific
set_property(TARGET BansheeOISInput PROPERTY FOLDER Plugins)