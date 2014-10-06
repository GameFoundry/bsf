#include "BsHandleManager.h"
#include "BsHandleDrawManager.h"
#include "BsHandleSliderManager.h"

namespace BansheeEngine
{
	HandleManager::HandleManager(const HCamera& camera)
		:mSliderManager(nullptr), mDrawManager(nullptr)
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

	void HandleManager::update(const Vector2I& inputPos, const Ray& inputRay, const Matrix4& viewMatrix, bool pressed)
	{
		refreshHandles();

		mSliderManager->update(inputPos, inputRay, viewMatrix, pressed);

		triggerHandles();

		queueDrawCommands();

		mDrawManager->draw();
	}
}