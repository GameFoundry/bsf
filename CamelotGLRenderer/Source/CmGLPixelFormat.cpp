/*
-----------------------------------------------------------------------------
This source file is part of OGRE
    (Object-oriented Graphics Rendering Engine)
For the latest info, see http://www.ogre3d.org

Copyright (c) 2000-2011 Torus Knot Software Ltd

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
-----------------------------------------------------------------------------
*/

#include "CmGLPixelFormat.h"
#include "CmRenderSystem.h"
#include "CmBitwise.h"
#include "CmDebug.h"

namespace CamelotFramework  {
	//-----------------------------------------------------------------------------
    GLenum GLPixelUtil::getGLOriginFormat(PixelFormat mFormat)
    {
        switch(mFormat)
        {
			case PF_R8:
				return GL_RED;
			case PF_R8G8:
				return GL_RG;
            case PF_R8G8B8:
                return GL_RGB;
            case PF_B8G8R8:
                return GL_BGR;
			case PF_R8G8B8X8:
			case PF_R8G8B8A8:
			case PF_A8R8G8B8:
			case PF_X8R8G8B8:
                return GL_RGBA;
            case PF_B8G8R8A8:
			case PF_B8G8R8X8:
			case PF_A8B8G8R8:
			case PF_X8B8G8R8:
                return GL_BGRA;
			case PF_FLOAT16_R:
                return GL_RED;
			case PF_FLOAT16_RG:
				return GL_RG;
            case PF_FLOAT16_RGB:
                return GL_RGB;
            case PF_FLOAT16_RGBA:
                return GL_RGBA;
			case PF_FLOAT32_R:
                return GL_RED;
			case PF_FLOAT32_RG:
				return GL_RG;
            case PF_FLOAT32_RGB:
                return GL_RGB;
            case PF_FLOAT32_RGBA:
                return GL_RGBA;
            case PF_DXT1:
                return GL_COMPRESSED_RGBA_S3TC_DXT1_EXT;
            case PF_DXT3:
                 return GL_COMPRESSED_RGBA_S3TC_DXT3_EXT;
            case PF_DXT5:
                 return GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
            default:
                return 0;
        }
    }
	//----------------------------------------------------------------------------- 
    GLenum GLPixelUtil::getGLOriginDataType(PixelFormat mFormat)
    {
        switch(mFormat)
        {
			case PF_R8:
            case PF_R8G8B8:
            case PF_B8G8R8:
			case PF_R8G8:
				return GL_UNSIGNED_BYTE;
			case PF_X8B8G8R8:
			case PF_A8B8G8R8:
			case PF_X8R8G8B8:
            case PF_A8R8G8B8:
				return GL_UNSIGNED_INT_8_8_8_8;
            case PF_B8G8R8A8:
			case PF_B8G8R8X8:
			case PF_R8G8B8A8:
			case PF_R8G8B8X8:
				return GL_UNSIGNED_INT_8_8_8_8_REV;
			case PF_FLOAT16_R:
			case PF_FLOAT16_RG:
            case PF_FLOAT16_RGB:
            case PF_FLOAT16_RGBA:
                return GL_HALF_FLOAT;
			case PF_FLOAT32_R:
			case PF_FLOAT32_RG:
            case PF_FLOAT32_RGB:
            case PF_FLOAT32_RGBA:
                return GL_FLOAT;
            default:
                return 0;
        }
    }

