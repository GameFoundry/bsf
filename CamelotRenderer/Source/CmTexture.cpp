/*
-----------------------------------------------------------------------------
This source file is part of OGRE
    (Object-oriented Graphics Rendering Engine)
For the latest info, see http://www.ogre3d.org/

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
#include "CmHardwarePixelBuffer.h"
#include "CmTexture.h"
#include "CmTextureRTTI.h"
#include "CmTextureData.h"
#include "CmDataStream.h"
#include "CmException.h"
#include "CmDebug.h"

namespace CamelotEngine {
	//--------------------------------------------------------------------------
    Texture::Texture()
        : 
            mHeight(512),
            mWidth(512),
            mDepth(1),
            mNumMipmaps(0),
			mHwGamma(false),
			mFSAA(0),
            mTextureType(TEX_TYPE_2D),            
            mFormat(PF_UNKNOWN),
            mUsage(TU_DEFAULT),
            mInternalResourcesCreated(false)
    {
        
    }
	//-------------------------------------------------------------------------
	void Texture::initialize(TextureType textureType, size_t width, size_t height, size_t depth, size_t numMipmaps, 
		PixelFormat format, int usage, bool hwGamma, UINT32 fsaa, const String& fsaaHint)
	{
		mTextureType = textureType;
		mWidth = width;
		mHeight = height;
		mDepth = depth;
		mNumMipmaps = numMipmaps;
		mFormat = format;
		mUsage = usage;
		mHwGamma = hwGamma;
		mFSAA = fsaa;
		mFSAAHint = fsaaHint;
	}
	//-------------------------------------------------------------------------
	void Texture::initialize_internal()
	{
		createInternalResources();
	}
    //--------------------------------------------------------------------------
    bool Texture::hasAlpha(void) const
    {
        return PixelUtil::hasAlpha(mFormat);
    }
    //--------------------------------------------------------------------------
	size_t Texture::calculateSize(void) const
	{
        return getNumFaces() * PixelUtil::getMemorySize(mWidth, mHeight, mDepth, mFormat);
	}
	//--------------------------------------------------------------------------
	size_t Texture::getNumFaces(void) const
	{
		return getTextureType() == TEX_TYPE_CUBE_MAP ? 6 : 1;
	}
	//-----------------------------------------------------------------------------
	void Texture::createInternalResources(void)
	{
		if (!mInternalResourcesCreated)
		{
			createInternalResourcesImpl();
			mInternalResourcesCreated = true;
		}
	}
	//-----------------------------------------------------------------------------
	void Texture::freeInternalResources(void)
	{
		if (mInternalResourcesCreated)
		{
			freeInternalResourcesImpl();
			mInternalResourcesCreated = false;
		}
	}
	//-----------------------------------------------------------------------------
	TextureDataPtr Texture::getTextureData(UINT32 face)
	{
		if(face < 0 || face >= getNumFaces())
		{
			CM_EXCEPT(InvalidParametersException, "Face index out of range: " + toString(face));
		}

		UINT32 numMips = getNumMipmaps();

		UINT32 totalSize = 0;
		UINT32 width = getWidth();
		UINT32 height = getHeight();
		UINT32 depth = getDepth();

		for(UINT32 j = 0; j <= numMips; j++)
		{
			UINT32 currentMipSize = PixelUtil::getMemorySize(
					width, height, depth, mFormat);

			totalSize += currentMipSize;

			if(width != 1) width /= 2;
			if(height != 1) height /= 2;
			if(depth != 1) depth /= 2;
		}

		UINT8* buffer = new UINT8[totalSize]; // TextureData frees this
		TextureDataPtr texData(new TextureData(getWidth(), getHeight(), totalSize, mFormat, buffer, getDepth(), 0, getNumMipmaps()));

		for(UINT32 j = 0; j <= numMips; j++)
		{
			PixelData pixels = texData->getPixels(j);
			getBuffer(face, j)->blitToMemory(pixels);
		}

		return texData;
	}
	//-----------------------------------------------------------------------------
	void Texture::setTextureData(UINT32 face, TextureDataPtr textureData)
	{
		if(face < 0 || face >= getNumFaces())
		{
			CM_EXCEPT(InvalidParametersException, "Face index out of range: " + toString(face));
		}

		if(mTextureData.size() <= face)
			mTextureData.resize(face + 1);

		mTextureData[face] = textureData;
	}

	void Texture::setRawPixels(const PixelData& data, UINT32 face, UINT32 mip)
	{
		// TODO - Queue command
	}

	void Texture::setRawPixels_internal(const PixelData& data, UINT32 face, UINT32 mip)
	{
		if(mip < 0 || mip > mNumMipmaps)
			CM_EXCEPT(InvalidParametersException, "Invalid mip level: " + toString(mip) + ". Min is 0, max is " + toString(mNumMipmaps));

		if(face < 0 || mip > getNumFaces())
			CM_EXCEPT(InvalidParametersException, "Invalid face index: " + toString(face) + ". Min is 0, max is " + toString(getNumFaces()));

		if(mFormat != data.format)
			CM_EXCEPT(InvalidParametersException, "Provided PixelData has invalid format. Needed: " + toString(mFormat) + ". Got: " + toString(data.format));

		if(mWidth != data.getWidth() || mHeight != data.getHeight() || mDepth != data.getDepth())
		{
			CM_EXCEPT(InvalidParametersException, "Provided PixelData size doesn't match the texture size." \
				" Width: " + toString(mWidth) + "/" + toString(data.getWidth()) + 
				" Height: " + toString(mHeight) + "/" + toString(data.getHeight()) + 
				" Depth: " + toString(mDepth) + "/" + toString(data.getDepth()));
		}

		getBuffer(face, mip)->blitFromMemory(data);
	}

	PixelDataPtr Texture::getRawPixels(UINT32 face, UINT32 mip)
	{
		// TODO - Queue command
		return nullptr;
	}

	PixelDataPtr Texture::getRawPixels_internal(UINT32 face, UINT32 mip)
	{
		if(mip < 0 || mip > mNumMipmaps)
			CM_EXCEPT(InvalidParametersException, "Invalid mip level: " + toString(mip) + ". Min is 0, max is " + toString(mNumMipmaps));

		if(face < 0 || mip > getNumFaces())
			CM_EXCEPT(InvalidParametersException, "Invalid face index: " + toString(face) + ". Min is 0, max is " + toString(getNumFaces()));

		UINT32 numMips = getNumMipmaps();

		UINT32 totalSize = 0;
		UINT32 width = getWidth();
		UINT32 height = getHeight();
		UINT32 depth = getDepth();

		for(UINT32 j = 0; j <= mip; j++)
		{
			totalSize = PixelUtil::getMemorySize(width, height, depth, mFormat);

			if(width != 1) width /= 2;
			if(height != 1) height /= 2;
			if(depth != 1) depth /= 2;
		}

		UINT8* buffer = new UINT8[totalSize]; 
		PixelDataPtr src(new PixelData(width, height, depth, getFormat(), buffer, true));

		getBuffer(face, mip)->blitToMemory(*src);

		return src;
	}

	//-----------------------------------------------------------------------------
	void Texture::initializeFromTextureData()
	{
		if(mTextureData.size() < 1)
			CM_EXCEPT(InvalidParametersException, "Cannot load empty vector of images");

		if(getTextureType() == TEX_TYPE_CUBE_MAP)
		{
			if(mTextureData.size() != 6)
				CM_EXCEPT(InvalidParametersException, "Cube map textures require six faces.");
		}
		else
		{
			if(mTextureData.size() > 1)
			{
				gDebug().log("Non-cube textures can only have one face. Loading only first face from the provided array.", "D3D9RenderSystem");
			}
		}

		// Set desired texture size and properties from images[0]
		mWidth = mTextureData[0]->getWidth();
		mHeight = mTextureData[0]->getHeight();
		mDepth = mTextureData[0]->getDepth();

		// Get source image format and adjust if required
		PixelFormat srcFormat = mTextureData[0]->getFormat();

		// The custom mipmaps in the image have priority over everything
		size_t imageMips = mTextureData[0]->getNumMipmaps();

		if(imageMips > 0)
		{
			mNumMipmaps = mTextureData[0]->getNumMipmaps();
		}

		// Create the texture
		createInternalResources();
		// Check if we're loading one image with multiple faces
		// or a vector of images representing the faces
		size_t faces;
		if(mTextureData.size() > 1)
			faces = mTextureData.size();
		else
			faces = 1;

		// Check whether number of faces in images exceeds number of faces
		// in this texture. If so, clamp it.
		if(faces > getNumFaces())
			faces = getNumFaces();

		// Main loading loop
		// imageMips == 0 if the image has no custom mipmaps, otherwise contains the number of custom mips
		for(size_t mip = 0; mip<=imageMips; ++mip)
		{
			for(size_t i = 0; i < faces; ++i)
			{
				PixelData src = mTextureData[i]->getPixels(mip);

				// Sets to treated format in case is difference
				src.format = srcFormat;

				// Destination: entire texture. blitFromMemory does the scaling to
				// a power of two for us when needed
				getBuffer(i, mip)->blitFromMemory(src);
			}
		}
		// Update size (the final size, not including temp space)
		mSize = getNumFaces() * PixelUtil::getMemorySize(mWidth, mHeight, mDepth, mFormat);
	}
	//-----------------------------------------------------------------------------
	void Texture::unloadImpl(void)
	{
		freeInternalResources();
	}
    //-----------------------------------------------------------------------------   
    void Texture::copyToTexture( TexturePtr& target )
    {
        if(target->getNumFaces() != getNumFaces())
        {
            CM_EXCEPT(InvalidParametersException, 
                "Texture types must match");
        }

        size_t numMips = std::min(getNumMipmaps(), target->getNumMipmaps());
        for(unsigned int face=0; face<getNumFaces(); face++)
        {
            for(unsigned int mip=0; mip<=numMips; mip++)
            {
                target->getBuffer(face, mip)->blit(getBuffer(face, mip));
            }
        }
    }

	/************************************************************************/
	/* 								SERIALIZATION                      		*/
	/************************************************************************/

	RTTITypeBase* Texture::getRTTIStatic()
	{
		return TextureRTTI::instance();
	}

	RTTITypeBase* Texture::getRTTI() const
	{
		return Texture::getRTTIStatic();
	}

	/************************************************************************/
	/* 								STATICS	                      			*/
	/************************************************************************/
	TexturePtr Texture::create(TextureType texType, UINT32 width, UINT32 height, UINT32 depth, 
		int num_mips, PixelFormat format, int usage, bool hwGammaCorrection, UINT32 fsaa, const String& fsaaHint)
	{
		return TextureManager::instance().create(texType, 
			width, height, depth, num_mips, format, usage, hwGammaCorrection, fsaa, fsaaHint);
	}
	
	TexturePtr Texture::create(TextureType texType, UINT32 width, UINT32 height, 
		int num_mips, PixelFormat format, int usage, bool hwGammaCorrection, UINT32 fsaa, const String& fsaaHint)
	{
		return TextureManager::instance().create(texType, 
			width, height, num_mips, format, usage, hwGammaCorrection, fsaa, fsaaHint);
	}
}
