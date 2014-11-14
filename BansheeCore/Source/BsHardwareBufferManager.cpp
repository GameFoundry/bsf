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
		GpuParamBlockBufferPtr paramBlockPtr = createGpuParamBlockBufferImpl();
		paramBlockPtr->_setThisPtr(paramBlockPtr);
		paramBlockPtr->initialize(size, usage);

		return paramBlockPtr;
	}

	GpuBufferPtr HardwareBufferManager::createGpuBuffer(UINT32 elementCount, UINT32 elementSize, 
		GpuBufferType type, GpuBufferUsage usage, bool randomGpuWrite, bool useCounter)
	{
		GpuBufferPtr gbuf = createGpuBufferImpl(elementCount, elementSize, type, usage, randomGpuWrite, useCounter);
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

	SPtr<VertexDeclarationCore> HardwareBufferCoreManager::createVertexDeclarationInternal(const List<VertexElement>& elements)
	{
		VertexDeclarationCore* decl = new (bs_alloc<VertexDeclarationCore, GenAlloc>()) VertexDeclarationCore(elements);

		SPtr<VertexDeclarationCore> ret = bs_shared_ptr<VertexDeclarationCore, GenAlloc>(decl);
		ret->_setThisPtr(ret);

		return ret;
	}
}
