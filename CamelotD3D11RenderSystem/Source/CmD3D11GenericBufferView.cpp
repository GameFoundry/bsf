#include "CmD3D11GenericBufferView.h"
#include "CmD3D11RenderSystem.h"
#include "CmD3D11Device.h"
#include "CmException.h"

namespace CamelotEngine
{
	D3D11GenericBufferView::D3D11GenericBufferView(ID3D11Buffer* buffer, GenericBufferType type, 
		UINT32 firstElement, UINT32 elementWidth, UINT32 numElements, bool randomGpuWrite, bool useCounter)
		:GenericBufferView(firstElement, elementWidth, numElements, randomGpuWrite), mSRV(nullptr), mUAV(nullptr)
	{
		if(randomGpuWrite)
			mSRV = createSRV(buffer, type, firstElement, elementWidth, numElements);
		else
			mUAV = createUAV(buffer, type, firstElement, numElements, useCounter);
	}

	D3D11GenericBufferView::~D3D11GenericBufferView()
	{
		SAFE_RELEASE(mSRV);
		SAFE_RELEASE(mUAV);
	}

	ID3D11ShaderResourceView* D3D11GenericBufferView::createSRV(ID3D11Buffer* buffer, 
		GenericBufferType type, UINT32 firstElement, UINT32 elementWidth, UINT32 numElements)
	{
		if(type == GBT_APPENDCONSUME)
			CM_EXCEPT(InvalidParametersException, "Cannot create ShaderResourceView for an append/consume buffer.");

		D3D11_SHADER_RESOURCE_VIEW_DESC desc;
		ZeroMemory(&desc, sizeof(desc));

		if(type == GBT_STRUCTURED)
		{
			desc.Format = DXGI_FORMAT_UNKNOWN;
			desc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
			desc.Buffer.ElementOffset = firstElement * elementWidth;
			desc.Buffer.ElementWidth = elementWidth;
		}
		else if(type == GBT_RAW)
		{
			desc.Format = DXGI_FORMAT_R32_TYPELESS;
			desc.ViewDimension = D3D11_SRV_DIMENSION_BUFFEREX;
			desc.BufferEx.Flags = D3D11_BUFFEREX_SRV_FLAG_RAW;
			desc.BufferEx.FirstElement = firstElement;
			desc.BufferEx.NumElements = numElements;
		}
		else if(type == GBT_INDIRECTARGUMENT)
		{
			desc.Format = DXGI_FORMAT_R32_UINT;
			desc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
			desc.Buffer.ElementOffset = firstElement * elementWidth;
			desc.Buffer.ElementWidth = elementWidth;
		}

		ID3D11ShaderResourceView* srv = nullptr;

		D3D11RenderSystem* d3d11rs = static_cast<D3D11RenderSystem*>(D3D11RenderSystem::instancePtr());
		HRESULT hr = d3d11rs->getPrimaryDevice().getD3D11Device()->CreateShaderResourceView(buffer, &desc, &srv);

		if (FAILED(hr) || d3d11rs->getPrimaryDevice().hasError())
		{
			String msg = d3d11rs->getPrimaryDevice().getErrorDescription();
			CM_EXCEPT(RenderingAPIException, "Cannot create ShaderResourceView: " + msg);
		}

		return srv;
	}

	ID3D11UnorderedAccessView* D3D11GenericBufferView::createUAV(ID3D11Buffer* buffer, 
		GenericBufferType type, UINT32 firstElement, UINT32 numElements, bool useCounter)
	{
		D3D11_UNORDERED_ACCESS_VIEW_DESC desc;
		ZeroMemory(&desc, sizeof(desc));

		desc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;

		if(type == GBT_STRUCTURED)
		{
			desc.Format = DXGI_FORMAT_UNKNOWN;
			desc.Buffer.FirstElement = firstElement;
			desc.Buffer.NumElements = numElements;

			if(useCounter)
				desc.Buffer.Flags = D3D11_BUFFER_UAV_FLAG_COUNTER;
			else
				desc.Buffer.Flags = 0;
		}
		else if(type == GBT_RAW)
		{
			desc.Format = DXGI_FORMAT_R32_TYPELESS;
			desc.Buffer.Flags = D3D11_BUFFER_UAV_FLAG_RAW;
			desc.Buffer.FirstElement = firstElement;
			desc.Buffer.NumElements = numElements;
		}
		else if(type == GBT_INDIRECTARGUMENT)
		{
			desc.Format = DXGI_FORMAT_R32_UINT;
			desc.Buffer.Flags = 0;
			desc.Buffer.FirstElement = firstElement;
			desc.Buffer.NumElements = numElements;
		}
		else if(type == GBT_APPENDCONSUME)
		{
			desc.Format = DXGI_FORMAT_UNKNOWN;
			desc.Buffer.Flags = D3D11_BUFFER_UAV_FLAG_APPEND;
			desc.Buffer.FirstElement = firstElement;
			desc.Buffer.NumElements = numElements;
		}

		ID3D11UnorderedAccessView* uav = nullptr;

		D3D11RenderSystem* d3d11rs = static_cast<D3D11RenderSystem*>(D3D11RenderSystem::instancePtr());
		HRESULT hr = d3d11rs->getPrimaryDevice().getD3D11Device()->CreateUnorderedAccessView(buffer, &desc, &uav);

		if (FAILED(hr) || d3d11rs->getPrimaryDevice().hasError())
		{
			String msg = d3d11rs->getPrimaryDevice().getErrorDescription();
			CM_EXCEPT(RenderingAPIException, "Cannot create UnorderedAccessView: " + msg);
		}

		return uav;
	}
}