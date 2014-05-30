#include "BsCoreApplication.h"

#include "BsRenderSystem.h"
#include "BsRenderSystemManager.h"

#include "BsPlatform.h"
#include "BsHardwareBufferManager.h"
#include "BsRenderWindow.h"
#include "BsViewport.h"
#include "BsVector2.h"
#include "BsGpuProgram.h"
#include "BsCoreObjectManager.h"
#include "BsGameObjectManager.h"
#include "BsDynLib.h"
#include "BsDynLibManager.h"
#include "BsCoreSceneManager.h"
#include "BsImporter.h"
#include "BsResources.h"
#include "BsMesh.h"
#include "BsSceneObject.h"
#include "BsTime.h"
#include "BsInput.h"
#include "BsRendererManager.h"
#include "BsGpuProgramManager.h"
#include "BsMeshManager.h"
#include "BsMaterialManager.h"
#include "BsFontManager.h"
#include "BsRenderWindowManager.h"
#include "BsRenderer.h"
#include "BsDeferredCallManager.h"
#include "BsCoreThread.h"
#include "BsStringTable.h"
#include "BsProfilingManager.h"
#include "BsProfilerCPU.h"
#include "BsProfilerGPU.h"
#include "BsQueryManager.h"
#include "BsThreadPool.h"
#include "BsTaskScheduler.h"
#include "BsUUID.h"

#include "BsMaterial.h"
#include "BsShader.h"
#include "BsTechnique.h"
#include "BsPass.h"

#include "BsRendererManager.h"

namespace BansheeEngine
{
	CoreApplication::CoreApplication(START_UP_DESC& desc)
		:mPrimaryWindow(nullptr), mIsFrameRenderingFinished(true), mRunMainLoop(false), mSceneManagerPlugin(nullptr)
	{
		UINT32 numWorkerThreads = BS_THREAD_HARDWARE_CONCURRENCY - 1; // Number of cores while excluding current thread.

		Platform::_startUp();
		MemStack::beginThread();

		UUIDGenerator::startUp();
		ProfilerCPU::startUp();
		ProfilingManager::startUp();
		ThreadPool::startUp<TThreadPool<ThreadBansheePolicy>>((numWorkerThreads));
		TaskScheduler::startUp();
		TaskScheduler::instance().removeWorker();
		CoreThread::startUp();
		StringTable::startUp();
		DeferredCallManager::startUp();
		Time::startUp();
		DynLibManager::startUp();
		CoreObjectManager::startUp();
		GameObjectManager::startUp();
		Resources::startUp();
		GpuProgramManager::startUp();
		RenderSystemManager::startUp();

		mPrimaryWindow = RenderSystemManager::instance().initialize(desc.renderSystem, desc.primaryWindowDesc);

		Input::startUp();
		RendererManager::startUp();

		loadPlugin(desc.renderer);
		RendererManager::instance().setActive(desc.renderer);

		loadPlugin(desc.sceneManager, &mSceneManagerPlugin);

		ProfilerGPU::startUp();
		MeshManager::startUp();
		MaterialManager::startUp();
		FontManager::startUp();

		Importer::startUp();

		for (auto& importerName : desc.importers)
			loadPlugin(importerName);

		loadPlugin(desc.input, nullptr, mPrimaryWindow.get());
	}

	CoreApplication::~CoreApplication()
	{
		mPrimaryWindow->destroy();
		mPrimaryWindow = nullptr;

		Importer::shutDown();
		FontManager::shutDown();
		MaterialManager::shutDown();
		MeshManager::shutDown();
		ProfilerGPU::shutDown();

		unloadPlugin(mSceneManagerPlugin);

		RendererManager::shutDown();
		RenderSystemManager::shutDown();
		Input::shutDown();

		GpuProgramManager::shutDown();
		Resources::shutDown();
		GameObjectManager::shutDown();
		CoreObjectManager::shutDown(); // Must shut down before DynLibManager to ensure all objects are destroyed before unloading their libraries
		DynLibManager::shutDown();
		Time::shutDown();
		DeferredCallManager::shutDown();
		StringTable::shutDown();

		CoreThread::shutDown();
		TaskScheduler::shutDown();
		ThreadPool::shutDown();
		ProfilingManager::shutDown();
		ProfilerCPU::shutDown();
		UUIDGenerator::shutDown();

		MemStack::endThread();
		Platform::_shutDown();
	}

