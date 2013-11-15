#pragma once

#include "CmPrerequisites.h"
#include "CmCommonEnums.h"
#include "CmCoreObject.h"

namespace CamelotFramework
{
	class CM_EXPORT GpuParamBlock
	{
	public:
		GpuParamBlock(UINT32 size);
		GpuParamBlock(GpuParamBlock* otherBlock);

		virtual ~GpuParamBlock();

		void write(UINT32 offset, const void* data, UINT32 size);
		void read(UINT32 offset, void* data, UINT32 size);
		void zeroOut(UINT32 offset, UINT32 size);

		UINT32 getSize() const { return mSize; }
		UINT8* getData() const { return mData; }
		bool isDirty() const { return mDirty; }

		void setDirty() { mDirty = true; }
	protected:
		UINT8* mData;
		UINT32 mSize;
		bool mDirty;
	};
}