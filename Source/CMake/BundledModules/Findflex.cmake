# Find flex installation
#
# This module defines
#  flex_EXECUTABLE
#  flex_FOUND

set(flex_INSTALL_DIRS ${BSF_SOURCE_DIR}/../Dependencies/tools/flex CACHE PATH "")

message(STATUS "Looking for flex installation...")

find_program(flex_EXECUTABLE NAMES flex bin/flex PATHS ${flex_INSTALL_DIRS} NO_DEFAULT_PATH)
find_program(flex_EXECUTABLE NAMES flex bin/flex PATHS ${flex_INSTALL_DIRS})

if(flex_EXECUTABLE)
	set(flex_FOUND TRUE)
else()
	set(flex_FOUND FALSE)
endif()

if(NOT flex_FOUND)
	if(flex_FIND_REQUIRED)
		message(FATAL_ERROR "Cannot find flex installation. Try modifying the flex_INSTALL_DIRS path.")
		return()
	else()
		message(WARNING "Cannot find flex installation. Try modifying the flex_INSTALL_DIRS path.")
	endif()
else()
	message(STATUS "...flex OK.")
endif()

mark_as_advanced(flex_INSTALL_DIRS flex_EXECUTABLE)
