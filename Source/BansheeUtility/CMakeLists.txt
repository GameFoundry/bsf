# Source files and their filters
include(CMakeSources.cmake)

# Packages
find_package(snappy REQUIRED)

# Includes
set(BansheeUtility_INC 
	"./" 
	"ThirdParty")

if(WIN32)
	set(BansheeUtility_INC ${BansheeUtility_INC} "Win32")
else()
	# TODO_OTHER_PLATFORMS_GO_HERE
endif()

include_directories(${BansheeUtility_INC})	

# Target
add_library(BansheeUtility SHARED ${BS_BANSHEEUTILITY_SRC})

add_executable(BansheeUtilityTest BsUtilityTest.cpp)
target_link_libraries(BansheeUtilityTest BansheeUtility)

# Defines
target_compile_definitions(BansheeUtility PRIVATE -DBS_UTILITY_EXPORTS)

# Libraries
## External lib: Snappy
target_link_libraries(BansheeUtility PRIVATE ${snappy_LIBRARIES})	

## OS libs
if(WIN32)
target_link_libraries(BansheeUtility PRIVATE
	DbgHelp
	IPHLPAPI
	Rpcrt4
	)
else()
	target_link_libraries(BansheeUtility PRIVATE dl pthread)
endif()

# IDE specific
set_property(TARGET BansheeUtility PROPERTY FOLDER Layers)
