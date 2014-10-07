#include "BsHandleManager.h"
#include "BsHandleDrawManager.h"
#include "BsHandleSliderManager.h"

namespace BansheeEngine
{
	HandleManager::HandleManager(const HCamera& camera)
		:mSliderManager(nullptr), mDrawManager(nullptr), mCamera(camera)
	{
		mSliderManager = bs_new<HandleSliderManager>();
		mDrawManager = bs_new<HandleDrawManager>(camera);
	}

	HandleManager::~HandleManager()
	{
		bs_delete(mSliderManager);
		bs_delete(mDrawManager);
	}

	bool HandleManager::isHandleActive() const
	{
		return mSliderManager->isSliderActive();
	}

	void HandleManager::update(const Vector2I& inputPos, const Ray& inputRay, bool pressed)
	{
		refreshHandles();

		mSliderManager->update(mCamera, inputPos, inputRay, pressed);

		triggerHandles();

		queueDrawCommands();

		mDrawManager->draw();
	}
}