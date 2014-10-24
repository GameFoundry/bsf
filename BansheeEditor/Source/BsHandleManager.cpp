#include "BsHandleManager.h"
#include "BsHandleDrawManager.h"
#include "BsHandleSliderManager.h"
#include "BsSceneEditorWidget.h"

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

	void HandleManager::update()
	{
		SceneEditorWidget* sceneView = SceneViewLocator::instance();
		if (sceneView != nullptr)
		{
			HCamera sceneCamera = sceneView->getSceneCamera();

			queueDrawCommands();
			mDrawManager->draw(sceneCamera);
		}
	}

	void HandleManager::handleInput(const Vector2I& inputPos, const Ray& inputRay, bool pressed)
	{
		SceneEditorWidget* sceneView = SceneViewLocator::instance();
		if (sceneView != nullptr)
		{
			HCamera sceneCamera = sceneView->getSceneCamera();

			refreshHandles();
			mSliderManager->update(sceneCamera, inputPos, inputRay, pressed);
			triggerHandles();
		}
	}
}