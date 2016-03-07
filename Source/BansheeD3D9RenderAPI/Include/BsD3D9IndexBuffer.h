//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#pragma once

#include "BsD3D9Prerequisites.h"
#include "BsIndexBuffer.h"
#include "BsD3D9Resource.h"

namespace BansheeEngine 
{ 
	/** @addtogroup D3D9
	 *  @{
	 */

	/**	DirectX 9 implementation of an index buffer. */
    class BS_D3D9_EXPORT D3D9IndexBufferCore : public IndexBufferCore, public D3D9Resource
    {
	protected:
		/**	Container for internal buffer resources. */
		struct BufferResources
		{
			IDirect3DIndexBuffer9* mBuffer;
			bool mOutOfDate;
			UINT32 mLockOffset;
			UINT32 mLockLength;
			GpuLockOptions mLockOptions;
		};

    public:
		D3D9IndexBufferCore(IndexType idxType, UINT32 numIndexes, GpuBufferUsage usage);
		~D3D9IndexBufferCore();

		/** @copydoc IndexBufferCore::readData */
        void readData(UINT32 offset, UINT32 length, void* dest) override;

		/** @copydoc IndexBufferCore::writeData */
		void writeData(UINT32 offset, UINT32 length, const void* source, 
			BufferWriteType writeFlags = BufferWriteType::Normal) override;

		/** @copydoc D3D9Resource::notifyOnDeviceCreate */
		void notifyOnDeviceCreate(IDirect3DDevice9* d3d9Device) override;

		/** @copydoc D3D9Resource::notifyOnDeviceDestroy */
		void notifyOnDeviceDestroy(IDirect3DDevice9* d3d9Device) override;

		/** @copydoc D3D9Resource::notifyOnDeviceLost */
		void notifyOnDeviceLost(IDirect3DDevice9* d3d9Device) override;

		/** @copydoc D3D9Resource::notifyOnDeviceReset */
		void notifyOnDeviceReset(IDirect3DDevice9* d3d9Device) override;

		/**	Creates a DX9 index buffer object in the provided memory pool. */
		void createBuffer(IDirect3DDevice9* d3d9Device, D3DPOOL ePool);
	
		/**	Returns the DX9 index buffer object. */
        IDirect3DIndexBuffer9* getD3DIndexBuffer();		

	protected:
		/** @copydoc IndexBufferCore::lockImpl */
		void* lockImpl(UINT32 offset, UINT32 length, GpuLockOptions options) override;

		/** @copydoc IndexBufferCore::unlockImpl */
		void unlockImpl() override;

		/**	Updates buffer resources from cached system memory buffer. */
		bool updateBufferResources(const UINT8* systemMemoryBuffer, BufferResources* bufferResources);

		/** @copydoc IndexBufferCore::initialize */
		void initialize() override;
		
	protected:		
		Map<IDirect3DDevice9*, BufferResources*> mMapDeviceToBufferResources;
		D3DINDEXBUFFER_DESC	mBufferDesc;	
		UINT8* mSystemMemoryBuffer;
    };

	/** @} */
}