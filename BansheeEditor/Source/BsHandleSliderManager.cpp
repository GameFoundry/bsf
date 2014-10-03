#include "BsHandleSliderManager.h"
#include "BsDrawHelper.h"
#include "BsMaterial.h"
#include "BsBuiltinEditorResources.h"
#include "BsCoreThread.h"
#include "BsRendererManager.h"
#include "BsRenderer.h"
#include "BsTransientMesh.h"
#include "BsCamera.h"

using namespace std::placeholders;

namespace BansheeEngine
{
	HandleSliderManager::HandleSliderManager()
	{

	}

	HandleSliderManager::~HandleSliderManager()
	{

	}

	void HandleSliderManager::update(const Vector2I& inputPos, const Ray& inputRay, const Matrix4& viewMatrix, bool pressed)
	{
		// TODO - Handle mouse input
		//  - Detect active and hover sliders
	}

	bool HandleSliderManager::isSliderActive() const
	{
		// TODO - Return true if we are dragging any slider
	}

	void HandleSliderManager::_registerCapsuleCollider(const Capsule& collider, HandleSlider* slider)
	{
		if (mSliders.find(slider) == mSliders.end())
			mSliders.insert(slider);

		mCapsuleColliders[slider] = collider;
	}

	void HandleSliderManager::_registerSphereCollider(const Sphere& collider, HandleSlider* slider)
	{
		if (mSliders.find(slider) == mSliders.end())
			mSliders.insert(slider);

		mSphereColliders[slider] = collider;
	}

	void HandleSliderManager::_registerRectCollider(const Rect3& collider, HandleSlider* slider)
	{
		if (mSliders.find(slider) == mSliders.end())
			mSliders.insert(slider);

		mRectColliders[slider] = collider;
	}

	void HandleSliderManager::_registerTorusCollider(const Torus& collider, HandleSlider* slider)
	{
		if (mSliders.find(slider) == mSliders.end())
			mSliders.insert(slider);

		mTorusColliders[slider] = collider;
	}

	void HandleSliderManager::_unregisterSlider(HandleSlider* slider)
	{
		mCapsuleColliders.erase(slider);
		mSphereColliders.erase(slider);
		mRectColliders.erase(slider);
		mTorusColliders.erase(slider);
		mSliders.erase(slider);
	}
}