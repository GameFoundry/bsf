#include "BsPhysXBoxCollider.h"
#include "BsPhysX.h"
#include "PxPhysics.h"
#include "BsFPhysXCollider.h"

using namespace physx;

namespace BansheeEngine
{
	PhysXBoxCollider::PhysXBoxCollider(PxPhysics* physx, const Vector3& position, const Quaternion& rotation, 
		const Vector3& extents)
	{
		PxBoxGeometry geometry(extents.x, extents.y, extents.z);

		PxShape* shape = physx->createShape(geometry, *gPhysX().getDefaultMaterial(), true);
		shape->setLocalPose(toPxTransform(position, rotation));
		shape->userData = this;

		mInternal = bs_new<FPhysXCollider>(shape);
	}

	PhysXBoxCollider::~PhysXBoxCollider()
	{
		bs_delete(mInternal);
	}

	void PhysXBoxCollider::setExtents(const Vector3& extents)
	{
		PxBoxGeometry geometry(extents.x, extents.y, extents.z);

		getInternal()->_getShape()->setGeometry(geometry);
	}

	Vector3 PhysXBoxCollider::getExtents() const
	{
		return fromPxVector(getInternal()->_getShape()->getGeometry().box().halfExtents);
	}

	FPhysXCollider* PhysXBoxCollider::getInternal() const
	{
		return static_cast<FPhysXCollider*>(mInternal);
	}
}