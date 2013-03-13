#include "CmApplication.h"

#include "CmRenderSystem.h"
#include "CmRenderSystemManager.h"

#include "CmWindowEventUtilities.h"
#include "CmHardwareBufferManager.h"
#include "CmRenderWindow.h"
#include "CmCamera.h"
#include "CmViewport.h"
#include "CmVector2.h"
#include "CmHighLevelGpuProgram.h"
#include "CmHighLevelGpuProgramManager.h"
#include "CmCoreObjectManager.h"
#include "CmDynLib.h"
#include "CmDynLibManager.h"
#include "CmSceneManager.h"
#include "CmImporter.h"
#include "CmResources.h"
#include "CmMesh.h"
#include "CmGameObject.h"
#include "CmTime.h"
#include "CmInput.h"
#include "CmRendererManager.h"
#include "CmMeshManager.h"
#include "CmMaterialManager.h"
#include "CmFontManager.h"
#include "CmRenderer.h"
#include "CmDeferredRenderContext.h"

#include "CmMaterial.h"
#include "CmShader.h"
#include "CmTechnique.h"
#include "CmPass.h"

#include "CmRendererManager.h"

namespace CamelotEngine
{
	Application::Application()
		:mPrimaryRenderWindow(nullptr), mIsFrameRenderingFinished(true), mRunMainLoop(false)
	{ }

	void Application::startUp(const String& renderSystemName, const String& rendererName)
	{
		Time::startUp(new Time());
		Input::startUp(new Input());
		DynLibManager::startUp(new DynLibManager());
		CoreGpuObjectManager::startUp(new CoreGpuObjectManager());
		Resources::startUp(new Resources("D:\\CamelotResourceMetas"));
		HighLevelGpuProgramManager::startUp(new HighLevelGpuProgramManager());

		RenderSystemManager::startUp(new RenderSystemManager());
		RenderSystemManager::instance().setActive(renderSystemName);

		RendererManager::startUp(new RendererManager());

		loadPlugin(rendererName);
		RendererManager::instance().setActive("ForwardRenderer");

		RenderSystem* renderSystem = RenderSystem::instancePtr();

		RENDER_WINDOW_DESC renderWindowDesc;
		renderWindowDesc.width = 1280;
		renderWindowDesc.height = 720;
		renderWindowDesc.title = "Camelot Renderer";
		renderWindowDesc.fullscreen = false;

		mPrimaryRenderWindow = RenderWindow::create(renderWindowDesc);
		mPrimaryRenderWindow->waitUntilInitialized(); // TODO: Created window is required for proper initialization of the render system so I need to wait
		                                              // I plan on handling this differently. Either by somehow allowing the RS to be initialized without a window,
		                                              // or forcing a window to be created with RenderSystemManager::startUp. Initializing OpenGL context without a window
		                                              // is especially troublesome (apparently possible just poorly documented)

		mPrimaryRenderContext = renderSystem->createDeferredContext();

		SceneManager::startUp(new SceneManager());

		MeshManager::startUp(new MeshManager());
		MaterialManager::startUp(new MaterialManager());
		FontManager::startUp(new FontManager());

		Importer::startUp(new Importer());
		loadPlugin("CamelotFreeImgImporter"); // TODO - Load this automatically somehow
		loadPlugin("CamelotFBXImporter"); // TODO - Load this automatically somehow

		loadPlugin("CamelotOISInput"); // TODO - Load this automatically somehow
	}

	void Application::runMainLoop()
	{
		mRunMainLoop = true;

		while(mRunMainLoop)
		{
			gSceneManager().update();

			RenderSystem* renderSystem = RenderSystem::instancePtr();
			RendererManager::instance().getActive()->renderAll();

			// Only queue new commands if render thread has finished rendering
			// TODO - There might be a more optimal way to sync simulation and render threads so we maximize
			// the amount of rendered frames
			bool readyForNextFrame = false;
			{
				CM_LOCK_MUTEX(mFrameRenderingFinishedMutex);
				readyForNextFrame = mIsFrameRenderingFinished;
			}

			if(readyForNextFrame)
			{
				{
					CM_LOCK_MUTEX(mFrameRenderingFinishedMutex);
					mIsFrameRenderingFinished = false;
				}
				
				renderSystem->queueCommand(boost::bind(&Application::updateMessagePump, this));
				mPrimaryRenderContext->submitToGpu();
				renderSystem->queueCommand(boost::bind(&Application::frameRenderingFinishedCallback, this));
			}
			else
				mPrimaryRenderContext->cancelAll();

			gTime().update();
			gInput().update();
		}
	}

	void Application::stopMainLoop()
	{
		mRunMainLoop = false; // No sync primitives needed, in that rare case of 
		// a race condition we might run the loop one extra iteration which is acceptable
	}

	void Application::updateMessagePump()
	{
		WindowEventUtilities::messagePump();
	}

	void Application::frameRenderingFinishedCallback()
	{
		CM_LOCK_MUTEX(mFrameRenderingFinishedMutex);

		mIsFrameRenderingFinished = true;
	}

	void Application::shutDown()
	{
		mPrimaryRenderWindow = nullptr;

		Importer::shutDown();
		FontManager::shutDown();
		MaterialManager::shutDown();
		MeshManager::shutDown();

		SceneManager::shutDown();
		RendererManager::shutDown();
		RenderSystem::shutDown();

		HighLevelGpuProgramManager::shutDown();
		Resources::shutDown();
		CoreGpuObjectManager::shutDown(); // Must shut down before DynLibManager to ensure all objects are destroyed before unloading their libraries
		DynLibManager::shutDown();
		Input::shutDown();
		Time::shutDown();
	}

	void Application::loadPlugin(const String& pluginName)
	{
		String name = pluginName;
#if CM_PLATFORM == CM_PLATFORM_LINUX
		// dlopen() does not add .so to the filename, like windows does for .dll
		if (name.substr(name.length() - 3, 3) != ".so")
			name += ".so";
#elif CM_PLATFORM == CM_PLATFORM_APPLE
		// dlopen() does not add .dylib to the filename, like windows does for .dll
		if (name.substr(name.length() - 6, 6) != ".dylib")
			name += ".dylib";
#elif CM_PLATFORM == CM_PLATFORM_WIN32
		// Although LoadLibraryEx will add .dll itself when you only specify the library name,
		// if you include a relative path then it does not. So, add it to be sure.
		if (name.substr(name.length() - 4, 4) != ".dll")
			name += ".dll";
#endif

		DynLib* library = gDynLibManager().load(name);

		if(library != nullptr)
		{
			typedef const void (*LoadPluginFunc)();

			LoadPluginFunc loadPluginFunc = (LoadPluginFunc)library->getSymbol("loadPlugin");
			loadPluginFunc();
		}
	}

	UINT64 Application::getAppWindowId()
	{
		if(!mPrimaryRenderWindow)
		{
			CM_EXCEPT(InternalErrorException, "Unable to get window handle. No active window is set!");
		}

		UINT64 windowId = 0;
		mPrimaryRenderWindow->getCustomAttribute("WINDOW", &windowId);

		return windowId;
	}

	Application& gApplication()
	{
		static Application application;
		return application;
	}
}