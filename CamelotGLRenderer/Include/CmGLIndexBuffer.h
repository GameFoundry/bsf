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
#ifndef __GLHARDWAREINDEXBUFFER_H__
#define __GLHARDWAREINDEXBUFFER_H__

#include "CmGLPrerequisites.h"
#include "CmIndexBuffer.h"

namespace CamelotEngine { 


    class CM_RSGL_EXPORT GLIndexBuffer : public IndexBuffer
    {
    private:
        GLuint mBufferId;
		// Scratch buffer handling
		bool mLockedToScratch;
		UINT32 mScratchOffset;
		UINT32 mScratchSize;
		void* mScratchPtr;
		bool mScratchUploadOnUnlock;
    protected:
        /** See HardwareBuffer. */
        void* lockImpl(UINT32 offset, UINT32 length, GpuLockOptions options);
        /** See HardwareBuffer. */
        void unlockImpl(void);
    public:
        GLIndexBuffer(HardwareBufferManagerBase* mgr, IndexType idxType, UINT32 numIndexes, 
            GpuBufferUsage usage); 
        ~GLIndexBuffer();
        /** See HardwareBuffer. */
        void readData(UINT32 offset, UINT32 length, void* pDest);
        /** See HardwareBuffer. */
        void writeData(UINT32 offset, UINT32 length, 
            const void* pSource, bool discardWholeBuffer = false);

        GLuint getGLBufferId(void) const { return mBufferId; }
    };

}

#endif // __GLHARDWAREINDEXBUFFER_H__

