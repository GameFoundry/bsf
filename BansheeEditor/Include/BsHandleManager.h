#pragma once

#include "BsEditorPrerequisites.h"
#include "BsModule.h"

namespace BansheeEngine
{
	class BS_ED_EXPORT HandleManager : public Module<HandleManager>
	{
	public:
		HandleManager();
		virtual ~HandleManager();

		void handleInput(const Vector2I& inputPos, const Ray& inputRay, bool pressed);
		void update();

		HandleSliderManager& getSliderManager() const { return *mSliderManager; }
		HandleDrawManager& getDrawManager() const { return *mDrawManager; }

		bool isHandleActive() const;

	protected:
		HandleSliderManager* mSliderManager;
		HandleDrawManager* mDrawManager;

		virtual void refreshHandles() = 0;
		virtual void triggerHandles() = 0;
		virtual void queueDrawCommands() = 0;
	};
}