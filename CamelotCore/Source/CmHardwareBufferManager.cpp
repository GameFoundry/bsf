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

#include "CmHardwareBufferManager.h"
#include "CmVertexData.h"
#include "CmIndexData.h"
#include "CmGpuBuffer.h"
#include "CmVertexDeclaration.h"
#include "CmGpuParamBlockBuffer.h"

namespace CamelotFramework {
    //-----------------------------------------------------------------------
    HardwareBufferManager::HardwareBufferManager()
    {
    }
    //-----------------------------------------------------------------------
    HardwareBufferManager::~HardwareBufferManager()
    {
    }
    //-----------------------------------------------------------------------
    VertexDeclarationPtr HardwareBufferManager::createVertexDeclaration(void)
    {
        VertexDeclarationPtr decl = createVertexDeclarationImpl();
		decl->setThisPtr(decl);
		decl->initialize();
        return decl;
    }
	//-----------------------------------------------------------------------
	VertexBufferPtr HardwareBufferManager::createVertexBuffer(UINT32 vertexSize, UINT32 numVerts, GpuBufferUsage usage, bool streamOut)
	{
		assert (numVerts > 0);

		VertexBufferPtr vbuf = createVertexBufferImpl(vertexSize, numVerts, usage, streamOut);
		vbuf->setThisPtr(vbuf);
		vbuf->initialize();
		return vbuf;
	}
	//-----------------------------------------------------------------------
	IndexBufferPtr HardwareBufferManager::createIndexBuffer(IndexBuffer::IndexType itype, UINT32 numIndexes, GpuBufferUsage usage)
	{
		assert (numIndexes > 0);

		IndexBufferPtr ibuf = createIndexBufferImpl(itype, numIndexes, usage);
		ibuf->setThisPtr(ibuf);
		ibuf->initialize();
		return ibuf;

	}
	//-----------------------------------------------------------------------
	GpuParamBlockBufferPtr HardwareBufferManager::createGpuParamBlockBuffer(UINT32 size, GpuParamBlockUsage usage)
	{
		GpuParamBlockBufferPtr paramBlockPtr = createGpuParamBlockBufferImpl();
		paramBlockPtr->setThisPtr(paramBlockPtr);
		paramBlockPtr->initialize(size, usage);

		return paramBlockPtr;
	}
	//-----------------------------------------------------------------------
	GpuBufferPtr HardwareBufferManager::createGpuBuffer(UINT32 elementCount, UINT32 elementSize, 
		GpuBufferType type, GpuBufferUsage usage, bool randomGpuWrite, bool useCounter)
	{
		GpuBufferPtr gbuf = createGpuBufferImpl(elementCount, elementSize, type, usage, randomGpuWrite, useCounter);
		gbuf->setThisPtr(gbuf);
		gbuf->initialize();
		return gbuf;
	}
	//-----------------------------------------------------------------------
	VertexDeclarationPtr HardwareBufferManager::createVertexDeclarationImpl(void)
	{
		return VertexDeclarationPtr(CM_NEW(VertexDeclaration, PoolAlloc) VertexDeclaration(), &CoreObject::_deleteDelayed<VertexDeclaration, PoolAlloc>);
	}
}
