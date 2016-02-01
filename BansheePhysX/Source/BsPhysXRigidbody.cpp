#include "BsPhysXRigidbody.h"
#include "PxRigidDynamic.h"
#include "PxScene.h"

using namespace physx;

namespace BansheeEngine
{
	PhysXRigidbody::PhysXRigidbody(PxPhysics* physx, PxScene* scene, const Vector3& position, const Quaternion& rotation)
	{
		PxTransform tfrm = toPxTransform(position, rotation);

		mInternal = physx->createRigidDynamic(tfrm);
		scene->addActor(*mInternal);
	}

	PhysXRigidbody::~PhysXRigidbody()
	{
		// TODO - Remove from scene? Or is that part of release()?
		mInternal->release();
	}
}