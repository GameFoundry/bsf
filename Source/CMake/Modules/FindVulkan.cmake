# Find Vulkan installation
#
# This module defines
#  Vulkan_INCLUDE_DIRS
#  Vulkan_LIBRARIES
#  Vulkan_FOUND

set(Vulkan_INSTALL_DIRS "$ENV{VULKAN_SDK}" CACHE PATH "")

if(WIN32)
	set(Vulkan_INCLUDE_SEARCH_DIRS "${Vulkan_INSTALL_DIRS}/Include")
	
	if(BS_64BIT)
		set(Vulkan_LIBRARY_SEARCH_DIRS "${Vulkan_INSTALL_DIRS}/Bin")
	else()
		set(Vulkan_LIBRARY_SEARCH_DIRS "${Vulkan_INSTALL_DIRS}/Bin32")
	endif()
else()
	set(Vulkan_INCLUDE_SEARCH_DIRS "${Vulkan_INSTALL_DIRS}/include")
	set(Vulkan_LIBRARY_SEARCH_DIRS "${Vulkan_INSTALL_DIRS}/lib")
endif()

message(STATUS "Looking for Vulkan installation...")
find_path(Vulkan_INCLUDE_DIR vulkan/vulkan.h PATHS ${Vulkan_INCLUDE_SEARCH_DIRS})
find_library(Vulkan_LIBRARY NAMES vulkan-1 PATHS ${Vulkan_LIBRARY_SEARCH_DIRS})

if(Vulkan_INCLUDE_DIR AND Vulkan_LIBRARY)
	set(Vulkan_FOUND TRUE)
endif()

if(NOT Vulkan_FOUND)
	if(Vulkan_FIND_REQUIRED)
		message(FATAL_ERROR "Cannot find Vulkan installation. Try modifying the Vulkan_INSTALL_DIRS path.")
	else()
		message(WARNING "Cannot find Vulkan installation. Try modifying the Vulkan_INSTALL_DIRS path.")
	endif()
else()
	message(STATUS "...Vulkan OK.")
endif()

if(Vulkan_FOUND)
	add_imported_library(Vulkan "${Vulkan_LIBRARY}" "${Vulkan_LIBRARY}")
	set_target_properties(Vulkan PROPERTIES INTERFACE_INCLUDE_DIRECTORIES "${Vulkan_INCLUDE_DIR}")
endif()

mark_as_advanced(
	Vulkan_INSTALL_DIRS 
	Vulkan_INCLUDE_DIR 
	Vulkan_LIBRARY)
set(Vulkan_INCLUDE_DIRS ${Vulkan_INCLUDE_DIR})
set(Vulkan_LIBRARIES Vulkan)