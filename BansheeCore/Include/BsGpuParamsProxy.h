#pragma once

#include "BsCorePrerequisites.h"

namespace BansheeEngine
{
	/**
	 * @brief	Contains a snapshot of GpuParams data. It can be used for
	 *			creating a temporary or permanent copy of GpuParams data for use
	 *			on another thread.
	**/
	class BS_CORE_EXPORT GpuParamsProxy
	{
	public:
		GpuParamsProxy(const GpuParamsPtr& sourceParams, FrameAlloc* allocator);
		~GpuParamsProxy();

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
		const GpuParamDesc& mParamDesc;
		UINT8* mData;

		UINT32 mNumParamBlocks;
		UINT32 mNumTextures;
		UINT32 mNumSamplerStates;

		FrameAlloc* mAllocator;
		GpuParamsBlockProxy** mParamBlocks;
		GpuParamBlockBufferPtr* mParamBlockBuffers;
		HTexture* mTextures;
		HSamplerState* mSamplerStates;
	};
}