//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#pragma once

#include "BsCorePrerequisites.h"
#include "BsModule.h"

namespace BansheeEngine
{
	/** @addtogroup Animation-Internal
	 *  @{
	 */
	
	/** 
	 * Keeps track of all active animations, queues animation thread tasks and synchronizes data between simulation, core
	 * and animation threads.
	 */
	class AnimationManager : public Module<AnimationManager>
	{
	public:
		AnimationManager();

	private:
		friend class Animation;

		/** 
		 * Registers a new animation and returns a unique ID for it. Must be called whenever an Animation is constructed. 
		 */
		UINT64 registerAnimation(Animation* anim);

		/** Unregisters an animation with the specified ID. Must be called before an Animation is destroyed. */
		void unregisterAnimation(UINT64 id);

		UINT64 mNextId;
		UnorderedMap<UINT64, Animation*> mAnimations;
	};

	/** @} */
}