#include "BsHandleManager.h"
#include "BsHandleDrawManager.h"
#include "BsHandleSliderManager.h"
#include "BsCCamera.h"
#include "BsSceneObject.h"
#include "BsEditorSettings.h"
#include "BsTime.h"

namespace BansheeEngine
{
	HandleManager::HandleManager()
		:mSliderManager(nullptr), mDrawManager(nullptr), mSettingsHash(0xFFFFFFFF),
		mLastDrawFrameIdx((UINT64)-1), mInputStarted(false)
	{
		mSliderManager = bs_new<HandleSliderManager>();
		mDrawManager = bs_new<HandleDrawManager>();
	}

	HandleManager::~HandleManager()
	{
		bs_delete(mSliderManager);
		bs_delete(mDrawManager);
	}

	bool HandleManager::isHandleActive(const CameraPtr& camera) const
	{
		return mSliderManager->isSliderActive(camera);
	}

	void HandleManager::beginInput()
	{
		triggerPreInput();
		mInputStarted = true;
	}

	void HandleManager::updateInput(const CameraPtr& camera, const Vector2I& inputPos, const Vector2I& inputDelta)
	{
		if(!mInputStarted)
		{
			LOGWRN("Updating handle input without calling beginInput() first. Input won't be processed.");
			return;
		}

		if (mSettings != nullptr && mSettingsHash != mSettings->getHash())
			updateFromEditorSettings();

		mSliderManager->update(camera, inputPos, inputDelta);
	}

	void HandleManager::endInput()
	{
		triggerPostInput();
		mInputStarted = false;
	}

	void HandleManager::draw(const CameraPtr& camera)
	{
		UINT64 frameIdx = gTime().getFrameIdx();
		if (frameIdx != mLastDrawFrameIdx)
		{
			mDrawManager->clear();
			queueDrawCommands();
			mLastDrawFrameIdx = frameIdx;
		}

		mDrawManager->draw(camera);
	}

	void HandleManager::setSettings(const EditorSettingsPtr& settings)
	{
		mSettings = settings;

		updateFromEditorSettings();
	}

	void HandleManager::updateFromEditorSettings()
	{
		setDefaultHandleSize(mSettings->getHandleSize());

		mSettingsHash = mSettings->getHash();
	}

	void HandleManager::trySelect(const CameraPtr& camera, const Vector2I& inputPos)
	{
		return mSliderManager->trySelect(camera, inputPos);
	}

	void HandleManager::clearSelection(const CameraPtr& camera)
	{
		return mSliderManager->clearSelection(camera);
	}

	float HandleManager::getHandleSize(const CameraPtr& camera, const Vector3& handlePos) const
	{
		if (camera->getProjectionType() == PT_PERSPECTIVE)
		{
			Vector3 cameraPos = camera->getPosition();

			Vector3 diff = handlePos - cameraPos;
			float distAlongViewDir = Math::abs(diff.dot(camera->getRotation().zAxis()));

			return distAlongViewDir * mDefaultHandleSize;
		}
		else
		{
			return camera->getOrthoWindowHeight() * mDefaultHandleSize;
		}
	}
}