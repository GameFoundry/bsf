# Find freeimg dependency
#
# This module defines
#  freeimg_INCLUDE_DIRS
#  freeimg_LIBRARIES
#  freeimg_FOUND

start_find_package(freeimg)

set(freeimg_INSTALL_DIR ${BS_DEPENDENCY_DIR}/freeimg CACHE PATH "")
gen_default_lib_search_dirs(freeimg)

find_imported_includes(freeimg FreeImage.h)
find_imported_library(freeimg freeimage)

end_find_package(freeimg freeimage)

