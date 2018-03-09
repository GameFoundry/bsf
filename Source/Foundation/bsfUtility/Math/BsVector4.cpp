//************************************ bs::framework - Copyright 2018 Marko Pintera **************************************//
//*********** Licensed under the MIT license. See LICENSE.md for full terms. This notice is not to be removed. ***********//
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