	GLenum GLPixelUtil::getBaseFormatFromCompressedInternalFormat(GLenum internalFormat)
	{
		 switch(internalFormat) 
		 {
		 case GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT1_EXT:
		 case GL_COMPRESSED_RGBA_S3TC_DXT1_EXT:
		 case GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT3_EXT:
		 case GL_COMPRESSED_RGBA_S3TC_DXT3_EXT:
		 case GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT5_EXT:
		 case GL_COMPRESSED_RGBA_S3TC_DXT5_EXT:
			 return GL_RGBA;
		 case GL_COMPRESSED_RGB_S3TC_DXT1_EXT:
		 case GL_COMPRESSED_SRGB_S3TC_DXT1_EXT:
			 return GL_RGB;
		 case GL_COMPRESSED_RED_RGTC1:
		 case GL_COMPRESSED_SIGNED_RED_RGTC1:
			 return GL_RED;
		 case GL_COMPRESSED_RG_RGTC2:
		 case GL_COMPRESSED_SIGNED_RG_RGTC2:
			 return GL_RG;
		 }

		 return GL_RGBA;
	}
    
    GLenum GLPixelUtil::getGLInternalFormat(PixelFormat mFormat, bool hwGamma)
    {
        switch(mFormat) {
            case PF_R8:
                return GL_R8;
			case PF_R8G8:
				return GL_RG8;
            case PF_R8G8B8:
            case PF_B8G8R8:
			case PF_B8G8R8X8:
			case PF_R8G8B8X8:
				if (hwGamma)
					return GL_SRGB8;
				else
					return GL_RGB8;
            case PF_B8G8R8A8:
			case PF_R8G8B8A8:
				if (hwGamma)
					return GL_SRGB8_ALPHA8;
				else
					return GL_RGBA8;
			case PF_FLOAT16_R:
				return GL_R16F;
            case PF_FLOAT16_RGB:
                return GL_RGB16F;
			case PF_FLOAT16_RG: 
				return GL_RG16F;
            case PF_FLOAT16_RGBA:
                return GL_RGBA16F;
			case PF_FLOAT32_R:
				return GL_R32F;
			case PF_FLOAT32_RG:
				return GL_RG32F;
            case PF_FLOAT32_RGB:
                return GL_RGB32F;
            case PF_FLOAT32_RGBA:
                return GL_RGBA32F;
			case PF_DXT1:
				if (hwGamma)
					return GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT1_EXT;
				else
					return GL_COMPRESSED_RGBA_S3TC_DXT1_EXT;
            case PF_DXT3:
				if (hwGamma)
					return GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT3_EXT;
				else
	                return GL_COMPRESSED_RGBA_S3TC_DXT3_EXT;
            case PF_DXT5:
				if (hwGamma)
					return GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT5_EXT;
				else
	                return GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
			case PF_D16:
				return GL_DEPTH_COMPONENT16;
			case PF_D32:
				return GL_DEPTH_COMPONENT32F;
			case PF_D24S8:
				return GL_DEPTH24_STENCIL8;
			case PF_D32_S8X24:
				return GL_DEPTH32F_STENCIL8;
            default:
                return GL_NONE;
        }
    }

    GLenum GLPixelUtil::getClosestGLInternalFormat(PixelFormat mFormat, bool hwGamma)
    {
        GLenum format = getGLInternalFormat(mFormat, hwGamma);
        if(format==GL_NONE)
		{
			if (hwGamma)
				return GL_SRGB8;
			else
				return GL_RGBA8;
		}
        else
            return format;
    }

	GLenum GLPixelUtil::getDepthStencilTypeFromFormat(PixelFormat mFormat)
	{
		switch(mFormat)
		{
		case PF_D32_S8X24:
			return GL_FLOAT_32_UNSIGNED_INT_24_8_REV;
		case PF_D24S8:
			return GL_UNSIGNED_INT_24_8;
		case PF_D32:
			return GL_FLOAT;
		case PF_D16:
			return GL_UNSIGNED_SHORT;
		}

		CM_EXCEPT(InvalidParametersException, "Invalid depth stencil format");
	}
	
