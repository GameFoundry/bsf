cmake_minimum_required (VERSION 3.12.0)
project (bsf)

set (BSF_SOURCE_DIR ${PROJECT_SOURCE_DIR}/Source)
set (APP_ROOT_DIR ${PROJECT_SOURCE_DIR})
set (BS_IS_BANSHEE3D 0)

# Default install dir
if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
	if(LINUX)
		set(CMAKE_INSTALL_PREFIX /usr/local/opt/bsf CACHE PATH "..." FORCE)
	endif()
endif()

include(Source/CMake/Properties.cmake)
include(Source/CMake/FindPackageOrBuild.cmake)
include(Source/CMake/HelperMethods.cmake)


set(BUILD_TESTS OFF CACHE BOOL "If true, build targets for running unit tests will be included in the output.")
if (BUILD_TESTS) 
	# Enables testing for this directory and below. See also the add_test
	# command. Note that ctest expects to find a test file in the build
	# directory root. Therefore, this command should be in the source
	# directory root.
	enable_testing()
endif()

add_subdirectory(Source)

# Options
set(SCRIPT_API "None" CACHE STRING "Which scripting API/language to use, if any. If no scripting API is chosen only native C++ bsf core will be built. If a scripting API is chosen, then a separate scripting layer on top of the core will be included in the build as well.")
set_property(CACHE SCRIPT_API PROPERTY STRINGS "None" "C#")

set(SCRIPT_BINDING_GENERATION OFF CACHE BOOL "If true, script binding generation will be supported through a specialized build target. Enable this if you plan on modifying the scripting API. Requires the SBGen tool dependency. Only relevant if you have selected a SCRIPT_API other than \"None\".")
