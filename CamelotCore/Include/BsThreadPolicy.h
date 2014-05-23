#pragma once

#include "CmPrerequisites.h"
#include "CmProfiler.h"

namespace BansheeEngine
{
	/**
	 * @brief	Banshee thread policy that performs special startup/shutdown on threads
	 *			managed by thread pool.
	 */
	class CM_EXPORT ThreadBansheePolicy
	{
	public:
		static void onThreadStarted(const String& name);
		static void onThreadEnded(const String& name);
	};
}