#include "BsD3D9HardwareBufferManager.h"
#include "BsD3D9VertexBuffer.h"
#include "BsD3D9IndexBuffer.h"
#include "BsD3D9VertexDeclaration.h"
#include "BsD3D9GpuBuffer.h"
#include "BsGpuParamBlockBuffer.h"
#include "BsException.h"

namespace BansheeEngine 
{
	SPtr<VertexBufferCore> D3D9HardwareBufferCoreManager::createVertexBufferInternal(UINT32 vertexSize, UINT32 numVerts, GpuBufferUsage usage, bool streamOut)
	{
		assert(numVerts > 0);

		SPtr<D3D9VertexBufferCore> ret = bs_shared_ptr<D3D9VertexBufferCore>(vertexSize, numVerts, usage, streamOut);
		ret->_setThisPtr(ret);

		return ret;
	}

	SPtr<IndexBufferCore> D3D9HardwareBufferCoreManager::createIndexBufferInternal(IndexType itype, UINT32 numIndexes, GpuBufferUsage usage)
	{
		assert(numIndexes > 0);

		SPtr<D3D9IndexBufferCore> ret = bs_shared_ptr<D3D9IndexBufferCore>(itype, numIndexes, usage);
		ret->_setThisPtr(ret);

		return ret;
	}

	SPtr<VertexDeclarationCore> D3D9HardwareBufferCoreManager::createVertexDeclarationInternal(const List<VertexElement>& elements)
	{
		D3D9VertexDeclarationCore* decl = new (bs_alloc<D3D9VertexDeclarationCore, GenAlloc>()) D3D9VertexDeclarationCore(elements);
		
		SPtr<D3D9VertexDeclarationCore> declPtr = bs_shared_ptr<D3D9VertexDeclarationCore, GenAlloc>(decl);
		declPtr->_setThisPtr(declPtr);

		return declPtr;
	}

	SPtr<GpuParamBlockBufferCore> D3D9HardwareBufferCoreManager::createGpuParamBlockBufferInternal(UINT32 size, GpuParamBlockUsage usage)
	{
		GenericGpuParamBlockBufferCore* paramBlockBuffer = new (bs_alloc<GenericGpuParamBlockBufferCore, GenAlloc>()) GenericGpuParamBlockBufferCore(size, usage);

		SPtr<GpuParamBlockBufferCore> paramBlockBufferPtr = bs_shared_ptr<GenericGpuParamBlockBufferCore, GenAlloc>(paramBlockBuffer);
		paramBlockBufferPtr->_setThisPtr(paramBlockBufferPtr);

		return paramBlockBufferPtr;
	}

	SPtr<GpuBufferCore> D3D9HardwareBufferCoreManager::createGpuBufferInternal(UINT32 elementCount, UINT32 elementSize,
		GpuBufferType type, GpuBufferUsage usage, bool randomGpuWrite, bool useCounter)
	{
		D3D9GpuBufferCore* buffer = new (bs_alloc<D3D9GpuBufferCore, GenAlloc>()) D3D9GpuBufferCore(elementCount, elementSize, type, usage, randomGpuWrite, useCounter);

		SPtr<GpuBufferCore> bufferPtr = bs_shared_ptr<D3D9GpuBufferCore, GenAlloc>(buffer);
		bufferPtr->_setThisPtr(bufferPtr);

		return bufferPtr;
	}
}
