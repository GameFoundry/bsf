//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#pragma once

#include "BsCorePrerequisites.h"
#include "BsCoreObject.h"
#include "BsFlags.h"
#include "BsSkeleton.h"

namespace BansheeEngine
{
	/** @addtogroup Animation-Internal
	 *  @{
	 */

	/** Determines how an animation clip behaves when it reaches the end. */
	enum class AnimWrapMode
	{
		Loop, /**< Loop around to the beginning/end when the last/first frame is reached. */
		Clamp /**< Clamp to end/beginning, keeping the last/first frame active. */
	};

	/** Determines what happens to other animation clips when a new clip starts playing. */
	enum class AnimPlayMode
	{
		StopAll, /**< All other animation clips are stopped. */
		StopLayer /**< Only the clips within the current layer are stopped. */
	};

	/** Flags that determine which portion of Animation was changed and needs to be updated. */
	enum class AnimDirtyStateFlag
	{
		Clean = 0,
		Value = 1,
		Layout = 2
	};

	typedef Flags<AnimDirtyStateFlag> AnimDirtyState;
	BS_FLAGS_OPERATORS(AnimDirtyStateFlag)

	/** Contains information about a currently playing animation clip. */
	struct AnimationClipState
	{
		AnimationClipState() { }

		/** Layer the clip is playing on. Multiple clips can be played simulatenously on different layers. */
		UINT32 layer = 0; 
		float time = 0.0f; /**< Current time the animation is playing from. */
		float speed = 1.0f; /**< Speed at which the animation is playing. */
		float weight = 1.0f; /**< Determines how much of an influence does the clip have on the final pose. */
		/** Determines what happens to other animation clips when a new clip starts playing. */
		AnimWrapMode wrapMode = AnimWrapMode::Loop;
	};

	/** Internal information about a single playing animation clip within Animation. */
	struct PlayingClipInfo
	{
		PlayingClipInfo() { }
		PlayingClipInfo(const HAnimationClip& clip);

		HAnimationClip clip;
		AnimationClipState state;
	};

	/** Represents a copy of the Animation data for use specifically on the animation thread. */
	struct AnimationProxy
	{
		AnimationProxy();
		~AnimationProxy();

		/** 
		 * Updates the proxy data with a new skeleton and clips. Very expensive update operation.
		 *
		 * @note	Should be called from the sim thread when the caller is sure the animation thread is not using it.
		 */
		void updateSkeleton(const SPtr<Skeleton>& skeleton, const Vector<PlayingClipInfo>& clipInfos);

		/** 
		 * Updates the proxy data with new clips. Very expensive update operation.
		 *
		 * @note	Should be called from the sim thread when the caller is sure the animation thread is not using it.
		 */
		void updateLayout(const Vector<PlayingClipInfo>& clipInfos);

		/** 
		 * Updates the proxy data with new information about the clips. Caller must guarantee that clip layout didn't 
		 * change. Fairly cheap update operation.
		 *
		 * @note	Should be called from the sim thread when the caller is sure the animation thread is not using it.
		 */
		void updateValues(const Vector<PlayingClipInfo>& clipInfos);

		/** 
		 * Updates the proxy data with new clip times. Caller must guarantee that clip layout didn't change. Very cheap
		 * update operation.
		 *
		 * @note	Should be called from the sim thread when the caller is sure the animation thread is not using it.
		 */
		void updateTime(const Vector<PlayingClipInfo>& clipInfos);

		AnimationStateLayer* layers;
		UINT32 numLayers;
		SPtr<Skeleton> skeleton;
		SkeletonPose pose;
	};

	/**
	 * Handles animation playback. Takes one or multiple animation clips as input and evaluates them every animation update
	 * tick depending on set properties. The evaluated data is used by the core thread for skeletal animation, by the sim
	 * thread for updating attached scene objects and bones (if skeleton is attached), or the data is made available for
	 * manual queries in the case of generic animation.
	 */
	class BS_CORE_EXPORT Animation : public CoreObject
	{
	public:
		~Animation();

