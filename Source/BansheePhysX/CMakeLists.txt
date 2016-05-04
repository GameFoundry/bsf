# Source files and their filters
include(CMakeSources.cmake)

# Includes
set(BansheePhysX_INC 
	"Include"
	"../../Dependencies/BansheePhysX/Include"
	"../BansheeUtility/Include" 
	"../BansheeCore/Include")

include_directories(${BansheePhysX_INC})	
	
# Target
add_library(BansheePhysX SHARED ${BS_BANSHEEPHYSX_SRC})

# Defines
target_compile_definitions(BansheePhysX PRIVATE -DBS_PHYSX_EXPORTS)

# Libraries
if(BS_64BIT)
	set(BS_PHYSX_SUFFIX _x64)
else()
	set(BS_PHYSX_SUFFIX _x86)
endif()

## External lib: PhysX
add_library_per_config_suffix(BansheePhysX PhysX3 ${BS_PHYSX_SUFFIX} CHECKED${BS_PHYSX_SUFFIX})
add_library_per_config_suffix(BansheePhysX PhysX3Common ${BS_PHYSX_SUFFIX} CHECKED${BS_PHYSX_SUFFIX})
add_library_per_config_suffix(BansheePhysX PhysX3Cooking ${BS_PHYSX_SUFFIX} CHECKED${BS_PHYSX_SUFFIX})	
add_library_per_config_suffix(BansheePhysX PhysX3CharacterKinematic ${BS_PHYSX_SUFFIX} CHECKED${BS_PHYSX_SUFFIX})
add_library_per_config_suffix(BansheePhysX PhysX3Extensions "" CHECKED)

## Local libs
target_link_libraries(BansheePhysX PUBLIC BansheeUtility BansheeCore)

# IDE specific
set_property(TARGET BansheePhysX PROPERTY FOLDER Plugins)