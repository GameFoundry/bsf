#include "CmD3D11GenericBuffer.h"
#include "CmD3D11RenderSystem.h"
#include "CmD3D11Device.h"
#include "CmException.h"

namespace CamelotEngine
{
	D3D11GenericBuffer::D3D11GenericBuffer(UINT32 elementCount, UINT32 elementSize, GenericBufferType type, GpuBufferUsage usage, bool randomGpuWrite, bool useCounter) 
		: GenericBuffer(elementCount, elementSize, type, usage, randomGpuWrite, useCounter), mBuffer(nullptr)
	{  
		D3D11_BUFFER_DESC desc;
		ZeroMemory(&desc, sizeof(desc));

		if(type == GBT_APPENDCONSUME && !randomGpuWrite)
			CM_EXCEPT(InvalidParametersException, "Append/Consume buffer must be created with randomGpuWrite enabled.");

		if(type != GBT_STRUCTURED && useCounter)
			CM_EXCEPT(InvalidParametersException, "Counter can only be used with a structured buffer.");

		if(useCounter && !randomGpuWrite)
			CM_EXCEPT(InvalidParametersException, "Counter can only be used with buffers that have randomGpuWrite enabled.");

		if(type == GBT_STRUCTURED || type == GBT_APPENDCONSUME)
		{
			desc.ByteWidth = elementCount * elementSize;
			desc.StructureByteStride = elementSize;
			desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
		}
		else if(type == GBT_RAW)
		{
			desc.ByteWidth = elementCount * elementSize;
			desc.StructureByteStride = 0;
			desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_ALLOW_RAW_VIEWS;
		}
		else if(type == GBT_INDIRECTARGUMENT)
		{
			desc.ByteWidth = elementCount * elementSize;
			desc.StructureByteStride = 0;
			desc.MiscFlags = D3D11_RESOURCE_MISC_DRAWINDIRECT_ARGS;
		}

		if(!randomGpuWrite)
		{
			desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

			if(usage == GBU_STATIC)
			{
				desc.Usage = D3D11_USAGE_DEFAULT;
				desc.CPUAccessFlags = 0;
			}
			else if(usage == GBU_DYNAMIC)
			{
				desc.Usage = D3D11_USAGE_DYNAMIC;
				desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
			}
			else
				CM_EXCEPT(InternalErrorException, "Invalid usage flag: " + toString(usage));
		}
		else
		{
			desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
			desc.Usage = D3D11_USAGE_DEFAULT;
			desc.CPUAccessFlags = 0;
		}

		D3D11RenderSystem* d3d11rs = static_cast<D3D11RenderSystem*>(D3D11RenderSystem::instancePtr());
		HRESULT hr = d3d11rs->getPrimaryDevice().getD3D11Device()->CreateBuffer(&desc, nullptr, &mBuffer);

		if (FAILED(hr) || d3d11rs->getPrimaryDevice().hasError())
		{
			String msg = d3d11rs->getPrimaryDevice().getErrorDescription();
			CM_EXCEPT(RenderingAPIException, "Cannot create D3D11 buffer: " + msg);
		}
	}

	D3D11GenericBuffer::~D3D11GenericBuffer() 
	{
		SAFE_RELEASE(mBuffer);
	}

