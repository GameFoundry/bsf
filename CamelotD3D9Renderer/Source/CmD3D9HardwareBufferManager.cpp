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
#include "CmD3D9HardwareBufferManager.h"
#include "CmD3D9VertexBuffer.h"
#include "CmD3D9IndexBuffer.h"
#include "CmD3D9VertexDeclaration.h"
#include "CmD3D9GpuBuffer.h"
#include "CmGpuParamBlock.h"
#include "CmException.h"

namespace CamelotFramework 
{
    //-----------------------------------------------------------------------
    D3D9HardwareBufferManager::D3D9HardwareBufferManager()       
    {
    }
    //-----------------------------------------------------------------------
    D3D9HardwareBufferManager::~D3D9HardwareBufferManager()
    {
    }
    //-----------------------------------------------------------------------
    VertexBufferPtr D3D9HardwareBufferManager::createVertexBufferImpl(UINT32 vertexSize, UINT32 numVerts, GpuBufferUsage usage, bool streamOut)
    {
		assert (numVerts > 0);

        D3D9VertexBuffer* buffer = CM_NEW(D3D9VertexBuffer, PoolAlloc) D3D9VertexBuffer(this, vertexSize, numVerts, usage, false);
		return VertexBufferPtr(buffer, &CoreObject::_deleteDelayed<D3D9VertexBuffer, PoolAlloc>);
    }
    //-----------------------------------------------------------------------
	IndexBufferPtr D3D9HardwareBufferManager::createIndexBufferImpl(IndexBuffer::IndexType itype, UINT32 numIndexes, GpuBufferUsage usage)
    {
		assert (numIndexes > 0);

		D3D9IndexBuffer* buffer = CM_NEW(D3D9IndexBuffer, PoolAlloc) D3D9IndexBuffer(this, itype, numIndexes, usage, false);
		return IndexBufferPtr(buffer, &CoreObject::_deleteDelayed<D3D9IndexBuffer, PoolAlloc>);
    }
	//-----------------------------------------------------------------------
	GpuParamBlockPtr D3D9HardwareBufferManager::createGpuParamBlockImpl()
	{
		GpuParamBlock* paramBlock = CM_NEW(GpuParamBlock, PoolAlloc) GpuParamBlock();
		return GpuParamBlockPtr(paramBlock, &CoreObject::_deleteDelayed<GpuParamBlock, PoolAlloc>);
	}
	//-----------------------------------------------------------------------
	GpuBufferPtr D3D9HardwareBufferManager::createGpuBufferImpl(UINT32 elementCount, UINT32 elementSize, 
		GpuBufferType type, GpuBufferUsage usage, bool randomGpuWrite, bool useCounter)
	{
		D3D9GpuBuffer* buffer = CM_NEW(D3D9GpuBuffer, PoolAlloc) D3D9GpuBuffer(elementCount, elementSize, type, usage, randomGpuWrite, useCounter);
		return GpuBufferPtr(buffer, &CoreObject::_deleteDelayed<D3D9GpuBuffer, PoolAlloc>);
	}
    //-----------------------------------------------------------------------
    VertexDeclarationPtr D3D9HardwareBufferManager::createVertexDeclarationImpl(void)
    {
        D3D9VertexDeclaration* decl = CM_NEW(D3D9VertexDeclaration, PoolAlloc) D3D9VertexDeclaration();
		return VertexDeclarationPtr(decl, &CoreObject::_deleteDelayed<D3D9VertexDeclaration, PoolAlloc>);
    }
}
