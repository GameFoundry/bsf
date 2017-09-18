# Source files and their filters
include(CMakeSources.cmake)

# Find packages
if(NOT WIN32)
	find_package(X11 REQUIRED)
	find_package(LibUUID REQUIRED)
	find_package(OpenGL REQUIRED)

	if(NOT X11_Xcursor_FOUND)
		message(FATAL_ERROR "Could not find XCursor library.")
	endif()

	if(NOT X11_Xrandr_FOUND)
		message(FATAL_ERROR "Could not find XRandR library.")
	endif()
endif()

# Includes
set(BansheeGLRenderAPI_INC 
	"./" 
	"../BansheeUtility" 
	"../BansheeCore"
	"GLSL/include")

if(WIN32)
	set(BansheeGLRenderAPI_INC ${BansheeGLRenderAPI_INC} "Source/Win32")
else()
	set(BansheeGLRenderAPI_INC ${BansheeGLRenderAPI_INC} ${OPENGL_INCLUDE_DIR})
# TODO_OTHER_PLATFORMS_GO_HERE
endif()
	
include_directories(${BansheeGLRenderAPI_INC})	
	
# Target
add_library(BansheeGLRenderAPI SHARED ${BS_BANSHEEGLRENDERAPI_SRC})

# Defines
target_compile_definitions(BansheeGLRenderAPI PRIVATE -DBS_RSGL_EXPORTS -DGLEW_STATIC)

# Libraries
if(WIN32)
	## External lib: OpenGL
	target_link_libraries(BansheeGLRenderAPI PRIVATE glu32 opengl32)
else()
	## External lib: OpenGL, X11, LibUUID
	target_link_libraries(BansheeGLRenderAPI PRIVATE ${X11_LIBRARIES} ${X11_Xcursor_LIB} ${X11_Xrandr_LIB}
        ${OPENGL_LIBRARIES} ${LibUUID_LIBRARIES})
endif()

## Local libs
target_link_libraries(BansheeGLRenderAPI PUBLIC BansheeUtility BansheeCore)

# IDE specific
set_property(TARGET BansheeGLRenderAPI PROPERTY FOLDER Plugins)