# Source files and their filters
include(CMakeSources.cmake)

# Includes
set(BansheeOpenAudio_INC 
	"Include" 
	"../BansheeUtility/Include" 
	"../BansheeCore/Include"
	"../../Dependencies/BansheeOpenAudio/Include"
	"../../Dependencies/BansheeOpenAudio/Include/vorbis")

include_directories(${BansheeOpenAudio_INC})	
	
# Target
add_library(BansheeOpenAudio SHARED ${BS_BANSHEEOPENAUDIO_SRC})

# Defines
target_compile_definitions(BansheeOpenAudio PRIVATE -DBS_OA_EXPORTS)

# Libraries
## External libs: FLAC, Vorbis, Ogg, OpenAL
add_library_per_config(BansheeOpenAudio libFLAC)
add_library_per_config(BansheeOpenAudio libvorbis)
add_library_per_config(BansheeOpenAudio libvorbisfile)
add_library_per_config(BansheeOpenAudio libogg)
add_library_per_config(BansheeOpenAudio OpenAL32)

## Local libs
target_link_libraries(BansheeOpenAudio PUBLIC BansheeUtility BansheeCore)

# IDE specific
set_property(TARGET BansheeOpenAudio PROPERTY FOLDER Plugins)