#pragma once

#include "BsD3D11Prerequisites.h"
#include "BsTextureView.h"

namespace BansheeEngine
{
	class BS_D3D11_EXPORT D3D11TextureView : public TextureView
	{
	public:
		ID3D11ShaderResourceView*	getSRV() const { return mSRV; }
		ID3D11RenderTargetView*		getRTV() const { return mRTV; }
		ID3D11UnorderedAccessView*	getUAV() const { return mUAV; }
		ID3D11DepthStencilView*		getDSV() const { return mDSV; }

	protected:
		friend class D3D11Texture;

		D3D11TextureView();

		void initialize_internal();

		void destroy_internal();

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