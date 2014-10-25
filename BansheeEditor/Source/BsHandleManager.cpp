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
			mSliderManager->handleInput(sceneCamera, inputPos, inputRay, pressed);
			triggerHandles();
		}
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