	ID3D11ShaderResourceView* D3D11GenericBuffer::createSRV(UINT32 firstElement, UINT32 elementWidth, UINT32 numElements)
	{
		if(mType == GBT_APPENDCONSUME)
			CM_EXCEPT(InvalidParametersException, "Cannot create ShaderResourceView for an append/consume buffer.");

		D3D11_SHADER_RESOURCE_VIEW_DESC desc;
		ZeroMemory(&desc, sizeof(desc));

		if(mType == GBT_STRUCTURED)
		{
			desc.Format = DXGI_FORMAT_UNKNOWN;
			desc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
			desc.Buffer.ElementOffset = firstElement * elementWidth;
			desc.Buffer.ElementWidth = elementWidth;
		}
		else if(mType == GBT_RAW)
		{
			desc.Format = DXGI_FORMAT_R32_TYPELESS;
			desc.ViewDimension = D3D11_SRV_DIMENSION_BUFFEREX;
			desc.BufferEx.Flags = D3D11_BUFFEREX_SRV_FLAG_RAW;
			desc.BufferEx.FirstElement = firstElement;
			desc.BufferEx.NumElements = numElements;
		}
		else if(mType == GBT_INDIRECTARGUMENT)
		{
			desc.Format = DXGI_FORMAT_R32_UINT;
			desc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
			desc.Buffer.ElementOffset = firstElement * elementWidth;
			desc.Buffer.ElementWidth = elementWidth;
		}

		ID3D11ShaderResourceView* srv = nullptr;

		D3D11RenderSystem* d3d11rs = static_cast<D3D11RenderSystem*>(D3D11RenderSystem::instancePtr());
		HRESULT hr = d3d11rs->getPrimaryDevice().getD3D11Device()->CreateShaderResourceView(mBuffer, &desc, &srv);

		if (FAILED(hr) || d3d11rs->getPrimaryDevice().hasError())
		{
			String msg = d3d11rs->getPrimaryDevice().getErrorDescription();
			CM_EXCEPT(RenderingAPIException, "Cannot create ShaderResourceView: " + msg);
		}

		return srv;
	}

	ID3D11UnorderedAccessView* D3D11GenericBuffer::createUAV(UINT32 firstElement, UINT32 numElements)
	{
		D3D11_UNORDERED_ACCESS_VIEW_DESC desc;
		ZeroMemory(&desc, sizeof(desc));

		desc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;

		if(mType == GBT_STRUCTURED)
		{
			desc.Format = DXGI_FORMAT_UNKNOWN;
			desc.Buffer.FirstElement = firstElement;
			desc.Buffer.NumElements = numElements;

			if(mUseCounter)
				desc.Buffer.Flags = D3D11_BUFFER_UAV_FLAG_COUNTER;
			else
				desc.Buffer.Flags = 0;
		}
		else if(mType == GBT_RAW)
		{
			desc.Format = DXGI_FORMAT_R32_TYPELESS;
			desc.Buffer.Flags = D3D11_BUFFER_UAV_FLAG_RAW;
			desc.Buffer.FirstElement = firstElement;
			desc.Buffer.NumElements = numElements;
		}
		else if(mType == GBT_INDIRECTARGUMENT)
		{
			desc.Format = DXGI_FORMAT_R32_UINT;
			desc.Buffer.Flags = 0;
			desc.Buffer.FirstElement = firstElement;
			desc.Buffer.NumElements = numElements;
		}
		else if(mType == GBT_APPENDCONSUME)
		{
			desc.Format = DXGI_FORMAT_UNKNOWN;
			desc.Buffer.Flags = D3D11_BUFFER_UAV_FLAG_APPEND;
			desc.Buffer.FirstElement = firstElement;
			desc.Buffer.NumElements = numElements;
		}

		ID3D11UnorderedAccessView* uav = nullptr;

		D3D11RenderSystem* d3d11rs = static_cast<D3D11RenderSystem*>(D3D11RenderSystem::instancePtr());
		HRESULT hr = d3d11rs->getPrimaryDevice().getD3D11Device()->CreateUnorderedAccessView(mBuffer, &desc, &uav);

		if (FAILED(hr) || d3d11rs->getPrimaryDevice().hasError())
		{
			String msg = d3d11rs->getPrimaryDevice().getErrorDescription();
			CM_EXCEPT(RenderingAPIException, "Cannot create UnorderedAccessView: " + msg);
		}

		return uav;
	}

	void* D3D11GenericBuffer::lock(UINT32 offset, UINT32 length, GpuLockOptions options)
	{
		return nullptr;
	}

	void D3D11GenericBuffer::unlock()
	{

	}

	void D3D11GenericBuffer::readData(UINT32 offset, UINT32 length, void* pDest)
	{

	}

	void D3D11GenericBuffer::writeData(UINT32 offset, UINT32 length, const void* pSource, bool discardWholeBuffer)
	{

	}

	void D3D11GenericBuffer::copyData(GenericBuffer& srcBuffer, UINT32 srcOffset, 
		UINT32 dstOffset, UINT32 length, bool discardWholeBuffer)
	{

	}
}