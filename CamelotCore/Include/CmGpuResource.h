#pragma once

#include "CmPrerequisites.h"
#include "CmResource.h"

namespace CamelotFramework
{
	class CM_EXPORT GpuResource : public Resource
	{
	public:
		/**
		 * @brief	Updates a part of the current resource with the provided data. Specific resource
		 * 			implementations provide a way to retrieve a subresource index.
		 * 			
		 * @note	Core thread only.
		 */
		virtual void writeSubresource(UINT32 subresourceIdx, const GpuResourceData& data, bool discardEntireBuffer) = 0;

		/**
		 * @brief	Reads a part of the current resource into the provided "data" parameter.
		 * 			Data buffer needs to be pre-allocated. Specific resource implementations 
		 * 			provide a way to retrieve a subresource index and a way to allocate
		 * 			the GpuResourceData buffer.
		 * 			
		 * @note	Core thread only.
		 */
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