include(FetchContent)
FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  # GIT_TAG        release-1.8.1
  
  # instead of 1.8.1 which was 2018, we'll use a recent stable commit of 2019
  # so that it's up to date with recent api changes.
  GIT_TAG 			437e1008c97b6bf595fec85da42c6925babd96b2
)

FetchContent_MakeAvailable(googletest)

include(GoogleTest)
# Now simply link against gtest or gtest_main as needed. Eg
file(GLOB_RECURSE TEST_SRCS Foundation/*_test.cpp)

add_executable(unit_tests ${TEST_SRCS})

target_compile_options(unit_tests PUBLIC -msse4.1)

target_link_libraries(unit_tests gtest_main bsf)

# if(RENDERER_MODULE MATCHES "RenderBeast")
# 	set(RENDERER_MODULE_LIB bsfRenderBeast)
# 	# render beast as unit-tests.
# 	target_link_libraries(unit_tests bsfRenderBeast)
# 	# manually include bsfRenderBeast
# 	target_include_directories(unit_tests PRIVATE
# 		$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/Plugins/bsfRenderBeast>
# 	)
# endif()

# this isn't working.
gtest_discover_tests(unit_tests)

# make test command.
add_custom_target(check ${CMAKE_COMMAND} -E env CTEST_OUTPUT_ON_FAILURE=1
									${CMAKE_CTEST_COMMAND} -C $<CONFIG> --verbose
									WORKING_DIRECTORY ${CMAKE_BINARY_DIR})