	//----------------------------------------------------------------------------- 	
	PixelFormat GLPixelUtil::getClosestEngineFormat(GLenum fmt)
	{
		switch(fmt) 
		{
		case GL_R8:
			return PF_R8;
		case GL_RG8:
			return PF_R8G8;
		case GL_RGB8:
		case GL_SRGB8:
			return PF_R8G8B8X8;
		case GL_RGBA8:
		case GL_SRGB8_ALPHA8:
			return PF_R8G8B8A8;
		case GL_R16F:
			return PF_FLOAT16_R;
		case GL_RG16F:
			return PF_FLOAT16_RG;
		case GL_RG32F:
			return PF_FLOAT32_RG;
		case GL_R32F:
			return PF_FLOAT32_R;
		case GL_RGB16F:
			return PF_FLOAT16_RGB;
		case GL_RGBA16F:
			return PF_FLOAT16_RGBA;
		case GL_RGB32F:
			return PF_FLOAT32_RGB;
		case GL_RGBA32F:
			return PF_FLOAT32_RGBA;
		case GL_COMPRESSED_RGB_S3TC_DXT1_EXT:
		case GL_COMPRESSED_RGBA_S3TC_DXT1_EXT:
		case GL_COMPRESSED_SRGB_S3TC_DXT1_EXT:
		case GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT1_EXT:
			return PF_DXT1;
		case GL_COMPRESSED_RGBA_S3TC_DXT3_EXT:
		case GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT3_EXT:
			return PF_DXT3;
		case GL_COMPRESSED_RGBA_S3TC_DXT5_EXT:
		case GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT5_EXT:
			return PF_DXT5;
		case GL_DEPTH_COMPONENT16:
			return PF_D16;
		case GL_DEPTH_COMPONENT32F:
			return PF_D32;
		case GL_DEPTH24_STENCIL8:
			return PF_D24S8;
		case GL_DEPTH32F_STENCIL8:
			return PF_D32_S8X24;
		default:
			return PF_R8G8B8A8;
		};
	}
	//----------------------------------------------------------------------------- 	
	PixelFormat GLPixelUtil::getClosestValidFormat(PixelFormat fmt)
	{
		switch(fmt) 
		{
		case PF_R8:
			return PF_R8;
		case PF_R8G8:
			return PF_R8G8;
		case PF_R8G8B8:
		case PF_B8G8R8:
		case PF_B8G8R8X8:
		case PF_R8G8B8X8:
			return PF_R8G8B8X8;
		case PF_B8G8R8A8:
		case PF_R8G8B8A8:
			return PF_R8G8B8A8;
		case PF_FLOAT16_R:
			return PF_FLOAT16_R;
		case PF_FLOAT16_RGB:
			return PF_FLOAT16_RGB;
		case PF_FLOAT16_RG: 
			return PF_FLOAT16_RG;
		case PF_FLOAT16_RGBA:
			return PF_FLOAT16_RGBA;
		case PF_FLOAT32_R:
			return PF_FLOAT32_R;
		case PF_FLOAT32_RG:
			return PF_FLOAT32_RG;
		case PF_FLOAT32_RGB:
			return PF_FLOAT32_RGB;
		case PF_FLOAT32_RGBA:
			return PF_FLOAT32_RGBA;
		case PF_DXT1:
			return PF_DXT1;
		case PF_DXT3:
			return PF_DXT3;
		case PF_DXT5:
			return PF_DXT5;
		case PF_D16:
			return PF_D16;
		case PF_D32:
			return PF_D32;
		case PF_D24S8:
			return PF_D24S8;
		case PF_D32_S8X24:
			return PF_D32_S8X24;
		default:
			return PF_UNKNOWN;
		}
	}
    //-----------------------------------------------------------------------------    
    UINT32 GLPixelUtil::optionalPO2(UINT32 value)
    {
        const RenderSystemCapabilities *caps = CamelotFramework::RenderSystem::instancePtr()->getCapabilities();
        if(caps->hasCapability(RSC_NON_POWER_OF_2_TEXTURES))
            return value;
        else
            return Bitwise::firstPO2From((UINT32)value);
    }   

	
};
