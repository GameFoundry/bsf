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

#ifndef __DefaultHardwareBufferManager_H__
#define __DefaultHardwareBufferManager_H__

#include "CmPrerequisites.h"
#include "CmHardwareBufferManager.h"
#include "CmHardwareVertexBuffer.h"
#include "CmHardwareIndexBuffer.h"
#include "CmHardwareConstantBuffer.h"

namespace CamelotEngine {
	/** \addtogroup Core
	*  @{
	*/
	/** \addtogroup RenderSystem
	*  @{
	*/

    /// Specialisation of HardwareVertexBuffer for emulation
    class CM_EXPORT DefaultHardwareVertexBuffer : public HardwareVertexBuffer 
    {
	protected:
		unsigned char* mpData;
        /** See HardwareBuffer. */
        void* lockImpl(UINT32 offset, UINT32 length, LockOptions options);
        /** See HardwareBuffer. */
		void unlockImpl(void);
    public:
		DefaultHardwareVertexBuffer(UINT32 vertexSize, UINT32 numVertices, HardwareBuffer::Usage usage);
		DefaultHardwareVertexBuffer(HardwareBufferManagerBase* mgr, UINT32 vertexSize, UINT32 numVertices, 
            HardwareBuffer::Usage usage);
        ~DefaultHardwareVertexBuffer();
        /** See HardwareBuffer. */
        void readData(UINT32 offset, UINT32 length, void* pDest);
        /** See HardwareBuffer. */
        void writeData(UINT32 offset, UINT32 length, const void* pSource,
				bool discardWholeBuffer = false);
        /** Override HardwareBuffer to turn off all shadowing. */
        void* lock(UINT32 offset, UINT32 length, LockOptions options);
        /** Override HardwareBuffer to turn off all shadowing. */
		void unlock(void);


    };

	/// Specialisation of HardwareIndexBuffer for emulation
    class CM_EXPORT DefaultHardwareIndexBuffer : public HardwareIndexBuffer
    {
	protected:
		unsigned char* mpData;
        /** See HardwareBuffer. */
        void* lockImpl(UINT32 offset, UINT32 length, LockOptions options);
        /** See HardwareBuffer. */
		void unlockImpl(void);
    public:
		DefaultHardwareIndexBuffer(IndexType idxType, UINT32 numIndexes, HardwareBuffer::Usage usage);
        ~DefaultHardwareIndexBuffer();
        /** See HardwareBuffer. */
        void readData(UINT32 offset, UINT32 length, void* pDest);
        /** See HardwareBuffer. */
        void writeData(UINT32 offset, UINT32 length, const void* pSource,
				bool discardWholeBuffer = false);
        /** Override HardwareBuffer to turn off all shadowing. */
        void* lock(UINT32 offset, UINT32 length, LockOptions options);
        /** Override HardwareBuffer to turn off all shadowing. */
		void unlock(void);

    };

	/// Specialisation of HardwareConstantBuffer for emulation
	class CM_EXPORT DefaultHardwareConstantBuffer : public HardwareConstantBuffer
	{
	protected:
		unsigned char* mpData;
		/** See HardwareBuffer. */
		void* lockImpl(UINT32 offset, UINT32 length, LockOptions options);
		/** See HardwareBuffer. */
		void unlockImpl(void);
	public:
		DefaultHardwareConstantBuffer(HardwareBufferManagerBase* mgr, UINT32 sizeBytes, HardwareBuffer::Usage usage);
		~DefaultHardwareConstantBuffer();
		/** See HardwareBuffer. */
		void readData(UINT32 offset, UINT32 length, void* pDest);
		/** See HardwareBuffer. */
		void writeData(UINT32 offset, UINT32 length, const void* pSource,
			bool discardWholeBuffer = false);
		/** Override HardwareBuffer to turn off all shadowing. */
		void* lock(UINT32 offset, UINT32 length, LockOptions options);
		/** Override HardwareBuffer to turn off all shadowing. */
		void unlock(void);
	};

	/** Specialisation of HardwareBufferManagerBase to emulate hardware buffers.
	@remarks
		You might want to instantiate this class if you want to utilise
		classes like MeshSerializer without having initialised the 
		rendering system (which is required to create a 'real' hardware
		buffer manager.
	*/
	class CM_EXPORT DefaultHardwareBufferManagerBase : public HardwareBufferManagerBase
	{
    public:
        DefaultHardwareBufferManagerBase();
        ~DefaultHardwareBufferManagerBase();
        /// Creates a hardware vertex buffer
		HardwareVertexBufferPtr 
            createVertexBuffer(UINT32 vertexSize, UINT32 numVerts, 
				HardwareBuffer::Usage usage, bool streamOut = false);
		/// Create a hardware index buffer
		HardwareIndexBufferPtr 
            createIndexBuffer(HardwareIndexBuffer::IndexType itype, UINT32 numIndexes, 
				HardwareBuffer::Usage usage);
		/// Create a hardware constant buffer
		HardwareConstantBufferPtr 
			createConstantBuffer(UINT32 sizeBytes, HardwareBuffer::Usage usage);
    };

	/// DefaultHardwareBufferManager as a Singleton
	class CM_EXPORT DefaultHardwareBufferManager : public HardwareBufferManager
	{
	public:
		DefaultHardwareBufferManager()
			: HardwareBufferManager(new DefaultHardwareBufferManagerBase()) 
		{

		}
		~DefaultHardwareBufferManager()
		{
			delete mImpl;
		}
	};

	/** @} */
	/** @} */

}

#endif
