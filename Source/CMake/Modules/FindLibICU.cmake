# Find LibICU dependency
#
# This module defines
#  LibICU_INCLUDE_DIRS
#  LibICU_LIBRARIES
#  LibICU_FOUND

start_find_package(LibICU)

set(LibICU_INSTALL_DIR ${BS_DEPENDENCY_DIR}/libICU CACHE PATH "")
gen_default_lib_search_dirs(LibICU)

find_imported_includes(LibICU unicode/utypes.h)
find_imported_library(LibICU icudata)
find_imported_library(LibICU icuuc)

end_find_package(LibICU icudata)
