#pragma once

#include "CmD3D11Prerequisites.h"
#include "CmGpuBuffer.h"
#include "CmGpuBufferView.h"

namespace CamelotEngine
{
	class CM_D3D11_EXPORT D3D11GpuBufferView : public GpuBufferView
	{
	public:
		D3D11GpuBufferView(ID3D11Buffer* buffer, GpuBufferType type, GPU_BUFFER_DESC& desc);
		virtual ~D3D11GpuBufferView();

	private:
		ID3D11ShaderResourceView* mSRV;
		ID3D11UnorderedAccessView* mUAV;

		ID3D11ShaderResourceView* createSRV(ID3D11Buffer* buffer, GpuBufferType type, UINT32 firstElement, UINT32 elementWidth, UINT32 numElements);
		ID3D11UnorderedAccessView* createUAV(ID3D11Buffer* buffer, GpuBufferType type, UINT32 firstElement, UINT32 numElements, bool useCounter);
	};
}