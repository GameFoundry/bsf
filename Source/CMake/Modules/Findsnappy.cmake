# Find Snappy dependency
#
# This module defines
#  snappy_INCLUDE_DIRS
#  snappy_LIBRARIES
#  snappy_FOUND

start_find_package(snappy)

set(snappy_INSTALL_DIR ${BS_DEPENDENCY_DIR}/snappy CACHE PATH "")
gen_default_lib_search_dirs(snappy)

find_imported_includes(snappy snappy.h)
find_imported_library(snappy snappy)

end_find_package(snappy snappy)
