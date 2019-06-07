# Download and unpack googletest at configure time
configure_file(CMake/Gtest.cmake googletest-download/CMakeLists.txt)
execute_process(COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" .
  RESULT_VARIABLE result
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/googletest-download )
if(result)
  message(FATAL_ERROR "CMake step for googletest failed: ${result}")
endif()
execute_process(COMMAND ${CMAKE_COMMAND} --build .
  RESULT_VARIABLE result
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/googletest-download )
if(result)
  message(FATAL_ERROR "Build step for googletest failed: ${result}")
endif()

# Prevent overriding the parent project's compiler/linker
# settings on Windows
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

# Add googletest directly to our build. This defines
# the gtest and gtest_main targets.
add_subdirectory(${CMAKE_CURRENT_BINARY_DIR}/googletest-src
                 ${CMAKE_CURRENT_BINARY_DIR}/googletest-build
                 EXCLUDE_FROM_ALL)

# The gtest/gtest_main targets carry header search path
# dependencies automatically when using CMake 2.8.11 or
# later. Otherwise we have to add them here ourselves.
if (CMAKE_VERSION VERSION_LESS 2.8.11)
  include_directories("${gtest_SOURCE_DIR}/include")
endif()

include(GoogleTest)
# Now simply link against gtest or gtest_main as needed. Eg
file(GLOB_RECURSE TEST_SRCS *_test.cpp)

add_executable(unit_tests ${TEST_SRCS})

target_compile_options(unit_tests PUBLIC -msse4.1)
target_include_directories(unit_tests PRIVATE
  "${PROJECT_BINARY_DIR}/Generated/bsfUtility"
  "Plugins/bsfEnTT/ThirdParty/EnTT/src"
  "Plugins/bsfRenderBeast"
  "Plugins"
  "Foundation/bsfCore"
  "Foundation/bsfUtility"
  "Foundation/bsfUtility/ThirdParty"
)

target_link_libraries(unit_tests gtest_main bsf bsfEnTT)

# this isn't working.
gtest_discover_tests(unit_tests)

# make test command.
add_custom_target(check ${CMAKE_COMMAND} -E env CTEST_OUTPUT_ON_FAILURE=1
                  ${CMAKE_CTEST_COMMAND} -C $<CONFIG> --verbose
                  WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
