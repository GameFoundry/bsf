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

#ifndef __GLDefaultHardwareBufferManager_H__
#define __GLDefaultHardwareBufferManager_H__

#include "CmGLPrerequisites.h"
#include "CmHardwareBufferManager.h"
#include "CmHardwareVertexBuffer.h"
#include "CmHardwareIndexBuffer.h"

namespace CamelotEngine {

    /// Specialisation of HardwareVertexBuffer for emulation
    class CM_RSGL_EXPORT GLDefaultHardwareVertexBuffer : public HardwareVertexBuffer 
    {
	protected:
		unsigned char* mpData;
        /// @copydoc HardwareBuffer::lock
        void* lockImpl(UINT32 offset, UINT32 length, LockOptions options);
        /// @copydoc HardwareBuffer::unlock
        void unlockImpl(void);

    public:
		GLDefaultHardwareVertexBuffer(UINT32 vertexSize, UINT32 numVertices, HardwareBuffer::Usage usage);
		GLDefaultHardwareVertexBuffer(HardwareBufferManagerBase* mgr, UINT32 vertexSize, UINT32 numVertices, 
            HardwareBuffer::Usage usage);
        ~GLDefaultHardwareVertexBuffer();
        /// @copydoc HardwareBuffer::readData
        void readData(UINT32 offset, UINT32 length, void* pDest);
        /// @copydoc HardwareBuffer::writeData

        void writeData(UINT32 offset, UINT32 length, const void* pSource,
				bool discardWholeBuffer = false);
        /** Override HardwareBuffer to turn off all shadowing. */
        void* lock(UINT32 offset, UINT32 length, LockOptions options);
        /** Override HardwareBuffer to turn off all shadowing. */
		void unlock(void);

        //void* getDataPtr(void) const { return (void*)mpData; }
        void* getDataPtr(UINT32 offset) const { return (void*)(mpData + offset); }
    };

	/// Specialisation of HardwareIndexBuffer for emulation
    class CM_RSGL_EXPORT GLDefaultHardwareIndexBuffer : public HardwareIndexBuffer
    {
	protected:
		unsigned char* mpData;
        /// @copydoc HardwareBuffer::lock
        void* lockImpl(UINT32 offset, UINT32 length, LockOptions options);
        /// @copydoc HardwareBuffer::unlock
        void unlockImpl(void);
    public:
		GLDefaultHardwareIndexBuffer(IndexType idxType, UINT32 numIndexes, HardwareBuffer::Usage usage);
        ~GLDefaultHardwareIndexBuffer();
        /// @copydoc HardwareBuffer::readData
        void readData(UINT32 offset, UINT32 length, void* pDest);
        /// @copydoc HardwareBuffer::writeData
        void writeData(UINT32 offset, UINT32 length, const void* pSource,
				bool discardWholeBuffer = false);
        /** Override HardwareBuffer to turn off all shadowing. */
        void* lock(UINT32 offset, UINT32 length, LockOptions options);
        /** Override HardwareBuffer to turn off all shadowing. */
        void unlock(void);

        void* getDataPtr(UINT32 offset) const { return (void*)(mpData + offset); }
    };

	/** Specialisation of HardwareBufferManager to emulate hardware buffers.
	@remarks
		You might want to instantiate this class if you want to utilise
		classes like MeshSerializer without having initialised the 
		rendering system (which is required to create a 'real' hardware
		buffer manager.
	*/
	class CM_RSGL_EXPORT GLDefaultHardwareBufferManagerBase : public HardwareBufferManagerBase
	{
    public:
        GLDefaultHardwareBufferManagerBase();
        ~GLDefaultHardwareBufferManagerBase();

		/**
		 * @copydoc HardwareBufferManagerBase::createVertexBuffer
		 */
		HardwareVertexBufferPtr 
            createVertexBuffer(UINT32 vertexSize, UINT32 numVerts, 
				HardwareBuffer::Usage usage, bool streamOut = false);

		/**
		 * @copydoc HardwareBufferManagerBase::createIndexBuffer
		 */
		HardwareIndexBufferPtr 
            createIndexBuffer(HardwareIndexBuffer::IndexType itype, UINT32 numIndexes, 
				HardwareBuffer::Usage usage);

		/** @copydoc HardwareBufferManagerInterface::createGpuParamBlock */
		GpuParamBlockPtr createGpuParamBlock(const GpuParamBlockDesc& paramDesc);
    };

	/// GLDefaultHardwareBufferManagerBase as a Singleton
	class CM_RSGL_EXPORT GLDefaultHardwareBufferManager : public HardwareBufferManager
	{
	public:
		GLDefaultHardwareBufferManager()
			: HardwareBufferManager(new GLDefaultHardwareBufferManagerBase()) 
		{

		}
		~GLDefaultHardwareBufferManager()
		{
			delete mImpl;
		}
	};
}

#endif
