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
#include "CmGLIndexBuffer.h"
#include "CmGLHardwareBufferManager.h"
#include "CmException.h"

namespace CamelotEngine {

	//---------------------------------------------------------------------
    GLIndexBuffer::GLIndexBuffer(HardwareBufferManager* mgr, IndexType idxType,
        UINT32 numIndexes, GpuBufferUsage usage)
        : IndexBuffer(mgr, idxType, numIndexes, usage, false)
    {  }
	//---------------------------------------------------------------------
    GLIndexBuffer::~GLIndexBuffer()
    {    }
	//---------------------------------------------------------------------
	void GLIndexBuffer::initialize_internal()
	{
		glGenBuffersARB( 1, &mBufferId );

		if (!mBufferId)
		{
			CM_EXCEPT(InternalErrorException, 
				"Cannot create GL index buffer");
		}

		glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, mBufferId);

		// Initialise buffer and set usage
		glBufferDataARB(GL_ELEMENT_ARRAY_BUFFER_ARB, mSizeInBytes, NULL, 
			GLHardwareBufferManager::getGLUsage(mUsage));

		IndexBuffer::initialize_internal();
	}
	//---------------------------------------------------------------------
	void GLIndexBuffer::destroy_internal()
	{
		glDeleteBuffersARB(1, &mBufferId);

		IndexBuffer::destroy_internal();
	}
	//---------------------------------------------------------------------
    void* GLIndexBuffer::lockImpl(UINT32 offset, 
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
			glBindBufferARB( GL_ELEMENT_ARRAY_BUFFER_ARB, mBufferId );
			// Use glMapBuffer
			if(options == GBL_WRITE_ONLY_DISCARD)
			{
				// Discard the buffer
				glBufferDataARB(GL_ELEMENT_ARRAY_BUFFER_ARB, mSizeInBytes, NULL, 
					GLHardwareBufferManager::getGLUsage(mUsage));
			}
			if (options == GBL_WRITE_ONLY || options == GBL_WRITE_ONLY_NO_OVERWRITE)
				access = GL_WRITE_ONLY_ARB;
			else if (options == GBL_READ_ONLY)
				access = GL_READ_ONLY_ARB;
			else
				access = GL_READ_WRITE_ARB;

			void* pBuffer = glMapBufferARB( GL_ELEMENT_ARRAY_BUFFER_ARB, access );

			if(pBuffer == 0)
			{
				CM_EXCEPT(InternalErrorException, 
					"Index Buffer: Out of memory");
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
	void GLIndexBuffer::unlockImpl(void)
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

			glBindBufferARB( GL_ELEMENT_ARRAY_BUFFER_ARB, mBufferId );

			if(!glUnmapBufferARB( GL_ELEMENT_ARRAY_BUFFER_ARB ))
			{
				CM_EXCEPT(InternalErrorException, 
					"Buffer data corrupted, please reload");
			}
		}

        mIsLocked = false;
    }
	//---------------------------------------------------------------------
    void GLIndexBuffer::readData(UINT32 offset, UINT32 length, 
        void* pDest)
    {
		glBindBufferARB( GL_ELEMENT_ARRAY_BUFFER_ARB, mBufferId );
		glGetBufferSubDataARB(GL_ELEMENT_ARRAY_BUFFER_ARB, offset, length, pDest);
    }
	//---------------------------------------------------------------------
    void GLIndexBuffer::writeData(UINT32 offset, UINT32 length, 
            const void* pSource, bool discardWholeBuffer)
    {
        glBindBufferARB( GL_ELEMENT_ARRAY_BUFFER_ARB, mBufferId );

        if (offset == 0 && length == mSizeInBytes)
        {
            glBufferDataARB(GL_ELEMENT_ARRAY_BUFFER_ARB, mSizeInBytes, pSource,
                GLHardwareBufferManager::getGLUsage(mUsage));
        }
        else
        {
            if(discardWholeBuffer)
            {
                glBufferDataARB(GL_ELEMENT_ARRAY_BUFFER_ARB, mSizeInBytes, NULL,
                    GLHardwareBufferManager::getGLUsage(mUsage));
            }

            // Now update the real buffer
            glBufferSubDataARB(GL_ELEMENT_ARRAY_BUFFER_ARB, offset, length, pSource);
        }
    }
}
