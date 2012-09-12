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

#include "OgreDefaultHardwareBufferManager.h"

namespace CamelotEngine {

	DefaultHardwareVertexBuffer::DefaultHardwareVertexBuffer(size_t vertexSize, size_t numVertices, 
															 HardwareBuffer::Usage usage)
	: HardwareVertexBuffer(0, vertexSize, numVertices, usage, true, false) // always software, never shadowed
	{
        // Allocate aligned memory for better SIMD processing friendly.
        mpData = static_cast<unsigned char*>(_aligned_malloc(mSizeInBytes, CM_SIMD_ALIGNMENT));
	}
	//-----------------------------------------------------------------------
	DefaultHardwareVertexBuffer::DefaultHardwareVertexBuffer(HardwareBufferManagerBase* mgr, size_t vertexSize, size_t numVertices, 
		HardwareBuffer::Usage usage)
        : HardwareVertexBuffer(mgr, vertexSize, numVertices, usage, true, false) // always software, never shadowed
	{
        // Allocate aligned memory for better SIMD processing friendly.
        mpData = static_cast<unsigned char*>(_aligned_malloc(mSizeInBytes, CM_SIMD_ALIGNMENT));
	}
	//-----------------------------------------------------------------------
    DefaultHardwareVertexBuffer::~DefaultHardwareVertexBuffer()
	{
		_aligned_free(mpData);
	}
	//-----------------------------------------------------------------------
    void* DefaultHardwareVertexBuffer::lockImpl(size_t offset, size_t length, LockOptions options)
	{
        // Only for use internally, no 'locking' as such
		return mpData + offset;
	}
	//-----------------------------------------------------------------------
	void DefaultHardwareVertexBuffer::unlockImpl(void)
	{
        // Nothing to do
	}
	//-----------------------------------------------------------------------
    void* DefaultHardwareVertexBuffer::lock(size_t offset, size_t length, LockOptions options)
	{
        mIsLocked = true;
		return mpData + offset;
	}
	//-----------------------------------------------------------------------
	void DefaultHardwareVertexBuffer::unlock(void)
	{
        mIsLocked = false;
        // Nothing to do
	}
	//-----------------------------------------------------------------------
    void DefaultHardwareVertexBuffer::readData(size_t offset, size_t length, void* pDest)
	{
		assert((offset + length) <= mSizeInBytes);
		memcpy(pDest, mpData + offset, length);
	}
	//-----------------------------------------------------------------------
    void DefaultHardwareVertexBuffer::writeData(size_t offset, size_t length, const void* pSource,
			bool discardWholeBuffer)
	{
		assert((offset + length) <= mSizeInBytes);
		// ignore discard, memory is not guaranteed to be zeroised
		memcpy(mpData + offset, pSource, length);

	}
	//-----------------------------------------------------------------------

	DefaultHardwareIndexBuffer::DefaultHardwareIndexBuffer(IndexType idxType, 
		size_t numIndexes, HardwareBuffer::Usage usage) 
		: HardwareIndexBuffer(0, idxType, numIndexes, usage, true, false) // always software, never shadowed
	{
		mpData = (unsigned char*)malloc(sizeof(unsigned char) * mSizeInBytes);
	}
	//-----------------------------------------------------------------------
    DefaultHardwareIndexBuffer::~DefaultHardwareIndexBuffer()
	{
		free(mpData);
	}
	//-----------------------------------------------------------------------
    void* DefaultHardwareIndexBuffer::lockImpl(size_t offset, size_t length, LockOptions options)
	{
        // Only for use internally, no 'locking' as such
		return mpData + offset;
	}
	//-----------------------------------------------------------------------
	void DefaultHardwareIndexBuffer::unlockImpl(void)
	{
        // Nothing to do
	}
	//-----------------------------------------------------------------------
    void* DefaultHardwareIndexBuffer::lock(size_t offset, size_t length, LockOptions options)
	{
        mIsLocked = true;
		return mpData + offset;
	}
	//-----------------------------------------------------------------------
	void DefaultHardwareIndexBuffer::unlock(void)
	{
        mIsLocked = false;
        // Nothing to do
	}
	//-----------------------------------------------------------------------
    void DefaultHardwareIndexBuffer::readData(size_t offset, size_t length, void* pDest)
	{
		assert((offset + length) <= mSizeInBytes);
		memcpy(pDest, mpData + offset, length);
	}
	//-----------------------------------------------------------------------
    void DefaultHardwareIndexBuffer::writeData(size_t offset, size_t length, const void* pSource,
			bool discardWholeBuffer)
	{
		assert((offset + length) <= mSizeInBytes);
		// ignore discard, memory is not guaranteed to be zeroised
		memcpy(mpData + offset, pSource, length);

	}
	//-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    DefaultHardwareBufferManagerBase::DefaultHardwareBufferManagerBase()
	{
	}
    //-----------------------------------------------------------------------
    DefaultHardwareBufferManagerBase::~DefaultHardwareBufferManagerBase()
	{
        destroyAllDeclarations();
        destroyAllBindings(); 
	}
    //-----------------------------------------------------------------------
	HardwareVertexBufferPtr 
        DefaultHardwareBufferManagerBase::createVertexBuffer(size_t vertexSize, 
		size_t numVerts, HardwareBuffer::Usage usage, bool useShadowBuffer)
	{
        DefaultHardwareVertexBuffer* vb = new DefaultHardwareVertexBuffer(this, vertexSize, numVerts, usage);
        return HardwareVertexBufferPtr(vb);
	}
    //-----------------------------------------------------------------------
	HardwareIndexBufferPtr 
        DefaultHardwareBufferManagerBase::createIndexBuffer(HardwareIndexBuffer::IndexType itype, 
		size_t numIndexes, HardwareBuffer::Usage usage, bool useShadowBuffer)
	{
        DefaultHardwareIndexBuffer* ib = new DefaultHardwareIndexBuffer(itype, numIndexes, usage);
		return HardwareIndexBufferPtr(ib);
	}
}
