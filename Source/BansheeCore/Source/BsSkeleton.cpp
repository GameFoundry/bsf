//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#include "BsSkeleton.h"
#include "BsSkeletonRTTI.h"

namespace BansheeEngine
{
	Skeleton::Skeleton()
		:mBindPoses(nullptr), mBoneInfo(nullptr), mNumBones(0)
	{ }

	Skeleton::Skeleton(BONE_DESC* bones, UINT32 numBones)
		:mBindPoses(bs_newN<Matrix4>(numBones)), mBoneInfo(bs_newN<SkeletonBoneInfo>(numBones)), mNumBones(numBones)
	{
		for(UINT32 i = 0; i < numBones; i++)
		{
			mBindPoses[i] = bones[i].invBindPose;
			mBoneInfo[i].name = bones[i].name;
			mBoneInfo[i].parent = bones[i].parent;
		}
	}

	Skeleton::~Skeleton()
	{
		if(mBindPoses != nullptr)
			bs_deleteN(mBindPoses, mNumBones);

		if (mBoneInfo != nullptr)
			bs_deleteN(mBoneInfo, mNumBones);
	}

	SPtr<Skeleton> Skeleton::create(BONE_DESC* bones, UINT32 numBones)
	{
		Skeleton* rawPtr = new (bs_alloc<Skeleton>()) Skeleton(bones, numBones);

		return bs_shared_ptr<Skeleton>(rawPtr);
	}

	void Skeleton::getPose(SkeletonPose& pose, const AnimationClip& clip, float time, bool loop)
	{
		ANIM_BLEND_STATE_DESC state;
		state.clip = &clip;
		state.layer = 0;
		state.loop = loop;
		state.weight = 1.0f;

		getPose(pose, &state, 1, time);
	}

	void Skeleton::getPose(SkeletonPose& pose, const ANIM_BLEND_STATE_DESC* states, UINT32 numStates, float time)
	{
		// TODO -Blend locally, normalize all weights to 1, layers for additive animations

		// TODO
	}

	SPtr<Skeleton> Skeleton::createEmpty()
	{
		Skeleton* rawPtr = new (bs_alloc<Skeleton>()) Skeleton();

		SPtr<Skeleton> newSkeleton = bs_shared_ptr<Skeleton>(rawPtr);
		return newSkeleton;
	}

	RTTITypeBase* Skeleton::getRTTIStatic()
	{
		return SkeletonRTTI::instance();
	}

	RTTITypeBase* Skeleton::getRTTI() const
	{
		return getRTTIStatic();
	}
}