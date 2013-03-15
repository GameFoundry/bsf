#pragma once

#include "CmPrerequisitesUtil.h"

namespace CamelotEngine
{
	class CM_UTILITY_EXPORT BitmapWriter
	{
	public:
		static void rawPixelsToBMP(const UINT8* input, UINT8* output, UINT32 width, UINT32 height, UINT32 bytesPerPixel);
		static UINT32 getBMPSize(UINT32 width, UINT32 height, UINT32 bytesPerPixel);
	};
}