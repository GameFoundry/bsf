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
#include "CmGLHardwareBufferManager.h"
#include "CmGLVertexBuffer.h"
#include "CmGLIndexBuffer.h"
#include "CmGLGpuBuffer.h"
#include "CmHardwareBuffer.h"
#include "CmGLGpuParamBlockBuffer.h"
#include "CmRenderSystem.h"
#include "CmRenderSystemCapabilities.h"

namespace BansheeEngine 
{
	struct GLScratchBufferAlloc
	{
		/// Size in bytes
		UINT32 size: 31;
		/// Free? (pack with size)
		UINT32 free: 1;
	};

	#define SCRATCH_POOL_SIZE 1 * 1024 * 1024
	#define SCRATCH_ALIGNMENT 32

    GLHardwareBufferManager::GLHardwareBufferManager() 
		: mScratchBufferPool(NULL), mMapBufferThreshold(CM_GL_DEFAULT_MAP_BUFFER_THRESHOLD)
    {
		// Init scratch pool
		// TODO make it a configurable size?
		// 32-bit aligned buffer
		mScratchBufferPool = static_cast<char*>(_aligned_malloc(SCRATCH_POOL_SIZE, SCRATCH_ALIGNMENT));
		GLScratchBufferAlloc* ptrAlloc = (GLScratchBufferAlloc*)mScratchBufferPool;
		ptrAlloc->size = SCRATCH_POOL_SIZE - sizeof(GLScratchBufferAlloc);
		ptrAlloc->free = 1;

		// non-Win32 machines are having issues glBufferSubData, looks like buffer corruption
		// disable for now until we figure out where the problem lies			
#	if CM_PLATFORM != CM_PLATFORM_WIN32
		mMapBufferThreshold = 0;
#	endif

		// Win32 machines with ATI GPU are having issues glMapBuffer, looks like buffer corruption
		// disable for now until we figure out where the problem lies			
#	if CM_PLATFORM == CM_PLATFORM_WIN32
		if (BansheeEngine::RenderSystem::instancePtr()->getCapabilities()->getVendor() == GPU_AMD) 
		{
			mMapBufferThreshold = 0xffffffffUL  /* maximum unsigned long value */;
		}
#	endif

    }

    GLHardwareBufferManager::~GLHardwareBufferManager()
    {
		_aligned_free(mScratchBufferPool);
    }

    VertexBufferPtr GLHardwareBufferManager::createVertexBufferImpl(
        UINT32 vertexSize, UINT32 numVerts, GpuBufferUsage usage, bool streamOut)
    {
		return cm_core_ptr<GLVertexBuffer, PoolAlloc>(new (cm_alloc<GLVertexBuffer, PoolAlloc>()) GLVertexBuffer(vertexSize, numVerts, usage));
    }

    IndexBufferPtr GLHardwareBufferManager::createIndexBufferImpl(IndexBuffer::IndexType itype, UINT32 numIndexes, GpuBufferUsage usage)
    {
		return cm_core_ptr<GLIndexBuffer, PoolAlloc>(new (cm_alloc<GLIndexBuffer, PoolAlloc>()) GLIndexBuffer(itype, numIndexes, usage));
    }

	GpuParamBlockBufferPtr GLHardwareBufferManager::createGpuParamBlockBufferImpl()
	{
		return cm_core_ptr<GLGpuParamBlockBuffer, PoolAlloc>(new (cm_alloc<GLGpuParamBlockBuffer, PoolAlloc>()) GLGpuParamBlockBuffer());
	}

	GpuBufferPtr GLHardwareBufferManager::createGpuBufferImpl(UINT32 elementCount, UINT32 elementSize, 
		GpuBufferType type, GpuBufferUsage usage, bool randomGpuWrite, bool useCounter)
	{
		return cm_core_ptr<GLGpuBuffer, PoolAlloc>(new (cm_alloc<GLGpuBuffer, PoolAlloc>()) GLGpuBuffer(elementCount, elementSize, type, usage, randomGpuWrite, useCounter));
	}

