# Find Mono compiler (mcs) & build system installation
#
# This module defines
#  mcs_EXECUTABLE
#  xbuild_EXECUTABLE
#  mcs_FOUND

message(STATUS "Looking for Mono compiler (mcs) and xbuild installation...")

if(APPLE)
	set(MONO_SEARCH_PATH /Library/Frameworks/Mono.framework/Versions/Current/Commands/)
endif()

find_program(mcs_EXECUTABLE NAMES mcs HINTS ${MONO_SEARCH_PATH})
find_program(xbuild_EXECUTABLE NAMES xbuild HINTS ${MONO_SEARCH_PATH})

if(mcs_EXECUTABLE AND xbuild_EXECUTABLE)
	set(mcs_FOUND TRUE)
else()
	set(mcs_FOUND FALSE)
endif()

if(NOT mcs_FOUND)
	if(mcs_FIND_REQUIRED)
		message(FATAL_ERROR "Cannot find mcs and/or xbuild installation. Make sure Mono runtime and xbuild are installed.")
		return()
	else()
		message(WARNING "Cannot find mcs and/or xbuild installation. Make sure Mono runtime and xbuild are installed.")
	endif()
else()
	message(STATUS "...mcs and xbuild OK.")
endif()

mark_as_advanced(mcs_EXECUTABLE xbuild_EXECUTABLE)
