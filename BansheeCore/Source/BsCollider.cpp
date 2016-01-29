#include "BsCollider.h"
#include "BsRigidbody.h"

namespace BansheeEngine
{
	Collider::~Collider()
	{
		if (mRigidbody != nullptr)
			mRigidbody->removeCollider(this);
	}

	void Collider::setRigidbody(const SPtr<Rigidbody>& rigidbody)
	{
		if (mRigidbody == rigidbody)
			return;

		if (mRigidbody != nullptr)
			mRigidbody->removeCollider(this);

		mRigidbody = rigidbody;

		if(mRigidbody != nullptr)
			mRigidbody->addCollider(this);
	}

	void Collider::setMaterial(const HPhysicsMaterial& material)
	{
		mMaterial = material;
	}

	void Collider::setIsActive(bool value)
	{
		mIsActive = value;
	}

	void Collider::setLayer(UINT64 layer)
	{
		mLayer = layer;
	}
}