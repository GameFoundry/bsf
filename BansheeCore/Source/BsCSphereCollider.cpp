//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#include "BsCSphereCollider.h"
#include "BsSceneObject.h"
#include "BsCRigidbody.h"
#include "BsCSphereColliderRTTI.h"

namespace BansheeEngine
{
	CSphereCollider::CSphereCollider(const HSceneObject& parent, float radius)
		: CCollider(parent), mRadius(radius)
	{
		setName("BoxCollider");
	}

	void CSphereCollider::setRadius(float radius)
	{
		if (mRadius == radius)
			return;

		mRadius = radius; 

		if (mInternal != nullptr)
		{
			_getInternal()->setRadius(radius);

			if (mParent != nullptr)
				mParent->_updateMassDistribution();
		}
	}

	void CSphereCollider::setCenter(const Vector3& center)
	{
		if (mLocalPosition == center)
			return;

		mLocalPosition = center; 
		
		if (mInternal != nullptr)
			updateTransform();
	}

	SPtr<Collider> CSphereCollider::createInternal()
	{
		SPtr<Collider> collider = SphereCollider::create(mRadius, SO()->getWorldPosition(), SO()->getWorldRotation());

		collider->_setOwner(PhysicsOwnerType::Component, this);
		return collider;
	}

	RTTITypeBase* CSphereCollider::getRTTIStatic()
	{
		return CSphereColliderRTTI::instance();
	}

	RTTITypeBase* CSphereCollider::getRTTI() const
	{
		return CSphereCollider::getRTTIStatic();
	}
}