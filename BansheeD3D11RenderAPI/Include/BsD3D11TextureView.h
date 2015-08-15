#pragma once

#include "BsD3D11Prerequisites.h"
#include "BsTextureView.h"

namespace BansheeEngine
{
	/**
	 * @brief	DirectX implementation of a texture resource view.
	 */
	class BS_D3D11_EXPORT D3D11TextureView : public TextureView
	{
	public:
		~D3D11TextureView();

		/**
		 * @brief	Returns a shader resource view. Caller must take care this texture view
		 *			actually contains a shader resource view, otherwise it returns null.
		 */
		ID3D11ShaderResourceView* getSRV() const { return mSRV; }

		/**
		 * @brief	Returns a render target view. Caller must take care this texture view
		 *			actually contains a render target view, otherwise it returns null.
		 */
		ID3D11RenderTargetView*	getRTV() const { return mRTV; }

		/**
		 * @brief	Returns a unordered access view. Caller must take care this texture view
		 *			actually contains a unordered access view, otherwise it returns null.
		 */
		ID3D11UnorderedAccessView* getUAV() const { return mUAV; }

		/**
		 * @brief	Returns a depth stencil view. Caller must take care this texture view
		 *			actually contains a depth stencil view, otherwise it returns null.
		 */
		ID3D11DepthStencilView*	getDSV() const { return mDSV; }

	protected:
		friend class D3D11TextureCore;

		D3D11TextureView(const SPtr<TextureCore>& texture, const TEXTURE_VIEW_DESC& desc);
	private:
		/**
		 * @brief	Creates a shader resource view that allows the provided surfaces 
		 *			to be bound as normal shader resources.
		 *
		 * @param	texture			Texture to create the resource view for.
		 * @param	mostDetailMip	First mip level to create the resource view for (0 - base level). 
		 * @param	numMips			Number of mip levels to create the view for.
		 * @param	firstArraySlice	First array slice to create the view for. This will be array index 
		 *							for 1D and 2D array textures, texture slice index for 3D textures, and face
		 *							index for cube textures (cube index * 6).
		 * @param	nuMArraySlices	Number of array slices to create the view for. This will be number of
		 *							array elements for 1D and 2D array textures, number of slices for 3D textures,
		 *							and number of cubes for cube textures.
		 */
		ID3D11ShaderResourceView* createSRV(D3D11TextureCore* texture, 
			UINT32 mostDetailMip, UINT32 numMips, UINT32 firstArraySlice, UINT32 numArraySlices);

		/**
		 * @brief	Creates a shader resource view that allows the provided surfaces 
		 *			to be bound as render targets.
		 *
		 * @param	texture			Texture to create the resource view for.
		 * @param	mipSlice		Mip level to create the resource view for (0 - base level).
		 * @param	firstArraySlice	First array slice to create the view for. This will be array index 
		 *							for 1D and 2D array textures, texture slice index for 3D textures, and face
		 *							index for cube textures (cube index * 6).
		 * @param	nuMArraySlices	Number of array slices to create the view for. This will be number of
		 *							array elements for 1D and 2D array textures, number of slices for 3D textures,
		 *							and number of cubes for cube textures.
		 */
		ID3D11RenderTargetView* createRTV(D3D11TextureCore* texture, 
			UINT32 mipSlice, UINT32 firstArraySlice, UINT32 numArraySlices);

		/**
		 * @brief	Creates a shader resource view that allows the provided surfaces 
		 *			to be bound as unordered access buffers.
		 *
		 * @param	texture			Texture to create the resource view for.
		 * @param	mipSlice		Mip level to create the resource view for (0 - base level).
		 * @param	firstArraySlice	First array slice to create the view for. This will be array index
		 *							for 1D and 2D array textures, texture slice index for 3D textures, and face
		 *							index for cube textures (cube index * 6).
		 * @param	nuMArraySlices	Number of array slices to create the view for. This will be number of
		 *							array elements for 1D and 2D array textures, number of slices for 3D textures,
		 *							and number of cubes for cube textures.
		 */
		ID3D11UnorderedAccessView* createUAV(D3D11TextureCore* texture, 
			UINT32 mipSlice, UINT32 firstArraySlice, UINT32 numArraySlices);

		/**
		 * @brief	Creates a shader resource view that allows the provided surfaces 
		 *			to be bound as depth stencil buffers.
		 *
		 * @param	texture			Texture to create the resource view for.
		 * @param	mipSlice		Mip level to create the resource view for (0 - base level).
		 * @param	firstArraySlice	First array slice to create the view for. This will be array index
		 *							for 1D and 2D array textures, texture slice index for 3D textures, and face
		 *							index for cube textures (cube index * 6).
		 * @param	nuMArraySlices	Number of array slices to create the view for. This will be number of
		 *							array elements for 1D and 2D array textures, number of slices for 3D textures,
		 *							and number of cubes for cube textures.
		 */
		ID3D11DepthStencilView* createDSV(D3D11TextureCore* texture, 
			UINT32 mipSlice, UINT32 firstArraySlice, UINT32 numArraySlices);

		ID3D11ShaderResourceView* mSRV;
		ID3D11RenderTargetView* mRTV;
		ID3D11UnorderedAccessView* mUAV;
		ID3D11DepthStencilView*	mDSV;
	};
}