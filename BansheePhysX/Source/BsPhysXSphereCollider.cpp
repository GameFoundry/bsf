#include "BsPhysXSphereCollider.h"
#include "BsPhysX.h"
#include "PxPhysics.h"
#include "BsFPhysXCollider.h"

using namespace physx;

namespace BansheeEngine
{
	PhysXSphereCollider::PhysXSphereCollider(PxPhysics* physx, const Vector3& position, const Quaternion& rotation,
		float radius)
	{
		PxSphereGeometry geometry(radius);

		PxShape* shape = physx->createShape(geometry, *gPhysX().getDefaultMaterial(), true);
		shape->setLocalPose(toPxTransform(position, rotation));
		shape->userData = this;

		mInternal = bs_new<FPhysXCollider>(shape);
	}

	PhysXSphereCollider::~PhysXSphereCollider()
	{
		bs_delete(mInternal);
	}

	void PhysXSphereCollider::setRadius(float radius)
	{
		PxSphereGeometry geometry(radius);

		getInternal()->_getShape()->setGeometry(geometry);
	}

	float PhysXSphereCollider::getRadius() const
	{
		return getInternal()->_getShape()->getGeometry().sphere().radius;
	}

	FPhysXCollider* PhysXSphereCollider::getInternal() const
	{
		return static_cast<FPhysXCollider*>(mInternal);
	}
}