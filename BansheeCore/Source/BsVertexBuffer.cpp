#include "BsVertexBuffer.h"
#include "BsColor.h"
#include "BsException.h"
#include "BsHardwareBufferManager.h"
#include "BsRenderSystem.h"
#include "BsVertexDeclarationRTTI.h"

namespace BansheeEngine 
{
    VertexBuffer::VertexBuffer(UINT32 vertexSize,  UINT32 numVertices, GpuBufferUsage usage, bool useSystemMemory) 
        : HardwareBuffer(usage, useSystemMemory), mNumVertices(numVertices), mVertexSize(vertexSize)
    {
        mSizeInBytes = mVertexSize * numVertices;
    }

	VertexBufferPtr VertexBuffer::create(UINT32 vertexSize, UINT32 numVerts, GpuBufferUsage usage, bool streamOut)
	{
		return HardwareBufferManager::instance().createVertexBuffer(vertexSize, numVerts, usage, streamOut);
	}
}
