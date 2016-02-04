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
		shape->userData = this;

		mInternal = bs_new<FPhysXCollider>(shape);
	}

	PhysXCapsuleCollider::~PhysXCapsuleCollider()
	{
		bs_delete(mInternal);
	}

	void PhysXCapsuleCollider::setHalfHeight(float halfHeight)
	{
		PxCapsuleGeometry geometry(getRadius(), halfHeight);

		getInternal()->_getShape()->setGeometry(geometry);
	}

	float PhysXCapsuleCollider::getHalfHeight() const
	{
		return getInternal()->_getShape()->getGeometry().capsule().halfHeight;
	}

	void PhysXCapsuleCollider::setRadius(float radius)
	{
		PxCapsuleGeometry geometry(radius, getHalfHeight());

		getInternal()->_getShape()->setGeometry(geometry);
	}

	float PhysXCapsuleCollider::getRadius() const
	{
		return getInternal()->_getShape()->getGeometry().capsule().radius;
	}

	FPhysXCollider* PhysXCapsuleCollider::getInternal() const
	{
		return static_cast<FPhysXCollider*>(mInternal);
	}
}