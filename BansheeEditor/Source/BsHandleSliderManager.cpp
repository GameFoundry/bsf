#include "BsHandleSliderManager.h"
#include "BsDrawHelper.h"
#include "BsMaterial.h"
#include "BsBuiltinEditorResources.h"
#include "BsCoreThread.h"
#include "BsRendererManager.h"
#include "BsCoreRenderer.h"
#include "BsTransientMesh.h"
#include "BsCamera.h"
#include "BsHandleSlider.h"

using namespace std::placeholders;

namespace BansheeEngine
{
	HandleSliderManager::HandleSliderManager()
		:mActiveSlider(nullptr), mHoverSlider(nullptr)
	{

	}

	HandleSliderManager::~HandleSliderManager()
	{

	}

	void HandleSliderManager::update(const CameraHandlerPtr& camera, const Vector2I& inputPos)
	{
		for (auto& slider : mSliders)
			slider->update(camera);

		if (mActiveSlider != nullptr)
		{
			Ray inputRay = camera->screenPointToRay(inputPos);
			mActiveSlider->handleInput(camera, inputPos, inputRay);
		}
		else
		{
			HandleSlider* newHoverSlider = findUnderCursor(camera, inputPos);

			if (newHoverSlider != mHoverSlider)
			{
				if (mHoverSlider != nullptr)
				{
					mHoverSlider->setInactive();
					mHoverSlider = nullptr;
				}

				if (newHoverSlider != nullptr)
				{
					mHoverSlider = newHoverSlider;
					mHoverSlider->setHover();
				}
			}
		}
	}

	void HandleSliderManager::trySelect(const CameraHandlerPtr& camera, const Vector2I& inputPos)
	{
		HandleSlider* newActiveSlider = findUnderCursor(camera, inputPos);

		if (mHoverSlider != nullptr)
		{
			mHoverSlider->setInactive();
			mHoverSlider = nullptr;
		}

		if (newActiveSlider != mActiveSlider)
		{
			if (mActiveSlider != nullptr)
			{
				mActiveSlider->setInactive();
				mActiveSlider = nullptr;
			}

			if (newActiveSlider != nullptr)
			{
				mActiveSlider = newActiveSlider;
				mActiveSlider->setActive(inputPos);
			}
		}
	}

	void HandleSliderManager::clearSelection()
	{
		if (mActiveSlider != nullptr)
		{
			mActiveSlider->setInactive();
			mActiveSlider = nullptr;
		}
	}

	HandleSlider* HandleSliderManager::findUnderCursor(const CameraHandlerPtr& camera, const Vector2I& inputPos) const
	{
		Ray inputRay = camera->screenPointToRay(inputPos);

		float nearestT = std::numeric_limits<float>::max();
		HandleSlider* overSlider = nullptr;
		
		for (auto& slider : mSliders)
		{
			float t;
			if (slider->intersects(inputRay, t))
			{
				if (t < nearestT)
				{
					overSlider = slider;
					nearestT = t;
				}
			}
		}

		return overSlider;
	}

	void HandleSliderManager::_registerSlider(HandleSlider* slider)
	{
		mSliders.insert(slider);
	}

	void HandleSliderManager::_unregisterSlider(HandleSlider* slider)
	{
		mSliders.erase(slider);

		if (mActiveSlider == slider)
			mActiveSlider = nullptr;
	}
}