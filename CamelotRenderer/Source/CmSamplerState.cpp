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
#include "CmSamplerState.h"
#include "CmException.h"

namespace CamelotEngine {

	SamplerState SamplerState::EMPTY;
    //-----------------------------------------------------------------------
    SamplerState::SamplerState()
		: mHwGamma(false)
		, mMinFilter(FO_LINEAR)
		, mMagFilter(FO_LINEAR)
		, mMipFilter(FO_POINT)
		, mMaxAniso(0)
		, mMipmapBias(0)
		, mBindingType(BT_FRAGMENT)
    {
		setTextureAddressingMode(TAM_WRAP);
    }

    //-----------------------------------------------------------------------
    SamplerState::SamplerState(const SamplerState& oth )
    {
        *this = oth;
    }

    //-----------------------------------------------------------------------
    SamplerState::~SamplerState()
    {
    }
    //-----------------------------------------------------------------------
    SamplerState & SamplerState::operator = ( 
        const SamplerState &oth )
    {
        return *this;
    }
	//-----------------------------------------------------------------------
	void SamplerState::setBindingType(SamplerState::BindingType bt)
	{
		mBindingType = bt;

	}
	//-----------------------------------------------------------------------
	SamplerState::BindingType SamplerState::getBindingType(void) const
	{
		return mBindingType;
	}
	//-----------------------------------------------------------------------
	void SamplerState::setHardwareGammaEnabled(bool g)
	{
		mHwGamma = g;
	}
	//-----------------------------------------------------------------------
	bool SamplerState::isHardwareGammaEnabled() const
	{
		return mHwGamma;
	}
    //-----------------------------------------------------------------------
    const SamplerState::UVWAddressingMode& 
	SamplerState::getTextureAddressingMode(void) const
    {
        return mAddressMode;
    }
    //-----------------------------------------------------------------------
    void SamplerState::setTextureAddressingMode(
		SamplerState::TextureAddressingMode tam)
    {
        mAddressMode.u = tam;
        mAddressMode.v = tam;
        mAddressMode.w = tam;
    }
    //-----------------------------------------------------------------------
    void SamplerState::setTextureAddressingMode(
		SamplerState::TextureAddressingMode u, 
		SamplerState::TextureAddressingMode v,
		SamplerState::TextureAddressingMode w)
    {
        mAddressMode.u = u;
        mAddressMode.v = v;
        mAddressMode.w = w;
    }
    //-----------------------------------------------------------------------
    void SamplerState::setTextureAddressingMode(
		const SamplerState::UVWAddressingMode& uvw)
    {
        mAddressMode = uvw;
    }
	//-----------------------------------------------------------------------
	void SamplerState::setTextureFiltering(TextureFilterOptions filterType)
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
	}
	//-----------------------------------------------------------------------
    void SamplerState::setTextureFiltering(FilterType ft, FilterOptions fo)
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
    }
	//-----------------------------------------------------------------------
    void SamplerState::setTextureFiltering(FilterOptions minFilter, 
        FilterOptions magFilter, FilterOptions mipFilter)
    {
        mMinFilter = minFilter;
        mMagFilter = magFilter;
        mMipFilter = mipFilter;
    }
	//-----------------------------------------------------------------------
	FilterOptions SamplerState::getTextureFiltering(FilterType ft) const
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
	void SamplerState::setTextureAnisotropy(unsigned int maxAniso)
	{
		mMaxAniso = maxAniso;
	}
	//-----------------------------------------------------------------------
	unsigned int SamplerState::getTextureAnisotropy() const
	{
        return mMaxAniso;
	}
}
