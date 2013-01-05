#pragma once

#include "CmPrerequisites.h"

namespace CamelotEngine
{
	class CM_EXPORT GpuParamBlock
	{
	private:
		struct GpuParamBlockSharedData
		{
			bool mDirty;
			bool mInitialized;
		};

	public:
		GpuParamBlock(const GpuParamBlockDesc& desc);
		virtual ~GpuParamBlock();

		void write(UINT32 offset, const void* data, UINT32 size);
		void zeroOut(UINT32 offset, UINT32 size);

		const UINT8* getDataPtr(UINT32 offset) const;
		UINT32 getSize() const { return mSize; }

		virtual void updateIfDirty();

		virtual GpuParamBlockPtr clone() const;
		
		static GpuParamBlockPtr create(const GpuParamBlockDesc& desc);
	protected:
		GpuParamBlockSharedData* sharedData;
		bool mOwnsSharedData;
		UINT8* mData;
		UINT32 mSize;
	};
}