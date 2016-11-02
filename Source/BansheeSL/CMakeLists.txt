
if(NOT WIN32)
	find_package(BISON REQUIRED)
	find_package(FLEX REQUIRED)
	message("Building lexer/parser...")
	execute_process(
		COMMAND ${FLEX_EXECUTABLE} ${BS_FLEX_OPTIONS} ${PROJECT_SOURCE_DIR}/BansheeSL/BsLexerFX.l
		COMMAND ${BISON_EXECUTABLE} ${PROJECT_SOURCE_DIR}/BansheeSL/BsParserFX.y)
	execute_process(
		COMMAND mv BsLexerFX.h ${PROJECT_SOURCE_DIR}/BansheeSL/Include
		COMMAND mv BsLexerFX.c ${PROJECT_SOURCE_DIR}/BansheeSL/Source
		COMMAND mv BsParserFX.h ${PROJECT_SOURCE_DIR}/BansheeSL/Include
		COMMAND mv BsParserFX.c ${PROJECT_SOURCE_DIR}/BansheeSL/Source)
endif()

# Source files and their filters
include(CMakeSources.cmake)

# Includes
set(BansheeSL_INC
	"Include"
	"../BansheeUtility/Include"
	"../BansheeCore/Include"
	"../BansheeEngine/Include")

include_directories(${BansheeSL_INC})

# Target
add_library(BansheeSL SHARED ${BS_BANSHEESL_SRC})

# Defines
target_compile_definitions(BansheeSL PRIVATE -DBS_SL_EXPORTS)

# Pre-build step
if(WIN32)
	add_custom_command(TARGET BansheeSL PRE_BUILD
		COMMAND ${PROJECT_SOURCE_DIR}/../Dependencies/tools/flex/flex.exe --wincompat ${PROJECT_SOURCE_DIR}/BansheeSL/BsLexerFX.l
		COMMAND ${PROJECT_SOURCE_DIR}/../Dependencies/tools/bison/bison.exe ${PROJECT_SOURCE_DIR}/BansheeSL/BsParserFX.y
		COMMAND xcopy /Y /I BsLexerFX.h \"${PROJECT_SOURCE_DIR}/BansheeSL/Include\"
		COMMAND xcopy /Y /I BsLexerFX.c \"${PROJECT_SOURCE_DIR}/BansheeSL/Source\"
		COMMAND xcopy /Y /I BsParserFX.h \"${PROJECT_SOURCE_DIR}/BansheeSL/Include\"
		COMMAND xcopy /Y /I BsParserFX.c \"${PROJECT_SOURCE_DIR}/BansheeSL/Source\"
		COMMAND del BsLexerFX.h BsLexerFX.c BsParserFX.h BsParserFX.c
		COMMENT "Building lexer/parser...")
endif()

# Libraries
## Local libs
target_link_libraries(BansheeSL BansheeEngine BansheeUtility BansheeCore)

# IDE specific
set_property(TARGET BansheeSL PROPERTY FOLDER Plugins)
