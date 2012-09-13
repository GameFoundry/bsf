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
#include "CmTextureState.h"
#include "CmException.h"

namespace CamelotEngine {

    //-----------------------------------------------------------------------
    TextureState::TextureState(TextureType type)
		: mTextureType(type)
        , mDesiredFormat(PF_UNKNOWN)
		, mTextureSrcMipmaps(MIP_DEFAULT)
		, mIsAlpha(false)
		, mHwGamma(false)
		, mMinFilter(FO_LINEAR)
		, mMagFilter(FO_LINEAR)
		, mMipFilter(FO_POINT)
		, mMaxAniso(0)
		, mMipmapBias(0)
		, mIsDefaultAniso(true)
		, mIsDefaultFiltering(true)
		, mBindingType(BT_FRAGMENT)
    {
		setTextureAddressingMode(TAM_WRAP);
    }

    //-----------------------------------------------------------------------
    TextureState::TextureState(const TextureState& oth )
    {
        *this = oth;
    }

    //-----------------------------------------------------------------------
    TextureState::~TextureState()
    {
    }
    //-----------------------------------------------------------------------
    TextureState & TextureState::operator = ( 
        const TextureState &oth )
    {
        return *this;
    }
	//-----------------------------------------------------------------------
	void TextureState::setBindingType(TextureState::BindingType bt)
	{
		mBindingType = bt;

	}
	//-----------------------------------------------------------------------
	TextureState::BindingType TextureState::getBindingType(void) const
	{
		return mBindingType;
	}
    //-----------------------------------------------------------------------
    bool TextureState::isCubic(void) const
    {
        return mTextureType == TEX_TYPE_CUBE_MAP;
    }
    //-----------------------------------------------------------------------
    bool TextureState::is3D(void) const
    {
        return mTextureType == TEX_TYPE_3D;
    }
    //-----------------------------------------------------------------------
    TextureType TextureState::getTextureType(void) const
    {
        return mTextureType;
	}
    //-----------------------------------------------------------------------
    void TextureState::setDesiredFormat(PixelFormat desiredFormat)
    {
        mDesiredFormat = desiredFormat;
    }
    //-----------------------------------------------------------------------
    PixelFormat TextureState::getDesiredFormat(void) const
    {
        return mDesiredFormat;
    }
    //-----------------------------------------------------------------------
    void TextureState::setNumMipmaps(int numMipmaps)
    {
        mTextureSrcMipmaps = numMipmaps;
    }
    //-----------------------------------------------------------------------
    int TextureState::getNumMipmaps(void) const
    {
        return mTextureSrcMipmaps;
    }
    //-----------------------------------------------------------------------
    void TextureState::setIsAlpha(bool isAlpha)
    {
        mIsAlpha = isAlpha;
    }
    //-----------------------------------------------------------------------
    bool TextureState::getIsAlpha(void) const
    {
        return mIsAlpha;
    }
	//-----------------------------------------------------------------------
	void TextureState::setHardwareGammaEnabled(bool g)
	{
		mHwGamma = g;
	}
	//-----------------------------------------------------------------------
	bool TextureState::isHardwareGammaEnabled() const
	{
		return mHwGamma;
	}
    //-----------------------------------------------------------------------
    const TextureState::UVWAddressingMode& 
	TextureState::getTextureAddressingMode(void) const
    {
        return mAddressMode;
    }
    //-----------------------------------------------------------------------
    void TextureState::setTextureAddressingMode(
		TextureState::TextureAddressingMode tam)
    {
        mAddressMode.u = tam;
        mAddressMode.v = tam;
        mAddressMode.w = tam;
    }
    //-----------------------------------------------------------------------
    void TextureState::setTextureAddressingMode(
		TextureState::TextureAddressingMode u, 
		TextureState::TextureAddressingMode v,
		TextureState::TextureAddressingMode w)
    {
        mAddressMode.u = u;
        mAddressMode.v = v;
        mAddressMode.w = w;
    }
    //-----------------------------------------------------------------------
    void TextureState::setTextureAddressingMode(
		const TextureState::UVWAddressingMode& uvw)
    {
        mAddressMode = uvw;
    }
	//-----------------------------------------------------------------------
	void TextureState::setTextureFiltering(TextureFilterOptions filterType)
	{
        switch (filterType)
        {
        case TFO_NONE:
            setTextureFiltering(FO_POINT, FO_POINT, FO_NONE);
            break;
        case TFO_BILINEAR:
            setTextureFiltering(FO_LINEAR, FO_LINEAR, FO_POINT);
            break;
        case TFO_TRILINEAR:
            setTextureFiltering(FO_LINEAR, FO_LINEAR, FO_LINEAR);
            break;
        case TFO_ANISOTROPIC:
            setTextureFiltering(FO_ANISOTROPIC, FO_ANISOTROPIC, FO_LINEAR);
            break;
        }
        mIsDefaultFiltering = false;
	}
	//-----------------------------------------------------------------------
    void TextureState::setTextureFiltering(FilterType ft, FilterOptions fo)
    {
        switch (ft)
        {
        case FT_MIN:
            mMinFilter = fo;
            break;
        case FT_MAG:
            mMagFilter = fo;
            break;
        case FT_MIP:
            mMipFilter = fo;
            break;
        }
        mIsDefaultFiltering = false;
    }
	//-----------------------------------------------------------------------
    void TextureState::setTextureFiltering(FilterOptions minFilter, 
        FilterOptions magFilter, FilterOptions mipFilter)
    {
        mMinFilter = minFilter;
        mMagFilter = magFilter;
        mMipFilter = mipFilter;
        mIsDefaultFiltering = false;
    }
	//-----------------------------------------------------------------------
	FilterOptions TextureState::getTextureFiltering(FilterType ft) const
	{
        switch (ft)
        {
        case FT_MIN:
            return mMinFilter;
        case FT_MAG:
            return mMagFilter;
        case FT_MIP:
            return mMipFilter;
        }
		// to keep compiler happy
		return mMinFilter;
	}

	//-----------------------------------------------------------------------
	void TextureState::setTextureAnisotropy(unsigned int maxAniso)
	{
		mMaxAniso = maxAniso;
        mIsDefaultAniso = false;
	}
	//-----------------------------------------------------------------------
	unsigned int TextureState::getTextureAnisotropy() const
	{
        return mMaxAniso;
	}
}
