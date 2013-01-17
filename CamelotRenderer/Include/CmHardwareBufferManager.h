/*
-----------------------------------------------------------------------------
This source file is part of OGRE
    (Object-oriented Graphics Rendering Engine)
For the latest info, see http://www.ogre3d.org/

Copyright (c) 2000-2011 Torus Knot Software Ltd

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
-----------------------------------------------------------------------------
*/
#ifndef __HardwareBufferManager__
#define __HardwareBufferManager__

// Precompiler options
#include "CmPrerequisites.h"

#include "CmModule.h"
#include "CmVertexBuffer.h"
#include "CmIndexBuffer.h"
#include "CmGpuParamBlock.h"

namespace CamelotEngine {
	/** \addtogroup Core
	*  @{
	*/
	/** \addtogroup RenderSystem
	*  @{
	*/

	/** Base definition of a hardware buffer manager.
	*/
	class CM_EXPORT HardwareBufferManager : public Module<HardwareBufferManager>
	{
    protected:
        /** WARNING: The following three members should place before all other members.
            Members destruct order is very important here, because destructing other
            members will cause notify back to this class, and then will access to this
            two members.
        */
        typedef set<VertexBuffer*>::type VertexBufferList;
        typedef set<IndexBuffer*>::type IndexBufferList;
        VertexBufferList mVertexBuffers;
        IndexBufferList mIndexBuffers;

        /// Internal method for creates a new vertex declaration, may be overridden by certain rendering APIs
        virtual VertexDeclarationPtr createVertexDeclarationImpl(void);

    public:
        HardwareBufferManager();
        virtual ~HardwareBufferManager();
		/** Create a hardware vertex buffer.
        @remarks
            This method creates a new vertex buffer; this will act as a source of geometry
            data for rendering objects. Note that because the meaning of the contents of
            the vertex buffer depends on the usage, this method does not specify a
            vertex format; the user of this buffer can actually insert whatever data 
            they wish, in any format. However, in order to use this with a RenderOperation,
            the data in this vertex buffer will have to be associated with a semantic element
            of the rendering pipeline, e.g. a position, or texture coordinates. This is done 
            using the VertexDeclaration class, which itself contains VertexElement structures
            referring to the source data.
        @remarks Note that because vertex buffers can be shared, they are reference
            counted so you do not need to worry about destroying themm this will be done
            automatically.
        @param vertexSize The size in bytes of each vertex in this buffer; you must calculate
            this based on the kind of data you expect to populate this buffer with.
        @param numVerts The number of vertices in this buffer.
        @param usage One or more members of the HardwareBuffer::Usage enumeration; you are
            strongly advised to use HBU_STATIC_WRITE_ONLY wherever possible, if you need to 
            update regularly, consider HBU_DYNAMIC_WRITE_ONLY and useShadowBuffer=true.
		@param streamOut Whether the vertex buffer will be used for steam out operations of the
			geometry shader.
        */
		virtual VertexBufferPtr 
            createVertexBuffer(UINT32 vertexSize, UINT32 numVerts, GpuBufferUsage usage, bool streamOut = false) = 0;

		/** Create a hardware index buffer.
        @remarks Note that because buffers can be shared, they are reference
            counted so you do not need to worry about destroying them this will be done
            automatically.
		@param itype The type in index, either 16- or 32-bit, depending on how many vertices
			you need to be able to address
		@param numIndexes The number of indexes in the buffer
        @param usage One or more members of the HardwareBuffer::Usage enumeration.
        */
		virtual IndexBufferPtr 
            createIndexBuffer(IndexBuffer::IndexType itype, UINT32 numIndexes, GpuBufferUsage usage) = 0;

		/**
		 * @brief	Creates an GPU parameter block that you can use for setting parameters
		 * 			for GPU programs.
		 *
		 * @return	The new GPU parameter block.
		 */
		virtual GpuParamBlockPtr createGpuParamBlock(const GpuParamBlockDesc& paramDesc) = 0;

		/**
		 * @brief	Creates a generic buffer that can be passed as a parameter to a shader.
		 *
		 * @param	elementCount  	Number of elements in the buffer. 
		 * @param	elementSize   	Size of each individual element in the buffer, in bytes.
		 * @param	type		  	Type of the buffer.
		 * @param	usage		  	Determines how will the buffer be used.
		 * @param	randomGpuWrite	(optional) Allows the GPU to write to the resource.
		 * @param	useCounter	  	(optional) Binds a counter that can be used from a shader to the buffer.
		 *
		 * Be aware that some of these settings cannot be used together, and you will receive an assert if in debug mode.
		 */
		virtual GpuBufferPtr createGpuBuffer(UINT32 elementCount, UINT32 elementSize, 
			GpuBufferType type, GpuBufferUsage usage, bool randomGpuWrite = false, bool useCounter = false) = 0;

        /** Creates a new vertex declaration. */
        virtual VertexDeclarationPtr createVertexDeclaration(void);

		/// Notification that a hardware vertex buffer has been destroyed
		void _notifyVertexBufferDestroyed(VertexBuffer* buf);
		/// Notification that a hardware index buffer has been destroyed
		void _notifyIndexBufferDestroyed(IndexBuffer* buf);
	};

	/** @} */
	/** @} */
}

#endif

