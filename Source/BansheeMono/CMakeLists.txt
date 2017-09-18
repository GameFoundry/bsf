# Source files and their filters
include(CMakeSources.cmake)

# Find packages
find_package(mono)

# Includes
set(BansheeMono_INC 
	"./" 
	"../BansheeUtility" 
	"../BansheeCore"
	"../BansheeEngine")

include_directories(${BansheeMono_INC})	

# Target
add_library(BansheeMono SHARED ${BS_BANSHEEMONO_SRC})

# Defines
target_compile_definitions(BansheeMono PRIVATE -DBS_MONO_EXPORTS)

# Libraries
## External lib: Mono
target_link_libraries(BansheeMono PRIVATE ${mono_LIBRARIES})

## Local libs
target_link_libraries(BansheeMono PUBLIC BansheeEngine BansheeUtility BansheeCore)

# IDE specific
set_property(TARGET BansheeMono PROPERTY FOLDER Plugins)