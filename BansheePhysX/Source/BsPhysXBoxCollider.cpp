//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#include "BsPhysXBoxCollider.h"
#include "BsPhysX.h"
#include "PxPhysics.h"
#include "BsFPhysXCollider.h"

using namespace physx;

namespace BansheeEngine
{
	PhysXBoxCollider::PhysXBoxCollider(PxPhysics* physx, const Vector3& position, const Quaternion& rotation, 
		const Vector3& extents)
		:mExtents(extents)
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

	void PhysXBoxCollider::setScale(const Vector3& scale)
	{
		BoxCollider::setScale(scale);
		applyGeometry();
	}

	void PhysXBoxCollider::setExtents(const Vector3& extents)
	{
		mExtents = extents;
		applyGeometry();
	}

	Vector3 PhysXBoxCollider::getExtents() const
	{
		return mExtents;
	}

	void PhysXBoxCollider::applyGeometry()
	{
		PxBoxGeometry geometry(mExtents.x * mScale.x, mExtents.y * mScale.y, mExtents.z * mScale.z);

		getInternal()->_getShape()->setGeometry(geometry);
	}

	FPhysXCollider* PhysXBoxCollider::getInternal() const
	{
		return static_cast<FPhysXCollider*>(mInternal);
	}
}