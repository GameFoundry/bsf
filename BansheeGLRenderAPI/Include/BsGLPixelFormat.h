#pragma once

#include "BsGLPrerequisites.h"
#include "BsPixelUtil.h"

namespace BansheeEngine 
{
	/**
	 * @brief	Converts pixel formats to OpenGL formats
	 */
	class BS_RSGL_EXPORT GLPixelUtil
	{
	public:
		/**
		 * @brief	Returns matching OpenGL base pixel format type if one is found,
		 *			zero otherwise.
		 */
		static GLenum getGLOriginFormat(PixelFormat mFormat);
	
		/**
		 * @brief	Returns OpenGL data type used in the provided format.
		 */
		static GLenum getGLOriginDataType(PixelFormat mFormat);
        
		/**
		 * @brief	Returns matching OpenGL internal pixel format type if one is found,
		 *			zero otherwise. Optionally returns an SRGB format if "hwGamma" is specified
		 *			and such format exists.
		 */
		static GLenum getGLInternalFormat(PixelFormat mFormat, bool hwGamma = false);
	
		/**
		 * @brief	Returns matching OpenGL internal pixel format type if one is found,
		 *			otherwise it returns the closest matching format. Optionally returns an 
		 *			SRGB format if "hwGamma" is specified and such format exists.
		 */
		static GLenum getClosestGLInternalFormat(PixelFormat mFormat, bool hwGamma = false);
		
		/**
		 * @brief	Returns an OpenGL type that should be used for creating a buffer for the specified
		 * 			depth/stencil format.
		 */
		static GLenum getDepthStencilTypeFromFormat(PixelFormat mFormat);

		/**
		 * @brief	Returns pixel format closest to the provided internal OpenGL format.
		 */
		static PixelFormat getClosestEngineFormat(GLenum fmt);

		/**
		 * @brief	Returns closest supported format to the provided format.
		 */
		static PixelFormat getClosestValidFormat(PixelFormat fmt);

		/**
		 * @brief	Converts an OpenGL internal format into base format.
		 */
		static GLenum getBaseFormatFromCompressedInternalFormat(GLenum internalFormat);
	};
};