#pragma once

#include "BsPrerequisites.h"
#include "BsIReflectable.h"

namespace BansheeEngine
{
	/**
	 * @brief	Contains settings used for controlling game start-up, as well as persisting various other properties
	 * 			through game sessions.
	 */
	class BS_EXPORT GameSettings : public IReflectable
	{
	public:
		WeakResourceHandle<Prefab> mainScene; /**< Resource UUID of the default scene that is loaded when the application is started. */
		bool fullscreen = true; /**< If true the application will be started in fullscreen using user's desktop resolution. */
		bool useDesktopResolution = true; /**< If running in fullscreen should the user's desktop resolution be used instead of the specified resolution. */
		UINT32 resolutionWidth = 1280; /**< Width of the window. */
		UINT32 resolutionHeight = 720; /**< Height of the window. */
		WString titleBarText; /**< Text displayed in window's titlebar. */

		/************************************************************************/
		/* 								RTTI		                     		*/
		/************************************************************************/
	public:
		friend class GameSettingsRTTI;
		static RTTITypeBase* getRTTIStatic();
		virtual RTTITypeBase* getRTTI() const override;
	};
}