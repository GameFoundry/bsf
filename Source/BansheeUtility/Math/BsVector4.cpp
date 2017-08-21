//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#include "Math/BsVector4.h"
#include "Math/BsMath.h"

namespace bs
{
    const Vector4 Vector4::ZERO(0, 0, 0, 0);

	bool Vector4::isNaN() const
	{
		return Math::isNaN(x) || Math::isNaN(y) || Math::isNaN(z) || Math::isNaN(w);
	}
}