    GLenum GLHardwareBufferManager::getGLUsage(unsigned int usage)
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

    GLenum GLHardwareBufferManager::getGLType(unsigned int type)
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

	void* GLHardwareBufferManager::allocateScratch(UINT32 size)
	{
		// simple forward link search based on alloc sizes
		// not that fast but the list should never get that long since not many
		// locks at once (hopefully)
		CM_LOCK_MUTEX(mScratchMutex)


		// Alignment - round up the size to 32 bits
		// control blocks are 32 bits too so this packs nicely
		if (size % 4 != 0)
		{
			size += 4 - (size % 4);
		}

		UINT32 bufferPos = 0;
		while (bufferPos < SCRATCH_POOL_SIZE)
		{
			GLScratchBufferAlloc* pNext = (GLScratchBufferAlloc*)(mScratchBufferPool + bufferPos);
			// Big enough?
			if (pNext->free && pNext->size >= size)
			{
				// split? And enough space for control block
				if(pNext->size > size + sizeof(GLScratchBufferAlloc))
				{
					UINT32 offset = (UINT32)sizeof(GLScratchBufferAlloc) + size;

					GLScratchBufferAlloc* pSplitAlloc = (GLScratchBufferAlloc*)
						(mScratchBufferPool + bufferPos + offset);
					pSplitAlloc->free = 1;
					// split size is remainder minus new control block
					pSplitAlloc->size = pNext->size - size - sizeof(GLScratchBufferAlloc);

					// New size of current
					pNext->size = size;
				}
				// allocate and return
				pNext->free = 0;

				// return pointer just after this control block (++ will do that for us)
				return ++pNext;

			}

			bufferPos += (UINT32)sizeof(GLScratchBufferAlloc) + pNext->size;

		}

		// no available alloc
		return 0;

	}

	void GLHardwareBufferManager::deallocateScratch(void* ptr)
	{
		CM_LOCK_MUTEX(mScratchMutex)

		// Simple linear search dealloc
		UINT32 bufferPos = 0;
		GLScratchBufferAlloc* pLast = 0;
		while (bufferPos < SCRATCH_POOL_SIZE)
		{
			GLScratchBufferAlloc* pCurrent = (GLScratchBufferAlloc*)(mScratchBufferPool + bufferPos);
			
			// Pointers match?
			if ((mScratchBufferPool + bufferPos + sizeof(GLScratchBufferAlloc))
				== ptr)
			{
				// dealloc
				pCurrent->free = 1;
				
				// merge with previous
				if (pLast && pLast->free)
				{
					// adjust buffer pos
					bufferPos -= (pLast->size + (UINT32)sizeof(GLScratchBufferAlloc));
					// merge free space
					pLast->size += pCurrent->size + sizeof(GLScratchBufferAlloc);
					pCurrent = pLast;
				}

				// merge with next
				UINT32 offset = bufferPos + pCurrent->size + (UINT32)sizeof(GLScratchBufferAlloc);
				if (offset < SCRATCH_POOL_SIZE)
				{
					GLScratchBufferAlloc* pNext = (GLScratchBufferAlloc*)(
						mScratchBufferPool + offset);
					if (pNext->free)
					{
						pCurrent->size += pNext->size + sizeof(GLScratchBufferAlloc);
					}
				}

				// done
				return;
			}

			bufferPos += (UINT32)sizeof(GLScratchBufferAlloc) + pCurrent->size;
			pLast = pCurrent;

		}

		// Should never get here unless there's a corruption
		assert (false && "Memory deallocation error");
	}

	const UINT32 GLHardwareBufferManager::getGLMapBufferThreshold() const
	{
		return mMapBufferThreshold;
	}

	void GLHardwareBufferManager::setGLMapBufferThreshold(const UINT32 value)
	{
		mMapBufferThreshold = value;
	}
}
