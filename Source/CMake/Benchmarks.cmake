include(FetchContent)

set(BENCHMARK_ENABLE_TESTING CACHE INTERNAL OFF)

FetchContent_Declare(
  googlebench
  GIT_REPOSITORY https://github.com/google/benchmark.git
  GIT_TAG 		 v1.5.0
)
# if > cmake 3.14 
# FetchContent_MakeAvailable(googlebench)
# else
FetchContent_GetProperties(googlebench)
if(NOT googlebench_POPULATED)
  FetchContent_Populate(googlebench)
  add_subdirectory(${googlebench_SOURCE_DIR} ${googlebench_BINARY_DIR})
endif()
# make sure the googletest is marked as advacned so its variables aren't
# showing up in our gui cache options. kinda annoying this is necessary...
mark_as_advanced(
	FETCHCONTENT_BASE_DIR
	FETCHCONTENT_FULLY_DISCONNECTED
	FETCHCONTENT_SOURCE_DIR_GOOGLEBENCH
	FETCHCONTENT_UPDATES_DISCONNECTED_GOOGLEBENCH
)

# Now simply link against gtest or gtest_main as needed. Eg
file(GLOB_RECURSE TEST_BENCHES Foundation/*_bench.cpp)

add_executable(bsfBenchmarks ${TEST_BENCHES})

target_link_libraries(bsfBenchmarks benchmark bsf)

if(NOT BS_IS_BANSHEE3D)
	install(TARGETS bsfBenchmarks RUNTIME DESTINATION bin)	
endif()

# This only works if enable_testing() is at the root cmakeLists
# however I've decided to disable it because ctest doesn't play well with google-test 
# in terms of - it wants to run tests as separate threads which can be annoying if we want to 
# to use the same application thread for all the unit-tests (so that window doesn't keep popping up for every test)
# gtest_discover_tests(unit_tests)