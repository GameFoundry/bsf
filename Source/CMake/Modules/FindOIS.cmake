# Find OIS dependency
#
# This module defines
#  OIS_INCLUDE_DIRS
#  OIS_LIBRARIES
#  OIS_FOUND

start_find_package(OIS)

set(OIS_INSTALL_DIR ${PROJECT_SOURCE_DIR}/../Dependencies/OIS CACHE PATH "")
gen_default_lib_search_dirs(OIS)

find_imported_includes(OIS OIS/OIS.h)
find_imported_library(OIS OIS)

end_find_package(OIS OIS)
