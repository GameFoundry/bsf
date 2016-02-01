#include "BsPhysXBoxCollider.h"
#include "BsPhysX.h"
#include "PxPhysics.h"
#include "BsFPhysXCollider.h"

using namespace physx;

namespace BansheeEngine
{
	PhysXBoxCollider::PhysXBoxCollider(PxPhysics* physx, const Vector3& position, const Quaternion& rotation, 
		float extentX, float extentY, float extentZ)
	{
		PxBoxGeometry geometry(extentX, extentY, extentZ);

		PxShape* shape = physx->createShape(geometry, *gPhysX().getDefaultMaterial(), true);
		shape->setLocalPose(toPxTransform(position, rotation));

		mInternal = bs_new<FPhysXCollider>(shape);
	}

	PhysXBoxCollider::~PhysXBoxCollider()
	{
		bs_delete(mInternal);
	}

	FPhysXCollider* PhysXBoxCollider::getInternal() const
	{
		return static_cast<FPhysXCollider*>(mInternal);
	}
}