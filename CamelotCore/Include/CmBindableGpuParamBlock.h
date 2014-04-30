#pragma once

#include "CmPrerequisites.h"
#include "CmCommonEnums.h"
#include "CmCoreObject.h"

namespace BansheeEngine
{
	/**
	 * @brief	Specialized class for binding GPU parameters to the render system. Represents a single
	 * 			parameter block buffer and is used for temporary storage of GPU parameters.
	 * 
	 * @note	Due to the way allocation is handled, this class is not allowed to have a destructor.
	 * 			
	 * @see		BindableGpuParams
 	 */
	class CM_EXPORT BindableGpuParamBlock
	{
	public:
		BindableGpuParamBlock();

		/**
		 * @brief	Uploads the current data to the specified buffer, and marks the block a non-dirty.
		 * 			
		 * @note	Core thread only.			
		 */
		void uploadToBuffer(GpuParamBlockBufferPtr buffer);

		/**
		 * @brief	Query if this object is dirty. If dirty the data of this block
		 * 			will need to be uploaded to a GPU buffer.
		 *
		 * @note	Core thread only.
		 */
		bool isDirty() const { return mDirty; }
	protected:
		friend class GpuParams;
		friend class BindableGpuParams;

		UINT8* mData;
		UINT32 mSize;
		bool mDirty;
	};
}