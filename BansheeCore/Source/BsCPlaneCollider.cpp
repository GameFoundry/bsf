//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#include "BsCPlaneCollider.h"
#include "BsSceneObject.h"
#include "BsCPlaneColliderRTTI.h"

namespace BansheeEngine
{
	CPlaneCollider::CPlaneCollider(const HSceneObject& parent)
		: CCollider(parent)
	{
		setName("PlaneCollider");
	}

	void CPlaneCollider::setNormal(const Vector3& normal)
	{
		if (mNormal == normal)
			return;

		mNormal = normal;
		mNormal.normalize();
		
		mLocalRotation = Quaternion::getRotationFromTo(Vector3::UNIT_X, normal);
		mLocalPosition = mNormal * mDistance;

		if(mInternal != nullptr)
			updateTransform();
	}

	void CPlaneCollider::setDistance(float distance)
	{
		if (mDistance == distance)
			return;

		mDistance = distance; 
		mLocalPosition = mNormal * distance;

		if (mInternal != nullptr)
			updateTransform();
	}

	SPtr<Collider> CPlaneCollider::createInternal()
	{
		return PlaneCollider::create(SO()->getWorldPosition(), SO()->getWorldRotation());
	}

	RTTITypeBase* CPlaneCollider::getRTTIStatic()
	{
		return CPlaneColliderRTTI::instance();
	}

	RTTITypeBase* CPlaneCollider::getRTTI() const
	{
		return CPlaneCollider::getRTTIStatic();
	}
}