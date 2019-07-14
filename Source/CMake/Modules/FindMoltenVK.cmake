# Find MoltenVK installation
#
# This module defines
#  MoltenVK_INCLUDE_DIRS
#  MoltenVK_LIBRARIES
#  MoltenVK_FOUND

start_find_package(MoltenVK)

set(MoltenVK_INSTALL_DIR ${BSF_SOURCE_DIR}/../Dependencies/MoltenVK CACHE PATH "")
gen_default_lib_search_dirs(MoltenVK)

find_imported_includes(MoltenVK MoltenVK/mvk_vulkan.h)
find_imported_library(MoltenVK MoltenVK)

end_find_package(MoltenVK MoltenVK)
