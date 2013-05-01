#include "CmApplication.h"

#include "CmRenderSystem.h"
#include "CmRenderSystemManager.h"

#include "CmWindowEventUtilities.h"
#include "CmHardwareBufferManager.h"
#include "CmRenderWindow.h"
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
#include "CmSceneObject.h"
#include "CmTime.h"
#include "CmInput.h"
#include "CmRendererManager.h"
#include "CmMeshManager.h"
#include "CmMaterialManager.h"
#include "CmFontManager.h"
#include "CmRenderer.h"

#include "CmMaterial.h"
#include "CmShader.h"
#include "CmTechnique.h"
#include "CmPass.h"

#include "CmRendererManager.h"

namespace CamelotFramework
{
	Application::Application()
		:mPrimaryRenderWindow(nullptr), mIsFrameRenderingFinished(true), mRunMainLoop(false)
	{ }

	void Application::startUp(START_UP_DESC& desc)
	{
		Time::startUp(CM_NEW(Time, GenAlloc) Time());
		Input::startUp(CM_NEW(Input, GenAlloc) Input());
		DynLibManager::startUp(CM_NEW(DynLibManager, GenAlloc) DynLibManager());
		CoreGpuObjectManager::startUp(CM_NEW(CoreGpuObjectManager, GenAlloc) CoreGpuObjectManager());
		Resources::startUp(CM_NEW(Resources, GenAlloc) Resources(desc.resourceCacheDirectory));
		HighLevelGpuProgramManager::startUp(CM_NEW(HighLevelGpuProgramManager, GenAlloc) HighLevelGpuProgramManager());

		RenderSystemManager::startUp(CM_NEW(RenderSystemManager, GenAlloc) RenderSystemManager());
		mPrimaryRenderWindow = RenderSystemManager::instance().initialize(desc.renderSystem, desc.primaryWindowDesc);

		RendererManager::startUp(CM_NEW(RendererManager, GenAlloc) RendererManager());

		loadPlugin(desc.renderer);
		RendererManager::instance().setActive(desc.renderer);

		RenderSystem* renderSystem = RenderSystem::instancePtr();

		mPrimaryRenderContext = renderSystem->createDeferredContext();
		mPrimarySyncedRenderContext = &renderSystem->getSyncedDeferredContext();

		SceneManager::startUp((SceneManager*)loadPlugin(desc.sceneManager));

		MeshManager::startUp(CM_NEW(MeshManager, GenAlloc) MeshManager());
		MaterialManager::startUp(CM_NEW(MaterialManager, GenAlloc) MaterialManager());
		FontManager::startUp(CM_NEW(FontManager, GenAlloc) FontManager());

		Importer::startUp(CM_NEW(Importer, GenAlloc) Importer());

		for(auto& importerName : desc.importers)
			loadPlugin(importerName);

		loadPlugin(desc.input);
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
		mPrimaryRenderWindow->destroy();
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
		Input::shutDown();
		DynLibManager::shutDown();
		Time::shutDown();
	}

	void* Application::loadPlugin(const String& pluginName)
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
			typedef void* (*LoadPluginFunc)();

			LoadPluginFunc loadPluginFunc = (LoadPluginFunc)library->getSymbol("loadPlugin");
			return loadPluginFunc();
		}

		return nullptr;
	}

	UINT64 Application::getAppWindowId()
	{
		if(!mPrimaryRenderWindow)
		{
			CM_EXCEPT(InternalErrorException, "Unable to get window handle. No active window exists!");
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

	RenderContext& gMainRC()
	{
		return *gApplication().mPrimaryRenderContext.get();
	}

	SyncedRenderContext& gMainSyncedRC()
	{
		return *gApplication().mPrimarySyncedRenderContext;
	}
}