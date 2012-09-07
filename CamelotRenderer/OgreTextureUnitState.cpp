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
#include "OgreTextureUnitState.h"
#include "OgreException.h"

namespace Ogre {

    //-----------------------------------------------------------------------
    TextureUnitState::TextureUnitState(TextureType type)
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
    TextureUnitState::TextureUnitState(const TextureUnitState& oth )
    {
        *this = oth;
    }

    //-----------------------------------------------------------------------
    TextureUnitState::~TextureUnitState()
    {
    }
    //-----------------------------------------------------------------------
    TextureUnitState & TextureUnitState::operator = ( 
        const TextureUnitState &oth )
    {
        return *this;
    }
	//-----------------------------------------------------------------------
	void TextureUnitState::setBindingType(TextureUnitState::BindingType bt)
	{
		mBindingType = bt;

	}
	//-----------------------------------------------------------------------
	TextureUnitState::BindingType TextureUnitState::getBindingType(void) const
	{
		return mBindingType;
	}
    //-----------------------------------------------------------------------
    bool TextureUnitState::isCubic(void) const
    {
        return mTextureType == TEX_TYPE_CUBE_MAP;
    }
    //-----------------------------------------------------------------------
    bool TextureUnitState::is3D(void) const
    {
        return mTextureType == TEX_TYPE_3D;
    }
    //-----------------------------------------------------------------------
    TextureType TextureUnitState::getTextureType(void) const
    {
        return mTextureType;
	}
    //-----------------------------------------------------------------------
    void TextureUnitState::setDesiredFormat(PixelFormat desiredFormat)
    {
        mDesiredFormat = desiredFormat;
    }
    //-----------------------------------------------------------------------
    PixelFormat TextureUnitState::getDesiredFormat(void) const
    {
        return mDesiredFormat;
    }
    //-----------------------------------------------------------------------
    void TextureUnitState::setNumMipmaps(int numMipmaps)
    {
        mTextureSrcMipmaps = numMipmaps;
    }
    //-----------------------------------------------------------------------
    int TextureUnitState::getNumMipmaps(void) const
    {
        return mTextureSrcMipmaps;
    }
    //-----------------------------------------------------------------------
    void TextureUnitState::setIsAlpha(bool isAlpha)
    {
        mIsAlpha = isAlpha;
    }
    //-----------------------------------------------------------------------
    bool TextureUnitState::getIsAlpha(void) const
    {
        return mIsAlpha;
    }
	//-----------------------------------------------------------------------
	void TextureUnitState::setHardwareGammaEnabled(bool g)
	{
		mHwGamma = g;
	}
	//-----------------------------------------------------------------------
	bool TextureUnitState::isHardwareGammaEnabled() const
	{
		return mHwGamma;
	}
    //-----------------------------------------------------------------------
    const TextureUnitState::UVWAddressingMode& 
	TextureUnitState::getTextureAddressingMode(void) const
    {
        return mAddressMode;
    }
    //-----------------------------------------------------------------------
    void TextureUnitState::setTextureAddressingMode(
		TextureUnitState::TextureAddressingMode tam)
    {
        mAddressMode.u = tam;
        mAddressMode.v = tam;
        mAddressMode.w = tam;
    }
    //-----------------------------------------------------------------------
    void TextureUnitState::setTextureAddressingMode(
		TextureUnitState::TextureAddressingMode u, 
		TextureUnitState::TextureAddressingMode v,
		TextureUnitState::TextureAddressingMode w)
    {
        mAddressMode.u = u;
        mAddressMode.v = v;
        mAddressMode.w = w;
    }
    //-----------------------------------------------------------------------
    void TextureUnitState::setTextureAddressingMode(
		const TextureUnitState::UVWAddressingMode& uvw)
    {
        mAddressMode = uvw;
    }
	//-----------------------------------------------------------------------
	void TextureUnitState::setTextureFiltering(TextureFilterOptions filterType)
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
    void TextureUnitState::setTextureFiltering(FilterType ft, FilterOptions fo)
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
    void TextureUnitState::setTextureFiltering(FilterOptions minFilter, 
        FilterOptions magFilter, FilterOptions mipFilter)
    {
        mMinFilter = minFilter;
        mMagFilter = magFilter;
        mMipFilter = mipFilter;
        mIsDefaultFiltering = false;
    }
	//-----------------------------------------------------------------------
	FilterOptions TextureUnitState::getTextureFiltering(FilterType ft) const
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
	void TextureUnitState::setTextureAnisotropy(unsigned int maxAniso)
	{
		mMaxAniso = maxAniso;
        mIsDefaultAniso = false;
	}
	//-----------------------------------------------------------------------
	unsigned int TextureUnitState::getTextureAnisotropy() const
	{
        return mMaxAniso;
	}
}
