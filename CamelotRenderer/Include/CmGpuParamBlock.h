#pragma once

#include "CmPrerequisites.h"

namespace CamelotEngine
{
	class GpuParamBlock
	{
	public:
		GpuParamBlock(UINT32 size);
		~GpuParamBlock();

		void write(UINT32 offset, const void* data, UINT32 size);
		void zeroOut(UINT32 offset, UINT32 size);

		virtual void updateIfDirty();
	private:
		bool mDirty;
		UINT8* mData;
		UINT32 mSize;
	};
}