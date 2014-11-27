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
#include "BsCoreRenderer.h"
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
#include "BsRenderStats.h"
#include "BsMessageHandler.h"

#include "BsMaterial.h"
#include "BsShader.h"
#include "BsTechnique.h"
#include "BsPass.h"

#include "BsRendererManager.h"

#include <csignal>

namespace BansheeEngine
{
	void handleAbort(int error)
	{
		BS_EXCEPT(InternalErrorException, "abort() called.");
	}

	CoreApplication::CoreApplication(START_UP_DESC& desc)
		:mPrimaryWindow(nullptr), mIsFrameRenderingFinished(true), mRunMainLoop(false), 
		mSceneManagerPlugin(nullptr), mRendererPlugin(nullptr)
	{
		signal(SIGABRT, handleAbort);

		UINT32 numWorkerThreads = BS_THREAD_HARDWARE_CONCURRENCY - 1; // Number of cores while excluding current thread.

		Platform::_startUp();
		MemStack::beginThread();

		MessageHandler::startUp();
		UUIDGenerator::startUp();
		ProfilerCPU::startUp();
		ProfilingManager::startUp();
		ThreadPool::startUp<TThreadPool<ThreadBansheePolicy>>((numWorkerThreads));
		TaskScheduler::startUp();
		TaskScheduler::instance().removeWorker();
		RenderStats::startUp();
		CoreThread::startUp();
		StringTable::startUp();
		DeferredCallManager::startUp();
		Time::startUp();
		DynLibManager::startUp();
		CoreObjectManager::startUp();
		GameObjectManager::startUp();
		Resources::startUp();
		GpuProgramManager::startUp();
		GpuProgramCoreManager::startUp();
		RenderSystemManager::startUp();

		mPrimaryWindow = RenderSystemManager::instance().initialize(desc.renderSystem, desc.primaryWindowDesc);

		Input::startUp();
		RendererManager::startUp();

		loadPlugin(desc.renderer, &mRendererPlugin);
		loadPlugin(desc.sceneManager, &mSceneManagerPlugin);

		RendererManager::instance().setActive(desc.renderer);

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

		shutdownPlugin(mSceneManagerPlugin);

		RendererManager::shutDown();
		shutdownPlugin(mRendererPlugin);
		
		Input::shutDown();

		Resources::shutDown();
		GameObjectManager::shutDown();

		// All CoreObject related modules should be shut down now. They have likely queued CoreObjects for destruction, so
		// we need to wait for those objects to get destroyed before continuing.
		gCoreThread().update();
		gCoreThread().submitAccessors(true);

		unloadPlugin(mSceneManagerPlugin);
		unloadPlugin(mRendererPlugin);

		RenderSystemManager::shutDown();
		GpuProgramCoreManager::shutDown();
		GpuProgramManager::shutDown();

		CoreObjectManager::shutDown(); // Must shut down before DynLibManager to ensure all objects are destroyed before unloading their libraries
		DynLibManager::shutDown();
		Time::shutDown();
		DeferredCallManager::shutDown();
		StringTable::shutDown();

		CoreThread::shutDown();
		RenderStats::shutDown();
		TaskScheduler::shutDown();
		ThreadPool::shutDown();
		ProfilingManager::shutDown();
		ProfilerCPU::shutDown();
		UUIDGenerator::shutDown();
		MessageHandler::shutDown();

		MemStack::endThread();
		Platform::_shutDown();
	}

