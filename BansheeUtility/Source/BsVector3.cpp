//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#include "BsVector3.h"
#include "BsMath.h"
#include "BsVector4.h"

namespace BansheeEngine
{
	Vector3::Vector3(const Vector4& vec)
		:x(vec.x), y(vec.y), z(vec.z)
	{

	}

    const Vector3 Vector3::ZERO(0, 0, 0);
	const Vector3 Vector3::ONE(1, 1, 1);
	const Vector3 Vector3::INF =
		Vector3(std::numeric_limits<float>::infinity(), std::numeric_limits<float>::infinity(), std::numeric_limits<float>::infinity());

    const Vector3 Vector3::UNIT_X(1, 0, 0);
    const Vector3 Vector3::UNIT_Y(0, 1, 0);
    const Vector3 Vector3::UNIT_Z(0, 0, 1);    
}
