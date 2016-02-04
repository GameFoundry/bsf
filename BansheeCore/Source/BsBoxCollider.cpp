#include "BsBoxCollider.h"
#include "BsPhysics.h"

namespace BansheeEngine
{
	BoxCollider::BoxCollider()
	{ }

	SPtr<BoxCollider> BoxCollider::create(const Vector3& extents,
		const Vector3& position, const Quaternion& rotation)
	{
		return Physics::instance().createBoxCollider(extents, position, rotation);
	}
}