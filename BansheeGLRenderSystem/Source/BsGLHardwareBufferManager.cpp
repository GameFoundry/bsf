#include "BsGLHardwareBufferManager.h"
#include "BsGLVertexBuffer.h"
#include "BsGLIndexBuffer.h"
#include "BsGLGpuBuffer.h"
#include "BsHardwareBuffer.h"
#include "BsGLGpuParamBlockBuffer.h"
#include "BsRenderSystem.h"
#include "BsRenderSystemCapabilities.h"

namespace BansheeEngine 
{
    VertexBufferPtr GLHardwareBufferManager::createVertexBufferImpl(
        UINT32 vertexSize, UINT32 numVerts, GpuBufferUsage usage, bool streamOut)
    {
		return bs_core_ptr<GLVertexBuffer, PoolAlloc>(new (bs_alloc<GLVertexBuffer, PoolAlloc>()) GLVertexBuffer(vertexSize, numVerts, usage));
    }

    IndexBufferPtr GLHardwareBufferManager::createIndexBufferImpl(IndexType itype, UINT32 numIndexes, GpuBufferUsage usage)
    {
		return bs_core_ptr<GLIndexBuffer, PoolAlloc>(new (bs_alloc<GLIndexBuffer, PoolAlloc>()) GLIndexBuffer(itype, numIndexes, usage));
    }

	GpuParamBlockBufferPtr GLHardwareBufferManager::createGpuParamBlockBufferImpl()
	{
		return bs_core_ptr<GLGpuParamBlockBuffer, PoolAlloc>(new (bs_alloc<GLGpuParamBlockBuffer, PoolAlloc>()) GLGpuParamBlockBuffer());
	}

	GpuBufferPtr GLHardwareBufferManager::createGpuBufferImpl(UINT32 elementCount, UINT32 elementSize, 
		GpuBufferType type, GpuBufferUsage usage, bool randomGpuWrite, bool useCounter)
	{
		return bs_core_ptr<GLGpuBuffer, PoolAlloc>(new (bs_alloc<GLGpuBuffer, PoolAlloc>()) GLGpuBuffer(elementCount, elementSize, type, usage, randomGpuWrite, useCounter));
	}

    GLenum GLHardwareBufferManager::getGLUsage(GpuBufferUsage usage)
    {
        switch(usage)
        {
        case GBU_STATIC:
            return GL_STATIC_DRAW;
        case GBU_DYNAMIC:
            return GL_DYNAMIC_DRAW;
        default:
            return GL_DYNAMIC_DRAW;
        };
    }

    GLenum GLHardwareBufferManager::getGLType(VertexElementType type)
    {
        switch(type)
        {
            case VET_FLOAT1:
            case VET_FLOAT2:
            case VET_FLOAT3:
            case VET_FLOAT4:
                return GL_FLOAT;
            case VET_SHORT1:
            case VET_SHORT2:
            case VET_SHORT3:
            case VET_SHORT4:
                return GL_SHORT;
            case VET_COLOR:
			case VET_COLOR_ABGR:
			case VET_COLOR_ARGB:
            case VET_UBYTE4:
                return GL_UNSIGNED_BYTE;
            default:
                return 0;
        };
    }
}
