//__________________________ Banshee Project - A modern game development toolkit _________________________________//
//_____________________________________ www.banshee-project.com __________________________________________________//
//________________________ Copyright (c) 2014 Marko Pintera. All rights reserved. ________________________________//
#pragma once

#include "BsCorePrerequisites.h"
#include "BsImportOptions.h"
#include "BsGpuProgram.h"
#include "BsPixelUtil.h"

namespace BansheeEngine
{
	/**
	* @brief	Contains import options you may use to control how is a texture imported.
	*/
	class BS_CORE_EXPORT TextureImportOptions : public ImportOptions
	{
	public:
		TextureImportOptions();

		/**
		 * @brief	Sets the pixel format that the imported texture will have.
		 */
		void setFormat(PixelFormat format) { mFormat = format; }

		/**
		 * @brief	Enables or disables mipmap generation for the texture.	
		 */
		void setGenerateMipmaps(bool generate) { mGenerateMips = generate; }

		/**
		 * @brief	Sets the maximum mip level to generate when generating mipmaps. If 0
		 *			then maximum amount of mip levels will be generated.
		 */
		void setMaxMip(UINT32 maxMip) { mMaxMip = maxMip; }

		/**
		 * @brief	Gets the pixel format that the imported texture will have.
		 */
		PixelFormat getFormat() const { return mFormat; }

		/**
		 * @brief	Checks will be imported texture have automatically generated mipmaps.
		 */
		bool getGenerateMipmaps() const { return mGenerateMips; }

		/**
		 * @brief	Gets the maximum mip level to generate when generating mipmaps. If 0
		 *			then maximum amount of mip levels will be generated.
		 */
		UINT32 getMaxMip() const { return mMaxMip; }

		/************************************************************************/
		/* 								SERIALIZATION                      		*/
		/************************************************************************/
	public:
		friend class TextureImportOptionsRTTI;
		static RTTITypeBase* getRTTIStatic();
		virtual RTTITypeBase* getRTTI() const;

	private:
		PixelFormat mFormat;
		bool mGenerateMips;
		UINT32 mMaxMip;
	};
}