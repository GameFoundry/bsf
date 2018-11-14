find_path(UUID_INCLUDE_DIR uuid/uuid.h)
find_library(UUID_LIBRARY uuid)

set(UUID_INCLUDE_DIRS ${UUID_INCLUDE_DIR})
set(UUID_LIBRARIES ${UUID_LIBRARY})

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(UUID REQUIRED_VARS UUID_INCLUDE_DIR UUID_LIBRARY)

if(UUID_FOUND AND NOT TARGET uuid::uuid)
	add_library(uuid::uuid SHARED IMPORTED)
	set_target_properties(uuid::uuid PROPERTIES
		IMPORTED_LOCATION "${UUID_LIBRARY}"
		INTERFACE_INCLUDE_DIRECTORIES "${UUID_INCLUDE_DIRS}")
endif()
