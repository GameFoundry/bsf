# Find PhysX
#
# This module defines
#  PhysX_INCLUDE_DIRS
#  PhysX_LIBRARIES
#  PhysX_FOUND

start_find_package(PhysX)

# Always use bundled library as it is not commonly available
set(PhysX_INSTALL_DIR ${BSF_SOURCE_DIR}/../Dependencies/PhysX CACHE PATH "")
gen_default_lib_search_dirs(PhysX)

if(NOT APPLE)
	if(BS_64BIT)
		set(BS_PHYSX_SUFFIX _64)
	else()
		set(BS_PHYSX_SUFFIX _86)
	endif()
endif()

find_imported_includes(PhysX PxPhysics.h)
if(NOT APPLE)
	find_imported_library_shared2(PhysX PhysX${BS_PHYSX_SUFFIX} PhysXCHECKED${BS_PHYSX_SUFFIX})
	find_imported_library_shared2(PhysX PhysXCommon${BS_PHYSX_SUFFIX} PhysXCommonCHECKED${BS_PHYSX_SUFFIX})
	find_imported_library_shared2(PhysX PhysXCooking${BS_PHYSX_SUFFIX} PhysXCookingCHECKED${BS_PHYSX_SUFFIX})
	find_imported_library_shared2(PhysX PhysXCharacterKinematic_static${BS_PHYSX_SUFFIX} PhysXCharacterKinematicCHECKED_static${BS_PHYSX_SUFFIX})
	find_imported_library2(PhysX PhysXExtensions_static${BS_PHYSX_SUFFIX} PhysXExtensionsCHECKED_static${BS_PHYSX_SUFFIX})
else()
	find_imported_library(PhysX LowLevel)
	find_imported_library(PhysX LowLevelCloth)
	find_imported_library(PhysX PhysX)
	find_imported_library(PhysX PhysXCommon)
	find_imported_library(PhysX PhysXCooking)
	find_imported_library(PhysX PhysXCharacterKinematic)
	find_imported_library(PhysX PhysXExtensions)
	find_imported_library(PhysX PhysXProfileSDK)
	find_imported_library(PhysX PvdRuntime)
	find_imported_library(PhysX PxTask)
	find_imported_library(PhysX SceneQuery)
	find_imported_library(PhysX SimulationController)
endif()

install_dependency_binaries(PhysX)

end_find_package(PhysX PhysX${BS_PHYSX_SUFFIX})
