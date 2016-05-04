# Source files and their filters
include(CMakeSources.cmake)

# Includes
set(BansheeD3D9RenderAPI_INC 
	"Include" 
	"../BansheeUtility/Include" 
	"../BansheeCore/Include"
	"$ENV{DXSDK_DIR}Include")

include_directories(${BansheeD3D9RenderAPI_INC})	
	
# Target
add_library(BansheeD3D9RenderAPI SHARED ${BS_BANSHEED3D9RENDERAPI_SRC})

# Defines
target_compile_definitions(BansheeD3D9RenderAPI PRIVATE -DBS_RSD3D9_EXPORTS)

# Libraries

if(BS_64BIT)
	set(BS_DX9_LIB_PATH "$ENV{DXSDK_DIR}Lib/x64/")
else()
	set(BS_DX9_LIB_PATH "$ENV{DXSDK_DIR}Lib/x86/")
endif()
	
## External lib: DirectX 9
target_link_libraries(BansheeD3D9RenderAPI 
	${BS_DX9_LIB_PATH}d3d9.lib
	${BS_DX9_LIB_PATH}d3dx9.lib
	${BS_DX9_LIB_PATH}DxErr.lib
	${BS_DX9_LIB_PATH}dxguid.lib
	legacy_stdio_definitions.lib)	
	
## Local libs
target_link_libraries(BansheeD3D9RenderAPI BansheeUtility BansheeCore)

# IDE specific
set_property(TARGET BansheeD3D9RenderAPI PROPERTY FOLDER Plugins)