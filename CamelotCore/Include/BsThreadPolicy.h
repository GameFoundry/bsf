#pragma once

#include "CmPrerequisites.h"
#include "CmProfiler.h"

namespace BansheeEngine
{
	class CM_EXPORT ThreadBansheePolicy
	{
	public:
		static void onThreadStarted(const String& name);
		static void onThreadEnded(const String& name);
	};
}