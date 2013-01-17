#pragma once

#include "CmD3D11Prerequisites.h"
#include "CmTextureView.h"

namespace CamelotEngine
{
	class CM_D3D11_EXPORT D3D11TextureView : public TextureView
	{
	public:
		D3D11TextureView(Texture* texture, TEXTURE_VIEW_DESC& _desc);
		virtual ~D3D11TextureView();

	private:
		ID3D11ShaderResourceView*	mSRV;
		ID3D11RenderTargetView*		mRTV;
		ID3D11UnorderedAccessView*	mUAV;
		ID3D11DepthStencilView*		mDSV;

		ID3D11ShaderResourceView* createSRV(D3D11Texture* texture, 
			UINT32 mostDetailMip, UINT32 numMips, UINT32 firstArraySlice, UINT32 numArraySlices);

		ID3D11RenderTargetView* createRTV(D3D11Texture* texture, 
			UINT32 mostDetailMip, UINT32 numMips, UINT32 firstArraySlice, UINT32 numArraySlices);

		ID3D11UnorderedAccessView* createUAV(D3D11Texture* texture, 
			UINT32 mostDetailMip, UINT32 numMips, UINT32 firstArraySlice, UINT32 numArraySlices);

		ID3D11DepthStencilView* createDSV(D3D11Texture* texture, 
			UINT32 mostDetailMip, UINT32 numMips, UINT32 firstArraySlice, UINT32 numArraySlices);
	};
}