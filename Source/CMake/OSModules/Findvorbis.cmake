find_path(VORBIS_INCLUDE_DIR vorbis/codec.h)
find_library(VORBIS_LIBRARY vorbis)

set(VORBIS_INCLUDE_DIRS ${VORBIS_INCLUDE_DIR})
set(VORBIS_LIBRARIES ${VORBIS_LIBRARY})

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(VORBIS REQUIRED_VARS VORBIS_INCLUDE_DIR VORBIS_LIBRARY)

if(VORBIS_FOUND AND NOT TARGET vorbis::vorbis)
	add_library(vorbis::vorbis SHARED IMPORTED)
	set_target_properties(vorbis::vorbis PROPERTIES
		IMPORTED_LOCATION "${VORBIS_LIBRARY}"
		INTERFACE_INCLUDE_DIRECTORIES "${VORBIS_INCLUDE_DIRS}")
endif()
