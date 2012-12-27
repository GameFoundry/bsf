#pragma once

#include "CmD3D11Prerequisites.h"
#include "CmTexture.h"

namespace CamelotEngine
{
	class D3D11Texture : public Texture
	{
	public:
		~D3D11Texture();

		/**
		 * @copydoc Texture::setRawPixels_internal
		 */
		void setRawPixels_internal(const PixelData& data, UINT32 face = 0, UINT32 mip = 0);

		/**
		 * @copydoc Texture::getRawPixels_internal
		 */
		void getRawPixels_internal(UINT32 face, UINT32 mip, AsyncOp& op);

		/**
		 * @copydoc Texture::copy_internal
		 */
		void copy_internal(TexturePtr& target);

		PixelData lock(LockOptions options, UINT32 mipLevel = 0, UINT32 face = 0);
		void unlock();

		ID3D11Resource* getDX11Resource() const { return mTex; }
	protected:
		D3D11Texture();

		ID3D11Texture1D* m1DTex;
		ID3D11Texture2D* m2DTex;
		ID3D11Texture3D* m3DTex;	
		ID3D11Resource* mTex;		

		ID3D11ShaderResourceView* mShaderResourceView;
		D3D11_SHADER_RESOURCE_VIEW_DESC mSRVDesc;

		ID3D11Resource* mStagingBuffer;
		PixelData* mStaticBuffer;
		UINT32 mLockedSubresourceIdx;
		bool mLockedForReading;

		/// internal method, create a blank normal 1D Dtexture
		void _create1DTex();
		/// internal method, create a blank normal 2D texture
		void _create2DTex();
		/// internal method, create a blank cube texture
		void _create3DTex();

		void _createStagingBuffer();

		void* _map(ID3D11Resource* res, D3D11_MAP flags, UINT32 mipLevel, UINT32 face);
		void _unmap(ID3D11Resource* res);

		void* _mapstagingbuffer(D3D11_MAP flags, UINT32 mipLevel, UINT32 face);
		void _unmapstagingbuffer();
		
		void* _mapstaticbuffer(PixelData lock, UINT32 mipLevel, UINT32 slice);
		void _unmapstaticbuffer();

		/**
		 * @brief	Resource::initialize_internal
		 */
		void initialize_internal();

		/**
		 * @copydoc Texture::createInternalResourcesImpl
		 */
		void createInternalResourcesImpl();
		
		/**
		 * @copydoc Texture::freeInternalResourcesImpl
		 */
		void freeInternalResourcesImpl();
	};
}