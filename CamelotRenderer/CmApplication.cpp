#include "CmApplication.h"

#include "OgreRenderSystem.h"
#include "CmRenderSystemManager.h"

namespace CamelotEngine
{
	Application::Application()
	{ }

	void Application::startUp()
	{
		RenderSystemManager::initialize("D3D9RenderSystem");
		RenderSystemManager::getActive()->_initialise(true, "Camelot Renderer");
	}

	void Application::shutDown()
	{
		if(RenderSystemManager::getActive() != nullptr)
			RenderSystemManager::getActive()->shutdown();
	}

	Application& gApplication()
	{
		static Application application;
		return application;
	}
}