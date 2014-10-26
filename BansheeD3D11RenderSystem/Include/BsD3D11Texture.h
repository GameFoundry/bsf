#pragma once

#include "BsD3D11Prerequisites.h"
#include "BsTexture.h"

namespace BansheeEngine
{
	/**
	 * @brief	DirectX 11 implementation of a texture.
	 */
	class D3D11Texture : public Texture
	{
	public:
		~D3D11Texture();

		/**
		 * @brief	Returns internal DX11 texture resource object.
		 */
		ID3D11Resource* getDX11Resource() const { return mTex; }

		/**
		 * @brief	Returns shader resource view associated with the texture.
		 */
		ID3D11ShaderResourceView* getSRV() const { return mShaderResourceView; }

		/**
		 * @brief	Returns unordered access view associated with the texture.
		 *
		 * @note	Used for random read/write operations from GPU programs.
		 */
		ID3D11UnorderedAccessView* getUAV() const { return mUnorderedAccessView; }

		/**
		 * @brief	Returns DXGI pixel format used by the texture.
		 */
		DXGI_FORMAT getDXGIFormat() const { return mDXGIFormat; }

	protected:
		friend class D3D11TextureManager;

		D3D11Texture();

		/**
		* @copydoc	Texture::initialize_internal()
		*/
		void initialize_internal();

		/**
		* @copydoc Texture::destroy_internal()
		*/
		void destroy_internal();

		/**
		 * @copydoc Texture::lockImpl
		 */
		PixelData lockImpl(GpuLockOptions options, UINT32 mipLevel = 0, UINT32 face = 0);

		/**
		 * @copydoc Texture::unlockImpl
		 */
		void unlockImpl();

		/**
		 * @copydoc Texture::copyImpl
		 */
		void copyImpl(UINT32 srcFace, UINT32 srcMipLevel, UINT32 destFace, UINT32 destMipLevel, TexturePtr& target);

		/**
		 * @copydoc Texture::readData
		 */
		void readData(PixelData& dest, UINT32 mipLevel = 0, UINT32 face = 0);

		/**
		 * @copydoc Texture::writeData
		 */
		void writeData(const PixelData& src, UINT32 mipLevel = 0, UINT32 face = 0, bool discardWholeBuffer = false);

		/**
		 * @brief	Creates a blank DX11 1D texture object.
		 */
		void create1DTex();

		/**
		 * @brief	Creates a blank DX11 2D texture object.
		 */
		void create2DTex();

		/**
		 * @brief	Creates a blank DX11 3D texture object.
		 */
		void create3DTex();

		/**
		 * @brief	Creates a staging buffer that is used as a temporary buffer for read operations on textures
		 *			that do not support direct reading.
		 */
		void createStagingBuffer();

		/**
		 * @brief	Maps the specified texture surface for reading/writing. 
		 *
		 * @param	res			Texture resource to map.
		 * @param	flags		Mapping flags that let the API know what are we planning to do with mapped memory.
		 * @param	mipLevel	Mip level to map (0 being the base level).
		 * @param	face		Texture face to map, in case texture has more than one.
		 * @param	rowPitch	Output size of a single row in bytes.
		 * @param	slicePitch	Output size of a single slice in bytes (relevant only for 3D textures).
		 *
		 * @returns	Pointer to the mapped area of memory.
		 *
		 * @note	Non-staging textures must be dynamic in order to be mapped directly and only for writing.
		 *			No restrictions are made on staging textures.
		 */
		void* map(ID3D11Resource* res, D3D11_MAP flags, UINT32 mipLevel, UINT32 face, UINT32& rowPitch, UINT32& slicePitch);

		/**
		 * @brief	Unmaps a previously mapped texture.
		 */
		void unmap(ID3D11Resource* res);

		/**
		 * @brief	Copies texture data into a staging buffer and maps the staging buffer. Will create a staging
		 *			buffer if one doesn't already exist (potentially wasting a lot of memory).
		 *
		 * @param	flags		Mapping flags that let the API know what are we planning to do with mapped memory.
		 * @param	mipLevel	Mip level to map (0 being the base level).
		 * @param	face		Texture face to map, in case texture has more than one.
		 * @param	rowPitch	Output size of a single row in bytes.
		 * @param	slicePitch	Output size of a single slice in bytes (relevant only for 3D textures).
		 *
		 * @returns	Pointer to the mapped area of memory.
		 */
		void* mapstagingbuffer(D3D11_MAP flags, UINT32 mipLevel, UINT32 face, UINT32& rowPitch, UINT32& slicePitch);

		/**
		 * @brief	Unmaps a previously mapped staging buffer.
		 */
		void unmapstagingbuffer();
		
		/**
		 * @brief	Maps a static buffer, for writing only. Returned pointer points to temporary CPU memory
		 *			that will be copied to the mapped resource on "unmap" call.
		 *
		 * @param	flags		Mapping flags that let the API know what are we planning to do with mapped memory.
		 * @param	mipLevel	Mip level to map (0 being the base level).
		 * @param	face		Texture face to map, in case texture has more than one.
		 * @param	rowPitch	Output size of a single row in bytes.
		 * @param	slicePitch	Output size of a single slice in bytes (relevant only for 3D textures).
		 *
		 * @returns	Pointer to the mapped area of memory.
		 */
		void* mapstaticbuffer(PixelData lock, UINT32 mipLevel, UINT32 slice);

		/**
		 * @brief	Unmaps a previously mapped static buffer and flushes its data to the actual GPU buffer.
		 */
		void unmapstaticbuffer();

		/**
		 * @brief	Creates an empty and uninitialized texture view object.
		 */
		TextureViewPtr createView();

	protected:
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
	};
}