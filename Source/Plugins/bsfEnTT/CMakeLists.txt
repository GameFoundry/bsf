
file(GLOB_RECURSE SRCS
	Components/*.cpp
	Scene/*.cpp
)


add_library(bsfEnTT SHARED ${SRCS})

target_compile_features(bsfEnTT
    PRIVATE
    cxx_std_17
)


target_include_directories(bsfEnTT PUBLIC "ThirdParty/EnTT/src")
target_include_directories(bsfEnTT PUBLIC ".")