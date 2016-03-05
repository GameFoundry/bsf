//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#pragma once

#include "BsD3D9Prerequisites.h"
#include "BsVertexBuffer.h"
#include "BsD3D9Resource.h"

namespace BansheeEngine 
{
	/**
	 * @brief	DirectX 9 implementation of a vertex buffer.
	 */
    class BS_D3D9_EXPORT D3D9VertexBufferCore : public VertexBufferCore, public D3D9Resource
    {   
	protected:
		/**
		 * @brief	Container for internal buffer resources.
		 */
		struct BufferResources
		{
			IDirect3DVertexBuffer9*	mBuffer;
			bool mOutOfDate;
			UINT32 mLockOffset;
			UINT32 mLockLength;
			GpuLockOptions mLockOptions;
		};

    public:
		D3D9VertexBufferCore(UINT32 vertexSize, UINT32 numVertices, GpuBufferUsage usage, bool streamOut);
		~D3D9VertexBufferCore();

		/**
		 * @copydoc	VertexBufferCore::readData
		 */
        void readData(UINT32 offset, UINT32 length, void* dest);

		/**
		 * @copydoc	VertexBufferCore::writeData
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
		 * @brief	Creates a DX9 vertex buffer object in the provided memory pool.
		 */
		void createBuffer(IDirect3DDevice9* d3d9Device, D3DPOOL pool);
		
		/**
		 * @brief	Returns the DX9 vertex buffer object.
		 */
        IDirect3DVertexBuffer9* getD3D9VertexBuffer();

	protected:	
		/**
		 * @copydoc VertexBufferCore::initialize
		 */
		void initialize();	
		
		/**
		 * @copydoc	VertexBufferCore::lockImpl
		 */
		void* lockImpl(UINT32 offset, UINT32 length, GpuLockOptions options);

		/**
		 * @copydoc	VertexBufferCore::unlockImpl
		 */
		void unlockImpl();

		/**
		 * @brief	Updates buffer resources from cached system memory buffer.
		 */
		bool updateBufferResources(const UINT8* systemMemoryBuffer, BufferResources* bufferResources);		

	protected:
		Map<IDirect3DDevice9*, BufferResources*> mMapDeviceToBufferResources;
		D3DVERTEXBUFFER_DESC mBufferDesc;	
		UINT8* mSystemMemoryBuffer;
    };
}