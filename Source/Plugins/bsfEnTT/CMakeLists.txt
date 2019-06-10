
file(GLOB_RECURSE SRCS
	Components/*.cpp
	RenderComponents/*.cpp
	Scene/*.cpp
)
list(FILTER SRCS EXCLUDE REGEX ".*test.cpp")


add_library(bsfEnTT SHARED ${SRCS})

# target_compile_features(bsfEnTT
#     PRIVATE
#     cxx_std_17
# )
# prevents the "undefined reference to typeinfo"
target_compile_options(bsfEnTT PUBLIC -fno-rtti -fPIC)

add_common_flags(bsfEnTT)

target_include_directories(bsfEnTT PRIVATE "ThirdParty/EnTT/src")
target_include_directories(bsfEnTT PRIVATE ".")
target_include_directories(bsfEnTT PUBLIC
	$<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/Source/Plugins>
	$<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/Source/Foundation/bsfUtility>
	$<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/Source/Foundation/bsfCore>
	$<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/Source/Foundation/bsfEngine>
	$<BUILD_INTERFACE:${PROJECT_BINARY_DIR}/Generated/bsfUtility>
	$<BUILD_INTERFACE:${PROJECT_BINARY_DIR}/Generated/bsfEngine>
	# $<INSTALL_INTERFACE:${CMAKE_INSTALL_PREFIX}/include/bsfUtility>
	# $<INSTALL_INTERFACE:${CMAKE_INSTALL_PREFIX}/include/bsfCore>
	# $<INSTALL_INTERFACE:${CMAKE_INSTALL_PREFIX}/include/bsfEngine>
)