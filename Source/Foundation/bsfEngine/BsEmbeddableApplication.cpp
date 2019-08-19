//************************************ bs::framework - Copyright 2018 Marko Pintera **************************************//
//*********** Licensed under the MIT license. See LICENSE.md for full terms. This notice is not to be removed. ***********//
#include "BsEmbeddableApplication.h"

#include "Renderer/BsRendererManager.h"
#include "Audio/BsAudio.h"
#include "Physics/BsPhysics.h"
#include "Threading/BsTaskScheduler.h"
#include "Platform/BsPlatform.h"
#include "Profiling/BsProfilingManager.h"
#include "Profiling/BsProfilerCPU.h"
#include "Profiling/BsProfilerGPU.h"
#include "Utility/BsDeferredCallManager.h"
#include "Input/BsInput.h"
#include "Managers/BsRenderWindowManager.h"
#include "Scene/BsSceneManager.h"
#include "Renderer/BsRenderer.h"
#include "Animation/BsAnimationManager.h"
#include "Particles/BsParticleManager.h"
#include "Managers/BsResourceListenerManager.h"
#include "Managers/BsQueryManager.h"

namespace bs
{
	EmbeddableApplication::EmbeddableApplication(const START_UP_DESC& desc)
		: Application(desc)
	{ }

	EmbeddableApplication::~EmbeddableApplication()
	{

	}

	void EmbeddableApplication::renderFrame()
	{
		UINT64 currentTime = gTime().getTimePrecise();
		mLastFrameTime = currentTime;

		gProfilerCPU().beginThread("Sim");

		Platform::_update();
		DeferredCallManager::instance()._update();
		gTime()._update();
		gInput()._update();
		// RenderWindowManager::update needs to happen after Input::update and before Input::_triggerCallbacks,
		// so that all input is properly captured in case there is a focus change, and so that
		// focus change is registered before input events are sent out (mouse press can result in code
		// checking if a window is in focus, so it has to be up to date)
		RenderWindowManager::instance()._update();
		gInput()._triggerCallbacks();
		gDebug()._triggerCallbacks();

		preUpdate();

		// Trigger fixed updates if required
		{
			UINT64 step;
			const UINT32 numIterations = gTime()._getFixedUpdateStep(step);

			const float stepSeconds = step / 1000000.0f;
			for (UINT32 i = 0; i < numIterations; i++)
			{
				fixedUpdate();
				PROFILE_CALL(gSceneManager()._fixedUpdate(), "Scene fixed update");
				PROFILE_CALL(gPhysics().fixedUpdate(stepSeconds), "Physics simulation");

				gTime()._advanceFixedUpdate(step);
			}
		}

		PROFILE_CALL(gSceneManager()._update(), "Scene update");
		gAudio()._update();
		gPhysics().update();

		// Update plugins
		for (auto& pluginUpdateFunc : mPluginUpdateFunctions)
			pluginUpdateFunc.second();

		postUpdate();

		PerFrameData perFrameData;

		// Evaluate animation after scene and plugin updates because the renderer will just now be displaying the
		// animation we sent on the previous frame, and we want the scene information to match to what is displayed.
		perFrameData.animation = AnimationManager::instance().update();
		perFrameData.particles = ParticleManager::instance().update(*perFrameData.animation);

		// Send out resource events in case any were loaded/destroyed/modified
		ResourceListenerManager::instance().update();

		// Trigger any renderer task callbacks (should be done before scene object update, or core sync, so objects have
		// a chance to respond to the callback).
		RendererManager::instance().getActive()->update();

		gSceneManager()._updateCoreObjectTransforms();
		PROFILE_CALL(RendererManager::instance().getActive()->renderAll(perFrameData), "Render");

		// Core and sim thread run in lockstep. This will result in a larger input latency than if I was
		// running just a single thread. Latency becomes worse if the core thread takes longer than sim
		// thread, in which case sim thread needs to wait. Optimal solution would be to get an average
		// difference between sim/core thread and start the sim thread a bit later so they finish at nearly the same time.
		{
			Lock lock(mFrameRenderingFinishedMutex);

			while(!mIsFrameRenderingFinished)
			{
				TaskScheduler::instance().addWorker();
				mFrameRenderingFinishedCondition.wait(lock);
				TaskScheduler::instance().removeWorker();
			}

			mIsFrameRenderingFinished = false;
		}

		gCoreThread().queueCommand(std::bind(&CoreApplication::beginCoreProfiling, this), CTQF_InternalQueue);
		gCoreThread().queueCommand(&Platform::_coreUpdate, CTQF_InternalQueue);
		gCoreThread().queueCommand(std::bind(&ct::RenderWindowManager::_update, ct::RenderWindowManager::instancePtr()), CTQF_InternalQueue);

		gCoreThread().update();
		gCoreThread().submitAll();

		gCoreThread().queueCommand(std::bind(&CoreApplication::frameRenderingFinishedCallback, this), CTQF_InternalQueue);

		gCoreThread().queueCommand(std::bind(&ct::QueryManager::_update, ct::QueryManager::instancePtr()), CTQF_InternalQueue);
		gCoreThread().queueCommand(std::bind(&CoreApplication::endCoreProfiling, this), CTQF_InternalQueue);

		gProfilerCPU().endThread();
		gProfiler()._update();		  
	}

	void EmbeddableApplication::waitUntilFrameFinished()
	{
		{
			Lock lock(mFrameRenderingFinishedMutex);

			while (!mIsFrameRenderingFinished)
			{
				TaskScheduler::instance().addWorker();
				mFrameRenderingFinishedCondition.wait(lock);
				TaskScheduler::instance().removeWorker();
			}
		}
	}

	void EmbeddableApplication::onShutDown()
	{
		waitUntilFrameFinished();

		Application::onShutDown();
	}


	// START_UP_DESC EmbeddableApplication::buildStartUpDesc(VideoMode videoMode, const String& title, bool fullscreen)
	// {
	//	START_UP_DESC desc;

	//	// Set up default plugins
	//	desc.renderAPI = BS_RENDER_API_MODULE;
	//	desc.renderer = BS_RENDERER_MODULE;
	//	desc.audio = BS_AUDIO_MODULE;
	//	desc.physics = BS_PHYSICS_MODULE;

	//	desc.importers.push_back("bsfFreeImgImporter");
	//	desc.importers.push_back("bsfFBXImporter");
	//	desc.importers.push_back("bsfFontImporter");
	//	desc.importers.push_back("bsfSL");

	//	desc.primaryWindowDesc.videoMode = videoMode;
	//	desc.primaryWindowDesc.fullscreen = fullscreen;
	//	desc.primaryWindowDesc.title = title;

	//	return desc;
	// }

	EmbeddableApplication& gEmbeddableApplication()
	{
		return static_cast<EmbeddableApplication&>(EmbeddableApplication::instance());
	}
}
