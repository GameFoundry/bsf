# Find PhysX
#
# This module defines
#  PhysX_INCLUDE_DIRS
#  PhysX_LIBRARIES
#  PhysX_FOUND

start_find_package(PhysX)

set(PhysX_INSTALL_DIR ${PROJECT_SOURCE_DIR}/../Dependencies/PhysX CACHE PATH "")
gen_default_lib_search_dirs(PhysX)

if(BS_64BIT)
	set(BS_PHYSX_SUFFIX _x64)
else()
	set(BS_PHYSX_SUFFIX _x86)
endif()

find_imported_includes(PhysX PxPhysics.h)
find_imported_library_shared2(PhysX PhysX3${BS_PHYSX_SUFFIX} PhysX3CHECKED${BS_PHYSX_SUFFIX})
find_imported_library_shared2(PhysX PhysX3Common${BS_PHYSX_SUFFIX} PhysX3CommonCHECKED${BS_PHYSX_SUFFIX})
find_imported_library_shared2(PhysX PhysX3Cooking${BS_PHYSX_SUFFIX} PhysX3CookingCHECKED${BS_PHYSX_SUFFIX})
find_imported_library_shared2(PhysX PhysX3CharacterKinematic${BS_PHYSX_SUFFIX} PhysX3CharacterKinematicCHECKED${BS_PHYSX_SUFFIX})
find_imported_library2(PhysX PhysX3Extensions PhysX3ExtensionsCHECKED)

end_find_package(PhysX PhysX3${BS_PHYSX_SUFFIX})
