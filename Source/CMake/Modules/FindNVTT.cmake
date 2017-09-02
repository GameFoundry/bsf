# Find nvtt dependency
#
# This module defines
#  nvtt_INCLUDE_DIRS
#  nvtt_LIBRARIES
#  nvtt_FOUND

set(nvtt_INSTALL_DIR ${PROJECT_SOURCE_DIR}/../Dependencies/nvtt CACHE PATH "")
set(nvtt_INCLUDE_SEARCH_DIRS "${nvtt_INSTALL_DIR}/include")

if(BS_64BIT)
	list(APPEND nvtt_LIBRARY_RELEASE_SEARCH_DIRS "${nvtt_INSTALL_DIR}/lib/x64/Release")
	list(APPEND nvtt_LIBRARY_DEBUG_SEARCH_DIRS "${nvtt_INSTALL_DIR}/lib/x64/Debug")
else()
	list(APPEND nvtt_LIBRARY_RELEASE_SEARCH_DIRS "${nvtt_INSTALL_DIR}/lib/x86/Release")
	list(APPEND nvtt_LIBRARY_DEBUG_SEARCH_DIRS "${nvtt_INSTALL_DIR}/lib/x86/Debug")
endif()

message(STATUS "Looking for NVTT installation...")
	
find_path(nvtt_INCLUDE_DIR nvtt.h PATHS ${nvtt_INCLUDE_SEARCH_DIRS})	
	
if(nvtt_INCLUDE_DIR)
	set(nvtt_FOUND TRUE)
else()
	set(nvtt_FOUND FALSE)
endif()	
	
find_imported_library(nvtt nvtt)
find_imported_library(nvtt bc6h)
find_imported_library(nvtt bc7)
find_imported_library(nvtt nvcore)
find_imported_library(nvtt nvimage)
find_imported_library(nvtt nvmath)
find_imported_library(nvtt nvthread)
find_imported_library(nvtt squish)

if(NOT nvtt_FOUND)
	if(nvtt_FIND_REQUIRED)
		message(FATAL_ERROR "Cannot find nvtt installation. Try modifying the nvtt_INSTALL_DIR path.")
	elseif(NOT nvtt_FIND_QUIETLY)
		message(WARNING "Cannot find nvtt installation. Try modifying the nvtt_INSTALL_DIR path.")
	endif()
else()
	set_target_properties(nvtt::nvtt PROPERTIES INTERFACE_INCLUDE_DIRECTORIES "${nvtt_INCLUDE_DIR}")	
	message(STATUS "...nvtt OK.")
endif()

mark_as_advanced(nvtt_INSTALL_DIR nvtt_INCLUDE_DIR)

set(nvtt_INCLUDE_DIRS ${nvtt_INCLUDE_DIR})