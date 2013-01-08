#pragma once

#include "CmD3D11Prerequisites.h"
#include "CmGenericBuffer.h"
#include "CmGenericBufferView.h"

namespace CamelotEngine
{
	class CM_D3D11_EXPORT D3D11GenericBufferView : public GenericBufferView
	{
	public:
		D3D11GenericBufferView(ID3D11Buffer* buffer, GenericBufferType type, UINT32 firstElement, UINT32 elementWidth, UINT32 numElements, bool randomGpuWrite, bool useCounter = false);
		virtual ~D3D11GenericBufferView();

	private:
		ID3D11ShaderResourceView* mSRV;
		ID3D11UnorderedAccessView* mUAV;

		ID3D11ShaderResourceView* createSRV(ID3D11Buffer* buffer, GenericBufferType type, UINT32 firstElement, UINT32 elementWidth, UINT32 numElements);
		ID3D11UnorderedAccessView* createUAV(ID3D11Buffer* buffer, GenericBufferType type, UINT32 firstElement, UINT32 numElements, bool useCounter);
	};
}