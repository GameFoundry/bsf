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
		 * @brief	Sets the pixel format to import as.
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
		 * @brief	Sets whether the texture data is also stored in CPU memory.
		 */
		void setCPUReadable(bool readable) { mCPUReadable = readable; }

		/**
		 * @brief	Sets whether the texture data should be treated as if its in sRGB (gamma) space.
		 *			Such texture will be converted by hardware to linear space before use.
		 */
		void setSRGB(bool sRGB) { mSRGB = sRGB; }

		/**
		 * @brief	Gets the pixel format to import as.
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

		/**
		 * @brief	Retrieves whether the texture data is also stored in CPU memory.
		 */
		bool getCPUReadable() const { return mCPUReadable; }

		/**
		 * @brief	Retrieves whether the texture data should be treated as if its in sRGB (gamma) space.
		 *			Such texture will be converted by hardware to linear space before use.
		 */
		bool getSRGB() const { return mSRGB; }

		/************************************************************************/
		/* 								SERIALIZATION                      		*/
		/************************************************************************/
	public:
		friend class TextureImportOptionsRTTI;
		static RTTITypeBase* getRTTIStatic();
		virtual RTTITypeBase* getRTTI() const override;

	private:
		PixelFormat mFormat;
		bool mGenerateMips;
		UINT32 mMaxMip;
		bool mCPUReadable;
		bool mSRGB;
	};
}