#pragma once

#include "CmPrerequisites.h"
#include "CmCommonEnums.h"
#include "CmCoreObject.h"

namespace BansheeEngine
{
	/**
	* @note	Due to the way allocation is handled, this class is not allowed to have a destructor.
	*/
	class CM_EXPORT BindableGpuParamBlock
	{
	public:
		BindableGpuParamBlock();

		/**
		 * @brief	Uploads the current data to the specified buffer, and marks the block a non-dirty.
		 * 			Should only be called from the core thread.
		 */
		void uploadToBuffer(GpuParamBlockBufferPtr buffer);

		bool isDirty() const { return mDirty; }
	protected:
		friend class GpuParams;
		friend class BindableGpuParams;

		UINT8* mData;
		UINT32 mSize;
		bool mDirty;
	};
}