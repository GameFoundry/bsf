#pragma once

#include "CmPrerequisites.h"
#include "CmResource.h"

namespace CamelotFramework
{
	class CM_EXPORT GpuResource : public Resource
	{
	public:
		virtual void writeSubresource(UINT32 subresourceIdx, const GpuResourceData& data) = 0;
		virtual void readSubresource(UINT32 subresourceIdx, GpuResourceData& data) = 0;

		/************************************************************************/
		/* 								SERIALIZATION                      		*/
		/************************************************************************/
	public:
		friend class GpuResourceRTTI;
		static RTTITypeBase* getRTTIStatic();
		virtual RTTITypeBase* getRTTI() const;
	};
}