# Find LibICU dependency
#
# This module defines
#  LibICU_INCLUDE_DIRS
#  LibICU_LIBRARIES
#  LibICU_FOUND

start_find_package(LibICU)

set(snappy_INSTALL_DIR ${BSF_SOURCE_DIR}/../Dependencies/LibICU CACHE PATH "")
gen_default_lib_search_dirs(LibICU)

find_imported_includes(LibICU unicode/utypes.h)
find_imported_library(LibICU icudata)
find_imported_library(LibICU icuuc)

end_find_package(LibICU icudata)
