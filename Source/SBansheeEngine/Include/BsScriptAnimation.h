//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#pragma once

#include "BsScriptEnginePrerequisites.h"
#include "BsScriptObject.h"
#include "BsAnimation.h"

namespace BansheeEngine
{
	class ScriptAnimationClip;

	/** @addtogroup ScriptInteropEngine
	 *  @{
	 */

	/**	Interop class between C++ & CLR for Animation. */
	class BS_SCR_BE_EXPORT ScriptAnimation : public ScriptObject <ScriptAnimation>
	{
	public:
		SCRIPT_OBJ(ENGINE_ASSEMBLY, "BansheeEngine", "NativeAnimation")

		/**	Returns the native wrapped animation. */
		SPtr<Animation> getInternal() const { return mAnimation; }

	private:
		ScriptAnimation(MonoObject* managedInstance);
		~ScriptAnimation();

		SPtr<Animation> mAnimation;

		/************************************************************************/
		/* 								CLR HOOKS						   		*/
		/************************************************************************/
		static void internal_Create(MonoObject* instance);
		static void internal_Destroy(ScriptAnimation* thisPtr);

		static void internal_SetWrapMode(ScriptAnimation* thisPtr, AnimWrapMode wrapMode);
		static void internal_SetSpeed(ScriptAnimation* thisPtr, float speed);

		static void internal_Play(ScriptAnimation* thisPtr, ScriptAnimationClip* clip, UINT32 layer, AnimPlayMode playMode);
		static void internal_Blend(ScriptAnimation* thisPtr, ScriptAnimationClip* clip, float weight, float fadeLength, UINT32 layer);
		static void internal_CrossFade(ScriptAnimation* thisPtr, ScriptAnimationClip* clip, float fadeLength, UINT32 layer, AnimPlayMode playMode);

		static void internal_Stop(ScriptAnimation* thisPtr, UINT32 layer);
		static void internal_StopAll(ScriptAnimation* thisPtr);
		static bool internal_IsPlaying(ScriptAnimation* thisPtr);

		static bool internal_GetState(ScriptAnimation* thisPtr, ScriptAnimationClip* clip, AnimationClipState* state);
		static void internal_SetState(ScriptAnimation* thisPtr, ScriptAnimationClip* clip, AnimationClipState* state);
	};

	/** @} */
}