# Find Mono compiler (mcs) & build system installation
#
# This module defines
#  mcs_EXECUTABLE
#  msbuild_EXECUTABLE
#  mcs_FOUND

message(STATUS "Looking for Mono compiler (mcs) and msbuild installation...")

if(APPLE)
	set(MONO_SEARCH_PATH /Library/Frameworks/Mono.framework/Versions/Current/Commands/)
endif()

find_program(mcs_EXECUTABLE NAMES mcs HINTS ${MONO_SEARCH_PATH})
find_program(msbuild_EXECUTABLE NAMES msbuild HINTS ${MONO_SEARCH_PATH})

if(mcs_EXECUTABLE AND msbuild_EXECUTABLE)
	set(mcs_FOUND TRUE)
else()
	set(mcs_FOUND FALSE)
endif()

if(NOT mcs_FOUND)
	if(mcs_FIND_REQUIRED)
		message(FATAL_ERROR "Cannot find mcs and/or msbuild installation. Make sure Mono runtime and msbuild are installed.")
		return()
	else()
		message(WARNING "Cannot find mcs and/or msbuild installation. Make sure Mono runtime and msbuild are installed.")
	endif()
else()
	message(STATUS "...mcs and msbuild OK.")
endif()

mark_as_advanced(mcs_EXECUTABLE msbuild_EXECUTABLE)
