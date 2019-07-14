//************************************ bs::framework - Copyright 2018 Marko Pintera **************************************//
//*********** Licensed under the MIT license. See LICENSE.md for full terms. This notice is not to be removed. ***********//
#include "BsNullPhysicsRigidbody.h"
#include "Physics/BsCollider.h"
#include "BsNullPhysicsColliders.h"
#include "Scene/BsSceneObject.h"
#include "Physics/BsPhysics.h"
#include "BsNullPhysics.h"

namespace bs
{
	NullPhysicsRigidbody::NullPhysicsRigidbody(const HSceneObject& linkedSO)
		:Rigidbody(linkedSO)
	{
		mPosition = linkedSO->getTransform().getPosition();
		mRotation = linkedSO->getTransform().getRotation();
	}

	void NullPhysicsRigidbody::move(const Vector3& position)
	{
		setTransform(position, getRotation());
	}

	void NullPhysicsRigidbody::rotate(const Quaternion& rotation)
	{
		setTransform(getPosition(), rotation);
	}

	Vector3 NullPhysicsRigidbody::getPosition() const
	{
		return mPosition;
	}

	Quaternion NullPhysicsRigidbody::getRotation() const
	{
		return mRotation;
	}

	void NullPhysicsRigidbody::setTransform(const Vector3& pos, const Quaternion& rot)
	{
		mPosition = pos;
		mRotation = rot;
	}

	void NullPhysicsRigidbody::setCenterOfMass(const class Vector3& position, const Quaternion& rotation)
	{
		mCenterOfMassPosition = position;
		mCenterOfMassRotation = rotation;
	}
}
