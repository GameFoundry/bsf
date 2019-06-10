# Source files and their filters
include(CMakeSources.cmake)

# Target
add_library(bsfECSRenderer SHARED ${BS_ECS_SRC})

# Common flags
add_common_flags(bsfECSRenderer)

# Includes
target_include_directories(bsfECSRenderer PRIVATE
  "./"
  "${CMAKE_SOURCE_DIR}/Source/Plugins"
  "${CMAKE_SOURCE_DIR}/Source/Plugins/bsfEnTT"
  "${CMAKE_SOURCE_DIR}/Source/Plugins/bsfEnTT/ThirdParty/EnTT/src"
)

# Defines
target_compile_definitions(bsfECSRenderer PRIVATE -DBS_BSRND_EXPORTS)

# Libraries
## Local libs
target_link_libraries(bsfECSRenderer bsf)

# IDE specific
set_property(TARGET bsfECSRenderer PROPERTY FOLDER Plugins)

# Install
if(RENDERER_MODULE MATCHES "ECS")
	install_bsf_target(bsfECSRenderer)
endif()

conditional_cotire(bsfECSRenderer)
