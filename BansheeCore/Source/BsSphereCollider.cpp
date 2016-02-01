#include "BsSphereCollider.h"
#include "BsPhysics.h"

namespace BansheeEngine
{
	SphereCollider::SphereCollider()
	{ }

	SPtr<SphereCollider> SphereCollider::create(float radius, const Vector3& position, const Quaternion& rotation)
	{
		return Physics::instance().createSphereCollider(radius, position, rotation);
	}
}