#pragma once

#include "CmPrerequisites.h"

namespace CamelotFramework
{
	class CM_EXPORT GpuResourceData
	{
	public:
		GpuResourceData();
		~GpuResourceData();

	protected:
		friend class DeferredRenderContext;
		friend class RenderSystem;

		void initialize(UINT32 size);

		UINT8* getData() const;

		void lock() const;
		void unlock() const;

	private:
		UINT8* mData;
		mutable bool mLocked;
	};
}