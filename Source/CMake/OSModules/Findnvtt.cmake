find_path(NVTT_INCLUDE_DIR nvtt.h PATH_SUFFIXES nvtt)
find_library(NVTT_LIBRARY nvtt)

set(NVTT_INCLUDE_DIRS ${NVTT_INCLUDE_DIR})
set(NVTT_LIBRARIES ${NVTT_LIBRARY})

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(NVTT REQUIRED_VARS NVTT_INCLUDE_DIR NVTT_LIBRARY)

if(NVTT_FOUND AND NOT TARGET nvtt::nvtt)
	add_library(nvtt::nvtt SHARED IMPORTED)
	set_target_properties(nvtt::nvtt PROPERTIES
		IMPORTED_LOCATION "${NVTT_LIBRARY}"
		INTERFACE_INCLUDE_DIRECTORIES "${NVTT_INCLUDE_DIRS}")
endif()
