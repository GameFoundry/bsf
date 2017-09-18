# Find XShaderCompiler dependency
#
# This module defines
#  XShaderCompiler_INCLUDE_DIRS
#  XShaderCompiler_LIBRARIES
#  XShaderCompiler_FOUND

start_find_package(XShaderCompiler)

set(XShaderCompiler_INSTALL_DIR ${PROJECT_SOURCE_DIR}/../Dependencies/XShaderCompiler CACHE PATH "")
gen_default_lib_search_dirs(XShaderCompiler)

find_imported_includes(XShaderCompiler Xsc/Xsc.h)
find_imported_library(XShaderCompiler xsc_core)

end_find_package(XShaderCompiler xsc_core)