	void CoreApplication::runMainLoop()
	{
		mRunMainLoop = true;

		while(mRunMainLoop)
		{
			gProfilerCPU().beginThread("Sim");

			Platform::_update();
			DeferredCallManager::instance()._update();
			RenderWindowManager::instance()._update();
			gTime().update();
			gInput()._update();

			PROFILE_CALL(gCoreSceneManager()._update(), "SceneManager");

			gCoreThread().queueCommand(std::bind(&CoreApplication::beginCoreProfiling, this));
			gCoreThread().queueCommand(std::bind(&QueryManager::_update, QueryManager::instancePtr()));

			// Update plugins
			for (auto& pluginUpdateFunc : mPluginUpdateFunctions)
				pluginUpdateFunc.second();

			update();

			// Sync all dirty sim thread CoreObject data to core thread
			CoreObjectManager::instance().syncDownload(CoreObjectSync::Sim, gCoreThread().getFrameAlloc());
			gCoreAccessor().queueCommand(std::bind(&CoreObjectManager::syncUpload, CoreObjectManager::instancePtr(), CoreObjectSync::Sim));

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

			// Sync all dirty core thread CoreObject data to sim thread
			// Note: This relies on core thread having finished the frame (ensured by the sync primitive above)
			CoreObjectManager::instance().syncUpload(CoreObjectSync::Core);

			gCoreThread().queueCommand(&Platform::_coreUpdate);

			FrameAlloc* coreFrameAlloc = gCoreThread().getFrameAlloc();
			gCoreThread().update(); // Active frame allocator now belongs to core thread, do not use it on sim thread anymore
			gCoreThread().submitAccessors(); 

			// This should be called after accessors are submitted to ensure we don't sync CoreObjects that are 
			// about to be destroyed (They're only ever destroyed from accessors)
			gCoreThread().queueCommand(std::bind(&CoreObjectManager::syncDownload, CoreObjectManager::instancePtr(), 
				CoreObjectSync::Core, coreFrameAlloc));
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
		ProfilerGPU::instance().beginFrame();
	}

	void CoreApplication::endCoreProfiling()
	{
		ProfilerGPU::instance().endFrame();
		ProfilerGPU::instance()._update();

		gProfilerCPU().endThread();
		gProfiler()._updateCore();
	}

	void* CoreApplication::loadPlugin(const String& pluginName, DynLib** library, void* passThrough)
	{
		String name = pluginName;
#if BS_PLATFORM == BS_PLATFORM_LINUX
		if (name.substr(name.length() - 3, 3) != ".so")
			name += ".so";
#elif BS_PLATFORM == BS_PLATFORM_APPLE
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

		void* retVal = nullptr;
		if(loadedLibrary != nullptr)
		{
			if (passThrough == nullptr)
			{
				typedef void* (*LoadPluginFunc)();

				LoadPluginFunc loadPluginFunc = (LoadPluginFunc)loadedLibrary->getSymbol("loadPlugin");

				if (loadPluginFunc != nullptr)
					retVal = loadPluginFunc();
			}
			else
			{
				typedef void* (*LoadPluginFunc)(void*);

				LoadPluginFunc loadPluginFunc = (LoadPluginFunc)loadedLibrary->getSymbol("loadPlugin");

				if (loadPluginFunc != nullptr)
					retVal = loadPluginFunc(passThrough);
			}

			UpdatePluginFunc loadPluginFunc = (UpdatePluginFunc)loadedLibrary->getSymbol("updatePlugin");

			if (loadPluginFunc != nullptr)
				mPluginUpdateFunctions[loadedLibrary] = loadPluginFunc;
		}

		return retVal;
	}

	void CoreApplication::unloadPlugin(DynLib* library)
	{
		typedef void (*UnloadPluginFunc)();

		UnloadPluginFunc unloadPluginFunc = (UnloadPluginFunc)library->getSymbol("unloadPlugin");

		if(unloadPluginFunc != nullptr)
			unloadPluginFunc();

		gDynLibManager().unload(library);
	}

	void CoreApplication::shutdownPlugin(DynLib* library)
	{
		typedef void(*ShutdownPluginFunc)();

		ShutdownPluginFunc shutdownPluginFunc = (ShutdownPluginFunc)library->getSymbol("shutdownPlugin");

		if (shutdownPluginFunc != nullptr)
			shutdownPluginFunc();

		mPluginUpdateFunctions.erase(library);
	}

	CoreApplication& gCoreApplication()
	{
		return CoreApplication::instance();
	}
}