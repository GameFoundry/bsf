# Source files and their filters
include(CMakeSources.cmake)

# Packages
if(RENDER_API_MODULE MATCHES "Vulkan" OR INCLUDE_ALL_IN_WORKFLOW)
	find_package(Vulkan)
	find_package(glslang)
endif()

# Includes
set(BansheeVulkanRenderAPI_INC 
	"./" 
	"../BansheeUtility" 
	"../BansheeCore"
	${Vulkan_INCLUDE_DIR}
	${glslang_INCLUDE_DIR})

if(WIN32)
	set(BansheeVulkanRenderAPI_INC ${BansheeVulkanRenderAPI_INC} "Source/Win32")
else()
# TODO_OTHER_PLATFORMS_GO_HERE
endif()	
	
include_directories(${BansheeVulkanRenderAPI_INC})	
	
# Target
add_library(BansheeVulkanRenderAPI SHARED ${BS_BANSHEEVULKANRENDERAPI_SRC})

# Defines
target_compile_definitions(BansheeVulkanRenderAPI PRIVATE -DBS_VULKAN_EXPORTS)

# Libraries	
## External libs: Vulkan, glslang
target_link_libraries(BansheeVulkanRenderAPI PRIVATE ${Vulkan_LIBRARIES})
target_link_libraries(BansheeVulkanRenderAPI PRIVATE ${glslang_LIBRARIES})

## Local libs
target_link_libraries(BansheeVulkanRenderAPI PRIVATE BansheeUtility BansheeCore)

# IDE specific
set_property(TARGET BansheeVulkanRenderAPI PROPERTY FOLDER Plugins)