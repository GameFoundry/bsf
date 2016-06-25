//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#include "BsAnimation.h"
#include "BsAnimationManager.h"
#include "BsAnimationClip.h"

namespace BansheeEngine
{
	PlayingClipInfo::PlayingClipInfo()
		:layerIdx(0), stateIdx(0)
	{ }

	PlayingClipInfo::PlayingClipInfo(const HAnimationClip& clip)
		:clip(clip), layerIdx(0), stateIdx(0)
	{ }

	AnimationProxy::AnimationProxy()
		:layers(nullptr), numLayers(0)
	{ }

	AnimationProxy::~AnimationProxy()
	{
		if (layers != nullptr)
			bs_free(layers);
	}

	void AnimationProxy::rebuild(const SPtr<Skeleton>& skeleton, Vector<PlayingClipInfo>& clipInfos)
	{
		this->skeleton = skeleton;

		if (skeleton != nullptr)
			pose = SkeletonPose(skeleton->getNumBones());
		else
			pose = SkeletonPose();

		rebuild(clipInfos);
	}

	void AnimationProxy::rebuild(Vector<PlayingClipInfo>& clipInfos)
	{
		if (layers != nullptr)
			bs_free(layers);
		
		bs_frame_mark();
		{
			FrameVector<AnimationStateLayer> tempLayers;
			for (auto& clipInfo : clipInfos)
			{
				auto iterFind = std::find_if(tempLayers.begin(), tempLayers.end(), 
					[&](auto& x)
				{
					return x.index == clipInfo.state.layer;
				});

				if (iterFind == tempLayers.end())
				{
					tempLayers.push_back(AnimationStateLayer());
					AnimationStateLayer& newLayer = tempLayers.back();

					newLayer.index = clipInfo.state.layer;
					newLayer.normalizeWeights = false; // TODO - This needs to be deduced from PlayingClipInfo
				}
			}

			std::sort(tempLayers.begin(), tempLayers.end(), 
				[&](auto& x, auto& y)
			{
				return x.index < y.index;
			});

			UINT32 numLayers = (UINT32)tempLayers.size();
			UINT32 numClips = (UINT32)clipInfos.size();
			UINT32 numBones;
			
			if (skeleton != nullptr)
				numBones = skeleton->getNumBones();
			else
				numBones = 0;

			UINT32 layersSize = sizeof(AnimationStateLayer) * numLayers;
			UINT32 clipsSize = sizeof(AnimationState) * numClips;
			UINT32 boneMappingSize = numBones * numClips * sizeof(AnimationCurveMapping);
			UINT8* data = (UINT8*)bs_alloc(layersSize + clipsSize + boneMappingSize);

			layers = (AnimationStateLayer*)data;
			memcpy(layers, tempLayers.data(), layersSize);
			data += layersSize;

			AnimationState* states = (AnimationState*)data;
			data += clipsSize;

			AnimationCurveMapping* boneMappings = (AnimationCurveMapping*)data;
			
			UINT32 curLayerIdx = 0;
			UINT32 curStateIdx = 0;

			for(auto& layer : tempLayers)
			{
				layer.states = &states[curStateIdx];
				layer.numStates = 0;

				UINT32 localStateIdx = 0;
				for(auto& clipInfo : clipInfos)
				{
					if (clipInfo.state.layer != layer.index)
						continue;

					new (&states[curStateIdx]) AnimationState();
					AnimationState& state = states[curStateIdx];
					state.curves = clipInfo.clip->getCurves();
					state.weight = clipInfo.state.weight;
					state.loop = clipInfo.state.wrapMode == AnimWrapMode::Loop;

					state.positionEval.time = clipInfo.state.time;
					state.rotationEval.time = clipInfo.state.time;
					state.scaleEval.time = clipInfo.state.time;

					clipInfo.layerIdx = curLayerIdx;
					clipInfo.stateIdx = localStateIdx;
					clipInfo.curveVersion = clipInfo.clip->getVersion();

					if (skeleton != nullptr)
					{
						state.boneToCurveMapping = &boneMappings[curStateIdx * numBones];
						clipInfo.clip->getBoneMapping(*skeleton, state.boneToCurveMapping);
					}
					else
						state.boneToCurveMapping = nullptr;

					layer.numStates++;
					curStateIdx++;
					localStateIdx++;
				}

				curLayerIdx++;

				// Must be larger than zero otherwise the layer.states pointer will point to data held by some other layer
				assert(layer.numStates > 0);
			}			
		}
		bs_frame_clear();
	}

	void AnimationProxy::updateValues(const Vector<PlayingClipInfo>& clipInfos)
	{
		for(auto& clipInfo : clipInfos)
		{
			AnimationState& state = layers[clipInfo.layerIdx].states[clipInfo.stateIdx];

			state.loop = clipInfo.state.wrapMode == AnimWrapMode::Loop;
			state.weight = clipInfo.state.weight;

			state.positionEval.time = clipInfo.state.time;
			state.rotationEval.time = clipInfo.state.time;
			state.scaleEval.time = clipInfo.state.time;
		}
	}

	void AnimationProxy::updateTime(const Vector<PlayingClipInfo>& clipInfos)
	{
		for (auto& clipInfo : clipInfos)
		{
			AnimationState& state = layers[clipInfo.layerIdx].states[clipInfo.stateIdx];

			state.positionEval.time = clipInfo.state.time;
			state.rotationEval.time = clipInfo.state.time;
			state.scaleEval.time = clipInfo.state.time;
		}
	}

