#include "BsD3D9VertexBuffer.h"
#include "BsD3D9Mappings.h"
#include "BsException.h"
#include "BsD3D9HardwareBufferManager.h"
#include "BsD3D9RenderSystem.h"
#include "BsD3D9Device.h"
#include "BsD3D9ResourceManager.h"
#include "BsRenderStats.h"

namespace BansheeEngine 
{
	D3D9VertexBufferCore::D3D9VertexBufferCore(GpuBufferUsage usage, bool useSystemMemory, const VertexBufferProperties& properties)
		: VertexBufferCore(usage, useSystemMemory, properties), mSystemMemoryBuffer(nullptr)
    { }

	void D3D9VertexBufferCore::initialize()
	{
		D3D9_DEVICE_ACCESS_CRITICAL_SECTION;

		// Set the desired memory pool
		mBufferDesc.Pool = mSystemMemory ? D3DPOOL_SYSTEMMEM : D3DPOOL_DEFAULT;

		// Allocate the system memory buffer.
		mSystemMemoryBuffer = (UINT8*)bs_alloc(getSizeInBytes());
		memset(mSystemMemoryBuffer, 0, getSizeInBytes());

		// Case we have to create this buffer resource on loading.
		if (D3D9RenderSystem::getResourceManager()->getCreationPolicy() == RCP_CREATE_ON_ALL_DEVICES)
		{
			for (UINT32 i = 0; i < D3D9RenderSystem::getResourceCreationDeviceCount(); ++i)
			{
				IDirect3DDevice9* d3d9Device = D3D9RenderSystem::getResourceCreationDevice(i);

				createBuffer(d3d9Device, mBufferDesc.Pool);
			}
		}

		BS_INC_RENDER_STAT_CAT(ResCreated, RenderStatObject_VertexBuffer);
		VertexBufferCore::initialize();
	}

	void D3D9VertexBufferCore::destroy()
	{
		D3D9_DEVICE_ACCESS_CRITICAL_SECTION;

		for (auto& bufferResourcesPair : mMapDeviceToBufferResources)
		{
			BufferResources* bufferResources = bufferResourcesPair.second;

			SAFE_RELEASE(bufferResources->mBuffer);

			if (bufferResources != nullptr)
				bs_delete(bufferResources);
		}

		mMapDeviceToBufferResources.clear();

		if (mSystemMemoryBuffer != nullptr)
			bs_free(mSystemMemoryBuffer);

		BS_INC_RENDER_STAT_CAT(ResDestroyed, RenderStatObject_VertexBuffer);
		VertexBufferCore::destroy();
	}

	void* D3D9VertexBufferCore::lockImpl(UINT32 offset, UINT32 length, GpuLockOptions options)
    {		
		D3D9_DEVICE_ACCESS_CRITICAL_SECTION

#if BS_PROFILING_ENABLED
		if (options == GBL_READ_ONLY || options == GBL_READ_WRITE)
		{
			BS_INC_RENDER_STAT_CAT(ResRead, RenderStatObject_VertexBuffer);
		}

		if (options == GBL_READ_WRITE || options == GBL_WRITE_ONLY || options == GBL_WRITE_ONLY_DISCARD || options == GBL_WRITE_ONLY_NO_OVERWRITE)
		{
			BS_INC_RENDER_STAT_CAT(ResWrite, RenderStatObject_VertexBuffer);
		}
#endif

		if (options != GBL_READ_ONLY)
		{
			for (auto& bufferResourcesPair : mMapDeviceToBufferResources)
			{
				BufferResources* bufferResources = bufferResourcesPair.second;
				bufferResources->mOutOfDate = true;

				if(bufferResources->mLockLength > 0)
				{
					UINT32 highPoint = std::max( offset + length, 
						bufferResources->mLockOffset + bufferResources->mLockLength );
					bufferResources->mLockOffset = std::min( bufferResources->mLockOffset, offset );
					bufferResources->mLockLength = highPoint - bufferResources->mLockOffset;
				}
				else
				{
					if (offset < bufferResources->mLockOffset)
						bufferResources->mLockOffset = offset;
					if (length > bufferResources->mLockLength)
						bufferResources->mLockLength = length;
				}
				
				if (bufferResources->mLockOptions != GBL_WRITE_ONLY_DISCARD)
					bufferResources->mLockOptions = options;					
			}
		}

		return mSystemMemoryBuffer + offset;		
    }

