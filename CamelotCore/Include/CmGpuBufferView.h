#pragma once

#include "CmPrerequisites.h"
#include "CmCommonEnums.h"

namespace CamelotFramework
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

		GpuBufferView();
		virtual ~GpuBufferView();

		virtual void initialize(GpuBufferPtr buffer, GPU_BUFFER_DESC& desc);

		const GPU_BUFFER_DESC& getDesc() const { return mDesc; }
		GpuBufferPtr getBuffer() const { return mBuffer; }

		UINT32 getFirstElement() const { return mDesc.firstElement; }
		UINT32 getElementWidth() const { return mDesc.elementWidth; }
		UINT32 getNumElements() const { return mDesc.numElements; }
		bool getUseCounter() const { return mDesc.useCounter; }
		GpuViewUsage getUsage() const { return mDesc.usage; }

	protected:
		GPU_BUFFER_DESC mDesc;
		GpuBufferPtr mBuffer;
	};
}