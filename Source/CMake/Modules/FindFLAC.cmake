# Find FLAC dependency
#
# This module defines
#  FLAC_INCLUDE_DIRS
#  FLAC_LIBRARIES
#  FLAC_FOUND

start_find_package(FLAC)

if(USE_BUNDLED_LIBRARIES)
	set(FLAC_INSTALL_DIR ${BSF_SOURCE_DIR}/../Dependencies/libFLAC CACHE PATH "")
endif()
gen_default_lib_search_dirs(FLAC)

if(WIN32)
	set(FLAC_LIBNAME libFLAC)
else()
	set(FLAC_LIBNAME FLAC)
endif()

find_imported_includes(FLAC FLAC/all.h)

if(UNIX)
	find_imported_library(FLAC ${FLAC_LIBNAME})
else()
	find_imported_library_shared(FLAC ${FLAC_LIBNAME})
endif()

if(WIN32)
	# .dll has a different name than .lib, so we must register it separately
	if(BS_64BIT)
		set(PLATFORM "x64")
	else()
		set(PLATFORM "x86")
	endif()
	
	set(RELEASE_FILENAME "libFLAC_dynamic.dll")
	set(DEBUG_FILENAME "libFLAC_dynamic.dll")

	set(SRC_RELEASE "${PROJECT_SOURCE_DIR}/bin/${PLATFORM}/Release/${RELEASE_FILENAME}")
	set(SRC_DEBUG "${PROJECT_SOURCE_DIR}/bin/${PLATFORM}/Debug/${DEBUG_FILENAME}")
	set(DESTINATION_DIR bin)

	install(
		FILES ${SRC_RELEASE}
		DESTINATION ${DESTINATION_DIR}
		CONFIGURATIONS Release RelWithDebInfo MinSizeRel
		RENAME ${RELEASE_FILENAME}
	)
		
	install(
		FILES ${SRC_DEBUG}
		DESTINATION ${DESTINATION_DIR}
		CONFIGURATIONS Debug
		RENAME ${DEBUG_FILENAME}
	)
endif()

if(USE_BUNDLED_LIBRARIES)
	install_dependency_binaries(FLAC)
endif()

end_find_package(FLAC ${FLAC_LIBNAME})
