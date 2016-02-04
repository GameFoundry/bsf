//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#include "BsCRigidbody.h"
#include "BsSceneObject.h"
#include "BsCRigidbodyRTTI.h"

namespace BansheeEngine
{
	CRigidbody::CRigidbody(const HSceneObject& parent)
		: Component(parent)
	{
		setName("Rigidbody");
	}

	RTTITypeBase* CRigidbody::getRTTIStatic()
	{
		return CRigidbodyRTTI::instance();
	}

	RTTITypeBase* CRigidbody::getRTTI() const
	{
		return CRigidbody::getRTTIStatic();
	}
}