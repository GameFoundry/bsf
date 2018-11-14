find_path(OGG_INCLUDE_DIR ogg/ogg.h)
find_library(OGG_LIBRARY ogg)

set(OGG_INCLUDE_DIRS ${OGG_INCLUDE_DIR})
set(OGG_LIBRARIES ${OGG_LIBRARY})

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(OGG REQUIRED_VARS OGG_INCLUDE_DIR OGG_LIBRARY)

if(OGG_FOUND AND NOT TARGET ogg::ogg)
	add_library(ogg::ogg SHARED IMPORTED)
	set_target_properties(ogg::ogg PROPERTIES
		IMPORTED_LOCATION "${OGG_LIBRARY}"
		INTERFACE_INCLUDE_DIRECTORIES "${OGG_INCLUDE_DIRS}")
endif()
