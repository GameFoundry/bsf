//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#include "BsSkeletonEx.h"

using namespace std::placeholders;

namespace bs
{
	SkeletonBoneInfoEx SkeletonEx::getBoneInfo(const SPtr<Skeleton>& thisPtr, int boneIdx)
	{
		const SkeletonBoneInfo& boneInfo = thisPtr->getBoneInfo(boneIdx);
		SkeletonBoneInfoEx boneInfoEx;
		boneInfoEx.name = boneInfo.name;
		boneInfoEx.parent = boneInfo.parent;
		boneInfoEx.invBindPose = thisPtr->getInvBindPose(boneIdx);

		return boneInfoEx;
	}
}