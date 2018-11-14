find_path(FREEIMAGE_INCLUDE_DIR FreeImage.h)
find_library(FREEIMAGE_LIBRARY freeimage)

set(FREEIMAGE_INCLUDE_DIRS ${FREEIMAGE_INCLUDE_DIR})
set(FREEIMAGE_LIBRARIES ${FREEIMAGE_LIBRARY})

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(FREEIMAGE REQUIRED_VARS FREEIMAGE_INCLUDE_DIR FREEIMAGE_LIBRARY)

if(FREEIMAGE_FOUND AND NOT TARGET freeimage::freeimage)
	add_library(freeimage::freeimage SHARED IMPORTED)
	set_target_properties(freeimage::freeimage PROPERTIES
		IMPORTED_LOCATION "${FREEIMAGE_LIBRARY}"
		INTERFACE_INCLUDE_DIRECTORIES "${FREEIMAGE_INCLUDE_DIRS}")
endif()