		/** 
		 * Changes the skeleton which will the translation/rotation/scale animation values manipulate. If no skeleton is set
		 * the animation will only evaluate the generic curves, and the root translation/rotation/scale curves.
		 */
		void setSkeleton(const SPtr<Skeleton>& skeleton);

		/** 
		 * Changes the wrap mode for all active animations. Wrap mode determines what happens when animation reaches the 
		 * first or last frame. 
		 *
		 * @see	AnimWrapMode
		 */
		void setWrapMode(AnimWrapMode wrapMode);

		/** Changes the speed for all animations. The default value is 1.0f. Use negative values to play-back in reverse. */
		void setSpeed(float speed);

		/** 
		 * Plays the specified animation clip at the specified layer. 
		 *
		 * @param[in]	clip		Clip to play.
		 * @param[in]	layer		Layer to play the clip in. Multiple clips can be playing at once in separate layers.
		 * @param[in]	playMode	Determines how are other playing animations handled when this animation starts.
		 */
		void play(const HAnimationClip& clip, UINT32 layer = 0, AnimPlayMode playMode = AnimPlayMode::StopLayer);

		/**
		 * Blends the specified animation clip with other animation clips playing in the specified layer.
		 *
		 * @param[in]	clip		Clip to blend.
		 * @param[in]	weight		Determines how much of an effect will the blended animation have on the final output.
		 *							In range [0, 1]. All animation weights on the same layer will be normalized.
		 * @param[in]	fadeLength	Applies the blend over a specified time period, increasing the weight as the time
		 *							passes. Set to zero to blend immediately. In seconds.
		 * @param[in]	layer		Layer to play the clip in. Multiple clips can be playing at once in separate layers.
		 */
		void blend(const HAnimationClip& clip, float weight, float fadeLength = 0.0f, UINT32 layer = 0);

		/**
		 * Fades the specified animation clip in, while fading other playing animations out, over the specified time
		 * period.
		 *
		 * @param[in]	clip		Clip to fade in.
		 * @param[in]	fadeLength	Determines the time period over which the fade occurs. In seconds.
		 * @param[in]	layer		Layer to play the clip in. Multiple clips can be playing at once in separate layers.
		 * @param[in]	playMode	Determines should all other animations be faded out, or just the ones on the same layer.
		 */
		void crossFade(const HAnimationClip& clip, float fadeLength, UINT32 layer = 0, 
			AnimPlayMode playMode = AnimPlayMode::StopLayer);

		/** Stops playing all animations on the provided layer. */
		void stop(UINT32 layer);

		/** Stops playing all animations. */
		void stopAll();
		
		/** Checks if any animation clips are currently playing. */
		bool isPlaying() const;

		/** 
		 * Retrieves detailed information about a currently playing animation clip. 
		 *
		 * @param[in]	clip	Clip to retrieve the information for.
		 * @param[out]	state	Animation clip state containing the requested information. Only valid if the method returns
		 *						true.
		 * @return				True if the state was found (animation clip is playing), false otherwise.
		 */
		bool getState(const HAnimationClip& clip, AnimationClipState& state);

		/**
		 * Changes the state of a playing animation clip. If animation clip is not currently playing the state change is
		 * ignored.
		 *
		 * @param[in]	clip	Clip to change the state for.
		 * @param[in]	state	New state of the animation (e.g. changing the time for seeking).
		 */
		void setState(const HAnimationClip& clip, AnimationClipState state);

		/** Creates a new empty Animation object. */
		static SPtr<Animation> create();

	private:
		friend class AnimationManager;

		Animation();

		/** 
		 * Updates the animation proxy object based on the currently set skeleton, playing clips and dirty flags. 
		 *
		 * @param[in]	timeDelta	Seconds passed since the last call to this method.
		 */
		void updateAnimProxy(float timeDelta);

		UINT64 mId;
		AnimWrapMode mDefaultWrapMode;
		float mDefaultSpeed;
		AnimDirtyState mDirty;

		SPtr<Skeleton> mSkeleton;
		Vector<PlayingClipInfo> mPlayingClips;

		// Animation thread only
		AnimationProxy mAnimProxy;
	};

	/** @} */
}