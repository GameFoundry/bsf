//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#pragma once

#include "BsCorePrerequisites.h"
#include "BsCollider.h"
#include "BsVector3.h"
#include "BsQuaternion.h"

namespace BansheeEngine
{
	class BS_CORE_EXPORT CapsuleCollider : public Collider
	{
	public:
		CapsuleCollider();

		static SPtr<CapsuleCollider> create(float radius = 0.0f, float halfHeight = 0.0f, 
			const Vector3& position = Vector3::ZERO, const Quaternion& rotation = Quaternion::IDENTITY);
	};
}