//__________________________ Banshee Project - A modern game development toolkit _________________________________//
//_____________________________________ www.banshee-project.com __________________________________________________//
//________________________ Copyright (c) 2014 Marko Pintera. All rights reserved. ________________________________//
#pragma once

#include "BsCorePrerequisites.h"
#include "BsResource.h"

namespace BansheeEngine
{
	/**
	 * @brief	Represents a resource that in some way deals directly with the rendering
	 *			API and the GPU.
	 *
	 * @note	e.g. texture, mesh, buffer, etc.
	 */
	class BS_CORE_EXPORT GpuResource : public Resource
	{
	public:
		/**
		 * @brief	Called just before writeSubresource is queued. Called from sim thread.
		 *
		 * @note	Sim thread only.
		 */
		virtual void _writeSubresourceSim(UINT32 subresourceIdx, const GpuResourceData& data, bool discardEntireBuffer) { }

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