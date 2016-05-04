set(BS_BANSHEEPHYSX_INC_NOFILTER
	"Include/BsPhysXPrerequisites.h"
	"Include/BsPhysX.h"
	"Include/BsPhysXMaterial.h"
	"Include/BsPhysXRigidbody.h"
	"Include/BsPhysXBoxCollider.h"
	"Include/BsFPhysXCollider.h"
	"Include/BsPhysXSphereCollider.h"
	"Include/BsPhysXPlaneCollider.h"
	"Include/BsPhysXCapsuleCollider.h"
	"Include/BsPhysXMesh.h"
	"Include/BsPhysXMeshCollider.h"
	"Include/BsFPhysXJoint.h"
	"Include/BsPhysXFixedJoint.h"
	"Include/BsPhysXDistanceJoint.h"
	"Include/BsPhysXHingeJoint.h"
	"Include/BsPhysXSliderJoint.h"
	"Include/BsPhysXSphericalJoint.h"
	"Include/BsPhysXD6Joint.h"
	"Include/BsPhysXCharacterController.h"
)

set(BS_BANSHEEPHYSX_SRC_NOFILTER
	"Source/BsPhysXPlugin.cpp"
	"Source/BsPhysX.cpp"
	"Source/BsPhysXMaterial.cpp"
	"Source/BsPhysXRigidbody.cpp"
	"Source/BsPhysXBoxCollider.cpp"
	"Source/BsFPhysXCollider.cpp"
	"Source/BsPhysXSphereCollider.cpp"
	"Source/BsPhysXPlaneCollider.cpp"
	"Source/BsPhysXCapsuleCollider.cpp"
	"Source/BsPhysXMesh.cpp"
	"Source/BsPhysXMeshCollider.cpp"
	"Source/BsFPhysXJoint.cpp"
	"Source/BsPhysXFixedJoint.cpp"
	"Source/BsPhysXDistanceJoint.cpp"
	"Source/BsPhysXHingeJoint.cpp"
	"Source/BsPhysXSliderJoint.cpp"
	"Source/BsPhysXSphericalJoint.cpp"
	"Source/BsPhysXD6Joint.cpp"
	"Source/BsPhysXCharacterController.cpp"
)

set(BS_BANSHEEPHYSX_INC_RTTI
	"Include/BsPhysXMeshRTTI.h"
)

source_group("Header Files" FILES ${BS_BANSHEEPHYSX_INC_NOFILTER})
source_group("Source Files" FILES ${BS_BANSHEEPHYSX_SRC_NOFILTER})
source_group("Header Files\\RTTI" FILES ${BS_BANSHEEPHYSX_INC_RTTI})

set(BS_BANSHEEPHYSX_SRC
	${BS_BANSHEEPHYSX_INC_NOFILTER}
	${BS_BANSHEEPHYSX_SRC_NOFILTER}
	${BS_BANSHEEPHYSX_INC_RTTI}
)