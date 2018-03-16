# Find bsf dependency
#
# This module defines
#  bsf_INCLUDE_DIRS
#  bsf_LIBRARIES
#  bsf_FOUND

message(STATUS "Looking for bsf installation...")
set(bsf_INSTALL_DIR ${PROJECT_SOURCE_DIR}/Dependencies/bsf CACHE PATH "")

set(bsf_INCLUDE_SEARCH_DIRS "${bsf_INSTALL_DIR}/include")
set(bsf_LIBRARY_SEARCH_DIRS "${bsf_INSTALL_DIR}/lib")
set(INCLUDE_FILES "bsfCore/BsCorePrerequisites.h")

find_path(bsf_INCLUDE_DIR NAMES NAMES ${INCLUDE_FILES} PATHS ${bsf_INCLUDE_SEARCH_DIRS} NO_DEFAULT_PATH)
find_path(bsf_INCLUDE_DIR NAMES NAMES ${INCLUDE_FILES} PATHS ${bsf_INCLUDE_SEARCH_DIRS})

if(bsf_INCLUDE_DIR)
	set(bsf_FOUND TRUE)
else()
	message(STATUS "...Cannot find include file \"${INCLUDE_FILES}\" at path ${bsf_INCLUDE_SEARCH_DIRS}")
	set(bsf_FOUND FALSE)
endif()

MACRO(add_imported_library LIB_NAME LIB_PATH)
	if(NOT WIN32)
		add_library(${LIB_NAME} SHARED IMPORTED)
	else()
		add_library(${LIB_NAME} STATIC IMPORTED)
	endif()

	set_target_properties(${LIB_NAME} PROPERTIES IMPORTED_LOCATION "${LIB_PATH}")
ENDMACRO()

MACRO(find_imported_library FOLDER_NAME LIB_NAME IS_OPTIONAL)
	find_library(${LIB_NAME}_LIBRARY NAMES ${LIB_NAME} PATHS ${${FOLDER_NAME}_LIBRARY_SEARCH_DIRS} NO_DEFAULT_PATH)
	find_library(${LIB_NAME}_LIBRARY NAMES ${LIB_NAME} PATHS ${${FOLDER_NAME}_LIBRARY_SEARCH_DIRS})
	
	if(${LIB_NAME}_LIBRARY)
		add_imported_library(${FOLDER_NAME}::${LIB_NAME} "${${LIB_NAME}_LIBRARY}")
		
		list(APPEND ${FOLDER_NAME}_LIBRARIES ${FOLDER_NAME}::${LIB_NAME})
	elseif(NOT ${IS_OPTIONAL})
		set(${FOLDER_NAME}_FOUND FALSE)
		message(STATUS "...Cannot find imported library: ${LIB_NAME} ${${LIB_NAME}_LIBRARY}")
	endif()
	
	mark_as_advanced(${LIB_NAME}_LIBRARY)
ENDMACRO()

find_imported_library(bsf bsf NO)
find_imported_library(bsf bsfD3D11RenderAPI YES)
find_imported_library(bsf bsfGLRenderAPI YES)
find_imported_library(bsf bsfVulkanRenderAPI YES)
find_imported_library(bsf bsfOpenAudio YES)
find_imported_library(bsf bsfFMOD YES)
find_imported_library(bsf bsfFBXImporter YES)
find_imported_library(bsf bsfFontImporter YES)
find_imported_library(bsf bsfFreeImgImporter YES)
find_imported_library(bsf bsfPhysX NO)
find_imported_library(bsf bsfRenderBeast NO)
find_imported_library(bsf bsfSL NO)

if(NOT bsf_FOUND)
	if(bsf_FIND_REQUIRED)
		message(FATAL_ERROR "Cannot find bsf installation. Try modifying the bsf_INSTALL_DIR path.")
	elseif(NOT bsf_FIND_QUIETLY)
		message(WARNING "Cannot find bsf installation. Try modifying the bsf_INSTALL_DIR path.")
	endif()
else()
	list(APPEND INCLUDE_DIRS "${bsf_INCLUDE_DIR}/bsfUtility")
	list(APPEND INCLUDE_DIRS "${bsf_INCLUDE_DIR}/bsfCore")
	list(APPEND INCLUDE_DIRS "${bsf_INCLUDE_DIR}/bsfEngine")
	
	set_target_properties(bsf::bsf PROPERTIES INTERFACE_INCLUDE_DIRECTORIES "${INCLUDE_DIRS}")
	mark_as_advanced(bsf_INSTALL_DIR)
	message(STATUS "...bsf OK.")
endif()

mark_as_advanced(bsf_INCLUDE_DIR)
set(bsf_INCLUDE_DIRS ${bsf_INCLUDE_DIR})