# Source files and their filters
include(CMakeSources.cmake)

# Includes
set(BansheeFMOD_INC 
	"Include" 
	"../BansheeUtility/Include" 
	"../BansheeCore/Include")

include_directories(${BansheeFMOD_INC})	
	
# Target
add_library(BansheeFMOD SHARED ${BS_BANSHEEFMOD_SRC})

# Defines
target_compile_definitions(BansheeFMOD PRIVATE -DBS_FMOD_EXPORTS)

# Options
set(FMOD_PATH "C:\\Program Files (x86)\\FMOD SoundSystem\\FMOD Studio API Windows" CACHE PATH "Path to the FMOD installation directory.")
mark_as_advanced(FMOD_PATH)

# Libraries
## External libs: FMOD
# TODO

## Local libs
target_link_libraries(BansheeFMOD PUBLIC BansheeUtility BansheeCore)

# IDE specific
set_property(TARGET BansheeFMOD PROPERTY FOLDER Plugins)