# Find Snappy dependency
#
# This module defines
#  snappy_INCLUDE_DIRS
#  snappy_LIBRARIES
#  snappy_FOUND

set(snappy_INSTALL_DIR ${PROJECT_SOURCE_DIR}/../Dependencies/snappy CACHE PATH "")
set(snappy_INCLUDE_SEARCH_DIRS "${snappy_INSTALL_DIR}/include")

if(BS_64BIT)
	list(APPEND snappy_LIBRARY_RELEASE_SEARCH_DIRS "${snappy_INSTALL_DIR}/lib/x64/Release")
	list(APPEND snappy_LIBRARY_DEBUG_SEARCH_DIRS "${snappy_INSTALL_DIR}/lib/x64/Debug")
else()
	list(APPEND snappy_LIBRARY_RELEASE_SEARCH_DIRS "${snappy_INSTALL_DIR}/lib/x86/Release")
	list(APPEND snappy_LIBRARY_DEBUG_SEARCH_DIRS "${snappy_INSTALL_DIR}/lib/x86/Debug")
endif()

message(STATUS "Looking for Snappy installation...")
	
find_path(snappy_INCLUDE_DIR snappy.h PATHS ${snappy_INCLUDE_SEARCH_DIRS})	
	
if(snappy_INCLUDE_DIR)
	set(snappy_FOUND TRUE)
else()
	set(snappy_FOUND FALSE)
endif()	
	
find_imported_library(snappy snappy)

if(NOT snappy_FOUND)
	if(snappy_FIND_REQUIRED)
		message(FATAL_ERROR "Cannot find snappy installation. Try modifying the snappy_INSTALL_DIR path.")
	elseif(NOT snappy_FIND_QUIETLY)
		message(WARNING "Cannot find snappy installation. Try modifying the snappy_INSTALL_DIR path.")
	endif()
else()
	set_target_properties(snappy::snappy PROPERTIES INTERFACE_INCLUDE_DIRECTORIES "${snappy_INCLUDE_DIR}")
	message(STATUS "...snappy OK.")
endif()

mark_as_advanced(snappy_INSTALL_DIR snappy_INCLUDE_DIR)

set(snappy_INCLUDE_DIRS ${snappy_INCLUDE_DIR})