
file(GLOB_RECURSE SRCS
	Components/*.cpp
	RenderComponents/*.cpp
	Scene/*.cpp
)


add_library(bsfEnTT SHARED ${SRCS})

target_compile_features(bsfEnTT
    PRIVATE
    cxx_std_17
)

add_common_flags(bsfEnTT)

target_include_directories(bsfEnTT PRIVATE "ThirdParty/EnTT/src")
target_include_directories(bsfEnTT PRIVATE ".")
target_include_directories(bsfEnTT PUBLIC
	$<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/Source/Foundation/bsfUtility>
	$<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/Source/Foundation/bsfCore>
	$<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/Source/Foundation/bsfEngine>
	$<BUILD_INTERFACE:${PROJECT_BINARY_DIR}/Generated/bsfUtility>
	$<BUILD_INTERFACE:${PROJECT_BINARY_DIR}/Generated/bsfEngine>
	# $<INSTALL_INTERFACE:${CMAKE_INSTALL_PREFIX}/include/bsfUtility>
	# $<INSTALL_INTERFACE:${CMAKE_INSTALL_PREFIX}/include/bsfCore>
	# $<INSTALL_INTERFACE:${CMAKE_INSTALL_PREFIX}/include/bsfEngine>
)