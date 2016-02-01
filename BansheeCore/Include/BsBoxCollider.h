//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#pragma once

#include "BsCorePrerequisites.h"
#include "BsCollider.h"
#include "BsVector3.h"
#include "BsQuaternion.h"

namespace BansheeEngine
{
	class BS_CORE_EXPORT BoxCollider : public Collider
	{
	public:
		BoxCollider();

		static SPtr<BoxCollider> create(float extentX = 0.0f, float extentY = 0.0f, float extentZ = 0.0f, 
			const Vector3& position = Vector3::ZERO, const Quaternion& rotation = Quaternion::IDENTITY);
	};
}