#include "BsRigidbody.h"
#include "BsPhysics.h"

namespace BansheeEngine
{
	Rigidbody::~Rigidbody()
	{
		// It is assumed that child colliders will keep the parent Rigidbody alive, so we don't need to clear their parents
	}

	void Rigidbody::addCollider(Collider* collider)
	{
		mColliders.push_back(collider);
	}

	void Rigidbody::removeCollider(Collider* collider)
	{
		auto iterFind = std::find(mColliders.begin(), mColliders.end(), collider);
		if (iterFind != mColliders.end())
			mColliders.erase(iterFind);
	}

	void Rigidbody::setIsActive(bool value)
	{
		mIsActive = value;
	}

	SPtr<Rigidbody> Rigidbody::create(const Vector3& position, const Quaternion& rotation)
	{
		return gPhysics().createRigidbody(position, rotation);
	}
}