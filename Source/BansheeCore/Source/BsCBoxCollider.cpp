//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#include "BsCBoxCollider.h"
#include "BsSceneObject.h"
#include "BsCRigidbody.h"
#include "BsCBoxColliderRTTI.h"

namespace BansheeEngine
{
	CBoxCollider::CBoxCollider(const HSceneObject& parent, const Vector3& extents)
		: CCollider(parent), mExtents(extents)
	{
		setName("BoxCollider");
	}

	void CBoxCollider::setExtents(const Vector3& extents)
	{
		if (mExtents == extents)
			return;

		mExtents = extents; 

		if (mInternal != nullptr)
		{
			_getInternal()->setExtents(extents);

			if (mParent != nullptr)
				mParent->_updateMassDistribution();
		}
	}

	void CBoxCollider::setCenter(const Vector3& center)
	{
		if (mLocalPosition == center)
			return;

		mLocalPosition = center;

		if (mInternal != nullptr)
			updateTransform();
	}

	SPtr<Collider> CBoxCollider::createInternal()
	{
		SPtr<Collider> collider = BoxCollider::create(mExtents, SO()->getWorldPosition(), SO()->getWorldRotation());
		collider->_setOwner(PhysicsOwnerType::Component, this);

		return collider;
	}

	RTTITypeBase* CBoxCollider::getRTTIStatic()
	{
		return CBoxColliderRTTI::instance();
	}

	RTTITypeBase* CBoxCollider::getRTTI() const
	{
		return CBoxCollider::getRTTIStatic();
	}
}