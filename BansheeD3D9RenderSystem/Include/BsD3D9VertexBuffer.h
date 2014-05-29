#pragma once

#include "BsD3D9Prerequisites.h"
#include "BsVertexBuffer.h"
#include "BsD3D9Resource.h"

namespace BansheeEngine 
{
    /// Specialisation of HardwareVertexBuffer for D3D9
    class BS_D3D9_EXPORT D3D9VertexBuffer : public VertexBuffer, public D3D9Resource
    {   
	protected:
		struct BufferResources
		{
			IDirect3DVertexBuffer9*	mBuffer;
			bool mOutOfDate;
			UINT32 mLockOffset;
			UINT32 mLockLength;
			GpuLockOptions mLockOptions;
		};

    public:
        ~D3D9VertexBuffer();
        /** See HardwareBuffer. */
        void readData(UINT32 offset, UINT32 length, void* dest);
        /** See HardwareBuffer. */
        void writeData(UINT32 offset, UINT32 length, const void* source, BufferWriteType writeFlags = BufferWriteType::Normal);
	
		/**
		 * @copydoc	D3D9Resource::notifyOnDeviceCreate
		 */
		virtual void notifyOnDeviceCreate(IDirect3DDevice9* d3d9Device);

		/**
		 * @copydoc	D3D9Resource::notifyOnDeviceDestroy
		 */
		virtual void notifyOnDeviceDestroy(IDirect3DDevice9* d3d9Device);

		/**
		 * @copydoc	D3D9Resource::notifyOnDeviceLost
		 */
		virtual void notifyOnDeviceLost(IDirect3DDevice9* d3d9Device);

		/**
		 * @copydoc	D3D9Resource::notifyOnDeviceReset
		 */
		virtual void notifyOnDeviceReset(IDirect3DDevice9* d3d9Device);

		// Create the actual vertex buffer.
		void createBuffer(IDirect3DDevice9* d3d9Device, D3DPOOL pool);
		
        /// Get D3D9-specific vertex buffer
        IDirect3DVertexBuffer9* getD3D9VertexBuffer();

		virtual bool vertexColorReqRGBFlip() { return true; }

	protected:	
		friend class D3D9HardwareBufferManager;

		D3D9VertexBuffer(UINT32 vertexSize, UINT32 numVertices, GpuBufferUsage usage, bool useSystemMem);
	
		/** See HardwareBuffer. */
		void* lockImpl(UINT32 offset, UINT32 length, GpuLockOptions options);		
		/** See HardwareBuffer. */
		void unlockImpl();			
		// updates buffer resources from system memory buffer.
		bool updateBufferResources(const UINT8* systemMemoryBuffer, BufferResources* bufferResources);		

		/**
		 * @copydoc VertexBuffer::initialize_internal()
		 */
		void initialize_internal();	
		
		/**
		 * @copydoc VertexBuffer::destroy_internal()
		 */
		void destroy_internal();

	protected:
		Map<IDirect3DDevice9*, BufferResources*> mMapDeviceToBufferResources; // Map between device to buffer resources.
		D3DVERTEXBUFFER_DESC mBufferDesc; // Buffer description.		
		UINT8* mSystemMemoryBuffer; // Consistent system memory buffer for multiple devices support.
    };
}