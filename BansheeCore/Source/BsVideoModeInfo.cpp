//__________________________ Banshee Project - A modern game development toolkit _________________________________//
//_____________________________________ www.banshee-project.com __________________________________________________//
//________________________ Copyright (c) 2014 Marko Pintera. All rights reserved. ________________________________//
#include "BsVideoModeInfo.h"

namespace BansheeEngine
{
	VideoMode::VideoMode(UINT32 width, UINT32 height, float refreshRate, UINT32 outputIdx)
		:mWidth(width), mHeight(height), mRefreshRate(refreshRate), mOutputIdx(outputIdx), mIsCustom(true)
	{

	}

	VideoMode::~VideoMode()
	{ }

	bool VideoMode::operator== (const VideoMode& other) const
	{
		return mWidth == other.mWidth && mHeight == other.mHeight && 
			mOutputIdx == other.mOutputIdx && mRefreshRate == other.mRefreshRate;
	}

	VideoOutputInfo::~VideoOutputInfo()
	{ 
		for (auto& videoMode : mVideoModes)
			bs_delete(videoMode);

		if (mDesktopVideoMode != nullptr)
			bs_delete(mDesktopVideoMode);
	}

	VideoModeInfo::~VideoModeInfo()
	{ 
		for (auto& output : mOutputs)
			bs_delete(output);
	}
}