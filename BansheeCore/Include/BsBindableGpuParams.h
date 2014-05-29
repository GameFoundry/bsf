#pragma once

#include "BsCorePrerequisites.h"

namespace BansheeEngine
{
	/**
	 * @brief	Specialized class for binding GPU parameters to the render system. This is a temporary class that
	 * 			is used for temporarily saving parameter data while parameters are scheduled to be bound to the GPU.
	 * 			This allows us to freely modify base GpuParams without worrying about changing scheduled by still 
	 * 			not executed parameter binds.
	 * 			
	 * @note	Upon assignment this class transfers ownership of its internal data. Internal data
	 * 			is destroyed when last assigned instance goes out of scope.
	 * 			(In short, you should never have more than one active copy of an instance of this class)
	 * 			
	 *			Created on the sim thread and used exclusively on the core thread.
	 *			
	 * @see		CoreThreadAccessorBase::bindGpuParams
	**/
	class BS_CORE_EXPORT BindableGpuParams
	{
	public:
		BindableGpuParams(const GpuParamsPtr& sourceParams, FrameAlloc* allocator);
		BindableGpuParams(const BindableGpuParams& source);
		~BindableGpuParams();

		/**
		 * @brief	Uploads all CPU stored parameter buffer data to the GPU buffers.
		 */
		void updateHardwareBuffers();

		/**
		 * @brief	Gets a parameter block buffer from the specified slot.
		 */
		GpuParamBlockBufferPtr getParamBlockBuffer(UINT32 slot) const;

		/**
		 * @brief	Gets a parameter block buffer by name.
		 */
		GpuParamBlockBufferPtr getParamBlockBuffer(const String& name) const;

		/**
		 * @brief	Gets a texture bound to the specified slot.
		 */
		HTexture getTexture(UINT32 slot);

		/**
		 * @brief	Gets a sampler state bound to the specified slot.
		 */
		HSamplerState getSamplerState(UINT32 slot);

		/**
		 * @brief	Gets a description of all parameters stored in this class.
		 */
		const GpuParamDesc& getParamDesc() const { return mParamDesc; }

	private:
		mutable bool mOwnsData;
		const GpuParamDesc& mParamDesc;
		UINT8* mData;

		UINT32 mNumParamBlocks;
		UINT32 mNumTextures;
		UINT32 mNumSamplerStates;

		FrameAlloc* mAllocator;
		BindableGpuParamBlock** mParamBlocks;
		GpuParamBlockBufferPtr* mParamBlockBuffers;
		HTexture* mTextures;
		HSamplerState* mSamplerStates;
	};
}