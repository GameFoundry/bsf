//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#include "BsCCapsuleCollider.h"
#include "BsSceneObject.h"
#include "BsCRigidbody.h"
#include "BsCCapsuleColliderRTTI.h"

namespace BansheeEngine
{
	CCapsuleCollider::CCapsuleCollider(const HSceneObject& parent, float radius, float halfHeight)
		: CCollider(parent), mRadius(radius), mHalfHeight(halfHeight)
	{
		setName("CapsuleCollider");
	}

	void CCapsuleCollider::setNormal(const Vector3& normal)
	{
		if (mNormal == normal)
			return;

		mNormal = normal;
		mNormal.normalize();

		mLocalRotation = Quaternion::getRotationFromTo(Vector3::UNIT_X, normal);

		if (mInternal != nullptr)
			updateTransform();
	}

	void CCapsuleCollider::setCenter(const Vector3& center)
	{
		if (mLocalPosition == center)
			return;

		mLocalPosition = center;

		if (mInternal != nullptr)
			updateTransform();
	}

	void CCapsuleCollider::setHalfHeight(float halfHeight)
	{
		if (mHalfHeight == halfHeight)
			return;

		mHalfHeight = halfHeight;

		if (mInternal != nullptr)
		{
			_getInternal()->setHalfHeight(halfHeight);

			if (mParent != nullptr)
				mParent->_updateMassDistribution();
		}
	}

	void CCapsuleCollider::setRadius(float radius)
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

	SPtr<Collider> CCapsuleCollider::createInternal()
	{
		return CapsuleCollider::create(mRadius, mHalfHeight, SO()->getWorldPosition(), SO()->getWorldRotation());
	}

	RTTITypeBase* CCapsuleCollider::getRTTIStatic()
	{
		return CCapsuleColliderRTTI::instance();
	}

	RTTITypeBase* CCapsuleCollider::getRTTI() const
	{
		return CCapsuleCollider::getRTTIStatic();
	}
}