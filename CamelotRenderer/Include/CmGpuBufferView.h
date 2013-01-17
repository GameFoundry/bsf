#pragma once

#include "CmPrerequisites.h"
#include "CmCommonEnums.h"

namespace CamelotEngine
{
	struct CM_EXPORT GPU_BUFFER_DESC
	{
		UINT32 firstElement;
		UINT32 elementWidth;
		UINT32 numElements;
		bool useCounter;
		GpuViewUsage usage;
	};

	class CM_EXPORT GpuBufferView
	{
	public:
		class HashFunction
		{
		public:
			size_t operator()(const GPU_BUFFER_DESC& key) const;
		};

		class EqualFunction
		{
		public:
			bool operator()(const GPU_BUFFER_DESC& a, const GPU_BUFFER_DESC& b) const;
		};

		GpuBufferView(GPU_BUFFER_DESC& desc);
		virtual ~GpuBufferView();

		const GPU_BUFFER_DESC& getDesc() const { return mDesc; }

	private:
		GPU_BUFFER_DESC mDesc;
	};
}