	void D3D9VertexBufferCore::unlockImpl()
    {
		D3D9_DEVICE_ACCESS_CRITICAL_SECTION

		for (auto& bufferResourcesPair : mMapDeviceToBufferResources)
		{
			BufferResources* bufferResources = bufferResourcesPair.second;

			if (bufferResources->mOutOfDate && bufferResources->mBuffer != nullptr)
				updateBufferResources(mSystemMemoryBuffer, bufferResources);
		}
    }

	void D3D9VertexBufferCore::readData(UINT32 offset, UINT32 length, void* dest)
    {
        void* pSrc = this->lock(offset, length, GBL_READ_ONLY);
        memcpy(dest, pSrc, length);
        this->unlock();
    }

	void D3D9VertexBufferCore::writeData(UINT32 offset, UINT32 length, const void* source,
		BufferWriteType writeFlags)
	{
		GpuLockOptions lockOption = GBL_WRITE_ONLY;
		if(writeFlags == BufferWriteType::Discard)
			lockOption = GBL_WRITE_ONLY_DISCARD;
		else if(writeFlags == BufferWriteType::NoOverwrite)
			lockOption = GBL_WRITE_ONLY_NO_OVERWRITE;

		void* pDst = this->lock(offset, length, lockOption);
		memcpy(pDst, source, length);
		this->unlock();
	}

	void D3D9VertexBufferCore::notifyOnDeviceCreate(IDirect3DDevice9* d3d9Device)
	{			
		D3D9_DEVICE_ACCESS_CRITICAL_SECTION

		if (D3D9RenderSystem::getResourceManager()->getCreationPolicy() == RCP_CREATE_ON_ALL_DEVICES)
			createBuffer(d3d9Device, mBufferDesc.Pool);
	}

	void D3D9VertexBufferCore::notifyOnDeviceDestroy(IDirect3DDevice9* d3d9Device)
	{	
		D3D9_DEVICE_ACCESS_CRITICAL_SECTION

		auto iterFind = mMapDeviceToBufferResources.find(d3d9Device);

		if (iterFind != mMapDeviceToBufferResources.end())	
		{								
			SAFE_RELEASE(iterFind->second->mBuffer);

			if(iterFind->second != nullptr)
				bs_delete(iterFind->second);

			mMapDeviceToBufferResources.erase(iterFind);
		}	
	}

	void D3D9VertexBufferCore::notifyOnDeviceLost(IDirect3DDevice9* d3d9Device)
	{	
		D3D9_DEVICE_ACCESS_CRITICAL_SECTION

		if (mBufferDesc.Pool == D3DPOOL_DEFAULT)
		{
			auto iterFind = mMapDeviceToBufferResources.find(d3d9Device);

			if (iterFind != mMapDeviceToBufferResources.end())
			{				
				SAFE_RELEASE(iterFind->second->mBuffer);	
			}					
		}
	}

	void D3D9VertexBufferCore::notifyOnDeviceReset(IDirect3DDevice9* d3d9Device)
	{		
		D3D9_DEVICE_ACCESS_CRITICAL_SECTION

		if (D3D9RenderSystem::getResourceManager()->getCreationPolicy() == RCP_CREATE_ON_ALL_DEVICES)
			createBuffer(d3d9Device, mBufferDesc.Pool);
	}

