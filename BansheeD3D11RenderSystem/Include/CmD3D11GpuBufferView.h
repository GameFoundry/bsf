#pragma once

#include "CmD3D11Prerequisites.h"
#include "CmGpuBufferView.h"

namespace BansheeEngine
{
	class BS_D3D11_EXPORT D3D11GpuBufferView : public GpuBufferView
	{
	public:
		D3D11GpuBufferView();
		virtual ~D3D11GpuBufferView();

		void initialize(GpuBufferPtr buffer, GPU_BUFFER_DESC& desc);

	private:
		ID3D11ShaderResourceView* mSRV;
		ID3D11UnorderedAccessView* mUAV;

		ID3D11ShaderResourceView* createSRV(D3D11GpuBuffer* buffer, UINT32 firstElement, UINT32 elementWidth, UINT32 numElements);
		ID3D11UnorderedAccessView* createUAV(D3D11GpuBuffer* buffer, UINT32 firstElement, UINT32 numElements, bool useCounter);
	};
}