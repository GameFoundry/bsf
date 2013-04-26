#pragma once

#include "CmPrerequisites.h"

namespace CamelotFramework
{
	class CM_EXPORT GpuBufferData
	{
	public:
		GpuBufferData();
		~GpuBufferData();

		UINT8* getData() const;

	protected:
		void initialize(UINT32 size);

		void lock();
		void unlock();

		UINT8* mData;
		bool mLocked;
	};
}