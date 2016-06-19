//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#include "BsSkeleton.h"
#include "BsAnimationClip.h"
#include "BsSkeletonRTTI.h"

namespace BansheeEngine
{
	SkeletonPose::SkeletonPose(UINT32 numBones)
		: numBones(numBones)
	{
		UINT32 elementSize = sizeof(Matrix4) + sizeof(Vector3) * 2 + sizeof(Quaternion);
		UINT8* buffer = (UINT8*)bs_alloc(elementSize * sizeof(numBones));

		bonePoses = (Matrix4*)buffer;
		buffer += sizeof(Matrix4) * numBones;

		positions = (Vector3*)buffer;
		buffer += sizeof(Vector3) * numBones;

		rotations = (Quaternion*)buffer;
		buffer += sizeof(Quaternion) * numBones;

		scales = (Vector3*)buffer;

		for (UINT32 i = 0; i < numBones; i++)
		{
			bonePoses[i] = Matrix4::IDENTITY;
			positions[i] = Vector3::ZERO;
			rotations[i] = Quaternion::IDENTITY;
			scales[i] = Vector3::ONE;
		}
	}

	SkeletonPose::~SkeletonPose()
	{
		bs_free(bonePoses);
	}

	Skeleton::Skeleton()
		:mInvBindPoses(nullptr), mBoneInfo(nullptr), mNumBones(0)
	{ }

	Skeleton::Skeleton(BONE_DESC* bones, UINT32 numBones)
		:mInvBindPoses(bs_newN<Matrix4>(numBones)), mBoneInfo(bs_newN<SkeletonBoneInfo>(numBones)), mNumBones(numBones)
	{
		for(UINT32 i = 0; i < numBones; i++)
		{
			mInvBindPoses[i] = bones[i].invBindPose;
			mBoneInfo[i].name = bones[i].name;
			mBoneInfo[i].parent = bones[i].parent;
		}
	}

	Skeleton::~Skeleton()
	{
		if(mInvBindPoses != nullptr)
			bs_deleteN(mInvBindPoses, mNumBones);

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
		AnimationState state;
		state.curves = clip.getCurves();
		state.loop = loop;
		state.weight = 1.0f;
		state.positionEval.time = time;
		state.rotationEval.time = time;
		state.scaleEval.time = time;

		AnimationStateLayer layer;
		layer.index = 0;
		layer.normalizeWeights = false;
		layer.states = &state;
		layer.numStates = 1;

		state.boneToCurveMapping.resize(mNumBones);
		clip.getBoneMapping(*this, state.boneToCurveMapping.data());

		getPose(pose, &layer, 1);
	}

	void Skeleton::getPose(SkeletonPose& pose, const AnimationStateLayer* layers, UINT32 numLayers)
	{
		assert(pose.numBones == mNumBones);

		for(UINT32 i = 0; i < mNumBones; i++)
		{
			pose.positions[i] = Vector3::ZERO;
			pose.rotations[i] = Quaternion::IDENTITY;
			pose.scales[i] = Vector3::ONE;
		}

		for(UINT32 i = 0; i < numLayers; i++)
		{
			const AnimationStateLayer& layer = layers[i];

			float invLayerWeight;
			if (layer.normalizeWeights)
			{
				float weightSum = 0.0f;
				for (UINT32 j = 0; j < layer.numStates; j++)
					weightSum += layer.normalizeWeights;

				invLayerWeight = 1.0f / weightSum;
			}
			else
				invLayerWeight = 1.0f;


			for (UINT32 j = 0; j < layer.numStates; j++)
			{
				const AnimationState& state = layer.states[i];

				float normWeight = state.weight * invLayerWeight;
				for (UINT32 k = 0; k < mNumBones; k++)
				{
					const AnimationCurveMapping& mapping = state.boneToCurveMapping[k];

					if (mapping.position != (UINT32)-1)
					{
						const TAnimationCurve<Vector3>& curve = state.curves->position[mapping.position].curve;
						pose.positions[k] += curve.evaluate(state.positionEval, state.loop) * normWeight;
					}

					if (mapping.rotation != (UINT32)-1)
					{
						const TAnimationCurve<Quaternion>& curve = state.curves->rotation[mapping.rotation].curve;
						pose.rotations[k] += curve.evaluate(state.rotationEval, state.loop) * normWeight;
					}

					if (mapping.scale != (UINT32)-1)
					{
						const TAnimationCurve<Vector3>& curve = state.curves->scale[mapping.scale].curve;
						pose.scales[k] += curve.evaluate(state.scaleEval, state.loop) * normWeight;
					}
				}
			}
		}

		// Calculate local pose matrices
		for(UINT32 i = 0; i < mNumBones; i++)
		{
			pose.rotations[i].normalize();

			pose.bonePoses[i] = Matrix4::TRS(pose.positions[i], pose.rotations[i], pose.scales[i]);
			pose.bonePoses[i] = pose.bonePoses[i] * mInvBindPoses[i];
		}

		// Calculate global poses
		UINT32 isGlobalBytes = sizeof(bool) * mNumBones;
		bool* isGlobal = (bool*)bs_stack_alloc(isGlobalBytes);
		memset(isGlobal, 0, isGlobalBytes);

		std::function<void(UINT32)> calcGlobal = [&](UINT32 boneIdx)
		{
			UINT32 parentBoneIdx = mBoneInfo[boneIdx].parent;
			if (parentBoneIdx == (UINT32)-1)
			{
				isGlobal[boneIdx] = true;
				return;
			}

			if (!isGlobal[parentBoneIdx])
				calcGlobal(parentBoneIdx);

			pose.bonePoses[boneIdx] = pose.bonePoses[parentBoneIdx] * pose.bonePoses[boneIdx];
			isGlobal[boneIdx] = true;
		};

		for (UINT32 i = 0; i < mNumBones; i++)
		{
			if (!isGlobal[i])
				calcGlobal(i);
		}

		bs_stack_free(isGlobal);
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