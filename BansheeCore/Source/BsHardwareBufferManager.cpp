#include "BsHardwareBufferManager.h"
#include "BsVertexData.h"
#include "BsGpuBuffer.h"
#include "BsVertexDeclaration.h"
#include "BsGpuParamBlockBuffer.h"

namespace BansheeEngine 
{
    HardwareBufferManager::HardwareBufferManager()
    {
    }

    HardwareBufferManager::~HardwareBufferManager()
    {
    }

	VertexDeclarationPtr HardwareBufferManager::createVertexDeclaration(const List<VertexElement>& elements)
    {
		VertexDeclaration* decl = new (bs_alloc<VertexDeclaration, GenAlloc>()) VertexDeclaration(elements);

		VertexDeclarationPtr declPtr = bs_core_ptr<VertexDeclaration, GenAlloc>(decl);
		declPtr->_setThisPtr(declPtr);
		declPtr->initialize();

		return declPtr;
    }

	VertexBufferPtr HardwareBufferManager::createVertexBuffer(UINT32 vertexSize, UINT32 numVerts, GpuBufferUsage usage, bool streamOut)
	{
		assert (numVerts > 0);

		VertexBufferPtr vbuf = bs_core_ptr<VertexBuffer, GenAlloc>(new (bs_alloc<VertexBuffer>()) VertexBuffer(vertexSize, numVerts, usage, streamOut));
		vbuf->_setThisPtr(vbuf);
		vbuf->initialize();
		return vbuf;
	}

	IndexBufferPtr HardwareBufferManager::createIndexBuffer(IndexType itype, UINT32 numIndexes, GpuBufferUsage usage)
	{
		assert (numIndexes > 0);

		IndexBufferPtr ibuf = bs_core_ptr<IndexBuffer, GenAlloc>(new (bs_alloc<IndexBuffer>()) IndexBuffer(itype, numIndexes, usage));
		ibuf->_setThisPtr(ibuf);
		ibuf->initialize();
		return ibuf;

	}

	GpuParamBlockBufferPtr HardwareBufferManager::createGpuParamBlockBuffer(UINT32 size, GpuParamBlockUsage usage)
	{
		GpuParamBlockBufferPtr paramBlockPtr = bs_core_ptr<GpuParamBlockBuffer, GenAlloc>(new (bs_alloc<GpuParamBlockBuffer>()) GpuParamBlockBuffer(size, usage));
		paramBlockPtr->_setThisPtr(paramBlockPtr);
		paramBlockPtr->initialize();
		return paramBlockPtr;
	}

	GpuBufferPtr HardwareBufferManager::createGpuBuffer(UINT32 elementCount, UINT32 elementSize, 
		GpuBufferType type, GpuBufferUsage usage, bool randomGpuWrite, bool useCounter)
	{
		GpuBufferPtr gbuf = bs_core_ptr<GpuBuffer, GenAlloc>(new (bs_alloc<GpuBuffer>()) GpuBuffer(elementCount, elementSize, type, usage, randomGpuWrite, useCounter));
		gbuf->_setThisPtr(gbuf);
		gbuf->initialize();

		return gbuf;
	}

	SPtr<IndexBufferCore> HardwareBufferCoreManager::createIndexBuffer(IndexType itype, UINT32 numIndexes, GpuBufferUsage usage)
	{
		assert(numIndexes > 0);

		SPtr<IndexBufferCore> ibuf = createIndexBufferInternal(itype, numIndexes, usage);
		ibuf->initialize();
		return ibuf;

	}

	SPtr<VertexBufferCore> HardwareBufferCoreManager::createVertexBuffer(UINT32 vertexSize, UINT32 numVerts, GpuBufferUsage usage, bool streamOut)
	{
		assert(numVerts > 0);

		SPtr<VertexBufferCore> vbuf = createVertexBufferInternal(vertexSize, numVerts, usage, streamOut);
		vbuf->initialize();
		return vbuf;
	}

	SPtr<VertexDeclarationCore> HardwareBufferCoreManager::createVertexDeclaration(const List<VertexElement>& elements)
	{
		SPtr<VertexDeclarationCore> declPtr = createVertexDeclarationInternal(elements);
		declPtr->initialize();

		return declPtr;
	}

	SPtr<GpuParamBlockBufferCore> HardwareBufferCoreManager::createGpuParamBlockBuffer(UINT32 size, GpuParamBlockUsage usage)
	{
		SPtr<GpuParamBlockBufferCore> paramBlockPtr = createGpuParamBlockBufferInternal(size, usage);
		paramBlockPtr->initialize();

		return paramBlockPtr;
	}

	SPtr<GpuBufferCore> HardwareBufferCoreManager::createGpuBuffer(UINT32 elementCount, UINT32 elementSize,
		GpuBufferType type, GpuBufferUsage usage, bool randomGpuWrite, bool useCounter)
	{
		SPtr<GpuBufferCore> gbuf = createGpuBufferInternal(elementCount, elementSize, type, usage, randomGpuWrite, useCounter);
		gbuf->initialize();

		return gbuf;
	}

	SPtr<VertexDeclarationCore> HardwareBufferCoreManager::createVertexDeclarationInternal(const List<VertexElement>& elements)
	{
		VertexDeclarationCore* decl = new (bs_alloc<VertexDeclarationCore, GenAlloc>()) VertexDeclarationCore(elements);

		SPtr<VertexDeclarationCore> ret = bs_shared_ptr<VertexDeclarationCore, GenAlloc>(decl);
		ret->_setThisPtr(ret);

		return ret;
	}
}
