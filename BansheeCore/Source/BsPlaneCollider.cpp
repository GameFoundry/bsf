#include "BsPlaneCollider.h"
#include "BsPhysics.h"

namespace BansheeEngine
{
	PlaneCollider::PlaneCollider()
	{ }

	SPtr<PlaneCollider> PlaneCollider::create(const Vector3& position, const Quaternion& rotation)
	{
		return Physics::instance().createPlaneCollider(position, rotation);
	}
}