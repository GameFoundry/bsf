//__________________________ Banshee Project - A modern game development toolkit _________________________________//
//_____________________________________ www.banshee-project.com __________________________________________________//
//________________________ Copyright (c) 2014 Marko Pintera. All rights reserved. ________________________________//
#pragma once

#include "BsCorePrerequisites.h"
#include "BsHardwareBuffer.h"
#include "BsCoreObject.h"
#include "BsColor.h"

namespace BansheeEngine 
{
	/**
	 * @brief	Specialization of a hardware buffer used for holding vertex data.
	 */
    class BS_CORE_EXPORT VertexBuffer : public HardwareBuffer, public CoreObject
    {
	public:
		virtual ~VertexBuffer() { }

		/**
		 * @brief	Gets the size in bytes of a single vertex in this buffer.
		 */
        UINT32 getVertexSize() const { return mVertexSize; }

		/**
		 * @brief	Get the number of vertices in this buffer.
		 */
        UINT32 getNumVertices() const { return mNumVertices; }

		/**
		 * @brief	Some render systems expect vertex color bits in an order different than
		 * 			RGBA, in which case override this to flip the RGBA order.
		 */
		virtual bool vertexColorReqRGBFlip() { return false; }

		static const int MAX_SEMANTIC_IDX = 8;
	protected:
		VertexBuffer(UINT32 vertexSize, UINT32 numVertices, GpuBufferUsage usage, bool useSystemMemory);

	protected:
		UINT32 mNumVertices;
		UINT32 mVertexSize;
    };
}