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

		virtual void setExtents(const Vector3& extents) = 0;
		virtual Vector3 getExtents() const = 0;

		static SPtr<BoxCollider> create(const Vector3& extents = Vector3::ZERO, 
			const Vector3& position = Vector3::ZERO, const Quaternion& rotation = Quaternion::IDENTITY);
	};
}