#include "BsPhysXRigidbody.h"
#include "PxRigidDynamic.h"

using namespace physx;

namespace BansheeEngine
{
	PhysXRigidbody::PhysXRigidbody(PxPhysics* physx, const Vector3& position, const Quaternion& rotation)
	{
		PxTransform tfrm(toPxVector(position), toPxQuaternion(rotation));

		mInternal = physx->createRigidDynamic(tfrm);
	}

	PhysXRigidbody::~PhysXRigidbody()
	{
		mInternal->release();
	}
}