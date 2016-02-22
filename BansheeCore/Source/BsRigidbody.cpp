//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#include "BsRigidbody.h"
#include "BsPhysics.h"
#include "BsFCollider.h"
#include "BsSceneObject.h"
#include "BsUtility.h"

namespace BansheeEngine
{
	Rigidbody::Rigidbody(const HSceneObject& linkedSO)
		:mLinkedSO(linkedSO)
	{
		mPriority = Utility::getSceneObjectDepth(linkedSO);
		gPhysics().registerRigidbody(this, mPriority);
	}

	Rigidbody::~Rigidbody()
	{
		gPhysics().unregisterRigidbody(mPhysicsId, mPriority);
	}

	void Rigidbody::_setPriority(UINT32 priority)
	{
		gPhysics().updatePriority(mPhysicsId, mPriority, priority);

		mPriority = priority;
	}

	void Rigidbody::_setTransform(const Vector3& position, const Quaternion& rotation)
	{
		mLinkedSO->setWorldPosition(position);
		mLinkedSO->setWorldRotation(rotation);
	}

	SPtr<Rigidbody> Rigidbody::create(const HSceneObject& linkedSO)
	{
		return gPhysics().createRigidbody(linkedSO);
	}
}