include(FetchContent)

FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  # GIT_TAG        release-1.8.1
  
  # instead of 1.8.1 which was 2018, we'll use a recent stable commit of 2019
  # so that it's up to date with recent api changes.
  GIT_TAG 			437e1008c97b6bf595fec85da42c6925babd96b2
)

# if > cmake 3.14 
# FetchContent_MakeAvailable(googletest)
# else
FetchContent_GetProperties(googletest)
if(NOT googletest_POPULATED)
  FetchContent_Populate(googletest)
  add_subdirectory(${googletest_SOURCE_DIR} ${googletest_BINARY_DIR})
endif()
# make sure the googletest is marked as advacned so its variables aren't
# showing up in our gui cache options. kinda annoying this is necessary...
mark_as_advanced(
	FETCHCONTENT_BASE_DIR
	FETCHCONTENT_FULLY_DISCONNECTED
	FETCHCONTENT_SOURCE_DIR_GOOGLETEST
	FETCHCONTENT_UPDATES_DISCONNECTED_GOOGLETEST
)

# Now simply link against gtest or gtest_main as needed. Eg
file(GLOB_RECURSE TEST_SRCS Foundation/*_test.cpp)

add_executable(bsfUnitTests ${TEST_SRCS})

target_compile_options(bsfUnitTests PUBLIC -msse4.1)

target_link_libraries(bsfUnitTests gtest_main bsf)

if(RENDERER_MODULE MATCHES "RenderBeast")
	file(GLOB_RECURSE RENDER_BEAST_SRCS Plugins/bsfRenderBeast/*_test.cpp)
	target_sources(bsfUnitTests PUBLIC ${RENDER_BEAST_SRCS})
	set(RENDERER_MODULE_LIB bsfRenderBeast)
	# render beast as unit-tests.
	target_link_libraries(bsfUnitTests bsfRenderBeast)
	# manually include bsfRenderBeast
	target_include_directories(bsfUnitTests PRIVATE
		$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/Plugins/bsfRenderBeast>
	)
endif()

if(NOT BS_IS_BANSHEE3D)
	install(TARGETS bsfUnitTests RUNTIME DESTINATION bin)	
endif()

# This only works if enable_testing() is at the root cmakeLists
# however I've decided to disable it because ctest doesn't play well with google-test 
# in terms of - it wants to run tests as separate threads which can be annoying if we want to 
# to use the same application thread for all the unit-tests (so that window doesn't keep popping up for every test)
# gtest_discover_tests(unit_tests)