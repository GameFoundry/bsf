#include "CmVertexBuffer.h"
#include "CmColor.h"
#include "CmException.h"
#include "CmHardwareBufferManager.h"
#include "CmRenderSystem.h"
#include "CmVertexDeclarationRTTI.h"

namespace BansheeEngine 
{
    VertexBuffer::VertexBuffer(UINT32 vertexSize,  UINT32 numVertices, GpuBufferUsage usage, bool useSystemMemory) 
        : HardwareBuffer(usage, useSystemMemory), mNumVertices(numVertices), mVertexSize(vertexSize)
    {
        mSizeInBytes = mVertexSize * numVertices;
    }
}
