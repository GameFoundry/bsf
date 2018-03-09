//************************************ bs::framework - Copyright 2018 Marko Pintera **************************************//
//*********** Licensed under the MIT license. See LICENSE.md for full terms. This notice is not to be removed. ***********//
#include "Physics/BsBoxCollider.h"
#include "Physics/BsPhysics.h"

namespace bs
{
	BoxCollider::BoxCollider()
	{ }

	SPtr<BoxCollider> BoxCollider::create(const Vector3& extents,
		const Vector3& position, const Quaternion& rotation)
	{
		return Physics::instance().createBoxCollider(extents, position, rotation);
	}
}