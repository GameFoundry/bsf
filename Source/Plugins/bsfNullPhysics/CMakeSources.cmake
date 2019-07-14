set(BS_NULL_PHYSICS_INC_NOFILTER
	"BsNullPhysicsPrerequisites.h"
	"BsNullPhysics.h"
	"BsNullPhysicsMaterial.h"
	"BsNullPhysicsRigidbody.h"
	"BsNullPhysicsColliders.h"
	"BsNullPhysicsMesh.h"
	"BsNullPhysicsJoints.h"
	"BsNullPhysicsCharacterController.h"
)

set(BS_NULL_PHYSICS_SRC_NOFILTER
	"BsNullPhysicsPlugin.cpp"
	"BsNullPhysics.cpp"
	"BsNullPhysicsMaterial.cpp"
	"BsNullPhysicsRigidbody.cpp"
	"BsNullPhysicsColliders.cpp"
	"BsNullPhysicsMesh.cpp"
	"BsNullPhysicsJoints.cpp"
	"BsNullPhysicsCharacterController.cpp"
)

set(BS_NULL_PHYSICS_INC_RTTI
	"RTTI/BsNullPhysicsMeshRTTI.h"
)

if(WIN32)
	set(BS_NULL_PHYSICS_WIN32RES
	"BsNullPhysicsWin32Resource.rc"
)
else()
	set(BS_NULL_PHYSICS_WIN32RES )
endif()

source_group("" FILES ${BS_NULL_PHYSICS_INC_NOFILTER} ${BS_NULL_PHYSICS_SRC_NOFILTER} ${BS_NULL_PHYSICS_WIN32RES})
source_group("RTTI" FILES ${BS_NULL_PHYSICS_INC_RTTI})

set(BS_NULL_PHYSICS_SRC
	${BS_NULL_PHYSICS_INC_NOFILTER}
	${BS_NULL_PHYSICS_SRC_NOFILTER}
	${BS_NULL_PHYSICS_INC_RTTI}
	${BS_NULL_PHYSICS_WIN32RES}
)
