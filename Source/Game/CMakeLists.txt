# Source files and their filters
include(CMakeSources.cmake)

# Includes
set(Game_INC 
	"Include"
	"../BansheeUtility/Include" 
	"../BansheeCore/Include"
	"../BansheeEngine/Include")

include_directories(${Game_INC})	
	
# Target
add_library(Game SHARED ${BS_GAME_SRC})

# Post-build step
if(WIN32)
	if(BS_64BIT)
		set(BS_COPY_FOLDER Win64)
	else()
		set(BS_COPY_FOLDER Win32)
	endif()

	add_custom_command(TARGET Game POST_BUILD 
		COMMAND xcopy /Y /I \"$(TargetDir)$(TargetName).exe\" \"$(SolutionDir)..\\..\\Data\\Binaries\\${BS_COPY_FOLDER}\\\"
)
else()
# TODO_OTHER_PLATFORMS_GO_HERE
endif()

# Libraries
## Local libs
target_link_libraries(Game BansheeEngine BansheeUtility BansheeCore)

# IDE specific
set_property(TARGET Game PROPERTY FOLDER Executable)

# Plugin dependencies
add_dependencies(Game BansheeGLRenderAPI BansheeMono BansheeOISInput BansheePhysX RenderBeast SBansheeEngine)

if(WIN32)
	add_dependencies(Game BansheeD3D11RenderAPI BansheeD3D9RenderAPI)
endif()

# Compiler flags
target_compile_options(BansheeEditorExec PUBLIC /wd4509)