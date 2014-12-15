#include "BsHandleManager.h"
#include "BsHandleDrawManager.h"
#include "BsHandleSliderManager.h"
#include "BsCamera.h"
#include "BsSceneObject.h"
#include "BsEditorSettings.h"

namespace BansheeEngine
{
	HandleManager::HandleManager()
		:mSliderManager(nullptr), mDrawManager(nullptr), mSettingsHash(0xFFFFFFFF)
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

	void HandleManager::update(const CameraHandlerPtr& camera, const Vector2I& inputPos, const Vector2I& inputDelta)
	{
		if (mSettings != nullptr && mSettingsHash != mSettings->getHash())
			updateFromProjectSettings();

		refreshHandles();
		mSliderManager->update(camera, inputPos, inputDelta);
		triggerHandles();

		queueDrawCommands();
		mDrawManager->draw(camera);
	}

	void HandleManager::setSettings(const EditorSettingsPtr& settings)
	{
		mSettings = settings;

		updateFromProjectSettings();
	}

	void HandleManager::updateFromProjectSettings()
	{
		setDefaultHandleSize(mSettings->getHandleSize());

		mSettingsHash = mSettings->getHash();
	}

	void HandleManager::trySelect(const CameraHandlerPtr& camera, const Vector2I& inputPos)
	{
		return mSliderManager->trySelect(camera, inputPos);
	}

	void HandleManager::clearSelection()
	{
		return mSliderManager->clearSelection();
	}

	float HandleManager::getHandleSize(const CameraHandlerPtr& camera, const Vector3& handlePos) const
	{
		Vector3 cameraPos = camera->getPosition();

		Vector3 diff = handlePos - cameraPos;
		float distAlongViewDir = Math::abs(diff.dot(camera->getRotation().zAxis()));

		return distAlongViewDir * mDefaultHandleSize;
	}
}