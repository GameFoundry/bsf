#pragma once

#include "BsGLPrerequisites.h"
#include "BsHardwareBufferManager.h"

namespace BansheeEngine {

// Default threshold at which glMapBuffer becomes more efficient than glBufferSubData (32k?)
#	define BS_GL_DEFAULT_MAP_BUFFER_THRESHOLD (1024 * 32)


    /** Implementation of HardwareBufferManager for OpenGL. */
    class BS_RSGL_EXPORT GLHardwareBufferManager : public HardwareBufferManager
    {
    public:
        GLHardwareBufferManager();
        ~GLHardwareBufferManager();

        /// Utility function to get the correct GL usage based on HBU's
        static GLenum getGLUsage(unsigned int usage);

        /// Utility function to get the correct GL type based on VET's
        static GLenum getGLType(unsigned int type);

		/** Allocator method to allow us to use a pool of memory as a scratch
			area for hardware buffers. This is because glMapBuffer is incredibly
			inefficient, seemingly no matter what options we give it. So for the
			period of lock/unlock, we will instead allocate a section of a local
			memory pool, and use glBufferSubDataARB / glGetBufferSubDataARB
			instead.
		*/
		void* allocateScratch(UINT32 size);

		/// @see allocateScratch
		void deallocateScratch(void* ptr);

		/** Threshold after which glMapBuffer is used and not glBufferSubData
		*/
		const UINT32 getGLMapBufferThreshold() const;
		void setGLMapBufferThreshold( const UINT32 value );

	protected:
		char* mScratchBufferPool;
		BS_MUTEX(mScratchMutex);
		UINT32 mMapBufferThreshold;

		/**
		 * @copydoc HardwareBufferManager::createVertexBufferImpl
		 */
        VertexBufferPtr createVertexBufferImpl(UINT32 vertexSize, 
            UINT32 numVerts, GpuBufferUsage usage, bool streamOut = false);

		/**
		 * @copydoc HardwareBufferManager::createIndexBufferImpl
		 */
        IndexBufferPtr createIndexBufferImpl(
            IndexBuffer::IndexType itype, UINT32 numIndexes, 
            GpuBufferUsage usage);

		/** @copydoc HardwareBufferManager::createGpuParamBlockBufferImpl */
		GpuParamBlockBufferPtr createGpuParamBlockBufferImpl();

		/**
		 * @copydoc HardwareBufferManager::createGenericBufferImpl
		 *
		 * OpenGL does not support generic buffers so this method will return a dummy instance.
		 */
		GpuBufferPtr createGpuBufferImpl(UINT32 elementCount, UINT32 elementSize, 
			GpuBufferType type, GpuBufferUsage usage, bool randomGpuWrite = false, bool useCounter = false);
    };
}