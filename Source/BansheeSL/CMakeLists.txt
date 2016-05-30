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
	COMMAND ../../../Dependencies/tools/flex/flex.exe --wincompat ../../../Source/BansheeSL/BsLexerFX.l
	COMMAND ../../../Dependencies/tools/bison/bison.exe ../../../Source/BansheeSL/BsParserFX.y
	COMMAND xcopy /Y /I BsLexerFX.h \"../../../Source/BansheeSL/Include\"
	COMMAND xcopy /Y /I BsLexerFX.c \"../../../Source/BansheeSL/Source\"
	COMMAND xcopy /Y /I BsParserFX.h \"../../../Source/BansheeSL/Include\"
	COMMAND xcopy /Y /I BsParserFX.c \"../../../Source/BansheeSL/Source\"
	COMMAND del BsLexerFX.h BsLexerFX.c BsParserFX.h BsParserFX.c
	COMMENT "Building lexer/parser..."
)
else()
# TODO_OTHER_PLATFORMS_GO_HERE
endif()

# Libraries
## Local libs
target_link_libraries(BansheeSL BansheeEngine BansheeUtility BansheeCore)

# IDE specific
set_property(TARGET BansheeSL PROPERTY FOLDER Plugins)