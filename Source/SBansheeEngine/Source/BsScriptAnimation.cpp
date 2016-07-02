//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#include "BsScriptAnimation.h"
#include "BsScriptMeta.h"
#include "BsMonoField.h"
#include "BsMonoClass.h"
#include "BsMonoManager.h"
#include "BsMonoUtil.h"
#include "BsScriptAnimationClip.h"

namespace BansheeEngine
{
	ScriptAnimation::ScriptAnimation(MonoObject* managedInstance)
		:ScriptObject(managedInstance), mAnimation(nullptr)
	{
		mAnimation = Animation::create();
	}

	ScriptAnimation::~ScriptAnimation()
	{ }

	void ScriptAnimation::initRuntimeData()
	{
		metaData.scriptClass->addInternalCall("Internal_Create", &ScriptAnimation::internal_Create);
		metaData.scriptClass->addInternalCall("Internal_Destroy", &ScriptAnimation::internal_Destroy);

		metaData.scriptClass->addInternalCall("Internal_SetWrapMode", &ScriptAnimation::internal_SetWrapMode);
		metaData.scriptClass->addInternalCall("Internal_SetSpeed", &ScriptAnimation::internal_SetSpeed);

		metaData.scriptClass->addInternalCall("Internal_Play", &ScriptAnimation::internal_Play);
		metaData.scriptClass->addInternalCall("Internal_Blend", &ScriptAnimation::internal_Blend);
		metaData.scriptClass->addInternalCall("Internal_CrossFade", &ScriptAnimation::internal_CrossFade);

		metaData.scriptClass->addInternalCall("Internal_Stop", &ScriptAnimation::internal_Stop);
		metaData.scriptClass->addInternalCall("Internal_StopAll", &ScriptAnimation::internal_StopAll);
		metaData.scriptClass->addInternalCall("Internal_IsPlaying", &ScriptAnimation::internal_IsPlaying);

		metaData.scriptClass->addInternalCall("Internal_GetState", &ScriptAnimation::internal_GetState);
		metaData.scriptClass->addInternalCall("Internal_SetState", &ScriptAnimation::internal_SetState);
	}

	void ScriptAnimation::internal_Create(MonoObject* instance)
	{
		new (bs_alloc<ScriptAnimation>()) ScriptAnimation(instance);
	}

	void ScriptAnimation::internal_Destroy(ScriptAnimation* thisPtr)
	{
		thisPtr->mAnimation = nullptr;
	}

	void ScriptAnimation::internal_SetWrapMode(ScriptAnimation* thisPtr, AnimWrapMode wrapMode)
	{
		thisPtr->getInternal()->setWrapMode(wrapMode);
	}

	void ScriptAnimation::internal_SetSpeed(ScriptAnimation* thisPtr, float speed)
	{
		thisPtr->getInternal()->setSpeed(speed);
	}

	void ScriptAnimation::internal_Play(ScriptAnimation* thisPtr, ScriptAnimationClip* clip, UINT32 layer, AnimPlayMode playMode)
	{
		HAnimationClip nativeClip;
		if (clip != nullptr)
			nativeClip = clip->getHandle();

		thisPtr->getInternal()->play(nativeClip, layer, playMode);
	}

	void ScriptAnimation::internal_Blend(ScriptAnimation* thisPtr, ScriptAnimationClip* clip, float weight, float fadeLength, UINT32 layer)
	{
		HAnimationClip nativeClip;
		if (clip != nullptr)
			nativeClip = clip->getHandle();

		thisPtr->getInternal()->blend(nativeClip, weight, fadeLength, layer);
	}

	void ScriptAnimation::internal_CrossFade(ScriptAnimation* thisPtr, ScriptAnimationClip* clip, float fadeLength, UINT32 layer, AnimPlayMode playMode)
	{
		HAnimationClip nativeClip;
		if (clip != nullptr)
			nativeClip = clip->getHandle();

		thisPtr->getInternal()->crossFade(nativeClip, fadeLength, layer, playMode);
	}

	void ScriptAnimation::internal_Stop(ScriptAnimation* thisPtr, UINT32 layer)
	{
		thisPtr->getInternal()->stop(layer);
	}

	void ScriptAnimation::internal_StopAll(ScriptAnimation* thisPtr)
	{
		thisPtr->getInternal()->stopAll();
	}

	bool ScriptAnimation::internal_IsPlaying(ScriptAnimation* thisPtr)
	{
		return thisPtr->getInternal()->isPlaying();
	}

	bool ScriptAnimation::internal_GetState(ScriptAnimation* thisPtr, ScriptAnimationClip* clip, AnimationClipState* state)
	{
		HAnimationClip nativeClip;
		if (clip != nullptr)
			nativeClip = clip->getHandle();

		return thisPtr->getInternal()->getState(nativeClip, *state);
	}

	void ScriptAnimation::internal_SetState(ScriptAnimation* thisPtr, ScriptAnimationClip* clip, AnimationClipState* state)
	{
		HAnimationClip nativeClip;
		if (clip != nullptr)
			nativeClip = clip->getHandle();

		thisPtr->getInternal()->setState(nativeClip, *state);
	}
}