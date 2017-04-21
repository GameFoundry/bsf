# Find XShaderCompiler dependency
#
# This module defines
#  XShaderCompiler_INCLUDE_DIRS
#  XShaderCompiler_LIBRARIES
#  XShaderCompiler_FOUND

set(XShaderCompiler_INSTALL_DIR ${PROJECT_SOURCE_DIR}/../Dependencies/XShaderCompiler CACHE PATH "")
set(XShaderCompiler_SOURCE_DIR ${PROJECT_SOURCE_DIR}/External/XShaderCompiler)
set(XShaderCompiler_BUILD_DIR ${PROJECT_SOURCE_DIR}/../Dependencies/Build/XShaderCompiler)

set(XShaderCompiler_INCLUDE_SEARCH_DIRS 
	"${XShaderCompiler_INSTALL_DIR}/include"
	"${XShaderCompiler_SOURCE_DIR}/inc")

if(BS_64BIT)
	list(APPEND XShaderCompiler_LIBRARY_RELEASE_SEARCH_DIRS "${XShaderCompiler_INSTALL_DIR}/lib/x64/Release")
	list(APPEND XShaderCompiler_LIBRARY_DEBUG_SEARCH_DIRS "${XShaderCompiler_INSTALL_DIR}/lib/x64/Debug")
else()
	list(APPEND XShaderCompiler_LIBRARY_RELEASE_SEARCH_DIRS "${XShaderCompiler_INSTALL_DIR}/lib/x86/Release")
	list(APPEND XShaderCompiler_LIBRARY_DEBUG_SEARCH_DIRS "${XShaderCompiler_INSTALL_DIR}/lib/x86/Debug")
endif()

list(APPEND XShaderCompiler_LIBRARY_RELEASE_SEARCH_DIRS "${XShaderCompiler_BUILD_DIR}/Release/install")
list(APPEND XShaderCompiler_LIBRARY_DEBUG_SEARCH_DIRS "${XShaderCompiler_BUILD_DIR}/Debug/install")

message(STATUS "Looking for XShaderCompiler installation...")
	
# Force the system to always look for files, instead of reading cached entries
unset(XShaderCompiler_INCLUDE_DIR CACHE)
unset(XShaderCompiler_LIBRARY_RELEASE CACHE)
unset(XShaderCompiler_LIBRARY_DEBUG CACHE)
	
find_path(XShaderCompiler_INCLUDE_DIR Xsc/Xsc.h PATHS ${XShaderCompiler_INCLUDE_SEARCH_DIRS})
find_library(XShaderCompiler_LIBRARY_RELEASE NAMES xsc_core PATHS ${XShaderCompiler_LIBRARY_RELEASE_SEARCH_DIRS})
find_library(XShaderCompiler_LIBRARY_DEBUG NAMES xsc_core PATHS ${XShaderCompiler_LIBRARY_DEBUG_SEARCH_DIRS})

if(XShaderCompiler_INCLUDE_DIR AND XShaderCompiler_LIBRARY_DEBUG AND XShaderCompiler_LIBRARY_RELEASE)
	set(XShaderCompiler_FOUND TRUE)
endif()

if(NOT XShaderCompiler_FOUND)
	if(XShaderCompiler_FIND_REQUIRED)
		message(FATAL_ERROR "Cannot find XShaderCompiler installation. Try modifying the XShaderCompiler_INSTALL_DIR path.")
	elseif(NOT XShaderCompiler_FIND_QUIETLY)
		message(WARNING "Cannot find XShaderCompiler installation. Try modifying the XShaderCompiler_INSTALL_DIR path.")
	endif()
else()
	message(STATUS "...XShaderCompiler OK.")
endif()

if(XShaderCompiler_FOUND)
	add_library(XShaderCompiler STATIC IMPORTED)
	set_target_properties(XShaderCompiler PROPERTIES IMPORTED_LOCATION_DEBUG "${XShaderCompiler_LIBRARY_DEBUG}")
	set_target_properties(XShaderCompiler PROPERTIES IMPORTED_LOCATION_OPTIMIZEDDEBUG "${XShaderCompiler_LIBRARY_DEBUG}")
	set_target_properties(XShaderCompiler PROPERTIES IMPORTED_LOCATION_RELEASE "${XShaderCompiler_LIBRARY_RELEASE}")
endif()

mark_as_advanced(
	XShaderCompiler_INSTALL_DIR 
	XShaderCompiler_INCLUDE_DIR 
	XShaderCompiler_LIBRARY_RELEASE
	XShaderCompiler_LIBRARY_DEBUG)

set(XShaderCompiler_INCLUDE_DIRS ${XShaderCompiler_INCLUDE_DIR})
set(XShaderCompiler_LIBRARIES XShaderCompiler)

set(XShaderCompiler_INCLUDE_DIRS ${XShaderCompiler_INCLUDE_DIR} PARENT_SCOPE)
set(XShaderCompiler_LIBRARIES XShaderCompiler PARENT_SCOPE)