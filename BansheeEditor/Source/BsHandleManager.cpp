#include "BsHandleManager.h"
#include "BsHandleDrawManager.h"
#include "BsHandleSliderManager.h"
#include "BsSceneEditorWidget.h"
#include "BsCamera.h"
#include "BsSceneObject.h"

namespace BansheeEngine
{
	HandleManager::HandleManager()
		:mSliderManager(nullptr), mDrawManager(nullptr)
	{
		mSliderManager = bs_new<HandleSliderManager>();
		mDrawManager = bs_new<HandleDrawManager>();
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

	void HandleManager::update(const HCamera& camera)
	{
		queueDrawCommands();
		mDrawManager->draw(camera);
	}

	bool HandleManager::hasHitHandle(const HCamera& camera, const Vector2I& inputPos) const
	{
		return mSliderManager->hasHitSlider(camera, inputPos);
	}

	void HandleManager::handleInput(const HCamera& camera, const Vector2I& inputPos, bool pressed)
	{
		refreshHandles();
		mSliderManager->handleInput(camera, inputPos, pressed);
		triggerHandles();
	}

	float HandleManager::getHandleSize(const HCamera& camera, const Vector3& handlePos) const
	{
		HSceneObject cameraSO = camera->SO();
		Vector3 cameraPos = camera->SO()->getWorldPosition();

		Vector3 diff = handlePos - cameraPos;
		float distAlongViewDir = diff.dot(cameraSO->getForward());

		return mDefaultHandleSize / std::max(distAlongViewDir, 0.0001f);
	}
}