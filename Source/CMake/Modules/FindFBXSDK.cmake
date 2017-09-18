# Find FBXSDK dependency
#
# This module defines
#  FBXSDK_INCLUDE_DIRS
#  FBXSDK_LIBRARIES
#  FBXSDK_FOUND

start_find_package(FBXSDK)

set(FBXSDK_INSTALL_DIR ${PROJECT_SOURCE_DIR}/../Dependencies/FBXSDK CACHE PATH "")
gen_default_lib_search_dirs(FBXSDK)

if(WIN32)
    set(FBXSDK_LIBNAME libfbxsdk-md)
else()
    set(FBXSDK_LIBNAME fbxsdk)
endif()

find_imported_includes(FBXSDK fbxsdk.h)
find_imported_library(FBXSDK ${FBXSDK_LIBNAME})

end_find_package(FBXSDK ${FBXSDK_LIBNAME})

