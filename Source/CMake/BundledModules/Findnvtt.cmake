# Find nvtt dependency
#
# This module defines
#  nvtt_INCLUDE_DIRS
#  nvtt_LIBRARIES
#  nvtt_FOUND

start_find_package(nvtt)

set(nvtt_INSTALL_DIR ${BSF_SOURCE_DIR}/../Dependencies/nvtt CACHE PATH "")
gen_default_lib_search_dirs(nvtt)

find_imported_includes(nvtt nvtt.h)
find_imported_library(nvtt nvtt)
find_imported_library(nvtt nvimage)
find_imported_library(nvtt bc6h)
find_imported_library(nvtt bc7)
find_imported_library(nvtt nvcore)
find_imported_library(nvtt nvmath)
find_imported_library(nvtt nvthread)
find_imported_library(nvtt squish)

target_link_libraries(nvtt::nvtt INTERFACE nvtt::nvimage nvtt::bc6h nvtt::bc7 nvtt::nvcore nvtt::nvmath nvtt::nvthread nvtt::squish)

end_find_package(nvtt nvtt)
