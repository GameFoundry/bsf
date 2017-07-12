//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#pragma once

#include "BsScriptEnginePrerequisites.h"
#include "BsScriptResource.h"
#include "BsSkeleton.h"

namespace bs
{
	/** @addtogroup ScriptInteropEngine
	 *  @{
	 */
	/** @cond SCRIPT_EXTENSIONS */

	/** Contains internal information about a single bone in a Skeleton. */
	struct BS_SCRIPT_EXPORT(n:BoneInfo,pl:true) SkeletonBoneInfoEx
	{
		/** Unique name of the bone. */
		String name;

		/** Index of the parent bone (within the relevant Skeleton object). -1 if root bone. */
		int parent;

		/** Inverse transform of the pose the skeleton was initially created in. */
		Matrix4 invBindPose;
	};

	/** Extension class for Skeleton, for adding additional functionality for the script version of the class. */
	class BS_SCRIPT_EXPORT(e:Skeleton) SkeletonEx
	{
	public:
		/** Returns information about a bone at the provided index.
		 *
		 * @param[in] boneIdx	Index of the bone to retrieve information for.
		 * @return				Information about the bone at the specified index.
		 */
		BS_SCRIPT_EXPORT(e:Skeleton,n:GetBoneInfo)
		static SkeletonBoneInfoEx getBoneInfo(const SPtr<Skeleton>& thisPtr, int boneIdx);
	};

	/** @endcond */
	/** @} */
}
