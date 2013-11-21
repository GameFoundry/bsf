#pragma once

#include "CmD3D11Prerequisites.h"
#include "CmTexture.h"

namespace CamelotFramework
{
	class D3D11Texture : public Texture
	{
	public:
		~D3D11Texture();

		ID3D11Resource* getDX11Resource() const { return mTex; }
		ID3D11ShaderResourceView* getSRV() const { return mShaderResourceView; }
		D3D11_SRV_DIMENSION getDimension() const { return mDimension; }
		DXGI_FORMAT getDXGIFormat() const { return mDXGIFormat; }

	protected:
		friend class D3D11TextureManager;

		D3D11Texture();

		ID3D11Texture1D* m1DTex;
		ID3D11Texture2D* m2DTex;
		ID3D11Texture3D* m3DTex;	
		ID3D11Resource* mTex;		

		ID3D11ShaderResourceView* mShaderResourceView;
		D3D11_SHADER_RESOURCE_VIEW_DESC mSRVDesc;
		D3D11_SRV_DIMENSION mDimension;
		DXGI_FORMAT mDXGIFormat;

		ID3D11Resource* mStagingBuffer;
		PixelData* mStaticBuffer;
		UINT32 mLockedSubresourceIdx;
		bool mLockedForReading;

		/**
		 * @copydoc Texture::lock
		 */
		PixelData lockImpl(GpuLockOptions options, UINT32 mipLevel = 0, UINT32 face = 0);

		/**
		 * @copydoc Texture::unlock
		 */
		void unlockImpl();

		/**
		 * @copydoc Texture::copy
		 */
		void copyImpl(TexturePtr& target);

		/**
		 * @copydoc Texture::readData
		 */
		void readData(PixelData& dest, UINT32 mipLevel = 0, UINT32 face = 0);

		/**
		 * @copydoc Texture::writeData
		 */
		void writeData(const PixelData& src, UINT32 mipLevel = 0, UINT32 face = 0, bool discardWholeBuffer = false);

		/// internal method, create a blank normal 1D Dtexture
		void create1DTex();
		/// internal method, create a blank normal 2D texture
		void create2DTex();
		/// internal method, create a blank cube texture
		void create3DTex();

		void createStagingBuffer();

		void* map(ID3D11Resource* res, D3D11_MAP flags, UINT32 mipLevel, UINT32 face, UINT32& rowPitch, UINT32& slicePitch);
		void unmap(ID3D11Resource* res);

		void* mapstagingbuffer(D3D11_MAP flags, UINT32 mipLevel, UINT32 face, UINT32& rowPitch, UINT32& slicePitch);
		void unmapstagingbuffer();
		
		void* mapstaticbuffer(PixelData lock, UINT32 mipLevel, UINT32 slice);
		void unmapstaticbuffer();

		/**
		 * @copydoc	Texture::initialize_internal()
		 */
		void initialize_internal();

		/**
		 * @copydoc Texture::destroy_internal()
		 */
		void destroy_internal();

		TextureViewPtr createView();
	};
}