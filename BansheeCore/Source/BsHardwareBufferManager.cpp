#include "BsHardwareBufferManager.h"
#include "BsVertexData.h"
#include "BsIndexData.h"
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

	VertexDeclarationPtr HardwareBufferManager::createVertexDeclaration(const VertexDeclaration::VertexElementList& elements)
    {
        VertexDeclarationPtr decl = createVertexDeclarationImpl(elements);
		decl->_setThisPtr(decl);
		decl->initialize();
        return decl;
    }

	VertexBufferPtr HardwareBufferManager::createVertexBuffer(UINT32 vertexSize, UINT32 numVerts, GpuBufferUsage usage, bool streamOut)
	{
		assert (numVerts > 0);

		VertexBufferPtr vbuf = createVertexBufferImpl(vertexSize, numVerts, usage, streamOut);
		vbuf->_setThisPtr(vbuf);
		vbuf->initialize();
		return vbuf;
	}

	IndexBufferPtr HardwareBufferManager::createIndexBuffer(IndexBuffer::IndexType itype, UINT32 numIndexes, GpuBufferUsage usage)
	{
		assert (numIndexes > 0);

		IndexBufferPtr ibuf = createIndexBufferImpl(itype, numIndexes, usage);
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

	VertexDeclarationPtr HardwareBufferManager::createVertexDeclarationImpl(const VertexDeclaration::VertexElementList& elements)
	{
		return bs_core_ptr<VertexDeclaration, PoolAlloc>(new (bs_alloc<VertexDeclaration, PoolAlloc>()) VertexDeclaration(elements));
	}
}
