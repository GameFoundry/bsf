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
#include "CmGLDefaultHardwareBufferManager.h"
#include "CmException.h"

namespace CamelotEngine {

	GLDefaultHardwareVertexBuffer::GLDefaultHardwareVertexBuffer(UINT32 vertexSize, UINT32 numVertices, 
																 HardwareBuffer::Usage usage)
	: HardwareVertexBuffer(0, vertexSize, numVertices, usage, true, false) // always software, never shadowed
	{
		mpData = static_cast<unsigned char*>(_aligned_malloc(mSizeInBytes, CM_SIMD_ALIGNMENT));
	}
	//-----------------------------------------------------------------------
	GLDefaultHardwareVertexBuffer::GLDefaultHardwareVertexBuffer(HardwareBufferManagerBase* mgr, UINT32 vertexSize, UINT32 numVertices, 
		HardwareBuffer::Usage usage)
        : HardwareVertexBuffer(mgr, vertexSize, numVertices, usage, true, false) // always software, never shadowed
	{
        mpData = static_cast<unsigned char*>(_aligned_malloc(mSizeInBytes, CM_SIMD_ALIGNMENT));
	}
	//-----------------------------------------------------------------------
    GLDefaultHardwareVertexBuffer::~GLDefaultHardwareVertexBuffer()
	{
		_aligned_free(mpData);
	}
	//-----------------------------------------------------------------------
    void* GLDefaultHardwareVertexBuffer::lockImpl(UINT32 offset, UINT32 length, LockOptions options)
	{
        // Only for use internally, no 'locking' as such
		return mpData + offset;
	}
	//-----------------------------------------------------------------------
	void GLDefaultHardwareVertexBuffer::unlockImpl(void)
	{
        // Nothing to do
	}
	//-----------------------------------------------------------------------
    void* GLDefaultHardwareVertexBuffer::lock(UINT32 offset, UINT32 length, LockOptions options)
	{
        mIsLocked = true;
		return mpData + offset;
	}
	//-----------------------------------------------------------------------
	void GLDefaultHardwareVertexBuffer::unlock(void)
	{
        mIsLocked = false;
        // Nothing to do
	}
	//-----------------------------------------------------------------------
    void GLDefaultHardwareVertexBuffer::readData(UINT32 offset, UINT32 length, void* pDest)
	{
		assert((offset + length) <= mSizeInBytes);
		memcpy(pDest, mpData + offset, length);
	}
	//-----------------------------------------------------------------------
    void GLDefaultHardwareVertexBuffer::writeData(UINT32 offset, UINT32 length, const void* pSource,
			bool discardWholeBuffer)
	{
		assert((offset + length) <= mSizeInBytes);
		// ignore discard, memory is not guaranteed to be zeroised
		memcpy(mpData + offset, pSource, length);

	}
	//-----------------------------------------------------------------------

	GLDefaultHardwareIndexBuffer::GLDefaultHardwareIndexBuffer(IndexType idxType, 
		UINT32 numIndexes, HardwareBuffer::Usage usage) 
		: HardwareIndexBuffer(0, idxType, numIndexes, usage, true, false) // always software, never shadowed
	{
		mpData = new unsigned char[mSizeInBytes];
	}
	//-----------------------------------------------------------------------
    GLDefaultHardwareIndexBuffer::~GLDefaultHardwareIndexBuffer()
	{
		delete [] mpData;
	}
	//-----------------------------------------------------------------------
    void* GLDefaultHardwareIndexBuffer::lockImpl(UINT32 offset, UINT32 length, LockOptions options)
	{
        // Only for use internally, no 'locking' as such
		return mpData + offset;
	}
	//-----------------------------------------------------------------------
	void GLDefaultHardwareIndexBuffer::unlockImpl(void)
	{
        // Nothing to do
	}
	//-----------------------------------------------------------------------
    void* GLDefaultHardwareIndexBuffer::lock(UINT32 offset, UINT32 length, LockOptions options)
	{
        mIsLocked = true;
		return mpData + offset;
	}
	//-----------------------------------------------------------------------
	void GLDefaultHardwareIndexBuffer::unlock(void)
	{
        mIsLocked = false;
        // Nothing to do
	}
	//-----------------------------------------------------------------------
    void GLDefaultHardwareIndexBuffer::readData(UINT32 offset, UINT32 length, void* pDest)
	{
		assert((offset + length) <= mSizeInBytes);
		memcpy(pDest, mpData + offset, length);
	}
	//-----------------------------------------------------------------------
    void GLDefaultHardwareIndexBuffer::writeData(UINT32 offset, UINT32 length, const void* pSource,
			bool discardWholeBuffer)
	{
		assert((offset + length) <= mSizeInBytes);
		// ignore discard, memory is not guaranteed to be zeroised
		memcpy(mpData + offset, pSource, length);

	}
	//-----------------------------------------------------------------------
	
	
    //-----------------------------------------------------------------------
    GLDefaultHardwareBufferManagerBase::GLDefaultHardwareBufferManagerBase()
	{
	}
    //-----------------------------------------------------------------------
    GLDefaultHardwareBufferManagerBase::~GLDefaultHardwareBufferManagerBase()
	{
        destroyAllBindings();
	}
    //-----------------------------------------------------------------------
	HardwareVertexBufferPtr 
        GLDefaultHardwareBufferManagerBase::createVertexBuffer(UINT32 vertexSize, 
		UINT32 numVerts, HardwareBuffer::Usage usage, bool useShadowBuffer)
	{
		return HardwareVertexBufferPtr(
			new GLDefaultHardwareVertexBuffer(this, vertexSize, numVerts, usage));
	}
    //-----------------------------------------------------------------------
	HardwareIndexBufferPtr 
        GLDefaultHardwareBufferManagerBase::createIndexBuffer(HardwareIndexBuffer::IndexType itype, 
		UINT32 numIndexes, HardwareBuffer::Usage usage, bool useShadowBuffer)
	{
		return HardwareIndexBufferPtr(
			new GLDefaultHardwareIndexBuffer(itype, numIndexes, usage) );
	}
}
