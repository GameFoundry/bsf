//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#pragma once

#include "BsCorePrerequisites.h"
#include "BsVector3.h"

namespace BansheeEngine
{
	struct ContactPoint
	{
		Vector3 point;
		Vector3 normal;
	};

	struct CollisionData
	{
		SPtr<Collider> collider;
		Vector<ContactPoint> contactPoints;
		Vector3 impulse;
		Vector3 relativeVelocity;
	};
}