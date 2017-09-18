# Find Snappy dependency
#
# This module defines
#  snappy_INCLUDE_DIRS
#  snappy_LIBRARIES
#  snappy_FOUND

start_find_package(snappy)

set(snappy_INSTALL_DIR ${PROJECT_SOURCE_DIR}/../Dependencies/snappy CACHE PATH "")
gen_default_lib_search_dirs(snappy)

find_imported_includes(snappy snappy.h)
find_imported_library(snappy snappy)

end_find_package(snappy snappy)
