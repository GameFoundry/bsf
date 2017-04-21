# Find NVTT dependency
#
# This module defines
#  NVTT_INCLUDE_DIRS
#  NVTT_LIBRARIES
#  NVTT_FOUND

set(NVTT_INSTALL_DIR ${PROJECT_SOURCE_DIR}/../Dependencies/NVTT CACHE PATH "")
set(NVTT_INCLUDE_SEARCH_DIRS "${NVTT_INSTALL_DIR}/include")

if(BS_64BIT)
	list(APPEND NVTT_LIBRARY_RELEASE_SEARCH_DIRS "${NVTT_INSTALL_DIR}/lib/x64/Release")
	list(APPEND NVTT_LIBRARY_DEBUG_SEARCH_DIRS "${NVTT_INSTALL_DIR}/lib/x64/Debug")
else()
	list(APPEND NVTT_LIBRARY_RELEASE_SEARCH_DIRS "${NVTT_INSTALL_DIR}/lib/x86/Release")
	list(APPEND NVTT_LIBRARY_DEBUG_SEARCH_DIRS "${NVTT_INSTALL_DIR}/lib/x86/Debug")
endif()

message(STATUS "Looking for NVTT installation...")
	
find_path(NVTT_INCLUDE_DIR nvtt.h PATHS ${NVTT_INCLUDE_SEARCH_DIRS})	
	
if(NVTT_INCLUDE_DIR)
	set(NVTT_FOUND TRUE)
else()
	set(NVTT_FOUND FALSE)
endif()	
	
find_imported_library(NVTT nvtt)
find_imported_library(NVTT bc6h)
find_imported_library(NVTT bc7)
find_imported_library(NVTT nvcore)
find_imported_library(NVTT nvimage)
find_imported_library(NVTT nvmath)
find_imported_library(NVTT nvthread)
find_imported_library(NVTT squish)

if(NOT NVTT_FOUND)
	if(NVTT_FIND_REQUIRED)
		message(FATAL_ERROR "Cannot find NVTT installation. Try modifying the NVTT_INSTALL_DIR path.")
	elseif(NOT NVTT_FIND_QUIETLY)
		message(WARNING "Cannot find NVTT installation. Try modifying the NVTT_INSTALL_DIR path.")
	endif()
else()
	message(STATUS "...NVTT OK.")
endif()

mark_as_advanced(NVTT_INSTALL_DIR NVTT_INCLUDE_DIR)

set(NVTT_INCLUDE_DIRS ${NVTT_INCLUDE_DIR})
set(NVTT_LIBRARIES nvtt bc6h bc7 nvcore nvimage nvmath nvthread squish)