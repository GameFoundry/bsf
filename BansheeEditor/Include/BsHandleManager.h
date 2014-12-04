#pragma once

#include "BsEditorPrerequisites.h"
#include "BsModule.h"
#include "BsDegree.h"

namespace BansheeEngine
{
	class BS_ED_EXPORT HandleManager : public Module<HandleManager>
	{
	public:
		HandleManager();
		virtual ~HandleManager();

		void update(const CameraHandlerPtr& camera, const Vector2I& inputPos);
		void trySelect(const CameraHandlerPtr& camera, const Vector2I& inputPos);
		void clearSelection();
		bool isHandleActive() const;

		HandleSliderManager& getSliderManager() const { return *mSliderManager; }
		HandleDrawManager& getDrawManager() const { return *mDrawManager; }

		float getHandleSize(const CameraHandlerPtr& camera, const Vector3& handlePos) const;

		void setDefaultHandleSize(float value) { mDefaultHandleSize = value; }
		void setSettings(const EditorSettingsPtr& settings);

	protected:
		void updateFromProjectSettings();

		HandleSliderManager* mSliderManager;
		HandleDrawManager* mDrawManager;

		float mDefaultHandleSize = 20.0f;

		EditorSettingsPtr mSettings;
		UINT32 mSettingsHash;

		virtual void refreshHandles() = 0;
		virtual void triggerHandles() = 0;
		virtual void queueDrawCommands() = 0;
	};
}