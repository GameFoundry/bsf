//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#include "BsAnimationManager.h"
#include "BsAnimation.h"
#include "BsTaskScheduler.h"
#include "BsTime.h"

namespace BansheeEngine
{
	AnimationManager::AnimationManager()
		: mNextId(1), mUpdateRate(1.0f / 60.0f), mAnimationTime(0.0f), mLastAnimationUpdateTime(0.0f)
		, mNextAnimationUpdateTime(0.0f), mPaused(false), mWorkerRunning(false)
	{
		mAnimationWorker = Task::create("Animation", std::bind(&AnimationManager::evaluateAnimation, this));
	}

	void AnimationManager::setPaused(bool paused)
	{
		mPaused = paused;
	}

	void AnimationManager::setUpdateRate(UINT32 fps)
	{
		if (fps == 0)
			fps = 1;

		mUpdateRate = 1.0f / fps;
	}

	void AnimationManager::preUpdate()
	{
		if (mPaused || !mWorkerRunning)
			return;

		mAnimationWorker->wait();

		// Make sure we don't load obsolete skeletal pose and other evaluation ouputs written by the animation thread
		std::atomic_thread_fence(std::memory_order_acquire);

		// TODO - Write TRS animation results to relevant SceneObjects
		// TODO - Transfer generic curve evaluated data back to Animation

		mWorkerRunning = false;
	}

	void AnimationManager::postUpdate()
	{
		if (mPaused)
			return;

		mAnimationTime += gTime().getFrameDelta();
		if (mAnimationTime < mNextAnimationUpdateTime)
			return;

		mNextAnimationUpdateTime = Math::floor(mAnimationTime / mUpdateRate) * mUpdateRate + mUpdateRate;

		float timeDelta = mAnimationTime - mLastAnimationUpdateTime;
		mLastAnimationUpdateTime = mAnimationTime;

		for (auto& anim : mAnimations)
			anim.second->updateAnimProxy(timeDelta);

		// TODO - Gather a list of all anim proxies?

		// Make sure thread finishes writing all changes to the anim proxies as they will be read by the animation thread
		std::atomic_thread_fence(std::memory_order_release);
		
		// Note: Animation thread will trigger about the same time as the core thread. The core thread will need to wait
		// until animation thread finishes, which might end up blocking it (and losing the multi-threading performance). 
		// Consider delaying displayed animation for a single frame or pre-calculating animations (by advancing time the
		// previous frame) for non-dirty animations.
		TaskScheduler::instance().addTask(mAnimationWorker);
		mWorkerRunning = true;
	}

	void AnimationManager::evaluateAnimation()
	{
		// Make sure we don't load obsolete anim proxy data written by the simulation thread
		std::atomic_thread_fence(std::memory_order_acquire);

		// TODO - Evaluate skeletal poses for clips that have a skeleton
		// TODO - Evaluate TRS for individual scene objects that have no skeleton
		// TODO - Evaluate generic curves

		// Make sure the thread finishes writing skeletal pose and other evaluation outputs as they will be read by sim and
		// core threads
		std::atomic_thread_fence(std::memory_order_release);
	}

	UINT64 AnimationManager::registerAnimation(Animation* anim)
	{
		mAnimations[mNextId] = anim;
		return mNextId++;
	}

	void AnimationManager::unregisterAnimation(UINT64 animId)
	{
		mAnimations.erase(animId);
	}
}