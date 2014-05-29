#pragma once

#include "BsPrerequisites.h"
#include "BsCoreApplication.h"
#include "BsEvent.h"

namespace BansheeEngine
{
	class BS_EXPORT Application : public CoreApplication
	{
	public:
		Application(RENDER_WINDOW_DESC& primaryWindowDesc, const String& renderSystem, const String& renderer);
		virtual ~Application();

		static void startUp(RENDER_WINDOW_DESC& primaryWindowDesc, const String& renderSystem, const String& renderer);

		const ViewportPtr& getPrimaryViewport() const;
	protected:
		virtual void onStartUp();

		virtual void update();

	private:
		DynLib* mMonoPlugin;
		DynLib* mSBansheeEnginePlugin;
	};

	BS_EXPORT Application& gApplication();
}