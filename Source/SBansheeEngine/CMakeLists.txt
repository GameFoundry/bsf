# Source files and their filters
include(CMakeSources.cmake)

source_group("Header Files\\Generated" FILES ${BS_GENERATED_ENGINE_H_FILES})
source_group("Source Files\\Generated" FILES ${BS_GENERATED_ENGINE_CPP_FILES})

# Includes
set(SBansheeEngine_INC 
	"./"
	"../BansheeUtility" 
	"../BansheeCore"
	"../BansheeEngine"
	"../BansheeEditor"
	"../BansheeMono"
	"${PROJECT_BINARY_DIR}/Generated/Engine/Include")

include_directories(${SBansheeEngine_INC})	

# Target
add_library(SBansheeEngine SHARED 
	${BS_SBANSHEEENGINE_SRC}
	${BS_GENERATED_ENGINE_H_FILES} 
	${BS_GENERATED_ENGINE_CPP_FILES})

# Defines
target_compile_definitions(SBansheeEngine PRIVATE -DBS_SCR_BE_EXPORTS)

# Libraries
## Local libs
target_link_libraries(SBansheeEngine BansheeMono BansheeEditor BansheeEngine BansheeUtility BansheeCore)

# IDE specific
set_property(TARGET SBansheeEngine PROPERTY FOLDER Script)