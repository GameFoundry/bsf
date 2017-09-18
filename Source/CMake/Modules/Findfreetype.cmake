# Find freetype dependency
#
# This module defines
#  freetype_INCLUDE_DIRS
#  freetype_LIBRARIES
#  freetype_FOUND

start_find_package(freetype)

set(freetype_INSTALL_DIR ${PROJECT_SOURCE_DIR}/../Dependencies/freetype CACHE PATH "")
gen_default_lib_search_dirs(freetype)

list(APPEND freetype_INCLUDE_SEARCH_DIRS /usr/local/include/freetype2)

find_imported_includes(freetype freetype/freetype.h)
find_imported_library(freetype freetype)

end_find_package(freetype freetype)

