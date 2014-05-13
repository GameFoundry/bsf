#pragma once

#include "CmPrerequisites.h"
#include "CmModule.h"
#include "CmVertexBuffer.h"
#include "CmIndexBuffer.h"
#include "CmVertexDeclaration.h"
#include "CmGpuParamBlock.h"

namespace BansheeEngine 
{
	/**
	 * @brief	Handles creation of various hardware buffers.
	 *
	 * @note	Thread safe.
	 */
	class CM_EXPORT HardwareBufferManager : public Module<HardwareBufferManager>
	{
    public:
        HardwareBufferManager();
        virtual ~HardwareBufferManager();

		/**
		 * @brief	Creates a new vertex buffer used for holding number of vertices and other
		 *			per-vertex data. Buffer can be bound to the pipeline and its data can
		 *			be passed to the active vertex GPU program.
		 *
		 * @param	vertexSize	Size of a single vertex in the buffer, in bytes.
		 * @param	numVerts	Number of vertices the buffer can hold.
		 * @param	usage		Usage that tells the hardware how will be buffer be used. 
		 * @param	streamOut	If true the buffer will be usable for streaming out data from the GPU.
		 */
		virtual VertexBufferPtr createVertexBuffer(UINT32 vertexSize, UINT32 numVerts, GpuBufferUsage usage, bool streamOut = false);

		/**
		 * @brief	Creates a new index buffer that holds indices referencing vertices in a vertex buffer.
		 *			Indices are interpreted by the pipeline and vertices are drawn in the order specified in
		 *			the index buffer.
		 *
		 * @param	itype		Index type, determines size of an index.
		 * @param	numIndexes	Number of indexes can buffer can hold.
		 * @param	usage		Usage that tells the hardware how will be buffer be used. 
		 */
		virtual IndexBufferPtr createIndexBuffer(IndexBuffer::IndexType itype, UINT32 numIndexes, GpuBufferUsage usage);

		/**
		 * @brief	Creates an GPU parameter block that you can use for setting parameters for GPU programs.
		 *			Parameter blocks may be used for sharing parameter data between multiple GPU programs, requiring
		 *			you to update only one buffer for all of them, potentially improving performance.
		 *
		 * @param	size	Size of the parameter buffer in bytes.
		 * @param	usage	Usage that tells the hardware how will be buffer be used. 
		 */
		virtual GpuParamBlockBufferPtr createGpuParamBlockBuffer(UINT32 size, GpuParamBlockUsage usage = GPBU_DYNAMIC);

		/**
		 * @brief	Creates a generic buffer that can be passed as a parameter to a shader. This type of buffer can hold
		 *			various type of data and can be used for various purposes. See "GpuBufferType" for explanation of
		 *			different buffer types.
		 *
		 * @param	elementCount  	Number of elements in the buffer. 
		 * @param	elementSize   	Size of each individual element in the buffer, in bytes.
		 * @param	type		  	Type of the buffer.
		 * @param	usage		  	Usage that tells the hardware how will be buffer be used. 
		 * @param	randomGpuWrite	(optional) Allows the GPU to write to the resource.
		 * @param	useCounter	  	(optional) Binds a counter that can be used from a GPU program on the buffer.
		 *
		 * @note	Be aware that due to some render API restrictions some of these settings cannot be used together, 
		 *			and if so you will receive an assert in debug mode.
		 */
		virtual GpuBufferPtr createGpuBuffer(UINT32 elementCount, UINT32 elementSize, 
			GpuBufferType type, GpuBufferUsage usage, bool randomGpuWrite = false, bool useCounter = false);

		/**
		 * @brief	Creates a new vertex declaration from a list of vertex elements.
		 */
		virtual VertexDeclarationPtr createVertexDeclaration(const VertexDeclaration::VertexElementList& elements);

	protected:
		/**
		 * @copydoc	createVertexBuffer
		 */
		virtual VertexBufferPtr createVertexBufferImpl(UINT32 vertexSize, UINT32 numVerts, GpuBufferUsage usage, bool streamOut = false) = 0;

		/**
		 * @copydoc	createIndexBuffer
		 */
		virtual IndexBufferPtr createIndexBufferImpl(IndexBuffer::IndexType itype, UINT32 numIndexes, GpuBufferUsage usage) = 0;

		/**
		 * @copydoc	createGpuParamBlockBuffer
		 */
		virtual GpuParamBlockBufferPtr createGpuParamBlockBufferImpl() = 0;

		/**
		 * @copydoc	createGpuBuffer
		 */
		virtual GpuBufferPtr createGpuBufferImpl(UINT32 elementCount, UINT32 elementSize, GpuBufferType type, GpuBufferUsage usage, 
			bool randomGpuWrite = false, bool useCounter = false) = 0;

		/**
		 * @copydoc	createVertexDeclaration
		 */
		virtual VertexDeclarationPtr createVertexDeclarationImpl(const VertexDeclaration::VertexElementList& elements);
	};
}