	Animation::Animation()
		:mDefaultWrapMode(AnimWrapMode::Loop), mDefaultSpeed(1.0f), mDirty(AnimDirtyStateFlag::Skeleton)
	{
		mAnimProxy = bs_shared_ptr_new<AnimationProxy>();
		mId = AnimationManager::instance().registerAnimation(this);
	}

	Animation::~Animation()
	{
		AnimationManager::instance().unregisterAnimation(mId);
	}

	void Animation::setSkeleton(const SPtr<Skeleton>& skeleton)
	{
		mSkeleton = skeleton;
		mDirty |= AnimDirtyStateFlag::Skeleton;
	}

	void Animation::setWrapMode(AnimWrapMode wrapMode)
	{
		mDefaultWrapMode = wrapMode;

		for (auto& clipInfo : mPlayingClips)
			clipInfo.state.wrapMode = wrapMode;

		mDirty |= AnimDirtyStateFlag::Value;
	}
	void Animation::setSpeed(float speed)
	{
		mDefaultSpeed = speed;

		for (auto& clipInfo : mPlayingClips)
			clipInfo.state.speed = speed;

		mDirty |= AnimDirtyStateFlag::Value;
	}

	void Animation::play(const HAnimationClip& clip, UINT32 layer, AnimPlayMode playMode)
	{
		if (playMode == AnimPlayMode::StopAll)
			mPlayingClips.clear();
		else
		{
			bs_frame_mark();
			{
				FrameVector<PlayingClipInfo> newClips;
				for (auto& clipInfo : mPlayingClips)
				{
					if (clipInfo.state.layer != layer && clipInfo.clip != clip)
						newClips.push_back(clipInfo);
				}

				mPlayingClips.resize(newClips.size());
				memcpy(mPlayingClips.data(), newClips.data(), sizeof(PlayingClipInfo) * newClips.size());
			}
			bs_frame_clear();
		}

		mPlayingClips.push_back(PlayingClipInfo(clip));
		PlayingClipInfo& newClipInfo = mPlayingClips.back();

		newClipInfo.state.layer = layer;
		
		mDirty |= AnimDirtyStateFlag::Layout;
	}

	void Animation::blend(const HAnimationClip& clip, float weight, float fadeLength, UINT32 layer)
	{
		// TODO
		mDirty |= AnimDirtyStateFlag::Layout;
	}

	void Animation::crossFade(const HAnimationClip& clip, float fadeLength, UINT32 layer, AnimPlayMode playMode)
	{
		// TODO
		mDirty |= AnimDirtyStateFlag::Layout;
	}

	void Animation::stop(UINT32 layer)
	{
		bs_frame_mark();
		{
			FrameVector<PlayingClipInfo> newClips;
			for (auto& clipInfo : mPlayingClips)
			{
				if (clipInfo.state.layer != layer)
					newClips.push_back(clipInfo);
			}

			mPlayingClips.resize(newClips.size());
			memcpy(mPlayingClips.data(), newClips.data(), sizeof(PlayingClipInfo) * newClips.size());
		}
		bs_frame_clear();

		mDirty |= AnimDirtyStateFlag::Layout;
	}

	void Animation::stopAll()
	{
		mPlayingClips.clear();
		mDirty |= AnimDirtyStateFlag::Layout;
	}

	bool Animation::isPlaying() const
	{
		return !mPlayingClips.empty();
	}

	bool Animation::getState(const HAnimationClip& clip, AnimationClipState& state)
	{
		for (auto& clipInfo : mPlayingClips)
		{
			if (clipInfo.clip == clip)
			{
				state = clipInfo.state;
				return true;
			}
		}
		
		return false;
	}

	void Animation::setState(const HAnimationClip& clip, AnimationClipState state)
	{
		for (auto& clipInfo : mPlayingClips)
		{
			if (clipInfo.clip == clip)
			{
				if (clipInfo.state.layer != state.layer)
					mDirty |= AnimDirtyStateFlag::Layout;

				clipInfo.state = state;
				mDirty |= AnimDirtyStateFlag::Value;
				return;
			}
		}
	}

	SPtr<Animation> Animation::create()
	{
		Animation* anim = new (bs_alloc<Animation>()) Animation();

		SPtr<Animation> animPtr = bs_core_ptr(anim);
		animPtr->_setThisPtr(animPtr);
		animPtr->initialize();

		return animPtr;
	}

	void Animation::updateAnimProxy(float timeDelta)
	{
		// Check if any of the clip curves are dirty and advance time
		for (auto& clipInfo : mPlayingClips)
		{
			clipInfo.state.time += timeDelta * clipInfo.state.speed;

			if (clipInfo.curveVersion != clipInfo.clip->getVersion())
				mDirty |= AnimDirtyStateFlag::Layout;
		}

		if((UINT32)mDirty == 0) // Clean
		{
			mAnimProxy->updateTime(mPlayingClips);
		}
		else
		{
			if (mDirty.isSet(AnimDirtyStateFlag::Skeleton))
				mAnimProxy->rebuild(mSkeleton, mPlayingClips);
			else if (mDirty.isSet(AnimDirtyStateFlag::Layout))
				mAnimProxy->rebuild(mPlayingClips);
			else if (mDirty.isSet(AnimDirtyStateFlag::Value))
				mAnimProxy->updateValues(mPlayingClips);
		}

		mDirty = AnimDirtyState();
	}
}