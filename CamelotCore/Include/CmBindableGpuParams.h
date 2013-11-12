#pragma once

#include "CmPrerequisites.h"

namespace CamelotFramework
{
	/**
		* @brief	Specialized class for binding GPU parameters to the render system. This is a temporary class that
		* 			is used for temporarily saving parameter data while parameters are scheduled to be bound to the GPU.
		* 			This allows us to freely modify base GpuParams without worrying about changing scheduled by still 
		* 			not executed parameter binds.
		* 			
		* @note		Upon assignment this class transfers ownership of its data. Internal data
		* 			is destroyed when last assigned instance goes out of scope.
		* 			(In short, you should never have more than one active copy of an instance of this class)
		* 			
		*			Created on the sim thread and used exclusively on the core thread.
	**/
	class CM_EXPORT BindableGpuParams
	{
	public:
		BindableGpuParams(const BindableGpuParams& source);
		~BindableGpuParams();

		/**
		 * @brief	Updates all used hardware parameter buffers. Should ONLY be called from core thread.
		 */
		void updateHardwareBuffers();

		GpuParamBlockBufferPtr getParamBlockBuffer(UINT32 slot) const;
		GpuParamBlockBufferPtr getParamBlockBuffer(const String& name) const;
		HTexture getTexture(UINT32 slot);
		HSamplerState getSamplerState(UINT32 slot);

		const GpuParamDesc& getParamDesc() const { return mParamDesc; }

	private:
		friend class GpuParams;

		BindableGpuParams(const GpuParamDesc& sourceParamDesc);

	private:
		mutable bool mOwnsData;
		const GpuParamDesc& mParamDesc;
		UINT8* mData;

		UINT32 mNumParamBlocks;
		UINT32 mNumTextures;
		UINT32 mNumSamplerStates;

		BindableGpuParamBlock** mParamBlocks;
		GpuParamBlockBufferPtr* mParamBlockBuffers;
		HTexture* mTextures;
		HSamplerState* mSamplerStates;
	};
}