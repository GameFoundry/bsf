#include "CmVideoModeInfo.h"

namespace BansheeEngine
{
	VideoMode::VideoMode(UINT32 width, UINT32 height, float refreshRate, UINT32 outputIdx)
		:mWidth(width), mHeight(height), mRefreshRate(refreshRate), mOutputIdx(outputIdx), mIsCustom(true)
	{

	}

	VideoMode::~VideoMode()
	{ }

	VideoOutputInfo::~VideoOutputInfo()
	{ 
		for (auto& videoMode : mVideoModes)
			cm_delete(videoMode);

		if (mDesktopVideoMode != nullptr)
			cm_delete(mDesktopVideoMode);
	}

	VideoModeInfo::~VideoModeInfo()
	{ 
		for (auto& output : mOutputs)
			cm_delete(output);
	}
}