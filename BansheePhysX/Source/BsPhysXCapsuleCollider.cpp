#include "BsPhysXCapsuleCollider.h"
#include "BsPhysX.h"
#include "PxPhysics.h"
#include "BsFPhysXCollider.h"

using namespace physx;

namespace BansheeEngine
{
	PhysXCapsuleCollider::PhysXCapsuleCollider(PxPhysics* physx, const Vector3& position, const Quaternion& rotation,
		float radius, float halfHeight)
	{
		PxCapsuleGeometry geometry(radius, halfHeight);

		PxShape* shape = physx->createShape(geometry, *gPhysX().getDefaultMaterial(), true);
		shape->setLocalPose(toPxTransform(position, rotation));

		mInternal = bs_new<FPhysXCollider>(shape);
	}

	PhysXCapsuleCollider::~PhysXCapsuleCollider()
	{
		bs_delete(mInternal);
	}

	FPhysXCollider* PhysXCapsuleCollider::getInternal() const
	{
		return static_cast<FPhysXCollider*>(mInternal);
	}
}