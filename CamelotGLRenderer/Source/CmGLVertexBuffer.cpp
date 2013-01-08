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
#include "CmException.h"

namespace CamelotEngine {

	//---------------------------------------------------------------------
    GLVertexBuffer::GLVertexBuffer(HardwareBufferManager* mgr, UINT32 vertexSize, 
        UINT32 numVertices, GpuBufferUsage usage)
        : VertexBuffer(mgr, vertexSize, numVertices, usage, false)
    {
        glGenBuffersARB( 1, &mBufferId );

        if (!mBufferId)
        {
            CM_EXCEPT(InternalErrorException, 
                "Cannot create GL vertex buffer");
        }

        glBindBufferARB(GL_ARRAY_BUFFER_ARB, mBufferId);

        // Initialise mapped buffer and set usage
        glBufferDataARB(GL_ARRAY_BUFFER_ARB, mSizeInBytes, NULL, 
            GLHardwareBufferManager::getGLUsage(usage));

        //std::cerr << "creating vertex buffer = " << mBufferId << std::endl;
    }
	//---------------------------------------------------------------------
    GLVertexBuffer::~GLVertexBuffer()
    {
        glDeleteBuffersARB(1, &mBufferId);
    }
	//---------------------------------------------------------------------
    void* GLVertexBuffer::lockImpl(UINT32 offset, 
        UINT32 length, GpuLockOptions options)
    {
        GLenum access = 0;

        if(mIsLocked)
        {
			CM_EXCEPT(InternalErrorException,
                "Invalid attempt to lock an index buffer that has already been locked");
        }


		void* retPtr = 0;

		GLHardwareBufferManager* glBufManager = static_cast<GLHardwareBufferManager*>(HardwareBufferManager::instancePtr());

		// Try to use scratch buffers for smaller buffers
		if( length < glBufManager->getGLMapBufferThreshold() )
		{
			// if this fails, we fall back on mapping
			retPtr = glBufManager->allocateScratch((UINT32)length);

			if (retPtr)
			{
				mLockedToScratch = true;
				mScratchOffset = offset;
				mScratchSize = length;
				mScratchPtr = retPtr;
				mScratchUploadOnUnlock = (options != GBL_READ_ONLY);

				if (options != GBL_WRITE_ONLY_DISCARD)
				{
					// have to read back the data before returning the pointer
					readData(offset, length, retPtr);
				}
			}
		}
		
		if (!retPtr)
		{
			// Use glMapBuffer
			glBindBufferARB( GL_ARRAY_BUFFER_ARB, mBufferId );
			// Use glMapBuffer
			if(options == GBL_WRITE_ONLY_DISCARD)
			{
				// Discard the buffer
				glBufferDataARB(GL_ARRAY_BUFFER_ARB, mSizeInBytes, NULL, 
					GLHardwareBufferManager::getGLUsage(mUsage));

			}
			if ((options == GBL_WRITE_ONLY) || (options == GBL_WRITE_ONLY_NO_OVERWRITE))
				access = GL_WRITE_ONLY_ARB;
			else if (options == GBL_READ_ONLY)
				access = GL_READ_ONLY_ARB;
			else
				access = GL_READ_WRITE_ARB;

			void* pBuffer = glMapBufferARB( GL_ARRAY_BUFFER_ARB, access);

			if(pBuffer == 0)
			{
				CM_EXCEPT(InternalErrorException, "Vertex Buffer: Out of memory");
			}

			// return offsetted
			retPtr = static_cast<void*>(
				static_cast<unsigned char*>(pBuffer) + offset);

			mLockedToScratch = false;
		}
		mIsLocked = true;
		return retPtr;
    }
	//---------------------------------------------------------------------
	void GLVertexBuffer::unlockImpl(void)
    {
		if (mLockedToScratch)
		{
			if (mScratchUploadOnUnlock)
			{
				// have to write the data back to vertex buffer
				writeData(mScratchOffset, mScratchSize, mScratchPtr, 
					mScratchOffset == 0 && mScratchSize == getSizeInBytes());
			}

			// deallocate from scratch buffer
			static_cast<GLHardwareBufferManager*>(
				HardwareBufferManager::instancePtr())->deallocateScratch(mScratchPtr);

			mLockedToScratch = false;
		}
		else
		{

			glBindBufferARB(GL_ARRAY_BUFFER_ARB, mBufferId);

			if(!glUnmapBufferARB( GL_ARRAY_BUFFER_ARB ))
			{
				CM_EXCEPT(InternalErrorException, "Buffer data corrupted, please reload");
			}
		}

        mIsLocked = false;
    }
	//---------------------------------------------------------------------
    void GLVertexBuffer::readData(UINT32 offset, UINT32 length, 
        void* pDest)
    {
        // get data from the real buffer
        glBindBufferARB(GL_ARRAY_BUFFER_ARB, mBufferId);
        
        glGetBufferSubDataARB(GL_ARRAY_BUFFER_ARB, offset, length, pDest);
    }
	//---------------------------------------------------------------------
    void GLVertexBuffer::writeData(UINT32 offset, UINT32 length, 
            const void* pSource, bool discardWholeBuffer)
    {
        glBindBufferARB(GL_ARRAY_BUFFER_ARB, mBufferId);

        if (offset == 0 && length == mSizeInBytes)
        {
            glBufferDataARB(GL_ARRAY_BUFFER_ARB, mSizeInBytes, pSource, 
                GLHardwareBufferManager::getGLUsage(mUsage));
        }
        else
        {
            if(discardWholeBuffer)
            {
                glBufferDataARB(GL_ARRAY_BUFFER_ARB, mSizeInBytes, NULL, 
                    GLHardwareBufferManager::getGLUsage(mUsage));
            }

            // Now update the real buffer
            glBufferSubDataARB(GL_ARRAY_BUFFER_ARB, offset, length, pSource); 
        }
    }
}