	void CoreApplication::runMainLoop()
	{
		mRunMainLoop = true;

		while(mRunMainLoop)
		{
			gProfilerCPU().beginThread("Sim");

			gCoreThread().update();
			Platform::_update();
			DeferredCallManager::instance()._update();
			RenderWindowManager::instance()._update();
			gInput()._update();
			gTime().update();

			PROFILE_CALL(gSceneManager()._update(), "SceneManager");

			gCoreThread().queueCommand(std::bind(&CoreApplication::beginCoreProfiling, this));
			gCoreThread().queueCommand(std::bind(&QueryManager::_update, QueryManager::instancePtr()));

			update();

			PROFILE_CALL(RendererManager::instance().getActive()->renderAll(), "Render");

			// Core and sim thread run in lockstep. This will result in a larger input latency than if I was 
			// running just a single thread. Latency becomes worse if the core thread takes longer than sim 
			// thread, in which case sim thread needs to wait. Optimal solution would be to get an average 
			// difference between sim/core thread and start the sim thread a bit later so they finish at nearly the same time.
			{
				BS_LOCK_MUTEX_NAMED(mFrameRenderingFinishedMutex, lock);

				while(!mIsFrameRenderingFinished)
				{
					TaskScheduler::instance().addWorker();
					BS_THREAD_WAIT(mFrameRenderingFinishedCondition, mFrameRenderingFinishedMutex, lock);
					TaskScheduler::instance().removeWorker();
				}

				mIsFrameRenderingFinished = false;
			}

			gCoreThread().queueCommand(&Platform::_coreUpdate);
			gCoreThread().submitAccessors();
			gCoreThread().queueCommand(std::bind(&ProfilerGPU::_update, ProfilerGPU::instancePtr()));
			gCoreThread().queueCommand(std::bind(&CoreApplication::endCoreProfiling, this));
			gCoreThread().queueCommand(std::bind(&CoreApplication::frameRenderingFinishedCallback, this));

			gProfilerCPU().endThread();
			gProfiler()._update();
		}
	}

	void CoreApplication::update()
	{
		// Do nothing
	}

	void CoreApplication::stopMainLoop()
	{
		mRunMainLoop = false; // No sync primitives needed, in that rare case of 
		// a race condition we might run the loop one extra iteration which is acceptable
	}

	void CoreApplication::frameRenderingFinishedCallback()
	{
		BS_LOCK_MUTEX(mFrameRenderingFinishedMutex);

		mIsFrameRenderingFinished = true;
		BS_THREAD_NOTIFY_ONE(mFrameRenderingFinishedCondition);
	}

	void CoreApplication::beginCoreProfiling()
	{
		gProfilerCPU().beginThread("Core");
	}

	void CoreApplication::endCoreProfiling()
	{
		gProfilerCPU().endThread();
		gProfiler()._updateCore();
	}

	void* CoreApplication::loadPlugin(const String& pluginName, DynLib** library, void* passThrough)
	{
		String name = pluginName;
#if BS_PLATFORM == BS_PLATFORM_LINUX
		// dlopen() does not add .so to the filename, like windows does for .dll
		if (name.substr(name.length() - 3, 3) != ".so")
			name += ".so";
#elif BS_PLATFORM == BS_PLATFORM_APPLE
		// dlopen() does not add .dylib to the filename, like windows does for .dll
		if (name.substr(name.length() - 6, 6) != ".dylib")
			name += ".dylib";
#elif BS_PLATFORM == BS_PLATFORM_WIN32
		// Although LoadLibraryEx will add .dll itself when you only specify the library name,
		// if you include a relative path then it does not. So, add it to be sure.
		if (name.substr(name.length() - 4, 4) != ".dll")
			name += ".dll";
#endif

		DynLib* loadedLibrary = gDynLibManager().load(name);
		if(library != nullptr)
			*library = loadedLibrary;

		if(loadedLibrary != nullptr)
		{
			if (passThrough == nullptr)
			{
				typedef void* (*LoadPluginFunc)();

				LoadPluginFunc loadPluginFunc = (LoadPluginFunc)loadedLibrary->getSymbol("loadPlugin");

				if (loadPluginFunc != nullptr)
					return loadPluginFunc();
			}
			else
			{
				typedef void* (*LoadPluginFunc)(void*);

				LoadPluginFunc loadPluginFunc = (LoadPluginFunc)loadedLibrary->getSymbol("loadPlugin");

				if (loadPluginFunc != nullptr)
					return loadPluginFunc(passThrough);
			}
		}

		return nullptr;
	}

	void CoreApplication::unloadPlugin(DynLib* library)
	{
		typedef void (*UnloadPluginFunc)();

		UnloadPluginFunc unloadPluginFunc = (UnloadPluginFunc)library->getSymbol("unloadPlugin");

		if(unloadPluginFunc != nullptr)
			unloadPluginFunc();

		gDynLibManager().unload(library);
	}

	CoreApplication& gCoreApplication()
	{
		return CoreApplication::instance();
	}
}