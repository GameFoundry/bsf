#include "BsFCollider.h"
#include "BsRigidbody.h"

namespace BansheeEngine
{
	FCollider::~FCollider()
	{
		if (mRigidbody != nullptr)
			mRigidbody->removeCollider(this);
	}

	void FCollider::setRigidbody(const SPtr<Rigidbody>& rigidbody)
	{
		if (mRigidbody == rigidbody)
			return;

		if (mRigidbody != nullptr)
			mRigidbody->removeCollider(this);

		mRigidbody = rigidbody;

		if (mRigidbody != nullptr)
			mRigidbody->addCollider(this);
	}

	void FCollider::setMaterial(const HPhysicsMaterial& material)
	{
		mMaterial = material;
	}

	void FCollider::setIsActive(bool value)
	{
		mIsActive = value;
	}

	void FCollider::setLayer(UINT64 layer)
	{
		mLayer = layer;
	}
}