	void D3D9VertexBufferCore::createBuffer(IDirect3DDevice9* d3d9Device, D3DPOOL ePool)
	{
		D3D9_DEVICE_ACCESS_CRITICAL_SECTION

		BufferResources* bufferResources;		
		HRESULT hr;

		// Find the vertex buffer of this device.
		auto iterFind = mMapDeviceToBufferResources.find(d3d9Device);
		if (iterFind != mMapDeviceToBufferResources.end())
		{
			bufferResources = iterFind->second;
			SAFE_RELEASE(bufferResources->mBuffer);
		}
		else
		{
			bufferResources = bs_new<BufferResources>();			
			mMapDeviceToBufferResources[d3d9Device] = bufferResources;
		}

		bufferResources->mBuffer = NULL;
		bufferResources->mOutOfDate = true;
		bufferResources->mLockOffset = 0;
		bufferResources->mLockLength = getSizeInBytes();
		bufferResources->mLockOptions = GBL_READ_WRITE;
		
		// Create the vertex buffer
		hr = d3d9Device->CreateVertexBuffer(
			static_cast<UINT>(mSizeInBytes), 
			D3D9Mappings::get(mUsage), 
			0,
			ePool,
			&bufferResources->mBuffer,
			NULL);

		if (FAILED(hr))
		{
			String msg = DXGetErrorDescription(hr);
			BS_EXCEPT(RenderingAPIException, "Cannot restore D3D9 vertex buffer: " + msg);
		}

		hr = bufferResources->mBuffer->GetDesc(&mBufferDesc);
		if (FAILED(hr))
		{
			String msg = DXGetErrorDescription(hr);
			BS_EXCEPT(RenderingAPIException, "Cannot get D3D9 Vertex buffer desc: " + msg);
		}		
	}

	IDirect3DVertexBuffer9* D3D9VertexBufferCore::getD3D9VertexBuffer()
	{
		IDirect3DDevice9* d3d9Device = D3D9RenderSystem::getActiveD3D9Device();
		auto iterFind = mMapDeviceToBufferResources.find(d3d9Device);

		// Case vertex buffer was not found for the current device -> create it.		
		if (iterFind == mMapDeviceToBufferResources.end() || iterFind->second->mBuffer == nullptr)
		{						
			createBuffer(d3d9Device, mBufferDesc.Pool);
			iterFind = mMapDeviceToBufferResources.find(d3d9Device);
		}

		if (iterFind->second->mOutOfDate)
			updateBufferResources(mSystemMemoryBuffer, iterFind->second);
		
		return iterFind->second->mBuffer;
	}	

	bool D3D9VertexBufferCore::updateBufferResources(const UINT8* systemMemoryBuffer, BufferResources* bufferResources)
	{		
		assert(bufferResources != nullptr);
		assert(bufferResources->mBuffer != nullptr);
		assert(bufferResources->mOutOfDate);
				
		if (bufferResources->mLockLength != 0)
		{
			void* dstBytes;
			HRESULT hr;

			// Lock the buffer.
			hr = bufferResources->mBuffer->Lock(
				static_cast<UINT>(bufferResources->mLockOffset),
				static_cast<UINT>(bufferResources->mLockLength),
				&dstBytes,
				D3D9Mappings::get(bufferResources->mLockOptions, mUsage));

			if (FAILED(hr))
			{
				String msg = DXGetErrorDescription(hr);
				BS_EXCEPT(RenderingAPIException, "Cannot lock D3D9 vertex buffer: " + msg);
			}

			memcpy(dstBytes, systemMemoryBuffer + bufferResources->mLockOffset, bufferResources->mLockLength);

			// Unlock the buffer.
			hr = bufferResources->mBuffer->Unlock();
			if (FAILED(hr))
			{
				String msg = DXGetErrorDescription(hr);
				BS_EXCEPT(RenderingAPIException, "Cannot unlock D3D9 vertex buffer: " + msg);
			}
		}

		bufferResources->mOutOfDate = false;
		bufferResources->mLockOffset = mSizeInBytes;
		bufferResources->mLockLength = 0;
		bufferResources->mLockOptions = GBL_READ_WRITE;

		return true;		
	}

	D3D9VertexBuffer::D3D9VertexBuffer(UINT32 vertexSize, UINT32 numVertices, GpuBufferUsage usage, bool useSystemMemory)
		: VertexBuffer(vertexSize, numVertices, usage, useSystemMemory)
	{ }

	SPtr<CoreObjectCore> D3D9VertexBuffer::createCore() const
	{
		return bs_shared_ptr<D3D9VertexBufferCore>(mUsage, mUseSystemMemory, mProperties);
	}
}
