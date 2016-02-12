#include "BsFCollider.h"
#include "BsRigidbody.h"

namespace BansheeEngine
{
	FCollider::~FCollider()
	{ }

	void FCollider::setMaterial(const HPhysicsMaterial& material)
	{
		mMaterial = material;
	}

	void FCollider::setLayer(UINT64 layer)
	{
		mLayer = layer;
	}
}