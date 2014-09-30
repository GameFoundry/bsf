#include "BsHandleManager.h"

namespace BansheeEngine
{
	void HandleManager::_registerCapsuleCollider(const Capsule& collider, HandleSlider* slider)
	{
		if (mSliders.find(slider) == mSliders.end())
			mSliders.insert(slider);

		mCapsuleColliders[slider] = collider;
	}

	void HandleManager::_registerSphereCollider(const Sphere& collider, HandleSlider* slider)
	{
		if (mSliders.find(slider) == mSliders.end())
			mSliders.insert(slider);

		mSphereColliders[slider] = collider;
	}

	void HandleManager::_registerRectCollider(const Rect3& collider, HandleSlider* slider)
	{
		if (mSliders.find(slider) == mSliders.end())
			mSliders.insert(slider);

		mRectColliders[slider] = collider;
	}

	void HandleManager::_registerTorusCollider(const Torus& collider, HandleSlider* slider)
	{
		if (mSliders.find(slider) == mSliders.end())
			mSliders.insert(slider);

		mTorusColliders[slider] = collider;
	}

	void HandleManager::_unregisterSlider(HandleSlider* slider)
	{
		mCapsuleColliders.erase(slider);
		mSphereColliders.erase(slider);
		mRectColliders.erase(slider);
		mTorusColliders.erase(slider);
		mSliders.erase(slider);
	}
}