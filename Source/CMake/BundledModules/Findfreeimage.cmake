# Find freeimage dependency
#
# This module defines
#  freeimage_INCLUDE_DIRS
#  freeimage_LIBRARIES
#  freeimage_FOUND

start_find_package(freeimage)

set(freeimage_INSTALL_DIR ${BSF_SOURCE_DIR}/../Dependencies/freeimg CACHE PATH "")
gen_default_lib_search_dirs(freeimage)

find_imported_includes(freeimage FreeImage.h)
find_imported_library(freeimage freeimage)

end_find_package(freeimage freeimage)

