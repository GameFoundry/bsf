#include "BsVertexBuffer.h"
#include "BsColor.h"
#include "BsException.h"
#include "BsHardwareBufferManager.h"
#include "BsRenderSystem.h"
#include "BsVertexDeclarationRTTI.h"

namespace BansheeEngine 
{
	VertexBufferCore::VertexBufferCore(GpuBufferUsage usage, bool useSystemMemory, const VertexBufferProperties& properties)
		:HardwareBuffer(usage, useSystemMemory), mProperties(properties)
	{
		mSizeInBytes = mProperties.mVertexSize * mProperties.mNumVertices;
	}

    VertexBuffer::VertexBuffer(UINT32 vertexSize, UINT32 numVertices, GpuBufferUsage usage, bool useSystemMemory) 
		: mUseSystemMemory(useSystemMemory), mUsage(usage)
    {
		mProperties.mVertexSize = vertexSize;
		mProperties.mNumVertices = numVertices;
    }

	SPtr<VertexBufferCore> VertexBuffer::getCore() const
	{
		return std::static_pointer_cast<VertexBufferCore>(mCoreSpecific);
	}

	VertexBufferPtr VertexBuffer::create(UINT32 vertexSize, UINT32 numVerts, GpuBufferUsage usage, bool streamOut)
	{
		return HardwareBufferManager::instance().createVertexBuffer(vertexSize, numVerts, usage, streamOut);
	}
}
