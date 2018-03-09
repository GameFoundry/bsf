//************************************ bs::framework - Copyright 2018 Marko Pintera **************************************//
//*********** Licensed under the MIT license. See LICENSE.md for full terms. This notice is not to be removed. ***********//
#pragma once

#include "BsCorePrerequisites.h"
#include "Importer/BsImportOptions.h"
#include "Image/BsPixelUtil.h"

namespace bs
{
	/** @addtogroup Importer
	 *  @{
	 */

	/** Contains import options you may use to control how is a texture imported. */
	class BS_CORE_EXPORT TextureImportOptions : public ImportOptions
	{
	public:
		TextureImportOptions();

		/** Sets the pixel format to import as. */
		void setFormat(PixelFormat format) { mFormat = format; }

		/** Enables or disables mipmap generation for the texture. */
		void setGenerateMipmaps(bool generate) { mGenerateMips = generate; }

		/**
		 * Sets the maximum mip level to generate when generating mipmaps. If 0 then maximum amount of mip levels will be 
		 * generated.
		 */
		void setMaxMip(UINT32 maxMip) { mMaxMip = maxMip; }

		/** Sets whether the texture data is also stored in main memory, available for fast CPU access. */
		void setCPUCached(bool cached) { mCPUCached = cached; }

		/** 
		 * Sets whether the texture data should be treated as if its in sRGB (gamma) space. Such texture will be converted 
		 * by hardware to linear space before use on the GPU.
		 */
		void setSRGB(bool sRGB) { mSRGB = sRGB; }

		/** Gets the pixel format to import as. */
		PixelFormat getFormat() const { return mFormat; }

		/** Checks will be imported texture have automatically generated mipmaps. */
		bool getGenerateMipmaps() const { return mGenerateMips; }

		/**
		 * Gets the maximum mip level to generate when generating mipmaps. If 0 then maximum amount of mip levels will be 
		 * generated.
		 */
		UINT32 getMaxMip() const { return mMaxMip; }

		/** Retrieves whether the texture data is also stored in main memory, available for fast CPU access. */
		bool getCPUCached() const { return mCPUCached; }

		/**
		 * Retrieves whether the texture data should be treated as if its in sRGB (gamma) space. Such texture will be 
		 * converted by hardware to linear space before use on the GPU.
		 */
		bool getSRGB() const { return mSRGB; }

		/** 
		 * Determines should the texture be imported as a cubemap. See setCubemapSource to choose how will the source
		 * texture be converted to a cubemap.
		 */
		void setIsCubemap(bool cubemap) { mCubemap = cubemap; }

		/** Checks if the texture will be imported as a cubemap. */
		bool getIsCubemap() const { return mCubemap; }

		/** 
		 * Sets a value that determines how should the source texture be interpreted when generating a cubemap. Only
		 * relevant when setIsCubemap() is set to true.
		 */
		void setCubemapSourceType(CubemapSourceType type) { mCubemapSourceType = type; }

		/** 
		 * Returns a value that determines how should the source texture be interpreted when generating a cubemap. Only
		 * relevant when setIsCubemap() is set to true.
		 */
		CubemapSourceType getCubemapSourceType() const { return mCubemapSourceType; }

		/** Creates a new import options object that allows you to customize how are textures imported. */
		static SPtr<TextureImportOptions> create();

		/************************************************************************/
		/* 								SERIALIZATION                      		*/
		/************************************************************************/
	public:
		friend class TextureImportOptionsRTTI;
		static RTTITypeBase* getRTTIStatic();
		RTTITypeBase* getRTTI() const override;

	private:
		PixelFormat mFormat;
		bool mGenerateMips;
		UINT32 mMaxMip;
		bool mCPUCached;
		bool mSRGB;
		bool mCubemap;
		CubemapSourceType mCubemapSourceType;
	};

	/** @} */
}