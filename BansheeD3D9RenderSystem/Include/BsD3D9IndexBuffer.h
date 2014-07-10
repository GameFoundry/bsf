//__________________________ Banshee Project - A modern game development toolkit _________________________________//
//_____________________________________ www.banshee-project.com __________________________________________________//
//________________________ Copyright (c) 2014 Marko Pintera. All rights reserved. ________________________________//
#pragma once

#include "BsD3D9Prerequisites.h"
#include "BsIndexBuffer.h"
#include "BsD3D9Resource.h"

namespace BansheeEngine 
{ 
	/**
	 * @brief	DirectX 9 implementation of an index buffer.
	 */
    class BS_D3D9_EXPORT D3D9IndexBuffer : public IndexBuffer, public D3D9Resource
    {
	protected:
		/**
		 * @brief	Container for internal buffer resources.
		 */
		struct BufferResources
		{
			IDirect3DIndexBuffer9* mBuffer;
			bool mOutOfDate;
			UINT32 mLockOffset;
			UINT32 mLockLength;
			GpuLockOptions mLockOptions;
		};

    public:
        ~D3D9IndexBuffer();

		/**
		 * @copydoc	IndexBuffer::readData
		 */
        void readData(UINT32 offset, UINT32 length, void* dest);

		/**
		 * @copydoc	IndexBuffer::writeData
		 */
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

		/**
		 * @brief	Creates a DX9 index buffer object in the provided memory pool.
		 */
		void createBuffer(IDirect3DDevice9* d3d9Device, D3DPOOL ePool);
	
		/**
		 * @brief	Returns the DX9 index buffer object.
		 */
        IDirect3DIndexBuffer9* getD3DIndexBuffer();		

	protected:
		friend class D3D9HardwareBufferManager;

		D3D9IndexBuffer(IndexType idxType, UINT32 numIndexes, GpuBufferUsage usage, bool useSystemMem);
		
		/**
		 * @copydoc	IndexBuffer::lockImpl
		 */
		void* lockImpl(UINT32 offset, UINT32 length, GpuLockOptions options);

		/**
		 * @copydoc	IndexBuffer::unlockImpl
		 */
		void unlockImpl();

		/**
		 * @brief	Updates buffer resources from cached system memory buffer.
		 */
		bool updateBufferResources(const UINT8* systemMemoryBuffer, BufferResources* bufferResources);

		/**
		 * @copydoc IndexBuffer::initialize_internal
		 */
		void initialize_internal();	
		
		/**
		 * @copydoc IndexBuffer::destroy_internal
		 */
		void destroy_internal();

	protected:		
		Map<IDirect3DDevice9*, BufferResources*> mMapDeviceToBufferResources;
		D3DINDEXBUFFER_DESC	mBufferDesc;	
		UINT8* mSystemMemoryBuffer;
    };
}