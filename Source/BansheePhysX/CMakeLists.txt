# Source files and their filters
include(CMakeSources.cmake)

# Includes
set(BansheePhysX_INC 
	"Include"
	"Dependencies/Include"
	"../BansheeUtility/Include" 
	"../BansheeCore/Include")

include_directories(${BansheePhysX_INC})	
	
# Target
add_library(BansheePhysX SHARED ${BS_BANSHEEPHYSX_SRC})

# Defines
target_compile_definitions(BansheePhysX PRIVATE -DBS_PHYSX_EXPORTS)

# Libraries
if(BS_64BIT)
	set(BS_LOCAL_LIB_SUFFIX _x64)
	set(BS_LOCAL_LIB_PATH ${CMAKE_CURRENT_SOURCE_DIR}/Dependencies/lib/x64)
else()
	set(BS_LOCAL_LIB_SUFFIX _x86)
	set(BS_LOCAL_LIB_PATH ${CMAKE_CURRENT_SOURCE_DIR}/Dependencies/lib/x86)
endif()

## External lib: PhysX
add_library(PhysX STATIC IMPORTED)
set_target_properties(PhysX PROPERTIES IMPORTED_LOCATION_DEBUG ${BS_LOCAL_LIB_PATH}/Debug/PhysX3CHECKED${BS_LOCAL_LIB_SUFFIX}${BS_LIBRARY_EXTENSION})
set_target_properties(PhysX PROPERTIES IMPORTED_LOCATION_OPTIMIZEDDEBUG ${BS_LOCAL_LIB_PATH}/OptimizedDebug/PhysX3${BS_LOCAL_LIB_SUFFIX}${BS_LIBRARY_EXTENSION})
set_target_properties(PhysX PROPERTIES IMPORTED_LOCATION_RELEASE ${BS_LOCAL_LIB_PATH}/Release/PhysX3${BS_LOCAL_LIB_SUFFIX}${BS_LIBRARY_EXTENSION})	
	
add_library(PhysXCommon STATIC IMPORTED)
set_target_properties(PhysXCommon PROPERTIES IMPORTED_LOCATION_DEBUG ${BS_LOCAL_LIB_PATH}/Debug/PhysX3CommonCHECKED${BS_LOCAL_LIB_SUFFIX}${BS_LIBRARY_EXTENSION})
set_target_properties(PhysXCommon PROPERTIES IMPORTED_LOCATION_OPTIMIZEDDEBUG ${BS_LOCAL_LIB_PATH}/OptimizedDebug/PhysX3Common${BS_LOCAL_LIB_SUFFIX}${BS_LIBRARY_EXTENSION})
set_target_properties(PhysXCommon PROPERTIES IMPORTED_LOCATION_RELEASE ${BS_LOCAL_LIB_PATH}/Release/PhysX3Common${BS_LOCAL_LIB_SUFFIX}${BS_LIBRARY_EXTENSION})

add_library(PhysXCooking STATIC IMPORTED)
set_target_properties(PhysXCooking PROPERTIES IMPORTED_LOCATION_DEBUG ${BS_LOCAL_LIB_PATH}/Debug/PhysX3CookingCHECKED${BS_LOCAL_LIB_SUFFIX}${BS_LIBRARY_EXTENSION})
set_target_properties(PhysXCooking PROPERTIES IMPORTED_LOCATION_OPTIMIZEDDEBUG ${BS_LOCAL_LIB_PATH}/OptimizedDebug/PhysX3Cooking${BS_LOCAL_LIB_SUFFIX}${BS_LIBRARY_EXTENSION})
set_target_properties(PhysXCooking PROPERTIES IMPORTED_LOCATION_RELEASE ${BS_LOCAL_LIB_PATH}/Release/PhysX3Cooking${BS_LOCAL_LIB_SUFFIX}${BS_LIBRARY_EXTENSION})

add_library(PhysXCharacterKinematic STATIC IMPORTED)
set_target_properties(PhysXCharacterKinematic PROPERTIES IMPORTED_LOCATION_DEBUG ${BS_LOCAL_LIB_PATH}/Debug/PhysX3CharacterKinematicCHECKED${BS_LOCAL_LIB_SUFFIX}${BS_LIBRARY_EXTENSION})
set_target_properties(PhysXCharacterKinematic PROPERTIES IMPORTED_LOCATION_OPTIMIZEDDEBUG ${BS_LOCAL_LIB_PATH}/OptimizedDebug/PhysX3CharacterKinematic${BS_LOCAL_LIB_SUFFIX}${BS_LIBRARY_EXTENSION})
set_target_properties(PhysXCharacterKinematic PROPERTIES IMPORTED_LOCATION_RELEASE ${BS_LOCAL_LIB_PATH}/Release/PhysX3CharacterKinematic${BS_LOCAL_LIB_SUFFIX}${BS_LIBRARY_EXTENSION})

add_library(PhysXExtensions STATIC IMPORTED)
set_target_properties(PhysXExtensions PROPERTIES IMPORTED_LOCATION_DEBUG ${BS_LOCAL_LIB_PATH}/Debug/PhysX3ExtensionsCHECKED${BS_LIBRARY_EXTENSION})
set_target_properties(PhysXExtensions PROPERTIES IMPORTED_LOCATION_OPTIMIZEDDEBUG ${BS_LOCAL_LIB_PATH}/OptimizedDebug/PhysX3Extensions${BS_LIBRARY_EXTENSION})
set_target_properties(PhysXExtensions PROPERTIES IMPORTED_LOCATION_RELEASE ${BS_LOCAL_LIB_PATH}/Release/PhysX3Extensions${BS_LIBRARY_EXTENSION})
	
target_link_libraries(BansheePhysX PhysX PhysXCommon PhysXCooking PhysXCharacterKinematic PhysXExtensions)	

## Local libs
target_link_libraries(BansheePhysX BansheeUtility BansheeCore)

# IDE specific
set_property(TARGET BansheePhysX PROPERTY FOLDER Plugins)