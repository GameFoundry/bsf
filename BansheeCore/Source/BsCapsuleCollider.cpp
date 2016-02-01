#include "BsCapsuleCollider.h"
#include "BsPhysics.h"

namespace BansheeEngine
{
	CapsuleCollider::CapsuleCollider()
	{ }

	SPtr<CapsuleCollider> CapsuleCollider::create(float radius, float halfHeight, 
		const Vector3& position, const Quaternion& rotation)
	{
		return Physics::instance().createCapsuleCollider(radius, halfHeight, position, rotation);
	}
}