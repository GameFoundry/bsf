set(BS_BANSHEEPHYSX_INC_NOFILTER
	"BsPhysXPrerequisites.h"
	"BsPhysX.h"
	"BsPhysXMaterial.h"
	"BsPhysXRigidbody.h"
	"BsPhysXBoxCollider.h"
	"BsFPhysXCollider.h"
	"BsPhysXSphereCollider.h"
	"BsPhysXPlaneCollider.h"
	"BsPhysXCapsuleCollider.h"
	"BsPhysXMesh.h"
	"BsPhysXMeshCollider.h"
	"BsFPhysXJoint.h"
	"BsPhysXFixedJoint.h"
	"BsPhysXDistanceJoint.h"
	"BsPhysXHingeJoint.h"
	"BsPhysXSliderJoint.h"
	"BsPhysXSphericalJoint.h"
	"BsPhysXD6Joint.h"
	"BsPhysXCharacterController.h"
)

set(BS_BANSHEEPHYSX_SRC_NOFILTER
	"BsPhysXPlugin.cpp"
	"BsPhysX.cpp"
	"BsPhysXMaterial.cpp"
	"BsPhysXRigidbody.cpp"
	"BsPhysXBoxCollider.cpp"
	"BsFPhysXCollider.cpp"
	"BsPhysXSphereCollider.cpp"
	"BsPhysXPlaneCollider.cpp"
	"BsPhysXCapsuleCollider.cpp"
	"BsPhysXMesh.cpp"
	"BsPhysXMeshCollider.cpp"
	"BsFPhysXJoint.cpp"
	"BsPhysXFixedJoint.cpp"
	"BsPhysXDistanceJoint.cpp"
	"BsPhysXHingeJoint.cpp"
	"BsPhysXSliderJoint.cpp"
	"BsPhysXSphericalJoint.cpp"
	"BsPhysXD6Joint.cpp"
	"BsPhysXCharacterController.cpp"
)

set(BS_BANSHEEPHYSX_INC_RTTI
	"RTTI/BsPhysXMeshRTTI.h"
)

source_group("Header Files" FILES ${BS_BANSHEEPHYSX_INC_NOFILTER})
source_group("Source Files" FILES ${BS_BANSHEEPHYSX_SRC_NOFILTER})
source_group("Header Files\\RTTI" FILES ${BS_BANSHEEPHYSX_INC_RTTI})

set(BS_BANSHEEPHYSX_SRC
	${BS_BANSHEEPHYSX_INC_NOFILTER}
	${BS_BANSHEEPHYSX_SRC_NOFILTER}
	${BS_BANSHEEPHYSX_INC_RTTI}
)