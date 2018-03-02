# Source files and their filters
include(CMakeSources.cmake)

# Find packages
if(LINUX)
	find_package(PkgConfig REQUIRED)
	pkg_check_modules(GTK3 REQUIRED gtk+-3.0)
endif()

# Includes
set(BansheeEditor_INC 
	"./" 
	"../BansheeUtility" 
	"../BansheeCore"
	"../BansheeEngine")

include_directories(${BansheeEditor_INC})

# Target
add_library(BansheeEditor SHARED ${BS_BANSHEEEDITOR_SRC})

# Defines
target_compile_definitions(BansheeEditor PRIVATE -DBS_ED_EXPORTS)

# Libraries
## Local libs
target_link_libraries(BansheeEditor PUBLIC BansheeUtility BansheeCore BansheeEngine)
	
if(WIN32)
	## OS libs
	target_link_libraries(BansheeEditor PRIVATE Advapi32)
elseif(LINUX)
	## External lib: GTK+ 3.0
	include_directories(${GTK3_INCLUDE_DIRS})
	target_link_libraries(BansheeEditor PRIVATE ${GTK3_LIBRARIES})
elseif(APPLE) # MacOS
	target_link_framework(BansheeEditor AppKit)
endif()

# IDE specific
set_property(TARGET BansheeEditor PROPERTY FOLDER Layers)