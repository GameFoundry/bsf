//************************************ bs::framework - Copyright 2018 Marko Pintera **************************************//
//*********** Licensed under the MIT license. See LICENSE.md for full terms. This notice is not to be removed. ***********//
#include "Physics/BsRigidbody.h"
#include "Physics/BsPhysics.h"
#include "Scene/BsSceneObject.h"

namespace bs
{
	Rigidbody::Rigidbody(const HSceneObject& linkedSO)
		:mLinkedSO(linkedSO)
	{

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