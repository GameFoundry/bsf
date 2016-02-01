#include "BsBoxCollider.h"
#include "BsPhysics.h"

namespace BansheeEngine
{
	BoxCollider::BoxCollider()
	{ }

	SPtr<BoxCollider> BoxCollider::create(float extentX, float extentY, float extentZ, 
		const Vector3& position, const Quaternion& rotation)
	{
		return Physics::instance().createBoxCollider(extentX, extentY, extentZ, position, rotation);
	}
}