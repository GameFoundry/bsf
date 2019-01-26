# Source files and their filters
include(CMakeSources.cmake)

file(GLOB BS_GENERATED_SCRIPT_H_FILES ${CMAKE_CURRENT_SOURCE_DIR}/Generated/*.h)
file(GLOB BS_GENERATED_SCRIPT_CPP_FILES ${CMAKE_CURRENT_SOURCE_DIR}/Generated/*.cpp)

source_group("Generated" FILES ${BS_GENERATED_SCRIPT_H_FILES} ${BS_GENERATED_SCRIPT_CPP_FILES})

# Target
add_library(bsfScript SHARED 
	${BS_SCRIPT_SRC}
	${BS_GENERATED_SCRIPT_H_FILES} 
	${BS_GENERATED_SCRIPT_CPP_FILES})

# Common flags
add_common_flags(bsfScript)
	
# Includes
set(bsfScript_INC 
	"./"
	"./Generated")

target_include_directories(bsfScript PUBLIC ${bsfScript_INC})
	
# Defines
target_compile_definitions(bsfScript PRIVATE -DBS_SCR_BE_EXPORTS)

# Libraries
## Local libs
target_link_libraries(bsfScript bsfMono bsf)

# IDE specific
set_property(TARGET bsfScript PROPERTY FOLDER Script)