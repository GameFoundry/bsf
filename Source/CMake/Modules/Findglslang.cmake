# Find glslang
#
# This module defines
#  glslang_INCLUDE_DIRS
#  glslang_LIBRARIES
#  glslang_FOUND

set(glslang_INSTALL_DIRS ${PROJECT_SOURCE_DIR}/../Dependencies/glslang CACHE PATH "")
set(glslang_INCLUDE_SEARCH_DIRS "${glslang_INSTALL_DIRS}/include")

if(BS_64BIT)
	set(glslang_LIBRARY_RELEASE_SEARCH_DIRS "${glslang_INSTALL_DIRS}/lib/x64")
else()
	set(glslang_LIBRARY_RELEASE_SEARCH_DIRS "${glslang_INSTALL_DIRS}/lib/x86")
endif()

message(STATUS "Looking for glslang installation...")

find_path(glslang_INCLUDE_DIR glslang/Public/ShaderLang.h PATHS ${glslang_INCLUDE_SEARCH_DIRS})

if(glslang_INCLUDE_DIR)
	set(glslang_FOUND TRUE)
else()
	set(glslang_FOUND FALSE)
endif()

find_imported_library2(glslang glslang glslangd)
find_imported_library2(glslang HLSL HLSLd)
find_imported_library2(glslang OSDependent OSDependentd)
find_imported_library2(glslang OGLCompiler OGLCompilerd)
find_imported_library2(glslang SPIRV SPIRVd)

if(NOT glslang_FOUND)
	if(glslang_FIND_REQUIRED)
		message(FATAL_ERROR "Cannot find glslang installation. Try modifying the glslang_INSTALL_DIRS path.")
	else()
		message(WARNING "Cannot find glslang installation. Try modifying the glslang_INSTALL_DIRS path.")
	endif()
else()
	message(STATUS "...glslang OK.")
endif()

if(glslang_FOUND)
	set_target_properties(glslang::glslang PROPERTIES INTERFACE_INCLUDE_DIRECTORIES "${glslang_INCLUDE_DIR}")	
endif()

mark_as_advanced(
	glslang_INSTALL_DIRS 
	glslang_INCLUDE_DIR 
)

set(glslang_INCLUDE_DIRS ${glslang_INCLUDE_DIR})